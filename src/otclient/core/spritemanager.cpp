/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "spritemanager.h"
#include <framework/core/resourcemanager.h>
#include <framework/core/eventdispatcher.h>
#include <framework/core/filestream.h>
#include <framework/graphics/graphics.h>
#include <framework/thirdparty/apngloader.h>

SpriteManager g_sprites;

SpriteManager::SpriteManager()
{
    m_spritesCount = 0;
    m_signature = 0;
}

bool SpriteManager::load(const std::string& file)
{
    try {
        m_spritesFile = g_resources.openFile(file);
        if(!m_spritesFile)
            return false;

        // cache file buffer to avoid lags from hard drive
        m_spritesFile->cache();

        m_signature = m_spritesFile->getU32();
        m_spritesCount = m_spritesFile->getU16();
        m_sprites.resize(m_spritesCount);
        m_loaded = true;
        return true;
    } catch(Exception& e) {
        logError("Failed to load sprites from '", file, "': ", e.what());
        return false;
    }
}

void SpriteManager::unload()
{
    m_sprites.clear();
    m_spritesCount = 0;
    m_signature = 0;
}

void SpriteManager::preloadSprites()
{
    // preload every 50 sprites, periodically
    const int burst = 50;
    const int interval = 10;
    auto preload = [this](int start) {
        for(int i=start;i<start+burst && i<=m_spritesCount;++i) {
            loadSpriteTexture(i);
        }
    };

    for(int i=1;i<=m_spritesCount;i+=burst)
        g_eventDispatcher.scheduleEvent(std::bind(preload, i), (i/burst) * interval);
}

TexturePtr SpriteManager::loadSpriteTexture(int id)
{
    m_spritesFile->seek(((id-1) * 4) + 6);

    uint32 spriteAddress = m_spritesFile->getU32();

    // no sprite? return an empty texture
    if(spriteAddress == 0)
        return g_graphics.getEmptyTexture();

    m_spritesFile->seek(spriteAddress);

    // skip color key
    m_spritesFile->getU8();
    m_spritesFile->getU8();
    m_spritesFile->getU8();

    uint16 pixelDataSize = m_spritesFile->getU16();

    static std::vector<uint8> pixels(SPRITE_SIZE);
    int writePos = 0;
    int read = 0;

    // decompress pixels
    while(read < pixelDataSize) {
        uint16 transparentPixels = m_spritesFile->getU16();
        uint16 coloredPixels = m_spritesFile->getU16();

        if(writePos + transparentPixels*4 + coloredPixels*3 >= SPRITE_SIZE)
            return g_graphics.getEmptyTexture();

        for(int i = 0; i < transparentPixels; i++) {
            pixels[writePos + 0] = 0x00;
            pixels[writePos + 1] = 0x00;
            pixels[writePos + 2] = 0x00;
            pixels[writePos + 3] = 0x00;
            writePos += 4;
        }

        for(int i = 0; i < coloredPixels; i++) {
            pixels[writePos + 0] = m_spritesFile->getU8();
            pixels[writePos + 1] = m_spritesFile->getU8();
            pixels[writePos + 2] = m_spritesFile->getU8();
            pixels[writePos + 3] = 0xFF;

            writePos += 4;
        }

        read += 4 + (3 * coloredPixels);
    }

    // fill remaining pixels with alpha
    while(writePos < SPRITE_SIZE) {
        pixels[writePos + 0] = 0x00;
        pixels[writePos + 1] = 0x00;
        pixels[writePos + 2] = 0x00;
        pixels[writePos + 3] = 0x00;
        writePos += 4;
    }

    TexturePtr spriteTex(new Texture(32, 32, 4, &pixels[0]));
    spriteTex->setSmooth(true);

    if(g_graphics.canUseMipmaps())
        spriteTex->generateSoftwareMipmaps(pixels);

    return spriteTex;
}

TexturePtr& SpriteManager::getSpriteTexture(int id)
{
    if(id == 0)
        return g_graphics.getEmptyTexture();

    assert(id > 0 && id <= m_spritesCount);

    // load sprites on demand
    TexturePtr& sprite = m_sprites[id-1];
    if(!sprite)
        sprite = loadSpriteTexture(id);

    //TODO: release unused sprites textures after X seconds
    // to avoid massive texture allocations
    return sprite;
}

bool SpriteManager::exportSprite(std::string fileName, int id, int compression_level/* = 6*/)
{
    m_spritesFile->seek(((id-1) * 4) + 6);
    uint32 spriteAddress = m_spritesFile->getU32();
    if(spriteAddress == 0)
        return false;

    m_spritesFile->seek(spriteAddress);

    // skip color key
    m_spritesFile->getU8();
    m_spritesFile->getU8();
    m_spritesFile->getU8();

    uint16 pixelDataSize = m_spritesFile->getU16();

    uchar pixels[SPRITE_SIZE];
    int writePos = 0;
    int read = 0;

    // decompress pixels
    while(read < pixelDataSize) {
        uint16 transparentPixels = m_spritesFile->getU16();
        uint16 coloredPixels = m_spritesFile->getU16();

        if(writePos + transparentPixels*4 + coloredPixels*3 >= SPRITE_SIZE)
            return false;

        for(int i = 0; i < transparentPixels; i++) {
            pixels[writePos + 0] = 0x00;
            pixels[writePos + 1] = 0x00;
            pixels[writePos + 2] = 0x00;
            pixels[writePos + 3] = 0x00;
            writePos += 4;
        }

        for(int i = 0; i < coloredPixels; i++) {
            pixels[writePos + 0] = m_spritesFile->getU8();
            pixels[writePos + 1] = m_spritesFile->getU8();
            pixels[writePos + 2] = m_spritesFile->getU8();
            pixels[writePos + 3] = 0xFF;

            writePos += 4;
        }

        read += 4 + (3 * coloredPixels);
    }

    // fill remaining pixels with alpha
    while(writePos < SPRITE_SIZE) {
        pixels[writePos + 0] = 0x00;
        pixels[writePos + 1] = 0x00;
        pixels[writePos + 2] = 0x00;
        pixels[writePos + 3] = 0x00;
        writePos += 4;
    }

    std::stringstream ss;
    save_png(ss, SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_CHANNELS, pixels, PNG_COMPRESSION);
    g_resources.saveFile(fileName, ss);
    return true;
}

void SpriteManager::exportSprites()
{
    g_resources.makeDir("sprites");

    for(volatile int i = 1; i <= m_spritesCount; i++) {
        std::string fileName = Fw::formatString("sprites/%d.png", i);
        exportSprite(fileName, i, PNG_COMPRESSION);
    }
}

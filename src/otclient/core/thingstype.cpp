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

#include "thingstype.h"
#include "spritemanager.h"
#include "thing.h"
#include <framework/core/resourcemanager.h>
#include <physfs.h>

ThingsType g_thingsType;
ThingType ThingsType::m_emptyThingType;

bool ThingsType::load(const std::string& file)
{
    try {
        std::stringstream fin;
        g_resources.loadFile(file, fin);

        m_signature = Fw::getU32(fin);

        int numThings[LastCategory];
        for(int i = 0; i < LastCategory; ++i)
            numThings[i] = Fw::getU16(fin);

        numThings[Item] -= 99;

        for(int i = 0; i < LastCategory; ++i) {
            m_things[i].resize(numThings[i]);
            for(int id = 0; id < numThings[i]; ++id)
                parseThingType(fin, m_things[i][id]);
        }

        m_loaded = true;
        return true;
    } catch(Exception& e) {
        logError("Failed to load dat from '", file, "': ", e.what());
        return false;
    }
}

void ThingsType::unload()
{
    for(int i = 0; i < LastCategory; ++i)
        m_things[i].clear();
}

void ThingsType::parseThingType(std::stringstream& fin, ThingType& thingType)
{
    assert(fin.good());

    while(true) {
        int property = Fw::getU8(fin);
        if(property == ThingType::LastPropertyValue)
            break;

        thingType.properties[property] = true;

        if(property == ThingType::IsGround)
            thingType.parameters[ThingType::GroundSpeed] = Fw::getU16(fin);
        else if(property == ThingType::IsWritable || property == ThingType::IsWritableOnce)
            thingType.parameters[ThingType::MaxTextLenght] = Fw::getU16(fin);
        else if(property == ThingType::HasLight) {
            thingType.parameters[ThingType::LightLevel] = Fw::getU16(fin);
            thingType.parameters[ThingType::LightColor] = Fw::getU16(fin);
        }
        else if(property == ThingType::HasDisplacement) {
            thingType.parameters[ThingType::DisplacementX] = Fw::getU16(fin);
            thingType.parameters[ThingType::DisplacementY] = Fw::getU16(fin);
        }
        else if(property == ThingType::HasElevation)
            thingType.parameters[ThingType::Elevation] = Fw::getU16(fin);
        else if(property == ThingType::MiniMap)
            thingType.parameters[ThingType::MiniMapColor] = Fw::getU16(fin);
        else if(property == ThingType::LensHelp)
            thingType.parameters[ThingType::LensHelpParameter] = Fw::getU16(fin);
        else if(property == ThingType::Cloth)
            thingType.parameters[ThingType::ClothSlot] = Fw::getU16(fin);
    }

    int totalSprites = 1;
    for(int i = 0; i < ThingType::LastDimension; ++i) {
        if(i == ThingType::ExactSize && thingType.dimensions[ThingType::Width] <= 1 && thingType.dimensions[ThingType::Height] <= 1) {
            thingType.dimensions[i] = 32;
            continue;
        }

        thingType.dimensions[i] = Fw::getU8(fin);

        if(i != ThingType::ExactSize)
            totalSprites *= thingType.dimensions[i];
    }

    thingType.sprites.resize(totalSprites);
    for(int i = 0; i < totalSprites; i++)
        thingType.sprites[i] = Fw::getU16(fin);
}

ThingType *ThingsType::getThingType(uint16 id, Categories category)
{
    if(id == 0)
        return &m_emptyThingType;

    if(category == Item)
        id -= 100;
    else if(category == Creature || category == Effect || category == Missile)
        id -= 1;

    if(id >= m_things[category].size())
        return &m_emptyThingType;
    return &m_things[category][id];
}

void ThingsType::exportThings()
{
    for(int i = Item; i < LastCategory; i++) {
        for(uint32 id = 0; id < m_things[i].size(); id++) {
            OTMLDocumentPtr doc = OTMLDocument::create();

            doc->writeAt("ID", id);

            if(m_things[i][id].properties[ThingType::IsGround])
                 doc->writeAt("GroundSpeed", m_things[i][id].parameters[ThingType::GroundSpeed]);

            if(m_things[i][id].properties[ThingType::IsWritable] || m_things[i][id].properties[ThingType::IsWritableOnce]){
                if(m_things[i][id].properties[ThingType::IsWritableOnce])
                    doc->writeAt("WritableOnce", true);

                doc->writeAt("MaxTextLenght", m_things[i][id].parameters[ThingType::MaxTextLenght]);
            }

            if(m_things[i][id].properties[ThingType::HasLight]) {
                doc->writeAt("LightLevel", m_things[i][id].parameters[ThingType::LightLevel]);
                doc->writeAt("LightColor", m_things[i][id].parameters[ThingType::LightColor]);
            }

            if(m_things[i][id].properties[ThingType::HasDisplacement]) {
                doc->writeAt("DisplacementX", m_things[i][id].parameters[ThingType::DisplacementX]);
                doc->writeAt("DisplacementY", m_things[i][id].parameters[ThingType::DisplacementY]);
            }

            if(m_things[i][id].properties[ThingType::HasElevation])
                doc->writeAt("Elevation", m_things[i][id].parameters[ThingType::Elevation]);

            if(m_things[i][id].properties[ThingType::MiniMap])
                doc->writeAt("MiniMapColor", m_things[i][id].parameters[ThingType::MiniMapColor]);

            if(m_things[i][id].properties[ThingType::LensHelp])
                doc->writeAt("LensHelp", m_things[i][id].parameters[ThingType::LensHelp]);

            if(m_things[i][id].properties[ThingType::Cloth])
                doc->writeAt("ClothSlot", m_things[i][id].parameters[ThingType::ClothSlot]);

            OTMLNodePtr sprites = OTMLNode::create("Sprites", true);
            std::string folderName;

            if(i == Item)
                folderName = Fw::formatString("data/items/%d", id);
            else if(i == Creature)
                 folderName = Fw::formatString("data/creatures/%d", id);
            else if(i == Effect)
                folderName = Fw::formatString("data/effects/%d", id);
            else if(i == Missile)
                 folderName = Fw::formatString("data/missiles/%d", id);
            else
                logError("[ThingsType::exportThings] Unexpected error.");

            g_resources.makeDir(folderName);
            for(uint32 count = 0; count < m_things[i][id].sprites.size(); count++)
            {
                std::string fileName = Fw::formatString("%s/%d.png", folderName.c_str(), m_things[i][id].sprites[count]);
                if(m_things[i][id].sprites[count] > 0) { // WTF?
                    if(!g_sprites.exportSprite(fileName, m_things[i][id].sprites[count], SpriteManager::PNG_COMPRESSION))
                        logError("id", id, "sprite", m_things[i][id].sprites[count]);

                    std::string key = Fw::formatString("Sprite%d", count);
                    sprites->writeAt(key, m_things[i][id].sprites[count]);
                }
            }

            doc->addChild(sprites);
            std::string fileName = Fw::formatString("%s/%d.otml", folderName.c_str(), id);
            doc->save(fileName);
        }
    }
}

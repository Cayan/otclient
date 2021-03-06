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

#include "creature.h"
#include "thingstype.h"
#include "localplayer.h"
#include "map.h"
#include "tile.h"
#include "item.h"
#include "game.h"
#include "effect.h"

#include <framework/graphics/graphics.h>
#include <framework/core/eventdispatcher.h>
#include <framework/core/clock.h>

#include <framework/graphics/paintershaderprogram.h>
#include <framework/graphics/painterogl2_shadersources.h>
#include <framework/graphics/texturemanager.h>
#include <framework/graphics/framebuffer.h>
#include "spritemanager.h"

Creature::Creature() : Thing()
{
    m_healthPercent = 0;
    m_showTimedSquare = false;
    m_showStaticSquare = false;
    m_direction = Otc::South;
    m_walkAnimationPhase = 0;
    m_walking = false;
    m_walkInterval = 0;
    m_walkAnimationInterval = 0;
    m_walkTurnDirection = Otc::InvalidDirection;
    m_skull = Otc::SkullNone;
    m_shield = Otc::ShieldNone;
    m_emblem = Otc::EmblemNone;
    m_shieldBlink = false;
    m_showShieldTexture = true;
    m_removed = false;
    m_informationFont = g_fonts.getFont("verdana-11px-rounded");
}

/*
PainterShaderProgramPtr outfitProgram;
int HEAD_COLOR_UNIFORM = 10;
int BODY_COLOR_UNIFORM = 11;
int LEGS_COLOR_UNIFORM = 12;
int FEET_COLOR_UNIFORM = 13;
int MASK_TEXTURE_UNIFORM = 14;
*/

void Creature::draw(const Point& dest, float scaleFactor, bool animate)
{
    Point animationOffset = animate ? m_walkOffset : Point(0,0);

    if(m_showTimedSquare && animate) {
        g_painter->setColor(m_timedSquareColor);
        g_painter->drawBoundingRect(Rect(dest + (animationOffset - getDisplacement() + 3)*scaleFactor, Size(28, 28)*scaleFactor), std::max((int)(2*scaleFactor), 1));
    }

    if(m_showStaticSquare && animate) {
        g_painter->setColor(m_staticSquareColor);
        g_painter->drawBoundingRect(Rect(dest + (animationOffset - getDisplacement() + 1)*scaleFactor, Size(Otc::TILE_PIXELS, Otc::TILE_PIXELS)*scaleFactor), std::max((int)(2*scaleFactor), 1));
    }

    internalDrawOutfit(dest + animationOffset * scaleFactor, scaleFactor, animate, animate, m_direction);

}

void Creature::internalDrawOutfit(const Point& dest, float scaleFactor, bool animateWalk, bool animateIdle, Otc::Direction direction)
{
    g_painter->setColor(Color::white);
    /*
    if(!outfitProgram) {
        outfitProgram = PainterShaderProgramPtr(new PainterShaderProgram);
        outfitProgram->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader + glslPositionOnlyVertexShader);
        outfitProgram->addShaderFromSourceFile(Shader::Fragment, "/game_shaders/outfit.frag");
        outfitProgram->link();
        outfitProgram->bindUniformLocation(HEAD_COLOR_UNIFORM, "headColor");
        outfitProgram->bindUniformLocation(BODY_COLOR_UNIFORM, "bodyColor");
        outfitProgram->bindUniformLocation(LEGS_COLOR_UNIFORM, "legsColor");
        outfitProgram->bindUniformLocation(FEET_COLOR_UNIFORM, "feetColor");
    }
    */

    int xPattern = 0, yPattern = 0, zPattern = 0;

    // outfit is a real creature
    if(m_outfit.getCategory() == ThingsType::Creature) {
        int animationPhase = animateWalk ? m_walkAnimationPhase : 0;

        if(isAnimateAlways() && animateIdle) {
            int ticksPerFrame = 1000 / getAnimationPhases();
            animationPhase = (g_clock.ticks() % (ticksPerFrame * getAnimationPhases())) / ticksPerFrame;
        }

        // xPattern => creature direction
        if(direction == Otc::NorthEast || direction == Otc::SouthEast)
            xPattern = Otc::East;
        else if(direction == Otc::NorthWest || direction == Otc::SouthWest)
            xPattern = Otc::West;
        else
            xPattern = direction;

        // yPattern => creature addon
        for(yPattern = 0; yPattern < getNumPatternsY(); yPattern++) {

            // continue if we dont have this addon.
            if(yPattern > 0 && !(m_outfit.getAddons() & (1 << (yPattern-1))))
                continue;

            /*
            g_painter->setShaderProgram(outfitProgram);

            outfitProgram->bind();
            outfitProgram->setUniformValue(HEAD_COLOR_UNIFORM, m_outfit.getHeadColor());
            outfitProgram->setUniformValue(BODY_COLOR_UNIFORM, m_outfit.getBodyColor());
            outfitProgram->setUniformValue(LEGS_COLOR_UNIFORM, m_outfit.getLegsColor());
            outfitProgram->setUniformValue(FEET_COLOR_UNIFORM, m_outfit.getFeetColor());
            */

            for(int h = 0; h < getDimensionHeight(); h++) {
                for(int w = 0; w < getDimensionWidth(); w++) {
                    // setup texture outfit mask
                    TexturePtr maskTex;
                    if(getLayers() > 1) {
                        int maskId = getSpriteId(w, h, 1, xPattern, yPattern, zPattern, animationPhase);
                        maskTex = g_sprites.getSpriteTexture(maskId);
                    }
                    //outfitProgram->setTexture(maskTex, 1);

                    internalDraw(dest + (-Point(w,h)*Otc::TILE_PIXELS)*scaleFactor,
                                 scaleFactor, w, h, xPattern, yPattern, zPattern, 0, animationPhase);
                }
            }

            //g_painter->resetShaderProgram();
        }
    // outfit is a creature imitating an item or the invisible effect
    } else  {
        int animationPhase = 0;
        int animationPhases = getAnimationPhases();
        int animateTicks = Otc::ITEM_TICKS_PER_FRAME;

        // when creature is an effect we cant render the first and last animation phase,
        // instead we should loop in the phases between
        if(m_outfit.getCategory() == ThingsType::Effect) {
            animationPhases = std::max(1, animationPhases-2);
            animateTicks = Otc::INVISIBLE_TICKS_PER_FRAME;
        }

        if(animationPhases > 1) {
            if(animateIdle)
                animationPhase = (g_clock.ticks() % (animateTicks * animationPhases)) / animateTicks;
            else
                animationPhase = animationPhases-1;
        }

        if(m_outfit.getCategory() == ThingsType::Effect)
            animationPhase = std::min(animationPhase+1, getAnimationPhases());

        internalDraw(dest, scaleFactor, 0, 0, 0, animationPhase);
    }
}

void Creature::drawOutfit(const Rect& destRect, bool resize)
{
    if(g_graphics.canUseFBO()) {
        static FrameBufferPtr outfitBuffer;
        if(!outfitBuffer)
            outfitBuffer = FrameBufferPtr(new FrameBuffer(Size(2*Otc::TILE_PIXELS, 2*Otc::TILE_PIXELS)));

        g_painter->saveAndResetState();
        outfitBuffer->bind();
        outfitBuffer->clear(Color::alpha);
        internalDrawOutfit(Point(Otc::TILE_PIXELS,Otc::TILE_PIXELS) + getDisplacement(), 1, false, true, Otc::South);
        outfitBuffer->release();
        g_painter->restoreSavedState();

        Rect srcRect;
        if(resize)
            srcRect.resize(getExactSize(), getExactSize());
        else
            srcRect.resize(2*Otc::TILE_PIXELS*0.75f, 2*Otc::TILE_PIXELS*0.75f);
        srcRect.moveBottomRight(Point(2*Otc::TILE_PIXELS - 1, 2*Otc::TILE_PIXELS - 1));
        outfitBuffer->draw(destRect, srcRect);
    } else {
        float scaleFactor;
        if(resize)
            scaleFactor = destRect.width() / (float)getExactSize();
        else
            scaleFactor = destRect.width() / (float)(2*Otc::TILE_PIXELS*0.75f);
        Point dest = destRect.bottomRight() - (Point(Otc::TILE_PIXELS,Otc::TILE_PIXELS) - getDisplacement())*scaleFactor;
        internalDrawOutfit(dest, scaleFactor, false, true, Otc::South);
    }
}

void Creature::drawInformation(const Point& point, bool useGray, const Rect& parentRect)
{
    Color fillColor = Color(96, 96, 96);

    if(!useGray)
        fillColor = m_informationColor;

    // calculate main rects
    Rect backgroundRect = Rect(point.x-(13.5), point.y, 27, 4);
    backgroundRect.bind(parentRect);

    Rect textRect = Rect(point.x - m_nameSize.width() / 2.0, point.y-12, m_nameSize);
    textRect.bind(parentRect);

    // distance them
    if(textRect.top() == parentRect.top())
        backgroundRect.moveTop(textRect.top() + 12);
    if(backgroundRect.bottom() == parentRect.bottom())
        textRect.moveTop(backgroundRect.top() - 12);

    // health rect is based on background rect, so no worries
    Rect healthRect = backgroundRect.expanded(-1);
    healthRect.setWidth((m_healthPercent / 100.0) * 25);

    // draw
    g_painter->setColor(Color::black);
    g_painter->drawFilledRect(backgroundRect);

    g_painter->setColor(fillColor);
    g_painter->drawFilledRect(healthRect);

    if(m_informationFont)
        m_informationFont->drawText(m_name, textRect, Fw::AlignTopCenter);

    if(m_skull != Otc::SkullNone && m_skullTexture) {
        g_painter->setColor(Color::white);
        g_painter->drawTexturedRect(Rect(point.x + 12, point.y + 5, m_skullTexture->getSize()), m_skullTexture);
    }
    if(m_shield != Otc::ShieldNone && m_shieldTexture && m_showShieldTexture) {
        g_painter->setColor(Color::white);
        g_painter->drawTexturedRect(Rect(point.x, point.y + 5, m_shieldTexture->getSize()), m_shieldTexture);
    }
    if(m_emblem != Otc::EmblemNone && m_emblemTexture) {
        g_painter->setColor(Color::white);
        g_painter->drawTexturedRect(Rect(point.x + 12, point.y + 16, m_emblemTexture->getSize()), m_emblemTexture);
    }
}

void Creature::turn(Otc::Direction direction)
{
    // if is not walking change the direction right away
    if(!m_walking)
        setDirection(direction);
    // schedules to set the new direction when walk ends
    else
        m_walkTurnDirection = direction;
}

void Creature::walk(const Position& oldPos, const Position& newPos)
{
    // get walk direction
    Otc::Direction direction = oldPos.getDirectionFromPosition(newPos);

    // set current walking direction
    setDirection(direction);

    // starts counting walk
    m_walking = true;
    m_walkTimer.restart();

    // calculates walk interval
    float interval = 1000;
    int groundSpeed = 0;

    TilePtr oldTile = g_map.getTile(oldPos);
    if(oldTile)
        groundSpeed = oldTile->getGroundSpeed();

    if(groundSpeed != 0)
        interval = (1000.0f * groundSpeed) / m_speed;

    interval = std::ceil(interval / g_game.getServerBeat()) * g_game.getServerBeat();

    m_walkAnimationInterval = interval;
    m_walkInterval = interval;

    // diagonal walking lasts 3 times more.
    if(direction == Otc::NorthWest || direction == Otc::NorthEast || direction == Otc::SouthWest || direction == Otc::SouthEast)
        m_walkInterval *= 3;

    // no direction needs to be changed when the walk ends
    m_walkTurnDirection = Otc::InvalidDirection;

    // starts updating walk
    nextWalkUpdate();
}

void Creature::stopWalk()
{
    if(!m_walking)
        return;

    // reset walk animation states
    m_walkOffset = Point(0,0);
    m_walkAnimationPhase = 0;

    // stops the walk right away
    terminateWalk();
}

void Creature::updateWalkAnimation(int totalPixelsWalked)
{
    // update outfit animation
    if(m_outfit.getCategory() != ThingsType::Creature)
        return;

    if(totalPixelsWalked == 32 || totalPixelsWalked == 0 || getAnimationPhases() <= 1)
        m_walkAnimationPhase = 0;
    else if(getAnimationPhases() > 1)
        m_walkAnimationPhase = 1 + ((totalPixelsWalked * 4) / Otc::TILE_PIXELS) % (getAnimationPhases() - 1);
}

void Creature::updateWalkOffset(int totalPixelsWalked)
{
    m_walkOffset = Point(0,0);
    if(m_direction == Otc::North || m_direction == Otc::NorthEast || m_direction == Otc::NorthWest)
        m_walkOffset.y = 32 - totalPixelsWalked;
    else if(m_direction == Otc::South || m_direction == Otc::SouthEast || m_direction == Otc::SouthWest)
        m_walkOffset.y = totalPixelsWalked - 32;

    if(m_direction == Otc::East || m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
        m_walkOffset.x = totalPixelsWalked - 32;
    else if(m_direction == Otc::West || m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
        m_walkOffset.x = 32 - totalPixelsWalked;
}

void Creature::updateWalkingTile()
{
    // determine new walking tile
    TilePtr newWalkingTile;
    Rect virtualCreatureRect(Otc::TILE_PIXELS + (m_walkOffset.x - getDisplacementX()),
                             Otc::TILE_PIXELS + (m_walkOffset.y - getDisplacementY()),
                             Otc::TILE_PIXELS, Otc::TILE_PIXELS);
    for(int xi = -1; xi <= 1 && !newWalkingTile; ++xi) {
        for(int yi = -1; yi <= 1 && !newWalkingTile; ++yi) {
            Rect virtualTileRect((xi+1)*Otc::TILE_PIXELS, (yi+1)*Otc::TILE_PIXELS, Otc::TILE_PIXELS, Otc::TILE_PIXELS);

            // only render creatures where bottom right is inside tile rect
            if(virtualTileRect.contains(virtualCreatureRect.bottomRight())) {
                const TilePtr& tile = g_map.getTile(m_position.translated(xi, yi, 0));
                if(!tile)
                    continue;
                newWalkingTile = tile;
            }
        }
    }

    if(newWalkingTile != m_walkingTile) {
        if(m_walkingTile)
            m_walkingTile->removeWalkingCreature(asCreature());
        if(newWalkingTile)
            newWalkingTile->addWalkingCreature(asCreature());
        m_walkingTile = newWalkingTile;
    }
}

void Creature::nextWalkUpdate()
{
    // remove any previous scheduled walk updates
    if(m_walkUpdateEvent)
        m_walkUpdateEvent->cancel();

    // do the update
    updateWalk();

    // schedules next update
    if(m_walking) {
        auto self = asCreature();
        m_walkUpdateEvent = g_eventDispatcher.scheduleEvent([self] {
            self->m_walkUpdateEvent = nullptr;
            self->nextWalkUpdate();
        }, m_walkAnimationInterval / 32);
    }
}

void Creature::updateWalk()
{
    float walkTicksPerPixel = m_walkAnimationInterval / 32;
    int totalPixelsWalked = std::min(m_walkTimer.ticksElapsed() / walkTicksPerPixel, 32.0f);

    // update walk animation and offsets
    updateWalkAnimation(totalPixelsWalked);
    updateWalkOffset(totalPixelsWalked);
    updateWalkingTile();

    // terminate walk
    if(m_walking && m_walkTimer.ticksElapsed() >= m_walkInterval)
        terminateWalk();
}

void Creature::terminateWalk()
{
    // remove any scheduled walk update
    if(m_walkUpdateEvent) {
        m_walkUpdateEvent->cancel();
        m_walkUpdateEvent = nullptr;
    }

    // now the walk has ended, do any scheduled turn
    if(m_walkTurnDirection != Otc::InvalidDirection)  {
        setDirection(m_walkTurnDirection);
        m_walkTurnDirection = Otc::InvalidDirection;
    }

    if(m_walkingTile) {
        m_walkingTile->removeWalkingCreature(asCreature());
        m_walkingTile = nullptr;
    }

    m_walking = false;
}

void Creature::setName(const std::string& name)
{
    if(m_informationFont)
        m_nameSize = m_informationFont->calculateTextRectSize(name);
    m_name = name;
}

void Creature::setHealthPercent(uint8 healthPercent)
{
    m_informationColor = Color::black;

    if(healthPercent > 92) {
        m_informationColor.setGreen(188);
    }
    else if(healthPercent > 60) {
        m_informationColor.setRed(80);
        m_informationColor.setGreen(161);
        m_informationColor.setBlue(80);
    }
    else if(healthPercent > 30) {
        m_informationColor.setRed(161);
        m_informationColor.setGreen(161);
    }
    else if(healthPercent > 8) {
        m_informationColor.setRed(160);
        m_informationColor.setGreen(39);
        m_informationColor.setBlue(39);
    }
    else if(healthPercent > 3) {
        m_informationColor.setRed(160);
    }
    else {
        m_informationColor.setRed(79);
    }

    m_healthPercent = healthPercent;
}

void Creature::setDirection(Otc::Direction direction)
{
    m_direction = direction;
}

void Creature::setOutfit(const Outfit& outfit)
{
    m_outfit = outfit;
    m_type = g_thingsType.getThingType(outfit.getId(), outfit.getCategory());
}

void Creature::setSkull(uint8 skull)
{
    m_skull = skull;
    callLuaField("onSkullChange", m_skull);
}

void Creature::setShield(uint8 shield)
{
    m_shield = shield;
    callLuaField("onShieldChange", m_shield);
}

void Creature::setEmblem(uint8 emblem)
{
    m_emblem = emblem;
    callLuaField("onEmblemChange", m_emblem);
}

void Creature::setSkullTexture(const std::string& filename)
{
    m_skullTexture = g_textures.getTexture(filename);
}

void Creature::setShieldTexture(const std::string& filename, bool blink)
{
    m_shieldTexture = g_textures.getTexture(filename);
    m_showShieldTexture = true;

    if(blink && !m_shieldBlink) {
        auto self = asCreature();
        g_eventDispatcher.scheduleEvent([self]() {
            self->updateShield();
        }, SHIELD_BLINK_TICKS);
    }

    m_shieldBlink = blink;
}

void Creature::setEmblemTexture(const std::string& filename)
{
    m_emblemTexture = g_textures.getTexture(filename);
}

void Creature::addTimedSquare(uint8 color)
{
    m_showTimedSquare = true;
    m_timedSquareColor = Color::from8bit(color);

    // schedule removal
    auto self = asCreature();
    g_eventDispatcher.scheduleEvent([self]() {
        self->removeTimedSquare();
    }, VOLATILE_SQUARE_DURATION);
}

void Creature::updateShield()
{
    m_showShieldTexture = !m_showShieldTexture;

    if(m_shield != Otc::ShieldNone && m_shieldBlink) {
        auto self = asCreature();
        g_eventDispatcher.scheduleEvent([self]() {
            self->updateShield();
        }, SHIELD_BLINK_TICKS);
    }
    else if(!m_shieldBlink)
        m_showShieldTexture = true;
}

Point Creature::getDrawOffset()
{
    Point drawOffset;
    if(m_walking) {
        if(m_walkingTile)
            drawOffset -= Point(1,1) * m_walkingTile->getDrawElevation();
        drawOffset += m_walkOffset;
    } else {
        const TilePtr& tile = getTile();
        if(tile)
            drawOffset -= Point(1,1) * tile->getDrawElevation();
    }
    return drawOffset;
}

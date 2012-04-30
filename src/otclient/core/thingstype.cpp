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
#include <framework/thirdparty/apngloader.h>

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

bool ThingsType::manualLoad()
{
    if(!g_resources.directoryExists("data/items/") || !g_resources.directoryExists("data/creatures/") || !g_resources.directoryExists("data/effects/") || !g_resources.directoryExists("data/missiles/"))
        return false;

    std::list<std::string> itemsFullPath;
    auto items = g_resources.listDirectoryFiles("data/items/");
    for(const std::string& itemPath : items) {
        auto itemFiles = g_resources.listDirectoryFiles("data/items/" + itemPath);
        for(const std::string& item : itemFiles) {
            if(boost::ends_with(item, ".otml")) {
                itemsFullPath.push_back("data/items/" + itemPath + "/" + item);
            }
        }
    }

    m_things[Item].resize(itemsFullPath.size());
    for(const std::string& itemFileName : itemsFullPath) {
        parseThingType(itemFileName, Item);
    }

    std::list<std::string> creaturesFullPath;
    auto creatures = g_resources.listDirectoryFiles("data/creatures/");
    for(const std::string& creaturePath : creatures) {
        auto creatureFiles = g_resources.listDirectoryFiles("data/creatures/" + creaturePath);
        for(const std::string& creature : creatureFiles) {
            if(boost::ends_with(creature, ".otml")) {
                itemsFullPath.push_back("data/creatures/" + creaturePath + "/" + creature);
            }
        }
    }

    m_things[Creature].resize(creaturesFullPath.size());
    for(const std::string& creatureFileName : creaturesFullPath) {
        parseThingType(creatureFileName, Creature);
    }

    std::list<std::string> effectsFullPath;
    auto effects = g_resources.listDirectoryFiles("data/effects/");
    for(const std::string& effectPath : effects) {
        auto effectFiles = g_resources.listDirectoryFiles("data/effects/" + effectPath);
        for(const std::string& effect : effectFiles) {
            if(boost::ends_with(effect, ".otml")) {
                itemsFullPath.push_back("data/effects/" + effectPath + "/" + effect);
            }
        }
    }

    m_things[Effect].resize(effectsFullPath.size());
    for(const std::string& effectFileName : effectsFullPath) {
        parseThingType(effectFileName, Effect);
    }

    std::list<std::string> missilesFullPath;
    auto missiles = g_resources.listDirectoryFiles("data/missiles/");
    for(const std::string& missilePath : missiles) {
        auto missileFiles = g_resources.listDirectoryFiles("data/missiles/" + missilePath);
        for(const std::string& missile : missileFiles) {
            if(boost::ends_with(missile, ".otml")) {
                itemsFullPath.push_back("data/missiles/" + missilePath + "/" + missile);
            }
        }
    }

    m_things[Missile].resize(missilesFullPath.size());
    for(const std::string& missileFileName : missilesFullPath) {
        parseThingType(missileFileName, Missile);
    }

    return true;
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

void ThingsType::parseThingType(std::string fileName, Categories category)
{
    OTMLDocumentPtr doc = OTMLDocument::parse(fileName);
    int id = doc->valueAt<int>("ID");

    OTMLNodePtr properties = doc->get("Properties");
    OTMLNodePtr parameters = doc->get("Parameters");

    if(properties) {
        if(properties->get("IsGroundBorder"))
            m_things[category][id].properties[ThingType::IsGroundBorder] = properties->valueAt<bool>("IsGroundBorder");

        if(properties->get("IsOnBottom"))
            m_things[category][id].properties[ThingType::IsOnBottom] = properties->valueAt<bool>("IsOnBottom");

        if(properties->get("IsOnTop"))
            m_things[category][id].properties[ThingType::IsOnTop] = properties->valueAt<bool>("IsOnTop");

        if(properties->get("IsContainer"))
            m_things[category][id].properties[ThingType::IsContainer] = properties->valueAt<bool>("IsContainer");

        if(properties->get("IsStackable"))
            m_things[category][id].properties[ThingType::IsStackable] = properties->valueAt<bool>("IsStackable");

        if(properties->get("IsForceUse"))
            m_things[category][id].properties[ThingType::IsForceUse] = properties->valueAt<bool>("IsForceUse");

        if(properties->get("IsMultiUse"))
            m_things[category][id].properties[ThingType::IsMultiUse] = properties->valueAt<bool>("IsMultiUse");

        if(properties->get("IsFluid"))
            m_things[category][id].properties[ThingType::IsFluid] = properties->valueAt<bool>("IsFluid");

        if(properties->get("NotWalkable"))
            m_things[category][id].properties[ThingType::NotWalkable] = properties->valueAt<bool>("NotWalkable");

        if(properties->get("IsNotMovable"))
            m_things[category][id].properties[ThingType::IsNotMovable] = properties->valueAt<bool>("IsNotMovable");

        if(properties->get("BlockProjectile"))
            m_things[category][id].properties[ThingType::BlockProjectile] = properties->valueAt<bool>("BlockProjectile");

        if(properties->get("NotPathable"))
            m_things[category][id].properties[ThingType::NotPathable] = properties->valueAt<bool>("NotPathable");

        if(properties->get("IsPickupable"))
            m_things[category][id].properties[ThingType::IsPickupable] = properties->valueAt<bool>("IsPickupable");

        if(properties->get("IsHangable"))
            m_things[category][id].properties[ThingType::IsHangable] = properties->valueAt<bool>("IsHangable");

        if(properties->get("HookSouth"))
            m_things[category][id].properties[ThingType::HookSouth] = properties->valueAt<bool>("HookSouth");

        if(properties->get("HookEast"))
            m_things[category][id].properties[ThingType::HookEast] = properties->valueAt<bool>("HookEast");

        if(properties->get("IsRotateable"))
            m_things[category][id].properties[ThingType::IsRotateable] = properties->valueAt<bool>("IsRotateable");

        if(properties->get("DontHide"))
            m_things[category][id].properties[ThingType::DontHide] = properties->valueAt<bool>("DontHide");

        if(properties->get("IsTranslucent"))
            m_things[category][id].properties[ThingType::IsTranslucent] = properties->valueAt<bool>("IsTranslucent");

        if(properties->get("IsLyingCorpse"))
            m_things[category][id].properties[ThingType::IsLyingCorpse] = properties->valueAt<bool>("IsLyingCorpse");

        if(properties->get("AnimateAlways"))
            m_things[category][id].properties[ThingType::AnimateAlways] = properties->valueAt<bool>("AnimateAlways");

        if(properties->get("IsFullGround"))
            m_things[category][id].properties[ThingType::IsFullGround] = properties->valueAt<bool>("IsFullGround");

        if(properties->get("IgnoreLook"))
            m_things[category][id].properties[ThingType::IgnoreLook] = properties->valueAt<bool>("IgnoreLook");

        if(properties->get("Animation"))
            m_things[category][id].properties[ThingType::Animation] = properties->valueAt<bool>("Animation");

        if(properties->get("IsGround")) {
            m_things[category][id].properties[ThingType::IsGround] = properties->valueAt<bool>("IsGround");

            if(m_things[category][id].properties[ThingType::IsGround] && parameters && parameters->get("GroundSpeed"))
                m_things[category][id].parameters[ThingType::GroundSpeed] = parameters->valueAt<int>("GroundSpeed");
        }

        if(properties->get("IsWritable") || properties->get("IsWritableOnce")) {
            if(properties->get("IsWritable"))
                m_things[category][id].properties[ThingType::IsWritable] = properties->valueAt<bool>("IsWritable");

            if(properties->get("IsWritableOnce"))
                m_things[category][id].properties[ThingType::IsWritableOnce] = properties->valueAt<bool>("IsWritableOnce");

            if((m_things[category][id].properties[ThingType::IsWritable] || m_things[category][id].properties[ThingType::IsWritableOnce]) && parameters && parameters->get("MaxTextLenght"))
                m_things[category][id].parameters[ThingType::MaxTextLenght] = parameters->valueAt<int>("MaxTextLenght");
        }

        if(properties->get("HasLight")) {
            m_things[category][id].properties[ThingType::HasLight] = properties->valueAt<bool>("HasLight");

            if(parameters) {
                if(m_things[category][id].properties[ThingType::HasLight] && parameters->get("LightLevel"))
                    m_things[category][id].parameters[ThingType::LightLevel] = parameters->valueAt<int>("LightLevel");

                if(m_things[category][id].properties[ThingType::HasLight] && parameters->get("LightColor"))
                    m_things[category][id].parameters[ThingType::LightColor] = parameters->valueAt<int>("LightColor");
            }
        }

        if(properties->get("HasDisplacement")) {
            m_things[category][id].properties[ThingType::HasDisplacement] = properties->valueAt<bool>("HasDisplacement");

            if(parameters) {
                if(m_things[category][id].properties[ThingType::HasDisplacement] && parameters->get("DisplacementX"))
                    m_things[category][id].parameters[ThingType::DisplacementX] = parameters->valueAt<int>("DisplacementX");

                if(m_things[category][id].properties[ThingType::HasDisplacement] && parameters->get("DisplacementY"))
                    m_things[category][id].parameters[ThingType::DisplacementY] = parameters->valueAt<int>("DisplacementY");
            }
        }

        if(properties->get("HasElevation")) {
            m_things[category][id].properties[ThingType::HasElevation] = properties->valueAt<bool>("HasElevation");

            if(m_things[category][id].properties[ThingType::HasElevation] && parameters && parameters->get("Elevation"))
                m_things[category][id].parameters[ThingType::Elevation] = parameters->valueAt<int>("Elevation");
        }

        if(properties->get("MiniMap")) {
            m_things[category][id].properties[ThingType::MiniMap] = properties->valueAt<bool>("MiniMap");

            if(m_things[category][id].properties[ThingType::MiniMap] && parameters && parameters->get("MiniMapColor"))
                m_things[category][id].parameters[ThingType::MiniMapColor] = parameters->valueAt<int>("MiniMapColor");
        }

        if(properties->get("LensHelp")) {
            m_things[category][id].properties[ThingType::LensHelp] = properties->valueAt<bool>("LensHelp");

            if(m_things[category][id].properties[ThingType::LensHelp] && parameters && parameters->get("LensHelpParameter"))
                m_things[category][id].parameters[ThingType::LensHelpParameter] = parameters->valueAt<int>("LensHelpParameter");
        }

        if(properties->get("Cloth")) {
            m_things[category][id].properties[ThingType::Cloth] = properties->valueAt<bool>("Cloth");

            if(m_things[category][id].properties[ThingType::Cloth] && parameters && parameters->get("ClothSlot"))
                m_things[category][id].parameters[ThingType::ClothSlot] = parameters->valueAt<int>("ClothSlot");
        }

        /* Debug
        logInfo("IsGroundBorder: ", m_things[category][id].properties[ThingType::IsGroundBorder]);
        logInfo("IsOnBottom: ", m_things[category][id].properties[ThingType::IsOnBottom]);
        logInfo("IsOnTop: ", m_things[category][id].properties[ThingType::IsOnTop]);
        logInfo("IsContainer: ", m_things[category][id].properties[ThingType::IsContainer]);
        logInfo("IsStackable: ", m_things[category][id].properties[ThingType::IsStackable]);
        logInfo("IsForceUse: ", m_things[category][id].properties[ThingType::IsForceUse]);
        logInfo("IsMultiUse: ", m_things[category][id].properties[ThingType::IsMultiUse]);
        logInfo("IsFluid: ", m_things[category][id].properties[ThingType::IsFluid]);
        logInfo("NotWalkable: ", m_things[category][id].properties[ThingType::NotWalkable]);
        logInfo("IsNotMovable: ", m_things[category][id].properties[ThingType::IsNotMovable]);
        logInfo("BlockProjectile: ", m_things[category][id].properties[ThingType::BlockProjectile]);
        logInfo("NotPathable: ", m_things[category][id].properties[ThingType::NotPathable]);
        logInfo("IsPickupable: ", m_things[category][id].properties[ThingType::IsPickupable]);
        logInfo("IsHangable: ", m_things[category][id].properties[ThingType::IsHangable]);
        logInfo("HookSouth: ", m_things[category][id].properties[ThingType::HookSouth]);
        logInfo("HookEast: ", m_things[category][id].properties[ThingType::HookEast]);
        logInfo("IsRotateable: ", m_things[category][id].properties[ThingType::IsRotateable]);
        logInfo("DontHide: ", m_things[category][id].properties[ThingType::DontHide]);
        logInfo("IsTranslucent: ", m_things[category][id].properties[ThingType::IsTranslucent]);
        logInfo("IsLyingCorpse: ", m_things[category][id].properties[ThingType::IsLyingCorpse]);
        logInfo("AnimateAlways: ", m_things[category][id].properties[ThingType::AnimateAlways]);
        logInfo("IsFullGround: ", m_things[category][id].properties[ThingType::IsFullGround]);
        logInfo("IgnoreLook: ", m_things[category][id].properties[ThingType::IgnoreLook]);
        logInfo("Animation: ", m_things[category][id].properties[ThingType::Animation]);
        logInfo("IsGround: ", m_things[category][id].properties[ThingType::IsGround]);
        logInfo("GroundSpeed: ", m_things[category][id].parameters[ThingType::GroundSpeed]);
        logInfo("IsWritable: ", m_things[category][id].properties[ThingType::IsWritable]);
        logInfo("IsWritableOnce: ", m_things[category][id].properties[ThingType::IsWritableOnce]);
        logInfo("MaxTextLenght: ", m_things[category][id].parameters[ThingType::MaxTextLenght]);
        logInfo("HasLight: ", m_things[category][id].properties[ThingType::HasLight]);
        logInfo("LightLevel: ", m_things[category][id].parameters[ThingType::LightLevel]);
        logInfo("LightColor: ", m_things[category][id].parameters[ThingType::LightColor]);
        logInfo("HasDisplacement: ", m_things[category][id].properties[ThingType::HasDisplacement]);
        logInfo("DisplacementX: ", m_things[category][id].parameters[ThingType::DisplacementX]);
        logInfo("DisplacementY: ", m_things[category][id].parameters[ThingType::DisplacementY]);
        logInfo("HasElevation: ", m_things[category][id].properties[ThingType::HasElevation]);
        logInfo("Elevation: ", m_things[category][id].parameters[ThingType::Elevation]);
        logInfo("MiniMap: ", m_things[category][id].properties[ThingType::MiniMap]);
        logInfo("MiniMapColor: ", m_things[category][id].parameters[ThingType::MiniMapColor]);
        logInfo("LensHelp: ", m_things[category][id].properties[ThingType::LensHelp]);
        logInfo("LensHelpParameter: ", m_things[category][id].parameters[ThingType::LensHelpParameter]);
        logInfo("Cloth: ", m_things[category][id].properties[ThingType::Cloth]);
        logInfo("ClothSlot: ", m_things[category][id].parameters[ThingType::ClothSlot]);
        */
    }

    OTMLNodePtr dimensions = doc->get("Dimensions");
    if(dimensions) {
        if(dimensions->get("Width"))
            m_things[category][id].dimensions[ThingType::Width] = dimensions->valueAt<int>("Width");

        if(dimensions->get("Height"))
            m_things[category][id].dimensions[ThingType::Height] = dimensions->valueAt<int>("Height");

        if(dimensions->get("ExactSize"))
            m_things[category][id].dimensions[ThingType::ExactSize] = dimensions->valueAt<int>("ExactSize");

        if(dimensions->get("Layers"))
            m_things[category][id].dimensions[ThingType::Layers] = dimensions->valueAt<int>("Layers");

        if(dimensions->get("PatternX"))
            m_things[category][id].dimensions[ThingType::PatternX] = dimensions->valueAt<int>("PatternX");

        if(dimensions->get("PatternY"))
            m_things[category][id].dimensions[ThingType::PatternY] = dimensions->valueAt<int>("PatternY");

        if(dimensions->get("PatternZ"))
            m_things[category][id].dimensions[ThingType::PatternZ] = dimensions->valueAt<int>("PatternZ");

        if(dimensions->get("AnimationPhases"))
            m_things[category][id].dimensions[ThingType::AnimationPhases] = dimensions->valueAt<int>("AnimationPhases");

        /* Debug
        logInfo("Width ", m_things[category][id].dimensions[ThingType::Width]);
        logInfo("Height: ", m_things[category][id].dimensions[ThingType::Height]);
        logInfo("ExactSize: ", m_things[category][id].dimensions[ThingType::ExactSize]);
        logInfo("Layers: ", m_things[category][id].dimensions[ThingType::Layers]);
        logInfo("PatternX: ", m_things[category][id].dimensions[ThingType::PatternX]);
        logInfo("PatternY: ", m_things[category][id].dimensions[ThingType::PatternY]);
        logInfo("PatternZ: ", m_things[category][id].dimensions[ThingType::PatternZ]);
        logInfo("AnimationPhases: ", m_things[category][id].dimensions[ThingType::AnimationPhases]);
        */
    }

    OTMLNodePtr sprites = doc->get("Sprites");
    if(sprites) {
        int spriteCount = 0;
        m_things[category][id].sprites.resize(sprites->size());
        for(OTMLNodePtr spriteNode : sprites->children()) {
            std::string sprite = spriteNode->value<std::string>();
            if(sprite == "None")
                m_things[category][id].sprites[spriteCount] = 0;
            else {
                // TODO: Check the existance of the sprite.
                auto pos = sprite.rfind(".");
                if(pos != std::string::npos)
                    m_things[category][id].sprites[spriteCount] = atoi(sprite.substr(0, pos).c_str());
            }

            /* Debug
            logInfo("Effect", spriteCount, ": ", m_things[category][id].sprites[spriteCount]);
            */
            spriteCount++;
        }
    }
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
            uint32 virtualId = id;

            OTMLNodePtr properties = OTMLNode::create("Properties", true);
            OTMLNodePtr parameters = OTMLNode::create("Parameters", true);

            if(m_things[i][id].properties[ThingType::IsGroundBorder])
                properties->writeAt("IsGroundBorder", true);

            if(m_things[i][id].properties[ThingType::IsOnBottom])
                properties->writeAt("IsOnBottom", true);

            if(m_things[i][id].properties[ThingType::IsOnTop])
                properties->writeAt("IsOnTop", true);

            if(m_things[i][id].properties[ThingType::IsContainer])
                properties->writeAt("IsContainer", true);

            if(m_things[i][id].properties[ThingType::IsStackable])
                properties->writeAt("IsStackable", true);

            if(m_things[i][id].properties[ThingType::IsForceUse])
                properties->writeAt("IsForceUse", true);

            if(m_things[i][id].properties[ThingType::IsMultiUse])
                properties->writeAt("IsMultiUse", true);

            if(m_things[i][id].properties[ThingType::IsFluidContainer])
                properties->writeAt("IsFluidContainer", true);

            if(m_things[i][id].properties[ThingType::IsFluid])
                properties->writeAt("IsFluid", true);

            if(m_things[i][id].properties[ThingType::NotWalkable])
                properties->writeAt("NotWalkable", true);

            if(m_things[i][id].properties[ThingType::IsNotMovable])
                properties->writeAt("IsNotMovable", true);

            if(m_things[i][id].properties[ThingType::BlockProjectile])
                properties->writeAt("BlockProjectile", true);

            if(m_things[i][id].properties[ThingType::NotPathable])
                properties->writeAt("NotPathable", true);

            if(m_things[i][id].properties[ThingType::IsPickupable])
                properties->writeAt("IsPickupable", true);

            if(m_things[i][id].properties[ThingType::IsHangable])
                properties->writeAt("IsHangable", true);

            if(m_things[i][id].properties[ThingType::HookSouth])
                properties->writeAt("HookSouth", true);

            if(m_things[i][id].properties[ThingType::HookEast])
                properties->writeAt("HookEast", true);

            if(m_things[i][id].properties[ThingType::IsRotateable])
                properties->writeAt("IsRotateable", true);

            if(m_things[i][id].properties[ThingType::DontHide])
                properties->writeAt("DontHide", true);

            if(m_things[i][id].properties[ThingType::IsTranslucent])
                properties->writeAt("IsTranslucent", true);

            if(m_things[i][id].properties[ThingType::IsLyingCorpse])
                properties->writeAt("IsLyingCorpse", true);

            if(m_things[i][id].properties[ThingType::AnimateAlways])
                properties->writeAt("AnimateAlways", true);

            if(m_things[i][id].properties[ThingType::IsFullGround])
                properties->writeAt("IsFullGround", true);

            if(m_things[i][id].properties[ThingType::IgnoreLook])
                properties->writeAt("IgnoreLook", true);

            if(m_things[i][id].properties[ThingType::Animation])
                properties->writeAt("Animation", true);

            if(m_things[i][id].properties[ThingType::IsGround]) {
                properties->writeAt("IsGround", true);
                parameters->writeAt("GroundSpeed", m_things[i][id].parameters[ThingType::GroundSpeed]);
            }

            if(m_things[i][id].properties[ThingType::IsWritable] || m_things[i][id].properties[ThingType::IsWritableOnce]){
                if(m_things[i][id].properties[ThingType::IsWritable])
                    properties->writeAt("IsWritable", true);

                if(m_things[i][id].properties[ThingType::IsWritableOnce])
                    properties->writeAt("IsWritableOnce", true);

                parameters->writeAt("MaxTextLenght", m_things[i][id].parameters[ThingType::MaxTextLenght]);
            }

            if(m_things[i][id].properties[ThingType::HasLight]) {
                properties->writeAt("HasLight", true);
                parameters->writeAt("LightLevel", m_things[i][id].parameters[ThingType::LightLevel]);
                parameters->writeAt("LightColor", m_things[i][id].parameters[ThingType::LightColor]);
            }

            if(m_things[i][id].properties[ThingType::HasDisplacement]) {
                properties->writeAt("HasDisplacement", true);
                parameters->writeAt("DisplacementX", m_things[i][id].parameters[ThingType::DisplacementX]);
                parameters->writeAt("DisplacementY", m_things[i][id].parameters[ThingType::DisplacementY]);
            }

            if(m_things[i][id].properties[ThingType::HasElevation]) {
                properties->writeAt("HasElevation", true);
                parameters->writeAt("Elevation", m_things[i][id].parameters[ThingType::Elevation]);
            }

            if(m_things[i][id].properties[ThingType::MiniMap]) {
                properties->writeAt("MiniMap", true);
                parameters->writeAt("MiniMapColor", m_things[i][id].parameters[ThingType::MiniMapColor]);
            }

            if(m_things[i][id].properties[ThingType::LensHelp]) {
                properties->writeAt("LensHelp", true);
                parameters->writeAt("LensHelpParameter", m_things[i][id].parameters[ThingType::LensHelpParameter]);
            }

            if(m_things[i][id].properties[ThingType::Cloth]) {
                properties->writeAt("Cloth", true);
                parameters->writeAt("ClothSlot", m_things[i][id].parameters[ThingType::ClothSlot]);
            }

            OTMLNodePtr dimensions = OTMLNode::create("Dimensions", true);
            dimensions->writeAt("Width", m_things[i][id].dimensions[ThingType::Width]);
            dimensions->writeAt("Height", m_things[i][id].dimensions[ThingType::Height]);
            dimensions->writeAt("ExactSize", m_things[i][id].dimensions[ThingType::ExactSize]);
            dimensions->writeAt("Layers", m_things[i][id].dimensions[ThingType::Layers]);
            dimensions->writeAt("PatternX", m_things[i][id].dimensions[ThingType::PatternX]);
            dimensions->writeAt("PatternY", m_things[i][id].dimensions[ThingType::PatternY]);
            dimensions->writeAt("PatternZ", m_things[i][id].dimensions[ThingType::PatternZ]);
            dimensions->writeAt("AnimationPhases", m_things[i][id].dimensions[ThingType::AnimationPhases]);

            OTMLNodePtr layers = OTMLNode::create("Layers", true);
            std::string folderName;

            if(i == Item) {
                virtualId += getFirstItemId();
                folderName = Fw::formatString("data/items/%d", virtualId);
            }
            else if(i == Creature)
                 folderName = Fw::formatString("data/creatures/%d", virtualId);
            else if(i == Effect)
                folderName = Fw::formatString("data/effects/%d", virtualId);
            else if(i == Missile)
                 folderName = Fw::formatString("data/missiles/%d", virtualId);
            else {
                logError("[ThingsType::exportThings] Unexpected category.");
                break;
                return;
            }

            g_resources.makeDir(folderName);
            int frameSize = SpriteManager::SPRITE_SIZE * m_things[i][id].dimensions[ThingType::Width] * m_things[i][id].dimensions[ThingType::Height];

            int resultSize = SpriteManager::SPRITE_SIZE * m_things[i][id].dimensions[ThingType::Width] * m_things[i][id].dimensions[ThingType::PatternX] * m_things[i][id].dimensions[ThingType::PatternZ] *
                                        m_things[i][id].dimensions[ThingType::Height] * m_things[i][id].dimensions[ThingType::PatternY] * m_things[i][id].dimensions[ThingType::AnimationPhases];

            for(int l = 0; l < m_things[i][id].dimensions[ThingType::Layers]; l++) {
                uint8* result = new uint8[resultSize];
                memset(result, 0, resultSize);

                for(int a = 0; a < m_things[i][id].dimensions[ThingType::AnimationPhases]; a++) {
                    for(int z = 0; z < m_things[i][id].dimensions[ThingType::PatternZ]; z++) {
                        for(int y = 0; y < m_things[i][id].dimensions[ThingType::PatternY]; y++) {
                            for(int x = 0; x < m_things[i][id].dimensions[ThingType::PatternX]; x++) {
                                uint8* frame = new uint8[frameSize];
                                memset(frame, 0, frameSize);

                                int adjustX;
                                int adjustY;
                                for(int h = 0; h < m_things[i][id].dimensions[ThingType::Height]; h++) {
                                    for(int w = 0; w < m_things[i][id].dimensions[ThingType::Width]; w++) {
                                        uint8* sprite = new uint8[SpriteManager::SPRITE_SIZE];
                                        memset(sprite, 0, SpriteManager::SPRITE_SIZE);

                                        if(g_sprites.getSpriteData(m_things[i][id].getSpriteId(w, h, l, x, y, z, a), sprite)) {
                                            adjustX = SpriteManager::SPRITE_WIDTH * m_things[i][id].dimensions[ThingType::Width] - SpriteManager::SPRITE_WIDTH*(w + 1);
                                            adjustY = SpriteManager::SPRITE_HEIGHT * m_things[i][id].dimensions[ThingType::Height] - SpriteManager::SPRITE_HEIGHT*(h + 1);

                                            g_sprites.insertSprite(adjustX, adjustY, sprite, SpriteManager::SPRITE_WIDTH, SpriteManager::SPRITE_HEIGHT, frame,
                                                         SpriteManager::SPRITE_WIDTH * m_things[i][id].dimensions[ThingType::Width],
                                                         SpriteManager::SPRITE_HEIGHT * m_things[i][id].dimensions[ThingType::Height], SpriteManager::SPRITE_CHANNELS);
                                        }

                                        delete sprite;
                                    }
                                }

                                adjustX = SpriteManager::SPRITE_WIDTH * m_things[i][id].dimensions[ThingType::Width] * (x + m_things[i][id].dimensions[ThingType::PatternX] * z);
                                adjustY = SpriteManager::SPRITE_HEIGHT * m_things[i][id].dimensions[ThingType::Height] * (y + m_things[i][id].dimensions[ThingType::PatternY] * a);

                                g_sprites.insertSprite(adjustX, adjustY,
                                            frame,
                                                SpriteManager::SPRITE_WIDTH * m_things[i][id].dimensions[ThingType::Width],
                                                SpriteManager::SPRITE_HEIGHT * m_things[i][id].dimensions[ThingType::Height],
                                            result,
                                                 SpriteManager::SPRITE_WIDTH * m_things[i][id].dimensions[ThingType::Width] * m_things[i][id].dimensions[ThingType::PatternX] * m_things[i][id].dimensions[ThingType::PatternZ],
                                                 SpriteManager::SPRITE_HEIGHT * m_things[i][id].dimensions[ThingType::Height] * m_things[i][id].dimensions[ThingType::PatternY] * m_things[i][id].dimensions[ThingType::AnimationPhases],
                                             SpriteManager::SPRITE_CHANNELS);

                                delete frame;
                            }
                        }
                    }
                }

                std::stringstream ss;
                save_png(ss, SpriteManager::SPRITE_WIDTH * m_things[i][id].dimensions[ThingType::Width] * m_things[i][id].dimensions[ThingType::PatternX] * m_things[i][id].dimensions[ThingType::PatternZ],
                     SpriteManager::SPRITE_HEIGHT * m_things[i][id].dimensions[ThingType::Height] * m_things[i][id].dimensions[ThingType::PatternY] * m_things[i][id].dimensions[ThingType::AnimationPhases],
                     SpriteManager::SPRITE_CHANNELS, result, SpriteManager::PNG_COMPRESSION);

                std::string value = Fw::formatString("layer%d.png", l);
                std::string fileName = Fw::formatString("%s/%s", folderName.c_str(), value.c_str());
                g_resources.saveFile(fileName, ss);
                layers->writeIn(value);
                delete result;
            }

            if(properties->size())
                doc->addChild(properties);

            if(parameters->size())
                doc->addChild(parameters);

            doc->addChild(dimensions);
            doc->addChild(layers);
            std::string fileName = Fw::formatString("%s/%d.otml", folderName.c_str(), virtualId);
            doc->save(fileName);
        }
    }
}

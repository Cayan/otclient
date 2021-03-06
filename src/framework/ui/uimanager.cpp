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

#include "uimanager.h"
#include "ui.h"

#include <framework/otml/otml.h>
#include <framework/graphics/graphics.h>
#include <framework/platform/platformwindow.h>
#include <framework/core/eventdispatcher.h>

UIManager g_ui;

void UIManager::init()
{
    // creates root widget
    m_rootWidget = UIWidgetPtr(new UIWidget);
    m_rootWidget->setId("root");
    m_mouseReceiver = m_rootWidget;
    m_keyboardReceiver = m_rootWidget;
}

void UIManager::terminate()
{
    // destroy root widget and its children
    m_rootWidget->destroy();
    m_mouseReceiver = nullptr;
    m_keyboardReceiver = nullptr;
    m_rootWidget = nullptr;
    m_draggingWidget = nullptr;
    m_hoveredWidget = nullptr;
    m_pressedWidget = nullptr;
}

void UIManager::render()
{
    m_rootWidget->draw(m_rootWidget->getRect());
}

void UIManager::resize(const Size& size)
{
    m_rootWidget->setSize(g_window.getSize());
}

void UIManager::inputEvent(const InputEvent& event)
{
    m_isOnInputEvent = true;
    UIWidgetList widgetList;
    switch(event.type) {
        case Fw::KeyTextInputEvent:
            m_keyboardReceiver->propagateOnKeyText(event.keyText);
            break;
        case Fw::KeyDownInputEvent:
            m_keyboardReceiver->propagateOnKeyDown(event.keyCode, event.keyboardModifiers);
            break;
        case Fw::KeyPressInputEvent:
            m_keyboardReceiver->propagateOnKeyPress(event.keyCode, event.keyboardModifiers, event.autoRepeatTicks);
            break;
        case Fw::KeyUpInputEvent:
            m_keyboardReceiver->propagateOnKeyUp(event.keyCode, event.keyboardModifiers);
            break;
        case Fw::MousePressInputEvent:
            if(event.mouseButton == Fw::MouseLeftButton && m_mouseReceiver->isVisible()) {
                UIWidgetPtr pressedWidget = m_mouseReceiver->recursiveGetChildByPos(event.mousePos, false);
                if(pressedWidget && !pressedWidget->isEnabled())
                    pressedWidget = nullptr;
                updatePressedWidget(pressedWidget, event.mousePos);
            }

            m_mouseReceiver->propagateOnMouseEvent(event.mousePos, widgetList);
            for(const UIWidgetPtr& widget : widgetList) {
                widget->recursiveFocus(Fw::MouseFocusReason);
                if(widget->onMousePress(event.mousePos, event.mouseButton))
                    break;
            }

            break;
        case Fw::MouseReleaseInputEvent: {
            // release dragging widget
            bool accepted = false;
            if(m_draggingWidget && event.mouseButton == Fw::MouseLeftButton)
                accepted = updateDraggingWidget(nullptr, event.mousePos);

            if(!accepted) {
                m_mouseReceiver->propagateOnMouseEvent(event.mousePos, widgetList);

                // mouse release is always fired first on the pressed widget
                if(m_pressedWidget) {
                    auto it = std::find(widgetList.begin(), widgetList.end(), m_pressedWidget);
                    if(it != widgetList.end())
                        widgetList.erase(it);
                    widgetList.push_front(m_pressedWidget);
                }

                for(const UIWidgetPtr& widget : widgetList) {
                    if(widget->onMouseRelease(event.mousePos, event.mouseButton))
                        break;
                }
            }

            if(m_pressedWidget && event.mouseButton == Fw::MouseLeftButton)
                updatePressedWidget(nullptr, event.mousePos);
            break;
        }
        case Fw::MouseMoveInputEvent: {
            // start dragging when moving a pressed widget
            if(m_pressedWidget && m_pressedWidget->isDragable() && m_draggingWidget != m_pressedWidget) {
                // only drags when moving more than 4 pixels
                if((event.mousePos - m_pressedWidget->getLastClickPosition()).length() >= 4)
                    updateDraggingWidget(m_pressedWidget, event.mousePos - event.mouseMoved);
            }

            // mouse move can change hovered widgets
            updateHoveredWidget();

            // first fire dragging move
            if(m_draggingWidget) {
                if(m_draggingWidget->onDragMove(event.mousePos, event.mouseMoved))
                    break;
            }

            m_mouseReceiver->propagateOnMouseMove(event.mousePos, event.mouseMoved, widgetList);
            for(const UIWidgetPtr& widget : widgetList) {
                if(widget->onMouseMove(event.mousePos, event.mouseMoved))
                    break;
            }
            break;
        }
        case Fw::MouseWheelInputEvent:
            m_mouseReceiver->propagateOnMouseEvent(event.mousePos, widgetList);
            for(const UIWidgetPtr& widget : widgetList) {
                if(widget->onMouseWheel(event.mousePos, event.wheelDirection))
                    break;
            }
            break;
        default:
            break;
    };
    m_isOnInputEvent = false;
}

void UIManager::updatePressedWidget(const UIWidgetPtr& newPressedWidget, const Point& clickedPos)
{
    UIWidgetPtr oldPressedWidget = m_pressedWidget;
    m_pressedWidget = newPressedWidget;

    // when releasing mouse inside pressed widget area send onClick event
    if(oldPressedWidget && oldPressedWidget->isEnabled() && oldPressedWidget->containsPoint(clickedPos))
        oldPressedWidget->onClick(clickedPos);

    if(newPressedWidget)
        newPressedWidget->updateState(Fw::PressedState);

    if(oldPressedWidget)
        oldPressedWidget->updateState(Fw::PressedState);
}

bool UIManager::updateDraggingWidget(const UIWidgetPtr& draggingWidget, const Point& clickedPos)
{
    bool accepted = false;

    UIWidgetPtr oldDraggingWidget = m_draggingWidget;
    m_draggingWidget = nullptr;
    if(oldDraggingWidget) {
        UIWidgetPtr droppedWidget;
        if(!clickedPos.isNull()) {
            auto clickedChildren = m_rootWidget->recursiveGetChildrenByPos(clickedPos);
            for(const UIWidgetPtr& child : clickedChildren) {
                if(child->onDrop(oldDraggingWidget, clickedPos)) {
                    droppedWidget = child;
                    break;
                }
            }
        }

        accepted = oldDraggingWidget->onDragLeave(droppedWidget, clickedPos);
        oldDraggingWidget->updateState(Fw::DraggingState);
    }

    if(draggingWidget) {
        if(draggingWidget->onDragEnter(clickedPos)) {
            m_draggingWidget = draggingWidget;
            draggingWidget->updateState(Fw::DraggingState);
            accepted = true;
        }
    }

    return accepted;
}

void UIManager::updateHoveredWidget()
{
    if(m_hoverUpdateScheduled)
        return;

    g_eventDispatcher.addEvent([this] {
        if(!m_rootWidget)
            return;

        m_hoverUpdateScheduled = false;
        UIWidgetPtr hoveredWidget = m_rootWidget->recursiveGetChildByPos(g_window.getMousePosition(), false);
        if(hoveredWidget && !hoveredWidget->isEnabled())
            hoveredWidget = nullptr;

        if(hoveredWidget != m_hoveredWidget) {
            UIWidgetPtr oldHovered = m_hoveredWidget;
            m_hoveredWidget = hoveredWidget;
            if(oldHovered) {
                oldHovered->updateState(Fw::HoverState);
                oldHovered->onHoverChange(false);
            }
            if(hoveredWidget) {
                hoveredWidget->updateState(Fw::HoverState);
                hoveredWidget->onHoverChange(true);
            }
        }
    });
    m_hoverUpdateScheduled = true;
}

void UIManager::onWidgetAppear(const UIWidgetPtr& widget)
{
    if(widget->containsPoint(g_window.getMousePosition()))
        updateHoveredWidget();
}

void UIManager::onWidgetDisappear(const UIWidgetPtr& widget)
{
    if(widget->containsPoint(g_window.getMousePosition()))
        updateHoveredWidget();
}

void UIManager::onWidgetDestroy(const UIWidgetPtr& widget)
{
    // release input grabs
    if(m_keyboardReceiver == widget)
        resetKeyboardReceiver();

    if(m_mouseReceiver == widget)
        resetMouseReceiver();

    if(m_hoveredWidget == widget)
        updateHoveredWidget();

    if(m_pressedWidget == widget)
        updatePressedWidget(nullptr);

    if(m_draggingWidget == widget)
        updateDraggingWidget(nullptr);

#ifdef DEBUG
    static UIWidgetList destroyedWidgets;
    static ScheduledEventPtr checkEvent;

    if(widget == m_rootWidget)
        return;

    destroyedWidgets.push_back(widget);

    if(checkEvent && !checkEvent->isExecuted())
        return;

    checkEvent = g_eventDispatcher.scheduleEvent([] {
        g_lua.collectGarbage();
        UIWidgetList backupList = destroyedWidgets;
        destroyedWidgets.clear();
        g_eventDispatcher.scheduleEvent([backupList] {
            g_lua.collectGarbage();
            for(const UIWidgetPtr& widget : backupList) {
                if(widget->getUseCount() != 1)
                    logWarning("widget '", widget->getId(), "' destroyed but still have ", widget->getUseCount()-1, " reference(s) left");
            }
        }, 1);
    }, 1000);
#endif
}

bool UIManager::importStyle(const std::string& file)
{
    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(file);

        for(const OTMLNodePtr& styleNode : doc->children())
            importStyleFromOTML(styleNode);
        return true;
    } catch(Exception& e) {
        logError("Failed to import UI styles from '", file, "': ", e.what());
        return false;
    }
}

void UIManager::importStyleFromOTML(const OTMLNodePtr& styleNode)
{
    std::string tag = styleNode->tag();
    std::vector<std::string> split;
    boost::split(split, tag, boost::is_any_of(std::string("<")));
    if(split.size() != 2)
        throw OTMLException(styleNode, "not a valid style declaration");

    std::string name = split[0];
    std::string base = split[1];

    boost::trim(name);
    boost::trim(base);

    // TODO: styles must be searched by widget scopes, in that way this warning could be fixed
    // this warning is disabled because many ppl was complening about it
    /*
    auto it = m_styles.find(name);
    if(it != m_styles.end())
        logWarning("style '", name, "' is being redefined");
    */

    OTMLNodePtr originalStyle = getStyle(base);
    if(!originalStyle)
        Fw::throwException("base style '", base, "' is not defined");
    OTMLNodePtr style = originalStyle->clone();
    style->merge(styleNode);
    style->setTag(name);
    m_styles[name] = style;
}

OTMLNodePtr UIManager::getStyle(const std::string& styleName)
{
    auto it = m_styles.find(styleName);
    if(it != m_styles.end())
        return m_styles[styleName];

    // styles starting with UI are automatically defined
    if(boost::starts_with(styleName, "UI")) {
        OTMLNodePtr node = OTMLNode::create(styleName);
        node->writeAt("__class", styleName);
        m_styles[styleName] = node;
        return node;
    }

    return nullptr;
}

std::string UIManager::getStyleClass(const std::string& styleName)
{
    OTMLNodePtr style = getStyle(styleName);
    if(style && style->get("__class"))
        return style->valueAt("__class");
    return "";
}

UIWidgetPtr UIManager::loadUI(const std::string& file, const UIWidgetPtr& parent)
{
    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(file);
        UIWidgetPtr widget;
        for(const OTMLNodePtr& node : doc->children()) {
            std::string tag = node->tag();

            // import styles in these files too
            if(tag.find("<") != std::string::npos)
                importStyleFromOTML(node);
            else {
                if(widget)
                    Fw::throwException("cannot have multiple main widgets in otui files");
                widget = createWidgetFromOTML(node, parent);
            }
        }

        return widget;
    } catch(Exception& e) {
        logError("failed to load UI from '", file, "': ", e.what());
        return nullptr;
    }
}

UIWidgetPtr UIManager::createWidgetFromStyle(const std::string& styleName, const UIWidgetPtr& parent)
{
    OTMLNodePtr node = OTMLNode::create(styleName);
    try {
        return createWidgetFromOTML(node, parent);
    } catch(Exception& e) {
        logError("failed to create widget from style '", styleName, "': ", e.what());
        return nullptr;
    }
}

UIWidgetPtr UIManager::createWidgetFromOTML(const OTMLNodePtr& widgetNode, const UIWidgetPtr& parent)
{
    OTMLNodePtr originalStyleNode = getStyle(widgetNode->tag());
    if(!originalStyleNode)
        Fw::throwException("'", widgetNode->tag(), "' is not a defined style");

    OTMLNodePtr styleNode = originalStyleNode->clone();
    styleNode->merge(widgetNode);

    std::string widgetType = styleNode->valueAt("__class");

    // call widget creation from lua
    UIWidgetPtr widget = g_lua.callGlobalField<UIWidgetPtr>(widgetType, "create");
    if(parent)
        parent->addChild(widget);

    if(widget) {
        widget->setStyleFromNode(styleNode);

        for(const OTMLNodePtr& childNode : styleNode->children()) {
            if(!childNode->isUnique()) {
                createWidgetFromOTML(childNode, widget);
                styleNode->removeChild(childNode);
            }
        }
    } else
        Fw::throwException("unable to create widget of type '", widgetType, "'");

    return widget;
}


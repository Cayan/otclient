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

#include <framework/luascript/luainterface.h>
#include <framework/application.h>
#include <framework/graphics/fontmanager.h>
#include <framework/ui/ui.h>
#include <framework/net/protocol.h>
#include <framework/core/eventdispatcher.h>
#include <framework/core/configmanager.h>
#include <framework/otml/otml.h>
#include <framework/graphics/graphics.h>
#include <framework/platform/platformwindow.h>
#include <framework/core/modulemanager.h>
#include <framework/core/module.h>
#include <framework/sound/soundmanager.h>

void Application::registerLuaFunctions()
{
    // conversion globals
    g_lua.bindGlobalFunction("torect", [](const std::string& v) { return Fw::fromstring<Rect>(v); });
    g_lua.bindGlobalFunction("topoint", [](const std::string& v) { return Fw::fromstring<Point>(v); });
    g_lua.bindGlobalFunction("tocolor", [](const std::string& v) { return Fw::fromstring<Color>(v); });
    g_lua.bindGlobalFunction("tosize", [](const std::string& v) { return Fw::fromstring<Size>(v); });
    g_lua.bindGlobalFunction("recttostring", [](const Rect& v) { return Fw::tostring(v); });
    g_lua.bindGlobalFunction("pointtostring", [](const Point& v) { return Fw::tostring(v); });
    g_lua.bindGlobalFunction("colortostring", [](const Color& v) { return Fw::tostring(v); });
    g_lua.bindGlobalFunction("sizetostring", [](const Size& v) { return Fw::tostring(v); });

    // Event
    g_lua.registerClass<Event>();
    g_lua.bindClassMemberFunction<Event>("cancel", &Event::cancel);
    g_lua.bindClassMemberFunction<Event>("execute", &Event::execute);
    g_lua.bindClassMemberFunction<Event>("isCanceled", &Event::isCanceled);
    g_lua.bindClassMemberFunction<Event>("isExecuted", &Event::isExecuted);

    // ScheduledEvent
    g_lua.registerClass<ScheduledEvent, Event>();
    g_lua.bindClassMemberFunction<ScheduledEvent>("reamaningTicks", &ScheduledEvent::reamaningTicks);
    g_lua.bindClassMemberFunction<ScheduledEvent>("ticks", &ScheduledEvent::ticks);

    // UIWidget

    g_lua.registerClass<UIWidget>();    g_lua.bindClassStaticFunction<UIWidget>("create", []{ return UIWidgetPtr(new UIWidget); });
    g_lua.bindClassMemberFunction<UIWidget>("addChild", &UIWidget::addChild);
    g_lua.bindClassMemberFunction<UIWidget>("insertChild", &UIWidget::insertChild);
    g_lua.bindClassMemberFunction<UIWidget>("removeChild", &UIWidget::removeChild);
    g_lua.bindClassMemberFunction<UIWidget>("focusChild", &UIWidget::focusChild);
    g_lua.bindClassMemberFunction<UIWidget>("focusNextChild", &UIWidget::focusNextChild);
    g_lua.bindClassMemberFunction<UIWidget>("focusPreviousChild", &UIWidget::focusPreviousChild);
    g_lua.bindClassMemberFunction<UIWidget>("lowerChild", &UIWidget::lowerChild);
    g_lua.bindClassMemberFunction<UIWidget>("raiseChild", &UIWidget::raiseChild);
    g_lua.bindClassMemberFunction<UIWidget>("moveChildToIndex", &UIWidget::moveChildToIndex);
    g_lua.bindClassMemberFunction<UIWidget>("lockChild", &UIWidget::lockChild);
    g_lua.bindClassMemberFunction<UIWidget>("unlockChild", &UIWidget::unlockChild);
    g_lua.bindClassMemberFunction<UIWidget>("mergeStyle", &UIWidget::mergeStyle);
    g_lua.bindClassMemberFunction<UIWidget>("applyStyle", &UIWidget::applyStyle);
    g_lua.bindClassMemberFunction<UIWidget>("addAnchor", &UIWidget::addAnchor);
    g_lua.bindClassMemberFunction<UIWidget>("removeAnchor", &UIWidget::removeAnchor);
    g_lua.bindClassMemberFunction<UIWidget>("fill", &UIWidget::fill);
    g_lua.bindClassMemberFunction<UIWidget>("centerIn", &UIWidget::centerIn);
    g_lua.bindClassMemberFunction<UIWidget>("breakAnchors", &UIWidget::breakAnchors);
    g_lua.bindClassMemberFunction<UIWidget>("updateParentLayout", &UIWidget::updateParentLayout);
    g_lua.bindClassMemberFunction<UIWidget>("updateLayout", &UIWidget::updateLayout);
    g_lua.bindClassMemberFunction<UIWidget>("lock", &UIWidget::lock);
    g_lua.bindClassMemberFunction<UIWidget>("unlock", &UIWidget::unlock);
    g_lua.bindClassMemberFunction<UIWidget>("focus", &UIWidget::focus);
    g_lua.bindClassMemberFunction<UIWidget>("lower", &UIWidget::lower);
    g_lua.bindClassMemberFunction<UIWidget>("raise", &UIWidget::raise);
    g_lua.bindClassMemberFunction<UIWidget>("grabMouse", &UIWidget::grabMouse);
    g_lua.bindClassMemberFunction<UIWidget>("ungrabMouse", &UIWidget::ungrabMouse);
    g_lua.bindClassMemberFunction<UIWidget>("grabKeyboard", &UIWidget::grabKeyboard);
    g_lua.bindClassMemberFunction<UIWidget>("ungrabKeyboard", &UIWidget::ungrabKeyboard);
    g_lua.bindClassMemberFunction<UIWidget>("bindRectToParent", &UIWidget::bindRectToParent);
    g_lua.bindClassMemberFunction<UIWidget>("destroy", &UIWidget::destroy);
    g_lua.bindClassMemberFunction<UIWidget>("destroyChildren", &UIWidget::destroyChildren);
    g_lua.bindClassMemberFunction<UIWidget>("setId", &UIWidget::setId);
    g_lua.bindClassMemberFunction<UIWidget>("setParent", &UIWidget::setParent);
    g_lua.bindClassMemberFunction<UIWidget>("setLayout", &UIWidget::setLayout);
    g_lua.bindClassMemberFunction<UIWidget>("setRect", &UIWidget::setRect);
    g_lua.bindClassMemberFunction<UIWidget>("setStyle", &UIWidget::setStyle);
    g_lua.bindClassMemberFunction<UIWidget>("setStyleFromNode", &UIWidget::setStyleFromNode);
    g_lua.bindClassMemberFunction<UIWidget>("setEnabled", &UIWidget::setEnabled);
    g_lua.bindClassMemberFunction<UIWidget>("setVisible", &UIWidget::setVisible);
    g_lua.bindClassMemberFunction<UIWidget>("setOn", &UIWidget::setOn);
    g_lua.bindClassMemberFunction<UIWidget>("setChecked", &UIWidget::setChecked);
    g_lua.bindClassMemberFunction<UIWidget>("setFocusable", &UIWidget::setFocusable);
    g_lua.bindClassMemberFunction<UIWidget>("setPhantom", &UIWidget::setPhantom);
    g_lua.bindClassMemberFunction<UIWidget>("setDragable", &UIWidget::setDragable);
    g_lua.bindClassMemberFunction<UIWidget>("setFixedSize", &UIWidget::setFixedSize);
    g_lua.bindClassMemberFunction<UIWidget>("setClipping", &UIWidget::setClipping);
    g_lua.bindClassMemberFunction<UIWidget>("setLastFocusReason", &UIWidget::setLastFocusReason);
    g_lua.bindClassMemberFunction<UIWidget>("setAutoRepeatDelay", &UIWidget::setAutoRepeatDelay);
    g_lua.bindClassMemberFunction<UIWidget>("setVirtualOffset", &UIWidget::setVirtualOffset);
    g_lua.bindClassMemberFunction<UIWidget>("isVisible", &UIWidget::isVisible);
    g_lua.bindClassMemberFunction<UIWidget>("isChildLocked", &UIWidget::isChildLocked);
    g_lua.bindClassMemberFunction<UIWidget>("hasChild", &UIWidget::hasChild);
    g_lua.bindClassMemberFunction<UIWidget>("getChildIndex", &UIWidget::getChildIndex);
    g_lua.bindClassMemberFunction<UIWidget>("getMarginRect", &UIWidget::getMarginRect);
    g_lua.bindClassMemberFunction<UIWidget>("getClippingRect", &UIWidget::getClippingRect);
    g_lua.bindClassMemberFunction<UIWidget>("getChildrenRect", &UIWidget::getChildrenRect);
    g_lua.bindClassMemberFunction<UIWidget>("getAnchoredLayout", &UIWidget::getAnchoredLayout);
    g_lua.bindClassMemberFunction<UIWidget>("getRootParent", &UIWidget::getRootParent);
    g_lua.bindClassMemberFunction<UIWidget>("getChildAfter", &UIWidget::getChildAfter);
    g_lua.bindClassMemberFunction<UIWidget>("getChildBefore", &UIWidget::getChildBefore);
    g_lua.bindClassMemberFunction<UIWidget>("getChildById", &UIWidget::getChildById);
    g_lua.bindClassMemberFunction<UIWidget>("getChildByPos", &UIWidget::getChildByPos);
    g_lua.bindClassMemberFunction<UIWidget>("getChildByIndex", &UIWidget::getChildByIndex);
    g_lua.bindClassMemberFunction<UIWidget>("recursiveGetChildById", &UIWidget::recursiveGetChildById);
    g_lua.bindClassMemberFunction<UIWidget>("recursiveGetChildByPos", &UIWidget::recursiveGetChildByPos);
    g_lua.bindClassMemberFunction<UIWidget>("recursiveGetChildrenByPos", &UIWidget::recursiveGetChildrenByPos);
    g_lua.bindClassMemberFunction<UIWidget>("backwardsGetWidgetById", &UIWidget::backwardsGetWidgetById);
    g_lua.bindClassMemberFunction<UIWidget>("asUIWidget", &UIWidget::asUIWidget);
    g_lua.bindClassMemberFunction<UIWidget>("resize", &UIWidget::resize);
    g_lua.bindClassMemberFunction<UIWidget>("move", &UIWidget::move);
    g_lua.bindClassMemberFunction<UIWidget>("hide", &UIWidget::hide);
    g_lua.bindClassMemberFunction<UIWidget>("show", &UIWidget::show);
    g_lua.bindClassMemberFunction<UIWidget>("disable", &UIWidget::disable);
    g_lua.bindClassMemberFunction<UIWidget>("enable", &UIWidget::enable);
    g_lua.bindClassMemberFunction<UIWidget>("isActive", &UIWidget::isActive);
    g_lua.bindClassMemberFunction<UIWidget>("isEnabled", &UIWidget::isEnabled);
    g_lua.bindClassMemberFunction<UIWidget>("isDisabled", &UIWidget::isDisabled);
    g_lua.bindClassMemberFunction<UIWidget>("isFocused", &UIWidget::isFocused);
    g_lua.bindClassMemberFunction<UIWidget>("isHovered", &UIWidget::isHovered);
    g_lua.bindClassMemberFunction<UIWidget>("isPressed", &UIWidget::isPressed);
    g_lua.bindClassMemberFunction<UIWidget>("isFirst", &UIWidget::isFirst);
    g_lua.bindClassMemberFunction<UIWidget>("isMiddle", &UIWidget::isMiddle);
    g_lua.bindClassMemberFunction<UIWidget>("isLast", &UIWidget::isLast);
    g_lua.bindClassMemberFunction<UIWidget>("isAlternate", &UIWidget::isAlternate);
    g_lua.bindClassMemberFunction<UIWidget>("isChecked", &UIWidget::isChecked);
    g_lua.bindClassMemberFunction<UIWidget>("isOn", &UIWidget::isOn);
    g_lua.bindClassMemberFunction<UIWidget>("isDragging", &UIWidget::isDragging);
    g_lua.bindClassMemberFunction<UIWidget>("isHidden", &UIWidget::isHidden);
    g_lua.bindClassMemberFunction<UIWidget>("isExplicitlyEnabled", &UIWidget::isExplicitlyEnabled);
    g_lua.bindClassMemberFunction<UIWidget>("isExplicitlyVisible", &UIWidget::isExplicitlyVisible);
    g_lua.bindClassMemberFunction<UIWidget>("isFocusable", &UIWidget::isFocusable);
    g_lua.bindClassMemberFunction<UIWidget>("isPhantom", &UIWidget::isPhantom);
    g_lua.bindClassMemberFunction<UIWidget>("isDragable", &UIWidget::isDragable);
    g_lua.bindClassMemberFunction<UIWidget>("isFixedSize", &UIWidget::isFixedSize);
    g_lua.bindClassMemberFunction<UIWidget>("isClipping", &UIWidget::isClipping);
    g_lua.bindClassMemberFunction<UIWidget>("isDestroyed", &UIWidget::isDestroyed);
    g_lua.bindClassMemberFunction<UIWidget>("hasChildren", &UIWidget::hasChildren);
    g_lua.bindClassMemberFunction<UIWidget>("containsPoint", &UIWidget::containsPoint);
    g_lua.bindClassMemberFunction<UIWidget>("getId", &UIWidget::getId);
    g_lua.bindClassMemberFunction<UIWidget>("getParent", &UIWidget::getParent);
    g_lua.bindClassMemberFunction<UIWidget>("getFocusedChild", &UIWidget::getFocusedChild);
    g_lua.bindClassMemberFunction<UIWidget>("getChildren", &UIWidget::getChildren);
    g_lua.bindClassMemberFunction<UIWidget>("getFirstChild", &UIWidget::getFirstChild);
    g_lua.bindClassMemberFunction<UIWidget>("getLastChild", &UIWidget::getLastChild);
    g_lua.bindClassMemberFunction<UIWidget>("getLayout", &UIWidget::getLayout);
    g_lua.bindClassMemberFunction<UIWidget>("getStyle", &UIWidget::getStyle);
    g_lua.bindClassMemberFunction<UIWidget>("getChildCount", &UIWidget::getChildCount);
    g_lua.bindClassMemberFunction<UIWidget>("getLastFocusReason", &UIWidget::getLastFocusReason);
    g_lua.bindClassMemberFunction<UIWidget>("getAutoRepeatDelay", &UIWidget::getAutoRepeatDelay);
    g_lua.bindClassMemberFunction<UIWidget>("getVirtualOffset", &UIWidget::getVirtualOffset);
    g_lua.bindClassMemberFunction<UIWidget>("getStyleName", &UIWidget::getStyleName);
    g_lua.bindClassMemberFunction<UIWidget>("getLastClickPosition", &UIWidget::getLastClickPosition);
    g_lua.bindClassMemberFunction<UIWidget>("setX", &UIWidget::setX);
    g_lua.bindClassMemberFunction<UIWidget>("setY", &UIWidget::setY);
    g_lua.bindClassMemberFunction<UIWidget>("setWidth", &UIWidget::setWidth);
    g_lua.bindClassMemberFunction<UIWidget>("setHeight", &UIWidget::setHeight);
    g_lua.bindClassMemberFunction<UIWidget>("setSize", &UIWidget::setSize);
    g_lua.bindClassMemberFunction<UIWidget>("setPosition", &UIWidget::setPosition);
    g_lua.bindClassMemberFunction<UIWidget>("setColor", &UIWidget::setColor);
    g_lua.bindClassMemberFunction<UIWidget>("setBackgroundColor", &UIWidget::setBackgroundColor);
    g_lua.bindClassMemberFunction<UIWidget>("setBackgroundOffsetX", &UIWidget::setBackgroundOffsetX);
    g_lua.bindClassMemberFunction<UIWidget>("setBackgroundOffsetY", &UIWidget::setBackgroundOffsetY);
    g_lua.bindClassMemberFunction<UIWidget>("setBackgroundOffset", &UIWidget::setBackgroundOffset);
    g_lua.bindClassMemberFunction<UIWidget>("setBackgroundWidth", &UIWidget::setBackgroundWidth);
    g_lua.bindClassMemberFunction<UIWidget>("setBackgroundHeight", &UIWidget::setBackgroundHeight);
    g_lua.bindClassMemberFunction<UIWidget>("setBackgroundSize", &UIWidget::setBackgroundSize);
    g_lua.bindClassMemberFunction<UIWidget>("setBackgroundRect", &UIWidget::setBackgroundRect);
    g_lua.bindClassMemberFunction<UIWidget>("setIcon", &UIWidget::setIcon);
    g_lua.bindClassMemberFunction<UIWidget>("setIconColor", &UIWidget::setIconColor);
    g_lua.bindClassMemberFunction<UIWidget>("setIconOffsetX", &UIWidget::setIconOffsetX);
    g_lua.bindClassMemberFunction<UIWidget>("setIconOffsetY", &UIWidget::setIconOffsetY);
    g_lua.bindClassMemberFunction<UIWidget>("setIconOffset", &UIWidget::setIconOffset);
    g_lua.bindClassMemberFunction<UIWidget>("setIconWidth", &UIWidget::setIconWidth);
    g_lua.bindClassMemberFunction<UIWidget>("setIconHeight", &UIWidget::setIconHeight);
    g_lua.bindClassMemberFunction<UIWidget>("setIconSize", &UIWidget::setIconSize);
    g_lua.bindClassMemberFunction<UIWidget>("setIconRect", &UIWidget::setIconRect);
    g_lua.bindClassMemberFunction<UIWidget>("setBorderWidth", &UIWidget::setBorderWidth);
    g_lua.bindClassMemberFunction<UIWidget>("setBorderWidthTop", &UIWidget::setBorderWidthTop);
    g_lua.bindClassMemberFunction<UIWidget>("setBorderWidthRight", &UIWidget::setBorderWidthRight);
    g_lua.bindClassMemberFunction<UIWidget>("setBorderWidthBottom", &UIWidget::setBorderWidthBottom);
    g_lua.bindClassMemberFunction<UIWidget>("setBorderWidthLeft", &UIWidget::setBorderWidthLeft);
    g_lua.bindClassMemberFunction<UIWidget>("setBorderColor", &UIWidget::setBorderColor);
    g_lua.bindClassMemberFunction<UIWidget>("setBorderColorTop", &UIWidget::setBorderColorTop);
    g_lua.bindClassMemberFunction<UIWidget>("setBorderColorRight", &UIWidget::setBorderColorRight);
    g_lua.bindClassMemberFunction<UIWidget>("setBorderColorBottom", &UIWidget::setBorderColorBottom);
    g_lua.bindClassMemberFunction<UIWidget>("setBorderColorLeft", &UIWidget::setBorderColorLeft);
    g_lua.bindClassMemberFunction<UIWidget>("setMargin", &UIWidget::setMargin);
    g_lua.bindClassMemberFunction<UIWidget>("setMarginHorizontal", &UIWidget::setMarginHorizontal);
    g_lua.bindClassMemberFunction<UIWidget>("setMarginVertical", &UIWidget::setMarginVertical);
    g_lua.bindClassMemberFunction<UIWidget>("setMarginTop", &UIWidget::setMarginTop);
    g_lua.bindClassMemberFunction<UIWidget>("setMarginRight", &UIWidget::setMarginRight);
    g_lua.bindClassMemberFunction<UIWidget>("setMarginBottom", &UIWidget::setMarginBottom);
    g_lua.bindClassMemberFunction<UIWidget>("setMarginLeft", &UIWidget::setMarginLeft);
    g_lua.bindClassMemberFunction<UIWidget>("setPadding", &UIWidget::setPadding);
    g_lua.bindClassMemberFunction<UIWidget>("setPaddingHorizontal", &UIWidget::setPaddingHorizontal);
    g_lua.bindClassMemberFunction<UIWidget>("setPaddingVertical", &UIWidget::setPaddingVertical);
    g_lua.bindClassMemberFunction<UIWidget>("setPaddingTop", &UIWidget::setPaddingTop);
    g_lua.bindClassMemberFunction<UIWidget>("setPaddingRight", &UIWidget::setPaddingRight);
    g_lua.bindClassMemberFunction<UIWidget>("setPaddingBottom", &UIWidget::setPaddingBottom);
    g_lua.bindClassMemberFunction<UIWidget>("setPaddingLeft", &UIWidget::setPaddingLeft);
    g_lua.bindClassMemberFunction<UIWidget>("setOpacity", &UIWidget::setOpacity);
    g_lua.bindClassMemberFunction<UIWidget>("getX", &UIWidget::getX);
    g_lua.bindClassMemberFunction<UIWidget>("getY", &UIWidget::getY);
    g_lua.bindClassMemberFunction<UIWidget>("getPosition", &UIWidget::getPosition);
    g_lua.bindClassMemberFunction<UIWidget>("getWidth", &UIWidget::getWidth);
    g_lua.bindClassMemberFunction<UIWidget>("getHeight", &UIWidget::getHeight);
    g_lua.bindClassMemberFunction<UIWidget>("getSize", &UIWidget::getSize);
    g_lua.bindClassMemberFunction<UIWidget>("getRect", &UIWidget::getRect);
    g_lua.bindClassMemberFunction<UIWidget>("getColor", &UIWidget::getColor);
    g_lua.bindClassMemberFunction<UIWidget>("getBackgroundColor", &UIWidget::getBackgroundColor);
    g_lua.bindClassMemberFunction<UIWidget>("getBackgroundOffsetX", &UIWidget::getBackgroundOffsetX);
    g_lua.bindClassMemberFunction<UIWidget>("getBackgroundOffsetY", &UIWidget::getBackgroundOffsetY);
    g_lua.bindClassMemberFunction<UIWidget>("getBackgroundOffset", &UIWidget::getBackgroundOffset);
    g_lua.bindClassMemberFunction<UIWidget>("getBackgroundWidth", &UIWidget::getBackgroundWidth);
    g_lua.bindClassMemberFunction<UIWidget>("getBackgroundHeight", &UIWidget::getBackgroundHeight);
    g_lua.bindClassMemberFunction<UIWidget>("getBackgroundSize", &UIWidget::getBackgroundSize);
    g_lua.bindClassMemberFunction<UIWidget>("getBackgroundRect", &UIWidget::getBackgroundRect);
    g_lua.bindClassMemberFunction<UIWidget>("getIconColor", &UIWidget::getIconColor);
    g_lua.bindClassMemberFunction<UIWidget>("getIconOffsetX", &UIWidget::getIconOffsetX);
    g_lua.bindClassMemberFunction<UIWidget>("getIconOffsetY", &UIWidget::getIconOffsetY);
    g_lua.bindClassMemberFunction<UIWidget>("getIconOffset", &UIWidget::getIconOffset);
    g_lua.bindClassMemberFunction<UIWidget>("getIconWidth", &UIWidget::getIconWidth);
    g_lua.bindClassMemberFunction<UIWidget>("getIconHeight", &UIWidget::getIconHeight);
    g_lua.bindClassMemberFunction<UIWidget>("getIconSize", &UIWidget::getIconSize);
    g_lua.bindClassMemberFunction<UIWidget>("getIconRect", &UIWidget::getIconRect);
    g_lua.bindClassMemberFunction<UIWidget>("getBorderTopColor", &UIWidget::getBorderTopColor);
    g_lua.bindClassMemberFunction<UIWidget>("getBorderRightColor", &UIWidget::getBorderRightColor);
    g_lua.bindClassMemberFunction<UIWidget>("getBorderBottomColor", &UIWidget::getBorderBottomColor);
    g_lua.bindClassMemberFunction<UIWidget>("getBorderLeftColor", &UIWidget::getBorderLeftColor);
    g_lua.bindClassMemberFunction<UIWidget>("getBorderTopWidth", &UIWidget::getBorderTopWidth);
    g_lua.bindClassMemberFunction<UIWidget>("getBorderRightWidth", &UIWidget::getBorderRightWidth);
    g_lua.bindClassMemberFunction<UIWidget>("getBorderBottomWidth", &UIWidget::getBorderBottomWidth);
    g_lua.bindClassMemberFunction<UIWidget>("getBorderLeftWidth", &UIWidget::getBorderLeftWidth);
    g_lua.bindClassMemberFunction<UIWidget>("getMarginTop", &UIWidget::getMarginTop);
    g_lua.bindClassMemberFunction<UIWidget>("getMarginRight", &UIWidget::getMarginRight);
    g_lua.bindClassMemberFunction<UIWidget>("getMarginBottom", &UIWidget::getMarginBottom);
    g_lua.bindClassMemberFunction<UIWidget>("getMarginLeft", &UIWidget::getMarginLeft);
    g_lua.bindClassMemberFunction<UIWidget>("getPaddingTop", &UIWidget::getPaddingTop);
    g_lua.bindClassMemberFunction<UIWidget>("getPaddingRight", &UIWidget::getPaddingRight);
    g_lua.bindClassMemberFunction<UIWidget>("getPaddingBottom", &UIWidget::getPaddingBottom);
    g_lua.bindClassMemberFunction<UIWidget>("getPaddingLeft", &UIWidget::getPaddingLeft);
    g_lua.bindClassMemberFunction<UIWidget>("getOpacity", &UIWidget::getOpacity);
    g_lua.bindClassMemberFunction<UIWidget>("setImageSource", &UIWidget::setImageSource);
    g_lua.bindClassMemberFunction<UIWidget>("setImageClip", &UIWidget::setImageClip);
    g_lua.bindClassMemberFunction<UIWidget>("setImageOffsetX", &UIWidget::setImageOffsetX);
    g_lua.bindClassMemberFunction<UIWidget>("setImageOffsetY", &UIWidget::setImageOffsetY);
    g_lua.bindClassMemberFunction<UIWidget>("setImageOffset", &UIWidget::setImageOffset);
    g_lua.bindClassMemberFunction<UIWidget>("setImageWidth", &UIWidget::setImageWidth);
    g_lua.bindClassMemberFunction<UIWidget>("setImageHeight", &UIWidget::setImageHeight);
    g_lua.bindClassMemberFunction<UIWidget>("setImageSize", &UIWidget::setImageSize);
    g_lua.bindClassMemberFunction<UIWidget>("setImageRect", &UIWidget::setImageRect);
    g_lua.bindClassMemberFunction<UIWidget>("setImageColor", &UIWidget::setImageColor);
    g_lua.bindClassMemberFunction<UIWidget>("setImageFixedRatio", &UIWidget::setImageFixedRatio);
    g_lua.bindClassMemberFunction<UIWidget>("setImageRepeated", &UIWidget::setImageRepeated);
    g_lua.bindClassMemberFunction<UIWidget>("setImageSmooth", &UIWidget::setImageSmooth);
    g_lua.bindClassMemberFunction<UIWidget>("setImageBorderTop", &UIWidget::setImageBorderTop);
    g_lua.bindClassMemberFunction<UIWidget>("setImageBorderRight", &UIWidget::setImageBorderRight);
    g_lua.bindClassMemberFunction<UIWidget>("setImageBorderBottom", &UIWidget::setImageBorderBottom);
    g_lua.bindClassMemberFunction<UIWidget>("setImageBorderLeft", &UIWidget::setImageBorderLeft);
    g_lua.bindClassMemberFunction<UIWidget>("setImageBorder", &UIWidget::setImageBorder);
    g_lua.bindClassMemberFunction<UIWidget>("getImageClip", &UIWidget::getImageClip);
    g_lua.bindClassMemberFunction<UIWidget>("getImageOffsetX", &UIWidget::getImageOffsetX);
    g_lua.bindClassMemberFunction<UIWidget>("getImageOffsetY", &UIWidget::getImageOffsetY);
    g_lua.bindClassMemberFunction<UIWidget>("getImageOffset", &UIWidget::getImageOffset);
    g_lua.bindClassMemberFunction<UIWidget>("getImageWidth", &UIWidget::getImageWidth);
    g_lua.bindClassMemberFunction<UIWidget>("getImageHeight", &UIWidget::getImageHeight);
    g_lua.bindClassMemberFunction<UIWidget>("getImageSize", &UIWidget::getImageSize);
    g_lua.bindClassMemberFunction<UIWidget>("getImageRect", &UIWidget::getImageRect);
    g_lua.bindClassMemberFunction<UIWidget>("getImageColor", &UIWidget::getImageColor);
    g_lua.bindClassMemberFunction<UIWidget>("isImageFixedRatio", &UIWidget::isImageFixedRatio);
    g_lua.bindClassMemberFunction<UIWidget>("isImageSmooth", &UIWidget::isImageSmooth);
    g_lua.bindClassMemberFunction<UIWidget>("getImageBorderTop", &UIWidget::getImageBorderTop);
    g_lua.bindClassMemberFunction<UIWidget>("getImageBorderRight", &UIWidget::getImageBorderRight);
    g_lua.bindClassMemberFunction<UIWidget>("getImageBorderBottom", &UIWidget::getImageBorderBottom);
    g_lua.bindClassMemberFunction<UIWidget>("getImageBorderLeft", &UIWidget::getImageBorderLeft);
    g_lua.bindClassMemberFunction<UIWidget>("resizeToText", &UIWidget::resizeToText);
    g_lua.bindClassMemberFunction<UIWidget>("clearText", &UIWidget::clearText);
    g_lua.bindClassMemberFunction<UIWidget>("setText", &UIWidget::setText);
    g_lua.bindClassMemberFunction<UIWidget>("setTextAlign", &UIWidget::setTextAlign);
    g_lua.bindClassMemberFunction<UIWidget>("setTextOffset", &UIWidget::setTextOffset);
    g_lua.bindClassMemberFunction<UIWidget>("setTextWrap", &UIWidget::setTextWrap);
    g_lua.bindClassMemberFunction<UIWidget>("setTextAutoResize", &UIWidget::setTextAutoResize);
    g_lua.bindClassMemberFunction<UIWidget>("setFont", &UIWidget::setFont);
    g_lua.bindClassMemberFunction<UIWidget>("getText", &UIWidget::getText);
    g_lua.bindClassMemberFunction<UIWidget>("getDrawText", &UIWidget::getDrawText);
    g_lua.bindClassMemberFunction<UIWidget>("getTextAlign", &UIWidget::getTextAlign);
    g_lua.bindClassMemberFunction<UIWidget>("getTextOffset", &UIWidget::getTextOffset);
    g_lua.bindClassMemberFunction<UIWidget>("getTextWrap", &UIWidget::getTextWrap);
    g_lua.bindClassMemberFunction<UIWidget>("getFont", &UIWidget::getFont);
    g_lua.bindClassMemberFunction<UIWidget>("getTextSize", &UIWidget::getTextSize);

    // UILayout
    g_lua.registerClass<UILayout>();
    g_lua.bindClassMemberFunction<UILayout>("update", &UILayout::update);
    g_lua.bindClassMemberFunction<UILayout>("updateLater", &UILayout::updateLater);
    g_lua.bindClassMemberFunction<UILayout>("applyStyle", &UILayout::applyStyle);
    g_lua.bindClassMemberFunction<UILayout>("addWidget", &UILayout::addWidget);
    g_lua.bindClassMemberFunction<UILayout>("removeWidget", &UILayout::removeWidget);
    g_lua.bindClassMemberFunction<UILayout>("disableUpdates", &UILayout::disableUpdates);
    g_lua.bindClassMemberFunction<UILayout>("enableUpdates", &UILayout::enableUpdates);
    g_lua.bindClassMemberFunction<UILayout>("setParent", &UILayout::setParent);
    g_lua.bindClassMemberFunction<UILayout>("getParentWidget", &UILayout::getParentWidget);
    g_lua.bindClassMemberFunction<UILayout>("isUpdateDisabled", &UILayout::isUpdateDisabled);
    g_lua.bindClassMemberFunction<UILayout>("isUpdating", &UILayout::isUpdating);
    g_lua.bindClassMemberFunction<UILayout>("asUILayout", &UILayout::asUILayout);
    g_lua.bindClassMemberFunction<UILayout>("asUIAnchorLayout", &UILayout::asUIAnchorLayout);
    g_lua.bindClassMemberFunction<UILayout>("asUIBoxLayout", &UILayout::asUIBoxLayout);
    g_lua.bindClassMemberFunction<UILayout>("asUIHorizontalLayout", &UILayout::asUIHorizontalLayout);
    g_lua.bindClassMemberFunction<UILayout>("asUIVerticalLayout", &UILayout::asUIVerticalLayout);
    g_lua.bindClassMemberFunction<UILayout>("asUIGridLayout", &UILayout::asUIGridLayout);

    // UIBoxLayout
    g_lua.registerClass<UIBoxLayout>();
    g_lua.bindClassMemberFunction<UIBoxLayout>("setSpacing", &UIBoxLayout::setSpacing);
    g_lua.bindClassMemberFunction<UIBoxLayout>("setFitChildren", &UIBoxLayout::setFitChildren);

    // UIVerticalLayout
    g_lua.registerClass<UIVerticalLayout, UIBoxLayout>();
    g_lua.bindClassStaticFunction<UIVerticalLayout>("create", [](UIWidgetPtr parent){ return UIVerticalLayoutPtr(new UIVerticalLayout(parent)); } );
    g_lua.bindClassMemberFunction<UIVerticalLayout>("setAlignBottom", &UIVerticalLayout::setAlignBottom);

    // UIHorizontalLayout
    g_lua.registerClass<UIHorizontalLayout, UIBoxLayout>();
    g_lua.bindClassStaticFunction<UIHorizontalLayout>("create", [](UIWidgetPtr parent){ return UIHorizontalLayoutPtr(new UIHorizontalLayout(parent)); } );
    g_lua.bindClassMemberFunction<UIHorizontalLayout>("setAlignRight", &UIHorizontalLayout::setAlignRight);

    // UIGridLayout
    g_lua.registerClass<UIGridLayout, UILayout>();
    g_lua.bindClassStaticFunction<UIGridLayout>("create", [](UIWidgetPtr parent){ return UIGridLayoutPtr(new UIGridLayout(parent)); });
    g_lua.bindClassMemberFunction<UIGridLayout>("setCellSize", &UIGridLayout::setCellSize);
    g_lua.bindClassMemberFunction<UIGridLayout>("setCellWidth", &UIGridLayout::setCellWidth);
    g_lua.bindClassMemberFunction<UIGridLayout>("setCellHeight", &UIGridLayout::setCellHeight);
    g_lua.bindClassMemberFunction<UIGridLayout>("setCellSpacing", &UIGridLayout::setCellSpacing);
    g_lua.bindClassMemberFunction<UIGridLayout>("setNumColumns", &UIGridLayout::setNumColumns);
    g_lua.bindClassMemberFunction<UIGridLayout>("setNumLines", &UIGridLayout::setNumLines);
    g_lua.bindClassMemberFunction<UIGridLayout>("asUIGridLayout", &UIGridLayout::asUIGridLayout);

    // UIAnchorLayout
    g_lua.registerClass<UIAnchorLayout, UILayout>();
    g_lua.bindClassStaticFunction<UIAnchorLayout>("create", [](UIWidgetPtr parent){ return UIAnchorLayoutPtr(new UIAnchorLayout(parent)); } );
    g_lua.bindClassMemberFunction<UIAnchorLayout>("removeAnchors", &UIAnchorLayout::removeAnchors);
    g_lua.bindClassMemberFunction<UIAnchorLayout>("centerIn", &UIAnchorLayout::centerIn);
    g_lua.bindClassMemberFunction<UIAnchorLayout>("fill", &UIAnchorLayout::fill);

    // UITextEdit
    g_lua.registerClass<UITextEdit, UIWidget>();
    g_lua.bindClassStaticFunction<UITextEdit>("create", []{ return UITextEditPtr(new UITextEdit); } );
    g_lua.bindClassMemberFunction<UITextEdit>("setTextHorizontalMargin", &UITextEdit::setTextHorizontalMargin);
    g_lua.bindClassMemberFunction<UITextEdit>("setCursorPos", &UITextEdit::setCursorPos);
    g_lua.bindClassMemberFunction<UITextEdit>("setCursorEnabled", &UITextEdit::setCursorEnabled);
    g_lua.bindClassMemberFunction<UITextEdit>("setTextHidden", &UITextEdit::setTextHidden);
    g_lua.bindClassMemberFunction<UITextEdit>("setAlwaysActive", &UITextEdit::setAlwaysActive);
    g_lua.bindClassMemberFunction<UITextEdit>("setValidCharacters", &UITextEdit::setValidCharacters);
    g_lua.bindClassMemberFunction<UITextEdit>("setShiftNavigation", &UITextEdit::setShiftNavigation);
    g_lua.bindClassMemberFunction<UITextEdit>("setMultiline", &UITextEdit::setMultiline);
    g_lua.bindClassMemberFunction<UITextEdit>("moveCursor", &UITextEdit::moveCursor);
    g_lua.bindClassMemberFunction<UITextEdit>("appendText", &UITextEdit::appendText);
    g_lua.bindClassMemberFunction<UITextEdit>("removeCharacter", &UITextEdit::removeCharacter);
    g_lua.bindClassMemberFunction<UITextEdit>("getDisplayedText", &UITextEdit::getDisplayedText);
    g_lua.bindClassMemberFunction<UITextEdit>("getTextPos", &UITextEdit::getTextPos);
    g_lua.bindClassMemberFunction<UITextEdit>("getTextHorizontalMargin", &UITextEdit::getTextHorizontalMargin);
    g_lua.bindClassMemberFunction<UITextEdit>("getCursorPos", &UITextEdit::getCursorPos);
    g_lua.bindClassMemberFunction<UITextEdit>("isCursorEnabled", &UITextEdit::isCursorEnabled);
    g_lua.bindClassMemberFunction<UITextEdit>("isAlwaysActive", &UITextEdit::isAlwaysActive);
    g_lua.bindClassMemberFunction<UITextEdit>("isTextHidden", &UITextEdit::isTextHidden);
    g_lua.bindClassMemberFunction<UITextEdit>("isShiftNavigation", &UITextEdit::isShiftNavigation);
    g_lua.bindClassMemberFunction<UITextEdit>("isMultiline", &UITextEdit::isMultiline);

    // UIFrameCounter
    g_lua.registerClass<UIFrameCounter, UIWidget>();
    g_lua.bindClassStaticFunction<UIFrameCounter>("create", []{ return UIFrameCounterPtr(new UIFrameCounter); } );
    g_lua.bindClassMemberFunction<UIFrameCounter>("getFrameCount", &UIFrameCounter::getFrameCount);

    // Protocol
    g_lua.registerClass<Protocol>();

    // Module
    g_lua.registerClass<Module>();
    g_lua.bindClassMemberFunction<Module>("load", &Module::load);
    g_lua.bindClassMemberFunction<Module>("unload", &Module::unload);
    g_lua.bindClassMemberFunction<Module>("reload", &Module::reload);
    g_lua.bindClassMemberFunction<Module>("canReload", &Module::canReload);
    g_lua.bindClassMemberFunction<Module>("canUnload", &Module::canUnload);
    g_lua.bindClassMemberFunction<Module>("isLoaded", &Module::isLoaded);
    g_lua.bindClassMemberFunction<Module>("isReloadble", &Module::isReloadable);
    g_lua.bindClassMemberFunction<Module>("getDescription", &Module::getDescription);
    g_lua.bindClassMemberFunction<Module>("getName", &Module::getName);
    g_lua.bindClassMemberFunction<Module>("getAuthor", &Module::getAuthor);
    g_lua.bindClassMemberFunction<Module>("getWebsite", &Module::getWebsite);
    g_lua.bindClassMemberFunction<Module>("getVersion", &Module::getVersion);
    g_lua.bindClassMemberFunction<Module>("isAutoLoad", &Module::isAutoLoad);
    g_lua.bindClassMemberFunction<Module>("getAutoLoadPriority", &Module::getAutoLoadPriority);

    // network manipulation via lua is disabled for a while
    /*
    // OutputMessage
    g_lua.registerClass<OutputMessage>();
    g_lua.bindClassStaticFunction<OutputMessage>("new", []{ return OutputMessagePtr(new OutputMessage); });
    g_lua.bindClassMemberFunction<OutputMessage>("reset", &OutputMessage::reset);
    g_lua.bindClassMemberFunction<OutputMessage>("addU8", &OutputMessage::addU8);
    g_lua.bindClassMemberFunction<OutputMessage>("addU16", &OutputMessage::addU16);
    g_lua.bindClassMemberFunction<OutputMessage>("addU32", &OutputMessage::addU32);
    g_lua.bindClassMemberFunction<OutputMessage>("addU64", &OutputMessage::addU64);
    g_lua.bindClassMemberFunction<OutputMessage>("addString", (void(OutputMessage::*)(const std::string&))&OutputMessage::addString);

    g_lua.bindClassStaticFunction<Protocol>("send", [](const ProtocolPtr proto, OutputMessagePtr msg) { proto->send(*msg.get()); });
    */

    // Application
    g_lua.registerStaticClass("g_app");
    g_lua.bindClassStaticFunction("g_app", "exit", std::bind(&Application::exit, g_app));
    g_lua.bindClassStaticFunction("g_app", "setFrameSleep", std::bind(&Application::setFrameSleep, g_app, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_app", "isRunning", std::bind(&Application::isRunning, g_app));
    g_lua.bindClassStaticFunction("g_app", "isStopping", std::bind(&Application::isStopping, g_app));
    g_lua.bindClassStaticFunction("g_app", "getFrameSleep", std::bind(&Application::getFrameSleep, g_app));
    g_lua.bindClassStaticFunction("g_app", "getName", std::bind(&Application::getName, g_app));
    g_lua.bindClassStaticFunction("g_app", "getVersion", std::bind(&Application::getVersion, g_app));
    g_lua.bindClassStaticFunction("g_app", "getBuildCompiler", std::bind(&Application::getBuildCompiler, g_app));
    g_lua.bindClassStaticFunction("g_app", "getBuildDate", std::bind(&Application::getBuildDate, g_app));
    g_lua.bindClassStaticFunction("g_app", "getBuildRevision", std::bind(&Application::getBuildRevision, g_app));
    g_lua.bindClassStaticFunction("g_app", "getBuildType", std::bind(&Application::getBuildType, g_app));

    // ConfigManager
    g_lua.registerStaticClass("g_configs");
    g_lua.bindClassStaticFunction("g_configs", "set", std::bind(&ConfigManager::set, &g_configs, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_configs", "setList", std::bind(&ConfigManager::setList, &g_configs, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_configs", "get", std::bind(&ConfigManager::get, &g_configs, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_configs", "getList", std::bind(&ConfigManager::getList, &g_configs, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_configs", "exists", std::bind(&ConfigManager::exists, &g_configs, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_configs", "remove", std::bind(&ConfigManager::remove, &g_configs, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_configs", "setNode", std::bind(&ConfigManager::setNode, &g_configs, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_configs", "addNode", std::bind(&ConfigManager::addNode, &g_configs, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_configs", "getNode", std::bind(&ConfigManager::getNode, &g_configs, std::placeholders::_1));

    // PlatformWindow
    g_lua.registerStaticClass("g_window");
    g_lua.bindClassStaticFunction("g_window", "move", std::bind(&PlatformWindow::move, &g_window, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_window", "resize", std::bind(&PlatformWindow::resize, &g_window, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_window", "show", std::bind(&PlatformWindow::show, &g_window));
    g_lua.bindClassStaticFunction("g_window", "hide", std::bind(&PlatformWindow::hide, &g_window));
    g_lua.bindClassStaticFunction("g_window", "maximize", std::bind(&PlatformWindow::maximize, &g_window));
    g_lua.bindClassStaticFunction("g_window", "restoreMouseCursor", std::bind(&PlatformWindow::restoreMouseCursor, &g_window));
    g_lua.bindClassStaticFunction("g_window", "showMouse", std::bind(&PlatformWindow::showMouse, &g_window));
    g_lua.bindClassStaticFunction("g_window", "hideMouse", std::bind(&PlatformWindow::hideMouse, &g_window));
    g_lua.bindClassStaticFunction("g_window", "setTitle", std::bind(&PlatformWindow::setTitle, &g_window, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_window", "setMouseCursor", std::bind(&PlatformWindow::setMouseCursor, &g_window, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_window", "setMinimumSize", std::bind(&PlatformWindow::setMinimumSize, &g_window, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_window", "setFullscreen", std::bind(&PlatformWindow::setFullscreen, &g_window, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_window", "setVerticalSync", std::bind(&PlatformWindow::setVerticalSync, &g_window, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_window", "setIcon", std::bind(&PlatformWindow::setIcon, &g_window, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_window", "setClipboardText", std::bind(&PlatformWindow::setClipboardText, &g_window, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_window", "getDisplaySize", std::bind(&PlatformWindow::getDisplaySize, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getClipboardText", std::bind(&PlatformWindow::getClipboardText, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getPlatformType", std::bind(&PlatformWindow::getPlatformType, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getDisplayWidth", std::bind(&PlatformWindow::getDisplayWidth, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getDisplayHeight", std::bind(&PlatformWindow::getDisplayHeight, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getUnmaximizedSize", std::bind(&PlatformWindow::getUnmaximizedSize, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getSize", std::bind(&PlatformWindow::getSize, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getWidth", std::bind(&PlatformWindow::getWidth, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getHeight", std::bind(&PlatformWindow::getHeight, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getUnmaximizedPos", std::bind(&PlatformWindow::getUnmaximizedPos, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getPosition", std::bind(&PlatformWindow::getPosition, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getX", std::bind(&PlatformWindow::getX, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getY", std::bind(&PlatformWindow::getY, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getMousePosition", std::bind(&PlatformWindow::getMousePosition, &g_window));
    g_lua.bindClassStaticFunction("g_window", "getKeyboardModifiers", std::bind(&PlatformWindow::getKeyboardModifiers, &g_window));
    g_lua.bindClassStaticFunction("g_window", "isKeyPressed", std::bind(&PlatformWindow::isKeyPressed, &g_window, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_window", "isMouseButtonPressed", std::bind(&PlatformWindow::isMouseButtonPressed, &g_window, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_window", "isVisible", std::bind(&PlatformWindow::isVisible, &g_window));
    g_lua.bindClassStaticFunction("g_window", "isFullscreen", std::bind(&PlatformWindow::isFullscreen, &g_window));
    g_lua.bindClassStaticFunction("g_window", "isMaximized", std::bind(&PlatformWindow::isMaximized, &g_window));
    g_lua.bindClassStaticFunction("g_window", "hasFocus", std::bind(&PlatformWindow::hasFocus, &g_window));

    // Logger
    g_lua.registerStaticClass("g_logger");
    g_lua.bindClassStaticFunction("g_logger", "log", std::bind(&Logger::log, &g_logger, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_logger", "fireOldMessages", std::bind(&Logger::fireOldMessages, &g_logger));
    g_lua.bindClassStaticFunction("g_logger", "setOnLog", std::bind(&Logger::setOnLog, &g_logger, std::placeholders::_1));

    // UI
    g_lua.registerStaticClass("g_ui");
    g_lua.bindClassStaticFunction("g_ui", "importStyle", std::bind(&UIManager::importStyle, &g_ui, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_ui", "getStyle", std::bind(&UIManager::getStyle, &g_ui, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_ui", "getStyleClass", std::bind(&UIManager::getStyleClass, &g_ui, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_ui", "loadUI", std::bind(&UIManager::loadUI, &g_ui, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_ui", "createWidgetFromStyle", std::bind(&UIManager::createWidgetFromStyle, &g_ui, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_ui", "createWidgetFromOTML", std::bind(&UIManager::createWidgetFromOTML, &g_ui, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_ui", "getRootWidget", std::bind(&UIManager::getRootWidget, &g_ui));
    g_lua.bindClassStaticFunction("g_ui", "getDraggingWidget", std::bind(&UIManager::getDraggingWidget, &g_ui));
    g_lua.bindClassStaticFunction("g_ui", "getPressedWidget", std::bind(&UIManager::getPressedWidget, &g_ui));
    g_lua.bindClassStaticFunction("g_ui", "setDebugBoxesDrawing", std::bind(&UIManager::setDebugBoxesDrawing, &g_ui, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_ui", "isDrawingDebugBoxes", std::bind(&UIManager::setDebugBoxesDrawing, &g_ui, std::placeholders::_1));

    // ModuleManager
    g_lua.registerStaticClass("g_modules");
    g_lua.bindClassStaticFunction("g_modules", "discoverModulesPath", std::bind(&ModuleManager::discoverModulesPath, &g_modules));
    g_lua.bindClassStaticFunction("g_modules", "discoverModules", std::bind(&ModuleManager::discoverModules, &g_modules));
    g_lua.bindClassStaticFunction("g_modules", "autoLoadModules", std::bind(&ModuleManager::autoLoadModules, &g_modules, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_modules", "discoverModule", std::bind(&ModuleManager::discoverModule, &g_modules, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_modules", "ensureModuleLoaded", std::bind(&ModuleManager::ensureModuleLoaded, &g_modules, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_modules", "unloadModules", std::bind(&ModuleManager::unloadModules, &g_modules));
    g_lua.bindClassStaticFunction("g_modules", "reloadModules", std::bind(&ModuleManager::reloadModules, &g_modules));
    g_lua.bindClassStaticFunction("g_modules", "getModule", std::bind(&ModuleManager::getModule, &g_modules, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_modules", "getModules", std::bind(&ModuleManager::getModules, &g_modules));

    // FontManager
    g_lua.registerStaticClass("g_fonts");
    g_lua.bindClassStaticFunction("g_fonts", "importFont", std::bind(&FontManager::importFont, &g_fonts, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_fonts", "fontExists", std::bind(&FontManager::fontExists, &g_fonts, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_fonts", "setDefaultFont", std::bind(&FontManager::setDefaultFont, &g_fonts, std::placeholders::_1));

    // SoundManager
    g_lua.registerStaticClass("g_sounds");
    g_lua.bindClassStaticFunction("g_sounds", "preload", std::bind(&SoundManager::preload, &g_sounds, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_sounds", "enableSound", std::bind(&SoundManager::enableSound, &g_sounds, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_sounds", "play", std::bind(&SoundManager::play, &g_sounds, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_sounds", "enableMusic", std::bind(&SoundManager::enableMusic, &g_sounds, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_sounds", "playMusic", std::bind(&SoundManager::playMusic, &g_sounds, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_sounds", "stopMusic", std::bind(&SoundManager::stopMusic, &g_sounds, std::placeholders::_1));
    g_lua.bindClassStaticFunction("g_sounds", "isMusicEnabled", std::bind(&SoundManager::isMusicEnabled, &g_sounds));
    g_lua.bindClassStaticFunction("g_sounds", "isSoundEnabled", std::bind(&SoundManager::isSoundEnabled, &g_sounds));
    g_lua.bindClassStaticFunction("g_sounds", "isAudioEnabled", std::bind(&SoundManager::isAudioEnabled, &g_sounds));
    g_lua.bindClassStaticFunction("g_sounds", "getCurrentMusic", std::bind(&SoundManager::getCurrentMusic, &g_sounds));

    // EventDispatcher
    g_lua.registerStaticClass("g_eventDispatcher");
    g_lua.bindClassStaticFunction("g_eventDispatcher", "addEvent", std::bind(&EventDispatcher::addEvent, &g_eventDispatcher, std::placeholders::_1, std::placeholders::_2));
    g_lua.bindClassStaticFunction("g_eventDispatcher", "scheduleEvent", std::bind(&EventDispatcher::scheduleEvent, &g_eventDispatcher, std::placeholders::_1, std::placeholders::_2));
}

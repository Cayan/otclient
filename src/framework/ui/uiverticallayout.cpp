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

#include "uiverticallayout.h"
#include "uiwidget.h"
#include <framework/core/eventdispatcher.h>

void UIVerticalLayout::applyStyle(const OTMLNodePtr& styleNode)
{
    UIBoxLayout::applyStyle(styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "align-bottom")
            setAlignBottom(node->value<bool>());
    }
}

bool UIVerticalLayout::internalUpdate()
{
    bool changed = false;

    UIWidgetPtr parentWidget = getParentWidget();
    if(!parentWidget)
        return false;

    UIWidgetList widgets = parentWidget->getChildren();

    if(m_alignBottom)
        std::reverse(widgets.begin(), widgets.end());

    Rect clippingRect = parentWidget->getClippingRect();
    Point pos = (m_alignBottom) ? clippingRect.bottomLeft() : clippingRect.topLeft();
    int preferredHeight = 0;
    int gap;

    for(const UIWidgetPtr& widget : widgets) {
        if(!widget->isExplicitlyVisible())
            continue;

        Size size = widget->getSize();

        gap = (m_alignBottom) ? -(widget->getMarginBottom()+widget->getHeight()) : widget->getMarginTop();
        pos.y += gap;
        preferredHeight += gap;

        if(widget->isFixedSize()) {
            // center it
            pos.x = clippingRect.left() + (clippingRect.width() - (widget->getMarginLeft() + widget->getWidth() + widget->getMarginRight()))/2;
            pos.x = std::max(pos.x, parentWidget->getX());
        } else {
            // expand width
            size.setWidth(clippingRect.width() - (widget->getMarginLeft() + widget->getMarginRight()));
            pos.x = clippingRect.left() + (clippingRect.width() - size.width())/2;
        }

        if(widget->setRect(Rect(pos - parentWidget->getVirtualOffset(), size)))
            changed = true;

        gap = (m_alignBottom) ? -widget->getMarginTop() : (widget->getHeight() + widget->getMarginBottom());
        gap += m_spacing;
        pos.y += gap;
        preferredHeight += gap;
    }

    preferredHeight -= m_spacing;
    preferredHeight += parentWidget->getPaddingTop() + parentWidget->getPaddingBottom();

    if(m_fitChildren && preferredHeight != parentWidget->getHeight()) {
        // must set the preferred width later
        g_eventDispatcher.addEvent([=] {
            parentWidget->setHeight(preferredHeight);
        });
    }

    return changed;
}

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

#include "container.h"
#include "item.h"

Container::Container()
{
    m_id = -1;
    m_itemId = 0;
    m_capacity = 20;
    m_name = "Container";
    m_hasParent = false;
}

void Container::open(const ContainerPtr& previousContainer)
{
    callLuaField("onOpen", previousContainer);
}

void Container::close()
{
    callLuaField("onClose");
}

void Container::addItem(const ItemPtr& item)
{
    m_items.push_front(item);
    updateItemsPositions();

    callLuaField("onAddItem", 0, item);
}

void Container::addItems(const std::vector<ItemPtr>& items)
{
    for(const ItemPtr& item : items)
        m_items.push_back(item);
    updateItemsPositions();
}

void Container::updateItem(int slot, const ItemPtr& item)
{
    if(slot < 0 || slot >= (int)m_items.size()) {
        logTraceError("slot not found");
        return;
    }

    ItemPtr oldItem = m_items[slot];
    m_items[slot] = item;
    item->setPosition(getSlotPosition(slot));

    callLuaField("onUpdateItem", slot, item, oldItem);
}

void Container::removeItem(int slot)
{
    if(slot < 0 || slot >= (int)m_items.size()) {
        logTraceError("slot not found");
        return;
    }

    ItemPtr item = m_items[slot];
    m_items.erase(m_items.begin() + slot);

    updateItemsPositions();

    callLuaField("onRemoveItem", slot, item);
}

ItemPtr Container::getItem(int slot)
{
    if(slot < 0 || slot >= (int)m_items.size())
        return nullptr;
    return m_items[slot];
}

void Container::updateItemsPositions()
{
    for(int slot = 0; slot < (int)m_items.size(); ++slot)
        m_items[slot]->setPosition(getSlotPosition(slot));
}

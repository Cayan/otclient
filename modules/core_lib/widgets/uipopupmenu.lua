UIPopupMenu = extends(UIWidget)

local currentMenu

function UIPopupMenu.create()
  local menu = UIPopupMenu.internalCreate()
  local layout = UIVerticalLayout.create(menu)
  layout:setFitChildren(true)
  menu:setLayout(layout)
  return menu
end

function UIPopupMenu:display(pos)
  -- don't display if not options was added
  if self:getChildCount() == 0 then
    self:destroy()
    return
  end

  if currentMenu then
    currentMenu:destroy()
  end

  rootWidget:addChild(self)
  self:setPosition(pos)
  self:grabMouse()
  self:grabKeyboard()
  currentMenu = self
end

function UIPopupMenu:onGeometryChange()
  self:bindRectToParent()
end

function UIPopupMenu:addOption(optionName, optionCallback)
  local optionWidget = createWidget(self:getStyleName() .. 'Button', self)
  local lastOptionWidget = self:getLastChild()
  optionWidget.onClick = function(self)
    optionCallback()
    self:getParent():destroy()
  end
  optionWidget:setText(optionName)
  local width = optionWidget:getTextSize().width + optionWidget:getMarginLeft() + optionWidget:getMarginRight() + 6
  self:setWidth(math.max(self:getWidth(), width))
end

function UIPopupMenu:addSeparator()
  createWidget(self:getStyleName() .. 'Separator', self)
end

function UIPopupMenu:onDestroy()
  if currentMenu == self then
    currentMenu = nil
  end
end

function UIPopupMenu:onMousePress(mousePos, mouseButton)
  -- clicks outside menu area destroys the menu
  if not self:containsPoint(mousePos) then
    self:destroy()
  end
  return true
end

function UIPopupMenu:onKeyPress(keyCode, keyboardModifiers)
  if keyCode == KeyEscape then
    self:destroy()
    return true
  end
  return false
end

-- close all menus when the window is resized
local function onRootGeometryUpdate()
  if currentMenu then
    currentMenu:destroy()
  end
end
connect(rootWidget, { onGeometryChange = onRootGeometryUpdate} )

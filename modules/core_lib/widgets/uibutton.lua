UIButton = extends(UIWidget)

function UIButton.create()
  local button = UIButton.internalCreate()
  button:setFocusable(false)
  return button
end

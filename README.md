# coolwindows
A module which allows interfering with windows in gmod.
CWIN.FindWindow/FindWindowEx are there for finding the windows to send input to.
CWIN.DoWinInput sends the window specified a message.
CWIN.GetWindowRender allows taking screenshots of windows and pushing them to gmod so that you can e.g. use them in a HUDPaint hook -  not done

Drag & Drop the dll to garrysmod/lua/bin/ change the name gmcl_cwin_win32.dll or gmsv_cwin_win32.dll  and you're set.

example usage:
require("cwin")
local win = CWIN.FindWindow("Valve001")

local s = "Hello World"
for i=1, #s do 
  CWIN.DoWinInput(win, 0x0102, string.byte(s[i]))
end

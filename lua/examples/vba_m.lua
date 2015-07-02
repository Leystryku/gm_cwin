-- Example script for CWIN
-- Made for the Visual Boy Advance-M ( http://vorboss.dl.sourceforge.net/project/vbam/Win32%20-%20MFC/VisualBoyAdvanceM1490.7z )

if(not CWIN) then
	require("CWIN")
	
	if(not CWIN) then
		ErrorNoHalt("CWIN is not installed!\nYou can get it from http://facepunch.com/showthread.php?t=1473181")
		return
	end
end

VBA = VBA or {} -- VBA table
VBA.ManageFocus = true
VBA.playing = false
VBA.Width = 0
VBA.Height = 0

function VBA.SetupWindow()
	-- argument number 1 is the classname, number 2 is the window name. You can only use the classname, or only the window name, or both
	VBA.window = CWIN.FindWindow(""," VisualBoyAdvance-M (SVN1490)") -- the VBA window ( your version may differ, if yes change the window name, case sensitive )
	if(not VBA.window) then
		print("VBA window not found!") -- The window couldn't be found, which means it's either closed or you didn't use FindWindow properly.
		return
	end

	VBA.LastActive = false
	
	VBA.SetWindowStatus(true) -- activate so that we can get the proper size

	-- Render Setup
	VBA.width, VBA.height = CWIN.GetWindowSize(VBA.window) -- Get the width & height of the window

	VBA.render = CWIN.GetWindowRender(VBA.window, VBA.width, VBA.height) -- Create a render texture ( width and height will be changed to a power of two aka 2^n )

end



function VBA.SetWindowStatus( active )
	if(not VBA.window) then return end
	
	-- prevent activating active windows/inactivating inactive windows
	if(active==VBA.LastActive) then return end 

	-- 0 = not active, 1 = active by 'software', 2 = active by mouseclick
	
	local state = 0
	
	if(active) then
		state = 1
	end
	
	CWIN.DoWinInput(VBA.window, WM.ACTIVATE, state)
	VBA.LastActive = active
end





-- KeyCache table since we gotta stop pressing
VBA.KeyCache = {}
VBA.MaxKeys = 255

function VBA.SendKeyInput( key, press )
	
	if(not VBA.window) then return end
	
	local keystate = WM.KEYUP
		
	if(press) then
		keystate = WM.KEYDOWN
	end
	
	CWIN.DoWinInput(VBA.window, keystate, key)
end

function VBA.CheckKeyInput( key )

	if(input.WasKeyPressed(key) ) then -- Key pressed
		if(not VBA.KeyCache[key]) then
			VBA.SendKeyInput(key, true)
			VBA.KeyCache[key] =true
		end
		
		return
	end

	if(VBA.KeyCache[key] and input.WasKeyReleased(key) ) then -- Key not pressed anymore
		VBA.SendKeyInput(key, false)
		VBA.KeyCache[key] = false
	end

end

function VBA.Move()

	-- Input handling
	for i=1, VBA.MaxKeys do -- Should be all keys
	
		VBA.CheckKeyInput(i)
		
	end
	
end

hook.Add("Move", "VBA.Move", VBA.Move)

--This FPS capper sucks
VBA.MaxFps = 70 -- The cap for the FPS. 60 should be good enough for most windows ( and screens ).
VBA.Frames = 0
VBA.FrameWait = 0

local hadfocus = false

function VBA.Tick( )
	if (not VBA.window or VBA.Width < 0 or VBA.Height < 0 or VBA.Width>50000 or VBA.Height>50000) then
		VBA.playing = false
		VBA.SetupWindow()
		return
	end
	
	if (not VBA.playing) then return end
	if (not system.HasFocus() or gui.IsConsoleVisible()) then
		VBA.SetWindowStatus(false)
		return
	else
		VBA.SetWindowStatus(true)
	end
	
	-- FPS Cap
--[[
	if(VBA.FrameWait>CurTime()) then
		return
	else
		VBA.Frames = 0
	end

	if(VBA.Frames >= VBA.MaxFps) then
		VBA.FrameWait = CurTime()+1
		return
	end
	
	VBA.Frames = VBA.Frames + 1
--]]

	VBA.render:Download() -- draw a new frame	
end

hook.Add("Tick","VBA.Tick", VBA.Tick) -- The tick hook is called every frame


function VBA.CheckSize( )
	if(not VBA.window) then return end

	local w, h = CWIN.GetWindowSize(VBA.window)
	
	if(w!=VBA.Width or h != VBA.Height) then
		VBA.Width, VBA.Height = w, h
		VBA.render = CWIN.GetWindowRender(VBA.window, w, h)
		print("Updated window size, w: " .. w .. "h: " .. h)
	end
	
end

timer.Create("VBA.CheckSize", 1, 0, VBA.CheckSize) -- Checks if the window size updated every second

function VBA.Render( )
	if(not VBA.window) then return end
	if(not VBA.playing) then return end
	
	render.DrawTextureToScreenRect( VBA.render, 0, 0, VBA.width, VBA.height)
end
hook.Add("HUDPaint","VBA.Render", VBA.Render)


function VBA.PlayerBindPress(ply, cmd)
	
	if(cmd=="vba") then -- Only the vba bind isn't supressed
		return false
	elseif ( VBA.playing ) then
		return true -- Supress input to Garry's Mod
	end
	
end

hook.Add("PlayerBindPress", "VBA.PlayerBindPress", VBA.PlayerBindPress) 

VBA.SetupWindow()

concommand.Add("vba", function(p,c,a)
	
	VBA.playing = not VBA.playing
	
	if(VBA.playing) then
		VBA.SetWindowStatus(true)
		print("Enabled VBA")
	else
		
		VBA.SetWindowStatus(false)
		print("Disabled VBA")
	end

end)

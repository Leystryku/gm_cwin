if(not CWIN) then require("CWIN") end
if(not CWIN) then return end


gbawindow = CWIN.FindWindow(""," VisualBoyAdvance-M (SVN1229)")

if(not gbawindow) then print("no gbawindow") return end
gba = Material("coolwindows")
local tex = gba:GetTexture("$basetexture")
CWIN.GetWindowRender(gbawindow, tex)


print(gbawindow)

timer.Create("refreshframe",0.3, 0, function()
	tex:Download()
end)

hook.Add("HUDPaint","kkk", function()
	
	if(not gba) then return end
	
	surface.SetDrawColor( 255, 255, 255, 255 )
	surface.SetMaterial( gba )
	surface.DrawTexturedRect( 0, 0, 320, 300 )
end)
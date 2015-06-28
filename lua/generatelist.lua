declutteredwms = {}

for i=1, string.len(wmshit) do
	local iscool = false
	if(wmshit[i] == "W" and wmshit[i+1] == "M" and wmshit[i+2] == "_") then
		
		local thewm = ""
		local kek = i
		
		for i=1, 50 do
			if(wmshit[kek] == "<") then break end
			thewm = thewm .. wmshit[kek]
			kek = kek + 1
		end
		table.insert(declutteredwms, thewm)
	end
end

local freecpp = ""

for k,v in pairs(declutteredwms) do
freecpp = freecpp .. "\n"
freecpp = freecpp .. "		"
freecpp = freecpp .. "LUA->PushNumber(" .. v .. ");"
freecpp = freecpp .. "\n"
freecpp = freecpp .. "		"
freecpp = freecpp .. 'LUA->SetField(-2, "' .. string.gsub(v, "WM_", "") .. '");'
freecpp = freecpp .. "\n"
end

file.Write("k.txt", freecpp)
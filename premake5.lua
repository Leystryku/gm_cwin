solution "coolwindows"
   language "C++"
   location "project"
   targetdir "build/release"

   configuration "vs*" -- speed shit
   --gr- = no typeinfo
   buildoptions({"/Qpar", "/Qfast_transcendentals", "/GL", "/Ox", "/Gm", "/MP", "/MD", "/Gy", "/Gw"})
   linkoptions { "/OPT:REF", "/OPT:ICF", "/LTCG"}

   flags { "Optimize", "NoMinimalRebuild", "NoFramePointer", "EnableSSE2", "FloatFast", "NoBufferSecurityCheck"}

   vpaths {
      ["Header Files/*"] = "src/**.h",
      ["Source Files/*"] = "src/**.cpp",
   }

   kind "SharedLib"

   configurations { "Debug", "Release" }

   files { "src/**.h", "src/**.cpp" }
   
   includedirs {}
   libdirs {}
   
   -- A project defines one build target
   project "coolwindows"
      targetname "gm_cwin_win32"

      configuration "Release"
         defines { "NDEBUG", "_GENERIC" }
         
      configuration "Debug"
         defines { "DEBUG", "_GENERIC" }
         flags { "Symbols", "EnableSSE2" }
         targetdir "build/debug"
-- premake5.lua
workspace "MultiThreadingAssignment"
   configurations { "Debug", "Release", "Test" }

project "MultiThreadingAssignment"
   kind "ConsoleApp"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"

   files { "**.h", "**.c" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "RELEASE" }
      optimize "On"

    filter "configurations:Test"
      defines { "TEST" }
      symbols "on"

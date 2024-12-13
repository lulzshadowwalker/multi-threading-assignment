-- premake5.lua
workspace "MultiThreadingAssignment"
   configurations { "Debug", "Release", "Test", "Unsafe" }

project "MultiThreadingAssignment"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   filter "configurations:Debug"
      defines { "DEBUG", "SAFE" }
      files { "safe.cpp" }
      symbols "On"

   filter "configurations:Release"
      defines { "RELEASE", "SAFE" }
      files { "safe.cpp" }
      optimize "On"

    filter "configurations:Test"
      defines { "TEST", "SAFE" }
      --  NOTE: Trying to keep things simple and condensed in a single safe.cpp file
      files { "test.cpp", "safe.cpp" }
      symbols "On"

    filter "configurations:Unsafe"
      defines { "UNSAFE" }
      files { "unsafe.cpp" }
      symbols "Off"
      optimize "On"

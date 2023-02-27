local sanit = false
local inspect = require 'inspect'
local caustic = loadfile("../caustic/caustic.lua")()
--print('caustic', inspect(caustic))

workspace "ray_example"
    configurations { "Debug", "Release" }

    defines{"GRAPHICS_API_OPENGL_43"}
    includedirs(caustic.libdirs)
    includedirs { 
        "Chipmunk2d/include/",
        "Chipmunk2d/include/include",
        "../caustic/src",
    }
    buildoptions { 
        "-ggdb3",
        "-fPIC",
        "-Wall",
        --"-pedantic",
    }

    language "C"
    kind "ConsoleApp"
    targetprefix ""
    targetdir "."
    symbols "On"

    project "ann_math_solver"
        buildoptions { 
            "-ggdb3",
        }
        files { 
            "./*.h", 
            "./*.c",
        }
        removefiles("*ann_train_set.c")

        print(inspect((caustic.links)))
        --links(caustic.links)
        --links('caustic')
        --links(caustic.links_internal)
        links({
            'raylib',
            'chipmunk',
            'genann',
            'utf8proc',
            'caustic', 
            'smallregex',
            'm'
        })
        --]]

        links('lua')

        libdirs(caustic.libdirs)
        print(inspect(caustic.libdirs))
        --libdirs { "Chipmunk2d/src/", "../caustic/", }
        if sanit then
            linkoptions {
                "-fsanitize=address",
                "-fsanitize-recover=address",
            }
            buildoptions { 
                "-fsanitize=address",
                "-fsanitize-recover=address",
            }
        end

    
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        buildoptions { 
            "-ggdb3"
        }
        linkoptions {
            "-fsanitize=address",
        }
        buildoptions { 
            "-fsanitize=address",
        }

    filter "configurations:Release"
        --buildoptions { 
            --"-O2"
        --}
        --symbols "On"
        --defines { "NDEBUG" }
        --optimize "On"


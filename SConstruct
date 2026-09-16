#!python
import os
import sys

SetOption('max_drift', 1)
CacheDir('.scons-cache')

env = SConscript("godot-cpp/SConstruct")

# --- تفعيل C++20 بدلاً من C++17 الافتراضي ---
if "-std=c++17" in env["CXXFLAGS"]:
    env["CXXFLAGS"].remove("-std=c++17")
env.Append(CXXFLAGS=["-std=c++20"])
# ----------------------------------------------

env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

if env["platform"] == "android":
    target_path = "project/bin/"
    env.Append(CCFLAGS=["-fPIC"])
    library = env.SharedLibrary(
        target_path + "libgridmap2.{}.{}.{}.so".format(env["platform"], env["target"], env["arch"]),
        source=sources,
    )
    Default(library)

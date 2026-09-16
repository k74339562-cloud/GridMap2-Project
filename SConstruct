#!python
import os
import sys

SetOption('max_drift', 1)
CacheDir('.scons-cache')

# تمرير api_version المعتمد رسمياً
env = SConscript("godot-cpp/SConstruct", {"api_version": "4.7"})

# تفعيل C++20
if "-std=c++17" in env["CXXFLAGS"]:
    env["CXXFLAGS"].remove("-std=c++17")
env.Append(CXXFLAGS=["-std=c++20"])

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

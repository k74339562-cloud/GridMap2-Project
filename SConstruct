#!python
import os
import sys

# تفعيل مجلد الكاش لتسريع البناء
SetOption('max_drift', 1)
CacheDir('.scons-cache')

env = SConscript("godot-cpp/SConstruct")

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

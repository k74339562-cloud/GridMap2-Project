#!python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# إعداد ملفات المصدر
env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

# تحديد مجلد الإخراج
if env["platform"] == "android":
    target_path = "project/bin/"
    env.Append(CCFLAGS=["-fPIC"])
    library = env.SharedLibrary(
        target_path + "libgridmap2.{}.{}.{}.so".format(env["platform"], env["target"], env["arch"]),
        source=sources,
    )
    Default(library)

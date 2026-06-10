import os

from setuptools import Extension, setup


extra_link_args = []
libraries = []

if os.name != "nt":
    libraries.append("m")

crolling = Extension(
    name="rollingstats._crolling",
    sources=["rollingstats/_crolling.c"],
    libraries=libraries,
    extra_link_args=extra_link_args,
)

setup(
    name="rollingstats",
    version="0.1.0",
    packages=["rollingstats"],
    ext_modules=[crolling],
)

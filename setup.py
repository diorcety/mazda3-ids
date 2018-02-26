#!/usr/bin/env python

import sys
from setuptools import setup

if sys.version_info[0] == 2:
    proxy = "ProxyTypes"
else:
    proxy = "objproxies"

setup(
    name='ids',
    version='1.0',
    description='IDS',
    author='Yann Diorcet',
    py_modules=['ids'],
    install_requires=[
        'termcolor',
        proxy,
    ],
)

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Register flake8 plugins:
#   http://flake8.pycqa.org/en/latest/plugin-development/registering-plugins.html
#
import setuptools

requires = [
   "flake8 > 3.0.0",
]

setuptools.setup(
   name="flake8_atlas",
   description="ATLAS plugins for flake8",
   entry_points={
      'flake8.extension': [
         'ATL100 = PyUtils.flake8_atlas.checks:OutputLevel'
      ],
   }
)

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
         'ATL100 = PyUtils.flake8_atlas.checks:delayed_string_interpolation',
         'ATL231 = PyUtils.flake8_atlas.python23:hacking_python3x_octal_literals',
         'ATL232 = PyUtils.flake8_atlas.python23:incompatible_print_statement',
         'ATL233 = PyUtils.flake8_atlas.python23:print_statement',
         'ATL234 = PyUtils.flake8_atlas.python23:hacking_no_assert_equals',
         'ATL235 = PyUtils.flake8_atlas.python23:hacking_no_assert_underscore',
         'ATL236 = PyUtils.flake8_atlas.python23:hacking_python3x_metaclass',
         'ATL237 = PyUtils.flake8_atlas.python23:hacking_no_removed_module',
         'ATL238 = PyUtils.flake8_atlas.python23:no_old_style_class',
         'ATL900 = PyUtils.flake8_atlas.checks:OutputLevel',
         'ATL901 = PyUtils.flake8_atlas.checks:print_for_logging',
      ],
   }
)

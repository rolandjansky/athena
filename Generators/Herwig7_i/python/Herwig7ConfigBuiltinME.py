#! /usr/bin/env python

## \file Herwig7ConfigBuiltinME.py
## \brief Configuration class for built-in / "old-style" matrix elements
## \author Daniel Rauch (daniel.rauch@desy.de)

import Herwig7Config as hw7Config
import Herwig7Control as hw7Control
import Herwig7Utils as hw7Utils


## Configuration class for built-in / "old-style" matrix elements
## 
## Example JobOptions are available in [`examples/BuiltinME`](https://svnweb.cern.ch/trac/atlasoff/browser/Generators/Herwig7_i/trunk/examples/BuiltinME) and [`tests/athenaBuiltinMEs/jobOptions`](https://svnweb.cern.ch/trac/atlasoff/browser/Generators/Herwig7_i/trunk/tests/athenaBuiltinMEs/jobOptions).
##
class Hw7ConfigBuiltinME(hw7Config.Hw7Config):


  def __init__(self, genSeq, runArgs, run_name="Herwig"):

    ## provide variables initialized by the parent class
    super(Hw7ConfigBuiltinME, self).__init__(genSeq, runArgs, run_name)

    self.event_generator = "LHCGenerator"


  def local_pre_commands(self):

    return """
## =================================================
## Local Pre-Commands from Herwig7ConfigBuiltinME.py
## =================================================

# > no local pre-commands at the moment
"""

  def local_post_commands(self):

    return """
## ==================================================
## Local Post-Commands from Herwig7ConfigBuiltinME.py
## ==================================================

saverun {} /Herwig/Generators/LHCGenerator
""".format(self.run_name)


  ## High-level function for triggering the process setup and the event generation.
  ##
  ## \warning Please do not modify the generator configuration in the job options after calling the `run()` function as
  ##          the modified settings would not be applied in the event generation
  ##
  def run(self):

    ## add default settings if they were not overwritten in the JobOptions

    self.default_commands += self.energy_commands()
    self.default_commands += self.random_seed_commands()

    if not self.set_printout_commands:
      self.default_commands += self.printout_commands()
    if not self.set_physics_parameter_commands:
      self.default_commands += self.physics_parameter_commands()
    if not self.set_technical_parameter_commands:
      self.default_commands += self.technical_parameter_commands()

    ## do read and run step in one go

    hw7Control.run(self)


  def powheg_commands():
    return """
## Set up Powheg truncated shower
set /Herwig/Shower/Evolver:HardEmissionMode POWHEG
"""

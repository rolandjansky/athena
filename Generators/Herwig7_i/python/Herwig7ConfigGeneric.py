#! /usr/bin/env python

## \file Herwig7ConfigGeneric.py
## \brief Python class for reproducing standalone runs inside of athena

import Herwig7Control as hw7Control
import Herwig7Utils as hw7Utils

## Configuration base class for %Herwig7
class Hw7ConfigGeneric(object):

  ## Constructor
  def __init__(self, genSeq, runArgs, infile_name=None, runfile_name=None):

    self.infile_name  = infile_name
    self.runfile_name = runfile_name
    self.genSeq  = genSeq
    self.runArgs = runArgs


  ## Do the read/run sequence and re-use an already existing infile.
  ##
  ## This function should provide the build, integrate, mergegrids and run step
  ## in one go without creating a gridpack - just pure and direct event generation
  def read_run(self):

    ## perform build/integrate/mergegrids sequence
    hw7Control.do_read_existing_infile(self)

    ## start the event generation
    hw7Control.do_run_existing_runfile(self)


  def build_integrate_run(self, integration_jobs=1, gridpack=None, cleanup_herwig_scratch=True):

    raise RuntimeError("not yet implemented")


  ## Just do the run step and re-use an already existing runfile.
  def run(self):

    ## start the event generation
    hw7Control.do_run_existing_runfile(self)

from ROOT import *
from array import array
import sys
import os
import glob
import re
import math

from ParseCurrentFile    import ReadCurrentHistograms

def ReadJERHistograms(fileToRead) :

  print "Beginning readJERHistograms"

  # Now read the histograms
  print "Reading JER hists from input..."
  # For now, everything except flavour and cross calibration inputs
  # are read in from the final 2012 calibration files.
  # Input file and jet types
  theCurrentHistos = ReadCurrentHistograms(fileToRead) 

  for jettype in theCurrentHistos.keys() :
    for systname in theCurrentHistos[jettype].keys() :
      if not systname.startswith("JER_") : 
        del theCurrentHistos[jettype][systname]

  return theCurrentHistos
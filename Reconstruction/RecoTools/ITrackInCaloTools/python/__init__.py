# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
#
# @file ITrackInCaloTools/python/__init__.py
# @author Bruno Lenzi
# @date Feb 2009
# @brief Module flag file.
#

# Binding for loading C++ dictionary, copied from Sebastien Binet's FourMomUtils
#import Bindings
def _installBindings():
  """load the python bindings to ITrackInCaloTools' function, at module import
  """
  import ROOT
  import PyCintex
  PyCintex.Cintex.Enable ()
  ## load the dictionary:
  PyCintex.loadDictionary('libITrackInCaloToolsDict')
  return

# execute at module import, only if Athena was set
import os
if 'AtlasVersion' in os.environ:
  try:
    _installBindings()
  except:
    print 'ITrackInCaloToolsDict not loaded (harmless if you are outside athena)'

# clean-up
del _installBindings

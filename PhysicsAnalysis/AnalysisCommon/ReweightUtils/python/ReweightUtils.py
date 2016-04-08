# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# sanitize shell output
try:
  from os import environ
  if environ.get('TERM','') == 'xterm':
    environ['TERM'] = 'vt100'
except ImportError:
  pass
except KeyError:
  pass

# filter out annoying warnings regarding missing converters
import warnings
warnings.filterwarnings( action='ignore', category=RuntimeWarning, message='creating converter.*' )

# import basics
from os import getenv
from imp import load_source
from distutils.version import StrictVersion
from platform import python_version

try:
  import ROOT
  # check if the python version is compatible
  if StrictVersion(python_version()) < StrictVersion('2.7.0'):
    raise ImportError("\033[1;31mFATAL\033[0m: unsupported python version, please use at least version 2.7.0")

  # we will need this library later
  ROOT.gSystem.Load("libTreePlayer")

  # retrieve the root core dir environment variable
  RootCoreDir = getenv ("ROOTCOREDIR")
  try:
    t = ROOT.McEventWeight()
  except:
      if RootCoreDir:
        ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
      else:
        print "Nothing foreseen in this case, complain to Olivier..."

except ImportError as err:
  print(err)

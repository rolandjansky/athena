#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os, os.path

def importConfiguration(modname):
    mod = __import__(modname)
    components = modname.split('.')
    for comp in components[1:]:
      mod = getattr(mod,comp)
    return mod

if __name__ == "__main__":

  if len(sys.argv) < 3:
    print "usage: ", sys.argv[0], "<configModule> <property>"
    sys.exit(1)

  configModule = sys.argv[1]
  property = sys.argv[2]    
  
  try:
    cmod = importConfiguration(configModule)
  except Exception, e:
    print "Could not import configuration module \'" + configModule + "\'"
    sys.exit(1)

  config = ""
  pycmd = "config = cmod.%s" % property

  try:
    exec pycmd
  except Exception, e:
    print "Configuration object '%s' not defined in module '%s'. Exit" % (property,configModule)
    sys.exit(1)

  print config


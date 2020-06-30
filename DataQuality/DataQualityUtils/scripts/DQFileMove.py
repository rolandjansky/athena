#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

## *****************************************************************************
def importConfiguration(modname):
    from DataQualityConfigurations import getmodule
    print('getting configuration', modname)
    return getmodule(modname)

def usage():
  cmdi = sys.argv[0].rfind("/")
  cmd = sys.argv[0][cmdi+1:]
  print("")
  print("Usage: ", cmd, "<config> <prefix>")
  print("")
  print("This is a production utility; use TEST config for development and testing.")
  print("")

if __name__ == '__main__':
    import sys
    from DataQualityUtils import filemovemod

    if len(sys.argv) != 3:
        usage()
        sys.exit(1)

    configModule = sys.argv[1]
    
    try:
        cmod = importConfiguration(configModule)
    except Exception:
        print("Could not import configuration module \'" + configModule + "\'")
        sys.exit(1)

    try:
        config = cmod.dqconfig
    except Exception:
        print("Configuration object 'dqconfig' not defined in module \'" + configModule + "\'")
        sys.exit(1)

    filemovemod.move_files(sys.argv[2], config)

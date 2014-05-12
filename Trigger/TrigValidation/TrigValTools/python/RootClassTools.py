# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Module to handle Root classes
# $Id: RootClassTools.py,v 1.3 2008-07-09 15:54:04 fwinkl Exp $

# import ROOT in batch mode and compile classes with ACLiC
def importRoot(classes, tmplibPath = "./tmplibs"):
   import sys
   import os
   _oldArgs = sys.argv[:]   
   sys.argv = sys.argv[:1] + ['-b'] + sys.argv[1:]
   
   import ROOT
   from ROOT import gSystem
   from ROOT import gROOT

   os.system("mkdir -p "+tmplibPath)
   gSystem.SetBuildDir(tmplibPath)
   path = os.environ["TrigValToolsDir"]
   for classname in classes:
      gROOT.ProcessLine(".L "+path+"/Root/"+classname+".C+")
      
   del sys.argv
   sys.argv = _oldArgs
   return ROOT

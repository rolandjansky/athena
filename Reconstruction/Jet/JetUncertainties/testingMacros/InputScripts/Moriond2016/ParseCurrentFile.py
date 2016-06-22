# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

def GetKeyNames(self,dir=""):
  self.cd(dir)
  return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames

def ReadCurrentHistograms(aSystFile,newjetsdict=[]):
 
    # Dictionary to translate from Bogdan's names to provider names
    useJetCollections = ["AntiKt4EMTopo","AntiKt4LCTopo"]
   
    # Just one file here with everything in it.
    systematicFile = TFile(aSystFile,"READ")

    if newjetsdict!=[] :
      useJetCollections = newjetsdict

    histos = {}
    for key in systematicFile.GetKeyNames() : 

      # Only keep things of the jet collections we are using.
      foundJetType = False
      print "looping through useJetCollections",useJetCollections
      for type in useJetCollections :
        if key.endswith(type) :
          jettype = type
          foundJetType = True
      if not foundJetType :
        print "No jet type matches"
        continue
      if jettype not in histos :
        print "Adding extra dict..."
        histos[jettype] = {}

      # Only keep things for which we do not have a more current version.
      systname = key.replace("_"+jettype,"")

      histo = systematicFile.Get(key)
      histo.SetDirectory(0)
      histos[jettype][systname] = histo

    # Done, return dictionary of histos
    return histos
            




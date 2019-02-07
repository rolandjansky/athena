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
 
    useJetCollections = {"AntiKt4EMTopo" : "AntiKt4Topo_EMJES",
                         "AntiKt4EMPFlow": "AntiKt4PFlow_EMJES"}
   
    # Just one file here with everything in it.
    systematicFile = TFile(aSystFile,"READ")

    if newjetsdict!=[] :
      useJetCollections = newjetsdict

    histos = {}
    for key in systematicFile.GetKeyNames() : 

      # Only keep things of the jet collections we are using.
      foundJetType = False
      for type in useJetCollections :
        if key.endswith(type) :
          jettype = type
          foundJetType = True
      if not foundJetType :
        print "No jet type matches"
        continue
      jetkey = useJetCollections[jettype]
      if jetkey not in histos :
        print "Adding extra dict..."
        histos[jetkey] = {}

      systname = key.replace("_"+jettype,"")

      histo = systematicFile.Get(key)
      histo.SetDirectory(0)
      histos[jetkey][systname] = histo

    # Done, return dictionary of histos
    return histos
            

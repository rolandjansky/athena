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


SystematicNames = {'lqResponsePythia' : 'flavorCompLight',
                   'gluonResponsePythia' : 'flavorCompGlu',
                   'gluonResponseDiff' : 'FlavorResponse' }
jetDefDict =    {
                    'AntiKt4EMTopo' : 'AntiKt4Topo_EMJES',
                    'AntiKt4EMPflow' : 'AntiKt4PFlow_EMJES',
                }

def ReadFlavourHistograms(dirName,freezepT=False):
    print "Beginning flavour"
    if not dirName.endswith("/") and not dirName.endswith(".root"):
        dirName = dirName + "/"
    if dirName.endswith("/") :
      files = sorted(glob.glob(dirName+"*.root"))
    else :
      files = [dirName]

    # Run over each file (one per jet definition for LQ/Gluon and one per jet definition for HvP)
    histos = {}
    for aFileName in files:
        # Determine the jet definition
        jetDef = ""
        for jetDef in jetDefDict.keys() :
          print "Beginning jetDef",jetDef
          if jetDefDict[jetDef] not in histos:
              histos[jetDefDict[jetDef]] = {}

          # Read in the histograms from the file
          # Note that we want all the histograms, but with this scheme not everything in the dict is in each file
          # So instead loop over all histos in the file
          inFile = TFile(aFileName,"READ")
          for histName in inFile.GetKeyNames():
              systName = histName.split("_")[0]  
              if not jetDef in histName :
                continue
              if not systName in SystematicNames.keys() :
                if "bResponse" in systName :
                  continue
                print "Error!!!",systName,"not in",SystematicNames.keys()
                continue
              systematicName = systName + "_" + jetDefDict[jetDef]
              histo = inFile.Get(histName)
              if histo is None:
                  print "Failed to get histogram:",systematicName
                  return None

              if freezepT :
                # Loop over eta bins.
                for ybin in range(histo.GetNbinsY()+2) :
                  # Loop out in pT.
                  freezeval = 0
                  for xbin in range(histo.GetNbinsX()+2) :
                    inbin = histo.GetBinContent(xbin,ybin)
                    #print "Bin at pT ",histo.GetXaxis().GetBinCenter(xbin)," and [ylow,y,yhigh] = ", binYLow, histo.GetYaxis().GetBinCenter(ybin), binYHigh
                    # Store bin contents as we go out: last one is one we want as frozen value.
                    if inbin > 0 :
                      freezeval = inbin
                    else :
                      histo.SetBinContent(xbin,ybin,freezeval)

              # Already eta symmetric.
              #histoSym = ProviderHistoHelpers.SymmetrizeHistoInEta(histo,systematicName)
              histo.SetDirectory(0)
              histos[jetDefDict[jetDef]][SystematicNames[systName]] = histo

        # Done reading, close the file
        inFile.Close()

    print "Done flavour.\n"
    return histos

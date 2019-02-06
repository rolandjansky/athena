from ROOT import *
from array import array
import glob
import re
import math
import ProviderHistoHelpers

# Dictionary to translate from Bogan's names to provider names
useJetCollections = {"AntiKt4EMTopo": "AntiKt4Topo_EMJES", "AntiKt4LCTopo": "AntiKt4Topo_LCJES"}

haveUpdatedVersions = ["FlavorResponse",
  "flavorCompGlu",
  "flavorCompLight",
  "Pileup_OffsetMu",
  "Pileup_OffsetNPV",
  "Pileup_PtTerm_Mu",
  "Pileup_PtTerm_NPV",
#  "PunchThrough_AFII",
#  "PunchThrough_MC12",
  "RelativeNonClosure_AFII",
  "RelativeNonClosure_MC12",
  "EtaIntercalibration_Modelling",
  "EtaIntercalibration_TotalStat",]

def GetKeyNames(self,dir=""):
  self.cd(dir)
  return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames

def Read2012Histograms(dirName,scaleEtaInter3Regions=False,scaleEtaInter2Regions=False):
    removeInSitu = False
    if not dirName.endswith("/"):
        dirName = dirName + "/"
    
    # Just one file here with everything in it.
    aSystFile = dirName + "JESUncertainty_2012.root"
    systematicFile = TFile(aSystFile,"READ")

    histos = {}
    for key in systematicFile.GetKeyNames() : 

      # Formatting is all already correct.

      # Only keep things of the jet collections we are using.
      jettype = key.split("_")[-1]
      if jettype not in useJetCollections.keys() :
        continue
      elif useJetCollections[jettype] not in histos :
        print "Adding extra dict..."
        histos[useJetCollections[jettype]] = {}

      # Only keep things for which we do not have a more current version.
      systname = key.replace("_"+jettype,"")
      if systname in haveUpdatedVersions :
        continue

      histo = systematicFile.Get(key)
      histo.SetDirectory(0)

      if key.find("EtaIntercalibration_Modelling")!=-1 and (scaleEtaInter3Regions or scaleEtaInter2Regions) :

        print "OH NO!!!"
        return

        # Loop over pT bins.
        for xbin in range(histo.GetNbinsX()+2) :
          # Loop out in eta.
          for ybin in range(histo.GetNbinsY()+2) :
            run1val = histo.GetBinContent(xbin,ybin)
            #print "Bin at pT ",histo.GetXaxis().GetBinCenter(xbin)," and [ylow,y,yhigh] = ", binYLow, histo.GetYaxis().GetBinCenter(ybin), binYHigh
            # Store bin contents as we go out: last one is one we want as frozen value.
            if abs(histo.GetYaxis().GetBinCenter(ybin)) < 0.8 :
              continue
            elif 0.8 < abs(histo.GetYaxis().GetBinCenter(ybin)) < 2.5 :
              if scaleEtaInter3Regions :
                histo.SetBinContent(xbin,ybin,2.0*run1val)
              else :
                continue
            else :
              histo.SetBinContent(xbin,ybin,3.0*run1val)

      if (key.find("LAr_")!=-1 or key.find("Zjet_")!=-1 or key.find("Gjet")!=-1) and removeInSitu :

        # Loop over all bins. If outside eta range, set to "no uncertainty" = 0
        for xbin in range(histo.GetNbinsX()+2) :
          for ybin in range(histo.GetNbinsY()+2) :
            # Set to zero if outside of eta range.
            #print 'Eta is', abs(histo.GetYaxis().GetBinCenter(ybin))
            if abs(histo.GetYaxis().GetBinCenter(ybin)) > 0.8 :
              histo.SetBinContent(xbin,ybin,0.0)
            else :
              continue
              #print "Leaving bin content."

      if key.find("PunchThrough")!=-1 :

        # Find maximum value on old histogram: we will use this everywhere
        maxVal = histo.GetMaximum()
        print "For punchthrough, using max val",maxVal

        # Loop over all bins in x, y, z.
        for xbin in range(histo.GetNbinsX()+2) :
          for ybin in range(histo.GetNbinsY()+2) :
            for zbin in range(histo.GetNbinsZ()+2) :
              # Permitted regions are only:
              #  - pT > 50 GeV
              #  - eta < 2.7
              #  - NSegments > 20

              # x axis is pT.
              # y axis is nSegments
              # z axis is eta
              if histo.GetXaxis().GetBinCenter(xbin) < 50 or \
                 histo.GetYaxis().GetBinCenter(ybin) < 20 or \
                 histo.GetZaxis().GetBinCenter(zbin) > 2.7 :
                histo.SetBinContent(xbin,ybin,zbin,0.0)

              else :
                # Set to maximum value
                histo.SetBinContent(xbin,ybin,zbin,maxVal)

      if key.find("MC12") :
        # New histo 
        newhist = histo.Clone()
        oldname = histo.GetName()
        newname = oldname.replace("MC12","MC15")
        newhist.SetName(newname)
        newkey = systname.replace("MC12","MC15")
        newhist.SetDirectory(0)
        histos[useJetCollections[jettype]][newkey] = newhist
        continue

      histos[useJetCollections[jettype]][systname] = histo

    # Done, return dictionary of histos
    return histos
            




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

SystematicNames_timeinvariant = {
  'EtaIntercalibration_TotalSyst' : 'EtaIntercalibration_TotalSyst',\
}

SystematicNames_timedependent = {
                   'E_4TeV'                        : 'EtaIntercalibration_NonClosure_highE',\
                   'Eta_2p4_to_2p6'                : 'EtaIntercalibration_NonClosure_posEta',\
                   'Eta_m2p6_to_m2p4'              : 'EtaIntercalibration_NonClosure_negEta'
                  }

jetDefDict =    {
                    'AntiKt4EMTopo' : 'AntiKt4Topo_EMJES',
                    'AntiKt4EMPFlow' : 'AntiKt4PFlow_EMJES',
                }

def ReadEtaIntercalibrationDetailedStats(filename, dictionary) :
  # Inside detailed stats files, individual components are inside of a directory.
  # Make a special name for each and add it to the output dictionary

  openFile = TFile.Open(filename,"READ")
  dirName = "StatNuisanceParams"
  stat_keys = openFile.GetKeyNames(dirName) 

  jet_def = ""
  for thing in jetDefDict.keys() :
    if thing in filename :
      jet_def = thing

  for key in stat_keys :
    hist = openFile.Get(dirName+"/"+key)
    hist.SetDirectory(0)

    save_name = "EtaIntercalibration_"+key
    hist.SetName(save_name+"_"+jet_def)
    hist.SetTitle(save_name+"_"+jet_def)

    dictionary[jetDefDict[jet_def]][save_name] = hist

  return dictionary

def ReadEtaIntercalibrationHistograms(dirName,year=""):
    if not dirName.endswith("/"):
        dirName = dirName + "/"

    # Run over each file (one per jet definition)
    # Collect files and figure out what they are, matching year token specified in run
    histos = {}
    files = sorted(glob.glob(dirName+"*{0}*.root".format(year)))
    for aFileName in files:
        # Determine the jet definition
        jetDef = ""
        for aFileDef,aJetDef in jetDefDict.iteritems():
            if aFileDef in aFileName:
              jetDef = aJetDef
              break
        if jetDef == "":
            print "Failed to determine jet definition for file:",aFileName
            return None
        if jetDef not in histos.keys() :
          histos[jetDef] = {}

        # Read in the histograms of interest from the file
        inFile = TFile(aFileName,"READ")
        contents = inFile.GetKeyNames()

        if year :
          SystematicNames = SystematicNames_timedependent
        else :
          SystematicNames = SystematicNames_timeinvariant

        for aName in SystematicNames.keys():
            if not "NonClosure" in SystematicNames[aName] :
              if "Topo" in jetDef :
                getsystematicName = aName + "_" + jetDef.replace("Topo","")
              else :
                getsystematicName = aName + "_" + jetDef.replace("4PFlow","4EMPFlow").replace("_EMJES","_PFlowJES")
            else :
              getsystematicName = aName

            if not getsystematicName in contents :
              continue

            histo = inFile.Get(getsystematicName)

            systematicName = SystematicNames[aName] + "_" + jetDef
            histo.SetName(systematicName+"_old")
 
            # Histo has different range, extend it
            histoNew = ProviderHistoHelpers.ExtendPtRangeOfHisto(histo,systematicName)
            histoNew.SetDirectory(0)
            histos[jetDef][SystematicNames[aName]] = histoNew

        # Done reading, close the file
        inFile.Close()
    
    # Since histos came from a whole mix of files, make sure we got them all in the end.
    for aName,storeName in SystematicNames.iteritems():
      for aJetDef in histos.keys():
        if not storeName in histos[aJetDef].keys() :
          print "Failed to find histogram:", storeName,"for jet definition",aJetDef

    # Now, if this is year dependent, add stat histograms
    if year :
      for thisDef in jetDefDict.keys() :
        files = sorted(glob.glob(dirName+"*{0}*_statErr_*{1}*.root".format(year,thisDef)))   
        if len(files) != 1 :
          print "Did not find right number of stat term files!"
          print files
          exit(1)
        filename = files[0]
        print histos
        histos = ReadEtaIntercalibrationDetailedStats(filename, histos)
        print histos

    print "Histos out here"
    print histos
    print "\n"
    return histos

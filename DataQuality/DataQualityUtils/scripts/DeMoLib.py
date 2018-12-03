# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
# Author : Benjamin Trocme (LPSC - Grenoble) - 2017
# Auxiliary libraries used DemoUpdate, DeMoStatus and DemoScan
##################################################################

from ROOT import THStack
from ROOT import TCanvas,TLegend
from ROOT import kYellow,kOrange,kRed,kBlue,kPink,kMagenta,kGreen,kSpring,kViolet,kAzure,kCyan

#import gb
from gb import MakeLegend,MakeTH1

########################################################################
# Return a string with the luminosity in a human readable way
# If the unit is %%, this is a percentage
def strLumi(lumi,unit="ub",latex = True,floatNumber = False):
  if (unit == "%%"):
    string0 = "%.2f%%"%(lumi)
  else:
    if (unit == "pb" or unit == "pb^{-1}" or unit == "pb-1"):
      lumi = lumi*1e6

    if lumi < 1e3:
      if latex:
        if (floatNumber):string0 = "%.2f #mub"%(lumi)
        else:string0 = "%.0f #mub"%(lumi)
      else:
        if (floatNumber):string0 = "%.2f ub"%(lumi)  
        else:string0 = "%.0f ub"%(lumi)  
    elif lumi<1e6:
      if (floatNumber):string0 = "%.2f nb"%(lumi/1e3)
      else: string0 = "%.0f nb"%(lumi/1e3)
    elif lumi<1e9:
      if (floatNumber):string0 = "%.2f pb"%(lumi/1e6)    
      else:string0 = "%.0f pb"%(lumi/1e6)    
    else:
      if (floatNumber):string0 = "%.3f fb"%(lumi/1e9)
      else:string0 = "%.1f fb"%(lumi/1e9)
    if latex:
      string0= string0+"^{-1}"
    else:
      string0= string0+"-1"

  return string0


########################################################################
def plotStack(name,histo,index,indexName,histoIntLumi,lumiBool,resStack,resCanvas,resLegend,recovBool = True,compBool = False,approvedPlots = False):
# name: Mainly an index of the output. Also used to define TAxis Title
# histo: dict of histograms or TProfile to be displayed
# index: list of keys of histo to be displayed
# indexName: dict of namesof index used for the TLegend
# histoIntLumi : integrated lumi with the same x binning as histo
# lumiBool : display results in term of lumi and not percent
# resStack, resCanvas, resLegend: dict of (stacks, canvas, legend) outputs
# recovBool: display the recoverable histograms (referenced as %%%__recov in histo
# compBool: this is a 2 yearTag plots. Write all numbers in TLegend and not only last bin (that is not meaningful in yearTag)

  # unit is the main unit. unitAux is the complementary one used only the TLegend
  if (lumiBool):
    unit = "pb^{-1}"
    unitAux = "%%"
  else:
    unit = "%%"
    unitAux = "pb^{-1}"

  nameSplitted = name.split("--") # Assume that the name is "Veto/defect (y axis) - Year/Run (x axis)- Dataset name"
  xAxisTitle = nameSplitted[1]
  if unit == "%%":
    yAxisTitle = "Lost luminosity due to %s [%%]"%(nameSplitted[0])
  else:
    yAxisTitle = "Lost luminosity due to %s [%s]"%(nameSplitted[0],unit)
  legendHeader = nameSplitted[2]

  resCanvas[name] = TCanvas(name,"%s - %s"%(yAxisTitle,xAxisTitle),200, 10, 1150, 500)
  resCanvas[name].SetLeftMargin(0.08)
  resCanvas[name].SetRightMargin(0.28)
  resCanvas[name].SetGridy(1)
  resStack[name] = THStack("%s_stack"%name,"")
  resLegend[name] = MakeLegend(0.725,0.8,0.98,0.95) # Y1 will be redefined later according tp the number of entries
  first = True
  totalIneff = 0.
  totalIneff1 = 0.

  nBinsX = histoIntLumi.GetNbinsX()
  totalIntegratedLumi = histoIntLumi.GetBinContent(nBinsX)
  if lumiBool:
    auxScaleFactor = 100./totalIntegratedLumi
  else:
    auxScaleFactor = totalIntegratedLumi/100.

  for iIndex in sorted(index,reverse=True):
    if first: # Create a recoverable histograms just in case of
      resStack["%s__recov"%name] = MakeTH1("h1_%s__recovTotal"%(name),"Recoverable","",-0.5,-0.5+nBinsX,nBinsX,1)
      resStack["%s__recov"%name].SetMarkerStyle(23)  
      first = False
    iIndexName = iIndex.split("_")[0]

    # Fill histo["%s_toStack"] the main histo
    # and histo["%s_aux"] the complementary one used only for TLegend
    if (histo[iIndex]).IsA().InheritsFrom("TProfile"):
      histo['%s_toStack'%iIndex] = histo[iIndex].ProjectionX()
      histo['%s_toStack'%iIndex].SetFillColor(histo[iIndex].GetFillColor())
    else:
      histo['%s_toStack'%iIndex] = histo[iIndex]
    if lumiBool:
      histo['%s_toStack'%iIndex].Multiply(histo['%s_toStack'%iIndex],histoIntLumi,0.01)
    resStack[name].Add(histo['%s_toStack'%iIndex])

  entryNb = 0
  for iIndex in sorted(index): # Reverse order to have the TLegend ordered as the stacks
    iIndexName = iIndex.split("__")[0] # Trick needed to get the defect name for the recoverable defect histogram    
    baseEntry = "%s"%(strLumi(histo['%s_toStack'%iIndex].GetBinContent(nBinsX),unit))
    auxEntry = "%s"%(strLumi(histo['%s_toStack'%iIndex].GetBinContent(nBinsX)*auxScaleFactor,unitAux))

    if (recovBool and "%s__recov"%iIndex in histo.keys() and histo["%s__recov"%iIndex].GetBinContent(nBinsX) != 0.):
      baseEntryRecov = "%s"%(strLumi(histo["%s__recov"%iIndex].GetBinContent(nBinsX),unit))
      entry = "#splitline{%s}{%s(recov:%s) / %s}"%(indexName[iIndexName],baseEntry,baseEntryRecov,auxEntry) # Second part of Legend to fix
      for iBin in range(nBinsX+1):
        resStack["%s__recov"%name].Fill(iBin-1,histo["%s__recov"%iIndex].GetBinContent(iBin))
    else:
      entry = "#splitline{%s}{%s / %s}"%(indexName[iIndex],baseEntry,auxEntry)

    if (compBool): # This is a >=2 yearTag histograms
      if histo[iIndex].GetNbinsX() == 2:
        entry = "#splitline{%s}{%s / %s }"%(indexName[iIndexName],strLumi(histo[iIndex].GetBinContent(1),unit),strLumi(histo[iIndex].GetBinContent(2),unit))
      elif histo[iIndex].GetNbinsX() == 3:
        entry = "#splitline{%s}{%s / %s / %s}"%(indexName[iIndexName],strLumi(histo[iIndex].GetBinContent(1),unit),strLumi(histo[iIndex].GetBinContent(2),unit),strLumi(histo[iIndex].GetBinContent(3),unit))
      else:
        entry = "%s"%(indexName[iIndexName])
        

    if (histo[iIndex].GetMaximum() != 0): 
      resLegend[name].AddEntry(histo[iIndex],entry,"f")
      entryNb = entryNb+1
      totalIneff = totalIneff + histo['%s_toStack'%iIndex].GetBinContent(nBinsX)
      if compBool:
        totalIneff1 = totalIneff1 + histo['%s_toStack'%iIndex].GetBinContent(1)          
      
  mx = resStack[name].GetMaximum()*1.2
  resStack[name].SetMaximum(mx)
  resStack[name].Draw("hist")
  resStack[name].GetXaxis().SetTitle("%s"%xAxisTitle)
  resStack[name].GetYaxis().SetTitle("%s"%yAxisTitle)
  resStack[name].GetYaxis().SetTitleOffset(0.7)
  if resStack[name].GetMaximum()>10.:
    resCanvas[name].SetLogy(1)

  if compBool:
    resLegend[name].SetHeader("#splitline{%s}{Total loss: %s / %s}"%(legendHeader,strLumi(totalIneff1,unit),strLumi(totalIneff,unit)))
  else:
    totalIneffAux = totalIneff*auxScaleFactor
    if (approvedPlots):
      resLegend[name].SetHeader("#splitline{%s}{Total loss: %s / %s}"%(legendHeader,strLumi(totalIneff,unit),strLumi(totalIneffAux,unitAux)))
    else:
      resLegend[name].SetHeader("#splitline{%s (%s)}{Total loss: %s / %s}"%(legendHeader,strLumi(totalIntegratedLumi,"pb"),strLumi(totalIneff,unit),strLumi(totalIneffAux,unitAux)))
  if resStack["%s__recov"%name].GetEntries() != 0.:
    resStack["%s__recov"%name].SetMarkerStyle(20)
    resStack["%s__recov"%name].SetMarkerColor(kAzure+8)
    resStack["%s__recov"%name].Draw("PSAME HIST")
    resLegend[name].AddEntry(resStack["%s__recov"%name],"#splitline{Recoverable}{total: %.2f%%}"%(resStack["%s__recov"%name].GetBinContent(nBinsX)),"p")
    entryNb = entryNb + 1
          
  resLegend[name].SetY1(max(0.83-entryNb*0.12,0.05))
  resLegend[name].Draw()

  resCanvas[name].Update()
  
  return totalIneff # totalIneff is used only with the savePage1 option in DeMoStatus

########################################################################
def initialize(system,yearTag,partitions,defects0,defectVeto,veto,signOff,year = "2017"):

  # Description used in TLegend and TAxis
  yearTag["description"] = {"Tier0_2015":"/Tier0",
                            "Reproc_2016":"/2016 reproc.",
                            "Tier0_2016":"/Tier0",
                            "Reproc_2017":"/2017 reproc.",
                            "Tier0_2017":"/Tier0",
                            "Reproc_2018":"/2017 reproc.",
                            "Tier0_2018":"/Tier0"}
  # DB tag for the defect database - Common to all systems
  yearTag["defect"] = {"Tier0_2015":"DetStatus-v73-pro19-08",
                       "Reproc_2016":"DetStatus-v75-repro20-01",
                       "Tier0_2016":"DetStatus-v84-pro20-16",
                       "Reproc_2017":"DetStatus-v89-pro21-01",
                       "Tier0_2017":"DetStatus-v97-pro21-13",
                       "Reproc_2018":"HEAD",
                       "Tier0_2018":"HEAD"}  

  yearTag["offlineLumiTag"] = "OflLumi-13TeV-010"

################################# TRT defects
# DB tag for the (veto) condition database
# So far, only LAr use event veto
# can be found with the twiki: https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/CoolProdTags#Tags_for_RUN_2_Bulk_Data_Process
  if system == "TRT":
    yearTag["veto"] = {}

    # Tags below are derived from the regular ones. 
    # They have the same defect/veto tags but have different options/GRL
    # The defect/veto tags are derived from the ones defined in the standard tags given before "."
    similarTags = {"Tier0_2018.onlDelivNorm":"/Tier0 (onl. deliv. lumi.)"}
    for iSimilar in similarTags.keys():
      baseTag = iSimilar.split(".")[0]
      yearTag["description"][iSimilar] = similarTags[iSimilar]
      yearTag["defect"][iSimilar] = yearTag["defect"][baseTag] 

    partitions["color"] = {}
    partitions["list"] = partitions["color"].keys()

    defects0["prefix"] = ["TRT"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = []
    defects0["partTol"] = []
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["BADGAS",
                             "NODATA_06"] 
    defects0["globTol"] = [] 

    # Some defects may not exist in past years. Remove them to avoid crashes
    # WARNING: this fix does not work with multiple year plot
    defectVeto["description"] = {"BADGAS":"Bad gas mixture", # First per partition TRT defects
                                 "NODATA_06":"Large part of TRT off"}

    defectVeto["color"]= {"BADGAS":kBlue-4, # First per partition LAr defects
                          "NODATA_06":kOrange-7,} # Color only for intolerable defects and veto

    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    signOff["EXPR."] = ["TRT_UNCHECKED"]
    signOff["BULK"] = ["TRT_BULK_UNCHECKED"]
    signOff["FINAL"] = []
  
################################# LAr defects
# DB tag for the (veto) condition database
# So far, only LAr use event veto
# can be found with the twiki: https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/CoolProdTags#Tags_for_RUN_2_Bulk_Data_Process
  if system == "LAr":
    yearTag["veto"] = {"Tier0_2015":"LARBadChannelsOflEventVeto-RUN2-UPD4-04",
                       "Reproc_2016":"LARBadChannelsOflEventVeto-RUN2-UPD4-04",
                       "Tier0_2016":"LARBadChannelsOflEventVeto-RUN2-UPD4-04",
                       "Reproc_2017":"LARBadChannelsOflEventVeto-RUN2-UPD4-06",
                       "Tier0_2017":"LARBadChannelsOflEventVeto-RUN2-UPD4-06",
                       "Reproc_2018":"LARBadChannelsOflEventVeto-RUN2-UPD4-08",
                       "Tier0_2018":"LARBadChannelsOflEventVeto-RUN2-UPD4-09"
                       }

    # Tags below are derived from the regular ones. 
    # They have the same defect/veto tags but have different options/GRL
    # The defect/veto tags are derived from the ones defined in the standard tags given before "."
    similarTags = {"Tier0_2015.onlDelivNorm":"/Tier0 (onl. deliv. lumi.)",
                   "Reproc_2016.onlDelivNorm":"/2016 reproc. (onl. deliv. lumi.)",
                   "Tier0_2016.onlDelivNorm":"/Tier0 (onl. deliv. lumi.)",
                   "Reproc_2017.onlDelivNorm":"/2017 reproc. (onl. deliv. lumi.)",
                   "Tier0_2017.onlDelivNorm":"/Tier0 (onl. deliv. lumi.)",
                   "Reproc_2018.onlDelivNorm":"/2016 reproc. (onl. deliv. lumi.)",
                   "Tier0_2018.onlDelivNorm":"/Tier0 (onl. deliv. lumi.)"}
    for iSimilar in similarTags.keys():
      baseTag = iSimilar.split(".")[0]
      yearTag["description"][iSimilar] = similarTags[iSimilar]
      yearTag["defect"][iSimilar] = yearTag["defect"][baseTag] 
      yearTag["veto"][iSimilar] = yearTag["veto"][baseTag] 

    partitions["color"] = { 'EMBA':kYellow-9,'EMBC':kYellow,'EMECA':kOrange,'EMECC':kOrange-3,'HECA':kRed-3,'HECC':kRed+2,'FCALA':kBlue-3,'FCALC':kBlue+2}
    partitions["list"] = partitions["color"].keys()

    defects0["prefix"] = ["LAR","CALO_ONLINEDB"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = ["HVTRIP","SEVNOISEBURST","SEVCOVERAGE","HVNONNOMINAL","SEVNOISYCHANNEL","SEVMISCALIB","SEVUNKNOWN"] # LAR Prefix - LAR_[PART]_[NAME]
    defects0["partTol"] = ["NOISEBURST"]
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["DATACORRUPT","RECOCORRUPT","SPECIALSTUDIES","BADTIMING", # LAR Prefix - LAR_[NAME] 
                             "ONLINEDB_LOWMUCONFIG_IN_HIGHMU_EmergencyMeasures"] # CALO Prefix - CALO_[NAME] 
    defects0["globTol"] = [] 

    # Some defects may not exist in past years. Remove them to avoid crashes
    # WARNING: this fix does not work with multiple year plot
    if ("2016" in year or "2015" in year):
      defects0["globIntol"].remove("SPECIALSTUDIES")
      defects0["globIntol"].remove("BADTIMING")

    defectVeto["description"] = {"HVTRIP":"high voltage trip", # First per partition LAr defects
                                 "NOISEBURST":"noise bursts (before veto)",
                                 "HVNONNOMINAL":"HV non nominal",
                                 "SEVNOISEBURST":"noise burst",
                                 "SEVNOISYCHANNEL":"noisy channels",
                                 "SEVCOVERAGE":"coverage",
                                 "SEVMISCALIB":"global miscalibration",
                                 "SEVUNKNOWN":"unknown reason",
                                 "DATACORRUPT":"data corruption", # Then global LAr defects
                                 "RECOCORRUPT":"corrupted reconstruction",
                                 "SPECIALSTUDIES":"Special studies (on purpose)",
                                 "BADTIMING":"Bad timing",
                                 "ONLINEDB_LOWMUCONFIG_IN_HIGHMU_EmergencyMeasures":"trigger misconfiguration", # And the global CALO defects
                                 "noiseBurst":"noise burst", # And finally the LAr veto
                                 "miniNoiseBurst":"mini noise burst",
                                 "corruption":"data corruption"}
    defectVeto["color"]= {"HVTRIP":kBlue-4, # First per partition LAr defects
                          "HVNONNOMINAL":kOrange-7,
                          "SEVNOISEBURST":kOrange,
                          "SEVNOISYCHANNEL":kRed+1,
                          "SEVCOVERAGE":kMagenta+2,
                          "SEVMISCALIB":kPink-3,
                          "SEVUNKNOWN":kYellow+1,
                          "DATACORRUPT":kGreen-2,# Then global LAr defects
                          "RECOCORRUPT":kSpring-6,
                          "SPECIALSTUDIES":kViolet-4,
                          "BADTIMING":kAzure-8,
                          "ONLINEDB_LOWMUCONFIG_IN_HIGHMU_EmergencyMeasures":kCyan+1, # And the global CALO defects
                          "noiseBurst":kBlue-2, # And finally the LAr veto
                          "miniNoiseBurst":kMagenta+1,
                          "corruption":kAzure+9} # Color only for intolerable defects and veto

    veto["all"] = ["noiseBurst","miniNoiseBurst","corruption"] # Veto name as defined in the COOL database
    veto["COOL"] = {"noiseBurst":"allNoise",
                 "miniNoiseBurst":"MNBNoise",
                 "corruption":"allCorruption"} # Veto name as defined in the COOL database

    signOff["EXPR."] = ["LAR_UNCHECKED"]
    signOff["BULK"] = ["LAR_BULK_UNCHECKED"]
    signOff["FINAL"] = ["LAR_UNCHECKED_FINAL"]

################################# Tile defects
# DB tag for the (veto) condition database
# So far, only LAr use event veto
# can be found with the twiki: https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/CoolProdTags#Tags_for_RUN_2_Bulk_Data_Process
  if system == "Tile":
    yearTag["veto"] = {}

    # Tags below are derived from the regular ones. 
    # They have the same defect/veto tags but have different options/GRL
    # The defect/veto tags are derived from the ones defined in the standard tags given before "."
    similarTags = {"Tier0_2018.onlDelivNorm":"/Tier0 (onl. deliv. lumi.)"}
    for iSimilar in similarTags.keys():
      baseTag = iSimilar.split(".")[0]
      yearTag["description"][iSimilar] = similarTags[iSimilar]
      yearTag["defect"][iSimilar] = yearTag["defect"][baseTag] 

    partitions["color"] = { 'EBA':kYellow-9,'EBC':kYellow,'LBA':kOrange,'LBC':kOrange-3}
    partitions["list"] = partitions["color"].keys()

    defects0["prefix"] = ["TILE"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = ["DAQ_PRB", # TILE Prefix - TILE_[PART]_[NAME]
                             "UNSPECIFIED_SEVERE"]
    defects0["partTol"] = []
    # Global intolerable and tolerable defects
    defects0["globIntol"] = [] 
    defects0["globTol"] = [] 

    # Some defects may not exist in past years. Remove them to avoid crashes
    # WARNING: this fix does not work with multiple year plot
    defectVeto["description"] = {"DAQ_PRB":"DAQ problem", # First per partition Tile defects
                                 "UNSPECIFIED_SEVERE":"Severe unspecified"}

    defectVeto["color"]= {"DAQ_PRB":kBlue-4, # First per partition LAr defects
                          "UNSPECIFIED_SEVERE":kOrange-7,} # Color only for intolerable defects and veto

    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    signOff["EXPR."] = ["TILE_UNCHECKED"]
    signOff["BULK"] = ["TILE_UNCHECKED"]
    signOff["FINAL"] = []

################################# Jet/MET/EGamma/Tau/CaloGlobal defects
  if system == "CaloCP":
    yearTag["veto"] = {}

    # Tags below are derived from the regular ones. 
    # They have the same defect/veto tags but have different options/GRL
    # The defect/veto tags are derived from the ones defined in the standard tags given before "."
    similarTags = {"Tier0_2018.onlDelivNorm":"/Tier0 (onl. deliv. lumi.)"}
    for iSimilar in similarTags.keys():
      baseTag = iSimilar.split(".")[0]
      yearTag["description"][iSimilar] = similarTags[iSimilar]
      yearTag["defect"][iSimilar] = yearTag["defect"][baseTag] 

    partitions["color"] = { 'BARREL':kYellow-9,'CRACK':kRed-3,'ENDCAP':kBlue-3, # EGamma partitions
                            'B':kYellow-9,'CR':kRed-3,'E':kBlue-3, # Tau partitions
                            'CALB':kYellow-9,'CALEA':kRed-3,'CALC':kBlue-3} # CaloGlobal partitions
                            
    partitions["list"] = partitions["color"].keys()

    defects0["prefix"] = ["JET","EGAMMA","MET","TAU","CALO_"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = ["LARNOISE","ENERGY", # EGAMMA Prefix - EGAMMA_[NAME]_[PART]
                             "CALO", # TAU Prefix- TAU_[PART]_[NAME]
                             "TopoClusterNoiseSevere"] # CaloGlobal Prefix - CALO_[PART]_[NAME]
                             
    defects0["partTol"] = [] 
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["NOJETSREGION","SEVERE_HOTSPOT", # JET Prefix - JET_[NAME] 
                             "BEAMSPOT",# EGAMMA Prefix - EGAMMA_[NAME] 
                             "Ex_largeshift","Ey_largeshift","SumEt_largeshift"] # MET Prefix - MET_[NAME]

    defects0["globTol"] = [] 


    defectVeto["description"] = {"NOJETSREGION":"[Jet] cold region",
                                 "SEVERE_HOTSPOT":"[Jet] hot region",
                                 "LARNOISE":"[EGamma] noise in LAr",
                                 "ENERGY":"[EGamma] problem in energy",
                                 "BEAMSPOT":"[EGamma] problem in beam spot",
                                 "Ex_largeshift":"[MEt] Ex large shift",
                                 "Ey_largeshift":"[MEt] EY large shift",
                                 "SumEt_largeshift":"[MEt] SumEt large shift",
                                 "CALO":"[Tau] calo problem",
                                 "TopoClusterNoiseSevere":"[TopoCluster] hot spot"}
    defectVeto["color"]= {"NOJETSREGION":kBlue-4,
                          "SEVERE_HOTSPOT":kOrange-7,
                          "LARNOISE":kOrange,
                          "ENERGY":kRed+1,
                          "BEAMSPOT":kMagenta+2,
                          "Ex_largeshift":kPink-3,
                          "Ey_largeshift":kGreen-2,
                          "SumEt_largeshift":kSpring-6,
                          "CALO":kYellow+1,
                          "TopoClusterNoiseSevere":kCyan+1} # Color only for intolerable defects and veto

    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    signOff["EXPR."] = ['CALO_UNCHECKED','EGAMMA_UNCHECKED','JET_UNCHECKED','MET_UNCHECKED','TAU_UNCHECKED']
    signOff["BULK"] = ['CALO_BULK_UNCHECKED','EGAMMA_BULK_UNCHECKED','JET_BULK_UNCHECKED','MET_BULK_UNCHECKED','TAU_BULK_UNCHECKED']
    signOff["FINAL"] = []

################################# Jet/MET/EGamma/Tau/CaloGlobal defects
  if system == "BTag":
    yearTag["veto"] = {}

    # Tags below are derived from the regular ones. 
    # They have the same defect/veto tags but have different options/GRL
    # The defect/veto tags are derived from the ones defined in the standard tags given before "."
    similarTags = {"Tier0_2018.onlDelivNorm":"/Tier0 (onl. deliv. lumi.)"}
    for iSimilar in similarTags.keys():
      baseTag = iSimilar.split(".")[0]
      yearTag["description"][iSimilar] = similarTags[iSimilar]
      yearTag["defect"][iSimilar] = yearTag["defect"][baseTag] 

    partitions["color"] = { } # No partition needed
                            
    partitions["list"] = partitions["color"].keys()

    defects0["prefix"] = ["BTAG"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = [] # No partition defect
                             
    defects0["partTol"] = [] 
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["BLAYER_SERIOUS_PROBLEM"] 

    defects0["globTol"] = ["BEAMSPOT_SHIFT",
                           "NOJETS"] 


    defectVeto["description"] = {"BLAYER_SERIOUS_PROBLEM":"B layer problem",
                                 "BEAMSPOT_SHIFT":"Beamspot shift",
                                 "BTAG_NOJETS":"No jets in monitoring"}
    defectVeto["color"]= {"BLAYER_SERIOUS_PROBLEM":kBlue-4,
                          "BEAMSPOT_SHIFT":kOrange-7,
                          "BTAG_NOJETS":kOrange} # Color only for intolerable defects and veto

    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    signOff["EXPR."] = ['BTAG_UNCHECKED']
    signOff["BULK"] = ['BTAG_BULK_UNCHECKED']
    signOff["FINAL"] = []


################ Definitions common to all systems
  defects0["part"] = defects0["partIntol"] + defects0["partTol"]
  defects0["glob"] = defects0["globIntol"] + defects0["globTol"]

  defects0["intol"] = defects0["globIntol"] + defects0["partIntol"]
  defects0["tol"] = defects0["globTol"] + defects0["partTol"]

  defects0["partIntol_recov"] = []
  defects0["globIntol_recov"] = []
  for idef in defects0["partIntol"]: # Create a duplicated list of intol defect to monitor the recoverability
    defects0["partIntol_recov"].append("%s__recov"%idef)
  for idef in defects0["globIntol"]: # Create a duplicated list of intol defect to monitor the recoverability
    defects0["globIntol_recov"].append("%s__recov"%idef)
  defects0["intol_recov"] = defects0["partIntol_recov"] + defects0["globIntol_recov"]

  return True

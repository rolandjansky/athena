# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Author : Benjamin Trocme (LPSC - Grenoble) - 2017
# Auxiliary libraries used DemoUpdate, DeMoStatus and DemoScan
##################################################################

from ROOT import THStack
from ROOT import TCanvas
from ROOT import kYellow,kOrange,kRed,kBlue,kPink,kMagenta,kGreen,kSpring,kViolet,kAzure,kCyan,kTeal,kBlack

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

#########################################################################################
#########################################################################################
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

  # Condition tag for the veto database - defined later per system when relevant
  yearTag["veto"] = {}

  # Tags below are derived from the regular ones. 
  # They have the same defect/veto tags but have different options/GRL
  # The defect/veto tags are derived from the ones defined in the standard tags given before "."
  # These tags are common to all system but some additional ones may be defined later.
  # This is why the defect/veto tags are defined after the system definitions
  similarTags = {"Tier0_2015.onlDelivNorm":"/Tier0 (onl. deliv. lumi.)",
                 "Reproc_2016.onlDelivNorm":"/2016 reproc. (onl. deliv. lumi.)",
                 "Tier0_2016.onlDelivNorm":"/Tier0 (onl. deliv. lumi.)",
                 "Reproc_2017.onlDelivNorm":"/2017 reproc. (onl. deliv. lumi.)",
                 "Tier0_2017.onlDelivNorm":"/Tier0 (onl. deliv. lumi.)",
                 "Reproc_2018.onlDelivNorm":"/2018 reproc. (onl. deliv. lumi.)",
                 "Tier0_2018.onlDelivNorm":"/Tier0 (onl. deliv. lumi.)"}


  yearTag["offlineLumiTag"] = {"preliminary":"OflPrefLumi-RUN2-UPD4-10","grl":"OflLumi-13TeV-001"}


#################################### NEWSYSTEM defects
###  if system == "NEWSYSTEM":
###    partitions["color"] = {}
###    partitions["list"] = partitions["color"].keys()
###
###    defects0["prefix"] = ["NEWSYSTEM"]
###    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
###    defects0["partIntol"] = []
###    defects0["partTol"] = []
###    # Global intolerable and tolerable defects
###    defects0["globIntol"] = [""] 
###    defects0["globTol"] = [] 
###    
###    veto["all"] = [] # Veto name as defined in the COOL database
###    veto["COOL"] = {} # Veto name as defined in the COOL database
###
###    defectVeto["description"] = {"":""}
###
###    signOff["EXPR."] = ["NEWSYSTEM_UNCHECKED"]
###    signOff["BULK"] = ["NEWSYSTEM_BULK_UNCHECKED"]
###    signOff["FINAL"] = []
  
#################################### Pixel defects
  if system == "Pixel":
    partitions["color"] = {'IBL':kYellow-9,'LAYER0':kYellow,'BARREL':kOrange,'ENDCAPC':kOrange-3,'ENDCAPA':kRed-3}
    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["PIXEL"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = ["DISABLED",
                             "GT30pct_NOTREADY",
                             "READOUT_PROBLEM",
                             "HVSCAN","TIMING",
                             "STANDBY"]
    defects0["partTol"] = []
    # Global intolerable and tolerable defects
    defects0["globIntol"] = [] 
    defects0["globTol"] = [] 
    
    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    defectVeto["description"] = {"DISABLED":"One layer disabled",
                                 "GT30pct_NOTREADY":">30% modules in error",
                                 "READOUT_PROBLEM":"Readout problem",
                                 "HVSCAN":"HV scan",
                                 "TIMING":"Timing scan",
                                 "STANDBY":"Standby"}

    signOff["EXPR."] = ["PIXEL_UNCHECKED"]
    signOff["BULK"] = ["PIXEL_BULK_UNCHECKED"]
    signOff["FINAL"] = []
  


################################# SCT defects
# https://twiki.cern.ch/twiki/bin/view/Atlas/SCTOfflineMonitoringShifts#List_of_Defects
  if system == "SCT":
    partitions["color"] = {}
    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["SCT"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = []
    defects0["partTol"] = []
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["GLOBAL_STANDBY",
                             "CRATE_OUT",
                             "ROD_OUT_MAJOR",
                             "PERIOD_ERR_MAJOR",
                             "GLOBAL_DESYNC",
                             "GLOBAL_RECONFIG",
                             "GLOBAL_UNKNOWN"] 
    defects0["globTol"] = ["MOD_OUT_GT40",
                           "MOD_ERR_GT40",
                           "MOD_NOISE_GT40",
                           "PERIOD_ERR_GT40",
                           "ROD_OUT",
                           "EFF_LT99",
                           "NOTNOMINAL_HV",
                           "NOTNOMINAL_THRESHOLD",
                           "NOTNOMINAL_TIMING",
                           "COOLINGLOOP_OUT_1"]

    defectVeto["description"] = {"GLOBAL_STANDBY":"Standby (HV < 1.5kV)", # Intolerable defects
                                 "CRATE_OUT":">=1 crate out",
                                 "ROD_OUT_MAJOR":"Large inefficency (ROD)",
                                 "PERIOD_ERR_MAJOR":"Large inefficiency (PERIOD)",
                                 "GLOBAL_DESYNC":"Global desync",
                                 "GLOBAL_RECONFIG":"Global reconfig",
                                 "GLOBAL_UNKNOWN":"Unknown",
                                 "MOD_OUT_GT40":"More than 40 modules excluded in DAQ in addition to the permanent disabled modules (37 modules as of June 2017)", # Tolerable defects
                                 "MOD_ERR_GT40":"More than 40 modules with bytestream errors ",
                                 "MOD_NOISE_GT40":"More than 40 noisy modules",
                                 "PERIOD_ERR_GT40":"More than 80 links with errors in a short period of time (fine for the rest of the run), corresponding to about 40 modules",
                                 "ROD_OUT":"One or more ROD(s) excluded from readout, however less than 5% region in eta-phi plane have masked link errors among more than two layers.",
                                 "EFF_LT99":"Less than 99% efficiency for 1st BC and less than 98% efficiency for all bunches in one or more DQ regions",
                                 "NOTNOMINAL_HV":"SCT neither at 150 V nor at 50 V",
                                 "NOTNOMINAL_THRESHOLD":"SCT threshold not at 1 fC",
                                 "NOTNOMINAL_TIMING":"Unusual timing settings, e.g. timing scan",
                                 "COOLINGLOOP_OUT_1":"Loss of a single cooling loop"}

    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    signOff["EXPR."] = ["SCT_UNCHECKED"]
    signOff["BULK"] = ["SCT_BULK_UNCHECKED"]
    signOff["FINAL"] = []
  
################################# TRT defects
# https://twiki.cern.ch/twiki/bin/view/Atlas/TRTDQDefects
  if system == "TRT":
    partitions["color"] = {}
    partitions["list"] = list(partitions["color"].keys())

    defects0["prefix"] = ["TRT"]
    # Pa intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = []
    defects0["partTol"] = []
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["BADCALIBRATION",
                             "BADSTRAWLIST",
                             "BADGAS",
                             "DAQPROBLEMS_OTHER",
                             "DESYNC",
                             "NODATA_06"] 
    defects0["globTol"] = ["BADCALIBRATION_MINOR",
                           "BADGAS_MINOR",
                           "BYTESTREAM_BITFLIPS",
                           "DAQPROBLEMS_OTHER_MINOR",
                           "NODATA_01",
                           "NONNOMINAL_HT",
                           "NONNOMINAL_LT"] 

    # Some defects may not exist in past years. Remove them to avoid crashes
    # WARNING: this fix does not work with multiple year plot
    defectVeto["description"] = {"BADCALIBRATION":"Bad calibration",
                                 "BADSTRAWLIST":"Bad dead stram list",
                                 "BADGAS":"Bad gas mixture",
                                 "DAQPROBLEMS_OTHER":"DAQ problems",
                                 "DESYNC":"Desynchronisation",
                                 "NODATA_06":"Large part of TRT off"}

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
                       "Tier0_2018":"LARBadChannelsOflEventVeto-RUN2-UPD4-10"
                       }

    # Additional similar tags specific to LAr
    similarTags["Reproc_2018.roughVeto"]="/2018 Reproc. (rough veto)"

    partitions["color"] = { 'EMBA':kYellow-9,'EMBC':kYellow,'EMECA':kOrange,'EMECC':kOrange-3,'HECA':kRed-3,'HECC':kRed+2,'FCALA':kBlue-3,'FCALC':kBlue+2}
    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["LAR","CALO_ONLINEDB"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = ["HVTRIP","SEVNOISEBURST","SEVCOVERAGE","HVNONNOMINAL","SEVNOISYCHANNEL","SEVMISCALIB","SEVUNKNOWN"] # LAR Prefix - LAR_[PART]_[NAME]
    defects0["partTol"] = ["COVERAGE","HVNONNOM_CORRECTED"]#["NOISEBURST"]
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
                                 "COVERAGE":"coverage (tolerable)",
                                 "ONLINEDB_LOWMUCONFIG_IN_HIGHMU_EmergencyMeasures":"trigger misconfiguration", # And the global CALO defects
                                 "noiseBurst":"noise burst", # And finally the LAr veto
                                 "miniNoiseBurst":"mini noise burst",
                                 "corruption":"data corruption"}

    veto["all"] = ["noiseBurst","miniNoiseBurst","corruption"] # Veto name as defined in the COOL database
    veto["COOL"] = {"noiseBurst":"allNoise",
                 "miniNoiseBurst":"MNBNoise",
                 "corruption":"allCorruption"} # Veto name as defined in the COOL database

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

    signOff["EXPR."] = ["LAR_UNCHECKED"]
    signOff["BULK"] = ["LAR_BULK_UNCHECKED"]
    signOff["FINAL"] = ["LAR_UNCHECKED_FINAL"]

################################# Tile defects
# https://twiki.cern.ch/twiki/bin/view/Atlas/TileDQLeaderManual#Global_Tile_Defects
  if system == "Tile":
    partitions["color"] = { 'EBA':kYellow-9,'EBC':kYellow,'LBA':kOrange,'LBC':kOrange-3}
    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["TILE"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = ["BAD_COVER",
                             "DAQ_PRB",
                             "DB_SEVERE",
                             "TIMING_SEVERE",
                             "UNSPECIFIED_SEVERE"]
    defects0["partTol"] = ["DB_MINOR","TIMING_MINOR","TRIP","UNSPECIFIED_MINOR"]
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["LOWSTAT"] 
    defects0["globTol"] = ["TIMEJUMPS_UNDEFINED"] 

    # Some defects may not exist in past years. Remove them to avoid crashes
    # WARNING: this fix does not work with multiple year plot
    defectVeto["description"] = {"BAD_COVER":"Coverage",
                                 "DAQ_PRB":"DAQ problem",
                                 "DB_SEVERE":"DB issue",
                                 "TIMING_SEVERE":"Timing issue",
                                 "UNSPECIFIED_SEVERE":"Severe unspecified",
                                 "LOWSTAT":"Low stats"}

    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    signOff["EXPR."] = ["TILE_UNCHECKED"]
    signOff["BULK"] = ["TILE_UNCHECKED"]
    signOff["FINAL"] = []

#################################### MUON-CSC defects
# https://twiki.cern.ch/twiki/bin/view/Atlas/MuonMCPDefectList
  if system == "CSC":
    partitions["color"] = {"EA":kYellow-9,'EC':kRed-3}
                           
    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["MS_CSC"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = ["STANDBY_HV",
                             "PROBLEM",
                             "ROD_DISABLED"]

    defects0["partTol"] = ["DISCONNECTED2","LATENCY_MINOR","THRESHOLD"]
    # Global intolerable and tolerable defects
    defects0["globIntol"] = []
    defects0["globTol"] = [] 
    
    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    defectVeto["description"] = {"STANDBY_HV":"Standby HV",
                                 "PROBLEM":"Coverage loss > 10%%",
                                 "ROD_DISABLED":">=1 ROD not readout"}

    signOff["EXPR."] = ["MS_UNCHECKED"]
    signOff["BULK"] = ["MS_BULK_UNCHECKED"]
    signOff["FINAL"] = []


#################################### MUON-MDT defects
# https://twiki.cern.ch/twiki/bin/view/Atlas/MuonMCPDefectList
  if system == "MDT":
    partitions["color"] = {"EA":kYellow-9,'EC':kRed-3,'BA':kBlue-3,'BC':kOrange-3}
                           
    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["MS_MDT"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = ["STANDBY_HV",
                             "PROBLEM",
                             "ROD_PROBLEM_5orMore"]

    defects0["partTol"] = ["ROD_PROBLEM_1","ROD_PROBLEM_2to4"]
    # Global intolerable and tolerable defects
    defects0["globIntol"] = [] 
    defects0["globTol"] = [] 
    
    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    defectVeto["description"] = {"STANDBY_HV":"Standby HV",
                                 "PROBLEM":"Coverage loss > 10%%",
                                 "ROD_PROBLEM_5orMore":">=5 RODs not readout"}

    signOff["EXPR."] = ["MS_UNCHECKED"]
    signOff["BULK"] = ["MS_BULK_UNCHECKED"]
    signOff["FINAL"] = []


#################################### MUON-RPC defects
# https://twiki.cern.ch/twiki/bin/view/Atlas/MuonMCPDefectList
  if system == "RPC":
    partitions["color"] = {'BA':kBlue-3,'BC':kOrange-3}
                           
    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["MS_RPC"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = ["STANDBY_HV",
                             "PROBLEM",
                             "PROBLEM_10to15percent",
                             "PROBLEM_MoreThan15percent",
                             "OutOfSync_3orMore",
                             "LowEfficiency_MoreThan10percent"]

    defects0["partTol"] = ["LowEfficiency_5to10percent","OutOfSync_2","OutOfSync_1","PROBLEM_1","PROBLEM_5to10percent"]
    # Global intolerable and tolerable defects
    defects0["globIntol"] = [] 
    defects0["globTol"] = [] 
    
    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    defectVeto["description"] = {"STANDBY_HV":"Standby HV",
                                 "PROBLEM":"Coverage loss > 10%%",
                                 "PROBLEM_10to15percent":"Coverage loss > 10%%",
                                 "PROBLEM_MoreThan15percent":"Coverage loss > 15%%",
                                 "OutOfSync_3orMore":">3 Out of sync",
                                 "LowEfficiency_MoreThan10percent":"Low efficiency > 10%%"}

    signOff["EXPR."] = ["MS_UNCHECKED"]
    signOff["BULK"] = ["MS_BULK_UNCHECKED"]
    signOff["FINAL"] = []


#################################### MUON-TGC defects
# https://twiki.cern.ch/twiki/bin/view/Atlas/MuonMCPDefectList
  if system == "TGC":
    partitions["color"] = {"EA":kYellow-9,'EC':kRed-3}

    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["MS_TGC"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = ["STANDBY_HV",
                             "PROBLEM",
                             "ROD_PROBLEM-2orMore"]

    defects0["partTol"] = ["PROBLEM_1"]
    # Global intolerable and tolerable defects
    defects0["globIntol"] = [] 
    defects0["globTol"] = [] 
    
    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    defectVeto["description"] = {"STANDBY_HV":"Standby HV",
                                 "PROBLEM":"Coverage loss > 10%%",
                                 "ROD_PROBLEM_2orMore":">=2 RODs not readout"}

    signOff["EXPR."] = ["MS_UNCHECKED"]
    signOff["BULK"] = ["MS_BULK_UNCHECKED"]
    signOff["FINAL"] = []


#################################### MUON CP defects
# https://twiki.cern.ch/twiki/bin/view/Atlas/MuonMCPDefectList
  if system == "MuonCP":
    defects0["prefix"] = ["MCP"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = []

    defects0["partTol"] = []
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["ALIGN_GEO",
                             "LOW_EFFICIENCY_MAJOR"] 
    defects0["globTol"] = ["CHI2_PROBLEM",
                           "ID_PROBLEM",
                           "LOW_EFFICIENCY_MINOR",
                           "MS_PROBLEM"] 
    
    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    defectVeto["description"] = {"ALIGN_GEO":"[MCP] Bad alignment/geometry)",
                                 "LOW_EFFICIENCY_MAJOR":"[MCP] Low reconstruction efficiency" }

    signOff["EXPR."] = ["MS_UNCHECKED"]
    signOff["BULK"] = ["MS_BULK_UNCHECKED"]
    signOff["FINAL"] = []


#################################### ID defects
  if system == "IDGlobal":
    partitions["color"] = {}
    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["ID"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = []
    defects0["partTol"] = []
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["NOTRACKS","x_TRACKCOVERAGE_SEVERE","UNKNOWN","BS_RUNAVERAGE","BS_PARAMETERSTEP","BS_NOTNOMINAL"] 
    defects0["globTol"] = ["ALIGN_DEGRADED","LOWSTAT","x_TRACKCOVERAGE","VERTEXBUG"] 
    
    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    defectVeto["description"] = {"NOTRACKS":"No tracks",
                                 "x_TRACKCOVERAGE_SEVERE":"> 10 %% coverage loss",
                                 "UNKNOWN":"Unknown",
                                 "BS_RUNAVERAGE":"Problematic BS determination",
                                 "BS_PARAMETERSTEP":"Large changes in BS",
                                 "BS_NOTNOMINAL":"Sizable modulation in d0 vs phi",
                                 "ALIGN_DEGRADED":"Degarded alignment",
                                 "LOWSTAT":"Low statistics",
                                 "x_TRACKCOVERAGE":"Significant change in coverage, but not severe (between 5-10% coverage loss)",
                                 "VERTEXBUG":"Problems in the determination of the primary vertex"}

    signOff["EXPR."] = ["ID_UNCHECKED"]
    signOff["BULK"] = ["ID_BULK_UNCHECKED"]
    signOff["FINAL"] = []

################################# Jet/MET/EGamma/Tau/CaloGlobal defects
# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EgammaShifts
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/JetEtMissDataQuality2016#Jets_defects
# https://twiki.cern.ch/twiki/bin/view/Atlas/TauDataQualityMonitoringRun2#Frequent_problems_or_defects
  if system == "CaloCP":
    partitions["color"] = { 'BARREL':kYellow-9,'CRACK':kRed-3,'ENDCAP':kBlue-3, # EGamma partitions
                            'B':kYellow-9,'CR':kRed-3,'E':kBlue-3, # Tau partitions
                            'CALB':kYellow-9,'CALEA':kRed-3,'CALC':kBlue-3} # CaloGlobal partitions
                            
    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["JET","EGAMMA","MET","TAU","CALO_"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = ["LARNOISE","ENERGY", # EGAMMA Prefix - EGAMMA_[NAME]_[PART]
                             "CALO","TRK","KIN","ID", # TAU Prefix- TAU_[PART]_[NAME]
                             "TopoClusterNoiseSevere"] # CaloGlobal Prefix - CALO_[PART]_[NAME]
                             
    defects0["partTol"] = [] 
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["NOJETSREGION","SEVERE_HOTSPOT", # JET Prefix - JET_[NAME] 
                             "BEAMSPOT",# EGAMMA Prefix - EGAMMA_[NAME] 
                             "Ex_largeshift","Ey_largeshift","SumEt_largeshift"] # MET Prefix - MET_[NAME]

    defects0["globTol"] = ["ETAPHI_SPIKES"] 


    defectVeto["description"] = {"NOJETSREGION":"[Jet] cold region",
                                 "SEVERE_HOTSPOT":"[Jet] hot region",
                                 "LARNOISE":"[EGamma] noise in LAr",
                                 "ENERGY":"[EGamma] problem in energy",
                                 "BEAMSPOT":"[EGamma] problem in beam spot",
                                 "ETAPHI_SPIKES":"[Egamma] eta/phi spikes",
                                 "Ex_largeshift":"[MEt] Ex large shift",
                                 "Ey_largeshift":"[MEt] EY large shift",
                                 "SumEt_largeshift":"[MEt] SumEt large shift",
                                 "CALO":"[Tau] calo problem",
                                 "TRK":"[Tau] track problem",
                                 "KIN":"[Tau] kinetic problem",
                                 "ID":"[Tau] identification problem",
                                 "TopoClusterNoiseSevere":"[TopoCluster] hot spot"}

    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    signOff["EXPR."] = ['CALO_UNCHECKED','EGAMMA_UNCHECKED','JET_UNCHECKED','MET_UNCHECKED','TAU_UNCHECKED']
    signOff["BULK"] = ['CALO_BULK_UNCHECKED','EGAMMA_BULK_UNCHECKED','JET_BULK_UNCHECKED','MET_BULK_UNCHECKED','TAU_BULK_UNCHECKED']
    signOff["FINAL"] = []

################################# BTAG defects
# https://twiki.cern.ch/twiki/bin/view/Atlas/FlavourTaggingDataQualityMonitoringShifterInstructions#Run_signoff
  if system == "BTag":
    partitions["color"] = { } # No partition needed
                            
    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["BTAG"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = [] # No partition defect
                             
    defects0["partTol"] = [] 
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["BLAYER_SERIOUS_PROBLEM",
                             "BTAG_SCT_SERIOUS_PROBLEM",
                             "BTAG_TRT_SERIOUS_PROBLEM",
                             "BTAG_JET_SEVHOTSPOT"
                             ] 

    defects0["globTol"] = ["BEAMSPOT_SHIFT",
                           "BTAG_BLAYER_PROBLEM",
                           "BTAG_SCT_PROBLEM",
                           "BTAG_TRT_PROBLEM",
                           "NOJETS"] 


    defectVeto["description"] = {"BLAYER_SERIOUS_PROBLEM":"B layer problem",
                                 "BTAG_SCT_SERIOUS_PROBLEM":"SCT problem",
                                 "BTAG_TRT_SERIOUS_PROBLEM":"TRT problem",
                                 "BTAG_JET_SEVHOTSPOT":"Jet hot spot",
                                 "BEAMSPOT_SHIFT":"Beamspot shift",
                                 "BTAG_NOJETS":"No jets in monitoring"}

    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    signOff["EXPR."] = ['BTAG_UNCHECKED']
    signOff["BULK"] = ['BTAG_BULK_UNCHECKED']
    signOff["FINAL"] = []

#################################### TRIG_L1 defects
# https://twiki.cern.ch/twiki/bin/view/Atlas/DataQualityTriggerDefects
  if system == "Trig_L1":
    partitions["color"] = {}
    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["TRIG_L1"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = []
    defects0["partTol"] = []
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["CAL_coverage","CAL_misconf_calib","CAL_misconf_electronics","CAL_misconf_satBCID","CAL_off","MUB_busy","MUB_coverage","MUB_failed_electronics","MUB_lost_sync","MUB_misconf_electronics","MUE_busy","MUE_coverage","MUE_misconf_electronics","MUE_pt15GeV","MUE_pt20GeV","CTP_CTP_MuCTPI_bcid","CTP_CTP_ROD_bcid","CTP_CTPsim","CTP_TAPnoTBP","CTP_TAVnoTAP","CTP_UNKNOWN","CTP_bcid","CTP_bcidrange","CTP_candnumber","CTP_clock","CTP_counter","CTP_lumiblockrange","CTP_lumiblocktime","CTP_multpt","CTP_nanosectime","CTP_prescale_error","CTP_roiCand","CTP_roiNum","CTP_wrong_BGK","CTP_CTPIN_MU","CTP_CTPIN_JET2","TOPO_inputs","TOPO_outputs","TOPO_misconf_calib","TOPO_misconf_electronics","TOPO_off","TOPO_readout"] 
    defects0["globTol"] = ["CAL_coverage_tolerable","CAL_misconf_calib_tolerable","CAL_misconf_electronics_tolerable","CAL_misconf_satBCID_tolerable","CAL_misconf_tile_drawer","CAL_readout_cpjep_tolerable","CAL_readout_pp_tolerable","CAL_mistimed_larTBB_SEU_tolerable","CTP_NONSTANDARD_CONFIG","MUB_lost_sync_tolerable","MUB_failed_electronics_tolerable","MUB_coverage_tolerable","MUB_misconf_electronics_tolerable","MUB_LOWER_EFFICIENCY_TOLERABLE","MUE_coverage_tolerable","MUE_pt15GeV_tolerable","MUE_pt20GeV_tolerable","MUE_FakeBurst","TOPO_inputs_tolerable","TOPO_outputs_tolerable","TOPO_misconf_calib_tolerable","TOPO_misconf_electronics_tolerable","TOPO_readout_tolerable","TOPO_readout_roib","TOPO_not_good_for_physics"] 

    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    defectVeto["description"] = {"CAL_coverage":"",
                                 "CAL_misconf_calib":"",
                                 "CAL_misconf_electronics":"",
                                 "CAL_misconf_satBCID":"",
                                 "CAL_off":"",
                                 "CAL_mistimed_larTBB_SEU_tolerable":"LAr TBB SEU leading to mistiming",
                                 "MUB_busy":"MUB - busy",
                                 "MUB_coverage":"MUB - coverage",
                                 "MUB_failed_electronics":"MUB - electronics failure",
                                 "MUB_lost_sync":"MUB - lost sync",
                                 "MUB_misconf_electronics":"MUB - electronics misconfig",
                                 "MUE_busy":"MUE - busy",
                                 "MUE_coverage":"MUE - coverage",
                                 "MUE_misconf_electronics":"MUB - electronics misconfig",
                                 "MUE_pt15GeV":"MUE - PT15GeV",
                                 "MUE_pt20GeV":"MUE - PT20GeV"}

    signOff["EXPR."] = ["TRIG_L1_CAL_UNCHECKED","TRIG_L1_CTP_UNCHECKED","TRIG_L1_MUB_UNCHECKED","TRIG_L1_MUE_UNCHECKED","TRIG_L1_TOPO_UNCHECKED"]
    signOff["BULK"] = []
    signOff["FINAL"] = []
  

#################################### Trig_HLT defects
  if system == "Trig_HLT":
    partitions["color"] = {}
    partitions["list"] = list(partitions["color"])

    defects0["prefix"] = ["TRIG_HLT"]
    # Partition intolerable and tolerable defects - Order determines what defect is proeminent
    defects0["partIntol"] = []
    defects0["partTol"] = []
    # Global intolerable and tolerable defects
    defects0["globIntol"] = ["BJT_beam_spot_flag","BJT_no_secvtx","BJT_no_tracking","BJT_INACCURATE_ONLINE_BEAMSPOT","BJT_ONLINE_BEAMSPOT_GT1p6MM","BJT_ONLINE_BEAMSPOT_GT2MM","BJT_NO_MULTIBJET","BPH_no_muon","BPH_no_tracking","CAL_LAR_SourceMajor","CAL_TILE_SourceMajor","CAL_missing_data","ELE_no_clustering","ELE_no_tracking","ELE_primary_chain_misconfigured","ELE_unknown","ELE_tracking_issue","GAM_no_clustering","GAM_partial_clustering","GAM_primary_chain_misconfigured","GAM_unknown","GENERAL_debugstream","GENERAL_no_primaries","GENERAL_prescale_problem","GENERAL_standby","GENERAL_xpu_misconf","IDT_EF_FAIL","IDT_IDS_FAIL","IDT_SIT_FAIL","IDT_PRIVX_INEFF","JET_algo_problem","JET_menu_misconf","JET_unknown","MBI_no_tracking","MET_missing_data","MUO_Upstream_Barrel_problem","MUO_Upstream_Endcap_problem","TAU_misconf","TAU_caloIssue","TAU_nocalo","TAU_no_tracking","TRG_HLT_TAU_tracking_issue","TAU_dbIssue_BeamSpot"] 
    defects0["globTol"] = ["BJT_partial_tracking","BJT_unknown","BPH_algcrash","BPH_misconf","BPH_partial_muon","BPH_partial_tracking","BPH_unknown","CAL_LAR_SourceMinor","CAL_ROI_EXCESS","CAL_TILE_SourceMinor","CAL_partial_missing_data","CAL_spike","CAL_incorrect_BCID_correction","CAL_unknown","ELE_chain_misconfigured","ELE_clustering_issue","ELE_lowEfficiency_all_electrons","ELE_non_primary_poor_performance_e15_HLTtighter","ELE_non_primary_poor_performance_e15_tight","ELE_nonprimary_misconfigured","ELE_partial_clustering","ELE_partial_tracking","ELE_primary_poor_performance_e20_medium1","ELE_primary_poor_performance_e22_medium1","ELE_tracking_issue_Tolerable","GAM_chain_misconfigured","GAM_clustering_issue","GAM_nonprimary_misconfigured","GENERAL_streaming","GENERAL_tolerableDebugstream","GENERAL_no_1e34_primaries","GENERAL_no_12e33_primaries","GENERAL_no_15e33_primaries","GENERAL_no_17e33_primaries","IDT_BSPOT_FAILUR","IDT_BSPOT_INVALID_STATUS","IDT_BSPOT_INVALIDATOR_PROBLEM","IDT_EFT_FAIL","IDT_LOWSTAT","IDT_SCT_OUTOFTIMEHITS","IDT_TRT_DATA_LOST","IDT_TRT_OUTOFTIMEHITS","IDT_TSF_FAIL","IDT_unknown","JET_calib_issue","JET_energy_excess","JET_GSC_BEAMSPOT_PROBLEM","JET_hotspot","JET_partialscan_issue","MBI_HI_time_shift_mbts","MBI_partial_tracking","MBI_unknown","MBI_spacepoint_noise","MET_XS_Triggers_OFF","MET_missingEt_spike","MET_partial_missing_data","MET_phi_spike","MET_sumEt_spike","MET_unknown","MUO_EFMSonly_problem","MUO_Fullscan_problem","MUO_L2Iso_problem","MUO_L2muonSA_problem","MUO_MSonly_Barrel_problem","MUO_MSonly_Endcapl_problem","MUO_MuComb_problem","MUO_MuGirl_problem","MUO_Multi_Muon_problemchains","MUO_MuonEFTrackIso_problem","MUO_MuonEF_problem","MUO_Slow_problem","MUO_unknown","MUO_chain_misconfigured","TAU_unknown","TAU_dbIssue_mu","TAU_tracking_issue_Tolerable"]
    
    veto["all"] = [] # Veto name as defined in the COOL database
    veto["COOL"] = {} # Veto name as defined in the COOL database

    defectVeto["description"] = {"":""}

    signOff["EXPR."] = ["TRIG_HLT_BJT_UNCHECKED","TRIG_HLT_BPH_UNCHECKED","TRIG_HLT_CAL_UNCHECKED","TRIG_HLT_ELE_UNCHECKED","TRIG_HLT_GAM_UNCHECKED","TRIG_HLT_IDT_UNCHECKED","TRIG_HLT_JET_UNCHECKED","TRIG_HLT_MBI_UNCHECKED","TRIG_HLT_MET_UNCHECKED","TRIG_HLT_MUO_UNCHECKED","TRIG_HLT_TAU_UNCHECKED"]
    signOff["BULK"] = []
    signOff["FINAL"] = []


#########################################################################################
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

# If the description is not available, define it with the defect name
  for iDef in defects0["intol"]+defects0["tol"]:
    if iDef not in defectVeto["description"].keys():
      defectVeto["description"][iDef] = iDef

# Define color if not yet done
  if not ("color" in defectVeto):
    colors = [kBlue-4,kOrange-7,kTeal+1,kRed+1,kMagenta+2,kPink-3,kYellow+1,kGreen-2,kSpring-6,kViolet-4,kAzure-8,kCyan+1,
              kBlue-2,kOrange+1,kTeal+7,kRed+3,kMagenta-2,kPink+1,kYellow-1,kGreen+4,kSpring-2,kViolet+1,kAzure-2,kCyan-5,
              kBlue+2,kOrange+5,kTeal-4,kRed-5,kMagenta-6,kPink+6,kYellow-5,kGreen-6,kSpring+4,kViolet+6,kAzure+4,kCyan+4,]
    defectVeto["color"] = {}
    for iColor,iDefectVeto in enumerate(defectVeto["description"].keys()):
      if (iColor<len(colors)):
        defectVeto["color"][iDefectVeto] = colors[iColor]
      else:
        defectVeto["color"][iDefectVeto] = kBlack

# Similar tags
  for iSimilar in similarTags.keys():
    baseTag = iSimilar.split(".")[0]
    yearTag["description"][iSimilar] = similarTags[iSimilar]
    yearTag["defect"][iSimilar] = yearTag["defect"][baseTag] 
    if (baseTag in yearTag["veto"]):
      yearTag["veto"][iSimilar] = yearTag["veto"][baseTag] 

  return True

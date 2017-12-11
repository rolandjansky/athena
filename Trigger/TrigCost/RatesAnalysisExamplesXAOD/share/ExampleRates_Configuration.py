# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# This file is not a full JO. It is utilised in all ExampleRates JobOptions.
#
#---- User Options -------

if not "GridMode" in dir():
  GridMode = False

if not "EvtMax" in dir():
  EvtMax = -1

if not "DoHistograms" in dir():
  DoHistograms = True

if not "NormaliseHistograms" in dir(): # Switch this off when running on the grid. hadd and normalise once all the files are back.
  NormaliseHistograms = True

if not "OutputHistogramFile" in dir():
  OutputHistogramFile = "RatesHistograms.root"

if not "DoUniqueRates" in dir():
  DoUniqueRates = True

if not "DoRatesVsPositionInTrain" in dir():
  DoRatesVsPositionInTrain = False

if not "EnableLumiExtrapolation" in dir(): 
  EnableLumiExtrapolation = True

if not "TargetLuminosity" in dir(): # Only if EnableLumiExtrapolation=True. See RatesAnalysis/RatesAnalysisAlg.h for more sophisticated scaling
  TargetLuminosity = 1e34

if not "VetoStartOfTrain" in dir(): # Implies DoRatesVsPositionInTrain=True
  VetoStartOfTrain = 0

# To simulate the application of prescales, need a prescale XML from the RuleBook
# Make sure this is somewhere PathResolver can find it e.g. your current directory
if not "PrescaleXML" in dir():
  PrescaleXML = ""

# AOD files to process
if not "FilesInput" in dir():
  # Test file from AtlasHLT,21.0.15: data16_13TeV.00309640.physics_EnhancedBias.merge.AOD.r9083_r9084_p3008_tid10654269_00
  FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/data16_13TeV/8d/de/AOD.10654269._000566.pool.root.1"]

# If running over MC - we need the process cross section & filter efficiency of the sample
# You can either supply these directly OR the dataset name for auto-lookup
# If the dataset name is in the input files path, then it will be fetched from there
# Note to enable autolookup, first run "lsetup pyami; voms-proxy-init -voms atlas" and enter your grid pass phrase
if not "MCDatasetName" in dir(): # Set manually or make sure it is contained in the file path
  MCDatasetName = ""

# Alternatively, supply the values by hand for MC
if not "MCCrossSection" in dir(): # Cross section of process in nb
  MCCrossSection = 0
if not "MCFilterEfficiency" in dir(): # Filter efficiency of any MC filter (0.0 - 1.0)
  MCFilterEfficiency = 0
if not "MCKFactor" in dir(): # Additional fudge-factor to apply to account for higher order corrections.  
  MCKFactor = 1

if not "ForcePOOL" in dir(): # If hybrid mode is failing for some reason (true at the moment for 21.X)
  ForcePOOL = False

#---- Setup -------

if GridMode:
  NormaliseHistograms = False

if VetoStartOfTrain > 0:
  DoRatesVsPositionInTrain = True

jps.AthenaCommonFlags.FilesInput = FilesInput
from PyUtils import AthFile
af = AthFile.fopen(FilesInput[0]) 
isMC = ('IS_SIMULATION' in af.fileinfos['evt_type'])

if isMC and (not MCCrossSection or not MCFilterEfficiency): # If the input file is MC then make sure we have the needed info
  from RatesAnalysisExamplesXAOD.RatesGetCrossSectionMC import GetCrossSectionAMI
  amiTool = GetCrossSectionAMI()
  if not MCDatasetName: # Can we get the dataset name from the input file path?
    MCDatasetName = amiTool.getDatasetNameFromPath(FilesInput[0])
  amiTool.queryAmi(MCDatasetName)
  MCCrossSection = amiTool.getCrossSection()
  MCFilterEfficiency = amiTool.getFilterEfficiency()

#---- ESD Flag -------

isESD = ('ESD' in FilesInput[0])

#---- Set read-mode -------

# xAOD hybrid mode is faster - but is not compatible with reading BICD position in train from DB or ESD files
if DoRatesVsPositionInTrain or isESD or ForcePOOL: import AthenaPoolCnvSvc.ReadAthenaPool #sets up reading of POOL files (e.g. xAODs)
else:                                              import AthenaRootComps.ReadAthenaxAODHybrid #alternative for FAST xAOD reading!

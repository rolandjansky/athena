#--------------------------------------------------------------
# Standard Athena Stuff
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

from AthenaCommon.AppMgr      import theApp
from AthenaCommon.AppMgr      import ServiceMgr as svcMgr
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()

#--------------------------------------------------------------
# Personal JobOptions, Input files, env variables and Flags
#--------------------------------------------------------------
doMyAANT           = True     # Produce AANT?
MyOutputLevel      = DEBUG    # Default output level for algorithms

doRTTEventInfo     = True
doPileupValidation = True
doPixelOccupancy   = True
doSCTOccupancy     = True
doMCEventInfo      = True

doMCEventCheck     = True
#doWriteMCCheck     = True

doWriteMcInfo      = True     # Write out Mc Info File?
doAllPixels        = False    # Save all pixel information?

# Flags
readESD            = False
readAOD            = False
doWriteESD         = False
doWriteAOD         = False
doJiveXML          = False
doTruth            = True

if not vars().has_key('McEventCollection'): McEventCollection = "GEN_AOD"

#--------------------------------------------------------------
# Athena-Aware NTuple making Tools
#--------------------------------------------------------------
#CBNTAthenaAware = True
#include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
#include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

#--------------------------------------------------------------
# Histo & ROOT Service
#--------------------------------------------------------------
if doMyAANT:

  from GaudiSvc.GaudiSvcConf import THistSvc
  svcMgr += THistSvc()
  svcMgr.THistSvc.Output = ["AANT DATAFILE='PileupDigiRTT.ntuple.root' OPT='RECREATE'"]

  from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
  topSequence                                    += AANTupleStream()
  topSequence.AANTupleStream.ExtraRefNames        = [ "StreamESD","StreamRDO" ]
  topSequence.AANTupleStream.OutputName           = 'PileupDigiRTT.ntuple.root'
  topSequence.AANTupleStream.WriteInputDataHeader = True
  topSequence.AANTupleStream.OutputLevel          = MyOutputLevel

#--------------------------------------------------------------
# Get CBNT setup
#--------------------------------------------------------------
#from CBNT_Athena.CBNT_AthenaConf import CBNT_AthenaAware


#--------------------------------------------------------------
# PileupSimpleEventInfo: 
#     What CBNT_Athena/CBNT_EventInfo_jobOptions.py used to do
#--------------------------------------------------------------
if doRTTEventInfo:
  from PileupRTT.PileupRTTConf import PileupRTT__PileupSimpleEventInfo
  topSequence += PileupRTT__PileupSimpleEventInfo()
  PileupRTT__PileupSimpleEventInfo  = PileupRTT__PileupSimpleEventInfo()
  PileupRTT__PileupSimpleEventInfo.OutputLevel       = MyOutputLevel  # Output message Level

  from AthenaCommon.AppMgr import ServiceMgr
  if not hasattr(ServiceMgr, 'THistSvc'):
      from GaudiSvc.GaudiSvcConf import THistSvc
      ServiceMgr += THistSvc()
      ServiceMgr.THistSvc.Output+=[ "AANT DATAFILE='PileupDigiRTT.ntuple.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Pileup Validation: 
#     EventInfo checks, including event recycling monitoring
#--------------------------------------------------------------
if doPileupValidation:
  from PileupRTT.PileupRTTConf import PileupRTT__PileupValidation
  topSequence += PileupRTT__PileupValidation()
  PileupRTT__PileupValidation   = PileupRTT__PileupValidation()
  PileupRTT__PileupValidation.OutputLevel         = MyOutputLevel   # Output message Level
  PileupRTT__PileupValidation.PileUpEventInfo     = "OverlayEvent"  # StoreGate Key for PileUpEventInfo
  PileupRTT__PileupValidation.EventInfo           = "McEventInfo"   # StoreGate Ket for the EventInfo

  from AthenaCommon.AppMgr import ServiceMgr
  if not hasattr(ServiceMgr, 'THistSvc'):
      from GaudiSvc.GaudiSvcConf import THistSvc
      ServiceMgr += THistSvc()
      ServiceMgr.THistSvc.Output+=[ "AANT DATAFILE='PileupDigiRTT.ntuple.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Pileup Pixel Occupancy: 
#     Measure pixel occupancy in pileup events
#--------------------------------------------------------------
if doPixelOccupancy:
  from PileupRTT.PileupRTTConf import PileupRTT__PileupPixelOccupancy
  topSequence += PileupRTT__PileupPixelOccupancy()
  PileupRTT__PileupPixelOccupancy = PileupRTT__PileupPixelOccupancy()
  PileupRTT__PileupPixelOccupancy.OutputLevel     = MyOutputLevel   # Output message Level
  PileupRTT__PileupPixelOccupancy.DoAllPixels     = doAllPixels     # Store all pixel information in tree
  PileupRTT__PileupPixelOccupancy.PIX_RDOName     = "PixelRDOs"     # StoreGate Key for Pixel hits

  from AthenaCommon.AppMgr import ServiceMgr
  if not hasattr(ServiceMgr, 'THistSvc'):
      from GaudiSvc.GaudiSvcConf import THistSvc
      ServiceMgr += THistSvc()
      ServiceMgr.THistSvc.Output+=[ "AANT DATAFILE='PileupDigiRTT.ntuple.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Pileup SCT Occupancy: 
#     Measure SCT occupancy in pileup events
#--------------------------------------------------------------
if doSCTOccupancy:
  from PileupRTT.PileupRTTConf import PileupRTT__PileupSCTOccupancy
  topSequence += PileupRTT__PileupSCTOccupancy()
  PileupRTT__PileupSCTOccupancy = PileupRTT__PileupSCTOccupancy()
  PileupRTT__PileupSCTOccupancy.OutputLevel     = MyOutputLevel   # Output message Level
  PileupRTT__PileupSCTOccupancy.SCT_RDOName     = "SCT_RDOs"      # StoreGate Key for SCT hits

#--------------------------------------------------------------
# Pileup MC Event Info: 
#     Details of MC Truth Info in pileup events
#--------------------------------------------------------------
if doMCEventInfo:
  from PileupRTT.PileupRTTConf import PileupRTT__PileupMCEventInfo
  topSequence += PileupRTT__PileupMCEventInfo()
  PileupRTT__PileupMCEventInfo  = PileupRTT__PileupMCEventInfo()
  PileupRTT__PileupMCEventInfo.OutputLevel       = MyOutputLevel  # Output message Level
  PileupRTT__PileupMCEventInfo.McEventCollection = McEventCollection   # StoreGate Key for SCT hits
  PileupRTT__PileupMCEventInfo.McOutFile         = "MCEventInfo.txt" # Output file for truth tree
  PileupRTT__PileupMCEventInfo.dumpMcInfo        = doWriteMcInfo  # Write out Mc Info?

  from AthenaCommon.AppMgr import ServiceMgr
  if not hasattr(ServiceMgr, 'THistSvc'):
      from GaudiSvc.GaudiSvcConf import THistSvc
      ServiceMgr += THistSvc()
      ServiceMgr.THistSvc.Output+=[ "AANT DATAFILE='PileupDigiRTT.ntuple.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Pileup MC Event Collection Checker: 
#     Looks for the signal GenEvent in each MCEventCollection
#--------------------------------------------------------------
if doMCEventCheck:
  from PileupRTT.PileupRTTConf import PileupRTT__PileupMCEventCollectionChecker
  topSequence += PileupRTT__PileupMCEventCollectionChecker("PileupMCEventCollectionChecker",
                                 OutputLevel = MyOutputLevel)
  topSequence.PileupMCEventCollectionChecker.McEventCollection = McEventCollection
  #PileupRTT__PileupMCEventCollectionChecker  = PileupRTT__PileupMCEventCollectionChecker()
  #PileupRTT__PileupMCEventCollectionChecker.OutputLevel       = MyOutputLevel  # Output message Level
  #PileupRTT__PileupMCEventCollectionChecker.McEventCollection = "TruthEvent"   # StoreGate Key for SCT hits
  
  from AthenaCommon.AppMgr import ServiceMgr
  if not hasattr(ServiceMgr, 'THistSvc'):
      from GaudiSvc.GaudiSvcConf import THistSvc
      ServiceMgr += THistSvc()
      ServiceMgr.THistSvc.Output+=[ "AANT DATAFILE='PileupDigiRTT.ntuple.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Pileup Validation Tools: 
#     Set output level for helper tools
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc
from PileupRTT.PileupRTTConf import PileupRTT__PileupValidationTools
PileupRTT__PileupValidationTools = PileupRTT__PileupValidationTools("PileupValidationTools")
PileupRTT__PileupValidationTools.OutputLevel = MyOutputLevel  # Output message Level
ToolSvc += PileupRTT__PileupValidationTools

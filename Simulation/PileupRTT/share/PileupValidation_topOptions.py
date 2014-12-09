#**************************************************************
#
# Job Options For Pileup RDO Validation
#
# AUTHOR      : David W. Miller
# EMAIL       : David.W.Miller@cern.ch
# DATE        : 16 April 2007
# INSTITUTION : SLAC
#
#==============================================================

#----------------------------------------
# Return a file list given a directory
#----------------------------------------
import os,commands

def getDirectoryList(path):
    cmd  = 'ls %s/' % (path)
    file = ["%s/%s" % (path,i) for i in commands.getoutput(cmd).split('\n')]
    return file

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
# POOL Catalog Patch
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
svcMgr.PoolSvc.AttemptCatalogPatch = True

#--------------------------------------------------------------
# Personal JobOptions, Input files, env variables and Flags
#--------------------------------------------------------------
doMyAANT           = True     # Produce AANT?
MyOutputLevel      = INFO    # Default output level for algorithms

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

#--------------------------------------------------------------
# Particle Properties, Dictionaries and Converters
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
#include( "PartPropSvc/PartPropSvc.py" )
#include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Setup geometry
#--------------------------------------------------------------
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'geant4'
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.ID_setOn()
globalflags.DetDescrVersion = 'ATLAS-GEO-16-00-00'
globalflags.ConditionsTag.set_Value_and_Lock( 'OFLCOND-SDR-BS7T-04-13' )
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

#--------------------------------------------------------------
# Input files
#--------------------------------------------------------------

#svcMgr.EventSelector.InputCollections = [ "PileupDigiRTTOutput.RDO.pool.root" ]
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput.get_Value()
#--------------------------------------------------------------
# Athena-Aware NTuple making Tools
#--------------------------------------------------------------
#CBNTAthenaAware = True
#include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
#include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

#--------------------------------------------------------------
# Number of Events to process
#--------------------------------------------------------------

theApp.EvtMax = -1

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
# Pileup Validation: 
#     EventInfo checks, including event recycling monitoring
#--------------------------------------------------------------
if doPileupValidation:
  from PileupRTT.PileupRTTConf import PileupRTT__PileupValidation
  #topSequence.CBNT_AthenaAware += PileupRTT__PileupValidation()
  PileupRTT__PileupValidation   = PileupRTT__PileupValidation()
  PileupRTT__PileupValidation.OutputLevel         = MyOutputLevel   # Output message Level
  PileupRTT__PileupValidation.PileUpEventInfo     = "OverlayEvent"  # StoreGate Key for PileUpEventInfo
  PileupRTT__PileupValidation.EventInfo           = "McEventInfo"   # StoreGate Ket for the EventInfo

#--------------------------------------------------------------
# Pileup Pixel Occupancy: 
#     Measure pixel occupancy in pileup events
#--------------------------------------------------------------
if doPixelOccupancy:
  from PileupRTT.PileupRTTConf import PileupRTT__PileupPixelOccupancy
  #topSequence.CBNT_AthenaAware   += PileupRTT__PileupPixelOccupancy()
  PileupRTT__PileupPixelOccupancy = PileupRTT__PileupPixelOccupancy()
  PileupRTT__PileupPixelOccupancy.OutputLevel     = MyOutputLevel   # Output message Level
  PileupRTT__PileupPixelOccupancy.DoAllPixels     = doAllPixels     # Store all pixel information in tree
  PileupRTT__PileupPixelOccupancy.PIX_RDOName     = "PixelRDOs"     # StoreGate Key for Pixel hits

#--------------------------------------------------------------
# Pileup SCT Occupancy: 
#     Measure SCT occupancy in pileup events
#--------------------------------------------------------------
if doSCTOccupancy:
  from PileupRTT.PileupRTTConf import PileupRTT__PileupSCTOccupancy
  #topSequence.CBNT_AthenaAware += PileupRTT__PileupSCTOccupancy()
  PileupRTT__PileupSCTOccupancy = PileupRTT__PileupSCTOccupancy()
  PileupRTT__PileupSCTOccupancy.OutputLevel     = MyOutputLevel   # Output message Level
  PileupRTT__PileupSCTOccupancy.SCT_RDOName     = "SCT_RDOs"      # StoreGate Key for SCT hits

#--------------------------------------------------------------
# Pileup MC Event Info: 
#     Details of MC Truth Info in pileup events
#--------------------------------------------------------------
if doMCEventInfo:
  from PileupRTT.PileupRTTConf import PileupRTT__PileupMCEventInfo
  #topSequence.CBNT_AthenaAware += PileupRTT__PileupMCEventInfo()
  PileupRTT__PileupMCEventInfo  = PileupRTT__PileupMCEventInfo()
  PileupRTT__PileupMCEventInfo.OutputLevel       = MyOutputLevel  # Output message Level
  PileupRTT__PileupMCEventInfo.McEventCollection = "TruthEvent"   # StoreGate Key for SCT hits
  PileupRTT__PileupMCEventInfo.McOutFile         = "MCEventInfo.txt" # Output file for truth tree
  PileupRTT__PileupMCEventInfo.dumpMcInfo        = doWriteMcInfo  # Write out Mc Info?

#--------------------------------------------------------------
# Pileup MC Event Collection Checker: 
#     Looks for the signal GenEvent in each MCEventCollection
#--------------------------------------------------------------
if doMCEventCheck:
  from PileupRTT.PileupRTTConf import PileupRTT__PileupMCEventCollectionChecker
  topSequence += PileupRTT__PileupMCEventCollectionChecker("PileupMCEventCollectionChecker",
                                 OutputLevel = MyOutputLevel)
  topSequence.PileupMCEventCollectionChecker.McEventCollection="TruthEvent"
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

#--------------------------------------------------------------
# output level threshold and limit
#--------------------------------------------------------------
# (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )

svcMgr.MessageSvc.OutputLevel  = MyOutputLevel
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages

StoreGateSvc      = Service( "StoreGateSvc" )
StoreGateSvc.Dump = False  #true will dump data store contents

#--------------------------------------------------------------
# Hack to get multiple ATHENA jobs to run on diff. inputs
#  * Essentially we are just renaming the eventselector
#  * from David R.
#  * RecExRecoTest/rttchainhack.py
#  * See also RTT: CaloAnaEx
#--------------------------------------------------------------

# hack starts here:

class DummyEventSelector   :
    InputCollections= []

class DummyServiceMgr  :
    EventSelector= DummyEventSelector

#create a dummy ServiceMgr
ServiceMgr=DummyServiceMgr()

#hack ends here



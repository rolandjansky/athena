#--------------------------------------------------------------
# JobOpt for muon sim hits Validation with :
# MuonHitTest
# ATLAS-GEO-08-00-02 Layout
# mu_pt_10GeV
# Dec. 2009
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags 
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ServiceMgr

#--------------------------------------------------------------
# Control - standard options (as in jobOptions.py)
#--------------------------------------------------------------
 
# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
doAuditors      = True
doNameAuditor   = True
doTimeLimit     = True
#--------------------------------------------------------------
import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False
#--------------------------------------------------------------------
# include( "AthenaCommon/AthenaCommonFlags.py" )
from AthenaCommon.DetFlags import DetFlags
# --- switch on MuonDetector
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOn()
# --- and switch off all the rest
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Print()
# ----- End of Flags section
#------------------------------------------------------------
# GeoModel Initialization
#--- GeoModel stuff -----------------------------------------
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
from AtlasGeoModel import GeoModelInit
#Set geometry tag
from AtlasGeoModel import SetGeometryVersion, GeoModelInit
GeoModelSvc = ServiceMgr.GeoModelSvc
GeoModelSvc.IgnoreTagDifference = True
#--------------------------------------------------------------
# Options for reading from POOL (use POOL implicit collection)
#--------------------------------------------------------------
include("MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py")
#--------------------------------------------------------------
# Pool input
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
#--------------------------------------------------------------
## the POOL converters
#--------------------------------------------------------------
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
theApp.Dlls   += [ "GeneratorObjectsAthenaPoolPoolCnv"]
#--------------------------------------------------------------
# to initialize converters for TracRecord(truth tracks info)
# hit AthenaPool converters 
#--------------------------------------------------------------
include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )
#--------------------------------------------------------------
# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc

globalflags.DetDescrVersion = "ATLAS-GEO-18-01-03" 
###############################################################
# End of user settings
###############################################################
if 'doReadBS' not in dir():
  doReadBS = False
 
# for backwards compatibility
DetDescrVersion = globalflags.DetDescrVersion()
GeoModelSvc.AtlasVersion = DetDescrVersion  
#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------
 
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
#from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_Value_and_Lock(atlas)

# --- set defaults
globalflags.DataSource = 'geant4'
if doReadBS:
  globalflags.InputFormat = 'bytestream'
else:
  globalflags.InputFormat = 'pool'
 
if not doReadBS:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  ServiceMgr.EventSelector.InputCollections += [ "SimHits_GEO-18-01-03.pool.root" ]
  
print 'DetDescrVersion = %r' % globalflags.DetDescrVersion()
  
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
if doAuditors:
  theApp.Dlls += [ "GaudiAud" ]
  # load AuditorSvc
  # write out a summary of the time spent
  from GaudiSvc.GaudiSvcConf import AuditorSvc
  ServiceMgr += AuditorSvc()
  theAuditorSvc = ServiceMgr.AuditorSvc
  #theAuditorSvc = AuditorSvc()
  # write out summary of the memory usage
  #   number of events to be skip to detect memory leak
  #   20 is default. May need to be made larger for complete jobs.
  theAuditorSvc.Auditors += [ "ChronoAuditor"]
  ServiceMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 5
  # memory auditors
  theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
  MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
  MemStatAuditor.OutputLevel = WARNING
  
# write out a short message upon entering or leaving each algorithm
if doNameAuditor:
  theAuditorSvc.Auditors     += [ "NameAuditor" ]
  theApp.AuditAlgorithms = True
  theApp.AuditServices   = True

from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
ServiceMgr += AthenaPoolCnvSvc()
ServiceMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

#-------------------------------------------------------------- 
ServiceMgr.StoreGateSvc.Dump = False  #true will dump data store contents
#-------------------------------------------------------------- 
# Number of events to be processed (default is 10)
theApp.EvtMax = -1
theApp.EvtSel = "EventSelector"
ServiceMgr.EventSelector.SkipEvents = 0
#--------------------------------------------------------------
# Configuration of MuonHitValAlg subalgorithms properties
#-------------------------------------------------------------- 
# get a handle on the job main sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from MuonHitTest.MuonHitTestConf import MuonHitValAlg,MDTHitsValAlg,RPCHitsValAlg,CSCHitsValAlg,TGCHitsValAlg
MuonHitValAlg=MuonHitValAlg()
#--------------------------------------------------------------
# declaration of MuonHitValAlg subAlg
#--------------------------------------------------------------
#Truth
#MuonHitValAlg.SubAlgorithms = [ "TruthValAlg/MyTruth" ]
#--------------------------------------------------------------
# declaration of MDTHitsValAlg subAlg
#--------------------------------------------------------------
MuonHitValAlg.SubAlgorithms += [ "MDTHitsValAlg" ]
MuonHitValAlg += MDTHitsValAlg( WriteNtuple = True, 
                                BarrelEtaCut = 1.,   
                                ChamberName = "XXX", ##ChamberName: Input "XXX" for all available chambers or type - the hardware name for one specific chamber
                                StationSize = "XXX", ##StationSize: Input "XXX" for both Large and Small chambers or type - "L" for Large and "S" for Small
                                StationEta = -100, # BL1A  ##StationEta/Phi: Input -100 for all chambers or type
                                StationPhi = -100 ,# BL1A   ##a specific station eta/phi (both constrained, then is a projective tower)
                                SpecRegion = 0, # 0:All regions, 1:Barrel region, 2:Overlap region, 3:Endcap region
                                WriteChamberGraphs=False,
                                LastEvent = theApp.EvtMax )
#--------------------------------------------------------------
# declaration of RPCHitsValAlg subAlg
#--------------------------------------------------------------
MuonHitValAlg.SubAlgorithms += [ "RPCHitsValAlg" ]
MuonHitValAlg += RPCHitsValAlg( WriteNtuple = True,
                                ChamberName = "XXX",  ##ChamberName: Input "XXX" for all available chambers or type - the hardware name for one specific chamber
                                StationSize = "XXX",  ##StationSize: Input "XXX" for both Large and Small chambers or type - "L" for Large and "S" for Small
                                StationEta = -100, # BL1A   ##StationEta/Phi: Input -100 for all chambers or type
                                StationPhi = -100, # BL1A   ##a specific station eta/phi (both constrained, then is a projective tower)
                                WriteChamberGraphs=False,
                                LastEvent = theApp.EvtMax )
#--------------------------------------------------------------
# declaration of CSCHitsValAlg subAlg
#--------------------------------------------------------------
MuonHitValAlg.SubAlgorithms += [ "CSCHitsValAlg" ]
MuonHitValAlg += CSCHitsValAlg( WriteNtuple = True,
                                WriteChamberGraphs=False,
                                LastEvent = theApp.EvtMax )
#--------------------------------------------------------------
# declaration of TGCHitsValAlg subAlg
#--------------------------------------------------------------
MuonHitValAlg.SubAlgorithms += [ "TGCHitsValAlg" ]
MuonHitValAlg += TGCHitsValAlg( WriteNtuple = True, 
                                WriteChamberGraphs=False,
                                LastEvent = theApp.EvtMax )
#-------------------------------------------------------------- 
#Muon Track Records (Truth)
#MuonHitValAlg.SubAlgorithms += [ "MuonTrackRecordValAlg/MuonTrackRecord" ]
topSequence += MuonHitValAlg 
#-------------------------------------------------------------- 
THistSvc = Service( "THistSvc")
sampletype='Hits';
atlastag='GEO-18-01-03';
RootHistOutputFileName = 'MuonHitTest_'+str(atlastag)+'.'+str(sampletype)+'.root' 
RootTupleOutputFileName = 'MuonHitTest_'+str(atlastag)+'.'+str(sampletype)+'.aan.root' 
#-------------------------------------------------------------- 
# histograms
#-------------------------------------------------------------- 
if os.path.exists(RootHistOutputFileName):
  os.remove(RootHistOutputFileName)
#--------------------------------------------------------------  
# Writing the histogram files from each sub-algorithm
#--------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output += ["muonhits DATAFILE='%s' OPT='NEW'" % RootHistOutputFileName ] 
#--------------------------------------------------------------
#-------------------------------------------------------------- 
# ntuples: AANT  (Athena Aware NTuples)
#-------------------------------------------------------------- 
if os.path.exists(RootTupleOutputFileName):
  os.remove(RootTupleOutputFileName)

ServiceMgr.THistSvc.Output += ["MuonHits DATAFILE='%s' OPT='NEW'" % RootTupleOutputFileName]
#--------------------------------------------------------------------------- 
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------------------- 
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999
ServiceMgr.MessageSvc.useColors = True
#--------------------------------------------------------------
# Application: load TagInfoMgr
#--------------------------------------------------------------
include( "EventInfoMgt/EventInfoMgt_jobOptions.py" )
TagInfoMgr = Service( "TagInfoMgr" )
TagInfoMgr.OutputLevel = INFO
#--------------------------------------------------------------
# Dump configuration to file
#--------------------------------------------------------------
from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt") 
#--------------------------------------------------------------
print "List of all top algorithms"
print theApp.TopAlg
#================================================================

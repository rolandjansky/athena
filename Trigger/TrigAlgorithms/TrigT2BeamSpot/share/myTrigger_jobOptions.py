#----------------------------------------
# Return a file list given a directory
#----------------------------------------

import os,commands

def getDirectoryList(path):
    cmd  = 'ls %s/' % (path)
    file = ["%s/%s" % (path,i) for i in commands.getoutput(cmd).split('\n')]
    return file

def getMixedDirectoryList(path):
    cmd  = 'ls %s/*.root*' % (path)
    file = ["%s" % (i) for i in commands.getoutput(cmd).split('\n')]
    return file    
    
def getCastorDirectoryList(path):
    cmd  = 'nsls %s/ ' % (path)
    file = ["rfio:%s/%s" % (path,i) for i in commands.getoutput(cmd).split('\n')]
    return files

#-----------------
# Input/Output
#-----------------

if ('Data' in dir()):
    if Data == "shift":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/ttbar_shift')
    elif Data == "gashalo":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/gashalo')
    elif Data == "mixed":
        PoolRDOInput  = getMixedDirectoryList('/u1/iaracena/data/MixedEventsEnhancedb/RDO')
    elif Data == "J2":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J2')
    elif Data == "J3":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J3_2')
    elif Data == "J1":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J1')
    elif Data == "J4":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J4') 
    elif Data == "J0":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J0')
else:
    Data = "shift"
    PoolRDOInput  = getDirectoryList('/u1/fizisist/ttbar_shift')

#PoolRDOInput=["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]
#PoolRDOInput  = ["rfio:/castor/cern.ch/user/f/fizisist/beamspot/misal1_csc11.005013.J4_pythia_jetjet.digit.RDO.v12003103_tid004016._02277.pool.root.1"]

#PoolRDOInput  = getCastorDirectoryList('/castor/cern.ch/user/f/fizisist/beamspot/ttbarShift')
#PoolRDOInput  = getCastorDirectoryList('/castor/cern.ch/grid/atlas/caf/mc12/mc12.005012.J3_pythia_jetjet.digit.RDO.v12000003')
#PoolRDOInput  = getCastorDirectoryList('/castor/cern.ch/grid/atlas/dq2/misal1_csc11/misal1_csc11.005013.J4_pythia_jetjet.digit.RDO.v12003103_tid004016')
#readRDOBS=["/u1/iaracena/data/14.0.X/Mixed_lumi1E31/daq.csc13.0000000.Single.Stream.LB0000.Athena._0001.data"]
#PoolRDOInput  = getDirectoryList('/u1/fizisist/ttbar_shift')
#PoolRDOInput  = getMixedDirectoryList('/u1/iaracena/data/MixedEventsEnhancedb')
#PoolRDOInput  = getDirectoryList('/u1/fizisist/J2')
#PoolRDOInput  = getDirectoryList('/u1/fizisist/J3') 
           
PoolESDOutput = 'rfio:/castor/cern.ch/user/f/fizisist/beamspot/myTriggerTest2.mixed.ESD.pool.root'
PoolAODOutput = 'rfio:/castor/cern.ch/user/f/fizisist/beamspot/myTriggerTest2.mixed.AOD.pool.root'

if not ('EvtMax' in dir()):                                   
    EvtMax=1000

NEvents = "%d" % EvtMax

#-----------------
# Global Flags
#-----------------
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.Print()
#GlobalFlags.DetDescrVersion ="ATLAS-CSC-01-02-00"
#DetDescrVersion = "ATLAS-CSC-01-02-00"

#-----------------
# Global Setup
#-----------------
doTAG          = False
doEgamma       = False
doMuon         = False
doJet          = False
doTauRec       = False
doObjMissingET = False
doMuGirl       = False
doMoore        = False
doMuonIDStandAlone = False
doMuonIDCombined   = False 

doCBNT     = False
doWriteAOD = False
doWriteESD = False
doWriteTAG = False
doAOD      = False 
doESD      = False 
doTAG      = False
doTruth    = False
doAtlfast  = False

#-----------------
# Trigger Setup
#-----------------
doTrigger           = True
TriggerModernConfig = True

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_flags.py")
#-----------------------------------------------------------

#-----------------
# InDet Flags
#-----------------
from InDetRecExample.InDetJobProperties import InDetFlags
from AthenaCommon.JobProperties import jobproperties
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
if Data == "gashalo":
  InDetFlags.Enabled                 = True
  InDetFlags.doBeamGas               = True
  InDetFlags.doBeamHalo              = True
  InDetFlags.doSiSPSeededTrackFinder = True
  InDetFlags.doPrintConfigurables    = True
  InDetTrigFlags.doBeamGas  = True
  InDetTrigFlags.doBeamHalo = True

#-----------------
# Trigger Flags
#-----------------

TriggerFlags.doCalo = True
TriggerFlags.doMuon = True
TriggerFlags.doID   = True
TriggerFlags.doLVL2 = True
TriggerFlags.doLVL1 = True
TriggerFlags.doEF   = False
TriggerFlags.readHLTconfigFromXML  = False
TriggerFlags.readLVL1configFromXML = False
TriggerFlags.Slices_all_setOn()
TriggerFlags.doNtuple         = True
TriggerFlags.enableMonitoring = [ 'Validation', 'Time', ]
TriggerFlags.triggerMenuSetup = 'default'
TriggerFlags.abortOnConfigurationError = True
TriggerFlags.doHypo = True
TriggerFlags.doFEX  = True

#-----------------
# SiTrack Setup
#-----------------

#from TrigSiTrack.TrigSiTrack_Config import *
#if ('SiTrackOpt' in dir()):
#    if SiTrackOpt == "loose.v2":
#        TrigSiTrackFlags.LoosenCuts = True
#    else:
#        TrigSiTrackFlags.LoosenCuts = False 
#else:
SiTrackOpt = "defaults"
#    TrigSiTrackFlags.LoosenCuts = False
    
#-----------------
# Beamspot Setup
#-----------------

from TriggerMenuPython.BeamSpotSliceFlags import *
BeamSpotSliceFlags.unsetAll()
TriggerFlags.BeamSpotSlice.signatures = [ 'vertex_bjet18' ]

from TrigT2BeamSpot.TrigT2BeamSpotConf import PESA__T2VertexBeamSpot

# Track Algorithm: SiTrack = 1, IdScan = 2
if ('TrackType' in dir()):
    if TrackType == "SiTrack":
        PESA__T2VertexBeamSpot.TrackAlgoID = 1
    else:
        PESA__T2VertexBeamSpot.TrackAlgoID = 2 
else:
    TrackType = "IdScan"
    PESA__T2VertexBeamSpot.TrackAlgoID = 2

PESA__T2VertexBeamSpot.UseBeamConditionService = True
PESA__T2VertexBeamSpot.BeamCondSvcName         = "BeamCondSvc" 

PESA__T2VertexBeamSpot.DoPerLBHistograms       = True 
PESA__T2VertexBeamSpot.LBHistoryLength         = 10
PESA__T2VertexBeamSpot.MonitoringHistoPath     = "/EXPERT/T2VertexBeamSpot_Fex/"
PESA__T2VertexBeamSpot.ExpressHistoPath        = "/EXPERT/T2VertexBeamSpot_Fex/"

PESA__T2VertexBeamSpot.HistoTrackAlgs          = False
PESA__T2VertexBeamSpot.SaveAllVertices         = False
    
PESA__T2VertexBeamSpot.TotalNTrackMin          = 6
PESA__T2VertexBeamSpot.TrackMinPt              = 1.0
PESA__T2VertexBeamSpot.TrackSeedPt             = 4.0
PESA__T2VertexBeamSpot.TrackMaxZ0              = 200.0
PESA__T2VertexBeamSpot.TrackMaxD0              = 10.0
PESA__T2VertexBeamSpot.TrackMaxZ0err           = 1.0
PESA__T2VertexBeamSpot.TrackMaxD0err           = 1.0
PESA__T2VertexBeamSpot.TrackMinQual            = 0.0
PESA__T2VertexBeamSpot.TrackMaxQual            = 10.0
PESA__T2VertexBeamSpot.TrackMinSiHits          = 5
PESA__T2VertexBeamSpot.TrackMinTRTHits         = -1

PESA__T2VertexBeamSpot.VertexMinNTrk           = 6
PESA__T2VertexBeamSpot.VertexMaxNTrk           = 30
PESA__T2VertexBeamSpot.VertexMaxXerr           = 0.1
PESA__T2VertexBeamSpot.VertexMaxYerr           = 0.1
PESA__T2VertexBeamSpot.VertexMaxQual           = 30.0

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["SHIFT DATAFILE='shift-monitoring.root' OPT='RECREATE'", 
                              "EXPERT DATAFILE='/u1/fizisist/beamspot/rootHistos/expert-monitoring.b18." + SiTrackOpt + "." + Data + "." + NEvents + "evt." + TrackType + ".root' OPT='RECREATE'", 
                              "RUNSTAT DATAFILE='runstat-monitoring.root' OPT='RECREATE'", 
                              "DEBUG DATAFILE='debug-monitoring.root' OPT='RECREATE'"]

jobproperties.PerfMonFlags.doMonitoring = False

Service ("StoreGateSvc" ).ActivateHistory = True

print AlgSequence
print ServiceMgr

if not ('OutputLevel' in dir()):
    OutputLevel = INFO 
    
AlgSequence().TrigSteer_L2.OutputLevel = OutputLevel
for algo in AlgSequence().TrigSteer_L2.getChildren():
   algo.OutputLevel = INFO 

MessageSvc.debugLimit  = 1000000
MessageSvc.OutputLevel = OutputLevel
MessageSvc.Format      = "% F%48W%S%7W%R%T %0W%M"

ServiceMgr.StoreGateSvc.Dump = True

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
# Input files
#-----------------
if ('Data' in dir()):
    if Data == "shift":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/ttbar_shift')
    elif Data == "gashalo":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/gashalo')
    elif Data == "mixed":
        PoolRDOInput  = getMixedDirectoryList('/u1/iaracena/data/MixedEventsEnhancedb/RDO')
    elif Data == "EnhancedBias":
        PoolRDOInput  = getMixedDirectoryList('/u1/data/user09.SethCaughron.EnhancedBias_10TeV.digit.RDO.e357_s462_d126') 
    elif Data == "J2":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J2')
    elif Data == "J3":
        PoolRDOInput  = getDirectoryList('/u1/data/J3_RDO_rel12')
    elif Data == "J1":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J1')
    elif Data == "J4":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J4') 
    elif Data == "J0":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J0')
else:
    Data = "shift"
    PoolRDOInput  = getDirectoryList('/u1/fizisist/ttbar_shift')

#-----------------
# Output files
#-----------------    
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
DetDescrVersion = "ATLAS-GEO-02-01-00"
GlobalFlags.DetDescrVersion = DetDescrVersion

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
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.BeamSpotSliceFlags import *
BeamSpotSliceFlags.setAll()

#-----------------
# BeamSpot Menu
#-----------------
def MyBeamSpotMenu():
  if ('Seed' in dir()):
      if Seed == "list":
        TriggerFlags.BeamSpotSlice.signatures = [ 'beamspot_vertex_list' ]
      elif Seed == "jet10":
        TriggerFlags.BeamSpotSlice.signatures = [ 'beamspot_vertex_jet10' ]
      elif Seed == "bjet18":
        TriggerFlags.BeamSpotSlice.signatures = [ 'beamspot_vertex_bjet18' ]
      elif Seed == "FSTracks":
        TriggerFlags.BeamSpotSlice.signatures = [ 'beamspot_vertex_FSTracks' ]
      elif Seed == "tau16i_loose":
        TriggerFlags.BeamSpotSlice.signatures = [ 'beamspot_vertex_tau16i_loose' ]
      elif Seed == "e10_medium":
        TriggerFlags.BeamSpotSlice.signatures = [ 'beamspot_vertex_e10_medium' ]
      elif Seed == "e10_medium_SiTrk":
        TriggerFlags.BeamSpotSlice.signatures = [ 'beamspot_vertex_e10_medium_SiTrk' ]
      elif Seed == "e10_medium_SiTrk_robust":
        TriggerFlags.BeamSpotSlice.signatures = [ 'beamspot_vertex_e10_medium_SiTrk_robust' ]
      elif Seed == "trk9i_calib":
        TriggerFlags.BeamSpotSlice.signatures = [ 'beamspot_vertex_trk9i_calib' ]
  else:
      TriggerFlags.BeamSpotSlice.signatures = [ 'beamspot_vertex_FSTracks' ]

from TriggerMenuPython.GenerateMenu import GenerateMenu
GenerateMenu.overwriteSignaturesWith(MyBeamSpotMenu)

#-----------------
# Trigger Options
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
#TriggerFlags.triggerMenuSetup='Test_lumi1E31'
TriggerFlags.abortOnConfigurationError = True
TriggerFlags.doHypo = True
TriggerFlags.doFEX  = True

#-----------------
# BeamSpot Config
#-----------------
from TrigT2BeamSpot.TrigT2BeamSpotConf import PESA__T2VertexBeamSpot
PESA__T2VertexBeamSpot.TrackAlgoID             = 2
PESA__T2VertexBeamSpot.HistoTrackAlgs          = True
PESA__T2VertexBeamSpot.SaveAllVertices         = False

PESA__T2VertexBeamSpot.TotalNTrackMin          = 3
PESA__T2VertexBeamSpot.TrackMinPt              = 0.7
PESA__T2VertexBeamSpot.TrackSeedPt             = 1.0
PESA__T2VertexBeamSpot.TrackMaxZ0              = 200.0
PESA__T2VertexBeamSpot.TrackMaxD0              = 10.0
PESA__T2VertexBeamSpot.TrackMaxZ0err           = 1.0
PESA__T2VertexBeamSpot.TrackMaxD0err           = 1.0
PESA__T2VertexBeamSpot.TrackMinQual            = 0.0
PESA__T2VertexBeamSpot.TrackMaxQual            = 10.0
PESA__T2VertexBeamSpot.TrackMinSiHits          = 4
PESA__T2VertexBeamSpot.TrackMinTRTHits         = 1

PESA__T2VertexBeamSpot.VertexMinNTrk           = 3
PESA__T2VertexBeamSpot.VertexMaxNTrk           = 30
PESA__T2VertexBeamSpot.VertexMaxXerr           = 0.2
PESA__T2VertexBeamSpot.VertexMaxYerr           = 0.2
PESA__T2VertexBeamSpot.VertexMaxQual           = 1000.0


#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["SHIFT DATAFILE='shift-monitoring.root' OPT='RECREATE'", 
                              "EXPERT DATAFILE='expert-monitoring." + SiTrackOpt + "." + Data + "." + Seed + "." + NEvents + "evt." + TrackType + ".root' OPT='RECREATE'", 
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
   algo.OutputLevel = OutputLevel 

MessageSvc.debugLimit  = 1000000
MessageSvc.OutputLevel = OutputLevel
MessageSvc.Format      = "% F%48W%S%7W%R%T %0W%M"

ServiceMgr.StoreGateSvc.Dump = False 

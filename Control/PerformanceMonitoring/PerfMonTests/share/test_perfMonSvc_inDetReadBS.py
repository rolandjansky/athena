###############################################################
#
# Job options file
#
#==============================================================

from AthenaCommon.AppMgr import theApp

if not 'doMonitoring' in dir():
    doMonitoring = True
    pass
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = doMonitoring
jobproperties.PerfMonFlags.doPersistencyMonitoring = True
jobproperties.PerfMonFlags.doDetailedMonitoring = True
jobproperties.PerfMonFlags.OutputFile = "perfmon.indetReadBS.root"

###############################
# Load perf service
###############################
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc(
    "PerfMonSvc",
    OutputLevel  = INFO,
    IoContainers = [
        "EventInfo#McEventInfo",
        "McEventCollection#TruthEvent",
        "DetailedTrackTruthCollection#DetailedTrackTruth",
        "InDet::PixelClusterContainer#PixelClusters",
        "InDet::SCT_ClusterContainer#SCT_Clusters",
        "InDet::TRT_DriftCircleContainer#TRT_DriftCircles",
        "Rec::TrackParticleContainer#TrackParticleCandidate",
        "TrackParticleTruthVector#TrackParticleTruthCollection",
        "Trk::PRD_MultiTruthCollection#PRD_MultiTruthPixel",
        "Trk::PRD_MultiTruthCollection#PRD_MultiTruthSCT",
        "Trk::PRD_MultiTruthCollection#PRD_MultiTruthTRT",
        "Trk::SegmentCollection#TRTSegments",
        "Trk::TrackCollection#ExtendedTracks",
        "Trk::TrackTruthCollection#TrackTruthCollection",
        "Trk::VxContainer#VxPrimaryCandidate"
        ]
    )

################################################################
# Steer InDetRec job
#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce trackingg ntuple
doTrkNtuple     = False
# --- produce an atlantis data file
doJiveXML       = False
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = True
doWriteAOD      = True
# --- read BS
doReadBS        = True
# --- do auditors ?
doAuditors      = True

import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True 
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False

#--------------------------------------------------------------
# Load InDetFlags configuration
#--------------------------------------------------------------

# --- setup class with default values
include( "InDetRecExample/ConfiguredInDetFlags.py")
InDetFlags = ConfiguredInDetFlags(xKalman      = True,
                                  iPatRec      = True,
                                  newTracking  = True,
                                  doTruth      = not doReadBS)
# --- enable back tracking
InDetFlags.enableBackTracking()
# --- active V0-Finder and Conversion-Finder
InDetFlags.enableV0Finder()
InDetFlags.enableConversions()
# --- different output options 
#InDetFlags.setAODall()
#InDetFlags.setRestrictedESD()
# --- activate Z vertex scan in new tracking
#InDetFlags.usePrimVertexZcoordinate ( True )
# --- activate if the InDetRecStatistics ntuple should be written:
#InDetFlags.enableStatistics(doStatNtuple=True, StatNtupleName="InDetRecStatistics.root")
# --- activate if fall-back to DKF and fixed layer material is needed
#InDetFlags.setTrackFitterType("KalmanDNAFitter")
#InDetFlags.setTrackFitterType("DistributedKalmanFilter")
#InDetFlags.setTrackFitterType("GlobalChi2Fitter")

#--------------------------------------------------------------
# detector description version
#--------------------------------------------------------------

DetDescrVersion = "ATLAS-DC3-07"
#DetDescrVersion = "ATLAS-CSC-01-00-00"

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = -1
#
if not doReadBS:
  # --- t tbar channel 5200 (DetDescrVersion = "ATLAS-DC3-07")
  # ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d38/ID_ValidationData/testIdeal_07.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000201_tid002760._00002.pool.root.1"]
  # --- TP split version (local)
  ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d89/InDetRecRDO.root"]
  # --- gamma 20 (ATLAS-CSC-01-00-00)
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/datafiles/egamma/DC3.007040.singlepart_gamma_Et20/digit/120031/ideal0_mc12.007040.singlepart_gamma_Et20.digit.RDO.v12003108_tid005022._00001.pool.root"]
  #ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/datafiles/egamma/DC3.007040.singlepart_gamma_Et20/digit/120031/ideal0_mc12.007040.singlepart_gamma_Et20.digit.RDO.v12003108_tid005022._00002.pool.root"]
###############################################################

from GaudiSvc.GaudiSvcConf import MessageSvc
svcMgr += MessageSvc( defaultLimit = 4000000 )
svcMgr.MessageSvc.OutputLevel  = ERROR

#==============================================================
#
# End of job options file
#
###############################################################

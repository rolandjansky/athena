#--------------------------------------------------------------------------
#
# MuonRec_topOptions.py
#
# Primary file to drive muon reconstruction
#
# Useage:  athena MuonRec_topOptions.py
#
# lots of these flags are temporary, and will be cleaned up once we move to using configurables
# Edward.Moyse@cern.ch
# Stephane.Willocq@cern.ch
#
#--------------------------------------------------------------------------

# Run MuPerf or not
doMuPerf = False

# Turn off all reconstruction algorithms
#doESD = False

# ---- Input / Output
readESD=False
doReadMuonDigit = False
doMuonRecStatistics = True
doWriteESD = False         # Decide whether to write ESD/AOD/TAG
doWriteAOD = True
doWriteTAG = False
doJiveXML = False

# ---- Reconstruction
doAOD = True              # Decide whether to run AOD-making algorithms
doAODall = True
doTrigger  = False
doMuonIDStandAlone = True # extrapolate Moore tracks to the IP
doMuonIDCombined   = True # combined reconstruction with MuIDCombined
doMoore            = True
doMuonboy          = True
doStaco            = True # combined reconstruction with Staco
doMuTag            = True # muon identification with MuTag (requires Muonboy and Staco)
doMuGirl           = True # muon identification with MuGirl
doCaloTrkMuId      = True # muon identification with Calorimeter
doCaloTopoCluster  = True # run calo topo cluster (needed for CaloLR muons)

# ---- Set up Muon Spectrometer reconstruction
from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.doMuonboy = True
muonRecFlags.doMoore = True
muonRecFlags.doTrackPerformance = True
muonRecFlags.TrackPerfDebugLevel = 0
muonRecFlags.doStandalone.set_Value_and_Lock(True)
muonRecFlags.doTrkD3PD = True

# ---- Misc
doVisualization = False    # Decide whether to run HepVis or not
doTrackRecordFilter = True # process Muon Spectrometer entry truth info
doTruth=True

# ----- End of Flags section

#--------------------------------------------------------------
# Needed for Configurables
#--------------------------------------------------------------
from RecExConfig.RecAlgsFlags import recAlgs as recAlgsFlags
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

topSequence = AlgSequence()
ToolSvc = Service("ToolSvc")

# Specify number of events to be processed
if not 'EvtMax' in dir():
    EvtMax = 10000

# Optionally run Inner Detector + Muon Spectrometer
# Turn on NewT if combined ID requested
if doMuonIDCombined:
    donewTracking = True

#re-enable InDet and Calo standalone by the lock mechanism
#from InDetRecExample.InDetJobProperties import InDetFlags
#InDetFlags.Enabled=True
#InDetFlags.Enabled._locked=True

#from CaloRec.CaloRecFlags import jobproperties
#jobproperties.CaloRecFlags.Enabled=True
#jobproperties.CaloRecFlags.Enabled._locked=True
 
# Select detector description

if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-GEO-16-00-00"

# Z -> mu mu RDO input files
# uncomment the following line to run interactively
# RTT system adds the input file automatically with
# the input provided via the xml configuration
#
#athenaCommonFlags.FilesInput.set_Value_and_Lock(["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/det-muon/valid/Zmumu_mc10/RDO.285895._000001.pool.root.1"])

# RDO input files (won't be used if readESD=True)
#PoolRDOInput = [
    #"root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/det-muon/valid/Zmumu_mc10/RDO.285895._000001.pool.root.1"
    #]

# common flags
include ("RecExCommon/RecExCommon_flags.py")

# if no combined muon reco is requested then turn off Calo and ID
# except for their detector description
if not recAlgsFlags.doMuonIDCombined() and not recAlgsFlags.doStaco() and not recAlgsFlags.doMuGirl():
    DetFlags.Calo_setOff()
    DetFlags.detdescr.Calo_setOn()
    DetFlags.ID_setOff()
    DetFlags.detdescr.ID_setOn()

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

include("MuonTrackPerformance/MuonTrackStatistics_jobOptions.py")
#from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackStatisticsAlg
#MuonTrackStatistics = MuonTrackStatisticsAlg("MuonTrackStatistics")
MuonTrackStatistics.writeToFile = True
MuonTrackStatistics.TrackLocationList  = ["MooreTracks"]
MuonTrackStatistics.TrackLocationList += ["ConvertedMuIdCBTracks"]
MuonTrackStatistics.TrackLocationList += ["MuGirlRefittedTracks"]
MuonTrackStatistics.TrackLocationList += ["MuTagIMOTracks"]
MuonTrackStatistics.TrackLocationList += ["ConvertedMBoyTracks"]
MuonTrackStatistics.TrackLocationList += ["ConvertedStacoTracks"]
MuonTrackStatistics.TrackLocationList += ["ConvertedMuTagTracks"]
MuonTrackStatistics.TrackLocationList += ["MuonSpectrometerTracks"]

# Add library loading output
import AthenaServices.AthDsoLogger as ADL
dso = ADL.DsoLogger() # monitor library sizes


#--------------------------------------------------------------------------

# Create MuPerf tuple.
if doMuPerf:
    include ("MuonSelectionTools/MuAtlasRelease.py")
    conname = "mucmb_all_kine"
    THistSvc = Service ( "THistSvc" )
    THistSvc.Output += ["hist DATAFILE='muperf_hist.root' OPT='UPDATE'"]
    THistSvc.Output += ["tree DATAFILE='muperf_tree.root' OPT='UPDATE'"]
    include("MuonIDValidation/MuonPerformanceOptions.py")
    
#--------------------------------------------------------------------------

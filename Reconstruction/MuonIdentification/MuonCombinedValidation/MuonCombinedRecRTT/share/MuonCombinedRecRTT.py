# MuonRecRTT_sim_standalone.py
#
# top options to run Muon Spectrometer reconstruction only
# on simulated data, using MuonRecExample/MuonRec_myTopOptions.py

# flags set up in MuonRec_myTopOptions.py can be modified
# and locked as in the example below

#from MuonRecExample.MuonRecFlags import muonRecFlags
#muonRecFlags.doCalib.set_Value_and_Lock(False)

# Turn on PerfMon to monitor the software performance
from RecExConfig.RecFlags import rec 
# recFlags.doPerfMon.set_Value_and_Lock(True)
from RecExConfig.RecFlags import rec as rec

# I'm sure there's a better way to do this, but I just wanted to get something working fast. Ed.
from MuonRecExample.MuonRecFlags import muonRecFlags


# Run MuPerf or not
doMuPerf = False

# Turn off all reconstruction algorithms

# ---- Input / Output
readESD=True
doMuonRecStatistics = False 
doWriteESD = False         # Decide whether to write ESD/AOD/TAG
doWriteAOD = True
doWriteTAG = False
doJiveXML = False
donewTracking = False
# ---- Reconstruction
doESD = True
doAOD = True              # Decide whether to run AOD-making algorithms
doAODall = True
rec.readAOD.set_Value_and_Lock(False)
rec.doApplyAODFix.set_Value_and_Lock(False)
#doTrigger  = True 
doMuonIDStandAlone = True # extrapolate Moore tracks to the IP
doMuonIDCombined   = True # combined reconstruction with MuIDCombined
doMoore            = True
doMuonboy          = True
doStaco            = True # combined reconstruction with Staco
doMuTag            = True # muon identification with MuTag (requires Muonboy and Staco)
doMuGirl           = True # muon identification with MuGirl
doCaloTrkMuId      = True # muon identification with Calorimeter
doCaloTopoCluster  = True # run calo topo cluster (needed for CaloLR muons)
import os
doNameAuditor   = True 
# ---- Misc
doVisualization = False    # Decide whether to run HepVis or not
doTrackRecordFilter = False # process Muon Spectrometer entry truth info to add it to CBNT
doCBNT=False



# muonRecFlags.doTrackPerformance = True
# muonRecFlags.TrackPerfDebugLevel = 0
# on lxplus209
InputCollection = []
InputCollection+=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-muon/MuonCombinedRTTInputs/Zmumu/ESD.213372._000088.pool.root.2"]
InputCollection+=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-muon/MuonCombinedRTTInputs/Zmumu/ESD.213372._000092.pool.root.2"]
InputCollection+=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-muon/MuonCombinedRTTInputs/Zmumu/ESD.213372._000100.pool.root.2"]
InputCollection+=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-muon/MuonCombinedRTTInputs/Zmumu/ESD.213372._000192.pool.root.2"]
InputCollection+=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-muon/MuonCombinedRTTInputs/Zmumu/ESD.213372._000302.pool.root.2"]
InputCollection+=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-muon/MuonCombinedRTTInputs/Zmumu/ESD.213372._000312.pool.root.2"]
InputCollection+=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-muon/MuonCombinedRTTInputs/Zmumu/ESD.213372._000407.pool.root.2"]
InputCollection+=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-muon/MuonCombinedRTTInputs/Zmumu/ESD.213372._000427.pool.root.2"]
InputCollection+=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-muon/MuonCombinedRTTInputs/Zmumu/ESD.213372._000459.pool.root.2"]
InputCollection+=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-muon/MuonCombinedRTTInputs/Zmumu/ESD.213372._000027.pool.root.2"]

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
#athenaCommonFlags.FilesInput.set_Value_and_Lock(InputCollection)





from RecExConfig.InputFilePeeker import inputFileSummary

DoTruth = False
if 'IS_SIMULATION' in inputFileSummary['evt_type']:
    DoTruth = True

if not DoTruth:
    rec.doTruth.set_Value_and_Lock(False)
    pass
rec.doForwardDet.set_Value_and_Lock(False)
include ("RecExCommon/RecExCommon_topOptions.py")

	
#####################################################################3
## Adding beamspot info
#from MinBiasD3PDMaker.MinBiasD3PDMakerConf import MinBiasD3PD__MinBiasD3PDPreProcessing
#MinBiasD3PDPreProcessingAlg = MinBiasD3PD__MinBiasD3PDPreProcessing( name = "MinBiasD3PDPreProcessingAlg", OutputLevel = INFO )
#topSequence += MinBiasD3PDPreProcessingAlg
#include('InDetBeamSpotService/BeamCondSvc.py')


include ("CaloD3PDMaker/CaloD3PD_Rapgap_jobOption.py")

include ("PhysicsD3PDMaker/MyJetMake.py")
include ("MuonCombinedRecRTT/MCPD3PDMaker.py")

tupleFileOutput = 'physics.root' 
alg = MCPphysicsD3PD (file=tupleFileOutput,tuplename='physics', streamNameRoot ='OutStream')


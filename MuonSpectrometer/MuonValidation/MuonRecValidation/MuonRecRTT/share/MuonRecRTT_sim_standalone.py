# MuonRecRTT_sim_standalone.py
#
# top options to run Muon Spectrometer reconstruction only
# on simulated data, using MuonRecExample/MuonRec_myTopOptions.py

# flags set up in MuonRec_myTopOptions.py can be modified
# and locked as in the example below

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#from MuonRecExample.MuonRecFlags import muonRecFlags
#muonRecFlags.doCalib.set_Value_and_Lock(False)

# Turn on PerfMon to monitor the software performance
from RecExConfig.RecFlags import rec as recFlags
recFlags.doPerfMon.set_Value_and_Lock(True)

# I'm sure there's a better way to do this, but I just wanted to get something working fast. Ed.
from MuonRecExample.MuonRecFlags import muonRecFlags

muonRecFlags.doTrkD3PD = True
muonRecFlags.doTrackPerformance = True
muonRecFlags.TrackPerfDebugLevel = 0

if 'doMoore' in dir():
  if doMoore==False:
    muonRecFlags.doMoore = False
if 'doMuonboy' in dir():
  if doMuonboy==False:
    muonRecFlags.doMoore = False
if 'doStandalone' in dir():
  if doStandalone==False:
    muonRecFlags.doStandalone = False

# Set input file
athenaCommonFlags.FilesInput.set_Value_and_Lock(["root://eosatlas//eos/atlas/atlascerngroupdisk/det-muon/valid/mc11_Zmumu/RDO.533482._000001.pool.root.1"])

include ("MuonRecExample/MuonRec_myTopOptions.py")


include("MuonTrackPerformance/MuonTrackStatistics_jobOptions.py")

# Add library loading output
import AthenaServices.AthDsoLogger as ADL
dso = ADL.DsoLogger() # monitor library sizes

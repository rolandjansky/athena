#======================================================================
# TOP JobOptions to run on Muons Cosmic Reconstruction
# R. Nikolaidou
# Modified for RTT - 14/02/08 B. Resende
#======================================================================

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec

#-----------------------------------------------------------------
# Setup Internal flags for the Muon Spectrometer (defaults values)
#-----------------------------------------------------------------

from MuonRecExample.MuonRecFlags import muonRecFlags

################################################################################
# Flags for RTT
################################################################################

athenaCommonFlags.EvtMax.set_Value_and_Lock(2000)
athenaCommonFlags.SkipEvents.set_Value_and_Lock(0)
globalflags.DataSource.set_Value_and_Lock('geant4')

rec.AutoConfiguration = ['everything']

# don't run on CSC data 
muonRecFlags.doCSCs=False

rec.doTrigger = False
rec.doTruth = True
rec.doWriteESD = False
rec.doESD = False
muonRecFlags.doMoore = True
muonRecFlags.doMuonboy = True
muonRecFlags.doStandalone.set_Value_and_Lock(False)
muonRecFlags.doTrkD3PD = True
rec.doPerfMon = True
rec.doDetailedPerfMon = True
rec.doWriteESD = True
rec.doMonitoring = False
rec.doPersint = False
rec.doDetailedAuditor = True
rec.Commissioning = True

# Reading from ESD
rec.readESD = False	
if "readESD" in dir():
  rec.readESD = readESD

if rec.readESD: #move to xml file if possible
  athenaCommonFlags.PoolESDInput = ["/castor/cern.ch/grid/atlas/atlasmcdisk/valid2/ESD/valid2.108869.CosSimMuonVolSolOnTorOn.recon.ESD.s535_d167_r676_tid065081/ESD.065081._00003.pool.root.1"]

################################################################################
# Main jobOptions
################################################################################

include ("MuonRecExample/MuonRec_myTopOptions.py")

MessageSvc.OutputLevel = INFO

from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")

# Add library loading output
import AthenaServices.AthDsoLogger as ADL
dso = ADL.DsoLogger() # monitor library sizes

include ("MuonTestEDM/MuonTestEDM_jobOptions.py")
MyEDMTester.DoDumpSummaryToFile = True
MyEDMTester.DoDumpToFile = False 

# Aesthetic track summary for all collections
include("MuonTrackPerformance/MuonTrackStatistics_jobOptions.py")
#from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackStatisticsAlg
#MuonTrackStatistics = MuonTrackStatisticsAlg("MuonTrackStatistics")
MuonTrackStatistics.writeToFile = True

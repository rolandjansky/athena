#======================================================================
# TOP JobOptions to run on Muons Cosmic Reconstruction
# R. Nikolaidou
# Modified for RTT - 14/02/08 B. Resende
#======================================================================

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
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
globalflags.DataSource.set_Value_and_Lock('data')
jobproperties.Beam.beamType.set_Value_and_Lock("collisions")

rec.AutoConfiguration = ['everything']

# don't run on CSC data 
##muonRecFlags.doCSCs=False

rec.doTrigger = False
rec.doTruth = False
rec.doWriteESD = False
rec.doESD = False
muonRecFlags.doMoore = True
muonRecFlags.doMuonboy = True
muonRecFlags.doStandalone.set_Value_and_Lock(True)
muonRecFlags.doTrkD3PD.set_Value_and_Lock(True)
rec.doPerfMon = True
rec.doDetailedPerfMon = True
rec.doWriteESD = True
rec.doMonitoring = False
rec.doPersint = False
rec.doDetailedAuditor = False
rec.Commissioning = True

# Reading from ESD
rec.readESD = False	
if "readESD" in dir():
  rec.readESD.set_Value_and_Lock(readESD)
if rec.readESD:
  globalflags.InputFormat.set_Value_and_Lock('pool')
else:
  globalflags.InputFormat.set_Value_and_Lock('bytestream')

if rec.readESD:
  athenaCommonFlags.FilesInput.set_Value_and_Lock(["root://eosatlas//eos/atlas/atlascerngroupdisk/det-muon/valid/ESDZmumuSkim/data12_8TeV.00214160.physics_Muons.recon.DESD_ZMUMU.f486_m1246_f486._0010.1","root://eosatlas//eos/atlas/atlascerngroupdisk/det-muon/valid/ESDZmumuSkim/data12_8TeV.00214160.physics_Muons.recon.DESD_ZMUMU.f486_m1246_f486._0011.1","root://eosatlas//eos/atlas/atlascerngroupdisk/det-muon/valid/ESDZmumuSkim/data12_8TeV.00214160.physics_Muons.recon.DESD_ZMUMU.f486_m1246_f486._0012.1"])
  #athenaCommonFlags.FilesInput.set_Value_and_Lock(["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/det-muon/Bytestream2010/MuonBytestreamZmumu_rec16.0.X.ESD.pool.root"])
  rec.doEgamma = False
  from JetRec.JetRecFlags import jetFlags
  jetFlags.Enabled=False
else:
  #athenaCommonFlags.BSRDOInput... no longer working as expected, March 2011
  athenaCommonFlags.FilesInput.set_Value_and_Lock(["root://eosatlas//eos/atlas/atlascerngroupdisk/det-muon/valid/RawZmumuSkim/data12_8TeV.00214160.physics_Muons.merge.DRAW_ZMUMU.f486_m1246._0010.1","root://eosatlas//eos/atlas/atlascerngroupdisk/det-muon/valid/RawZmumuSkim/data12_8TeV.00214160.physics_Muons.merge.DRAW_ZMUMU.f486_m1246._0011.1","root://eosatlas//eos/atlas/atlascerngroupdisk/det-muon/valid/RawZmumuSkim/data12_8TeV.00214160.physics_Muons.merge.DRAW_ZMUMU.f486_m1246._0012.1"])

  #athenaCommonFlags.BSRDOInput.set_Value_and_Lock(["/castor/cern.ch/atlas/atlascerngroupdisk/det-muon/Bytestream2010/MuonBytestreamRun155697_selectedCbMu10Events.data"])

################################################################################
# Main jobOptions
################################################################################

include ("MuonRecExample/MuonDataRec_myTopOptions.py")

MessageSvc.OutputLevel = INFO

from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")

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
MuonTrackStatistics.TrackLocationList  = ["MooreTracks"]
MuonTrackStatistics.TrackLocationList += ["ConvertedMBoyTracks"]
MuonTrackStatistics.TrackLocationList += ["MuonSpectrometerTracks"]

if not rec.readESD:
  # kluge to avoid "NameError: name 'ByteStreamInputSvc' is not defined" error
  # this should not be needed but is here to get things working (SW)
  ByteStreamInputSvc = svcMgr.ByteStreamInputSvc

# kluge to avoid crash in creating D3PD
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
AANTupleStream('D3PD').ExistDataHeader = False

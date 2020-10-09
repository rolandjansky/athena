# ---------------------------------------------------------------------------
# RecExCommon flags
# ---------------------------------------------------------------------------

include.block("RecExCommission/RecExCommissionCommonFlags_jobOptions.py")


# ---------------------------------------------------------------------------
# AthenaCommonFlags
# ---------------------------------------------------------------------------

# start using the new job properties
from AthenaCommon.JobProperties import jobproperties

# AthenaCommon flags
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags

# -----------------------------------------------------------------------
# being general reconstruction flags
# -----------------------------------------------------------------------
from RecExConfig.RecFlags  import jobproperties, rec

#Number of events to process or generate
athenaCommonFlags.EvtMax=10
# Number of events to skip when reading an input POOL file.
athenaCommonFlags.SkipEvents = 0
# The list of input POOL files containing collections of Raw data objects
# ['myfile.pool.root'] : file in run directory
# ['LFN:mylogicalfile.root']: logical file name as specified in PoolFileCatalog.cml
# ['rfio:/castor/cern.ch/somepath/somefile.pool.root']:file on castor (at CERN)
athenaCommonFlags.PoolRDOInput = ["/afs/cern.ch/atlas/maxidisk/d17/data/TileTest/dig.05AprProd-10000000.root"]
# The list of input ByteStream files containing collections of Raw data objects
#athenaCommonFlags.BSRDOInput = ["/castor/cern.ch/grid/atlas/t0/perm/DAQ/daq.m4_combined.0019783.Default.L1TT-b00000001.LB0001.SFO-1._0001.data"]
athenaCommonFlags.BSRDOInput = ["/castor/cern.ch/grid/atlas/t0/perm/DAQ/daq.m4_combined.0020720.debug.L1TT-b00000001.LB0000.SFO-1._0001.data"]
#athenaCommonFlags.BSRDOInput = ["/castor/cern.ch/grid/atlas/DAQ/M6/daq.NoTag.0043719.physics.HLT_Cosmic_AllTeIDSelected.LB0000.SFO-1._0001.data"]
# The list of input POOL files containing collections of ESD objects
athenaCommonFlags.PoolESDInput = ["castor:/castor/cern.ch/grid/atlas/t0/perm/M4reproc/0020720/FESD/M4.0020720.physics.L1TT-b00000010.FESD.v130026.part0001._lumi0002._0001.1"]
# The name of the output POOL file containing collections of ESD objects
athenaCommonFlags.PoolESDOutput = "ESD.root"
# The list of input POOL files containing collections of TAGs
#athenaCommonFlags.PoolTAGInput = "TAG.root"
# The name of the output POOL file containing collections of TAGs
athenaCommonFlags.PoolTAGOutput = "TAG.root" 
rec.PoolTAGCOMOutput="TAGCOM.root"
athenaCommonFlags.PoolInputQuery = "TRT_Cosmic_Tracks"
#AllowIgnoreExistingDataObject
#AllowIgnoreConfigError
#athenaCommonFlags.AthenaCommonFlags

# -----------------------------------------------------------------------
# GlobalFlags
# -----------------------------------------------------------------------

# GlobalFlags 
from AthenaCommon.GlobalFlags import globalflags

# Which detector configuration : atlas, combined test beam or commisisoning
# Commissioning: 'commis'
globalflags.DetGeo = 'commis'
# Detector geometry DB tag
globalflags.DetDescrVersion="ATLAS-CommNF-04-00-00"
# data source: Where does the data comes from : real data, geant3 or geant4
# real data: data
# simulation: 'geant4'
globalflags.DataSource = 'data'
# Input format:
# to read from BS: 'bytestream'
# to read from ESD, RDO Pool files = 'pool'
globalflags.InputFormat = 'bytestream'

# ---------------------------------------------------------------------------
# Beam flags to define the 
# ---------------------------------------------------------------------------
# Type of data to reconstruct: 'singlebeam','cosmics'
from AthenaCommon.BeamFlags import jobproperties
#jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")


# ---------------------------------------------------------------------------
# BField flags to define the
# ---------------------------------------------------------------------------
# Field configuration: solenoidOn()   barrelToroidOn() endcapToroidOn()
from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.solenoidOn=False
jobproperties.BField.barrelToroidOn=False
jobproperties.BField.endcapToroidOn=False

# -----------------------------------------------------------------------
# flags to drive the general behaviour of Reconstruction configuration
# -----------------------------------------------------------------------
#from RecExConfig.RecConfFlags import recConfFlags


#RecConfFlags.AllowDisable
#RecConfFlags.AllowIgnoreConfigError	
#RecConfFlags.AllowIgnoreExistingDataObject	
#RecConfFlags.RecConfFlags

rec.CBNTAthenaAware = True	
rec.doAOD = False
#rec.doAODall	
#rec.doAODCaloCells	
rec.doCBNT = True	
#rec.doCheckDictionary	
#rec.doCheckJOT	
#rec.doDetailedAuditor	
#rec.doDumpMC	
#rec.doDumpPoolInputContent	
#rec.doDumpProperties	
#rec.doDumpTDS	
#rec.doDumpTES	
#rec.doEdmMonitor	
#rec.doESD = True
# rec.doFileMetaData TODO might replace doDetStatus???
rec.doDetStatus = True
#rec.doFloatingPointException	
#rec.doHeavyIon	
rec.doHist = True
rec.doJiveXML = False	
#rec.doLowPt	
#rec.doMinimalRec	
#rec.doNameAuditor	
#rec.doPerfMon = False
rec.doPersint = False	
#rec.doRestrictedESD	
#rec.doSGAuditor	
#rec.doShowSizeStatistics	
#rec.doTimeLimit	
#rec.doTruth	
rec.doWriteAOD = False
#rec.doWriteBS
# If True writes out ESD file
rec.doWriteESD = True
#rec.doWriteRDO
# If True writes out TAG file
rec.doWriteTAG = True	
#rec.noESDTrigger	
#rec.oldFlagCompatibility	
#rec.oldFlagLandMine	
#rec.oldFlagTopSteering
# General msg output level ALL,VERBOSE,DEBUG,INFO,WARNING,ERROR,FATAL
rec.OutputLevel	= INFO
#rec.readAOD
#If True runs on ESD file  
rec.readESD = False	
#rec.readRDO	
#rec.readTAG	
#rec.Rec	
#rec.RecAlgs	
rec.RootHistoOutput = "monitoring.root"	
rec.RootNtupleOutput = "ntuple.root"	
#rec.TAGFromRDO	
#rec.UserAlgs

rec.doTile = True
rec.doLArg = True
rec.doInDet = True
rec.doMuon = True

## Switch on/off Calibration Ntuple
#from MuonRecExample.MuonRecFlags import muonRecFlags
#from MuonCalibAlgs.MuonCalibFlags import muonCalibFlags
#muonRecFlags.doCalib = True
#muonCalibFlags.Mode = 'trackNtuple'
#muonCalibFlags.EventTag = 'Moore'

### Switch on/off Combined Algorithms
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags,muidFlags
muonCombinedRecFlags.doMuGirl = False
muonCombinedRecFlags.doCaloTrkMuId = True
muonCombinedRecFlags.doStaco = False
muonCombinedRecFlags.doMuTag = False
muonCombinedRecFlags.doAODMuons = True  # switch off AOD making
muonCombinedRecFlags.doMergeMuons = True # switch off merging for ESD
muidFlags.SegmentTagger = 'MuTagIMO' # switch off  by ''
#muidFlags.Extrapolated = 'MuidStandalone' # switch off  by ''
muidFlags.Extrapolated = '' # switch off  by ''
#muidFlags.Combined = 'MuidCombined' # NOW not run # switch off  by ''
muidFlags.Combined = 'CombinedMuonFit' # switch off  by ''


# ----------------------------------------------------------------------
# being flags to switch algorithm on/off
# ----------------------------------------------------------------------
from RecExConfig.RecAlgsFlags  import recAlgs

#recAlgs.doTrigger 

#
# hack...
#
from RecExConfig.RecFlags import rec
if not 'InDetKeys' in dir():
    #
    # --- setup StoreGate keys (JobProperties!)
    #
    print("InDetRec_jobOptions: InDetKeys not set before - I import them now")
    from InDetRecExample.InDetKeys import InDetKeys
#    InDetKeys.lock_JobProperties()
                                                                         

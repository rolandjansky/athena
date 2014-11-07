# #######################################################################
# TopOptions to process cosmics /single beam data 
# M. Woudstra, R. Nikolaidou 
#
# By default we run on 10 events of the 121080 (taken on June 2009) run,
# autoconfiguration of bfield and conditions tag, full reconstruction
# Muonboy and Moore and we produce as
# output : ESD, Monitoring, ascii file with the configuration of the job  
# #######################################################################

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BFieldFlags import jobproperties
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from RecExConfig.RecFlags import rec
from RecExConfig.RecConfFlags import recConfFlags

# setup environment 
from MuonRecExample.MuonRecFlags import muonRecFlags,muonboyFlags,mooreFlags
from MuonRecExample import MuonRecStandaloneOnlySetup
from MuonRecExample import MuonRecUtils
from MuonRecExample.MuonRecUtils import assertCastorStager,hasJobPropertyBeenSet

# in Standalone mode, don't allow any configuration errors
athenaCommonFlags.AllowIgnoreConfigError = False

# Set properly beam type, DataSource, InputFormat, Detector Description, Conditions Tag
globalflags.DataSource.set_Value_and_Lock('data')
##jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
globalflags.InputFormat.set_Value_and_Lock('bytestream')

##############################################################################
if not hasJobPropertyBeenSet(athenaCommonFlags.FilesInput):
    assertCastorStager("castoratlast3","atlascerngroupdisk")
    athenaCommonFlags.FilesInput.set_Value_and_Lock(['rfio:/castor/cern.ch/atlas/atlascerngroupdisk/det-muon/Bytestream2010/MuonBytestreamRun155697_selectedCbMu10Events.data'])


# set the following to override the auto-config
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-08-00-01')
#jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
#jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
#jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
#globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-002-03')
#rec.triggerStream = "RPCwBeam"
rec.AutoConfiguration = ['TriggerStream','FieldAndGeo', 'ConditionsTag', 'BeamType' ]


## Descoping exercise (disable various parts in muon reconstruction chain ##
##


#---->  DESCOPING EXERCISE (disable various parts of reconstruction chain)
#

# Stop at formation of PRDs only
#muonRecFlags.Enabled = False

# Disable MDTs
#muonRecFlags.doMDTs=False

# Disable RPCs
#muonRecFlags.doRPCs=False

# Disable TGCs
#muonRecFlags.doTGCs=False

# Disable CSCs
#muonRecFlags.doCSCs=False

# Stop Muonboy at segment level 
#muonboyFlags.doSegmentsOnly = True

# Stop Moore at segment level
#mooreFlags.doSegmentsOnly = True

# Don't run on Muonboy at all
#muonRecFlags.doMuonboy = False

# Don't run on Moore at all
#muonRecFlags.doMoore = False

#----> END OF CONFIGURATION FOR DESCOPING EXERCISE 
#



# Run from an ESD file
#rec.readESD = True
#rec.doESD = False

#rec.doPerfMon = True
#rec.doDetailedAuditor = True

# Switch off the alignment corrections (on by default for data)
#muonRecFlags.useAlignmentCorrections = False

# Switch on/off the T0 fit (on by default for data)
#muonRecFlags.doSegmentT0Fit = False

# Calibration ntuple
#muonRecFlags.doCalibNtuple = False # write calibration ntuple?
#muonRecFlags.calibNtupleSegments = False # write segments to ntuple?
#muonRecFlags.calibNtupleTracks = False # write tracks to ntuple?
#muonRecFlags.calibNtupleTrigger = False # write trigger info to ntuple?
#muonRecFlags.calibMoore = False   # write Moore tracks/segments to ntuple?
#muonRecFlags.calibMuonboy = False # write Muonboy tracks/segments to ntuple?



# no truth when running on data
rec.doTruth = False

# Switch Perfmon studies 
rec.doPerfMon = True
rec.doDetailedPerfMon = True
rec.doNameAuditor = True

# For Trigger MBTS and L1 Calo we need the Tile Calorimeter
# the algorithm: MuonCalibExtraTreeTriggerAlg stores trigger info
DetFlags.makeRIO.Calo_setOn()
DetFlags.detdescr.LVL1_setOn()
DetFlags.readRDOBS.LVL1_setOn()
# Temporarily switch off Tile because of crash (bug #51240)
# ==> Will have less (trigger) info in Calibration Ntuple !!!
rec.doTile = False
# Write ESDs 
rec.doWriteESD = True


# Enable Persint Display  
rec.doPersint.set_Value_and_Lock(False)


#--------------------------------------------------------------------------------
# end of user flags
#--------------------------------------------------------------------------------

rec.doDetStatus.set_Value_and_Lock(False) # because of bug in DetectorStatus/DetStatusSvc_CondDB.py"
rec.Commissioning = True
# lock all flags. Very important!
#rec.lock_JobProperties()
muonRecFlags.lock_JobProperties()


# The main jobOption file
try:
    include("MuonRecExample/MuonRec_topOptions.py")
    ###### put any user finetuning after this line #####

    ###### put any user finetuning before this line #####

##### DO NOT ADD ANYTHING AFTER THIS LINE #####
except:
    # print the stacktrace (saving could fail, and would then obscure the real problem)
    import traceback
    print traceback.format_exc().rstrip()
    
    # always write config so far for debugging
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
    # add DetFlags
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    dumpDetFlags("config.txt")
    # but still exit with error
    import sys
    sys.exit(10)
else:
    # and write config to include user changes after topOptions
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
    # add DetFlags
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    dumpDetFlags("config.txt")

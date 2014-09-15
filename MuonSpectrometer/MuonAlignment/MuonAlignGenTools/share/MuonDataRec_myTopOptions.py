# #######################################################################
# TopOptions to process cosmics /single beam data 
# M. Woudstra, R. Nikolaidou 
#
# By default we run on 10 events of the 121080 (taken on June 2009) run,
# autoconfiguration of bfield and conditions tag, full reconstruction
# Muonboy and Moore and we produce as
# output : CBNT, ESD, Monitoring, ascii file with the configuration of the job  
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

# in Standalone mode, don't allow any configuration errors
athenaCommonFlags.AllowIgnoreConfigError = False

#
# command line parameters (-c)
#
if 'EvtMax' not in dir():
    EvtMax = 10 # maximum number of events to process

if 'SkipEvents' not in dir():
    SkipEvents = 0


# Set properly beam type, DataSource, InputFormat, Detector Description, Conditions Tag
globalflags.DataSource.set_Value_and_Lock('data')
#jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
#globalflags.InputFormat.set_Value_and_Lock('bytestream')

###############################################################################
# Data 2008 splash events
# https://twiki.cern.ch/twiki/bin/view/Atlas/IndividualInterestingSingleBeamEvents
## athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/afs/cern.ch/user/g/gencomm/w0/jboyd/GetInterestingEvents/BSfiles/87764/evt.194586._0001.data'])
## # set the following to override the auto-config
## globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-08-00-02')
## jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
## jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
## jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
## globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-ES1C-000-00')

##############################################################################
# Data 2009 without CSCs
#muonRecFlags.doCSCs = False
#athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/castor/cern.ch/grid/atlas/DAQ/2009/00121080/physics_RPCwBeam/data09_cos.00121080.physics_RPCwBeam.daq.RAW._lb0000._SFO-5._0605.data'])
#athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/castor/cern.ch/grid/atlas/DAQ/2009/00121064/physics_RPCwBeam/data09_cos.00121064.physics_RPCwBeam.daq.RAW._lb0000._SFO-5._0002.data'])
# set the following to override the auto-config
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-08-00-01')
#jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
#jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
#jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
#globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-002-03')
# Use auto-config with caution! It is tuned to Summer 2009 reprocessing.
#rec.triggerStream = "RPCwBeam"
rec.AutoConfiguration = ['TriggerStream','FieldAndGeo', 'ConditionsTag']
#rec.AutoConfiguration = ['TriggerStream','FieldAndGeo', 'ConditionsTag'] 

##############################################################################
## # Data 2008 with CSCs
## muonRecFlags.doCSCs=True
## athenaCommonFlags.BSRDOInput.set_Value_and_Lock([
##         '/castor/cern.ch/grid/atlas/DAQ/2008/91897/physics_TGCwBeam/daq.ATLAS.0091897.physics.TGCwBeam.LB0001.SFO-1._0001.data'])
## globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEONF-04-00-00')
## jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
## jobproperties.BField.barrelToroidOn.set_Value_and_Lock(False)
## jobproperties.BField.endcapToroidOn.set_Value_and_Lock(False)
## globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-001-02')


##############################################################################
## # Data 2008 without CSCs
## muonRecFlags.doCSCs=False
## athenaCommonFlags.BSRDOInput.set_Value_and_Lock([
##     '/castor/cern.ch/grid/atlas/atlasdatadisk/data08_cos/RAW/data08_cos.00091060.physics_RPCwBeam.daq.RAW.o4/daq.ATLAS.0091060.physics.RPCwBeam.LB0032.SFO-1._0013.data'])
##     #        '/castor/cern.ch/grid/atlas/atlasdatadisk/data08_cos/RAW/data08_cos.00091060.physics_RPCwBeam.daq.RAW.o4/daq.ATLAS.0091060.physics.RPCwBeam.LB0001.SFO-1._0001.data'])
##     #        '/castor/cern.ch/grid/atlas/DAQ/2008/87863/physics_BPTX/daq.NoTag.0087863.physics.BPTX.LB0000.SFO-1._0001.data'])
## # WARNING: the files on that atlsdatadisk may disappear after some time
## # WARNING: the STAGE_SVCCLASS below is only needed for files on atlasdatadisk
## if athenaCommonFlags.BSRDOInput()[0].startswith("/castor/cern.ch/grid/atlas/atlasdatadisk/"):
##    import os
##    os.environ["STAGE_SVCCLASS"]='atldata'
## globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEONF-04-00-00')
## jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
## jobproperties.BField.barrelToroidOn.set_Value_and_Lock(False)
## jobproperties.BField.endcapToroidOn.set_Value_and_Lock(False)
## globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-001-03')
##############################################################################


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



# Run from an ESD file and produce an ntuple
#rec.readESD = True
#rec.doCBNT = True
#rec.doESD = False


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



# General Reco flags jobproperties 
from RecExConfig.RecFlags import rec

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

# Max number of events to run, number of events to skip
athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)
athenaCommonFlags.SkipEvents.set_Value_and_Lock(SkipEvents)


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
    if 'EventList' in dir():
        include("MuonRecExample/SelectEvents_jobOptions.py")

    include("RecExCommission/RecExCommission.py")                
    include ("RecExCommon/RecExCommon_topOptions.py")
    if rec.doPersint():
        from MboyView.MboyViewConf import MboyView
        theMboyView = MboyView()
        if muonRecFlags.doMuonboy():
            theMboyView.TrkTrackCollectionLocationList+=["ConvertedMBoyTracks"]
        if muonRecFlags.doMoore():
            theMboyView.TrkTrackCollectionLocationList+=["MooreTracks"]
            theMboyView.TrkSegmentCollectionLocationList+=["MooreSegments"]

    try:
        del topSequence.Stream2Filter
    except AttributeError:
        pass

    # remove TileMuonFitter topSequence (CPU)
    try:
       del topSequence.TileMuonFitter
    except AttributeError:
       pass
    # move triggeralg to end (calo has run after it)
    try:
        # keep link to alg
        MuonCalibExtraTreeTriggerAlg = topSequence.MuonCalibExtraTreeTriggerAlg
        # then remove it from topSequence
        del topSequence.MuonCalibExtraTreeTriggerAlg
    except AttributeError:
       pass
    else:
       # and add at end (after Tile)
       topSequence += MuonCalibExtraTreeTriggerAlg


    ###### put any user finetuning before this line #####

##### DO NOT ADD ANYTHING AFTER THIS LINE #####
except:
    # always write config so far for debugging
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
    # add DetFlags
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    dumpDetFlags("config.txt")
    # but still exit with error
    raise
else:
    # and write config to include user changes after topOptions
    from AthenaCommon.ConfigurationShelve import saveToAscii
    saveToAscii("config.txt")
    # add DetFlags
    from MuonRecExample.MuonRecUtils import dumpDetFlags
    dumpDetFlags("config.txt")

if rec.doPersint():
    from MboyView.MboyViewConf import MboyView
    theMboyView = MboyView()
    if muonRecFlags.doMuonboy():
        theMboyView.TrkTrackCollectionLocationList+=["ConvertedMBoyTracks"]
    if muonRecFlags.doMoore():
        theMboyView.TrkTrackCollectionLocationList+=["MooreTracks"]
        theMboyView.TrkSegmentCollectionLocationList+=["MooreSegments"]



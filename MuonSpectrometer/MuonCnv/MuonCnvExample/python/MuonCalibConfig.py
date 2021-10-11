# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# JobOption fragments for Condition Database access
# of muon calibration constants

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)
logMuon = logging.getLogger(__name__)

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon import CfgMgr
#--------------------------------------------------------------

from MuonCnvExample.MuonCnvUtils import specialAddFolderSplitOnline,mdtCalibWindowNumber
from MuonCnvExample.MuonCalibFlags import mdtCalibFlags,cscCalibFlags
mdtCalibFlags.setDefaults()
cscCalibFlags.setDefaults()

################################################################################
# CSC calibration
################################################################################

def setupCscCondDB():
    # Access to the CSC calibration database
    # Copied and modified from:
    # include( "CscCalibTools/CscCalibTool_jobOptions.py" )
    from MuonCondSvc.CscCondDB import cscCondDB

    ## Load Ped File
    if cscCalibFlags.CscPedFromLocalFile():
        cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
        cscCondDB.addPedFolder()    #<--- Adds pedestal and noise folders
        cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
    else:
        cscCondDB.addPedFolder()    #<--- Adds pedestal and noise folders
        
    ## Load Noise File
    if cscCalibFlags.CscNoiseFromLocalFile():
        cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
        cscCondDB.addNoiseFolder()    #<--- Adds pedestal and noise folders
        cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
    else:
        cscCondDB.addNoiseFolder()    #<--- Adds pedestal and noise folders

    ## Load PSlope File
    if cscCalibFlags.CscPSlopeFromLocalFile():
        cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
        cscCondDB.addPSlopeFolder()    #<--- Adds pedestal and noise folders
        cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
    else:
        cscCondDB.addPSlopeFolder()    #<--- Adds pedestal and noise folders

    ## Load Status File
    if cscCalibFlags.CscStatusFromLocalFile():
        cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
        cscCondDB.addStatusFolder()    #<--- Adds pedestal and noise folders
        cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
    else:
        cscCondDB.addStatusFolder()    #<--- Adds pedestal and noise folders

    ## Load Rms File
    if cscCalibFlags.CscRmsFromLocalFile():
        cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
        cscCondDB.addRmsFolder()    #<--- Adds pedestal and noise folders
        cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
    else:
        cscCondDB.addRmsFolder()    #<--- Adds pedestal and noise folders

    ## Load F001 File
    if cscCalibFlags.CscF001FromLocalFile():
        cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
        cscCondDB.addF001Folder()   
        cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
    else:
        cscCondDB.addF001Folder()   


    if not athenaCommonFlags.isOnline():
        log = logging.getLogger(__name__+".setupCscCondDB()")
        log.info("This is for OffLine so T0Base and T0Phase folders are added!!")

        ## Load T0Base File
        if cscCalibFlags.CscT0BaseFromLocalFile():
            cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
            cscCondDB.addT0BaseFolder()   
            cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
        else:
            cscCondDB.addT0BaseFolder()   


        ## Load T0Phase File
        if cscCalibFlags.CscT0PhaseFromLocalFile():
            cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
            cscCondDB.addT0PhaseFolder()   
            cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
        else:
            cscCondDB.addT0PhaseFolder()   

    #cscCondDB.addPedFolders()    #<--- Adds pedestal and noise folders
    #cscCondDB.addAllFolders()

# end of function setupCscCondDB()


def CscCalibTool(name,**kwargs):
    # without the following line, the CscCondDbAlg is not scheduled, thus add flake8 ignore flag
    import MuonCondAlg.CscCondDbAlgConfig # noqa: F401
    # make tool
    return CfgMgr.CscCalibTool(
        Slope=0.19,
        Noise=3.5,
        Pedestal=2048.0,
        ReadFromDatabase=True,
        SlopeFromDatabase=False,
        integrationNumber=12.0,
        integrationNumber2=11.66,
        samplingTime=50.0,
        signalWidth=14.40922,
        timeOffset=71.825,
        IsOnline = athenaCommonFlags.isOnline(),
        Latency = 119
        )
    #               timeOffset=46.825) + 25 SimHIT digit time


################################################################################
# MDT calibration
################################################################################


if mdtCalibFlags.readMDTCalibFromBlob():
    mdt_folder_name_appendix = "BLOB"
else:
    mdt_folder_name_appendix=""


def setupMdtCondDB():

    global mdt_folder_name_appendix
    from IOVDbSvc.CondDB import conddb

    if mdtCalibFlags.mdtCalibrationSource()=="MDT":
        if not conddb.folderRequested('/MDT/Onl/RT' + mdt_folder_name_appendix) and not conddb.folderRequested('/MDT/RT' + mdt_folder_name_appendix):
            conddb.addFolderSplitOnline("MDT", '/MDT/Onl/RT' + mdt_folder_name_appendix,'/MDT/RT' + mdt_folder_name_appendix, className='CondAttrListCollection')
        if not conddb.folderRequested('/MDT/Onl/T0' + mdt_folder_name_appendix) and not conddb.folderRequested('/MDT/T0' + mdt_folder_name_appendix):
            conddb.addFolderSplitOnline("MDT", '/MDT/Onl/T0' + mdt_folder_name_appendix,'/MDT/T0' + mdt_folder_name_appendix, className='CondAttrListCollection')
    else:
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr.TagInfoMgr.ExtraTagValuePairs.update({"MDTCalibrationSource": mdtCalibFlags.mdtCalibrationSource()})
        if not conddb.folderRequested('/MDT/Onl/RT' + mdt_folder_name_appendix) and not conddb.folderRequested('/MDT/RT' + mdt_folder_name_appendix):
            specialAddFolderSplitOnline(mdtCalibFlags.mdtCalibrationSource(), '/MDT/Onl/RT' + mdt_folder_name_appendix,'/MDT/RT' + mdt_folder_name_appendix)
        if not conddb.folderRequested('/MDT/Onl/T0' + mdt_folder_name_appendix) and not conddb.folderRequested('/MDT/T0' + mdt_folder_name_appendix):
            specialAddFolderSplitOnline(mdtCalibFlags.mdtCalibrationSource(), '/MDT/Onl/T0' + mdt_folder_name_appendix,'/MDT/T0' + mdt_folder_name_appendix)

    from AthenaCommon.AlgSequence import AthSequencer
    from MuonCondAlg.MuonCondAlgConf import MdtCalibDbAlg
    condSequence = AthSequencer("AthCondSeq")
    if not hasattr(condSequence,"MdtCalibDbAlg"):
        condSequence += MdtCalibDbAlg("MdtCalibDbAlg")

    if conddb.isOnline and not conddb.isMC:
        condSequence.MdtCalibDbAlg.ReadKeyTube = "/MDT/T0"
        condSequence.MdtCalibDbAlg.ReadKeyRt = "/MDT/RT"
    else:
        condSequence.MdtCalibDbAlg.ReadKeyTube = "/MDT/T0" + mdt_folder_name_appendix
        condSequence.MdtCalibDbAlg.ReadKeyRt = "/MDT/RT" + mdt_folder_name_appendix
    condSequence.MdtCalibDbAlg.RT_InputFiles = ["Muon_RT_default.data"]
    if globalflags.DataSource == 'data':
        condSequence.MdtCalibDbAlg.defaultT0 = 40
    elif globalflags.DataSource == 'geant4':
        condSequence.MdtCalibDbAlg.defaultT0 = 799
    condSequence.MdtCalibDbAlg.UseMLRt = mdtCalibFlags.useMLRt()
    condSequence.MdtCalibDbAlg.TimeSlewingCorrection = mdtCalibFlags.correctMdtRtForTimeSlewing()
    condSequence.MdtCalibDbAlg.MeanCorrectionVsR = [ -5.45973, -4.57559, -3.71995, -3.45051, -3.4505, -3.4834, -3.59509, -3.74869, -3.92066, -4.10799, -4.35237, -4.61329, -4.84111, -5.14524 ]
    condSequence.MdtCalibDbAlg.PropagationSpeedBeta = mdtCalibFlags.mdtPropagationSpeedBeta()
    # the same as MdtCalibrationDbTool
    condSequence.MdtCalibDbAlg.CreateBFieldFunctions = mdtCalibFlags.correctMdtRtForBField()
    condSequence.MdtCalibDbAlg.CreateWireSagFunctions = mdtCalibFlags.correctMdtRtWireSag()
    condSequence.MdtCalibDbAlg.CreateSlewingFunctions = mdtCalibFlags.correctMdtRtForTimeSlewing()

def MdtCalibrationTool(name="MdtCalibrationTool", **kwargs):
    from MdtCalibSvc.MdtCalibSvcConf import MdtCalibrationTool
    kwargs.setdefault("DoSlewingCorrection", mdtCalibFlags.correctMdtRtForTimeSlewing())

    # Hack to use DoTemperatureCorrection for applyRtScaling; but applyRtScaling should not be used anyway, since MLRT can be used
    kwargs.setdefault("DoTemperatureCorrection", mdtCalibFlags.applyRtScaling())
    kwargs.setdefault("DoWireSagCorrection", mdtCalibFlags.correctMdtRtWireSag())

    # for collisions cut away hits that are far outside of the MDT time window
    if jobproperties.Beam.beamType() == 'collisions':
        kwargs.setdefault("DoTofCorrection", True)

        if globalflags.DataSource() == 'geant4' or globalflags.DataSource() == 'data':
            kwargs.setdefault("TimeWindowSetting", mdtCalibWindowNumber('Collision_G4'))
    else: # cosmics or single beam
        kwargs.setdefault("DoTofCorrection", False)
    
    kwargs.setdefault("CalibrationDbTool", MdtCalibrationDbTool())

    return MdtCalibrationTool(name, **kwargs)

def MdtCalibrationDbTool(name="MdtCalibrationDbTool", **kwargs):
    from MdtCalibSvc.MdtCalibSvcConf import MdtCalibrationDbTool
    kwargs.setdefault("CreateBFieldFunctions", mdtCalibFlags.correctMdtRtForBField())
    kwargs.setdefault("CreateWireSagFunctions", mdtCalibFlags.correctMdtRtWireSag())
    kwargs.setdefault("CreateSlewingFunctions", mdtCalibFlags.correctMdtRtForTimeSlewing())
    kwargs.setdefault("WasConfigured", True)

    return MdtCalibrationDbTool(name, **kwargs)


# return a list of dictionaires containing the calib config info (keys etc)
def getCalibConfigs():
    from MuonRecExample.MuonRecFlags import muonRecFlags
    from RecExConfig.RecFlags import rec
    from MuonRecExample.MuonRec import muonRec
    configs = []
    if muonRecFlags.calibMuonStandalone and (muonRecFlags.doStandalone or rec.readESD):
        configs.append(muonRec.getConfig("MuonStandalone").getCalibConfig())
    else:
        muonRecFlags.calibMuonStandalone = False

    return configs
        
    

## Setup MuonSegmenToCalibSegment algorithm.
# @param[in] segmentKeys list of segment keys to use or single segments key (string)
def getMuonSegmentToCalibSegment():
    from MuonRecExample.MuonRecFlags import muonRecFlags
    from AthenaCommon.AppMgr import topSequence
    try:
        return topSequence.MuonSegmentToCalibSegment
    except AttributeError:
        from AthenaCommon.CfgGetter import getPublicTool
        from MuonCalibPatRec.MuonCalibPatRecConf import MuonCalib__MuonSegmentToCalibSegment
        MuonSegmentToCalibSegment = MuonCalib__MuonSegmentToCalibSegment("MuonSegmentToCalibSegment")
        # set consistent time-of-flight correction with MdtDriftCircleOnTrackCreator
        mdtCreator = getPublicTool("MdtDriftCircleOnTrackCreator")
        from AthenaCommon.ConfiguredFactory import getProperty
        MuonSegmentToCalibSegment.DoTOF = getProperty(mdtCreator,"DoTofCorrection")
        # when doing segment fits with floating t0's
        MuonSegmentToCalibSegment.UpdateForT0Shift = type(MuonSegmentToCalibSegment.getDefaultProperty("UpdateForT0Shift")) (muonRecFlags.doSegmentT0Fit())
        MuonSegmentToCalibSegment.UseCscSegments = False
        MuonSegmentToCalibSegment.SegmentLocations = [ "MuonSegments" ]
        MuonSegmentToCalibSegment.SegmentAuthors = [ 4,8 ] 
        MuonSegmentToCalibSegment.ReadSegments = True # rather than SegmentCombinations

        from MuonCnvExample import MuonCalibConfig
        MuonCalibConfig.setupMdtCondDB()

        MuonSegmentToCalibSegment.CalibrationTool = MuonCalibConfig.MdtCalibrationTool()

        # finally add it to topSequence
        topSequence += MuonSegmentToCalibSegment

    return topSequence.MuonSegmentToCalibSegment


## Setup the basic MuonCalibAlg algorithm. Needs further configuration after this call
# depending on the use-case.
# @param[in] evenTag The event tag written to file
def getMuonCalibAlg(eventTag):
    from AthenaCommon.AppMgr import topSequence
    from MuonRecExample.MuonRecFlags import muonRecFlags
    from RecExConfig.RecFlags import rec
    try:
        return topSequence.MuonCalibAlg
    except AttributeError:
        from MuonCalibAlgs.MuonCalibAlgsConf import MuonCalib__MuonCalibAlg
        MuonCalibAlg = MuonCalib__MuonCalibAlg("MuonCalibAlg",
                                               doMDTs = muonRecFlags.doMDTs(),
                                               doCSCs = muonRecFlags.doCSCs(),
                                               doRPCs = muonRecFlags.doRPCs(),
                                               doTGCs = (muonRecFlags.doTGCs() and muonRecFlags.calibNtupleRawTGC()),
                                               doTGCCoinData = (muonRecFlags.doTGCs() and muonRecFlags.calibNtupleRawTGC()),
                                               doTruth = rec.doTruth(),
                                               DoPrdSelect = muonRecFlags.doPrdSelect(),  # optional cutting away of PRD hits to simulate dead channels
                                               NtupleName = muonRecFlags.calibNtupleOutput(),  # set the name of the output calibration ntuple
                                               EventTag = eventTag )

        if jobproperties.Beam.beamType() == 'singlebeam' or jobproperties.Beam.beamType() == 'cosmics':
            MuonCalibAlg.addTriggerTag = False
        else:
            MuonCalibAlg.addTriggerTag = rec.doTrigger()
        MuonCalibAlg.doTruth=rec.doTruth()
        topSequence += MuonCalibAlg
        return topSequence.MuonCalibAlg


def setupMuonCalibNtuple():
    from MuonRecExample.MuonRecFlags import muonRecFlags
    from AthenaCommon.DetFlags import DetFlags
    from RecExConfig.RecFlags import rec
    from AthenaCommon.AppMgr import topSequence
    from AthenaCommon.AppMgr import ToolSvc
    if not rec.doMuon() or not DetFlags.Muon_on():
        logMuon.warning("Not setting up requested Muon Calibration Ntuple because Muons are off")
        return
    
    logMuon.info("Setting up Muon Calibration Ntuple")
    configs = getCalibConfigs()
    # MuonSegmentToCalibSegment is only needed if we want segments
    if muonRecFlags.calibNtupleSegments and muonRecFlags.calibMuonStandalone:
        getMuonSegmentToCalibSegment()

    # MuonCalibAlg is always needed
    eventTag="UNKNOWN"
    if (muonRecFlags.calibNtupleSegments or muonRecFlags.calibNtupleTracks) and muonRecFlags.calibMuonStandalone:
        if len(configs) >= 1:
            eventTag = configs[0]["eventTag"]
    elif muonRecFlags.calibNtupleTrigger:
        eventTag = "TRIG"
    else:
        eventTag = "HITS"

    MuonCalibAlg = getMuonCalibAlg(eventTag)
    # configure for writing ntuple
    from MuonCalibTools.MuonCalibToolsConf import MuonCalib__PatternNtupleMaker
    MuonCalibTool = MuonCalib__PatternNtupleMaker("MuonCalibPatternNtupleMaker")
    MuonCalibTool.FillTruth = rec.doTruth()
    MuonCalibTool.DoRawTGC = (muonRecFlags.doTGCs() and muonRecFlags.calibNtupleRawTGC())
    ToolSvc += MuonCalibTool
    MuonCalibAlg.MuonCalibTool = MuonCalibTool

    # MuonCalibExtraTree only if we want to write tracks
    if muonRecFlags.calibNtupleTracks:
        MuonCalibTool.DelayFinish = True
        from MuonCalibExtraTreeAlg.MuonCalibExtraTreeAlgConf import MuonCalib__MuonCalibExtraTreeAlg
        MuonCalibExtraTreeAlg = MuonCalib__MuonCalibExtraTreeAlg("MuonCalibExtraTreeAlg",
                                                                 NtupleName = "PatternNtupleMaker",
                                                                 )
        segmentOnTrackSelector=None
        if hasattr(topSequence, "MuonSegmentToCalibSegment"):
          from MuonCalibExtraTreeAlg.MuonCalibExtraTreeAlgConf import MuonCalib__SegmentOnTrackSelector
          segmentOnTrackSelector=MuonCalib__SegmentOnTrackSelector()
          segmentOnTrackSelector.PattternLocation = "PatternsForCalibration"
          ToolSvc+=segmentOnTrackSelector
          MuonCalibExtraTreeAlg.SegmentOnTrackSelector= segmentOnTrackSelector
        if not rec.doMuonCombined():
           tool_nr=0
           from AthenaCommon.CfgGetter import getPrivateToolClone
           for config in configs:
             MuonCalibExtraTreeAlg.TrackFillerTools.append(getPrivateToolClone("ExtraTreeTrackFillerTool"+str(tool_nr), "ExtraTreeTrackFillerTool", TrackCollectionKey=config['tracksKey'], SegmentAuthors=[config['segmentAuthor']], TrackAuthor=config['trackAuthor']))
             tool_nr+=1
        # configure needed tools


        # add to topSequence
        topSequence += MuonCalibExtraTreeAlg


    # MuonCalibExtraTreeTriggerAlg only if trigger is available
    if muonRecFlags.calibNtupleTrigger: # and DetFlags.detdescr.LVL1_on() and DetFlags.haveRDO.LVL1_on():

        # delay writing of MuonCalibAlg
        MuonCalibTool.DelayFinish = True
        # also delay MuonCalibExtraTreeAlg if it is running
        try:
            topSequence.MuonCalibExtraTreeAlg.DelayFinish = True
        except AttributeError:
            pass
        
        from MuonCalibExtraTreeAlg.MuonCalibExtraTreeAlgConf import MuonCalib__MuonCalibExtraTreeTriggerAlg
        topSequence += MuonCalib__MuonCalibExtraTreeTriggerAlg( 'MuonCalibExtraTreeTriggerAlg',
                                                                doMuCTPI   = True,
                                                                doLVL1Calo = rec.doTile() or rec.doLArg() or DetFlags.haveRDO.Calo_on(),
                                                                doMBTS     = rec.doTile() or DetFlags.haveRDO.Tile_on() )



def setupMuonCalib():
    from RecExConfig.RecFlags import rec
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.DetFlags import DetFlags
    if not rec.doMuon() or not DetFlags.Muon_on():
        logMuon.warning("Not setting up requested Muon Calibration because Muons are off")
        return

    logMuon.info("Setting up Muon Calibration")
    from MuonCnvExample.MuonCalibFlags import muonCalibFlags
    muonCalibFlags.setDefaults()

    getCalibConfigs()
    #
    # MuonSegmentToCalibSegment
    #
    from MuonRecExample.MuonRec import muonRec
    muonRec.allConfigs()[0].getCalibConfig() #muonRec.getConfig(muonCalibFlags.EventTag()).getCalibConfig()
    getMuonSegmentToCalibSegment()
    #
    # MuonCalibAlg
    #
    MuonCalibAlg = getMuonCalibAlg(muonCalibFlags.EventTag())

    from MdtCalibTools.MdtCalibToolsConf import MuonCalib__MdtCalibTool
    MuonCalibTool = MuonCalib__MdtCalibTool()
    calibMode = muonCalibFlags.Mode()
    if calibMode == 'regionNtuple':
        from MdtCalibTools.MdtCalibToolsConf import MuonCalib__MdtCalibNtupleMakerTool
        MdtCalibTool = MuonCalib__MdtCalibNtupleMakerTool()
    else:
        raise RuntimeError( "Unknown Muon Calibration Mode: %r" % calibMode )

    ToolSvc += MdtCalibTool
    MuonCalibTool.MdtCalibTool = MdtCalibTool

    ToolSvc += MuonCalibTool
    MuonCalibAlg.MuonCalibTool = MuonCalibTool


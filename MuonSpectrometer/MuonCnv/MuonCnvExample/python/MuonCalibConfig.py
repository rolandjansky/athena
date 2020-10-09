# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# JobOption fragments for Condition Database access
# of muon calibration constants

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam
from AthenaCommon.BFieldFlags import jobproperties
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
    import MuonCondAlg.CscCondDbAlgConfig # MT-safe conditions access
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
        MdtCalibDbAlg.TubeFolder = "/MDT/T0"
        MdtCalibDbAlg.RtFolder = "/MDT/RT"
        MdtCalibDbAlg.ReadKeyTube = "/MDT/T0"
        MdtCalibDbAlg.ReadKeyRt = "/MDT/RT"
    else:
        MdtCalibDbAlg.TubeFolder = "/MDT/T0" + mdt_folder_name_appendix
        MdtCalibDbAlg.RtFolder = "/MDT/RT" + mdt_folder_name_appendix
        MdtCalibDbAlg.ReadKeyTube = "/MDT/T0" + mdt_folder_name_appendix
        MdtCalibDbAlg.ReadKeyRt = "/MDT/RT" + mdt_folder_name_appendix
    MdtCalibDbAlg.RT_InputFiles = ["Muon_RT_default.data"]
    if globalflags.DataSource == 'data':
        MdtCalibDbAlg.defaultT0 = 40
    elif globalflags.DataSource == 'geant4':
        MdtCalibDbAlg.defaultT0 = 799
    MdtCalibDbAlg.UseMLRt = mdtCalibFlags.useMLRt()
    MdtCalibDbAlg.TimeSlewingCorrection = mdtCalibFlags.correctMdtRtForTimeSlewing()
    MdtCalibDbAlg.MeanCorrectionVsR = [ -5.45973, -4.57559, -3.71995, -3.45051, -3.4505, -3.4834, -3.59509, -3.74869, -3.92066, -4.10799, -4.35237, -4.61329, -4.84111, -5.14524 ]
    MdtCalibDbAlg.PropagationSpeedBeta = mdtCalibFlags.mdtPropagationSpeedBeta()
    # the same as MdtCalibrationDbTool
    MdtCalibDbAlg.CreateBFieldFunctions = mdtCalibFlags.correctMdtRtForBField()
    MdtCalibDbAlg.CreateWireSagFunctions = mdtCalibFlags.correctMdtRtWireSag()
    MdtCalibDbAlg.CreateSlewingFunctions = mdtCalibFlags.correctMdtRtForTimeSlewing()

    from MdtCalibSvc.MdtCalibSvcConf import MdtCalibrationTool
    MdtCalibrationTool.DoSlewingCorrection = mdtCalibFlags.correctMdtRtForTimeSlewing()
    # Hack to use DoTemperatureCorrection for applyRtScaling; but applyRtScaling should not be used anyway, since MLRT can be used
    MdtCalibrationTool.DoTemperatureCorrection = mdtCalibFlags.applyRtScaling()
    MdtCalibrationTool.DoWireSagCorrection = mdtCalibFlags.correctMdtRtWireSag()
    # for collisions cut away hits that are far outside of the MDT time window
    if beamFlags.beamType() == 'collisions':
        MdtCalibrationTool.DoTofCorrection = True
        if globalflags.DataSource() == 'geant4':
            MdtCalibrationTool.TimeWindowSetting = mdtCalibWindowNumber('Collision_G4')
        elif globalflags.DataSource() == 'data':
            MdtCalibrationTool.TimeWindowSetting = mdtCalibWindowNumber('Collision_G4')
    else: # cosmics or single beam
        MdtCalibrationTool.DoTofCorrection = False

    from MdtCalibSvc.MdtCalibSvcConf import MdtCalibrationDbTool
    MdtCalibrationDbTool.CreateBFieldFunctions = mdtCalibFlags.correctMdtRtForBField()
    MdtCalibrationDbTool.CreateWireSagFunctions = mdtCalibFlags.correctMdtRtWireSag()
    MdtCalibrationDbTool.CreateSlewingFunctions = mdtCalibFlags.correctMdtRtForTimeSlewing()


# return a list of dictionaires containing the calib config info (keys etc)
def getCalibConfigs():
    global muonRecFlags,rec
    configs = []
    if muonRecFlags.calibMuonStandalone and (muonRecFlags.doStandalone or rec.readESD):
#        try:
            configs.append( muonRec.getConfig("MuonStandalone").getCalibConfig() )
#        except KeyError:
#            logMuon.warning("Could not get calibration config for MuonStandAlone - not adding MuonStandAlone info to calibration")
#            muonRecFlags.calibMuonStandalone = False
    else:
        muonRecFlags.calibMuonStandalone = False

    return configs
        
    

## Setup MuonSegmenToCalibSegment algorithm.
# @param[in] segmentKeys list of segment keys to use or single segments key (string)
def getMuonSegmentToCalibSegment():
    global topSequence,muonRecFlags
    try:
        return topSequence.MuonSegmentToCalibSegment
    except AttributeError:
            
        from MuonCalibPatRec.MuonCalibPatRecConf import MuonCalib__MuonSegmentToCalibSegment
        MuonSegmentToCalibSegment = MuonCalib__MuonSegmentToCalibSegment("MuonSegmentToCalibSegment")
        # set consistent time-of-flight correction with MdtDriftCircleOnTrackCreator
        mdtCreator = getPublicTool("MdtDriftCircleOnTrackCreator")
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
    global topSequence,beamFlags
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

        if beamFlags.beamType == 'singlebeam' or beamFlags.beamType == 'cosmics':
            MuonCalibAlg.addTriggerTag = False
        else:
            MuonCalibAlg.addTriggerTag = rec.doTrigger()
        MuonCalibAlg.doTruth=rec.doTruth()
        topSequence += MuonCalibAlg
        return topSequence.MuonCalibAlg


def setupMuonCalibNtuple():
    global topSequence,muonRecFlags,beamFlags,ToolSvc,rec,DetFlags
    if not rec.doMuon() or not DetFlags.Muon_on():
        logMuon.warning("Not setting up requested Muon Calibration Ntuple because Muons are off")
        return
    
    logMuon.info("Setting up Muon Calibration Ntuple")
    try:
        configs = getCalibConfigs()
        # MuonSegmentToCalibSegment is only needed if we want segments
        if muonRecFlags.calibNtupleSegments and muonRecFlags.calibMuonStandalone:
            MuonSegmentToCalibSegment = getMuonSegmentToCalibSegment()

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
               from MuonCalibExtraTreeAlg.MuonCalibExtraTreeAlgConf import MuonCalib__ExtraTreeTrackFillerTool
               resPullCalc=getPublicTool("ResidualPullCalculator")
               for config in configs:
                 trackDumpTool = MuonCalib__ExtraTreeTrackFillerTool("ExtraTreeTrackFillerTool" + str(tool_nr))
                 trackDumpTool.TrackCollectionKey = config['tracksKey']
                 trackDumpTool.SegmentAuthors = [config['segmentAuthor']]
                 trackDumpTool.TrackAuthor = config['trackAuthor']
                 trackDumpTool.PullCalculator = resPullCalc
                 ToolSvc+=trackDumpTool
                 MuonCalibExtraTreeAlg.TrackFillerTools.append(trackDumpTool)
                 tool_nr+=1
            # configure needed tools


            # add to topSequence
            topSequence += MuonCalibExtraTreeAlg


        # MuonCalibExtraTreeTriggerAlg only if trigger is available
        if muonRecFlags.calibNtupleTrigger: # and DetFlags.detdescr.LVL1_on() and DetFlags.haveRDO.LVL1_on():
            # protect against running without AtlasTrigger project
            doMuCTPI = True
            if doMuCTPI:
                try:
                    from TrigT1RPCRecRoiSvc import TrigT1RPCRecRoiConfig
                    from TrigT1TGCRecRoiSvc import TrigT1TGCRecRoiConfig
                except ImportError:
                    logMuon.warning("MuonCalibExtraTreeTriggerAlg.doMuCTPI = False because AtlasTrigger is not available")
                    doMuCTPI = False

            # delay writing of MuonCalibAlg
            MuonCalibTool.DelayFinish = True
            # also delay MuonCalibExtraTreeAlg if it is running
            try:
                topSequence.MuonCalibExtraTreeAlg.DelayFinish = True
            except AttributeError:
                pass
            
            from MuonCalibExtraTreeAlg.MuonCalibExtraTreeAlgConf import MuonCalib__MuonCalibExtraTreeTriggerAlg
            topSequence += MuonCalib__MuonCalibExtraTreeTriggerAlg( 'MuonCalibExtraTreeTriggerAlg',
                                                                    doMuCTPI   = doMuCTPI,
                                                                    doLVL1Calo = rec.doTile() or rec.doLArg() or DetFlags.haveRDO.Calo_on(),
                                                                    doMBTS     = rec.doTile() or DetFlags.haveRDO.Tile_on() )


    except:
        from AthenaCommon.Resilience import treatException
        treatException("Problem in MuonCalib - Muon Calibration Ntuple configuration probably incomplete")



def setupMuonCalib():
    global topSequence,ToolSvc
    if not rec.doMuon() or not DetFlags.Muon_on():
        logMuon.warning("Not setting up requested Muon Calibration because Muons are off")
        return

    logMuon.info("Setting up Muon Calibration")
    try:
        from MuonCnvExample.MuonCalibFlags import muonCalibFlags
        muonCalibFlags.setDefaults()

        configs = getCalibConfigs()
        #
        # MuonSegmentToCalibSegment
        #
        calibConfig = muonRec.allConfigs()[0].getCalibConfig() #muonRec.getConfig(muonCalibFlags.EventTag()).getCalibConfig()
        MuonSegmentToCalibSegment = getMuonSegmentToCalibSegment()
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

    except:
        from AthenaCommon.Resilience import treatException
        treatException("Problem in MuonCalib - Muon Calibration configuration probably incomplete")

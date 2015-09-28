# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os

from AthenaCommon.Logging import logging

log = logging.getLogger()
log.info("Loading module %s",__name__)

from AthenaCommon import CfgMgr
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

from RecExConfig.RecFlags import rec

from MuonRecFlags import muonRecFlags
from MuonRecUtils import logMuon,logMuonResil,override_lock_and_set_Value
from MuonRec import muonRec

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,getService,getServiceClone
from AthenaCommon.ConfiguredFactory import getProperty

muonRecFlags.setDefaults()

topSequence = AlgSequence()

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
# @input @c segmentKeys list of segment keys to use or single segments key (string)
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

        getService("MdtCalibrationSvc")

        # finally add it to topSequence
        topSequence += MuonSegmentToCalibSegment

    return topSequence.MuonSegmentToCalibSegment


## Setup the basic MuonCalibAlg algorithm. Needs further configuration after this call
# depending on the use-case.
# @input @c evenTag The event tag written to file
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


# chose the setup
if muonRecFlags.doCalibNtuple():
    setupMuonCalibNtuple()
elif muonRecFlags.doCalib():
    setupMuonCalib()
else:
    logMuon.warning("Loading %s but not setting up any MuonCalibration or Ntuple" % __name__ )

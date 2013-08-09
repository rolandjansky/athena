# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.ConfigurableDb import getConfigurable
from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
from AthenaCommon.Logging import logging
# flags
from MuonCnvExample.MuonCalibFlags import muonCalibFlags
from MuonRecExample.MuonRecFlags import muonRecFlags
from RecExConfig.RecFlags import rec as recFlags
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

import os

log = logging.getLogger("MuonCalib")

muonCalibFlags.setDefaults()

topSequence = AlgSequence()

MuonSegmentToCalibSegment = None
MuonCalibAlg = None
MuonCalibTool = None
MdtCalibTool = None
MuonCalibExtraTreeAlg = None


class MuonCalibConfig:
    """Configuration object holding the info of the MuonCalibration inputs and outputs"""
    __slots__ = ( 'eventTag', 'segmentsKey', 'segmentCombisKey', 'doPhi', 'doTracks',
                  'patternsKey', 'tracksKey' )
    def __init__(self,**kwargs):
        for n,v in kwargs.items():
            setattr(self,n,v)

    def __str__(self):
        """Print the attributes that have been set. One per line"""
        lines = []
        for n in self.__slots__:
          try:
             v = getattr(self,n)
             lines.append( "%s=%r" % (n,v) )
          except AttributeError:
             pass
        return os.linesep.join(lines)

# end of class MuonCalibConfig

#
# A list of standard configurations
#
MoMuConfig = MuonCalibConfig(
    eventTag = "MoMu",
    segmentCombisKey = "MdtSegmentCombinations",
    doPhi = True,
    doTracks = True,
    patternsKey = "MuonHoughPatternCombinations",
    tracksKey = "MooreTracks" )

MooreConfig = MuonCalibConfig(
    eventTag = "Moore",
    segmentsKey = "MooreSegments_",
    doPhi = True,
    doTracks = True,
    patternsKey = "phi_patterns_",
    tracksKey = "ConvertedMooreTracks" )

MuonboyConfig = MuonCalibConfig(
    eventTag = "Muonboy",
    segmentsKey = "ConvertedMBoySegments",
    doPhi = False,
    doTracks = True,
    patternsKey = "????",
    tracksKey = "ConvertedMBoyMuonSpectroOnlyTracks" )
    
MoMuMooreConfig = MuonCalibConfig(
    eventTag = "MoMuMoore",
    segmentCombisKey = "MdtSegmentCombinations",
    doPhi = True,
    doTracks = True,
    patternsKey = "MuonHoughPatternCombinations",
    tracksKey = "ConvertedMooreTracks" )


def configure(config):
    """Configure the calibration input and output.
    If both segmentsKey and segmentCombisKey are defined, segmentsKey has priority."""
    global topSequence,ToolSvc,MdtCalibTool, MuonSegmentToCalibSegment, MuonCalibAlg, MuonCalibTool, MuonCalibExtraTreeAlg, muonRecFlags
    log.info("Configuring with %s", config)
    
    calibMode = muonCalibFlags.Mode()
    #
    # Configure MuonSegmentToCalibSegment
    #
    try:
        MuonSegmentToCalibSegment = topSequence.MuonSegmentToCalibSegment
    except AttributeError:
        from MuonCalibPatRec.MuonCalibPatRecConf import MuonCalib__MuonSegmentToCalibSegment
        MuonSegmentToCalibSegment = MuonCalib__MuonSegmentToCalibSegment("MuonSegmentToCalibSegment")
        topSequence += MuonSegmentToCalibSegment
    MuonSegmentToCalibSegment.UseCscSegments = muonRecFlags.doCSCs()

    # segmentsKey
    try:
        segmentsKey = config.segmentsKey
    except AttributeError:
        MuonSegmentToCalibSegment.ReadSegments = False
    else:
        MuonSegmentToCalibSegment.SegmentLocation = segmentsKey
        MuonSegmentToCalibSegment.ReadSegments = True
    # segmentCombisKey
    try:
        segmentCombisKey = config.segmentCombisKey
    except AttributeError:
        pass
    else:
        MuonSegmentToCalibSegment.SegmentCombiLocation = segmentCombisKey
    
    #
    # MuonCalibAlg
    #
    try:
        MuonCalibAlg = topSequence.MuonCalibAlg
    except AttributeError:
        from MuonCalibAlgs.MuonCalibAlgsConf import MuonCalib__MuonCalibAlg
        MuonCalibAlg = MuonCalib__MuonCalibAlg( "MuonCalibAlg" )
        topSequence += MuonCalibAlg
    if beamFlags.beamType() == 'singlebeam' or beamFlags.beamType() == 'cosmics':
        MuonCalibAlg.addTriggerTag = False
    else:
        MuonCalibAlg.addTriggerTag = recFlags.doTrigger()
    MuonCalibAlg.doMDTs = muonRecFlags.doMDTs()
    MuonCalibAlg.doCSCs = muonRecFlags.doCSCs()
    MuonCalibAlg.doRPCs = muonRecFlags.doRPCs()
    MuonCalibAlg.doTGCs = muonRecFlags.doTGCs()
    try:
        eventTag = config.eventTag
    except AttributeError:
        pass
    else:
        MuonCalibAlg.EventTag = eventTag

    MuonCalibAlg.doTruth = recFlags.doTruth()


    if calibMode == 'ntuple' or calibMode == 'trackNtuple':
        from MuonCalibTools.MuonCalibToolsConf import MuonCalib__PatternNtupleMaker
        MuonCalibTool = MuonCalib__PatternNtupleMaker()
        if calibMode == 'trackNtuple':
            if muonRecFlags.doSegmentsOnly():
                log.warning("Making ntuple instead of trackNtuple because making segments only")
                muonCalibFlags.Mode = 'ntuple'
            else:
                MuonCalibTool.DelayFinish = True
                try:
                    MuonCalibExtraTreeAlg = topSequence.MuonCalibExtraTreeAlg
                except AttributeError:
                    from MuonCalibExtraTreeAlg.MuonCalibExtraTreeAlgConf import MuonCalib__MuonCalibExtraTreeAlg
                    MuonCalibExtraTreeAlg = MuonCalib__MuonCalibExtraTreeAlg("MuonCalibExtraTreeAlg")
                    MuonCalibExtraTreeAlg.NtupleName = "PatternNtupleMaker"
                    topSequence += MuonCalibExtraTreeAlg

                # doPhi
                try:
                    doPhi = config.doPhi
                except AttributeError:
                    doPhi = False
                MuonCalibExtraTreeAlg.doPhi = doPhi
                # doTracks
                try:
                    doTracks = config.doTracks
                except AttributeError:
                    doTracks = False
                MuonCalibExtraTreeAlg.doTracks = doTracks
                # patternsKey
                try:
                    patternsKey = config.patternsKey
                except AttributeError:
                    pass
                else:
                    MuonCalibExtraTreeAlg.PatternLocation = patternsKey
                # tracksKey
                try:
                    tracksKey = config.tracksKey
                except AttributeError:
                    pass
                else:
                    MuonCalibExtraTreeAlg.TrackLocation = tracksKey

    else: # other calib modes
        from MdtCalibTools.MdtCalibToolsConf import MuonCalib__MdtCalibTool
        MuonCalibTool = MuonCalib__MdtCalibTool()
        if calibMode == 'regionNtuple':
            from MdtCalibTools.MdtCalibToolsConf import MuonCalib__MdtCalibNtupleMakerTool
            MdtCalibTool = MuonCalib__MdtCalibNtupleMakerTool()
        else:
            raise RuntimeError( "Unknown Muon Calibration Mode: %r" % calibMode )

        ToolSvc += MdtCalibTool
        MuonCalibTool.MdtCalibTool = MdtCalibTool

    ToolSvc += MuonCalibTool
    MuonCalibAlg.MuonCalibTool = MuonCalibTool



def printConfig():
    global MuonSegmentToCalibSegment,MuonCalibAlg,MuonCalibTool,MdtCalibTool,MuonCalibExtraTreeAlg

    print MuonSegmentToCalibSegment
    print MuonCalibAlg
    print MuonCalibTool
    print MdtCalibTool
    print MuonCalibExtraTreeAlg


#
# Activate default based on muonCalibFlags.EventTag
#
eventTag = muonCalibFlags.EventTag()
for c in vars().values():
    if isinstance(c,MuonCalibConfig) and c.eventTag == eventTag:
        configure(c)
        break
else:
    log.warning("No standard configuration for muonCalibFlags.EventTag = %r. User needs to call MuonCalib.configure() by hand.", eventTag)
del c
del eventTag

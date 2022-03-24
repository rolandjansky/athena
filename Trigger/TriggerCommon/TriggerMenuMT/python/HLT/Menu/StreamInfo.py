# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import eformat
from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )

_allowed_tag_types = [eformat.helper.tagtype_to_string(t) for t in eformat.helper.TagType.values.values()]

class StreamInfo(object):
    def __init__(self, name, streamType, obeysLumiBlock, forceFullEventBuilding):
        assert type(name) == str, "name has to be str"
        assert type(streamType) == str, "streamType has to be str"
        assert streamType in _allowed_tag_types, "streamType '"+streamType+"' is not one of "+\
                                                 "the allowed types: "+str(_allowed_tag_types)
        assert type(obeysLumiBlock) == bool, "obeysLumiBlock has to be bool"
        assert type(forceFullEventBuilding) == bool, "forceFullEventBuilding has to be bool"
        self.__data = [name, streamType, obeysLumiBlock, forceFullEventBuilding]

    def __str__(self):
        return '({}, obeysLumiBlock={}, forceFullEventBuilding={})'.format(
            self.typeName(), self.obeysLumiBlock(), self.forceFullEventBuilding())

    def name(self):
        return self.__data[0]

    def type(self):
        return self.__data[1]

    def obeysLumiBlock(self):
        return self.__data[2]

    def forceFullEventBuilding(self):
        return self.__data[3]

    def typeName(self):
        return '{:s}_{:s}'.format(self.type(), self.name())


_all_streams = [
    # PHYSICS STREAMS
    StreamInfo('Main', 'physics', True, True),
    StreamInfo('CosmicMuons', 'physics', True, True),
    StreamInfo('CosmicCalo', 'physics', True, True),
    StreamInfo('IDCosmic', 'physics', True, True),
    StreamInfo('ZeroBias', 'physics', True, True),
    StreamInfo('Background', 'physics', True, True),
    StreamInfo('Standby', 'physics', True, True),
    StreamInfo('L1Calo', 'physics', True, True),
    StreamInfo('EnhancedBias', 'physics', True, True),
    StreamInfo('Late', 'physics', True, True),
    # TLA/PEB/DATA SCOUTING (physics) STREAMS
    StreamInfo('TLA','physics',True,False),
    # EXPRESS STREAM
    StreamInfo('express', 'express', True, True),
    # MONITORING STREAMS
    StreamInfo('IDMonitoring', 'monitoring', True, True),
    StreamInfo('CSC', 'monitoring', True, False),
    # CALIBRATION STREAMS
    StreamInfo('Muon_Calibration','calibration',False,True),
    StreamInfo('BphysPEB','calibration',True,False),
    StreamInfo('BeamSpot', 'calibration', True, False),
    StreamInfo('LArCells', 'calibration', False, False),
    StreamInfo('LArCellsEmpty', 'calibration', False, False),
    StreamInfo('LArNoiseBurst', 'calibration', False, True),
    StreamInfo('TgcNoiseBurst', 'calibration', False, True),
    StreamInfo('RPCSecondaryReadout', 'calibration', False, False),
    StreamInfo('CostMonitoring', 'calibration', False, False),
    StreamInfo('SCTNoise', 'calibration', False, False),
    StreamInfo('Tile', 'calibration', False, False),
    StreamInfo('ALFACalib', 'calibration', False, False),
    StreamInfo('LArPEB', 'calibration', False, False),
    StreamInfo('LArPEBDigitalTrigger', 'calibration', False, False),
    StreamInfo('L1TopoMismatches', 'calibration', False, True),
    StreamInfo('ZDCCalib', 'calibration', False, False),
    StreamInfo('IDCalib', 'calibration', False, False),
    StreamInfo('AFPCalib', 'calibration', False, False),
    # HI STREAMS
    StreamInfo('HardProbes', 'physics', True, True),
    StreamInfo('MinBias', 'physics', True, True),
    # DELAYED STREAMS
    StreamInfo('VBFDelayed'  , 'physics', True, True),
    StreamInfo('BphysDelayed', 'physics', True, True),
    # Special stream to be used only for special chains rejecting all events like timeburner
    StreamInfo('DISCARD', 'unknown', False, True),

    # Add new streams grouped by type as above, not at the end of the list
]


def getAllStreams():
    return _all_streams


def getStreamTag(streamName):
    matches = [s for s in _all_streams if s.name() == streamName]
    if len(matches) == 0:
        log.error('Stream %s is not defined in StreamInfo', streamName)
        return None
    elif len(matches) > 1:
        log.error('Stream %s has multiple definitions in StreamInfo', streamName)
        return None
    else:
        return matches[0]


def getStreamTags(streamNames):
    return [getStreamTag(name) for name in streamNames]

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from six.moves import UserList

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )


class StreamInfo(UserList):
    def __init__(self, name, streamType, obeysLumiBlock, forceFullEventBuilding):
        self.data = [name, streamType, obeysLumiBlock, forceFullEventBuilding]

    def name(self):
        return self.data[0]

    def type(self):
        return self.data[1]

    def obeysLumiBlock(self):
        return self.data[2]

    def forceFullEventBuilding(self):
        return self.data[3]

    def typeName(self):
        return '{:s}_{:s}'.format(self.type(), self.name())


_all_streams = [
    # PHYSICS STREAMS
    StreamInfo('Main', 'physics', True, True),
    StreamInfo('CosmicMuons','physics',True,True),
    StreamInfo('CosmicCalo','physics',True,True),
    StreamInfo('IDCosmic','physics',True,True),
    # EXPRESS STREAM
    StreamInfo('express', 'express', True, True),
    # MONITORING STREAM
    StreamInfo('IDMonitoring','monitoring', True, True),
    # CALIBRATION STREAMS
    StreamInfo('BeamSpot', 'calibration', True, False),
    StreamInfo('LArCells', 'calibration', False, False),
    StreamInfo('RPCSecondaryReadout', 'calibration', False, False),
    StreamInfo('CostMonitoring','calibration', False, False),
    StreamInfo('SCTNoise','calibration',False,False),
    StreamInfo('Tile','calibration',False,False),
    # HI STREAMS
    StreamInfo('HardProbes', 'physics', True, True),
    StreamInfo('MinBias', 'physics', True, True),
    # Special stream to be used only for special chains rejecting all events like timeburner
    StreamInfo('DISCARD', 'unknown', False, False)
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

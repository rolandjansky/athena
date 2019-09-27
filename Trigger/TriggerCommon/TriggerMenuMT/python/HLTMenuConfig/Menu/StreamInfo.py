# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import UserList
from AthenaCommon.Logging import logging
log = logging.getLogger('TriggerMenuMT.HLTMenuConfig.Menu.StreamInfo')


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
    # EXPRESS STREAM
    StreamInfo('express', 'express', True, True),
    # CALIBRATION STREAMS
    StreamInfo('BeamSpot', 'calibration', False, False),
    StreamInfo('LArCells', 'calibration', True, False),
    StreamInfo('RPCSecondaryReadout', 'calibration', True, False),
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

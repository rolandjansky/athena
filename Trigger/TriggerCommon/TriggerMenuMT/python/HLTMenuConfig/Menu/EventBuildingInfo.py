# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigEDMConfig import DataScoutingInfo
from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )

'''
This file defines Event Building identifiers which can be used in chain names.
To avoid circular dependency between TrigEDMConfig and Menu, the Data Scouting
identifiers are moved to TrigEDMConfig.DataScoutingInfo. Nevertheless, Data
Scouting is just a special case of Partial Event Building.

When adding new identifiers, please follow the naming convention
    SomeNamePEBVariant for Partial Event Building (here) or
    SomeNameDSVariant for Data Scouting (in DataScoutingInfo),
where SomeName and Variant are generally camel-case unless they're acronyms
like LAr or RPC. Variant is normally empty unless there are several variants
like RPCPEB and RPCPEBSecondaryReadout.

Possible examples:
LArPEB, LumiPEB, RPCPEB, RPCPEBSecondaryReadout, TrkPEB, JetDS, PhotonDS
'''


# PEB identifiers
PartialEventBuildingIdentifiers = [
    'BeamSpotPEB',
    'LArPEB',
    'RPCPEBSecondaryReadout',
    'SCTPEB',
    'TilePEB',
    'AlfaPEB',
    'CSCPEB'
]


def getAllPartialEventBuildingIdentifiers():
    return PartialEventBuildingIdentifiers


def getAllEventBuildingIdentifiers():
    return PartialEventBuildingIdentifiers + DataScoutingInfo.getAllDataScoutingIdentifiers()

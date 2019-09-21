# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('TriggerMenuMT.HLTMenuConfig.Menu.EventBuildingInfo')

'''
This file defines Event Building identifiers which can be used in chain names.
Please follow the naming convention SomeNamePEBVariant or SomeNameDSVariant,
where SomeName and Variant are generally camel-case unless they're acronyms
like LAr or RPC. Variant is normally empty unless there are several variants
like RPCPEB and RPCPEBSecondaryReadout.

Possible examples:
LArPEB, LumiPEB, RPCPEB, RPCPEBSecondaryReadout, TrkPEB, JetDS, PhotonDS
'''

# PEB identifiers
PartialEventBuildingIdentifiers = [
  'LArPEB',
  'RPCPEBSecondaryReadout'
]

# Data scouting identifiers and the corresponding HLT result ROBFragment module IDs
# WARNING: Never change the module IDs during data taking!
DataScoutingIdentifiers = {
  'JetDS': 5,
  'PhotonDS': 6
}

# Add DS identifiers to the allowed names
AllowedEventBuildingIdentifiers = []
AllowedEventBuildingIdentifiers.extend(PartialEventBuildingIdentifiers)
AllowedEventBuildingIdentifiers.extend(DataScoutingIdentifiers.keys())


def getDataScoutingResultID(name):
    if name in DataScoutingIdentifiers:
        return DataScoutingIdentifiers[name]
    else:
        log.error('Unknown name %s, cannot assign result ID', name)


def getAllDataScoutingResultIDs():
    return DataScoutingIdentifiers.values()


def getFullHLTResultID():
    # WARNING: Don't change this, it should always be zero.
    # It's defined here to avoid using a magic number in other places
    return 0


def getAllPartialEventBuildingIdentifiers():
    return DataScoutingIdentifiers.values()


def getAllEventBuildingIdentifiers():
    return AllowedEventBuildingIdentifiers

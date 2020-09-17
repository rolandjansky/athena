# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )

'''
This file defines the Data Scouting chain identifiers which serve also
as EDM target identifiers and their mapping to HLT Result module IDs.

When adding new identifiers, please follow the naming convention SomeNameDS,
where SomeName is generally camel-case unless it's an acronym like LAr or MET.

Possible examples:
CostMonDS, JetDS, PhotonDS
'''


# Data scouting identifiers and the corresponding HLT result ROBFragment module IDs
# WARNING: Never change the module IDs during data taking!
# WARNING: ID=0 is reserved for full HLT result
DataScoutingIdentifiers = {
    'CostMonDS': 1,
    'JetDS': 5
}


# Truncation thresholds (in bytes) for each HLT result type
TruncationThresholds = {
    0: 5*(1024**2),  # Main: 5 MB
    1: 2*(1024**2),  # CostMonDS: 2 MB
    5: 1*(1024**2),  # JetDS: 1 MB
}


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


def getAllDataScoutingIdentifiers():
    return list(DataScoutingIdentifiers.keys())

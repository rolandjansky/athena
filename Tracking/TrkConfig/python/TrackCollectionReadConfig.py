#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloBCIDAvgAlgConfig.py
# Created: Nov 2019, sss
# Purpose: Configure AthReadAlg for reading a TrackCollection.
#
# The TrackCollection converter has some conditions dependencies,
# so to read TrackCollection property in MT, we need to use an algorithm.

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def _guessTracksAlias (tcolls):
    """Given the list of TrackCollection keys from the input file, try to guess
which one is aliased to Tracks."""

    for a in ['CombinedInDetTracks',
              'MergedTracks',
              'DBMTracksDetailedTruth',
              'InDetPseudoTracks',
              'ConvertedIPatTracks',
              'ConvertedXKalmanTracks',
              'RefittedTracks']:
        if a in tcolls: return a
    return None


def TrackCollectionReadCfg (flags, key):
    """Configure an algorithm to read a TrackCollection.
KEY is the desired TrackCollection key.
Will do nothing in serial mode, or if the requested object is not
in the input file."""

    # Don't do anything if we're in serial mode.
    result = ComponentAccumulator()
    if flags.Concurrency.NumThreads <= 0:
        return result

    # Get the list of TrackCollection keys from the input file,
    # and try to guess the Tracks alias.
    from AthenaConfiguration.AutoConfigFlags import GetFileMD
    tcolls = set ([k for t,k in GetFileMD (flags.Input.Files).get('itemList', [])
                   if t == 'TrackCollection'])
    tracks_alias = _guessTracksAlias (tcolls)

    # If Tracks was requested, try to change to the target of the alias.
    if key == 'Tracks' and tracks_alias is not None and key not in tcolls:
        key = tracks_alias

    # Do nothing if the object is not in the input file.
    if key not in tcolls:
        return result

    # Mention Tracks as an alias if appropriate.
    aliases = []
    if key == tracks_alias and 'Tracks' not in tcolls:
        aliases = ['Tracks']

    # Configure the algorithm.
    AthReadAlg=CompFactory.AthReadAlg
    alg = AthReadAlg ('TrackCollectionRead_' + key,
                      Key = 'TrackCollection/' + key,
                      Aliases = aliases,
                      ExtraInputs = [('InDetDD::SiDetectorElementCollection', 'ConditionStore+PixelDetectorElementCollection'),
                                     ('InDetDD::SiDetectorElementCollection', 'ConditionStore+SCT_DetectorElementCollection'), 
                                     ( 'InDetDD::TRT_DetElementContainer' , 'ConditionStore+TRT_DetElementContainer' ) ])
    result.addEventAlgo (alg)

    # We also require AddressRemappingSvc.
    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    result.merge (AddressRemappingCfg())
    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags()

    print ('--- Reference aliased object by base name.')
    flags1 = ConfigFlags.clone()
    flags1.Concurrency.NumThreads = 1
    flags1.Input.Files = defaultTestFiles.ESD
    flags1.lock()
    acc1 = TrackCollectionReadCfg (flags1, 'CombinedInDetTracks')
    acc1.printConfig (summariseProps=True)
    acc1.wasMerged()

    print ('--- Reference aliased object by alias.')
    flags2 = ConfigFlags.clone()
    flags2.Concurrency.NumThreads = 1
    flags2.Input.Files = defaultTestFiles.ESD
    flags2.lock()
    acc2 = TrackCollectionReadCfg (flags2, 'Tracks')
    acc2.printConfig (summariseProps=True)
    acc2.wasMerged()

    print ('--- Non-aliased object.')
    flags3 = ConfigFlags.clone()
    flags3.Concurrency.NumThreads = 1
    flags3.Input.Files = defaultTestFiles.ESD
    flags3.lock()
    acc3 = TrackCollectionReadCfg (flags3, 'ResolvedForwardTracks')
    acc3.printConfig (summariseProps=True)
    acc3.wasMerged()

    print ('--- Non-existent object.')
    flags4 = ConfigFlags.clone()
    flags4.Concurrency.NumThreads = 1
    flags4.Input.Files = defaultTestFiles.ESD
    flags4.lock()
    acc4 = TrackCollectionReadCfg (flags4, 'FooTracks')
    acc4.printConfig (summariseProps=True)
    acc4.wasMerged()

    print ('--- Non-threaded.')
    flags5 = ConfigFlags.clone()
    flags5.Input.Files = defaultTestFiles.ESD
    flags5.lock()
    acc5 = TrackCollectionReadCfg (flags5, 'Tracks')
    acc5.printConfig (summariseProps=True)
    acc5.wasMerged()

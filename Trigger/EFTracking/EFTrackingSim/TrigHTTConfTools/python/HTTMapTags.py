# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
'''
@file HTTMapTags.py
@author Riley Xu - riley.xu@cern.ch
@date March 3rd 2020

This file declares a set of tags that the map service uses to select files.
This allows for a consistent selection of map files for downstream uses,
such as generating sectors or optimizing DC configuration.

All options can be further overridden from the command line or job options.
In general, tags should not be edited after creation or frequently overridden,
for consistent results.
'''

# Template to add a new tag, and some description of the fields
empty_tag = {
    'name':     '',     # Name of this tag
    'package':  'map',  # Package identifier
    'release':  '',     # Athena release number
    'geoTag':   '',     # ATLAS geometry tag
    'formatted': False, # If the filenames below have {} replacement fields (see HTTTagConfig)
    'withPU': False,    # Flag to say if there is pile-up or not

    # Files
    'mappingType':  '', # ?
    'pmap':         '', # Plane map fileath
    'rmap':         '', # Region map filepath
    'subrmap':      '', # Subregion map filepath
    'modulemap':    '', # Global-to-local module id map filepath
    'slices':       '', # Region slice filepath
    'NNmap':        '', # NN weighting filepath
    'sampleType':   '', # Samples being run (skipTruth, singleMuons, singleElectrons, singlePions)

    # Layers
    'layerOverride': [], # Override the logical layers used in 1st stage

    # Region
    'region':       '', # Region index or name to process. This defaults to the first region.
    'regionNames':  [], # List of region names, index by region

    # Hit Filtering
    'doRandomRemoval':   False,        # Remove hits/clusters at random (fractions to remove set below)
    'pixelHitRmFrac':    0,            # Fraction of pixel hits to remove at random
    'pixelClusRmFrac':   0,            # Fraction of pixel clusters to remove at random
    'stripHitRmFrac':    0,            # Fraction of strip hits to remove at random
    'stripClusRmFrac':   0,            # Fraction of strip clusters to remove at random
    'doStubs':           False,        # do stub filtering
    'stubCutsFile':      '',           # read in stub cuts from this text file (either absolute path, or TrigHTTHitFiltering/file.txt if in config/)
    'barrelStubDphiCut': 0,            # endcap stub dphi cut (if no cuts file given)
    'endcapStubDphiCut': 0,            # endcap stub dphi cut (if no cuts file given)
    'useNstrips':        False,        # use number of strips, rather than dPhi, to make stub cuts
}


# Default tag should be set to latest configuration
defaultTag = 'EF_TaskForce_dev22'

# Listed with most recent first
HTTMapTags = {
        'EF_TaskForce_dev23' : {
            'name':         'EF_TaskForce_dev23',
            'package':      'map',
            'release':      '21.9.15',
            'geoTag':       'ATLAS-P2-ITK-23-00-01',
            'formatted':    True,
            'withPU':       False,
            # Files
            'mappingType':  'FILE',
            'pmap':         '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/step3_01eta03_03phi05.pmap',
            'rmap':         '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/rmaps/{regionName}_ATLAS-P2-ITK-23-00-01.rmap',
            'subrmap':      '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/zslicemaps/ATLAS-P2-ITK-23-00-01/{regionName}_KeyLayer-strip_barrel_2_extra03_trim_0_001_NSlices-6.rmap',
            'modulemap':    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/ITk.global-to-local.moduleidmap',
            'slices':       '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/slices_v01_Jan21.txt',
            'NNmap':        '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/NN_DNN_Region_0p1_0p3_HTTFake_HTTTrueMu_SingleP_8L_Nom_v6.json',
            'sampleType':   'singleMuons',

            # Layer
            'layerOverride': [],
            # Region
            'region':       '0',
            'regionNames':  [
                'eta0103phi0305', # 0
                'eta0709phi0305', # 1
                'eta1214phi0305', # 2
                'eta2022phi0305', # 3
                'eta3234phi0305', # 4
            ],
        },
        'EF_TaskForce_dev22' : {
            'name':         'EF_TaskForce_dev22',
            'package':      'map',
            'release':      '21.9.16',
            'geoTag':       'ATLAS-P2-ITK-22-02-00',
            'formatted':    True,
            'withPU':       False,
            # Files
            'mappingType':  'FILE',
            'pmap':         '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/ATLAS-P2-ITK-22-02-00.pmap',
            'rmap':         '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/rmaps/{regionName}_ATLAS-P2-ITK-22-02-00.rmap',
            'subrmap':      '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/zslicemaps/ATLAS-P2-ITK-22-02-00/{regionName}_KeyLayer-strip_barrel_2_extra03_trim_0_001_NSlices-6.rmap',
            'modulemap':    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/ITk.global-to-local.moduleidmap',
            'slices':       '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/slices_v01_Jan21.txt',            
            'NNmap':        '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/NN_DNN_Region_0p1_0p3_HTTFake_HTTTrueMu_SingleP_8L_Nom_v6.json',
            'sampleType':   'singleMuons',

            # Layer
            'layerOverride': [],
            # Region
            'region':       '0',
            'regionNames':  [
                'eta0103phi0305', # 0
                'eta0709phi0305', # 1
                'eta1214phi0305', # 2
                'eta2022phi0305', # 3
                'eta3234phi0305', # 4
            ],
            # Hit Filtering
            'doRandomRemoval':   False,
            'pixelHitRmFrac':    0,
            'pixelClusRmFrac':   0,
            'stripHitRmFrac':    0,
            'stripClusRmFrac':   0,

            'doStubs':           False,
            'stubCutsFile':      'TrigHTTMaps/stub_cuts_2pct_50evt_rings_Jun11.txt',
            'barrelStubDphiCut': 3.0,
            'endcapStubDphiCut': 1.5,
            'useNstrips':        False,
        },
        'TDRConf-v1' : {
            'name':         'TDRConf-v1',
            'package':      'map',
            'release':      '21.9.2',
            'geoTag':       'ATLAS-P2-ITK-17-06-0',
            'formatted':    False,
            'withPU':       False,
            # Files
            'mappingType':  'FILE',
            'pmap':         '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/step3_01eta03_03phi05.pmap',
            'rmap':         '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/rmaps/eta0103phi0305_ATLAS-P2-ITK-23-00-01.rmap',
            'subrmap':      '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/zslicemaps/ATLAS-P2-ITK-23-00-01/eta0103phi0305_KeyLayer-strip_barrel_2_extra03_trim_0_001_NSlices-6.rmap',
            'modulemap':    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/ITk.global-to-local.moduleidmap',
            'slices':       '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/slices_v01_Jan21.txt',
            'NNmap':        '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/NN_DNN_Region_0p1_0p3_HTTFake_HTTTrueMu_SingleP_8L_Nom_v6.json',
            'sampleType':   'singleMuons',

            # Layer
            'layerOverride': [],
            # Region
            'region':       '0',
            'regionNames':  [
                'eta0103phi0305', # 0
                'eta0709phi0305', # 1
            ],
        },
}

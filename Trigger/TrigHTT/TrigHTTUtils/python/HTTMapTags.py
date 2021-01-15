# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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

    # Files
    'mappingType':  '', # ?
    'pmap':         '', # Plane map fileath
    'rmap':         '', # Region map filepath
    'ssmap':        '', # Superstrip map filepath
    'subrmap':      '', # Subregion map filepath
    'modulemap':    '', # Global-to-local module id map filepath

    # Region
    'region':       '', # Region index or name to process. This defaults to the first region.
    'regionNames':  [], # List of region names, index by region
    'regionSlices': [], # List of slices {phi_min, phi_max, eta_min, eta_max, nbins}, index by region
    'curv':         {}, # (min, max, nbins) curvature range
    'd0':           {}, # (min, max, nbins) d0 range
    'z0':           {}, # (min, max, nbins) z0 range
}


# Default tag should be set to latest configuration
defaultTag = 'TDRConf-v1'

# Listed with most recent first
HTTMapTags = {
        'TDRConf-v1' : {
            'name':         'TDRConf-v1',
            'package':      'map',
            'release':      '21.9.2',
            'geoTag':       'ATLAS-P2-ITK-17-06-0',
            # Files
            'mappingType':  'FILE',
            'pmap':         'htt_configuration/map_files/step3_01eta03_03phi05.pmap',
            'rmap':         'htt_configuration/map_files/step3_01eta03_03phi05.rmap',
            'ssmap':        'htt_configuration/ss_files/HTT_TestBankGenV2.ss',
            'subrmap':      'htt_configuration/map_files/01eta03_03phi05_zslices.rmap',
            'modulemap':    'htt_configuration/map_files/ITk.global-to-local.moduleidmap',
            # Region
            'region':       '0',
            'regionNames':  [
                '01eta03_03phi05', # 0
                '07eta09_03phi05', # 1
            ],
            'regionSlices': [
                {'phi_min': 0.3, 'phi_max': 0.5, 'eta_min': 0.1, 'eta_max': 0.3, 'nbins': 10},   # 0
                {'phi_min': 0.3, 'phi_max': 0.5, 'eta_min': 0.7, 'eta_max': 0.9, 'nbins': 10},   # 1
            ],
            'curv': {'min': -0.0005, 'max': 0.0005, 'nbins': 250},
            'd0':   {'min': -2.0, 'max': 2.0, 'nbins': 250},
            'z0':   {'min': -150, 'max': 150, 'nbins': 100},
        },
}

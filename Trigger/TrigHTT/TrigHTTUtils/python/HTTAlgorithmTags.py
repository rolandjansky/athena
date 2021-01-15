# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

'''
@file HTTAlgorithmTags.py
@author Riley Xu - riley.xu@cern.ch
@date Mar 16th 2020

This file declares tags for configuration parameters of algorithm jobs in this package.
All options can be further overridden from the command line or job options.

In general, tags should not be edited after creation for consistent results.

See TrigHTT README for more info on tag heirarchy and usage.
'''



empty_tag = { # template to add a new tag, and some description of the fields
    # Common
    'name':     '',     # Name of this tag
    'package':  'algo', # Package identifier

    # Input
    'randomRawHits': None,

    # Pattern Matching
    'sectorsAsPatterns': None,
    'hough': None,
    'hough_1d': None,
    'max_misses': None, # maximum allowed misses for a pattern to fire

    # Track Fitter
    'chi2dofrecoverymin' : None,
    'chi2dofrecoverymax' : None,

    'doMajority': None,

    'nHits_noRecovery': None,

    'GuessHits': None,
    'DoMissingHitsChecks': None,

    'DataClass': None
}


defaultTag = 'test_15-12-20'


HTTAlgorithmTags = {
        'test_20-07-20': {
            # Common
            'name':    'test_20-07-20',
            'package': 'algo',

            # Input
            'randomRawHits': False,

            # Pattern Matching
            'sectorsAsPatterns': False,
            'hough': False,
            'hough_1d': False,
            'max_misses': 1,

            # Track Fitter
            'chi2dofrecoverymin': 40,
            'chi2dofrecoverymax': 100000000000,

            'doMajority': 1,

            'nHits_noRecovery': -1,

            'GuessHits': True,
            'DoMissingHitsChecks': False,

            'DataClass': 1,
        },
    #After 1st re-write of data classes in !137
        'test_18-11-20': {
            # Common
            'name':    'test_18-11-20',
            'package': 'algo',

            # Input
            'randomRawHits': False,

            # Pattern Matching
            'sectorsAsPatterns': False,
            'hough': False,
            'hough_1d': False,
            'max_misses': 1,

            # Track Fitter
            'chi2dofrecoverymin': 40,
            'chi2dofrecoverymax': 100000000000,

            'doMajority': 1,

            'nHits_noRecovery': -1,

            'GuessHits': True,
            'DoMissingHitsChecks': False,

            'DataClass': 2,
        },
    #After re-write of HTTHit as the only hit class in !172
        'test_15-12-20': {
            # Common
            'name':    'test_15-12-20',
            'package': 'algo',

            # Input
            'randomRawHits': False,

            # Pattern Matching
            'sectorsAsPatterns': False,
            'hough': False,
            'hough_1d': False,
            'max_misses': 1,

            # Track Fitter
            'chi2dofrecoverymin': 40,
            'chi2dofrecoverymax': 100000000000,

            'doMajority': 1,

            'nHits_noRecovery': -1,

            'GuessHits': True,
            'DoMissingHitsChecks': False,

            'DataClass': 3,
        }
}

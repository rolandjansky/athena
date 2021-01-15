# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

'''
@file parseRunArgs.py
@author Riley Xu - riley.xu@cern.ch
@date July 30, 2020
@brief Configuration file that parses run arguments from transforms

This file collects all transform run argument parsing into a centralized area
for configuring various HTT transforms.

Note that the parse functions return a dictionary that contain keys to correspond
to those in the various package tags. We don't want to use the tags directly here
to maintain good factorization. However, any changes to the tag keys must now be
copied to this file.

Usage:
    transform:
        addHTTBanksArgs(trf.parser)
    skeleton:
        opts = parseHTTArgs(runArgs)
        HTTTagConfig.getTags(opts)
'''

import os
import itertools
import PyJobTransforms.trfArgClasses as trfArgClasses

###############################################################################
## HTTMaps
###############################################################################

_HTTMapsArgs = [
    ('mapTag', trfArgClasses.argString, 'Switch to a different tag than the default listed in HTTMapTags'),
    ('region', trfArgClasses.argString, 'Region index or name'),
]


def addHTTMapsArgs(parser):
    '''
    Adds arguments used by HTTMaps to a transform parser.
    '''

    parser.defineArgGroup('TrigHTTMaps', 'Options for TrigHTTMaps. '
            'These options will override the default configuration specified in HTTMapTags.')

    def addArg(argName, argType, helpText=""):
        parser.add_argument('--' + argName,
                type=trfArgClasses.argFactory(argType, runarg=True),
                help=helpText,
                group='TrigHTTMaps')

    for arg in _HTTMapsArgs:
        addArg(*arg)

###############################################################################
## HTTBanks
###############################################################################

_HTTBanksArgs = [
    ("bankTag", trfArgClasses.argString, "Switch to a different tag than the default listed in HTTBankTags"),
    ("bankDir", trfArgClasses.argString, "Common directory path to be prepended to all filenames below. If not all files are in the same directory, you can change bankDir to a common parent directory or just '' and use fully-qualified paths for the individual filenames."),
    ("constants1stStage", trfArgClasses.argString, "Filename of the 1st stage constants"),
    ("constants2ndStage", trfArgClasses.argString, "Filename of the 2nd stage constants"),
    ("sectorBank", trfArgClasses.argString, "Filename of the sector bank"),
    ("sectorSlices", trfArgClasses.argString, "Filename of the sector slices"),
    ("patternBank", trfArgClasses.argString, "Filename of the pattern bank"),
    #("bankType",  trfArgClasses.argString, "can be FILE or COOL")
    ("maxPatts", trfArgClasses.argInt, "[DCgen] Number of patterns that can fit in the bank, or -1"),
    ("nDCMax", trfArgClasses.argInt, "[DCgen] Max number of DC bits per pattern, or -1"),
    ("nDC", trfArgClasses.argIntList, "[DCgen] Number of DC bits in each layer (vector)"),
]


def addHTTBanksArgs(parser):
    '''
    Adds arguments used by HTTBanks to a transform parser.
    '''

    parser.defineArgGroup('TrigHTTBanks', 'Optional overrides for TrigHTTBanks. '
            'These options will override the default configuration specified in HTTBankTags.')

    def addArg(argName, argType, helpText=""):
        parser.add_argument('--' + argName,
                type=trfArgClasses.argFactory(argType, runarg=True),
                help=helpText,
                group='TrigHTTBanks')

    for arg in _HTTBanksArgs:
        addArg(*arg)


###############################################################################
## HTTAlgorithms
###############################################################################

_HTTAlgorithmsArgs = [
    ("algoTag", trfArgClasses.argString, "Tag to use for algorithm configuration"),
    ("randomRawHits", trfArgClasses.argBool, "Use random raw hits from inverse fits instead of reading input file"),
    ("sectorsAsPatterns", trfArgClasses.argBool, "Use sectors as patterns for debugging"),
    ("hough", trfArgClasses.argBool, "Use Hough transform for road finding"),
    ("hough_1d", trfArgClasses.argBool, "Use Elliot's Hough transform for road finding"),
]


def addHTTAlgorithmsArgs(parser):
    '''
    Adds arguments used by HTTAlgorithms to a transform parser.
    '''

    parser.defineArgGroup('HTTAlgorithms', 'Options for HTTAlgorithms configuration')

    def addArg(argName, argType, helpText=""):
        parser.add_argument('--' + argName,
                type=trfArgClasses.argFactory(argType, runarg=True),
                help=helpText,
                group='HTTAlgorithms')

    for arg in _HTTAlgorithmsArgs:
        addArg(*arg)


###############################################################################
## Parsing
###############################################################################

def parseHTTArgs(runArgs):
    '''
    Collects HTT arguments from runArgs into a dictionary of (parameter: value) pairs.
    Use the corresponding add***Args() functions above to add arguments to a transform parser.
    '''
    opts = {}
    for arg in itertools.chain(_HTTMapsArgs, _HTTBanksArgs, _HTTAlgorithmsArgs):
        key = arg[0]
        if hasattr(runArgs, key):
            opts[key] = getattr(runArgs, key)

    return opts

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from PyJobTransforms.trfArgClasses import argFactory, argFile, argInt, argBool
from PyJobTransforms.trfArgClasses import argString, argPOOLFile


def addEI_InputTypes_tfArgs(parser):
    parser.add_argument('--inputPOOLFile', nargs='+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input POOL file', group='Event Index')
    parser.add_argument('--inputAODFile', nargs='+',
                        type=argFactory(argFile, type='AOD', io='input'),
                        help='Input AOD file', group='Event Index')
    parser.add_argument('--inputESDFile', nargs='+',
                        type=argFactory(argFile, type='ESD', io='input'),
                        help='Input ESD file', group='Event Index')
    parser.add_argument('--inputEVNTFile', nargs='+',
                        type=argFactory(argFile, type='EVNT', io='input'),
                        help='Input EVNT file', group='Event Index')
    parser.add_argument('--inputHITSFile', nargs='+',
                        type=argFactory(argFile, type='HITS', io='input'),
                        help='Input HITS file', group='Event Index')
    parser.add_argument('--inputRDOFile', nargs='+',
                        type=argFactory(argFile, type='RDO', io='input'),
                        help='Input RDO file', group='Event Index')


def addEI_OutputTypes_tfArgs(parser):
    parser.add_argument('--outputEIFile',
                        type=argFactory(argFile, io='output', type='misc'),
                        help='Output Event Index file '
                        '(default: output.ei.pkl)',
                        group='Event Index')


def addEI_Basic_tfArgs(parser):
    parser.add_argument("--eidsname",
                        type=argFactory(argString),
                        help="Overrides input file dataset name "
                        "(default: read from job environment variable INDS)",
                        group='Event Index')
    parser.add_argument("--trigger",
                        type=argFactory(argBool),
                        help="Include trigger information (default: true)",
                        group='Event Index')
    parser.add_argument("--provenance",
                        type=argFactory(argBool),
                        help="Include provenance information (default: true)",
                        group='Event Index')
    parser.add_argument("--sendtobroker",
                        type=argFactory(argBool),
                        help="Send event index to message broker "
                        "(default: false)",
                        group='Event Index')
    parser.add_argument("--testbrk",
                        type=argFactory(argBool),
                        help="Use test message broker (default: false)",
                        group='Event Index')
    parser.add_argument("--eifmt",
                        type=argFactory(argInt),
                        help="Value ignored. Always prduce SPB format. "
                        "Argument kept for compatibility",
                        group='Event Index')

    # internal options for T0 jobs
    parser.add_argument("--_taskid",
                        type=argFactory(argString),
                        help="TaskID (for T0 jobs usage)",
                        group='Event Index')
    parser.add_argument("--_jobid",
                        type=argFactory(argString),
                        help="JobID (for T0 jobs usage)",
                        group='Event Index')
    parser.add_argument("--_attempt",
                        type=argFactory(argString),
                        help="Attempt number (for T0 jobs usage)",
                        group='Event Index')


def addEI_tfArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Event Index', 'Options for event index generation')
    addEI_InputTypes_tfArgs(parser)
    addEI_OutputTypes_tfArgs(parser)
    addEI_Basic_tfArgs(parser)

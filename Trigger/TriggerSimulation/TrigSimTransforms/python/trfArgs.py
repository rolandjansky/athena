# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



import PyJobTransforms.trfArgClasses as trfArgClasses
from PyJobTransforms.trfArgClasses import argFactory

## @brief Add trigger simulation related arguments
def addTriggerArguments(parser, addTrigFilter=True):
    parser.defineArgGroup('Trigger', 'Trigger Related Options')

    # input and output
    parser.add_argument('--outputRDO_TRIGFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output'),
                        help='Output RDO file after trigger and merging',
                        group='Trigger')

    # configuration
    parser.add_argument('--triggerConfig',
                        type=argFactory(trfArgClasses.argument), 
                        metavar='substep:triggerConf',
                        help='Trigger configuration string', 
                        group='Trigger')


    if addTrigFilter:

        parser.add_argument('--trigFilterList',
                            type=argFactory(trfArgClasses.argList), nargs="+",
                            help='Trigger filter list (multiple values can be given separately or split on commas; only understood in RAWtoESD)', 
                            group='Trigger')


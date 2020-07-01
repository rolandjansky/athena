#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## GetTfCommand.py - prints the job transform command accociated with an AMI tag.
# $Id$

from __future__ import print_function
import sys
import argparse

from PyJobTransforms.trfLogger import msg, stdLogLevels
if '--printOnlyCmdLine' not in sys.argv:
    msg.info('logging set in %s', sys.argv[0])

from PyJobTransforms.trfAMI import TagInfo
from PyJobTransforms.trfExceptions import TransformAMIException

def main():
    parser = argparse.ArgumentParser(argument_default = argparse.SUPPRESS, description = 'GetTfCommand.py - prints the job transform commands accociated with an AMI tag.')
    parser.add_argument('--AMI', '--AMIConfig', help = 'Production tag to be interpreted', required = True)
    parser.add_argument('--verbose', '--debug', action = 'store_true', help = 'set logging level to DEBUG')
    parser.add_argument('--doNotSuppressNonJobOptions', action = 'store_true', help = 'get full output from AMI')
    parser.add_argument('--printOnlyCmdLine', action = 'store_true', help = 'simply put out the TRF command line, nothing else')

    args = vars(parser.parse_args(sys.argv[1:]))

    if 'verbose' in args:
        msg.setLevel(stdLogLevels['DEBUG'])

    suppressNonJobOptions = True
    if 'doNotSuppressNonJobOptions' in args:
        suppressNonJobOptions = False

    try:
        tag = TagInfo(args['AMI'], suppressNonJobOptions)
    except TransformAMIException as e:
        print('An AMI exception was raised when trying to resolve the tag {0}.'.format(args['AMI']))
        print('Exception message was: {0}'.format(e.errMsg))
        print('Note that you need both suitable credentials to access AMI and access to the panda database (only works from inside CERN) for GetTfCommand.py to work.')
        sys.exit(1)

    if 'printOnlyCmdLine' not in args:
        print(tag)

        if 'argdict' in args:
            tag.dump(args['argdict'])
    else:
        # only print the command line, allows stuff like 
        # pathena --trf "`GetTfCommand --AMI q1234 --printOnlyCmdLine` --inputFile bla.input --maxEvents 42"
        trfCmdLine = tag.trfs[0].name + " " + tag.trfs[0]._argsToString(tag.trfs[0].physics)
        print(trfCmdLine.replace('"', '\\' + '"'))


if __name__ == '__main__':
        main()


#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## GetTfCommand.py - prints the job transform command accociated with an AMI tag.
# $Id$

import sys
import argparse

from PyJobTransforms.trfLogger import msg, stdLogLevels
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.trfAMI import TagInfo
from PyJobTransforms.trfExceptions import TransformAMIException

def main():

    parser = argparse.ArgumentParser(argument_default=argparse.SUPPRESS, description = 'GetTfCommand.py - prints the job transform commands accociated with an AMI tag.')
    parser.add_argument('--AMI', '--AMIConfig', help='Production tag to be interpreted', required=True)
    parser.add_argument('--verbose', '--debug', action='store_true', help='set logging level to DEBUG')
    
    args=vars(parser.parse_args(sys.argv[1:]))

    if 'verbose' in args:
        msg.setLevel(stdLogLevels['DEBUG'])

    try:
        tag=TagInfo(args['AMI'])
    except TransformAMIException, e:
        print 'An AMI exception was raised when trying to resolve the tag {0}.'.format(args['AMI'])
        print 'Exception message was: {0}'.format(e.errMsg)
        print 'Note that you need both suitable credentials to access AMI and access to the panda database (only works from inside CERN) for GetTfCommand.py to work.'
        sys.exit(1)

    print tag

    if 'argdict' in args:
        tag.dump(args['argdict'])
    

if __name__ == '__main__':

        main()


    


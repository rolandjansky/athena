#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# $Id: makeTrfSignatures.py 630078 2014-11-21 11:20:16Z graemes $
#

__doc__ = 'Dump job transform arguments into a file as a pickle'

import argparse
import glob
import os
import os.path
import json
import sys

import logging
logging.basicConfig(format='%(filename)s:%(levelname)s:%(message)s', level=logging.INFO)

def main():
    parser = argparse.ArgumentParser(description = __doc__, )
    parser.add_argument('--output', help='JSON output file', required = True)
    parser.add_argument('--transforms', help='Comma separated list of transforms to process', default='all')
    cliargs = vars(parser.parse_args())

    # Make sure we can import from where the trfs actually live
    # (N.B. This script is invoked from the cmt directory at install time)
    sys.path.insert(1, os.path.join(os.getcwd(), '../scripts'))
    
    myTrfSigs = {}
    myTrfSigDesc = {}

    if cliargs['transforms'] == 'all':
        # List comprehension strips off the path and removes the .py suffix
        # Look for all _tf.py (new style names)
        myTrfs = [ os.path.basename(t)[:-3] for t in glob.glob('../scripts/*_tf.py') ]
    else:
        myTrfs = cliargs['transforms'].split(',')
    logging.info('Will process this list of transforms: {0}'.format(' '.join(myTrfs)))
    processedTrfs = []
    
    for trf in myTrfs:
        logging.info('Processing argument signatures for {0}'.format(trf))
        # Use __import__ to allow us to import from the trf list
        try:
            trfModule = __import__('{0}'.format(trf), globals(), locals(), ['getTransform'], 0)
        except ImportError:
            logging.warning('Failed to import transform {0} - ignored'.format(trf))
            continue
        if 'getTransform' not in dir(trfModule):
            logging.warning('Transform {0} has no getTransform() functionality - ignored for pickle'.format(trf))
            continue
        transform = trfModule.getTransform()
        args = transform.parser.allArgs

        logging.debug('Trf %s: %s', trf, args)
        processedTrfs.append(trf)
        myTrfSigs[trf] = args
        myTrfSigDesc[trf] = transform.parser.getProdsysDesc
    try:
        logging.info('Writing JSON signatures to {0}'.format(cliargs['output']))
        sigFile = open(cliargs['output'], 'wb')
        json.dump(myTrfSigDesc, sigFile, indent=4)
    except (OSError, IOError) as e:
        logging.error('Failed to dump pickled signatures to %s: %s', cliargs['output'], e)
        sys.exit(1)
        
    logging.info('Successfully generated signature file "%s" for transforms %s', cliargs['output'], processedTrfs)
    sys.exit(0)

if __name__ == '__main__':
    main()

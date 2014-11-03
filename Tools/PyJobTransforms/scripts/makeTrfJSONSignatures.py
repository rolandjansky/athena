#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# $Id: makeTrfJSONSignatures.py 619492 2014-10-01 20:44:50Z graemes $
#

## @brief Dump job transform arguments into a file, JSON encoded

import argparse
import os
import os.path
import json
import sys

import logging
logging.basicConfig(format='%(filename)s:%(levelname)s:%(message)s', level=logging.INFO)

def _getTransformsFromPATH():
    transforms_list = []
    transforms_path_list = []
    path_elements = os.environ['PATH'].split(":")
    for path_element in path_elements:
        logging.info("Searching {0} for transforms".format(path_element))
        try:
            transforms = [ entry for entry in os.listdir(path_element) if entry.endswith("_tf.py") ]
            for trf in transforms:
                if trf not in transforms_list:
                    if trf in ('Athena_tf.py', 'Cat_tf.py', 'Echo_tf.py', 'ExeWrap_tf.py', 'Sleep_tf.py'):
                        # Test transform - not for production
                        continue
                    transforms_list.append(trf)
                    transforms_path_list.append(os.path.join(path_element, trf))
                    logging.info("...added {0} to list of transforms".format(trf))
        except OSError:
            pass
    return transforms_path_list

def main():
    parser = argparse.ArgumentParser(description = "Generate signature files for transforms, dumped in JSON format." )
    parser.add_argument('--output', help='JSON output file', required = True)
    parser.add_argument('--transforms', help='List of transforms to process'
                        ' (any path given is added to PYTHONPATH automatically).'
                        ' If not specified then all executable *_tf.py files'
                        ' found in PATH are added.', 
                        nargs='+', default=[])
    cliargs = vars(parser.parse_args())
    
    myTrfSigs = {}
    myTrfSigDesc = {}
    myTrfs = []

    if cliargs['transforms'] == []:
        cliargs['transforms'] = _getTransformsFromPATH()

    for fulltrf in cliargs['transforms']:
        myTrfs.append(os.path.basename(fulltrf))
        # Add the PATH to the PYTHONPATH if it's not there already
        trfpath = os.path.dirname(fulltrf)
        if len(trfpath) > 1:
            if trfpath not in sys.path:
                sys.path.insert(1, trfpath)

    logging.info('Will process this list of transforms: {0}'.format(' '.join(myTrfs)))
    processedTrfs = []
    
    for trf in myTrfs:
        if trf.endswith(".py"):
            trf = trf[:-3]
        logging.info('Processing argument signatures for {0}'.format(trf))
        # Use __import__ to allow us to import from the trf list
        try:
            trfModule = __import__('{0}'.format(trf), globals(), locals(), ['getTransform'], -1)
        except ImportError:
            logging.warning('Failed to import transform {0} - ignored'.format(trf))
            continue
        if 'getTransform' not in dir(trfModule):
            logging.warning('Transform {0} has no getTransform() functionality - ignored'.format(trf))
            continue
        transform = trfModule.getTransform()
        args = transform.parser.allArgs

        logging.debug('Trf %s: %s' % (trf, args))
        processedTrfs.append(trf)
        myTrfSigs[trf] = args
        myTrfSigDesc[trf] = transform.parser.getProdsysDesc()
    try:
        logging.info('Writing JSON signatures to {0}'.format(cliargs['output']))
        sigFile = open(cliargs['output'], 'wb')
        json.dump(myTrfSigDesc, sigFile, indent=4)
    except (OSError, IOError) as e:
        logging.error('Failed to dump signatures to %s: %s' % (cliargs['output'], e))
        sys.exit(1)
        
    logging.info('Successfully generated signature file "%s" for transforms %s' % (cliargs['output'], processedTrfs))
    sys.exit(0)

if __name__ == '__main__':
    main()

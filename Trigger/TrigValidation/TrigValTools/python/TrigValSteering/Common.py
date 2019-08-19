#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
Common variables and functions used in Trigger ART test steering
'''

import logging
import sys
import os


# Logging level used across the package
trigvalsteering_logging_level = logging.INFO

# Dictionary of required prefixes identifying a package, see ATR-19735
package_prefix_dict = {'TriggerTest':      'trig_',
                       'TrigP1Test':       'trigP1_',
                       'TrigAnalysisTest': 'trigAna_',
                       'TrigUpgradeTest':  'trigUpgr_'}

# Log file with all art-result statements
art_result_summary = 'art-result-summary.log'

# ART input directories for data and references
art_input_eos = '/eos/atlas/atlascerngroupdisk/data-art/grid-input/'
art_input_cvmfs = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/'


def get_logger():
    '''Default TrigValSteering logger'''
    logging.basicConfig(stream=sys.stdout,
                        format='%(asctime)s %(name)s %(levelname)-8s %(message)s',
                        datefmt='%Y-%m-%dT%H%M %Z',
                        level=trigvalsteering_logging_level)
    return logging.getLogger('TrigValSteering')


def art_result(result, name):
    '''Print art-result to stdout and summary log file'''

    line = 'art-result: {} {}\n'.format(result, name)
    sys.stdout.write(line)
    with open(art_result_summary, 'a') as summary:
        summary.write(line)


def clear_art_summary():
    '''Remove art-result summary file produced by the art_result function'''

    if os.path.isfile(art_result_summary):
        os.remove(art_result_summary)

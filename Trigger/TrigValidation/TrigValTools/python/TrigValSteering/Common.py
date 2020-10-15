#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
Common variables and functions used in Trigger ART test steering
'''

import logging
import sys
import os
from PyUtils.Decorators import memoize
from AthenaCommon.Utils.unixtools import FindFile


# Logging level used across the package
trigvalsteering_logging_level = logging.INFO

# Dictionary of required prefixes identifying a package, see ATR-19735
package_prefix_dict = {'TriggerTest':         'trig_',
                       'TrigP1Test':          'trigP1_',
                       'TrigAnalysisTest':    'trigAna_',
                       'TrigInDetValidation': 'trigID_'}

# Log file with all art-result statements
art_result_summary = 'art-result-summary.log'

# ART input directories for data and references
art_input_eos = '/eos/atlas/atlascerngroupdisk/data-art/grid-input/'
art_input_cvmfs = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/'


def get_logger():
    '''Default TrigValSteering logger'''
    logging.basicConfig(stream=sys.stdout,
                        format='%(asctime)s %(name)s %(levelname)-8s %(message)s',
                        datefmt='%Y-%m-%dT%H%M%S %Z',
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


def find_file(pattern):
    '''
    Bash inline command frequently used in multi-step tests
    to pass the output of one step to the input of another
    based on a name pattern rather than a fixed full file name
    '''
    return '`find . -name \'{:s}\' | tail -n 1`'.format(pattern)


@memoize
def check_job_options(jo_path):
    '''
    Check if the job options file exists locally or in JOBOPTSEARCHPATH.
    Returns True if the file is found and False otherwise.
    '''
    # Check if job options exists locally
    if os.path.isfile(jo_path):
        return True
    # Try to find the file in JOBOPTSEARCHPATH
    found = FindFile(jo_path, os.environ['JOBOPTSEARCHPATH'].split(os.pathsep), os.R_OK)
    return found is not None


@memoize
def running_in_CI():
    return os.environ.get('gitlabTargetBranch') is not None

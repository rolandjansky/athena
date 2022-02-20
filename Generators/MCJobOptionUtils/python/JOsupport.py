# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Pythonised helper functions for usage in top-level JobOptions
#    written by Zach Marshall <zach.marshall@cern.ch>
#    updates by Christian Gutschow <chris.g@cern.ch>

import os
from AthenaCommon import Logging
mglog = Logging.logging.getLogger('MCJobOptionUtils')


def get_physics_short():
    FIRST_DIR = (os.environ['JOBOPTSEARCHPATH']).split(":")[0]
    jofiles = [f for f in os.listdir(FIRST_DIR) if (f.startswith('mc') and f.endswith('.py'))]
    if len(jofiles)==0:
        raise RuntimeError('No job options found in '+FIRST_DIR)
    joparts = os.path.basename(jofiles[0]).split('.')
    if len(joparts)<2:
        raise RuntimeError('Malformed job options file name: '+jofiles[0])
    return joparts[1]


def check_reset_proc_number(opts):
    if 'ATHENA_PROC_NUMBER' in os.environ and int(os.environ['ATHENA_PROC_NUMBER'])>0:
        mglog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
        # Try to modify the opts underfoot
        if not hasattr(opts,'nprocs'):
            mglog.warning('Did not see option!')
        else:
            opts.nprocs = 0
        mglog.debug(str(opts))


def ls_dir(directory):
    mglog.info('For your information, ls of '+directory+':')
    mglog.info( sorted( os.listdir( directory ) ) )



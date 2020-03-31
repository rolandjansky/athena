# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Helper functions for class name manipulations

# For some of the finding
import re

# For logging errors and warnings
from AthenaCommon import Logging
cnm_log = Logging.logging.getLogger('ClassNameManips')

def pers_to_trans_name(p):
    # Baseline: transient name is identical
    t = p
    # Let's look for a version in the name
    vers = re.findall( r'_v\d+',t )
    if len(vers)>1:
        cnm_log.warning('Input name '+p+' seems to have multiple version numbers: '+str(vers)+' - will wipe them all')
    for v in vers:
        # Wipe out the version
        t = t.replace(v,'')
    # Now convert data vectors
    if 'DataVector<' in t:
        t = t.replace('DataVector<','').replace('>','')
        t = 'Container#'.join(t.split('#'))
    cnm_log.debug('Translated '+p+' to '+t)
    return t

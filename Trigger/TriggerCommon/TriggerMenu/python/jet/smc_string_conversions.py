# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""convert standard string such as nosmc, 0smc30, -INFsmc30,
30smcINF, to smc_min, smc_max strings"""

import re
smc_range_re = re.compile(r'^(?P<smc_min>((-)?\d+|-?INF))smc(?P<smc_max>((-)?\d+|-?INF))$')

def _extract(smc_range):
    
    if smc_range == 'nosmc':
        return '-INF', 'INF'
    
    match = smc_range_re.search(smc_range)
    if not match:
        msg = '%s.process_part() unknown smc range: %s does not match %s'
        
        msg = msg % ('smc_string_conversions._extract()',
                     str(smc_range),
                     smc_range_re.pattern)
        raise RuntimeError(msg)

    return (match.group('smc_min'), match.group('smc_max'))


def smc_string_to_strings(smc_range):
    return  _extract(smc_range)

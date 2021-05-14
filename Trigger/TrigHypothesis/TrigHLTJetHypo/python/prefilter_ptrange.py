# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.FastReductionAlgToolFactory import toolfactory

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

import re

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

pattern = r'^PTRANGE(?P<begin>\d+)(r)(?P<end>\d+)$'

rgx = re.compile(pattern)


def prefilter_ptrange(pf_string):
    """calculate the parameters needed to generate a RangeFilter config 
    AlgTool starting from the prefilter substring if it appears in the 
    chain dict"""

    assert pf_string.startswith('PTRANGE'),\
        'routing error, module %s: bad prefilter %s' % (__name__, pf_string)

    m = rgx.match(pf_string)
    groupdict = m.groupdict()

    vals = {}
    vals['begin'] = int(groupdict['begin']) - 1  # chainname indices atart at 1
    vals['end'] = int(groupdict['end'])  # so "end" is 1 + last pos in rangw

    toolclass, name =  toolfactory('RangeFilterConfigTool')
    vals['name'] = name
    
    return toolclass(**vals)


# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.RepeatedConditionParams import RepeatedConditionParams
from TrigHLTJetHypo.ConditionDefaults import defaults

from TrigHLTJetHypo.make_repeatedCondConfigurer import (
    make_repeatedCondCfgFromParams,
)

from TrigHLTJetHypo.FastReductionAlgToolFactory import toolfactory

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

import re
from copy import deepcopy

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

pattern = r'^MASK'\
    r'(?P<etalo>\d*)(?P<etatype>neta|ceta|peta>)(?P<etahi>\d*)XX'\
    r'(?P<philo>\d*)(?P<phitype>nphi|cphi|pphi>)(?P<phihi>\d*)$'

rgx = re.compile(pattern)


def prefilter_mask(pf_string):
    """calculate the parameters needed to generate a ConditonFilter config 
    AlgTool starting from the prefilter substring if it appears in the 
    chain dict"""

    assert pf_string.startswith('MASK'),\
        'routing error, module %s: bad prefilter %s' % (__name__, pf_string)

    m = rgx.match(pf_string)
    groupdict = m.groupdict()

    etatype = groupdict['etatype']
    etalo = groupdict['etalo']
    etahi = groupdict['etahi']
    if etatype in ('neta', 'ceta'):
        etalo = '-'+ etalo
    if etatype == 'neta':
        etahi = '-' + etahi

       
    phitype = groupdict['phitype']
    philo = groupdict['philo']
    phihi = groupdict['phihi']
    if phitype in ('nphi', 'cphi'):
        philo = '-'+ philo
    if phitype == 'nphi':
        phihi = '-' + phihi

    condargs = []
    
    vals = defaults(etatype, lo=etalo, hi=etahi)
    condargs.append((etatype, deepcopy(vals)))
      
    vals = defaults(phitype, lo=philo, hi=phihi)
    condargs.append((phitype, deepcopy(vals)))

    repcondarg = RepeatedConditionParams(tree_id=0,
                                         tree_pid=0,
                                         chainPartInd=-1,
                                         condargs=condargs,
                                         invert=True)
    
    repConditionMaker = make_repeatedCondCfgFromParams(repcondarg)

    
    toolclass, name =  toolfactory('ConditionFilterConfigTool')
    vals = {'name' : name,
            'conditionMakers': [repConditionMaker]}
    
    return toolclass(**vals)


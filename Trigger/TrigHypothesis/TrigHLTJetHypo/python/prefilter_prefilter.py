# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.ConditionDefaults import defaults

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

import re
from copy import deepcopy

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)


pattern = r'^prefilterSEP'\
    r'(?P<etalo>\d*)(?P<etatype>neta|ceta|peta>)(?P<etahi>\d*)SEP'\
    r'(?P<philo>\d*)(?P<phitype>nphi|cphi|pphi>)(?P<phihi>\d*)$'

rgx = re.compile(pattern)

def get_condargs(groupdict):
    """get the elemental condition args (cuts) for the backward and forward 
    jets"""

    condargs = []
    
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

        
    vals = defaults(etatype, lo=etalo, hi=etahi)
    condargs.append((etatype, deepcopy(vals)))
      
    vals = defaults(phitype, lo=philo, hi=phihi)
    condargs.append((phitype, deepcopy(vals)))

    return condargs


def prefilter_prefilter(pf_string):
    """produce a list of obkects that carry the paramrters needed to 
    instantiate configurers for a elemental Conditions that describe
    an eta-phi region. The input is a prefilter string obtained from the 
    chain_dict."""

    assert pf_string.startswith('prefilter'),\
        'routing error, module %s: bad prefilter string %s' % (__name__,
                                                               pf_string)
    
    m = rgx.match(pf_string)
    groupdict = m.groupdict()

    return  get_condargs(groupdict)

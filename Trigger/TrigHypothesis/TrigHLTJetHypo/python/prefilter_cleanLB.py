# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# from TrigHLTJetHypo.RepeatedConditionParams import RepeatedConditionParams
from TrigHLTJetHypo.ConditionDefaults import defaults

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

import re
from copy import deepcopy

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

pattern = r'^PREFILTER'\
    r'(?P<etalo>\d*)(?P<etatype>neta|ceta|peta>)(?P<etahi>\d*)XX'\
    r'(?P<philo>\d*)(?P<phitype>nphi|cphi|pphi>)(?P<phihi>\d*)$'

rgx = re.compile(pattern)


def prefilter_cleanLB(pf_string):
    """calculate the parameters needed to generate a ConditonFilter config 
    AlgTool starting from the prefilter substring if it appears in the 
    chain dict"""
    vals = deepcopy(defaults('et'))  # keep code checker happy for skeleton code
    logger.debug(vals)
    raise NotImplementedError

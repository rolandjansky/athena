# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.RepeatedConditionParams import RepeatedConditionParams

from TrigHLTJetHypo.make_repeatedCondConfigurer import (
    make_repeatedCondCfgFromParams,
)

from TrigHLTJetHypo.FastReductionAlgToolFactory import toolfactory

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG
from TriggerMenuMT.HLT.Jet.JetRecoCommon import getPrefilterCleaningString

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

def prefilter_clean(pf_string):
    """calculate the parameters needed to generate a ConditonFilter config 
    AlgTool starting from the prefilter substring if it appears in the 
    chain dict"""

    assert pf_string.startswith('CLEAN'),\
        'routing error, module %s: bad prefilter %s' % (__name__, pf_string)

    condargs = [('clean',{'val':getPrefilterCleaningString([pf_string])})]
    
    repcondarg = RepeatedConditionParams(tree_id=0,
                                         tree_pid=0,
                                         chainPartInd=-1,
                                         condargs=condargs,
                                         invert=False)
    
    repConditionMaker = make_repeatedCondCfgFromParams(repcondarg)

    
    toolclass, name =  toolfactory('ConditionFilterConfigTool')
    vals = {'name' : name,
            'conditionMakers': [repConditionMaker]}
    
    return toolclass(**vals)

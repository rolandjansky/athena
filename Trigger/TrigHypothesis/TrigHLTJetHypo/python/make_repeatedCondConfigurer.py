# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.FastReductionAlgToolFactory import toolfactory

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

def make_repeatedCond(tree_id, tree_pid, clique,
                      multiplicity=1,
                      chainPartInd=-1,
                      conditionMakers=[],
                      invert=False):
    
    """makes a RepeatedConditionConfigurer from explicit arguments)"""
    
    toolclass, name =  toolfactory('RepeatedConditionConfigTool')
    repeated_args = {'name': name}
    repeated_args['conditionMakers'] = sorted(conditionMakers, key=lambda cm: cm.name() if callable(cm.name) else cm.name)
    repeated_args['id'] =  tree_id
    repeated_args['pid'] =  tree_pid
    repeated_args['clique'] =  clique
    repeated_args['multiplicity'] = multiplicity
    repeated_args['chainPartInd'] = chainPartInd    
    repeated_args['invert'] = invert    

    return toolclass(**repeated_args)

def make_repeatedObj(repArgs, conditionMakers=[]):
    """makes a RepeatedConditionConfigurer from an objects holding
    the necessary parameters, usually built by a scenanario_XX module."""

    return make_repeatedCond(tree_id=repArgs.tree_id,
                             tree_pid=repArgs.tree_pid,
                             clique=repArgs.clique,
                             multiplicity=repArgs.multiplicity,
                             chainPartInd=repArgs.chainPartInd,
                             invert = repArgs.invert,
                             conditionMakers=conditionMakers,)
                      

def make_repeatedCondCfgFromParams(repcondarg):
    """paramsrepcondarg  container the parameters for building the internal
    condition configurers, and the containing RepeatedCondition configurer"""

    condobjs = []
    for key, vals in repcondarg.condargs:
        toolclass, name = toolfactory(key)
        vals['name'] = name
        condobjs.append(toolclass(**vals))
            
    return make_repeatedObj(repcondarg, condobjs)



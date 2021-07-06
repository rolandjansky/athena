# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" create a ConditionFilter configurer"""

from TrigHLTJetHypo.FastReductionAlgToolFactory import toolfactory

from TrigHLTJetHypo.make_repeatedCondConfigurer import (
    make_repeatedCondCfgFromParams,
)


def makeConditionFilterConfigurer(filterparams):

    assert filterparams.typename == 'ConditionFilter'

    # one Repeated Condition 
    assert len(filterparams.args) == 1

    repcond_arg = filterparams.args[0]

    # check that there are contained conditions
    assert repcond_arg.condargs, \
        'no elemental conditions, use PassThroughFilter?'
    
    repeatedConditionConfigurer = make_repeatedCondCfgFromParams(
        repcond_arg)
    
    toolclass, name =  toolfactory('ConditionFilterConfigTool')
    vals = {'name' : name,
            'conditionMakers': [repeatedConditionConfigurer]}

    
    # return a ConditionFilter configuration AlgTool that has one
    # RepeatedCondition and zero or more internal conditions

    return toolclass(**vals)

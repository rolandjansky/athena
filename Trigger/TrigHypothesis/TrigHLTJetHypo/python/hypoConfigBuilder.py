# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.FastReductionAlgToolFactory import toolfactory

# import modules concerned with extracting scenario paramters
# from a scenario string
from TrigHLTJetHypo.scenario_agg import scenario_agg
from TrigHLTJetHypo.scenario_dijet import scenario_dijet
from TrigHLTJetHypo.scenario_fbdjnoshared import scenario_fbdjnoshared
from TrigHLTJetHypo.scenario_fbdjshared import scenario_fbdjshared
from TrigHLTJetHypo.scenario_simple import scenario_simple

from TrigHLTJetHypo.prefilter_mask import prefilter_mask
from TrigHLTJetHypo.prefilter_cleanLB import prefilter_cleanLB

from TrigHLTJetHypo.makeConditionFilterConfigurer import (
    makeConditionFilterConfigurer,
)

from TrigHLTJetHypo.makePassThroughFilterConfigurer import (
    makePassThroughFilterConfigurer,
)

from TrigHLTJetHypo.make_repeatedCondConfigurer import (
    make_repeatedCond,
    make_repeatedCondCfgFromParams,
)

from DecisionHandling.TrigCompositeUtils import isLegId, getLegIndexInt

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)


def make_root_repcondconfig():
    """make a repeated condition configurer for the fast reduction
    root node. This will will have a single accapt all internal node."""
    
    toolclass, name =  toolfactory('all')
    args = {'name':  name}
    conditionMakers = [toolclass(**args)]

    configurer = make_repeatedCond(tree_id=0,
                                   tree_pid=0,
                                   conditionMakers=conditionMakers)
    
    return configurer

def buildHypoHelperConfigTool(params):
    """the parameter object passed in is build in the modules handling
    scenarios. It contains the information needed to build the
    configuration AlgTool that intialiases a hypo helper AlgTool"""

    # check that each Condition has a Filter
    assert len(params.repcondargs) == len(params.filterparams)
    
    # FastReducer root node
    repcondobjs = [make_root_repcondconfig()]
    for ra in params.repcondargs:
        repcondobjs.append(make_repeatedCondCfgFromParams(ra))


    # filter for FastReducer root node
    filtConditionMakers = [makePassThroughFilterConfigurer()]

    for ra in params.filterparams:
        if ra.typename == 'ConditionFilter':
            filtConditionMakers.append(makeConditionFilterConfigurer(ra))
        elif ra.typename == 'PassThroughFilter':
            filtConditionMakers.append(makePassThroughFilterConfigurer())
        else:
            raise NotImplementedError(
                'Filter type %s not implemented' % ra.typename
            )

    toolclass, name = toolfactory('HelperToolConfigTool')
                           
    vals = {'name': name,
            'conditionMakers': repcondobjs,
            'filterMakers': filtConditionMakers,
            'treeVector': params.treevec,
            'leafVector': params.leafvec,
            }
                           
    return toolclass(**vals)


def process_simple(chain_parts):
    """Obtain the paramters needed to build an AlgTool
    to initialise a jet hypo HelperAlgTool"""

    # obtain a list of parameter objects that will be used
    # to build a helper config AlgTools
    helper_params = scenario_simple(chain_parts)

    # build the helper config AlgTools
    helperconfigobjs = [buildHypoHelperConfigTool(params) for params in
                        helper_params]
    
    return helperconfigobjs



def process_agg(scenario, chainPartInd):
    """Obtain the paramters needed to build an AlgTool
    to initialise a jet hypo HelperAlgTool"""

    # obtain a list of parameter objects that will be used
    # to build a helper config AlgTools
    helper_params = scenario_agg(scenario, chainPartInd)

    # build the helper config AlgTools
    helperconfigobjs = [buildHypoHelperConfigTool(params) for params in
                        helper_params]
                           
    return helperconfigobjs


def process_dijet(scenario, chainPartInd):
    """Obtain the paramters needed to build an AlgTool
    to initialise a jet hypo HelperAlgTool"""

    # obtain a list of parameter objects that will be used
    # to build a helper config AlgTools
    helper_params = scenario_dijet(scenario, chainPartInd)

    # build the helper config AlgTools
    helperconfigobjs = [buildHypoHelperConfigTool(params) for params in
                        helper_params]
                           
    return helperconfigobjs


def process_fbdjshared(scenario, chainPartInd):
    """Obtain the paramters needed to build an AlgTool
    to initialise a jet hypo HelperAlgTool"""

    # obtain a list of parameter objects that will be used
    # to build a helper config AlgTools
    helper_params = scenario_fbdjshared(scenario, chainPartInd)

    # build the helper config AlgTools
    helperconfigobjs = [buildHypoHelperConfigTool(params) for params in
                        helper_params]
                           
    return helperconfigobjs


def process_fbdjnoshared(scenario, chainPartInd):
    """Obtain the paramters needed to build an AlgTool
    to initialise a jet hypo HelperAlgTool"""

    # obtain a list of parameter objects that will be used
    # to build a helper config AlgTools
    helper_params = scenario_fbdjnoshared(scenario, chainPartInd)

    # build the helper config AlgTools
    helperconfigobjs = [buildHypoHelperConfigTool(params) for params in
                        helper_params]
                           
    return helperconfigobjs


def process_nonsimple(scenario, chainPartInd):
    """Maker a list of helper tool config tool for a non-simple scenario.
    Note:  a non-simple scenario will  produce more than HelperToolConfigTool
    if jet sharing among Conditions is required."""
    
    router = {
        'agg': process_agg,
        'dijet': process_dijet,
        'fbdjshared': process_fbdjshared,
        'fbdjnoshared': process_fbdjnoshared,
    }

    key = scenario.split('SEP')[0]

    return router[key](scenario, chainPartInd)  # list of HelperToolConfigTool


def make_fastreduction_configurers(chain_dict):
    """Create HelperToolConfigTool  instances. Each instance
    configures a FastReduction tree. Chain parts with the 'simple' scenario
    use used to form a single HelperToolConfigTool. The information 
    may be spread over a number of chain parts. 

    There is at most one chain part with a non-simple scenario. 
    This may give rise to > 1 HelperToolConfigTool instance - as this
    is how jet sharing among Conditions is handled.

    If there are both simple and non-simple scenarios, there will be
    n HelperToolConfigTool instances, where n >=2: one for the simple 
    scenario chain parts, and n-1 for the non-simple scenario.
    """
    

    chain_parts = chain_dict['chainParts']
    simple_chainparts = [
        cp for cp in chain_parts if cp['hypoScenario'] == 'simple']

    helperToolConfigTools = []

    if simple_chainparts:
        helperToolConfigTools.extend(process_simple(simple_chainparts))

    scenario_chainparts =[
        cp for cp in chain_parts if cp['hypoScenario'] != 'simple']

    if scenario_chainparts:
        assert len(scenario_chainparts) == 1
        scenario_chainpart = scenario_chainparts[0]

        # We only allow threshold != 0 for the simple scenario.
        assert scenario_chainpart['threshold'] == '0'
        
        scenario = scenario_chainparts[0]['hypoScenario']
        # find the chain part index for a non-simple scenario.
        # assume simple is processed before non-simple, and that
        # there is at most one non-simple chainpart.
        # chainPartInd is needed to report passing jets to the
        # trigger framework.
        chainPartInd = scenario_chainpart['chainPartIndex']

        helperToolConfigTools.extend(process_nonsimple(scenario,
                                                       chainPartInd))
        
    return helperToolConfigTools


def make_prefilter_configurers(chain_dict):
    """Set up the prefilters fo the chain."""
    
    pf_strings = []
    chain_parts = [cp for cp in chain_dict['chainParts'] if
                   cp['signature'] == 'Jet' and 'prefilters' in cp]

    [pf_strings.extend(cp['prefilters']) for cp in chain_parts]
    
    # TEMPORARY - 'cleanLB' as a prefilter string also affects reconstruction,
    # and already appears in chain names for this purpose.
    # Until the cleanLB hypo prefilter code is implemented, remove the string
    # from the prefilter strings.
    try:
        pf_strings.remove('cleanLB')
    except ValueError:
        pass
    

 
    # if not pre filter strings (pf_strings) are found in the chainDict,
    # a PassThroughFilter configurer is made.

    if not pf_strings:
        return [makePassThroughFilterConfigurer()]

    # route the prefilter strings to rhe approriate handler
    prefilter_router = {
        'mask': prefilter_mask,
        'cleanLB': prefilter_cleanLB,
    }

    filters = []
    for pf_string in pf_strings:
        key = pf_string.split('SEP')[0]

        try:
            filters.append(prefilter_router[key](pf_string))
        except KeyError:
            raise RuntimeError('Unknown prefilter string: %s' % pf_string)
        
    return filters


def getLabelIndices(chain_dict):

    start_index = 0
    if isLegId(chain_dict['chainName']):
        start_index = getLegIndexInt(chain_dict['chainName'])

    end_index = start_index + len(chain_dict['chainParts'])

    return start_index, end_index


def  hypotool_from_chaindict(chain_dict, visit_debug=False):


    helperToolConfigTools =  make_fastreduction_configurers(chain_dict)

    prefilterMakers = make_prefilter_configurers(chain_dict)

    toolclass, name = toolfactory('helper_tool')
    args = {'name': name,
            'HypoConfigurers': helperToolConfigTools,
            'prefilterMakers': prefilterMakers
            }
    
    helper_tool = toolclass(**args)

    toolclass, name =  toolfactory('hypo_tool')
    
    startLabelIndex, endLabelIndex = getLabelIndices(chain_dict)
    args = {'name': chain_dict['chainName'],
            # for reporting passing jets:
            'visit_debug': visit_debug,
            'helper_tool': helper_tool,
            'chain_name': chain_dict['chainName'],
            'startLabelIndex': startLabelIndex,
            'endLabelIndex': endLabelIndex,
            }
    
    hypo_tool = toolclass(**args)

    return hypo_tool

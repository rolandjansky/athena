# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.FastReductionAlgToolFactory import \
    FastReductionAlgToolFactory

# import modules concerned with extracting scenario paramters
# from a scenario string
from TrigHLTJetHypo.scenario_agg import scenario_agg
from TrigHLTJetHypo.scenario_dijet import scenario_dijet
from TrigHLTJetHypo.scenario_fbdjnoshared import scenario_fbdjnoshared
from TrigHLTJetHypo.scenario_fbdjshared import scenario_fbdjshared
from TrigHLTJetHypo.scenario_simple import scenario_simple
from TrigHLTJetHypo.prefilter_prefilter import prefilter_prefilter

toolfactory = FastReductionAlgToolFactory()

from DecisionHandling.TrigCompositeUtils import isLegId, getLegIndexInt

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

# the following method is to be deletesd once the scenario modukes are present
def make_repeatedCond(tree_id, tree_pid, multiplicity=1, chainPartInd=-1, conditionMakers=[]):
    """makes a RepeatedConditionConfigurer from explicit arguments)"""
    
    toolclass, name =  toolfactory('RepeatedConditionConfigTool')
    repeated_args = {'name': name}
    repeated_args['conditionMakers'] = conditionMakers
    repeated_args['id'] =  tree_id
    repeated_args['pid'] =  tree_pid
    repeated_args['multiplicity'] = multiplicity
    repeated_args['chainPartInd'] = chainPartInd    

    return toolclass(**repeated_args)

def make_repeatedObj(repArgs, conditionMakers=[]):
    """makes a RepeatedConditionConfigurer from an objects holding
    the necessary parameters, usually built by a scenanario_XX module."""

    return make_repeatedCond(tree_id=repArgs.tree_id,
                             tree_pid=repArgs.tree_pid,
                             multiplicity=repArgs.multiplicity,
                             chainPartInd=repArgs.chainPartInd,
                             conditionMakers=conditionMakers)
                      

def make_fastreduct_rootnode():

    toolclass, name =  toolfactory('all')
    args = {'name':  name}
    conditionMakers = [toolclass(**args)]

    repeatedCondition = make_repeatedCond(tree_id=0,
                                          tree_pid=0,
                                          conditionMakers=conditionMakers)

    filtRepeatedCondition = make_repeatedCond(tree_id=0, tree_pid=0)
 
    return repeatedCondition, filtRepeatedCondition


def buildHypoHelperConfigTool(params):
    """the parameter object pased in is build in modules handloing
    scenarios. It contains the information needed to build the AlgTool
    that intialiases a hypo helper AlgTool"""

    root_cond, root_filt = make_fastreduct_rootnode()
    
    repcondobjs = [root_cond]
    for ra in params.repcondargs:
        condobjs = []
        for key, vals in ra.condargs:
            toolclass, name = toolfactory(key)
            vals['name'] = name
            condobjs.append(toolclass(**vals))

        repcondobjs.append(make_repeatedObj(ra, condobjs))

    repfiltobjs = [root_filt]
    for ra in params.repfiltargs:
        condobjs = []
        for key, vals in ra.condargs:
            toolclass, name = toolfactory(key)
            vals['name'] = name
            condobjs.append(toolclass(**vals))

        repfiltobjs.append(make_repeatedObj(ra, condobjs))

    toolclass, name = toolfactory('HelperToolConfigTool')
                           
    vals = {'name': name,
            'conditionMakers': repcondobjs,
            'filtConditionMakers': repfiltobjs,
            'treeVector': params.treevec,
            'leafVector': params.leafvec,
            }
                           
    return toolclass(**vals)


def process_simple(chain_parts, startLabelIndex):
    """Obtain the paramters needed to build an AlgTool
    to initialise a jet hypo HelperAlgTool"""

    # obtain a list of parameter objects that will be used
    # to build a helper config AlgTools
    helper_params = scenario_simple(chain_parts, startLabelIndex)

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


def make_fastreduction_configurers(chain_dict, startLabelIndex):
    """Create HelperToolConfigTool  instances. Each instance
    configures a FastReduction tree. Chain parts with the 'simple' scenario
    use used to form a single HelperToolConfigTool. The information 
    may be spread over a number of chain parts. 

    There is at most one chain part with a non-simple scenario. 
    This may give rise to > 1 HelperToolConfigTool instance - as this
    is how jet sharing among Conditions is handled.

    If there are bith simple and non-simple scenarios, there will be
    n HelperToolConfigTool instances, where n >=2: one for the simple 
    scenario chain parts, and n-1 for the non-simple scenario.
    """
    

    chain_parts = chain_dict['chainParts']
    simple_chainparts = [
        cp for cp in chain_parts if cp['hypoScenario'] == 'simple']

    helperToolConfigTools = []

    if simple_chainparts:
        helperToolConfigTools.extend(process_simple(simple_chainparts,
                                                    startLabelIndex))

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
        chainPartInd = startLabelIndex + len(chain_parts) - 1

        helperToolConfigTools.extend(process_nonsimple(scenario,
                                                       chainPartInd))
        
    return helperToolConfigTools


def make_prefilter_configurers(chain_dict):
    """Interim prefiltering code - handles only eta-phi holes"""

    pf_strings = []
    chain_parts = [cp for cp in chain_dict['chainParts'] if
                   cp['signature'] == 'Jet' and 'prefilters' in cp]

    [pf_strings.extend(cp['prefilters']) for cp in chain_parts]
    pf_strings = [s for s in pf_strings if s.startswith('prefilter')]

    if not  pf_strings:
        return []
    
    assert len(pf_strings) == 1
    condargs = prefilter_prefilter(pf_strings[0])

    assert len(condargs) == 2  # eta, phi

    condobjs = []
    for key, vals in condargs:
        toolclass, name = toolfactory(key)
        vals['name'] = name
        condobjs.append(toolclass(**vals))

    return condobjs
    

def getLabelIndices(chain_dict):

    start_index = 0
    if isLegId(chain_dict['chainName']):
        start_index = getLegIndexInt(chain_dict['chainName'])

    end_index = start_index + len(chain_dict['chainParts'])

    return start_index, end_index


def  hypotool_from_chaindict(chain_dict, visit_debug=False):

    startLabelIndex, endLabelIndex = getLabelIndices(chain_dict)

    helperToolConfigTools =  make_fastreduction_configurers(chain_dict,
                                                            startLabelIndex)

    prefilterConfigTools = make_prefilter_configurers(chain_dict)

    toolclass, name = toolfactory('helper_tool')
    args = {'name': name,
            'HypoConfigurers': helperToolConfigTools,
            'prefiltConditionMakers': prefilterConfigTools
            }
    
    helper_tool = toolclass(**args)

    toolclass, name =  toolfactory('hypo_tool')
    
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

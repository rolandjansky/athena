# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.FastReductionAlgToolFactory import toolfactory

# import modules concerned with extracting scenario paramters
# from a scenario string
from TrigHLTJetHypo.scenario_ht import scenario_ht
from TrigHLTJetHypo.scenario_dijet import scenario_dijet
from TrigHLTJetHypo.scenario_fbdjnoshared import scenario_fbdjnoshared
from TrigHLTJetHypo.scenario_fbdjshared import scenario_fbdjshared
from TrigHLTJetHypo.scenario_simple import scenario_simple

from TrigHLTJetHypo.prefilter_mask import prefilter_mask
from TrigHLTJetHypo.prefilter_ptrange import prefilter_ptrange
from TrigHLTJetHypo.prefilter_clean import prefilter_clean

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

import re

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

# Dictionary to interpret / map scenario aliases into actual scenario strings that can be understood by scenario_XX.py
aliasesDict = {
  'DJMASS200j20'                 : 'DIJET20j12ptXX200djmass',
  'DJMASS350j20'                 : 'DIJET20j12ptXX350djmass',
  'DJMASS500j35'                 : 'DIJET35j12ptXX500djmass',
  'DJMASS700j35'                 : 'DIJET35j12ptXX700djmass',
  'DJMASS1000j35'                : 'DIJET35j12ptXX1000djmass',
  'DJMASS700j40'                 : 'DIJET40j12ptXX700djmass',
  'DJMASS700j50x0eta240'         : 'DIJET50j12ptXX0j12eta240XX700djmass',
  'DJMASS700j80x0eta240'         : 'DIJET80j12ptXX0j12eta240XX700djmass',
  'DJMASS900j50'                 : 'DIJET50j12ptXX900djmass',
  'DJMASS1000j50'                : 'DIJET50j12ptXX1000djmass',
  'DJMASS1000j50dphi240'         : 'DIJET50j12ptXX1000djmassXXdjdphi240',
  'DJMASS1000j50dphi200x400deta' : 'DIJET50j12ptXX1000djmassXXdjdphi200XX400djdeta',
}

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



def process_ht(scenario, chainPartInd):
    """Obtain the paramters needed to build an AlgTool
    to initialise a jet hypo HelperAlgTool"""

    # obtain a list of parameter objects that will be used
    # to build a helper config AlgTools
    helper_params = scenario_ht(scenario, chainPartInd)

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
    
    # interpret scenario aliases
    if scenario in aliasesDict.keys(): scenario = aliasesDict[scenario]

    router = {
        'HT': process_ht,
        'DIJET': process_dijet,
        'FBDJSHARED': process_fbdjshared,
        'FBDJNOSHARED': process_fbdjnoshared,
    }

    # get scenario stub and make sure is correct
    pattern = r'^(?P<stub>[A-Z]+)'
    rgx     = re.compile(pattern)
    m       = rgx.match(scenario)
    assert m is not None,'No scenario stub was found'
    groupdict = m.groupdict()
    assert groupdict['stub'] in router,'scenario stub ({}) not recognized'.format(groupdict['stub'])

    return router[groupdict['stub']](scenario, chainPartInd)  # list of HelperToolConfigTool


def make_fastreduction_configurers(chain_dict):
    """Create HelperToolConfigTool  instances. Each instance
    configures a FastReduction tree. Chain parts with the 'simple' scenario
    are used to form a single HelperToolConfigTool. The information 
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

    simple_cpis = [cp['chainPartIndex'] for cp in simple_chainparts]

    # check that all the simple scenario parts occur before 
    # non-simple scenario chain parts

    if simple_cpis:
        assert simple_cpis == sorted(simple_cpis), "disordered chain parts" 
        assert simple_cpis[-1] - simple_cpis[0] == len(simple_cpis) - 1, "nonsequential chainParts"
    helperToolConfigTools = []

    # check for SHARED markers (chainPart['tboundary'] = 'SHARED')
    # in the list of simple chain parts.
    # Get a tree configuration each time SHARED == 1 is encountered.
    if simple_chainparts:

        assert simple_chainparts[-1]['tboundary'] == ''

        tree_cps = []
        for cp in simple_chainparts:
            tree_cps.append(cp)
            if cp['tboundary'] == 'SHARED':
                helperToolConfigTools.extend(process_simple(tree_cps))
                tree_cps = []

        # tree_cps  cannot be empty here
        assert tree_cps
        helperToolConfigTools.extend(process_simple(tree_cps))

    scenario_chainparts =[
        cp for cp in chain_parts if cp['hypoScenario'] != 'simple']

    if scenario_chainparts:
        for scenario_chainpart in scenario_chainparts:
            # scenario_chainpart = scenario_chainparts[0]
            
            # We only allow threshold != 0 for the simple scenario.
            assert scenario_chainpart['threshold'] == '0'
        
            scenario = scenario_chainpart['hypoScenario']
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
    
    # if not pre filter strings (pf_strings) are found in the chainDict,
    # a PassThroughFilter configurer is made.

    if not pf_strings:
        return [makePassThroughFilterConfigurer()]

    # route the prefilter strings to the appropriate handler
    prefilter_router = {
        'MASK': prefilter_mask,
        'PTRANGE': prefilter_ptrange,
        'CLEAN': prefilter_clean,
    }

    pattern = r'(?P<stub>[A-Z]*)'
    rgx     = re.compile(pattern)

    filters = []
    for pf_string in pf_strings:
        # get prefilter stub and make sure is correct
        m = rgx.match(pf_string)
        assert m is not None,'No prefilter stub was found'
        groupdict = m.groupdict()
        assert groupdict['stub'] in prefilter_router,'prefilter stub ({}) not recognized'.format(groupdict['stub'])
        filters.append(prefilter_router[groupdict['stub']](pf_string))
        
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

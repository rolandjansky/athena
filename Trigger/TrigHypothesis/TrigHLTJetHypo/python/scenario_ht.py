# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.RepeatedConditionParams import RepeatedConditionParams
from TrigHLTJetHypo.FilterParams import FilterParams
from TrigHLTJetHypo.HelperConfigToolParams import HelperConfigToolParams
from TrigHLTJetHypo.ConditionDefaults import defaults
from TrigHLTJetHypo.make_treevec import make_treevec

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

import re
from copy import deepcopy

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

pattern_pt_threshold = r'^HT(?P<htlo>\d+)'\
    r'(XX(?P<ptlo>\d*)pt(?P<pthi>\d*))?'

pattern_et_threshold = r'^HT(?P<htlo>\d+)'\
    r'(XX(?P<etlo>\d*)et(?P<ethi>\d*))?'

pattern_common = r'(XX(?P<etalo>\d*)eta(?P<etahi>\d*))?'\
    r'(XX(?P<jvtlo>\d*)jvt)?$'

pattern_ptfull = pattern_pt_threshold + pattern_common
rgx_pt         = re.compile(pattern_ptfull)

pattern_etfull = pattern_et_threshold + pattern_common
rgx_et         = re.compile(pattern_etfull)

def get_conditionfilter_args_from_matchdict(groupdict, threshold_var):
    """ Extract the arguments used by the filter of the HT condition
    from the dictionary greated during ghe regex matching to the sceanario
    string. THESE CHAINS HAS DEFAULT CONDITION FILTERING"""
    
    # care! if et no match, etlo and etahi are None.
    #       if et match, missing etlo, ethi = '' 
    # same for eta

    if groupdict[threshold_var+'lo'] is None:  # then default filtering for threshold_var
        groupdict[threshold_var+'lo'] = '30'
        groupdict[threshold_var+'hi'] = ''   # will be assigned default value

    if groupdict['etalo'] is None:  # then default filtering for eta
        groupdict['etalo'] = ''  # will be assigned default value
        groupdict['etahi'] = '320'

    condargs = []
    vals = defaults(threshold_var,
                    groupdict[threshold_var+'lo'],
                    groupdict[threshold_var+'hi'])
    condargs.append((threshold_var, deepcopy(vals)))
        
    vals = defaults('eta',
                    groupdict['etalo'],
                    groupdict['etahi'])
    condargs.append(('eta', deepcopy(vals)))

    # optional
    if groupdict['jvtlo'] is not None:
        vals = defaults('jvt', groupdict['jvtlo'])
        condargs.append(('jvt', deepcopy(vals)))

    return condargs


def scenario_ht(scenario, chainPartInd):
    """calculate the parameters needed to generate a hypo helper config AlgTool
    starting from a the hypoScenario which appears in the chainname for
    an HT condition. The HT condition is filtered"""

    assert scenario.startswith('HT'),\
        'routing error, module %s: bad scenario %s' % (__name__, scenario)

    threshold_var = 'pt'
    m = rgx_pt.match(scenario)
    if m is None:
        threshold_var = 'et'
        m = rgx_et.match(scenario)
    groupdict = m.groupdict()

    condargs = []
    vals = defaults('ht',
                    groupdict['htlo'])

    # find the constructor arguments for each elemental condition
    condargs.append(('ht', deepcopy(vals)))
    
    # treeVec is [0, 0] handle non-root nodes here
    repcondargs = [RepeatedConditionParams(tree_id = 1,
                                           tree_pid=0,
                                           chainPartInd=chainPartInd,
                                           condargs=condargs)]

    # get the arguments needed for the HT condition filter
    filterparams = None
    condargs = get_conditionfilter_args_from_matchdict(groupdict,threshold_var)

    if condargs:  # has default filterinf, so always True

        # make repeated conditions that filter the ht condition
        repfiltargs = [RepeatedConditionParams(tree_id=1,
                                               tree_pid=0,
                                               condargs=condargs)]
        filterparams = [FilterParams(typename='ConditionFilter',
                                     args=repfiltargs)]
    else:
        filterparams = [FilterParams(typename='PassThroughFilter')]

    # parameters to initalise the AlgTool that initialises the helper AlgTool

    # treevec[i] gives the tree_id of the parent of the
    # node with tree_id = i
    treevec = make_treevec(repcondargs)
    assert treevec == [0, 0]

    helper_params = HelperConfigToolParams(treevec=treevec,
                                           repcondargs=repcondargs,
                                           filterparams=filterparams)

    return [helper_params]  # a list with one entry per FastReduction tree


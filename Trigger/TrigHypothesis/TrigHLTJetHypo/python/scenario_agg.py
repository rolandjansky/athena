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

pattern = r'^aggSEP(?P<htlo>\d+)ht(?P<hthi>\d*)'\
    r'(SEP(?P<etlo>\d*)et(?P<ethi>\d*))?'\
    r'(SEP(?P<etalo>\d*)eta(?P<etahi>\d*))?$'


rgx = re.compile(pattern)


def get_conditionfilter_args_from_matchdict(groupdict):
    """ Extract the arguments used by the filter of the HT condition
    from the dictionary greated during ghe regex matching to the sceanario
    string. THESE CHAINS HAS DEFAULT CONDITION FILTERING"""
    
    # care! if et no match, etlo and etahi are None.
    #       if et match, missing etlo, ethi = '' 
    # same for eta

    if groupdict['etlo'] is None:  # then default filtering for et
        groupdict['etlo'] = '30'
        groupdict['ethi'] = ''   # will be assigned default value

    if groupdict['etalo'] is None:  # then default filtering for eta
        groupdict['etalo'] = ''  # will be assigned default value
        groupdict['etahi'] = '320'

    condargs = []
    vals = defaults('et',
                    groupdict['etlo'],
                    groupdict['ethi'])
    condargs.append(('et', deepcopy(vals)))
        
    vals = defaults('eta',
                    groupdict['etalo'],
                    groupdict['etahi'])

    condargs.append(('eta', deepcopy(vals)))

    return condargs


def scenario_agg(scenario, chainPartInd):
    """calculate the parameters needed to generate a hypo helper config AlgTool
    starting from a the hypoScenario which appears in the chainname for
    an HT condition. The HT condition is filtered"""

    assert scenario.startswith('agg'),\
        'routing error, module %s: bad scenario %s' % (__name__, scenario)

    m = rgx.match(scenario)
    groupdict = m.groupdict()

    condargs = []
    vals = defaults('ht',
                    groupdict['htlo'],
                    groupdict['hthi'])

    # find the constructor arguments for each elemental condition
    condargs.append(('ht', deepcopy(vals)))
    
    # treeVec is [0, 0] handle non-root nodes here
    repcondargs = [RepeatedConditionParams(tree_id = 1,
                                           tree_pid=0,
                                           chainPartInd=chainPartInd,
                                           condargs=condargs)]

    # get the arguments needed for thr HT condition filter
    filterparams = None
    condargs = get_conditionfilter_args_from_matchdict(groupdict)

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


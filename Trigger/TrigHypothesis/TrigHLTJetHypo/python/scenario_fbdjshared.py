# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.RepeatedConditionParams import RepeatedConditionParams
from TrigHLTJetHypo.FilterParams import FilterParams
from TrigHLTJetHypo.HelperConfigToolParams import HelperConfigToolParams
from TrigHLTJetHypo.ConditionDefaults import defaults
from TrigHLTJetHypo.make_treevec import make_treevec

from copy import deepcopy

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)


def scenario_fbdjshared(scenario, chainPartInd):
    """produce a list with two HelperToolConfigTool instances
    This list will initialise a Gaudi ToolArray

    This scenario has no arguments - it is illustrative only

    The tree vector 0 is [0, 0, 0]
    pos 0: root; pos 1 backward jet; pos 2 forward jet;

    The tree vector 0 is [0, 0, 1, 1]
    pos 0: root; pos 1 dijet; pos 2 dijet j0; pos 3 dijet j1;

    There is a  Repeated Condition configurer for each position

    No  conditions are filtered"""

    assert scenario.startswith('FBDJSHARED'),\
        'routing error, module %s: bad scenario %s' % (__name__, scenario)


    # list for the repeatedCondition parameters
    repcondargs = []
    
    # find the constructor arguments for each elemental condition
    condargs = []
    
    # backward jet
    vals = defaults('et', lo='50')
    condargs.append(('et', deepcopy(vals)))
    
    vals = defaults('neta', lo='-500')
    condargs.append(('neta', deepcopy(vals)))

    repcondargs.append(RepeatedConditionParams(tree_id = 1,
                                               tree_pid=0,
                                               chainPartInd=chainPartInd,
                                               condargs=condargs))

    # forward jet
    condargs = []

    vals = defaults('et', lo='50')
    condargs.append(('et', deepcopy(vals)))

    vals = defaults('peta', hi='500')
    condargs.append(('peta', deepcopy(vals)))

    repcondargs.append(RepeatedConditionParams(tree_id = 2,
                                               tree_pid=0,
                                               chainPartInd=chainPartInd,
                                               condargs=condargs))


    # make pass through filter params for each condition in the tree.
    nconds = len(repcondargs)
    filterparams = [FilterParams(typename='PassThroughFilter')
                    for i in range(nconds)]

    # treevec[i] gives the tree_id of the parent of the
    # node with tree_id = i
    treevec = make_treevec(repcondargs)
    assert treevec == [0, 0, 0]
    
    helperparams = HelperConfigToolParams(treevec=treevec,
                                          repcondargs=repcondargs,
                                          filterparams=filterparams)
    helperparamslist = [helperparams]

    #############################################33
    # new helperparams...
    
    # Conditions for the dijet
    repcondargs = []
    condargs = []
    vals = defaults('djmass', lo='34')
    condargs.append(('djmass', deepcopy(vals)))

    vals = defaults('djdphi', lo='260')
    condargs.append(('djdphi', deepcopy(vals)))
    

    repcondargs.append(RepeatedConditionParams(tree_id = 1,
                                               tree_pid=0,
                                               chainPartInd=-1,
                                               condargs=condargs))
 

    
    # Conditions for the dijet j1
    condargs = []
    vals = defaults('et', lo='10')
    condargs.append(('et', deepcopy(vals)))

    vals = defaults('eta', lo='0', hi='490')
    condargs.append(('eta', deepcopy(vals)))

    repcondargs.append(RepeatedConditionParams(tree_id = 2,
                                               tree_pid=1,
                                               chainPartInd=chainPartInd,
                                               condargs=condargs))

       
    # Conditions for the dijet j2
    condargs = []
    vals = defaults('et', lo='20')
    condargs.append(('et', deepcopy(vals)))

    vals = defaults('eta', lo='0', hi='490')
    condargs.append(('eta', deepcopy(vals)))

    repcondargs.append(RepeatedConditionParams(tree_id = 3,
                                               tree_pid=1,
                                               chainPartInd=chainPartInd,
                                               condargs=condargs))


    # make pass through filter params for each condition in the tree.
    nconds = len(repcondargs)
    filterparams = [FilterParams(typename='PassThroughFilter')
                    for i in range(nconds)]

    # parameters to initalise the AlgTool that initialises the helper AlgTool

    # treevec[i] gives the tree_id of the parent of the
    # node with tree_id = i
    treevec = make_treevec(repcondargs)
    assert treevec == [0, 0, 1, 1]
 
    helperparams = HelperConfigToolParams(treevec=treevec,
                                           repcondargs=repcondargs,
                                           filterparams=filterparams)
    
    # a list is one entry per FastReduction tree
    helperparamslist.append(helperparams)
    return helperparamslist

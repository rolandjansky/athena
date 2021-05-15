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


pattern = r'^FBDJNOSHARED'\
    r'(?P<j1etlo>\d*)et(?P<j1ethi>\d*)XX'\
    r'(?P<j2etlo>\d*)et(?P<j2ethi>\d*)XX'\
    r'(?P<masslo>\d*)mass(?P<masshi>\d*)XX'\
    r'(?P<fbetlo>\d*)fbet(?P<fbethi>\d*)$'

rgx = re.compile(pattern)

def get_fb_jet_args(groupdict, back):
    """get the elemental condition args (cuts) for the backward and forward 
    jets"""

    condargs = []
    vals = defaults('et', lo=groupdict['fbetlo'], hi=groupdict['fbethi'])
    condargs.append(('et', deepcopy(vals)))

    if back:
        vals = defaults('neta')
        condargs.append(('neta', deepcopy(vals)))
    else:
        vals = defaults('peta')
        condargs.append(('peta', deepcopy(vals)))

    return condargs


def get_dijet_args(groupdict):
    """get the elemental condition args (cuts) for the backward and forward 
    jets"""

    condargs = []
    vals = defaults('djmass',
                    lo=groupdict['masslo'],
                    hi=groupdict['masshi'])
    
    condargs.append(('djmass', deepcopy(vals)))
    
    vals = defaults('djdphi', lo='260')
    condargs.append(('djdphi', deepcopy(vals)))

    return condargs
    


def get_dijet_jet_args(groupdict, jstr):
    """get arguments for each of the jets that make up the dijet"""

    assert  jstr in ('j1', 'j2')
    
    condargs = []
    vals = defaults('et', lo=groupdict[jstr+'etlo'], hi=groupdict[jstr+'ethi'])
    condargs.append(('et', deepcopy(vals)))

    vals = defaults('eta', lo='0', hi='490')
    condargs.append(('eta', deepcopy(vals)))

    return condargs


def scenario_fbdjnoshared(scenario, chainPartInd):
    """produce a list with one element containing a HelperToolConfigTool.
    This list will inialise a Gaudi ToolArray

    The tree vector is [0, 0, 0, 0, 3, 3]
    pos 0: root; pos 1 backward jet; pos 2 forward jet; pos 3: dijet; 
    pos 4: dijet j1; pos 5: dijet jet 2
    
    There is a  Repeated Condition configurer for each position

    No  conditions are filtered"""

    assert scenario.startswith('FBDJNOSHARED'),\
        'routing error, module %s: bad scenario %s' % (__name__, scenario)

    m = rgx.match(scenario)
    groupdict = m.groupdict()

    # list for the repeatedCondition parameters
    repcondargs = []
    
    # find the constructor arguments for each elemental condition

    # backward jet
    condargs = get_fb_jet_args(groupdict, back=True)
    # treeVec is [0, 0, 0, 0, 3, 3] handle non-root nodes here

    repcondargs.append(RepeatedConditionParams(tree_id = 1,
                                               tree_pid=0,
                                               chainPartInd=chainPartInd,
                                               condargs=condargs))

    # forward jet
    condargs = get_fb_jet_args(groupdict, back=False)

    repcondargs.append(RepeatedConditionParams(tree_id = 2,
                                               tree_pid=0,
                                               chainPartInd=chainPartInd,
                                               condargs=condargs))
     
    # Conditions for the dijet
    condargs = get_dijet_args(groupdict)
    
    repcondargs.append(RepeatedConditionParams(tree_id = 3,
                                               tree_pid=0,
                                               chainPartInd=-1,
                                               condargs=condargs))
    
    # Conditions for the dijet j1
    condargs = get_dijet_jet_args(groupdict, 'j1')
    repcondargs.append(RepeatedConditionParams(tree_id = 4,
                                               tree_pid=3,
                                               chainPartInd=chainPartInd,
                                               condargs=condargs))

       
    # Conditions for the dijet j2
    condargs = get_dijet_jet_args(groupdict, 'j2')
    repcondargs.append(RepeatedConditionParams(tree_id = 5,
                                               tree_pid=3,
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
    assert treevec == [0, 0, 0, 0, 3, 3]
    
    helper_params = HelperConfigToolParams(treevec=treevec,
                                           repcondargs=repcondargs,
                                           filterparams=filterparams)
    
    return [helper_params]  # a list is one entry per FastReduction tree


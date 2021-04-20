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

pattern = r'^dijetSEP'\
    r'((?P<j12etlo>\d*)j12et(?P<j12ethi>\d*)SEP|'\
    r'(?P<j1etlo>\d*)j1et(?P<j1ethi>\d*)SEP'\
    r'(?P<j2etlo>\d*)j2et(?P<j2ethi>\d*)SEP)'\
    r'((?P<j12etalo>\d*)j12eta(?P<j12etahi>\d*)SEP|'\
    r'((?P<j1etalo>\d*)j1eta(?P<j1etahi>\d*)SEP)?'\
    r'((?P<j2etalo>\d*)j2eta(?P<j2etahi>\d*)SEP)?)?'\
    r'(?P<djmasslo>\d*)djmass(?P<djmasshi>\d*)'\
    r'(SEP(?P<djdphilo>\d*)djdphi(?P<djdphihi>\d*))?'\
    r'(SEP(?P<djdetalo>\d*)djdeta(?P<djdetahi>\d*))?$'

rgx = re.compile(pattern)


def get_dijet_args_from_matchdict(groupdict, keystubs):

    # find the constructor arguments for each keys starting
    # with the elements of the ksystubs list
    
    condargs = []

    dj_keystubs = set([k[:-2] for k in groupdict if k.startswith('dj')])

    for k in dj_keystubs:
        vals = defaults(k, groupdict[k+'lo'], groupdict[k+'hi'])
        condargs.append((k, deepcopy(vals)))

    return condargs


def get_singlejet_args_from_matchdict(groupdict, jstr):
    """obtain the single jet cuts"""

    assert jstr in ('j1','j2')
    
    condargs = []

    vals = defaults('et', groupdict[jstr+'etlo'], groupdict[jstr+'ethi'])
    condargs.append(('et', deepcopy(vals)))
   
    vals = defaults('eta', groupdict[jstr+'etalo'], groupdict[jstr+'etahi'],)
    condargs.append(('eta', deepcopy(vals)))

    return condargs


def scenario_dijet(scenario, chainPartInd):
    """ original dijet scenario work by J. Bossio.

    produce a singe element list containing a HelperToolConfigTool.

    Decodes the dijet scenario to form aConditions to select jet pairs
    with Conditions on the jet pair, and each of the two jets separately.
    
    Currently supported cuts:
    - dijet mass
    - dijet phi
    - dijet eta
    - jet1 et, eta
    - jet2 et, eta
    
    - default values are used for unspecified cuts, except for delta phi and 
    delta eta for which no cut is applied if not requested
    The cut set can be extended according to the pattern


    example scenarios:
    dijetSEP50j1etSEP80j2etSEP0j1eta240SEP0j2eta320SEP700djmass 
       mixed j1/j2 et/eta values
   
    dijetSEP80j12etSEP0j12eta240SEP700djmass                    
       same et/eta cuts for j1 and j2

    dijetSEP80j12etSEP700djmassSEP26djdphi
       including delta phi cut

    dijetSEP70j12etSEP1000djmassSEP20djdphiSEP40djdeta
       including delta eta cut

    The tree vector is [0, 0, 1, 1]
    # pos 0: root; pos 1 dijet cuts; pos 2: j1 cuts; pos 3: j2 cuts'"""

    assert scenario.startswith('dijet'), \
        'routing error, module %s: bad scenario %s' % (__name__, scenario)

    # Note:
    # j12et and j12eta respectively use the same values for j1et/j2et
    # and j1eta/j2eta
    # j1eta/j2eta,j12eta is allowed not to be in the scenario,
    # default values will be used in such a case: 0eta490
    # djdphi/djdeta is allowed not to be in the scenario,
    # no djdphi/djdeta cut will be applied in such a case

 
    m = rgx.match(scenario)

    assert m is not None, \
        'scenario_dijet.py - regex pat %s does not match scenario %s' % (
            pattern, scenario)
    
    groupdict = m.groupdict()

    to_delete = [k for k in groupdict if groupdict[k] is None]
    for k in to_delete: del groupdict[k]
    
    to_delete = []
    for k in ('j12etlo', 'j12ethi', 'j12etalo', 'j12etahi'):
        if k in groupdict:
            to_delete.append(k)
            new_key = 'j1' + k[len('j12'):]
            groupdict[new_key] = groupdict[k]
            new_key = 'j2' + k[len('j12'):]
            groupdict[new_key] = groupdict[k]

    for k in to_delete:
        del groupdict[k]

    # always make an eta cut even if not in scenario
    if 'j1etalo' not in groupdict: groupdict['j1etalo'] = '0'
    if 'j1etahi' not in groupdict: groupdict['j1etahi'] = '490'
    if 'j2etalo' not in groupdict: groupdict['j2etalo'] = '0'
    if 'j2etahi' not in groupdict: groupdict['j2etahi'] = '490'


    # get the  Condition parameters (cut values) for the dijet Conditions
    keystubs = set([k[:-2] for k in groupdict if k.startswith('dj')])
    condargs = get_dijet_args_from_matchdict(groupdict, keystubs)

    # treeVec is [0, 0, 1, 1] handle non-root nodes here

    repcondargs = [RepeatedConditionParams(tree_id = 1,
                                           tree_pid=0,
                                           chainPartInd=-1,
                                           condargs=condargs)]
   
    # make the condargs and the containing rep condition for j1

    condargs = get_singlejet_args_from_matchdict(groupdict, 'j1')

    repcondargs.append(RepeatedConditionParams(tree_id = 2,
                                           tree_pid=1,
                                           chainPartInd=chainPartInd,
                                           condargs=condargs))
          
    # make the condargs and the containing rep condition for j2

    condargs = get_singlejet_args_from_matchdict(groupdict, 'j2')

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
    
    helper_params = HelperConfigToolParams(treevec=treevec,
                                           repcondargs=repcondargs,
                                           filterparams=filterparams)
    
    return [helper_params]  # a list is one entry per FastReduction tree




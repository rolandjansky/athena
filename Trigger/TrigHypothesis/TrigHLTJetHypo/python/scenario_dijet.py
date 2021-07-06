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

pattern_thresh_pt = r'^DIJET'\
    r'((?P<j12ptlo>\d*)j12pt(?P<j12pthi>\d*)XX|'\
    r'(?P<j1ptlo>\d*)j1pt(?P<j1pthi>\d*)XX'\
    r'(?P<j2ptlo>\d*)j2pt(?P<j2pthi>\d*)XX)'


pattern_thresh_et = r'^DIJET'\
    r'((?P<j12etlo>\d*)j12et(?P<j12ethi>\d*)XX|'\
    r'(?P<j1etlo>\d*)j1et(?P<j1ethi>\d*)XX'\
    r'(?P<j2etlo>\d*)j2et(?P<j2ethi>\d*)XX)'


pattern_common =  r'((?P<j12etalo>\d*)j12eta(?P<j12etahi>\d*)XX|'\
    r'((?P<j1etalo>\d*)j1eta(?P<j1etahi>\d*)XX)?'\
    r'((?P<j2etalo>\d*)j2eta(?P<j2etahi>\d*)XX)?)?'\
    r'(?P<djmasslo>\d*)djmass(?P<djmasshi>\d*)'\
    r'(XX(?P<djdphilo>\d*)djdphi(?P<djdphihi>\d*))?'\
    r'(XX(?P<djdetalo>\d*)djdeta(?P<djdetahi>\d*))?$'

    
pattern_pt = pattern_thresh_pt + pattern_common
rgx_pt = re.compile(pattern_pt)

pattern_et = pattern_thresh_et + pattern_common
rgx_et = re.compile(pattern_et)


def get_dijet_args_from_matchdict(groupdict, keystubs):

    # find the constructor arguments for each keys starting
    # with the elements of the ksystubs list
    
    condargs = []

    dj_keystubs = set([k[:-2] for k in groupdict if k.startswith('dj')])

    for k in dj_keystubs:
        vals = defaults(k, groupdict[k+'lo'], groupdict[k+'hi'])
        condargs.append((k, deepcopy(vals)))

    return condargs


def get_singlejet_args_from_matchdict(groupdict, threshold_var, jstr):
    """obtain the single jet cuts."""

    assert jstr in ('j1','j2')
    assert threshold_var in ('pt', 'et')
    
    condargs = []

    vals = defaults(threshold_var,
                    groupdict[jstr+threshold_var+'lo'],
                    groupdict[jstr+threshold_var+'hi'])
    
    condargs.append((threshold_var, deepcopy(vals)))
   
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
    DIJET50j1etXX80j2etXX0j1eta240XX0j2eta320XX700djmass
       mixed j1/j2 et/eta values
   
    DIJET80j12etXX0j12eta240XX700djmass
       same et/eta cuts for j1 and j2

    DIJET80j12etXX700djmassXX26djdphi
       including delta phi cut

    DIJET70j12etXX1000djmassXX20djdphiXX40djdeta
       including delta eta cut

    The tree vector is [0, 0, 1, 1]
    # pos 0: root; pos 1 dijet cuts; pos 2: j1 cuts; pos 3: j2 cuts'"""

    assert scenario.startswith('DIJET'), \
        'routing error, module %s: bad scenario %s' % (__name__, scenario)

    # Note:
    # j12et and j12eta respectively use the same values for j1et/j2et
    # and j1eta/j2eta
    # j1eta/j2eta,j12eta is allowed not to be in the scenario,
    # default values will be used in such a case: 0eta490
    # djdphi/djdeta is allowed not to be in the scenario,
    # no djdphi/djdeta cut will be applied in such a case


    threshold_var = 'pt'
    m = rgx_pt.match(scenario)

    if m is None:
        threshold_var = 'et'
        m = rgx_et.match(scenario)

    assert m is not None, \
        'scenario_dijet.py - regex pat %s or %s do not match scenario %s' % (
            pattern_pt, pattern_et, scenario)
    
    groupdict = m.groupdict()

    to_delete = [k for k in groupdict if groupdict[k] is None]
    for k in to_delete: del groupdict[k]

    def massage_thresh(threshold_var, gdict):
        to_delete = []
        for k in ('j12'+threshold_var+'lo',
                  'j12'+threshold_var+'hi',
                  'j12etalo',
                  'j12etahi'):
            if k in gdict:
                to_delete.append(k)
                new_key = 'j1' + k[len('j12'):]
                gdict[new_key] = gdict[k]
                new_key = 'j2' + k[len('j12'):]
                gdict[new_key] = gdict[k]

        for k in to_delete:
            del gdict[k]

        return gdict
        
    groupdict = massage_thresh(threshold_var, groupdict)

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

    condargs = get_singlejet_args_from_matchdict(groupdict,
                                                 threshold_var,
                                                 'j1')

    repcondargs.append(RepeatedConditionParams(tree_id = 2,
                                           tree_pid=1,
                                           chainPartInd=chainPartInd,
                                           condargs=condargs))
          
    # make the condargs and the containing rep condition for j2

    condargs = get_singlejet_args_from_matchdict(groupdict,
                                                 threshold_var,
                                                 'j2')

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




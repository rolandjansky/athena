# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.RepeatedConditionParams import RepeatedConditionParams
from TrigHLTJetHypo.FilterParams import FilterParams
from TrigHLTJetHypo.HelperConfigToolParams import HelperConfigToolParams
from TrigHLTJetHypo.ConditionDefaults import defaults
from TrigHLTJetHypo.make_treevec import make_treevec

from TriggerMenuMT.HLT.Menu.SignatureDicts import (
    JetChainParts_Default,)

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

from copy import deepcopy

# make a list of all possible cut items for the simple scenario

all_elemental_keys = ('etaRange', 'jvt', 'smc',
                      'threshold', 'momCuts', 'bdips')

# Extract moment cuts
def _cuts_from_momCuts(momCuts):
    separator = 'XX'
    args      = momCuts.split(separator)
    if len(args) > 1:
        return args
    return ''

def get_condition_args_from_chainpart(cp):

    # determine which cut variable are present in this chain part,
    # and find their chain part values
    elemental_keys = [k for k in all_elemental_keys if k  in cp]
    cp_elemental_args = {k : cp[k] for k in elemental_keys if cp[k]}
        
    # remove place holders
    todelete = []
    for k, v in cp_elemental_args.items():
        if v == 'nosmc': todelete.append(k)

    for k in todelete: del cp_elemental_args[k]

    # decode the chain part cut values to find the numerical cut values
        
    condargs = list()
    
    for k, v in cp_elemental_args.items():
        if k == 'threshold':
            key = 'pt'
            vals = defaults(key, lo=v)
            condargs.append((key, deepcopy(vals)))
                
        if k == 'etaRange':
            key='eta'
            lo, hi = v.split(key)
            vals = defaults(key, lo=lo, hi=hi)
            condargs.append((key, deepcopy(vals)))

        if k == 'smc':
            key = 'smc'
            lo, hi = v.split(key)
            vals = defaults(key, lo=lo, hi=hi)
            condargs.append((key, deepcopy(vals)))
                
        if k == 'jvt':
            key    = 'jvt'
            values = v.split(key)
            assert values[1] == '','jvt condition takes only one argument, two were given' # protection when an upper (not supported) cut is requested
            lo   = values[0]
            vals = defaults(key, lo=lo)
            condargs.append((key, deepcopy(vals)))

        if k == 'bdips':
            key = 'bdips'
            values = v.split(key)
            assert values[1] == '','bdips condition takes only one argument, two were given' # protection when an upper (not supported) cut is requested
            
            #This dictionary maps the bdips efficiency into the WP cut to be applied to the DIPS output
            dips_WPs = {
                '':   float('-inf'),
                '95': -1.6495,
                '90': -0.8703,
                '85': -0.0295,
                '80': 0.7470,
                '77': 1.1540,
                '75': 1.4086,
                '60': 3.0447,
            }

            assert (values[0] in dips_WPs.keys()),f"The efficiency of the specified dips cut \'{v}\' can not be found in the WP dictionary. Please add or remove the WP from the dips WP dictionary."

            lo   = dips_WPs[values[0]]
            vals = defaults(key, lo=lo)
            condargs.append((key, deepcopy(vals)))

        if k == 'momCuts':
            from TrigHLTJetHypo.FastReductionAlgToolFactory import jetMoments
            if 'XX' in v: # several moment cuts are requested

                # loop over requested moment strings
                for cutstr in _cuts_from_momCuts(v): 
                    for moment in jetMoments: # loop over possible jet moments
                        if moment in cutstr:
                            key='mom{}'.format(moment)
                            lo, hi = cutstr.split(key)
                            vals   = defaults(k, lo=lo, hi=hi)
                            vals["moment"] = jetMoments[moment]
                            condargs.append((key, vals))
            else: # a single moment cut is requested
                for moment in jetMoments: # loop over possible jet moments
                    if moment in v:
                        key='mom{}'.format(moment)
                        lo, hi = v.split(key)
                        vals   = defaults(k, lo=lo, hi=hi)
                        vals["moment"] = jetMoments[moment]
                        condargs.append((key, vals))

    return condargs


def scenario_simple(chain_parts):
    """build two lists of RepeatedConditionConfig objects
    one list contains the Conditions to be used by FastReducer,
    and the other Contains the conditions used to filter the Condition.
    Each list has one entry per chain part"""

    assert chain_parts, 'routing error, module %s: no chain parts' % __name__


    # Enforce explicit etaRange in chainPartName for each Jet chain part if:
    # - More than one Jet chain part AND
    # - At least one Jet chain part does not use default etaRange AND
    # - At least one Jet chain part use default etaRange
    # Abort in such a case if chain part using default etaRange does not
    # have etaRange in chainPartName
    
    jetchain_parts = [ cp['signature'] == 'Jet' for cp in chain_parts ]
    if sum(jetchain_parts) > 1: # more than one Jet chain part
        useNonDefault         = 0
        useNonExplicitDefault = 0

        # collect chain part names which do not follow the naming convention
        chainPartNames2print  = [] 
        for cp in chain_parts: # loop over chain parts
            if cp['signature'] != 'Jet':
                # only enforce explicit etaRange by looking at only Jet
                # chain parts
                continue

            # using non-default etaRange
            if cp['etaRange'] != JetChainParts_Default['etaRange']: 
                useNonDefault += 1
            else: # using default etaRange
                
                # etaRange for this chain part not present in chain name
                if cp['etaRange'] not in cp['chainPartName']: 
                    useNonExplicitDefault += 1
                    chainPartNames2print.append(cp['chainPartName'])
        assert not (useNonDefault > 0 and useNonExplicitDefault > 0),\
        'Default etaRange should be explicit in the following '\
            'chain part(s): %s' % [n for n in chainPartNames2print]


    repcondargs = []
    filterparams = []
    
    ncp = 0
    
    # keep track of identical cond_args, which are given the same
    # clique number. 
    # the C++ code will use the clique number for optimisation of
    # the calculation of the combinations
    #
    # current implementation: condition filter not included.
    clique_list = []
    for cp in chain_parts:
        ncp += 1

        # get the Condition parameters (cut values) for the
        # elemental Conditions

        condargs = get_condition_args_from_chainpart(cp)
        
        multiplicity = int(cp['multiplicity'])
        chainPartInd = cp['chainPartIndex']
 
        # make an empty filter condition for the FR condition
        filterparams.append(FilterParams(typename='PassThroughFilter'))

        args = deepcopy(condargs)
        args.append(filterparams)
        clique = None
        try:
            clique = clique_list.index(condargs)
        except ValueError:
            # seen for the first time
            clique_list.append(condargs)
            clique = len(clique_list)-1

        repcondargs.append(RepeatedConditionParams(tree_id = ncp,
                                                   tree_pid=0,
                                                   clique=clique,
                                                   chainPartInd=chainPartInd,
                                                   multiplicity=multiplicity,
                                                   condargs=condargs))


    # treevec[i] gives the tree_id of the parent of the
    # node with tree_id = i
    treevec = make_treevec(repcondargs)
    assert treevec == [0 for i in range(len(chain_parts) + 1)]

    helper_params = HelperConfigToolParams(treevec=treevec,
                                           repcondargs=repcondargs,
                                           filterparams=filterparams)
    
    return [helper_params]  # a list is one entry per FastReduction tree

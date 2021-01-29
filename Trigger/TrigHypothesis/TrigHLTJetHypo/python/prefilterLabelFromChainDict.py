# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from TrigHLTJetHypo.constants import lchars
import re

cut_re = re.compile(r'(?P<lo>\d*)(?P<key>[%s]+)(?P<hi>\d*)' %lchars)

def prefilterArgsFromChainDict(chain_dict):
    """Marshal and return the strings use to build a prefilter for the jet
    hypo. These may be spread across > 1 chainPart within the chainDict."""
    
    chain_parts = [cp for cp in chain_dict['chainParts'] if
                   cp['signature'] in ('Jet', 'Bjet') and 'prefilters' in cp]
    
    prefilter_args = []

    [prefilter_args.extend(cp['prefilters']) for cp in chain_parts]
    
    for p in prefilter_args:
        assert p.startswith('pfltr')
                   
    return prefilter_args
                   

def prefilterLabelFromChainDict(chain_dict):
    """create a jet hypo label. For now, only arguments like
    'prefltrSEP100ceta90SEP100nphi50' are accepted. """

                   
    prefilter_args = prefilterArgsFromChainDict(chain_dict)
    if not prefilter_args: return ''
    
    label = 'root([]simple(['

    arg_spacer = ', '
    for pfa in prefilter_args:
        frags = pfa.split('SEP')[1:]
        assert frags
        label += '('
        for frag in frags:
            cut_match = cut_re.match(frag)
            assert cut_match is not None
            label += frag + arg_spacer
        label = label[:-len(arg_spacer)] + ')'
    label += ']))'

    return label
                  

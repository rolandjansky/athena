# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from __future__ import absolute_import

"""Check that if a scenario apppears in a chain part name, that it also
is a key in the chain part dict. This is to catch errors whereby a
scenario is present in the chain name, but is is not present in the
SignatureDicts.JetChainParts[hypoScenario] list, in which case 
hypoScenario is set to the default. The ommision of the full scenario
name from the list is an easy mistake to make, but detecting the problem can be
difficult"""


from TriggerMenuMT.HLT.Menu.SignatureDicts import JetChainParts

def checkScenarioPresence(chain_parts, chain_name):
    full_scenarios = JetChainParts['hypoScenario']
    headers = set([s.split('XX')[0] for s in full_scenarios])

    headers_in_chain_name = {h: chain_name.count(h) for h in headers}
        
    cp_scenarios = set([cp['hypoScenario'] for cp in chain_parts])

    headers_in_cp = {h: 0 for h in headers}

    for cps in cp_scenarios:
        if cps == 'simple': continue  # 'simple' not in chain name
        for h in headers:
            if cps.startswith(h): 
                headers_in_cp[h] += 1

    bad_headers = [h for h in headers_in_chain_name.keys() if
                   headers_in_chain_name[h] != headers_in_cp[h]]

    return bad_headers


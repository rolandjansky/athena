#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import json
import pprint

import argparse
parser = argparse.ArgumentParser(description='Get the inputs for chain extraction')

parser.add_argument('--chain', dest='chain', action='store', default='',
                   help='What chain are we extracting')
parser.add_argument('--menu_json', dest='input_json', action='store', default='',
                   help='What menu are we extracting the chain from')
                   
args = parser.parse_args()

chain = args.chain
input_json = args.input_json

the_menu = json.loads(open(input_json).read())

print("...extracting",chain,"from menu json",input_json)

if chain not in the_menu['chains']:
    raise Exception("The requested chain isn't in the provided menu json, please try again.")

pprint.pprint(the_menu['chains'][chain])

print("Steps in the chain:")

for isq,seq in enumerate(the_menu['chains'][chain]['sequencers']):

    if not seq:
        print('step',isq+1,'is empty')
        continue
    else:
        print('step',isq+1,'is sequence ',seq)

    other_chains = []
    for chain in the_menu['chains']:
        if seq in the_menu['chains'][chain]['sequencers']:
            other_chains += [chain]
    if len(other_chains) > 0:
        print ('   also used in the following chains:')
        pprint.pprint(other_chains)
    else: 
        print ('   is not used in any other chains.')

    print ('   contains algorithms/hypos:')
    pprint.pprint(the_menu['sequencers'][seq])
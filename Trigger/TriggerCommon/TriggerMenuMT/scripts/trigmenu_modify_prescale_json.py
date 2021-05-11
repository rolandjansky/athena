#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import json

import argparse
parser = argparse.ArgumentParser(description='Get the inputs for activating only the prescale of a given group')

parser.add_argument('-g', '--group',
                    required=True,
                    action='append',
                    help='Which group we are selecting')
parser.add_argument('-p', '--prescale_json',
                    required=True,
                    help='Which prescale json we are modifying')
parser.add_argument('-m', '--menu_json',
                    required=True,
                    help='Name of the menu file')

args          = parser.parse_args()
the_groups = args.group

prescale_json = args.prescale_json
prescale_file = open(prescale_json)
the_prescale  = json.load(prescale_file)
prescale_file.close()

menu_json = args.menu_json
menu_file = open(menu_json)
the_menu  = json.load(menu_file)
menu_file.close()

list_of_good_chains = []
for chain in the_menu['chains']:
    for group in the_groups:
        if group in the_menu['chains'][chain]['groups']:
            list_of_good_chains.append(chain) 

if len(list_of_good_chains)==0:
    raise Exception("There are no chains with the specified group in the Menu file.")

print("[modify_prescale_json] resetting all the chains that do not contain the groups:", the_groups)
print("[modify_prescale_json] number of chains found with the selected groups:", len(list_of_good_chains))

for chain in the_prescale['prescales']:
    if chain not in list_of_good_chains:
        the_prescale['prescales'][chain]['prescale'] = 0
        the_prescale['prescales'][chain]['enabled']  = False

print("[modify_prescale_json] overwriting the prescale json file:", prescale_json)
with open( prescale_json, 'w' ) as fp:
        json.dump( the_prescale, fp, indent=4, sort_keys=False )

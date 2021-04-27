#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import json

import argparse
parser = argparse.ArgumentParser(description='Get the inputs for activating only the prescale of a given group')

parser.add_argument('--group', dest='group', action='store', default='',
                    help='Which group are we selecting')
parser.add_argument('--prescale_json', dest='prescale_json', action='store', default='',
                    help='Which prescale json we are modifying')
parser.add_argument('--menu_json', dest='menu_json', action='store', default='',
                    help='Name of the menu file')
                   
args          = parser.parse_args()
the_group = args.group

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
    if the_group in the_menu['chains'][chain]['groups']:
        list_of_good_chains.append(chain) 

if len(list_of_good_chains)==0:
    raise Exception("There are no chains with the specified group in the Menu file.")

print("[modify_prescale_json] resetting all the chains that do not contain the group:", the_group)
print("[modify_prescale_json] number of chains found with the selected group:", len(list_of_good_chains))

for chain in the_prescale['prescales']:
    if chain not in list_of_good_chains:
        the_prescale['prescales'][chain]['prescale'] = 0
        the_prescale['prescales'][chain]['enabled']  = False

print("[modify_prescale_json] overwriting the prescale json file:", prescale_json)
with open( prescale_json, 'w' ) as fp:
        json.dump( the_prescale, fp, indent=4, sort_keys=False )

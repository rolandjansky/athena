#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
	This script reads the rulebook and find items that are tighter than the primaries that were not included
	TrigMenuRulebook needs to be checked out and installed.
	Currently it needs to be run from the script folder.
	One argument can be supplied specifying which "tag" to use, default is pp_v7 
"""

import importlib
import sys, re
from TriggerJobOpts.TriggerFlags					import TriggerFlags

def_maxlumi = 20000
tag = "pp_v7"
if len(sys.argv) > 1:
	tag = sys.argv[1]

mod_physics_rules = importlib.import_module("TrigMenuRulebook.Physics_%s_rules" % tag)
mod_physics_menu	= importlib.import_module("TriggerMenu.menu.Physics_%s" % tag)
mod_mc_menu				= importlib.import_module("TriggerMenu.menu.MC_%s" % tag)
mod_mc_menu.setupMenu()

base_pattern = "(\d*[a-zA-Z]*)(\d*)(.*)"

def main():
	
	primaries 						= get_primaries(def_maxlumi)
	tigher_than_primaries = get_tigher_than(primaries)
	for (a, b) in sorted(list(tigher_than_primaries)):
		print "%20s > %20s" %(a,b)
	return tigher_than_primaries

def get_primaries(maxlumi):
	primaries = set()
	for item, itemdict in mod_physics_rules.rules.iteritems():
		for lumi, rule in itemdict.iteritems():
			if lumi > maxlumi:	continue
			if "rate" in rule.keys(): break
			if "inputRate" in rule.keys(): break
			if "PS" in rule.keys() and rule["PS"]!=1: break
		else:
			if "comment" not in rule.keys() and "Primary" not in rule["comment"]: print "WTF!!??", lumi, rule
			if item.startswith("L1_"):	item = item.replace("L1_","")
			if item.startswith("HLT_"): item = item.replace("HLT_","")
			primaries.add(item)
	return primaries

def get_tigher_than(primaries):
	tighter_than = set()
	for item in primaries:
		for prop in dir(TriggerFlags):
			if prop[-5:]!='Slice': continue
			sliceName=prop
			m_slice=getattr(TriggerFlags,sliceName)
			for m in m_slice.signatures():
				if m[0] in primaries: continue
				if is_tighter_than(m[0],item): tighter_than.add((m[0],item))
	return tighter_than

def is_tighter_than(item, ref):
	m1 = re.search(base_pattern,item)
	m2 = re.search(base_pattern,ref)
	base1, thr1, rest1 = [m1.group(i) for i in range(1,4)]
	base2, thr2, rest2 = [m2.group(i) for i in range(1,4)]
	if base1 != base2: return False
	#print item, ref, base1, "-", base2
	if rest1==rest2 and int(thr1) > int(thr2): return True
	if rest1 == rest2.replace("loose","medium") and not int(thr1) < int(thr2): return True
	if rest1 == rest2.replace("loose","medium").replace("medium","tight") and not int(thr1) < int(thr2): return True
	tokens1 = item.split("_")
	tokens2 = ref.split("_")
	if len(tokens1) == len(tokens2):
		for t1, t2 in zip(tokens1, tokens2):
			m1 = re.search(base_pattern,t1)
			m2 = re.search(base_pattern,t2)
			base1, thr1, rest1 = [m1.group(i) for i in range(1,4)]
			base2, thr2, rest2 = [m2.group(i) for i in range(1,4)]
			if base1 != base2: return False
			if rest1 != rest2: return False
			if not thr1 or not thr2: return False
			if int(thr1) < int(thr2): return False
		return True
	return False
	

if __name__ == "__main__": main()
	

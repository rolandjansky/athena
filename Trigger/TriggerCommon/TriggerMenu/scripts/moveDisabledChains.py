#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
	This script reads the rulebook and swaps items between Physics and MC if they are disabled or not.
	TrigMenuRulebook needs to be checked out and installed.
	Currently it needs to be run from the script folder.
	One argument can be supplied specifying which "tag" to use, default is pp_v6 
"""

import importlib
import sys, re

tag = "pp_v6"
if len(sys.argv) > 1:
	tag = sys.argv[1]

def swapItems():
	physics_rulemod		 = importlib.import_module("TrigMenuRulebook.Physics_%s_rules" % tag)
	monitoring_rulemod = importlib.import_module("TrigMenuRulebook.Monitoring_%s_rules" % tag)
	standby_rulemod		 = importlib.import_module("TrigMenuRulebook.Standby_%s_rules" % tag)
	cosmic_rulemod		 = importlib.import_module("TrigMenuRulebook.Cosmic_%s_rules" % tag)
	monitoring_rulemod.rules = monitoring_rulemod.physics_rules
	modules = (physics_rulemod,monitoring_rulemod,standby_rulemod,cosmic_rulemod)
	
	dontmove = ['L1_BPH-7M15-2MU4_BPH-0DR24-2MU4', 'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4', 'L1_DR-EM15TAU12I']
	
	def isDisabled(item):
		hlt = "HLT_"+item
		disabled = True
		if item=="noalg_L12MU6": print item
		for mod in modules:
			if	 hlt	in mod.rules.keys(): key = hlt
			elif item in mod.rules.keys(): key = item
			else: continue
			if item=="noalg_L12MU6": print mod.rules[key]
			for rule in mod.rules[key].itervalues():
				if "PS" not in rule.keys() or rule["PS"] > -1:
						disabled = False
						break
			if not disabled: break
		return disabled
	
	lines_Physics = {}
	lines_MC = {}
	lines_Physics_fromMC = {}
	lines_MC_fromPhysics = {}
	count_toMC 			= 0
	count_toPhysics = 0
	items_MC_fromPhysics = set()
	
	current_Physics = "../python/menu/Physics_%s.py" % tag
	current_MC			 = "../python/menu/MC_%s.py" % tag
	slice_start_pattern = r"\s*TriggerFlags.(.*).signatures *=.*\[[^\]]"
	slice_end_pattern	 = "^\s*\]\s*$"
	item_pattern = """(?x)\['(.*)'\s*,	#chainName
												\s*'.*'\s*,	 #L1itemforchain
												\s*\[.*\]\s*, #[L1 items for chainParts]
												\s*\[.*\]\s*, #[stream]
												\s*\[.*\]\s*, #[groups]
												\s*.*\]*			#EBstep and ]"""
	ftk_pattern = "if TriggerFlags.doFTK()"
	last_slice_pattern = "TriggerFlags.(HeavyIon|MinBias|Beamspot)Slice.signatures"
	output_Physics = ""
	output_MC			= ""
	
	with open (current_Physics) as file_Physics:
		currentSlice = None
		linebuffer = ""
		lastslice = False
		for line in file_Physics:
			commented = line.strip().startswith("#")
			# Last slice pattern
			m = re.search(last_slice_pattern,line)
			if m and not commented: 
				lastslice = True
			if lastslice:
				output_Physics += line
				continue
			# Item pattern
			m = re.search(item_pattern,line)
			if m and not commented:
				assert(currentSlice != None)
				disabled = isDisabled(m.group(1))
				if disabled: 
					lines_MC_fromPhysics[currentSlice] += linebuffer+line
					items_MC_fromPhysics.add(m.group(1))
					count_toMC += 1
				else: 
					lines_Physics[currentSlice] += linebuffer+line
				linebuffer = ""
				continue
			# Slice start pattern
			m = re.search(slice_start_pattern,line)
			if m and not commented: 
				assert(currentSlice == None)
				currentSlice = m.group(1)
				output_Physics += "REPLACE_%s\n"%currentSlice
				if not currentSlice in lines_Physics.keys(): 				lines_Physics[currentSlice] = line
				if not currentSlice in lines_MC_fromPhysics.keys(): lines_MC_fromPhysics[currentSlice] = ""
				continue
			# Slice end pattern
			m = re.search(slice_end_pattern,line)
			if m and not commented: 
				if currentSlice!=None:
					lines_Physics[currentSlice] += linebuffer
					linebuffer = ""
					currentSlice = None
					continue
			# Store line in the proper place
			if currentSlice == None:
				output_Physics += line
			else:
				linebuffer += line
	
	with open (current_MC) as file_MC:
		currentSlice = None
		if_ftk = False
		linebuffer = ""
		lastslice = False
		for line in file_MC:
			commented = line.strip().startswith("#")
			# Last slice pattern
			m = re.search(last_slice_pattern,line)
			if m and not commented: 
				lastslice = True
			if lastslice:
				output_MC += line
				continue
			# Item pattern
			m = re.search(item_pattern,line)
			if m and not commented and not if_ftk: 
				assert(currentSlice != None)
				disabled = isDisabled(m.group(1))
				if disabled or any([vetoitem in line for vetoitem in dontmove]): 
					lines_MC[currentSlice] += linebuffer+line
				else: 
					lines_Physics_fromMC[currentSlice] += linebuffer+line
					count_toPhysics +=1
				linebuffer = ""
				continue
			# Slice start pattern
			m = re.search(slice_start_pattern,line)
			if m and not commented and not if_ftk: 
				assert(currentSlice == None)
				currentSlice = m.group(1)
				output_MC += "REPLACE_%s\n"%currentSlice
				if not currentSlice in lines_Physics_fromMC.keys(): lines_Physics_fromMC[currentSlice] = ""
				if not currentSlice in lines_MC.keys():							lines_MC[currentSlice] = line
				continue
			# Slice end pattern
			m = re.search(slice_end_pattern,line)
			if m and not commented: 
				if_ftk = False
				if currentSlice!=None:
					lines_MC[currentSlice] += linebuffer
					linebuffer = ""
					currentSlice = None
					continue
			# FTK pattern
			m = re.search(ftk_pattern,line)
			if m and not commented: 
				if_ftk = True
			# Store line in the proper place
			if currentSlice == None:
				output_MC += line
			else:
				linebuffer += line
	
	for key,val in lines_Physics.iteritems():
		toreplace	= val
		if key in lines_Physics_fromMC.keys():
			toreplace += lines_Physics_fromMC[key]
		toreplace += "			 ]\n"
		output_Physics = output_Physics.replace("REPLACE_%s\n"%key,toreplace)
	for key,val in lines_MC.iteritems():
		toreplace	= val
		if key in lines_MC_fromPhysics.keys():
			toreplace += lines_MC_fromPhysics[key]
		toreplace += "			 ]\n"
		output_MC = output_MC.replace("REPLACE_%s\n"%key,toreplace)
	
	
	with open (current_Physics+".edit","w") as outfile_Physics:
		outfile_Physics.write(output_Physics)
	with open (current_MC+".edit","w") as outfile_MC:
		outfile_MC.write(output_MC)
	
	print "Items moved MC -> Physics:",count_toPhysics
	print "Items moved Physics -> MC:",count_toMC
	return items_MC_fromPhysics

def cleanCPS(movedToMC):
	from TriggerMenu.menu.CPS import defineCPSGroups
	HLT_CPS_Groups = defineCPSGroups()
	cps_to_remove = set()
	for cps, items in HLT_CPS_Groups.iteritems():
		if sum(item in movedToMC for item in items) >= len(items)-1:
			cps_to_remove.add(cps)
	cpsfile = "../python/menu/CPS.py"
	cps_start_pattern = "'RATE:CPS:(.*)'\s*:\s*\["
	cps_end_pattern		= "\]\s*,"
	cps_item_pattern	= "^\s*'(.*)'\s*,\s*$"
	with open (cpsfile+".edit","w") as outcps, open (cpsfile) as incps:
		removing = False
		for line in incps:
			commented = line.strip().startswith("#")
			# CPS start pattern
			m = re.search(cps_start_pattern,line)
			if m and not commented:
				name = 'RATE:CPS:%s'%m.group(1)
				if name in cps_to_remove:
					removing = True
			#CPS item pattern
			if not m:
				m = re.search(cps_item_pattern,line)
				if m and not commented:
					if m.group(1) in movedToMC: continue
			if not removing:
				outcps.write(line)
			# CPS end pattern
			m = re.search(cps_end_pattern,line)
			if m and not commented:
				removing = False
			

if __name__ == "__main__":
	movedToMC = swapItems()
	cleanCPS(movedToMC)

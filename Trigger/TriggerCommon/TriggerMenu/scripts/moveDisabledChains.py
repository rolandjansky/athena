#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
	This script reads the rulebook and swaps items between Physics and MC if they are disabled or not.
	TrigMenuRulebook needs to be checked out and installed.
	Currently it needs to be run from the script folder.
	One argument can be supplied specifying which "tag" to use, default is pp_v7 
"""

import importlib
import sys, re, os

tag = "pp_v7"
if len(sys.argv) > 1:
	tag = sys.argv[1]

import checkTigherThanPrimary
tighter_than_primaries = set([x for x, y in checkTigherThanPrimary.main()])

keep = set()
if os.path.exists("keepChains.txt"):
	with open("keepChains.txt") as infile:
		for line in infile:
			if "------" in line: continue
			chain, dec = line.split("==>")
			chain = chain.strip()
			if "OK, move" in dec: dec = True
			elif "No, keep" in dec: dec = False
			else: print "WTF:",dec
			if dec==False: keep.add(chain)
	print "len(keep)",len(keep)

def swapItems():
	physics_rulemod		 		= importlib.import_module("TrigMenuRulebook.Physics_%s_rules" % tag)
	monitoring_rulemod 		= importlib.import_module("TrigMenuRulebook.Monitoring_%s_rules" % tag)
	standby_rulemod		 		= importlib.import_module("TrigMenuRulebook.Standby_%s_rules" % tag)
	cosmic_rulemod		 		= importlib.import_module("TrigMenuRulebook.Cosmic_%s_rules" % tag)
	toroidoff_rulemod	 		= importlib.import_module("TrigMenuRulebook.Physics_%s_ToroidOff_rules" % tag)
#	startup_rulemod		 		= importlib.import_module("TrigMenuRulebook.Physics_%s_startup_rules" % tag)
	monitoring_rulemod.rules = monitoring_rulemod.physics_rules
#	modules = (physics_rulemod,monitoring_rulemod,standby_rulemod,cosmic_rulemod,toroidoff_rulemod,commissioning_rulemod,startup_rulemod)
#	modules = (physics_rulemod,monitoring_rulemod,standby_rulemod,cosmic_rulemod,toroidoff_rulemod,commissioning_rulemod)
	modules = (physics_rulemod,monitoring_rulemod,standby_rulemod,cosmic_rulemod,toroidoff_rulemod)

	l1topo_pattern = "\w-\w"
	
	def getPS(item):
		hlt = "HLT_"+item
		ps = -1
		for mod in modules:
			if	 hlt	in mod.rules.keys(): key = hlt
			elif item in mod.rules.keys(): key = item
			else: continue
			for rule in mod.rules[key].itervalues():
				if "PS" not in rule.keys():
					if "rate" in rule.keys(): return 2
					else: ps = 1
				if rule["PS"] > ps:
					ps = rule["PS"]
					if ps > 1: return ps
		return ps
	
	lines_Physics = {}
	lines_MC = {}
	lines_Physics_fromMC = {}
	lines_MC_fromPhysics = {}
	count_toMC 			= 0
	count_toPhysics = 0
	items_MC_fromPhysics = set()
	items_MC_fromPhysics_dict = {}
	items_Physics_fromMC_withPS = set()
	items_Physics_withPS = set()
	
	current_Physics = "../python/menu/Physics_%s.py" % tag
	current_MC			 = "../python/menu/MC_%s.py" % tag
#	slice_start_pattern = r"\s*TriggerFlags.(\w*).signatures *=.*"
#	slice_start_pattern = r"\s*TriggerFlags.(.*).signatures *=.*\[[^\]]"
	slice_start_pattern = r"\s*TriggerFlags.(.*).signatures *=.*\[(?!\])"
	slice_end_pattern	 = "^\s*\]\s*$"
	item_pattern = """(?x)\['(.*)'\s*,	#chainName
												\s*'.*'\s*,	 #L1itemforchain
												\s*\[.*\]\s*, #[L1 items for chainParts]
												\s*\[.*\]\s*, #[stream]
												\s*\[.*\]\s*, #[groups]
												\s*.*\]*			#EBstep and ]"""
	ftk_pattern = "if TriggerFlags.doFTK()"
	ignore_slice_pattern = "TriggerFlags.(AFP)Slice.signatures"
	last_slice_pattern = "TriggerFlags.(HeavyIon|MinBias|Beamspot)Slice.signatures"
	output_Physics = ""
	output_MC			= ""
	
	with open (current_Physics) as file_Physics:
		currentSlice = None
		if_ftk = False
		ignoreSlice = False
		linebuffer = ""
		lastslice = False
		for line in file_Physics:
			cleanline = line.strip()
			if "#" in line: cleanline = line[:line.find("#")].strip()
			commented = line.strip().startswith("#")
			# Last slice pattern
			m = re.search(last_slice_pattern,cleanline)
			if m and not commented: 
				lastslice = True
			if lastslice:
				output_Physics += line
				continue
			# Item pattern
			m = re.search(item_pattern,cleanline)
			if m and not commented and not ignoreSlice and not if_ftk:
				assert currentSlice != None, (currentSlice, cleanline)
				name = m.group(1)
				ps = getPS(name)
				if name in tighter_than_primaries:
					print "Found item that is tighter than primary, instead of moving to MC please enable the rule in RB:", name
				if name not in keep and ps==-1 and not name in tighter_than_primaries and not re.search(l1topo_pattern,cleanline):
					lines_MC_fromPhysics[currentSlice] += linebuffer+line
					items_MC_fromPhysics.add(name)
					items_MC_fromPhysics_dict[currentSlice].append(name)
					count_toMC += 1
				else: 
					lines_Physics[currentSlice] += linebuffer+line
				linebuffer = ""
				if ps>1:
					items_Physics_withPS.add(name)
				continue
			# Slice start pattern
			m = re.search(slice_start_pattern,cleanline)
			m_ignore = re.search(ignore_slice_pattern,cleanline)
			if m and not commented and not if_ftk: 
				#print "Start slice:",line
				assert currentSlice == None, (currentSlice, cleanline)
				if m_ignore:
					ignoreSlice = True
					output_Physics += line
					continue
				currentSlice = m.group(1)
				output_Physics += "REPLACE_%s\n"%currentSlice
				if not currentSlice in lines_Physics.keys(): 				lines_Physics[currentSlice] = line
				if not currentSlice in lines_MC_fromPhysics.keys(): lines_MC_fromPhysics[currentSlice] = ""; items_MC_fromPhysics_dict[currentSlice] = []
				continue
			# Slice end pattern
			m = re.search(slice_end_pattern,cleanline)
			if m and not commented: 
				ignoreSlice = False
				if_ftk = False
				if currentSlice!=None:
					lines_Physics[currentSlice] += linebuffer
					linebuffer = ""
					currentSlice = None
					continue
			# FTK pattern
			m = re.search(ftk_pattern,cleanline)
			if m and not commented: 
				if_ftk = True
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
			cleanline = line.strip()
			if "#" in line: cleanline = line[:line.find("#")].strip()
			commented = line.strip().startswith("#")
			# Last slice pattern
			m = re.search(last_slice_pattern,cleanline)
			if m and not commented: 
				lastslice = True
			if lastslice:
				output_MC += line
				continue
			# Item pattern
			m = re.search(item_pattern,cleanline)
			if m and not commented and not ignoreSlice and not if_ftk: 
				assert currentSlice != None, (currentSlice, cleanline)
				ps = getPS(m.group(1))
				if ps==-1 or re.search(l1topo_pattern,cleanline):
					lines_MC[currentSlice] += linebuffer+line
				else: 
					lines_Physics_fromMC[currentSlice] += linebuffer+line
					if ps>1:
						items_Physics_fromMC_withPS.add(m.group(1))
					count_toPhysics +=1
				linebuffer = ""
				continue
			# Slice start pattern
			m = re.search(slice_start_pattern,cleanline)
			m_ignore = re.search(ignore_slice_pattern,cleanline)
			if m and not commented and not if_ftk: 
				assert currentSlice == None, (currentSlice, cleanline)
				if m_ignore:
					ignoreSlice = True
					output_MC += line
					continue
				currentSlice = m.group(1)
				output_MC += "REPLACE_%s\n"%currentSlice
				if not currentSlice in lines_Physics_fromMC.keys(): lines_Physics_fromMC[currentSlice] = ""
				if not currentSlice in lines_MC.keys():							lines_MC[currentSlice] = line
				continue
			# Slice end pattern
			m = re.search(slice_end_pattern,cleanline)
			if m and not commented: 
				if_ftk = False
				if currentSlice!=None:
					lines_MC[currentSlice] += linebuffer
					linebuffer = ""
					currentSlice = None
					continue
			# FTK pattern
			m = re.search(ftk_pattern,cleanline)
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
	
	print "Items moved Physics -> MC:",count_toMC
	print "Items moved MC -> Physics:",count_toPhysics
	if items_Physics_fromMC_withPS:
		print "Some new items in Physics are prescaled, you probably want to add them to CPS.py:"
		print sorted(list(items_Physics_fromMC_withPS))

	with open("disabled.txt","w") as outfile:
		for mslice,items in items_MC_fromPhysics_dict.iteritems():
			outfile.write(mslice+" ---------\n")
			for item in items:
				outfile.write("\t"+item+"\n")
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
			cleanline = line.strip()
			if "#" in line: cleanline = line[:line.find("#")].strip()
			commented = line.strip().startswith("#")
			# CPS start pattern
			m = re.search(cps_start_pattern,cleanline)
			if m and not commented:
				name = 'RATE:CPS:%s'%m.group(1)
				if name in cps_to_remove:
					removing = True
			#CPS item pattern
			if not m:
				m = re.search(cps_item_pattern,cleanline)
				if m and not commented:
					if m.group(1) in movedToMC: continue
			if not removing:
				outcps.write(line)
			# CPS end pattern
			m = re.search(cps_end_pattern,cleanline)
			if m and not commented:
				removing = False
			

if __name__ == "__main__":
	movedToMC = swapItems()
	cleanCPS(movedToMC)

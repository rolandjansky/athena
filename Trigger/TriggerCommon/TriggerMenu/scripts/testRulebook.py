#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, commands
import TriggerClasses
import logging
from operator import attrgetter
import xml.etree.cElementTree as ET

def checkFormat(file_path, trigger_rules = {}):
    """Read a rulebook, checking that each line has the appropriate
    formatting along the way
    """
    badlines = []
    duplicates = []

    for line in open(file_path, 'r'):
        if line.count('#') or not line.count(';'):
            continue

        rule = TriggerClasses.TriggerRule()

        if not rule.ReadLine(line):
            badlines.append(line.rstrip('\n'))
            continue
        
        if rule.GetTriggerName() in trigger_rules:
            duplicates.append(rule.GetTriggerName())

        trigger_rules[rule.GetTriggerName()] = rule


    if len(badlines) != 0 or len(duplicates) != 0:
        logging.error("========================================================")
        logging.error("Failed to read %s lines: \n" % len(badlines))
        for line in badlines:
            logging.error("%s\n" % line.strip())
        logging.error("Found %s duplicate entries: \n" % len(duplicates))
        for dup in duplicates:
            logging.error("%s\n" % dup.strip())
            
        raise NameError("checkFormat failed")


def checkMenu(trigger_rules, lvl1_file, hlt_file):
    """Compare a list of trigger rules against the trigger menu,
    checking that each has a corresponding entry in the other
    """
    rules_list = trigger_rules.keys()
    menu_list = []
    
    lvl1_info = ET.parse(lvl1_file).getroot()
    for item in lvl1_info.findall('TriggerMenu/TriggerItem'):
        name = item.get('name')
        if name in rules_list:
            rules_list.remove(name)
        else:
            menu_list.append(name)
        
    hlt_info = ET.parse(hlt_file).getroot()
    for item in hlt_info.findall('CHAIN_LIST/CHAIN'):
        name = item.get('chain_name')
        if name in rules_list:
            rules_list.remove(name)
        else:
            menu_list.append(name)
            
    all_good = True
    if len(rules_list) > 0 or len(menu_list) > 0:
        logging.error("========================================================")
        logging.error("Rulebook items without a matching menu: %s" % rules_list)
        logging.error("Menu items without a matching rule: %s" % menu_list)
        all_good = False
    
    if not all_good:
        raise NameError("checkMenu failed")

def buildChains(trigger_rules, lvl1_file, hlt_file, chains = []):
    """Breadth-first search of the chains
    """
    
    lvl1_info = ET.parse(lvl1_file).getroot()
    hlt_info = ET.parse(hlt_file).getroot()

    l2_items = {}
    ef_items = {}

    for hlt_item in hlt_info.findall('CHAIN_LIST/CHAIN'):
        chain_name = hlt_item.get('chain_name')
        lower_chain_name = hlt_item.get('lower_chain_name')

        if chain_name[0:2] == 'L2':
            group = l2_items.get(lower_chain_name, [])
            group.append(chain_name)
            l2_items[lower_chain_name] = group
        elif chain_name[0:2] == 'EF':
            group = ef_items.get(lower_chain_name, [])
            group.append(chain_name)
            ef_items[lower_chain_name] = group

    for l1_item in lvl1_info.findall('TriggerMenu/TriggerItem'):
        l1_name = l1_item.get('name')
        l2_group = l2_items.get(l1_name, [])
        if len(l2_group) == 0:
            myChain = TriggerClasses.Chain()
            myChain.buildChainFromNames(l1_name)
            myChain.buildRules(trigger_rules)
            chains.append(myChain)
            continue

        for l2_name in l2_group:
            ef_group = ef_items.get(l2_name, [])
            if len(ef_group) == 0:
                myChain = TriggerClasses.Chain()
                myChain.buildChainFromNames(l1_name, l2_name)
                myChain.buildRules(trigger_rules)
                chains.append(myChain)
                continue

            for ef_name in ef_group:
                myChain = TriggerClasses.Chain()
                myChain.buildChainFromNames(l1_name, l2_name, ef_name)
                myChain.buildRules(trigger_rules)
                chains.append(myChain)
            
            
        
def checkChainEvolution(chain):
    PS = chain.getPS()
    
    Lumi = PS["Lumi"]
    L1PS = PS["L1PS"]
    L2PS = PS["L2PS"]
    EFPS = PS["EFPS"]
    totalPS = PS["TotalPS"]
    L1PT = PS["L1PT"]
    L2PT = PS["L2PT"]
    EFPT = PS["EFPT"]
    L1slope = PS["L1hasSlope"]
    L2slope = PS["L2hasSlope"]
    EFslope = PS["EFhasSlope"]

    err = []
    for i in range(2,len(Lumi), 2):

        if totalPS[i+1] < totalPS[i] and totalPS[i+1] > 0:
            err.append('Total prescale decreased from %s to %s at %s (%s*%s*%s --> %s*%s*%s)' % (totalPS[i], totalPS[i+1], Lumi[i],
                                                                                                 L1PS[i], L2PS[i], EFPS[i],
                                                                                                 L1PS[i+1], L2PS[i+1], EFPS[i+1])
                       )

        if totalPS[i] <= -1 and totalPS[i+1] > 0:
            err.append("Disabled chain was re-enabled at %s (%s*%s*%s --> %s*%s*%s)" % (Lumi[i],
                                                                                        L1PS[i], L2PS[i], EFPS[i],
                                                                                        L1PS[i+1], L2PS[i+1], EFPS[i+1])
                       )
          
        if i!= 0 \
        and L1PS[i] == L1PS[i+1] and L2PS[i] == L2PS[i+1] and EFPS[i] == EFPS[i+1] \
        and L1PT[i] == L1PT[i+1] and L2PT[i] == L2PT[i+1] and EFPT[i] == EFPT[i+1] \
        and L1slope[i] == L1slope[i+1] and L2slope[i] == L2slope[i+1] and EFslope[i] == EFslope[i+1] :
            err.append("Redundant entry at %s (%s*%s*%s --> %s*%s*%s)" % (Lumi[i],
                                                                          L1PS[i], L2PS[i], EFPS[i],
                                                                          L1PS[i+1], L2PS[i+1], EFPS[i+1])
                       )


        if totalPS[i+1] > 1 and int(L1slope[i+1] + L2slope[i+1] + EFslope[i+1]) == 0:
            err.append("Chain's PS (%s) has no slope at %s (%s, %s, %s)" % (totalPS[i+1], Lumi[i], L1slope[i+1], L2slope[i+1], EFslope[i+1]))

        if totalPS[i+1] > 0 and \
           L1slope[i+1] + L2slope[i+1] + EFslope[i+1] > 1:
            err.append("Chain's PS (%s) has multiple slopes defined at %s (%s, %s, %s)" % (totalPS[i+1], Lumi[i], L1slope[i+1], L2slope[i+1], EFslope[i+1]))

    if len(err) > 0:

        logging.error("========================================================")
        logging.error("Chain = %s -> %s -> %s" % (PS["L1Name"], PS["L2Name"], PS["EFName"]))
        for e in err:
            logging.error(e)

        raise NameError("checkEvolution failed")

def checkOptimization(chains):
    """Group chain by L1 seed, and then by L2 seed, for each group and
    sub-group, check that we cannot move a PS to a lower level. An
    item is considered optimized if one or more of its daughters have
    a PS = 1
    """

    all_good = True

    orphan_groups = []
    l1_groups = {}
    l1_lumi = {}
    l2_groups = {}
    l2_lumi = {}
    
    # Breadth-first search to make L1 and L2 groups
    # Optimization will be tested at every L1/L2/EF breakpoint, so
    # merge the lists of luminosity points
    for chain in chains:

        # Hard to optimize something which seeds nothing
        if chain.L2Rule == None or chain.L1Rule == None:
            orphan_groups.append(chain)
            continue

        tmp_group = l1_groups.get(chain.L1Name, [])
        tmp_group.append(chain)
        l1_groups[chain.L1Name] = tmp_group
        tmp_lumi = l1_lumi.get(chain.L1Name, set())
        tmp_lumi = tmp_lumi.union(chain.Lumi)
        l1_lumi[chain.L1Name]   = tmp_lumi

        # Not all rules will have an EF item
        if chain.EFRule == None:
            continue
        
        tmp_group = l2_groups.get(chain.L2Name, [])
        tmp_group.append(chain)
        l2_groups[chain.L2Name] = tmp_group
        tmp_lumi = l2_lumi.get(chain.L2Name, set())
        tmp_lumi = tmp_lumi.union(chain.Lumi)
        l2_lumi[chain.L2Name]   = tmp_lumi


    # Check the L1->L2 optimization
    for (key, chains) in l1_groups.items():
        if len(l1_lumi[key]) == 0:
            logging.warning( "no lumi points for %s" % key)
            continue
        
        bad_lumi = []
        
        for lumi in sorted(l1_lumi[key]):
            all_prescaled = True
            for chain in chains:
                point = chain.L1Rule.FindLumiPoint(lumi)
                l1_ps = point.ComputePS(lumi)
                if l1_ps <= 0:
                    all_prescaled = False
                    continue
                
                point = chain.L2Rule.FindLumiPoint(lumi)
                l2_ps = point.ComputePS(lumi)
                logging.debug("%s @ %s = %s * %s" % (chain.L2Name, lumi, l1_ps, l2_ps))
                if l2_ps == 1:
                    all_prescaled = False

            if all_prescaled:
                bad_lumi.append(lumi)

        if len(bad_lumi) > 0:
            all_good = False
            logging.error("========================================================")
            logging.error( "Item %s -> %s not optimized at L1 for %s " % (key, [x.L2Name for x in chains], sorted(bad_lumi))) 

    # Check the L2->EF optimization
    for (key, chains) in l2_groups.items():
        if len(l2_lumi[key]) == 0:
            logging.warning( "no lumi points for %s" % key)
            continue

        bad_lumi = []

        for lumi in sorted(l2_lumi[key]):
            
            all_prescaled = True
            
            for chain in chains:
                point = chain.L1Rule.FindLumiPoint(lumi)
                l1_ps = point.ComputePS(lumi)
                if l1_ps <= 0:
                    all_prescaled = False
                    continue
                
                point = chain.L2Rule.FindLumiPoint(lumi)
                l2_ps = point.ComputePS(lumi)
                if l2_ps <= 0:
                    all_prescaled = False
                    continue
                
                point = chain.EFRule.FindLumiPoint(lumi)
                ef_ps = point.ComputePS(lumi)
                if ef_ps == 1:
                    all_prescaled = False
                
            if all_prescaled:
                bad_lumi.append(lumi)

        if len(bad_lumi) > 0:
            all_good = False
            logging.error("========================================================")
            logging.error( "Item %s -> %s not optimized at L2 for %s" % (key, [x.EFName for x in chains], sorted(bad_lumi)))

    if not all_good:
        raise NameError("checkOptimization failed")

        
                
if __name__ == "__main__":


    AtlasArea = os.environ.get("AtlasArea")
    L1_XML  = commands.getoutput("ls %s/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_Physics_pp_v1_*xml" % AtlasArea)
    HLT_XML = commands.getoutput("ls %s/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_Physics_pp_v1_*xml" % AtlasArea)
    
    from optparse import OptionParser
    p = OptionParser("usage: %prog [options] rulebook [chain]")
    
    p.add_option('--hlt'             , type = "str"         , default=L1_XML  , dest="hlt"          , help="location of the HLT menu - default [%default]")
    p.add_option('--lvl1'            , type = "str"         , default=HLT_XML , dest="lvl1"         , help="location of the L1 menu - default [%default]")
    p.add_option('--minLumi'         , type = "int"         , default=1000    , dest="minL"         , help="Force a minimum luminosity - default [%default]")
    p.add_option('--maxLumi'         , type = "int"         , default=100000  , dest="maxL"         , help="Force a maximum luminosity - default [%default]")
    p.add_option('--no-consistency'  , action="store_false" , default=True    , dest="do_consist"   , help="Disable the rulebook to menu consistency check")
    p.add_option('--no-evolution'    , action="store_false" , default=True    , dest="do_evolution" , help="Disable the rulebook evolution check")
    p.add_option('--no-optimization' , action="store_false" , default=True    , dest="do_opt"       , help="Disable the rulebook optimization check")
    p.add_option('--debug'           , action="store_true"  , default=False   , dest="debug"        , help="Enable debug level logging")

    (options, args) = p.parse_args()

    if options.debug:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)        

    ## Change the way we do this, start from L1, look for all parents, and call a "top-level chain" anything which caps the chain
    ## Depth first search? -- find a good algorithm for this

    if len(args) < 1:
        print p.usage()

    rules = args[0]
    
    logging.info( "Running with:")
    if len(args) > 1:
        logging.info( "Chain name         : %s" % args[1:] )
    logging.info( "HLT XML            : %s" % options.hlt )
    logging.info( "LVL1 XML           : %s" % options.lvl1 )
    logging.info( "Rulebook           : %s" % rules )
    logging.info( "Check consistency  : %s" % options.do_consist )
    logging.info( "Check evolution    : %s" % options.do_evolution )
    logging.info( "Check optimization : %s" % options.do_opt )

    check_results = True

    # Load the trigger rules and check the Rulebook format
    trigger_rules = {}
    try:
        checkFormat(rules, trigger_rules)
    except:
        check_results = False

    # check consistency btw the rulebook, lvl1, and hlt menus
    if options.do_consist:
        try:
            checkMenu(trigger_rules, options.lvl1, options.hlt)
        except:
            check_results = False

    # build the chains from the bottom up
    chains = []
    buildChains(trigger_rules, options.lvl1, options.hlt, chains)

    # sort the chains by L1 name, then L2 name
    chains = sorted(chains, key=attrgetter('L1Name', 'L2Name', 'EFName'))

    # check the evolution of each chain for redundancies and inconsistencies
    if options.do_evolution:
        for chain in chains:
            if len(args) > 1:
                if not (chain.L1Name in args or chain.L2Name in args or chain.EFName in args):
                    continue
                
            try:
                checkChainEvolution(chain)
            except:
                check_results = False

    if options.do_opt:
        try:
            checkOptimization(chains)
        except:
            check_results = False

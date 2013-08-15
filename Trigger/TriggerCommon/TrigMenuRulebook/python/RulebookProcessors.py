# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


"""Classes for the reading of rules and the writing of XMLs
"""

import logging
from RuleTools import read_online_rates, build_tree, calc_eff, round_figures, create_logger, find_file_in_env
import math, sys
from disabledOK import can_be_disabled
import xml.etree.cElementTree as ET

maxPS = 1.62E7

class RuleWriter:
    """Class to produce an XML with the tree's PS values
    """
    def __init__(self, node_list, metadata, xml_file):
        
        self.xml_file  = open(xml_file,'w')
        self.node_list = node_list
        self.metadata  = metadata
	
    def write_xml(self):
        """Iterate over tree and write output XML
        """
        output_text  = '<?xml version="1.0" encoding="ISO-8859-1"?>\n'
        output_text += '<?xml-stylesheet type="text/xsl" href="trigger_rates.xsl"?>\n'
        output_text += '<trigger>\n'
        output_text += '<PredictionLumi>%.2e</PredictionLumi>\n' % (self.metadata["lumi"] * 1e30)
        output_text += '<bunchgroup>\n'
        for key, value in self.metadata.iteritems():
            if key == 'lumi':
                continue
            output_text += '  <bunchgrouptype>\n'
            output_text += '    <bunchgroup_keynum>%s</bunchgroup_keynum>\n' % key
            output_text += '    <bunchgroup_key>BGRP%s</bunchgroup_key>\n'   % key
            output_text += '    <bunchgroup_size>%s</bunchgroup_size>\n'     % value
            output_text += '  </bunchgrouptype>\n'
        output_text += '</bunchgroup>\n' 
        output_text += '  <level>\n'
        
        output_text_l1 = ""
        output_text_l2 = ""
        output_text_ef = ""

        for node in self.node_list:

            if node.name.startswith('L1'):
                if not '<lvl_name>L1</lvl_name>' in output_text_l1:
                    output_text_l1 += '    <lvl_name>L1</lvl_name>\n'
                output_text_l1 += '    <signature>\n'
                output_text_l1 += '      <sig_name>%s</sig_name>\n' % node.name
                output_text_l1 += '      <chain_prescale>%3.1f</chain_prescale>\n' % node.ps
                output_text_l1 += '    </signature>\n'
            
            if node.name.startswith('L2'):
                if not '<lvl_name>L2</lvl_name>' in output_text_l2:
                    output_text_l2 += '  </level>\n'
                    output_text_l2 += '  <level>\n'
                    output_text_l2 += '    <lvl_name>L2</lvl_name>\n'
                output_text_l2 += '    <signature>\n'
                output_text_l2 += '      <sig_name>%s</sig_name>\n' % node.name
                output_text_l2 += '      <chain_prescale>%3.1f</chain_prescale>\n' % node.ps
                output_text_l2 += '      <passthrough>%d</passthrough>\n' % int(node.pt)
                output_text_l2 += '    </signature>\n'

            if node.name.startswith('EF'):
                if not '<lvl_name>EF</lvl_name>' in output_text_ef:
                    output_text_ef += '  </level>\n'
                    output_text_ef += '  <level>\n'
                    output_text_ef += '    <lvl_name>EF</lvl_name>\n'
                output_text_ef += '    <signature>\n'
                output_text_ef += '      <sig_name>%s</sig_name>\n' % node.name
                output_text_ef += '      <chain_prescale>%3.1f</chain_prescale>\n' % node.ps
                output_text_ef += '      <passthrough>%d</passthrough>\n' % int(node.pt)
                output_text_ef += '      <express_prescale>%3.1f</express_prescale>\n' % node.es
                output_text_ef += '    </signature>\n'
               
        output_text += output_text_l1
        output_text += output_text_l2
        output_text += output_text_ef
        output_text += '  </level>\n'
        output_text += '</trigger>\n'
            
        self.xml_file.writelines(output_text)

class RuleReader:
    """Encapsulate the processing of a rulebook and calculation of
    prescales
    """

    def special_cases(self):
        """Handle hacks for special cases here"""
        
        # L1 random trigger rates do not show up correctly in the
        # rates file. Calculate their rates here.

        bunch_rate = 5e6 / 3554.
        self.l1_items["L1_RD0_FILLED"].signature["rd_rate"] = self.config["online_filled"] * bunch_rate
        self.l1_items["L1_RD1_FILLED"].signature["rd_rate"] = self.config["online_filled"] * bunch_rate 
        self.l1_items["L1_RD0_EMPTY"].signature["rd_rate"] = self.config["online_empty"] * bunch_rate
        self.l1_items["L1_RD1_EMPTY"].signature["rd_rate"] = self.config["online_empty"] * bunch_rate
        self.l1_items["L1_RD0_UNPAIRED_ISO"].signature["rd_rate"] = self.config["online_unpaired_iso"] * bunch_rate
        self.l1_items["L1_RD0_FIRSTEMPTY"].signature["rd_rate"] = self.config["online_empty_after_filled"] * bunch_rate
        
        pass

    def __init__(self, config, rules, logger = None):

        self.config = config

        if logger == None:
            self.log = create_logger("RuleReader", config["verbosity"])
        else:
            self.log = logger
            
        self.log.info('Verbosity level set to %s' % logging.getLevelName(self.log.level))

        self.online_rates = read_online_rates(ET.parse(config["online_xml"]).getroot(), 
                                               config["online_lumi"])

        self.l1_items = build_tree(ET.parse(config["l1_xml"]).getroot(),
                                   ET.parse(config["hlt_xml"]).getroot(),
                                   rules,
                                   self.online_rates,
                                   self.config["target_lumi"],
                                   self.config["use_lowest_rule"]
                                   )

        ## After getting the rules we want to allow the user to modify
        ## the target luminosity
        self.config["target_lumi"] = int(self.config["target_lumi"] * self.config["scale_lumi"])

        self.all_items = {}
        self.dependent_items = {}

        for l1_node in self.l1_items.values():
            self.all_items[l1_node.name] = l1_node
            if l1_node.rule != None and "depends_on" in l1_node.rule:
                self.dependent_items[l1_node.name] = l1_node

            for l2_node in l1_node.daughters:
                self.all_items[l2_node.name] = l2_node
                if l2_node.rule != None and "depends_on" in l2_node.rule:
                    self.dependent_items[l2_node.name] = l2_node

                for ef_node in l2_node.daughters:
                    self.all_items[ef_node.name] = ef_node
                    if ef_node.rule != None and "depends_on" in ef_node.rule:
                        self.dependent_items[ef_node.name] = ef_node
                        
        self.special_cases()
        self.check_for_missing_triggers(rules)


        self.nsig = 3

    def check_for_missing_triggers(self, rules):
        """Iterate over the list of rules, notify the user about any
        which are not matched by an item in the menu
        """
        
        for trigger in rules.keys() :
            disabled = True
            for r in rules[trigger] :
                if "PS" not in rules[trigger][r].keys() or rules[trigger][r]['PS'] > 0 :
                    disabled = False
            if disabled :
                continue 
            if not trigger in self.all_items.keys() :
                self.log.error("No trigger exist in menu for rule '%s'" % str(trigger))

                         
    def calc_sf(self, trigger_node):
        """Calculate a scaling factor for the trigger rate taking into
        account bunch structure if necessary"""

        if (trigger_node.rule != None \
            and "scaling" in trigger_node.rule \
            and trigger_node.rule["scaling"] == "bunches"
            ) or ("L1_RD" == trigger_node.name[:5]):

            if self.config["disable_bunch_scaling"]:
                self.log.debug("Bunch scaling is disabled, returning scale factor of 1 for %s" % trigger_node.name)
                return 1
            
            if trigger_node.is_unpaired_iso():
                online_bunches = self.config["online_unpaired_iso"]
                target_bunches = self.config["target_unpaired_iso"]

            elif trigger_node.is_unpaired_noniso():
                online_bunches = self.config["online_unpaired_noniso"]
                target_bunches = self.config["target_unpaired_noniso"]

            elif trigger_node.is_unpaired():
                online_bunches = self.config["online_unpaired_iso"] + self.config["online_unpaired_noniso"]
                target_bunches = self.config["target_unpaired_iso"] + self.config["target_unpaired_noniso"]  

            elif trigger_node.is_firstempty():
                online_bunches = self.config["online_empty_after_filled"]
                target_bunches = self.config["target_empty_after_filled"]
                
            elif trigger_node.is_empty():
                online_bunches = self.config["online_empty"]
                target_bunches = self.config["target_empty"]  

            else:
                online_bunches = self.config["online_filled"]
                target_bunches = self.config["target_filled"]
                
            self.log.debug("Applying a scaling for %s from %s to %s!" % (trigger_node.name, online_bunches, target_bunches))

            try:
                #print target_bunches
                #print online_bunches
                return target_bunches / online_bunches
            except ZeroDivisionError:
                self.log.error("Online bunch structure is unknown!")
                return -1
            except TypeError:
                self.log.error("You are doing something really weird!")
                return -1            

        else:
            try:
                return self.config["target_lumi"] / float(self.config["online_lumi"])
            except ZeroDivisionError:
                return 1.0

    def check_depends(self, dependencies):
        for item_name in dependencies.split(","):
            try:
                if self.all_items[item_name].ps != 1:
                    return True
            except KeyError:
                sys.exit("\n*** Check dependency rule for %s; item doesn't exist\nExit\n " %item_name)
                
        return False
        

    def apply_rule(self, trigger_node, input_rate, node_eff, total_ps):
        """Interpret the rule and apply it to the given trigger item
        """

        prescale = 1

        if trigger_node.is_multiseed() and "rate" in trigger_node.rule:
            raise ValueError("Multiseeded items must be defined with a PS: %s" % trigger_node)

        if "PS" in trigger_node.rule or "PS_slope" in trigger_node.rule:
            prescale = float(trigger_node.rule.get("PS", 0))

            if "PS_slope" in trigger_node.rule:
                prescale += trigger_node.rule["PS_slope"] * \
                    self.config["target_lumi"] / trigger_node.rule["lumi"]

            if "depends_on" in trigger_node.rule:
                if not self.check_depends(trigger_node.rule["depends_on"]):
                    prescale = 1

            if prescale > 0 and prescale < 1:
                raise ValueError("PS rule for %s defines a fractional prescale: %s" % (trigger_node.name, prescale))

        elif "rate" in trigger_node.rule or "inputRate" in trigger_node.rule:

            if "inputRate" in trigger_node.rule:
                rate = input_rate
                target_rate = trigger_node.rule["inputRate"]
                target_max_rate = trigger_node.rule.get("inputMaxRate", target_rate)
                
            else:
                rate = input_rate * node_eff
                target_rate = trigger_node.rule["rate"]
                target_max_rate = trigger_node.rule.get("maxRate", target_rate)

            if rate > target_max_rate:
                try:
                    prescale = rate / target_rate
                except ZeroDivisionError:
                    prescale = -1
            elif "depends_on" in trigger_node.rule:
                if self.check_depends(trigger_node.rule["depends_on"]):
                    try:
                        prescale = rate / target_rate
                    except ZeroDivisionError:
                        prescale = -1

        elif "fraction" in trigger_node.rule:
               
            if len(trigger_node.rule["depends_on"].split(",")) != 1:
                raise ValueError("Don't know how to handle a fraction rule with multiple dependencies: %s" % trigger_node)

            rate = input_rate * node_eff

            try:
                dependency = self.all_items[trigger_node.rule["depends_on"]]
            except KeyError:
                self.log.error("%s depends on an unknown trigger (%s) and will be disabled" % (trigger_node.name, trigger_node.rule["depends_on"]))
                target_rate = 0
            else:
                target_rate = trigger_node.rule["fraction"] * dependency.rate if dependency.ps > 0 else 0.
                
            target_max_rate = trigger_node.rule.get("maxRate", target_rate)

            if float(rate) > float(target_max_rate):
                try:
                    prescale = rate / target_rate
                except ZeroDivisionError:
                    prescale = -1

            elif dependency.ps != 1:
                try:
                    prescale = rate / target_rate
                except ZeroDivisionError:
                    prescale = -1                 

        if trigger_node.rule.get("rerun", False) and prescale == -1:
            prescale = 0
            
        #Only keep 3 significant figures
        output = round_figures(prescale, self.nsig) 
        output = round_figures(prescale, self.nsig)
        if trigger_node.name[0:3] == "L1_":
            output = math.floor(output)
        if output > maxPS:
            output = round_figures(maxPS-1, self.nsig)
        return output
        
    def check_dependencies(self):
        """Check for circular dependencies
        """

        dependencies = []
        for item in self.dependent_items:
            dependencies.extend(item.rule["depends_on"].split(","))

        overlaps = set(dependencies) & set(self.dependent_items.keys())
        if len(overlaps) > 0:
            raise ValueError("Recursive dependencies with %s" % overlaps)
        

    def set_defaults(self, nodes = None, def_val = None):
        """ Set default prescale to -1 (or user value) if there is a
        chain that has no rule
        """

        rate_keys = set(["rate", "inputRate", "PS", "PS_slope", "fraction"])

        #If no nodes were specified by the user, process over the full set
        if nodes == None:
            nodes = self.l1_items.values()

        #If the user didn't spcify a default value, use -1
        if def_val == None:
            def_val = -1

        for l1_node in nodes:

            l1_rules = False
            if l1_node.rule != None and rate_keys & set(l1_node.rule.keys()):
                l1_rules = True

            l1_daughter_rules = False
            for l2_node in l1_node.daughters:
                
                l2_rules = False
                #Tell L1 a rule's been found, no need to look at the daughters
                if l2_node.rule != None  and rate_keys & set(l2_node.rule.keys()):
                    l1_daughter_rules = True
                    l2_rules = True
                
                l2_daughter_rules = False
                for ef_node in l2_node.daughters:

                    #Tell L1 and L2 a rule's been found
                    if ef_node.rule != None and rate_keys & set(ef_node.rule.keys()):
                        l2_daughter_rules = True
                        l1_daughter_rules = True
                
                    if ef_node.rule == None:
                        if not ef_node.name in can_be_disabled :
                            self.log.error('Trigger chain %s --> %s --> %s has no rules and will be disabled' % (l1_node.name, l2_node.name, ef_node.name))
                        ef_node.ps = def_val
                
                #We should only get here if the L1 and L2 has no rule, check daughter rules and set default value if necessary
                if l2_daughter_rules == False and l2_rules == False:
                    if len(l2_node.daughters) == 0:
                        if not l2_node.name in can_be_disabled :
                            self.log.error('Trigger chain %s --> %s has no rules and will be disabled' % (l1_node.name, l2_node.name))
                    l2_node.ps = def_val

            #We should only get here if the L1 has no rule, check daughter rules and set default value if necessary
            if l1_daughter_rules == False and l1_rules == False:
                if len(l1_node.daughters) == 0:
                    if not l1_node.name in can_be_disabled :
                        self.log.warning('Trigger chain %s has no rules and will be disabled' % l1_node.name)
                l1_node.ps = def_val

    def handle_undefined_rate(self, node):
        """Emmit warning message and set default value for items where a
        rule depends on an unknown rate
        """
        
        if node.signature == None:
            self.log.debug("No reference rate found for %s" %node.name)
            if node.rule != None and len(set(["rate", "maxRate"]) & set(node.rule.keys())) > 0:
                self.log.error("Can't set target rate if no reference rate found - setting PS = -1 for %s" % node.name)
                node.rule = {"PS" : -1}
                  
            node.signature = {"rate" : 0, "prescale" : 0}           

    def first_pass(self):
        """Initial pass to set the easy rules
        """

        self.log.debug("Begin looping over the trigger tree")
        for l1_node in self.l1_items.values():

            #self.log.debug("L1: %s" % l1_node.name)
            #*#** Apply protection here otherwise the l1_node.signature["rate"] doesn't work.
            if l1_node.is_multiseed() and l1_node.rule == None:
                l1_node.rule = {"PS" : -1}
                l1_node.signature = {"rate" : 0, "prescale" : 0}
                #raise ValueError("Multiseeded items must be defined with a PS: %s" % l1_node, [d.name for d in l1_node.daughters])

            if l1_node.signature == None:
                self.handle_undefined_rate(l1_node)

            #Some hacking here to handle calculated random rates not in the trigger costs file
            if 'L1_RD' == l1_node.name[:5]:
                l1_node.rate = l1_node.signature.get("rd_rate", 0)
            else:
                l1_node.rate = l1_node.signature["rate"] * l1_node.signature["prescale"]
            l1_node.rate *= self.calc_sf(l1_node)

            if l1_node.rule != None and "depends_on" not in l1_node.rule:
                l1_node.ps = self.apply_rule(l1_node, 
                                             l1_node.rate,
                                             1.,
                                             1)
            try:
                l1_node.rate *= 1 / l1_node.ps
            except ZeroDivisionError:
                l1_node.rate = 0.
            
            for l2_node in l1_node.daughters:
                #self.log.debug("  L2: %s" % l2_node.name)
                if l2_node.signature == None:
                    self.handle_undefined_rate(l2_node)

                try:
                    l2_eff = calc_eff(l2_node, l1_node)
                except ValueError,e:
                    self.log.warning(str(e)+" : Could not calculate a sensible efficiency for %s to %s, assuming 100%% efficiency" % (l1_node.name, l2_node.name))
                    l2_eff = 1

   
                if l2_node.rule  != None and "depends_on" not in l2_node.rule:
                    l2_node.ps = self.apply_rule(l2_node, 
                                                 l1_node.rate,
                                                 l2_eff,
                                                 l1_node.ps)

                l2_node.rate = l1_node.rate * l2_eff / l2_node.ps if l2_node.ps > 0 else 0.
#                 if l1_node.ps == -1 and l2_node.ps > 0 and (l2_node.rule == None or not l2_node.rule.get("rerun", False)):
#                     l2_node.ps = -1
    
                for ef_node in l2_node.daughters:
                    #self.log.debug("    EF: %s" % ef_node.name)
                    if ef_node.signature == None:
                        self.handle_undefined_rate(ef_node)

                    try:
                        ef_eff = calc_eff(ef_node, l2_node)
                    except ValueError,e:
                        self.log.warning(str(e)+" : Could not calculate a sensible efficiency for %s to %s, assuming 100%% efficiency" % (l2_node.name, ef_node.name))
                        ef_eff = 1
                    
                    if ef_node.rule != None and "depends_on" not in ef_node.rule:
                        ef_node.ps = self.apply_rule(ef_node, 
                                                     l2_node.rate,
                                                     ef_eff,
                                                     l1_node.ps * l2_node.ps)

                    try:
                        ef_node.rate = l2_node.rate * ef_eff / ef_node.ps
                    except ZeroDivisionError:
                        ef_node.rate = 0.

#                     if l2_node.ps == -1 and ef_node.ps > 0  and (ef_node.rule == None or not ef_node.rule.get("rerun", False)):
#                         ef_node.ps = -1
                        
        self.log.debug("Done looping over trigger tree")

    def resolve_dependencies(self):
        """After first_pass, apply PS to chains with a 'depends_on'
        rule"""

        ###EAS Too much copy/paste going on here, should be able to merge calls for first_pass()

        l1_items = set()

        #Walk up to the root of the tree
        for dep_item in self.dependent_items.values():
            parent = dep_item
            while len(parent.parents) > 0:
                parent = parent.parents[0]

            l1_items.add(parent)

        #Re-run through this sub-set of l1 items
        for l1_node in l1_items:

            l1_node.rate = l1_node.signature["rate"] * l1_node.signature["prescale"] * self.calc_sf(l1_node)


            if l1_node.rule != None and "depends_on" in l1_node.rule:
                l1_node.ps = self.apply_rule(l1_node, 
                                             l1_node.rate,
                                             1.,
                                             1)
    
            try:
                l1_node.rate *= 1 / l1_node.ps
            except ZeroDivisionError:
                l1_node.rate = 0

            for l2_node in l1_node.daughters:
                try:
                    l2_eff = calc_eff(l2_node, l1_node)
                except ValueError,e:
                    self.log.warning(str(e)+": Could not calculate a sensible efficiency for %s to %s, assuming 100%% efficiency" % (l1_node.name, l2_node.name))
                    l2_eff = 1                
    
                if l2_node.rule != None and "depends_on" in l2_node.rule:
                    l2_node.ps = self.apply_rule(l2_node, 
                                                 l1_node.rate,
                                                 l2_eff,
                                                 l1_node.ps)
                          
                try:
                    l2_node.rate = l1_node.rate * l2_eff / l2_node.ps
                except ZeroDivisionError:
                    l2_node.rate = 0

#                 if l1_node.ps == -1 and l2_node.ps > 0  and (l2_node.rule == None or not l2_node.rule.get("rerun", False)):
#                     l2_node.ps = -1
    
                for ef_node in l2_node.daughters:
                    try:
                        ef_eff = calc_eff(ef_node, l2_node)
                    except ValueError,e:
                        self.log.warning(str(e)+" : Could not calculate a sensible efficiency for %s to %s, assuming 100%% efficiency" % (l2_node.name, ef_node.name))
                        ef_eff = 1
                        
                    if ef_node.rule != None  and "depends_on" in ef_node.rule:
                        ef_node.ps = self.apply_rule(ef_node, 
                                                     l2_node.rate,
                                                     ef_eff,
                                                     l1_node.ps * l2_node.ps)
                    try:
                        ef_node.rate = l2_node.rate * ef_eff / ef_node.ps
                    except ZeroDivisionError:
                        ef_node.rate = 0

#                     if l2_node.ps == -1 and ef_node.ps > 0  and (ef_node.rule == None or not ef_node.rule.get("rerun", False)):
#                         ef_node.ps = -1
                
    def optimize_node(self, node):
        """Check the PS values of a node's daughters and move the PS up if possible"""

        prescales = []
        is_disabled = False
        is_rerun = False
        is_propagated = False

        #Node which ask for a PT should be fed enough rate to reach their target
        #if node.rule != None and node.rule.get("PTRate", 0) > 0:
        #    pt_ps = node.parents[0].rate / (node.rule["PTRate"] * node.ps)
        #    prescales.append(pt_ps)
                
        
        for daughter in node.daughters:

            #A daughter with the propagate flag set to false should
            #block optimization
            #A daughter with the propagate flag set to true should
            #force the prescale down regardless of this node's rule
            if daughter.rule != None and "propagate" in daughter.rule:
                if daughter.rule["propagate"]:
                    is_propagated = True
                else:
                    prescales.append(1)

            if node.name in ["L1_multiseed", "L1_unseeded", "L2_unseeded", "L2_multiseed"]:
                prescales.append(1)
        
            d_ps = self.optimize_node(daughter)

            if d_ps > 0:
                prescales.append(d_ps)
            #The rerun prescale shouldn't ever be propagated down to L1
            elif d_ps == 0 and node.name[0:2] != "L1":
                is_rerun = True
            else:
                is_disabled = True

        if is_disabled and len(prescales) == 0:
            prescales.append(-1)
        elif is_rerun and len(prescales) == 0:
            prescales.append(0)


        #at HLT, PS can have fractional values, so just use the
        #smallest PS
        try:
            min_ps = min(prescales)
        except ValueError:
            min_ps = 1.
            
        #At L1, PS values are expected to be integers, so find
        #greatest common denominator
        
        if node.name[0:2] == "L1":
            min_ps = math.floor(min_ps)
            #min_ps = reduce(fractions.gcd, prescales)
                
        #Nodes which explicitely set their PS or rate should not be
        #optimized, unless a daughter over-rides this with the
        #"propagate" flag
        if node.rule != None \
                and ("rate" in node.rule \
                         or "PS" in node.rule \
                         or "fraction" in node.rule) \
                and (not is_propagated):
            min_ps = 1.

        #Propagate the changes
        if node.ps != -1 and node.ps * min_ps <= maxPS:
            node.ps = round_figures(node.ps * min_ps, self.nsig)
        elif node.ps * min_ps > maxPS:
            min_ps = 1
        try:
            node.rate *= 1. / min_ps
        except ZeroDivisionError:
            node.rate = 0

        for daughter in node.daughters:
            if daughter.ps > 0:
                try:
                    daughter.ps = round_figures(daughter.ps / min_ps, self.nsig)
                except ZeroDivisionError:
                    daughter.ps = 0

        if (node.ps < 1 and node.ps > 0):
            self.log.error("%s's prescale has been optimized to less than 1, fixing it to 1" % node.name)
            node.ps = 1.0
        #If the propagate flag is explicitely set to false, then we do not optimize upstream

        if node.ps <= 0:
            node.rate = 0
        
        if (node.rule != None and not node.rule.get("propagate", True)):
            return 1.0
        else:
            return node.ps


    def optimize_all(self):
        """Run the optimization on the entire set of known l1 nodes"""
    
        for l1_node in self.l1_items.values():
            self.optimize_node(l1_node)
            

    def apply_ES_PT(self, parent_node, daughter_node, parent_rate):
        """For a given parent_node, daughter_node pair with parent
        output rate = parent_rate, compute the daughter ES and PT
        prescales
        """

        if daughter_node.rule == None:
            return

        try:
            daughter_node.pt = round_figures(
                parent_rate / (daughter_node.ps * daughter_node.rule.get("PTRate", 0))
                , self.nsig)
        except ZeroDivisionError:
            daughter_node.pt = 0

        try:
            daughter_node.es = round_figures(
                parent_rate * calc_eff(daughter_node, parent_node) \
                / (daughter_node.ps * daughter_node.rule.get("ESRate", 0))
                , self.nsig)
        except ValueError:
            daughter_node.es = 0
        except ZeroDivisionError:
            daughter_node.es = 0

        if "PTValue" in daughter_node.rule:
            daughter_node.pt = daughter_node.rule["PTValue"]
            
        if "ESValue" in daughter_node.rule:
            daughter_node.es = daughter_node.rule["ESValue"]

        for genre in ["pt", "es"]:
            if getattr(daughter_node, genre) < 1 and getattr(daughter_node, genre) > 0:
                self.log.error("Will not be able to reach target %s for %s" % (genre.upper(), daughter_node.name))
                setattr(daughter_node, genre, 1)

    def compute_ES_and_PT(self, items = None):
        """Travel through the tree, computing the ES and PT factors
        """

        if items == None:
            items = self.l1_items
        
        for l1_node in items.values():

            if l1_node.ps != 0:
                l1_rate = float(l1_node.signature["rate"]) * \
                          self.calc_sf(l1_node) *\
                          (l1_node.signature["prescale"] / l1_node.ps)
            else:
                l1_rate = 0

            for l2_node in l1_node.daughters:
                self.apply_ES_PT(l1_node, l2_node, l1_rate)

                try:
                    l2_eff = calc_eff(l2_node, l1_node)
                except ValueError,e:
                    self.log.warning(str(e)+" : Could not calculate a sensible efficiency for %s to %s, assuming 100%% efficiency" % (l1_node.name, l2_node.name))
                    l2_eff = 1
                    
                try:
                    l2_rate = l1_rate * l2_eff / l2_node.ps
                except ZeroDivisionError:
                    l2_rate = 0

                for ef_node in l2_node.daughters:
                    self.apply_ES_PT(l2_node, ef_node, l2_rate)


    def print_summary(self):
        """Make a human readable dump of the menu's current state"""

        output = ""

        output += "\n%s" % ("-" * 230)
        output += "\n%60s %60s %60s" % ("l1_name", "l2_name", "ef_name")

        for l1_node in self.l1_items.values():
            output += "\n%s" % ("-" * 230)
            
            if l1_node.ndaughters() == 0:
                output += "\nname:%55s" % (l1_node.name)
                output += "\nrate:%55s" % (l1_node.rate)
                output += "\nrule:%55s" % (l1_node.rule)
                output += "\nps:%57s" % (l1_node.ps)
                output += "\npt:%57s" % (l1_node.pt)
                output += "\nes:%57s" % (l1_node.es)
                output += "\n"

            for l2_node in l1_node.daughters:

                if l2_node.ndaughters() == 0:
                    output += "\nname:%55s %60s" % (l1_node.name, l2_node.name)
                    output += "\nrate:%55s %60s" % (l1_node.rate, l2_node.rate)
                    output += "\nrule:%55s %60s" % (l1_node.rule, l2_node.rule)
                    output += "\nps:%57s %60s" % (l1_node.ps, l2_node.ps)
                    output += "\npt:%57s %60s" % (l1_node.pt, l2_node.pt)
                    output += "\nes:%57s %60s" % (l1_node.es, l2_node.es)
                    output += "\n"

                for ef_node in l2_node.daughters:

                    output += "\nname:%55s %60s %60s" % (l1_node.name, l2_node.name, ef_node.name)
                    output += "\nrate:%55s %60s %60s" % (l1_node.rate, l2_node.rate, ef_node.rate)
                    output += "\nrule%55s %60s %60s" % (l1_node.rule, l2_node.rule, ef_node.rule)
                    output += "\nps:%57s %60s %60s" % (l1_node.ps, l2_node.ps, ef_node.ps)                    
                    output += "\npt:%57s %60s %60s" % (l1_node.pt, l2_node.pt, ef_node.pt)                    
                    output += "\nes:%57s %60s %60s" % (l1_node.es, l2_node.es, ef_node.es)                    
                    output += "\n"

        return output
        
    def compute_ps(self):
        """Invoke the series of methods necessary for calculating the full set of prescales
        """

        self.log.debug("Set default prescales")
        self.set_defaults()
        if self.log.level == logging.DEBUG:
            print self.print_summary()

        self.log.debug("First pass of prescale calculations")
        self.first_pass()
        
        if self.log.level == logging.DEBUG:
            print self.print_summary()
            
        self.log.debug("First optimization")
        self.optimize_all()
        
        if self.log.level == logging.DEBUG:
            print self.print_summary()
            
        self.log.debug("Dependencies")
        self.resolve_dependencies()
        if self.log.level == logging.DEBUG:
            print self.print_summary()
            
        self.log.debug("Second Optimization" )
        self.optimize_all()
        if self.log.level == logging.DEBUG:
            print self.print_summary()
            
        self.log.debug("ES and PT")
        self.compute_ES_and_PT()
        if self.log.level == logging.DEBUG:
            print self.print_summary()

        if self.config["log"] != "":
            log = open(self.config["log"], 'w')
            log.write(self.print_summary())
        #*#** Note: The l2 and l1 prescale optimization could eventually take place in 1 step, but for simplicity leaving it like that for now. We'll need to check whether having less steps than that could be an option, if the prescale generation takes time (which shouldn't anyways).



def check_rulebook_for_duplicates(rulebook, logger = None):
    """Parse the text in the rulebook to find duplicate keys
    """
    if logger == None:
        logger = logging.getLogger()

    #filename = find_file_in_env(rulebook+".py", "PYTHONPATH")
    filename = rulebook
    #print "EAS: %s" % filename
    
    rulebook_file = open(filename,"r")
    trigger_list_in_rulebook = []
    found_error = False
    for line in rulebook_file:
        if ":" in line :
            trigger = (line.split(":")[0]).strip(" ")
            if any(level in trigger for level in ["L1", "L2", "EF"] ) and "#" not in trigger:
                if trigger in trigger_list_in_rulebook:
                    logger.error("Rule defined more than once for trigger %s\n" %trigger)
                    found_error = True
                else:
                    trigger_list_in_rulebook.append(trigger)

    if found_error == True:
        sys.exit("\nEXIT: at least a trigger had rules defined more than once \n")


class OutputChecker:
    """Run over the rule reader's output and make sure we haven't bungled anything obvious
    To add a new check, define a method named 'check_*' and it will be called by check_all()
    """

    def __init__(self, l1_items, all_items, rules, l1_xml = None, hlt_xml = None, logger = None):
        if logger == None:
            self.log = create_logger("OutputChecker", 2)
        else:
            self.log = logger
        
        self.l1_items = l1_items
        self.all_items = all_items
        self.rules = rules
        self.l1_xml = l1_xml
        self.hlt_xml = hlt_xml

    def check_all(self):
        """Look for all "check_*" functions and call them one by one"""

        decision = True
        for func in dir(self):
            if func[0:6] == "check_" and func != "check_all":
                decision &= getattr(self, func)()

        if decision == False:
            self.log.critical("One or more checks have failed")

    def check_rerun_ps(self):
        """Check that all items with a "rerun" tag do not have PS = -1"""

        output = True
        for node in self.all_items.values():
            if node.rule != None and node.rule.get("rerun", False) and node.ps < 0:
                if "multiseed" not in node.name and node.name[0:2] != "L1":
                    self.log.error("Node %s has PS = %s but is in rerun mode" % (node.name, node.ps))
                    output &= False

        return output

    def check_rerun_xml(self):
        """Check that all items with a "retun" tag were properly configured as rerun chains in the input xml"""

        output = True
        if self.hlt_xml == None:
            return output
        
        rerun_chains = []
        for hlt_item in ET.parse(self.hlt_xml).getroot().findall("CHAIN_LIST/CHAIN"):
            if hlt_item.get('rerun_prescale') == "1":
                rerun_chains.append(hlt_item.get('chain_name'))
        
        for node in self.all_items.values():
            if node.rule != None and node.rule.get("rerun", False) and node.name not in rerun_chains:
                self.log.error("Node %s has rerun in the rulebook, but not the xml" % node.name)
                output = False

        return output

    def check_express_xml(self):
        """Check that all items with a "retun" tag were properly configured as rerun chains in the input xml"""

        output = True
        if self.hlt_xml == None:
            return output
        
        express_chains = []
        for hlt_item in ET.parse(self.hlt_xml).getroot().findall("CHAIN_LIST/CHAIN"):
            for stream_tag in hlt_item.findall("STREAMTAG_LIST/STREAMTAG"):
                if stream_tag.get('stream') == "express":
                    express_chains.append(hlt_item.get('chain_name'))

        for node in self.all_items.values():
            if node.rule != None and (node.rule.get("ESValue", False) or node.rule.get("ESRate", False)) and node.name not in express_chains:
                self.log.error("Node %s has express rate in the rulebook, but is not configured as an express chain in the xml" % node.name)
                output = False

        return output


    def check_legal_ps(self):
        """Check that the PS does not fail our conventions"""

        output = True
        for node in self.all_items.values():
            if node.ps > 0 and node.ps < 1:
                self.log.error("Node %s has positive PS < 1.0 (%s)" % (node.name, node.ps))
                output &= False
                
            if node.name[0:2] == "L1" and node.ps != int(node.ps):
                self.log.error("Node %s has a fractional PS at L1 (%s)" % (node.name, node.ps))
                output &= False

            if node.ps > maxPS:
                self.log.error("Node %s has a PS > maxPS (%s)" % (node.name, node.ps))
                output &= False

            if node.ps < -1:
                self.log.error("Node %s has a PS < -1 (%s)" % (node.name, node.ps))
                output &= False
                
        return output

    def check_signs(self):
        """Check that the rate and PS signs are compatible"""

        output = True
        for node in self.all_items.values():
            if node.total_ps() > 0 and node.rate < 0:
                self.log.error("Node %s has a positive prescale (%s) but a negative rate (%s)" % (node.name, node.total_ps(), node.rate))
                output &= False
                
            elif node.total_ps() < 0 and node.rate > 0:
                self.log.error("Node %s has a negative prescale (%s) but a positive rate (%s)" % (node.name, node.total_ps(), node.rate))
                output &= False
                
        return output
                
    def check_orphans(self):
        """Give a warning if there is an item at the end of a chain with no explicit rule and isn't disabled"""
        
        output = True
        for l1_node in self.l1_items.values():
            if len(l1_node.daughters) == 0 and l1_node.rule == None and l1_node.ps != -1:
                self.log.error("Node %s ends the chain with no rule, but isn't disabled" % l1_node.name)
                output &= False
                
            for l2_node in l1_node.daughters:
                if len(l2_node.daughters) == 0 and l2_node.rule == None and l2_node.ps != -1:
                    self.log.error("Node %s ends the chain with no rule, but isn't disabled" % l2_node.name)
                    output &= False
                    
                for ef_node in l2_node.daughters:
                    if ef_node.rule == None and ef_node.ps != -1:
                        self.log.error("Node %s ends the chain with no rule, but isn't disabled" % ef_node.name)
                        output &= False
                        
        return output

    def check_max_rates(self):
        """Check that the expected rate of individual chains is reasonable"""
        #The following is a very naive implementation, this requires further thought

        #L1max =

        return True

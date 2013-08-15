# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Helper functions and calculation tools for the rulebook
"""

from RepresentationClasses import TriggerSignature, TriggerNode
import math
import logging
import sys

########################################
# Tools for applying the rules to rates
########################################

def calc_eff(d_node, p_node):
    """Compute a node's efficiency with respect to it's parent
    """
    
    if d_node.signature == None:
        raise ValueError("Cannot compute efficiency for a chain without a signature: %s" % d_node)
        return 1

    if d_node.signature["prescale"] <= 0:
        raise ValueError("Cannot compute efficiency for a chain disabled online: %s" % d_node)
        return 1
    if p_node.signature["rate"] <= 0:
        raise ValueError("Cannot compute efficiency for a chain with no rate online: %s" % d_node)
        return 1

    output = d_node.signature["rate"] *  d_node.signature["prescale"]  / p_node.signature["rate"]

    if (output > 1):
        raise ValueError("Efficiency greater than 1")
        output = 1
    
    return output
    
def find_rule(lumi, rule, use_lowest = False):
    """Function to find the most appropriate rule for a target
    luminosity (eg. 1e30) or run condition (eg. Standby)
    """   

    output_rule = None

    if rule == None:
        return output_rule

    #EAS
    #This isn't yet handling all the subtleties of strings as
    #"lumi" points, or rules which aren't explicitely defined

    #In the case where a string is passed, or the lumi is identically matched, return the rule
    if lumi in rule:
        output_rule = rule[lumi]
        output_rule["lumi"] = lumi

    else:
       #If the lumi was a string, but isn't matched, then we have a problem
        try:
            lumi = int(lumi)
        except ValueError:
            raise ValueError('Non int lumi specified but not found in lumi keys: %s' % lumi)
            #return None

        #Look for the highest rule which is <= lumi and is an int
        lowest_key = None
        rule_keys = sorted(rule.keys(), reverse=True)

        assigned_rule = False
        for key in rule_keys:
            lowest_key = key
            try:
                if int(key) <= lumi:
                    output_rule = rule[key]
                    output_rule["lumi"] = key
                    assigned_rule = True 
            except ValueError:
                continue
            if assigned_rule == True:
                break
            
    #If no matching rule has been found, the user must explicitely ask to use the lowest available key
    if output_rule == None and use_lowest:
        try:
            output_rule = rule[lowest_key]
            output_rule["lumi"] = lowest_key
        except:
            print rule
            raise KeyError("Tried to use lowest key, but none were found")
        
    if output_rule != None:
        return output_rule

    #If no matching rule has been found, raise an error
    raise KeyError('No rule has been defined matching lumi %s' % lumi)
    

def round_figures(value, precision):
    """Returns x rounded to n significant figures."""

    if value == 0:
        return 0

    abs_value = abs(value)
    
    round_value =  round(abs_value, 
                       int(precision - math.ceil(math.log10(abs(abs_value))))
                       )

    return math.copysign(round_value, value)

def gcd(a, b):
    """Compute the greatest common denominator for a pair of values"""

    c = round(min(a,b),0)
    d = round(max(a,b),0)

    print c, d

    while c:
        c, d = d%c, c
    return d

######################################
# Tools for the online rates XML
######################################

def read_online_rates(rates_xml, lumi):
    """Convert the rates from XML to python objects"""
    output = {}

    for levels in rates_xml.findall('level'):
        for sig in levels.findall('signature'):
            sig_object = TriggerSignature(sig, lumi)
            if sig_object != None:
                output[sig_object["sig_name"]] = sig_object

    return output

def read_online_metadata(rates_xml, mapping):
    """Pull the bunch group structure and luminosity information from
    the XML"""
    output = {}
    

    # TriggerCosts format
    output["lumi"] = rates_xml.findtext("Luminosity")
    if output["lumi"] == None or output["lumi"] == '0':
        output["lumi"] = rates_xml.findtext("PredictionLumi")
        
    # exampleCosts from TRP format
    if (output["lumi"] == None or output["lumi"] == '0') and rates_xml.findtext("lumi_info/rec_lumi_ub-1") != None:
        try:
            output["lumi"] = "%d" % (float(rates_xml.findtext("lumi_info/rec_lumi_ub-1")) / float(rates_xml.findtext("lumi_info/live_time")) * 1e30)
        except ZeroDivisionError:
            output["lumi"] = "0"

    # TriggerCosts format
    bunch_groups = rates_xml.findall("bunchgroup/bunchgrouptype")
    for group in bunch_groups:
        key = int(group.findtext("bunchgroup_keynum"))
        if key not in mapping:
            continue
        output[mapping[key]] = group.findtext("bunchgroup_size")

    #Try the old style formatting of TriggerCosts files
    if len(output.keys()) == 1:
        for key in mapping.keys():
            value = rates_xml.find("PredictionBGRP%s" % key)
            if value != None:
                output[mapping[key]] = value.text

    #Try the exmapleCosts format
    if len(output.keys()) == 1:
        bunch_groups = [(i,
                         rates_xml.findtext("lumi_info/lb/nbunchgroup%d" % i),
                         rates_xml.findtext("lumi_info/lb/bunchgroupName%d" % i)
                         )
                        for i in xrange(8)
                        ]

        print mapping
        for group in bunch_groups:
            print group
            key = group[0]
            if key not in mapping:
                continue
            output[mapping[key]] = group[1]
            
            
    if None in output.values():
        raise KeyError("Missing metadata information: %s" % output)

    for key in output:
        if output[key] != None:
            output[key] = float(output[key])
        if key == "lumi":
            output[key] /= 1e30

    if set(output.keys()) != set(mapping.values() + ["lumi"]):
        raise KeyError("Mismatched metadata information, expected %s, found %s" % (mapping.values(), output.keys())) 
    
    return output

def rules_for_lumi(target_lumi, rules):
    """Condense the rulebook down to 1 rule per item, based on target lumi"""
    output = {}
    
    for item in rules:
        closest_lumi = 0
        lumi_points = sorted(rules[item].keys())
        for lumi in lumi_points:
            if lumi > target_lumi:
                break
            closest_lumi = lumi
        
        if closest_lumi == 0:
            closest_lumi = lumi_points[0]
            
        
            output[item] = {'lumi': closest_lumi, 
                            'rule' : rules[item][closest_lumi] }
        
    return output

######################################
# Tools for the menu XML
######################################

def sort_levels(lvl):
    """Key function to sort triggers by name"""
    levels = {"EF": 2, "L2" : 1, "L1" : 0}
    return "%s%s" % (levels[lvl[0:2]], lvl[2:])

def read_l1(l1_xml):
    """Return all of the L1 items in an XML menu"""
    output = []

    for l1_item in l1_xml.findall('TriggerMenu/TriggerItem'):    
        output.append(l1_item.get('name'))

    return output

def read_hlt(hlt_xml, level):
    """Return all of the L2 or EF items, and their lower chains
    in an XML menu
    """

    output = {}
    for hlt_item in hlt_xml.findall('CHAIN_LIST/CHAIN'):
        if hlt_item.get('level') == level:
            chain_name = hlt_item.get('chain_name')
            lower_chain_name = hlt_item.get('lower_chain_name')
            ##EAS this is a hack
            if ',' in lower_chain_name:
                if level == "L2":
                    lower_chain_name = "L1"
                elif level == "EF":
                    lower_chain_name = "L2"
                lower_chain_name += "_multiseed"
            elif len(lower_chain_name) == 0:
                if level == "L2":
                    lower_chain_name = "L1"
                elif level == "EF":
                    lower_chain_name = "L2"
                lower_chain_name += "_unseeded"

            output[chain_name] = lower_chain_name

    return output

def build_groups_xml(l2_triggers, ef_triggers):
    """Group triggers by their lower item names
    """

    #Placeholders
    l2_groups = {}
    ef_groups = {}

    for (name, lower_name) in l2_triggers.items():
        group = l2_groups.get(lower_name, [])
        group.append(name)
        l2_groups[lower_name] = group

    for (name, lower_name) in ef_triggers.items():
        group = ef_groups.get(lower_name, [])
        group.append(name)
        ef_groups[lower_name] = group
    
    return (l2_groups, ef_groups)

def build_groups_chain(chains):
    """Group triggers by their lower item names
    """
    
    groups = {}
    for item in chains:
        l1_group = groups.get(item.names["l1"], {})
        l2_group = l1_group.get(item.names["l2"], [])
        l2_group.append(item)
        l1_group[item.names["l2"]] = l2_group
        groups[item.names["l1"]] = l1_group

    return groups

def build_tree(l1_xml, hlt_xml, trigger_rules, trigger_signatures, target_lumi, use_lowest_rule):
    """Build a tree representation of the triggers"""

    output_tree = {}

    #Get all of the l1 triggers
    l1_triggers = read_l1(l1_xml)
    l1_triggers.append("L1_multiseed")
    l1_triggers.append("L1_unseeded")
    #Get all of the L2 and EF triggers and their lower names
    l2_triggers = read_hlt(hlt_xml, "L2")
    l2_triggers["L2_unseeded"] = "L1_unseeded"
    ef_triggers = read_hlt(hlt_xml, "EF")

    (l2_groups, ef_groups) = build_groups_xml(l2_triggers, ef_triggers)


    #Sort the groups
    for groups in (l2_groups, ef_groups):
        for key in groups:
            groups[key] = sorted(groups[key])

    for l1_name in l1_triggers:
        l1_node = TriggerNode(l1_name, 
                              find_rule(target_lumi, trigger_rules.get(l1_name, None), use_lowest_rule),
                              trigger_signatures.get(l1_name, None))
        
        output_tree[l1_node.name] = l1_node
        
        if not l1_name in l2_groups:
            continue
        
        l2_group = l2_groups.get(l1_name)
        for l2_name in l2_group:
            
            l2_node = TriggerNode(l2_name,
                                  find_rule(target_lumi, trigger_rules.get(l2_name, None), use_lowest_rule),
                                  trigger_signatures.get(l2_name, None))

            l2_node.add_parent(l1_node)
            l1_node.add_daughter(l2_node)

            if not l2_name in ef_groups:
                continue
            
            ef_group = ef_groups.get(l2_name)
            for ef_name in ef_group:

                ef_node = TriggerNode(ef_name,
                                      find_rule(target_lumi, trigger_rules.get(ef_name, None), use_lowest_rule),
                                      trigger_signatures.get(ef_name, None))
                ef_node.add_parent(l2_node)
                l2_node.add_daughter(ef_node)
                            

    return output_tree


######################################
# Tools for tools (hah!)
######################################

def create_logger(name, verbosity):
    """Create a logger with our desired formatting and level of
    verbosity
    """
    
    output = logging.getLogger(name)
    output.handlers = []
    verbosity_numbers = [  logging.CRITICAL, logging.ERROR
                           , logging.WARNING, logging.INFO
                           , logging.DEBUG]
    
    verbosity = max( min(verbosity, 5), 1)
    lvl = verbosity_numbers[verbosity-1]
    tab_formatting = logging.Formatter( '%(levelname)s\t  %(name)s\t%(message)s'
#                                       , datefmt = '%Y-%b-%d %H:%M:%S'
                                       )
    
    out_handler = logging.StreamHandler(sys.stdout)
    out_handler.setLevel(lvl)
    out_handler.setFormatter(tab_formatting)
    output.addHandler(out_handler)

    output.setLevel(lvl)
    return output

def find_file_in_env(filename, pathname):
    """Search through the environment paths (eg. PATH, PYTHONPATH, etc...) for a specific file
    """

    from os import environ
    from os.path import exists, join

    if filename[0:2] == "./" or filename[0] == "/":
        return filename

    if exists(filename):
        return filename

    pathname = pathname.upper()
    
    if pathname not in environ:
        return filename

    for path in environ[pathname].split(":"):
        path = join(path, filename)
        if exists(path):
            return path

    return filename

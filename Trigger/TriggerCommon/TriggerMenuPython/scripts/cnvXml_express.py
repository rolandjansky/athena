#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## use it as such:
## python cnvXML.py --lv1xml=... --hltxml=... --ps_name=... --ps_xml=...
## where lv1xml is the reference lv1 xml file
##       hltxml is the reference hlt xml file
##       ps_name is the prescales file
##       psxml is the output file name suffix (two output files will be generated, one for lvl1 and one for hlt.


#-----------------------------------------------------------------------------
# Create and configure option parser
#

from optparse import OptionParser
p = OptionParser(usage="usage: python cnvXML.py --lv1xml=... --hltxml=... --ps_name=... --ps_xml=...", version="1.0")

p.add_option( '--lv1xml',  type="string", default="l1_xml.xml", dest="lv1xml",  help="LV1 menu XML file - from P1 SMK")
p.add_option( '--hltxml',  type="string", default="hlt_xml.xml", dest="hltxml",  help="HLT menu XML file - from P1 SMK")
p.add_option( '--ps_name', type="string", default="", dest="ps_name", help="PS set name")
p.add_option( '--ps_xml',  type="string", default="", dest="ps_xml",  help="PS XML file with PS and PT")

p.add_option( '--lv1_id',  type="int", default=0, dest="lv1_id",  help="LV1 menu ID in P1 trigger DB")
p.add_option( '--hlt_id',  type="int", default=0, dest="hlt_id",  help="HLT menu ID in P1 trigger DB")

p.add_option('-d', '--debug',        action="store_true", default=False, dest="debug", help="print debug messages")


(options, args) = p.parse_args()

#-----------------------------------------------------------------------------
# Create and configure python logger
#
import logging

def getLog(name, level = logging.INFO):

    if options.debug:
        level = logging.DEBUG

    f = logging.Formatter("%(message)s")
    h = logging.StreamHandler()
    h.setFormatter(f)
    
    log = logging.getLogger(name)
    log.setLevel(level) 
    log.addHandler(h)

    return log

log = getLog('cnvXML')

#-----------------------------------------------------------------------------
# XML parsing functions
#

import xml.etree.cElementTree as ET

def readPsFile(fname):
    
    def fill(level):
        items = []
        v = level.findall('signature')
        for x in v:
            items.append( (x.findtext('sig_name'),
                           x.findtext('chain_prescale'),
                           x.findtext('passthrough'),
                           x.findtext('express_prescale')),)
        print 'N items: %d (%s)' % (len(items), level.findtext('lvl_name'))
        return items
        
                          
    doc = ET.parse(fname)
    root = doc.getroot()
    levels = root.findall('level')
    l1_items, l2_chains, ef_chains = [], [], []
    for level in levels:
        lvl_name = level.findtext('lvl_name')
        if lvl_name == 'L1':
            l1_items = fill(level)
        elif lvl_name == 'L2':
            l2_chains = fill(level)
        elif lvl_name == 'EF':
            ef_chains = fill(level)

    print         
    return (l1_items, l2_chains, ef_chains)

#-----------------------------------------------------------------------------
# Read menu XML file
#
name2ps = {}

def readXmlConfig(l1xml, hltxml):
    name2id = {}
    
    # L1
    doc = ET.parse(l1xml)
    root = doc.getroot()
    
    name2id['LVL1Config:name']  = root.get('name')
    name2id['LVL1Config:id']    = root.get('id')    
    name2id['TriggerMenu:name'] = root.find('TriggerMenu').get('name')
    name2id['TriggerMenu:id']   = root.find('TriggerMenu').get('id')    
    name2id['PrescaleSet:name'] = root.find('PrescaleSet').get('name')

    if name2id['LVL1Config:id'] == None or name2id['LVL1Config:id'] == '1':
        name2id['LVL1Config:id'] = "96"
        log.warning('==> LVL1 ID set to '+name2id['LVL1Config:id']+' (hardcoded)')
        
    if name2id['TriggerMenu:id'] == None or name2id['TriggerMenu:id'] == '1':
        name2id['TriggerMenu:id'] = "84"
        log.warning('==> Menu ID set to '+name2id['TriggerMenu:id']+' (hardcoded)')
                
    v = root.findall('TriggerMenu/TriggerItem')
    for x in v:
        name = x.get('name')
        id = x.get('ctpid')
        if name not in name2id: name2id[name] = id
        
    # HLT
    doc = ET.parse(hltxml)
    root = doc.getroot()

    name2id['HLT_MENU:name'] = root.get('name')
    name2id['HLT_MENU:id']   = root.get('id')    

    if name2id['HLT_MENU:name'] == None:
        name2id['HLT_MENU:name'] = "Physics_pp_v2"
        log.warning('==> HLT_MENU name set to '+name2id['HLT_MENU:name']+' (hardcoded)')

    if name2id['HLT_MENU:id'] == None:
        name2id['HLT_MENU:id'] = "1"
        log.warning('==> HLT_MENU id set to '+name2id['HLT_MENU:id']+' (hardcoded)')
        
    name2id['HLT_MENU:name']
    v = root.findall('CHAIN_LIST/CHAIN')
    for x in v:
        name = x.get('chain_name')
        id   = x.get('chain_counter')
        if name not in name2id: name2id[name] = id

        name2ps[name] = x.get('prescale')

    return name2id

#-----------------------------------------------------------------------------
# XML writing function
#
def writeXmlPS(ps_l1xml, ps_hltxml, l1_items, l2_chains, ef_chains, name2id, ps_name): 
    f = open(ps_l1xml, 'w')
    f.write('<?xml version="1.0" ?>\n')
    
    #
    # Set LV1 and menu ids from input XML files
    #
    f.write('<LVL1Config name="%s" id="%s">\n' % (name2id['LVL1Config:name'], name2id['LVL1Config:id']))
    f.write('  <TriggerMenu name="%s" id="%s">\n' % (name2id['TriggerMenu:name'], name2id['TriggerMenu:id']))
    
    for item in l1_items:
        ctpid = '-1'
        if item[0] in name2id.keys(): ctpid = name2id[item[0]]
        f.write('    <TriggerItem ctpid="%s" name="%s"/>\n' % (ctpid, item[0]))
    f.write('  </TriggerMenu>\n')
    f.write('  <PrescaleSet name="%s">\n' % ps_name)
    for item in l1_items:
        ctpid = '-1'
        if item[0] in name2id.keys(): ctpid = name2id[item[0]]
        f.write('    <Prescale ctpid="%s" n="%s" m="0" d="0">%s</Prescale>\n' \
                % (ctpid, item[1], item[1]))
    f.write('  </PrescaleSet>\n')
    f.write('</LVL1Config>\n')
    f.close()

    f = open(ps_hltxml, 'w')
    f.write('<?xml version="1.0" ?>\n')
    f.write('<HLT_MENU id="%s" menu_name="%s" prescale_set_name="%s">\n' % (name2id['HLT_MENU:id'], name2id['HLT_MENU:name'], ps_name))
    f.write('  <CHAIN_LIST>\n')
    print
    for item in l2_chains+ef_chains:
        id = None
        level = ''
        if item[0] in name2id.keys(): id = name2id[item[0]]
        if len(item[0]) > 2: level = item[0][0:2]

        if id == None:
            log.debug('Won t do anything with trigger not in P1 menu: %s' %item[0])
            continue
        
        if (item[3] == None):
            f.write('    <CHAIN chain_counter="%s" chain_name="%s" level="%s" prescale="%s" pass_through="%s"></CHAIN>\n' % (id, item[0], level, item[1], item[2]))
        else:
            f.write('    <CHAIN chain_counter="%s" chain_name="%s" level="%s" prescale="%s" pass_through="%s" express_stream="%s"></CHAIN>\n' % (id, item[0], level, item[1], item[2], item[3]))

    f.write('  </CHAIN_LIST>\n')
    f.write('</HLT_MENU>\n')
    f.close()
    print

    
#-----------------------------------------------------------------------------
# Check consistency of two Menus/XML files
#
def CheckMenus(l1_items, l2_chains, ef_chains, name2id):

    ignores = ['LVL1Config', 'TriggerMenu', 'PrescaleSet', 'TriggerMenu', 'HLTMenu', 'HLT_MENU']    
    no_matches = []
    
    for key, value in name2id.iteritems():
        
        ignore_list = [ x for x in ignores if key.count(x) ]
        if len(ignore_list):
            log.debug('Neglect key: %s' %key)
            continue

        match = False
        
        for item in l1_items+l2_chains+ef_chains:
            if len(item) < 1:
                log.warning('Item list size error in CheckMenus')
                continue

            name = item[0]
            if name == key:
                match = True
                break

        if not match:
            no_matches.append(key)

    no_matches.sort()

    for t in no_matches:
        log.info('Trigger is in P1 menu but has no PS rule: %s' %t)
        
if __name__ == '__main__':    

    ps_lv1xml_out = 'RuleBook_LV1PS_' + options.ps_name + '.xml'
    ps_hltxml_out = 'RuleBook_HLTPS_' + options.ps_name + '.xml'

    print
    print '-----------------------------------------'
    print 'Input:'
    print '  - Prescale XML file (TrigMonCosts): %s' % options.ps_xml
    print '  - LV1 XML file                    : %s' % options.lv1xml
    print '  - HLT XML file                    : %s' % options.hltxml
    print '  - New prescale set name           : %s' % options.ps_name
    print 'Output:'
    print '  - Simplified LV1 XML file         : %s' % ps_lv1xml_out
    print '  - Simplified HLT XML file         : %s' % ps_hltxml_out    
    print '-----------------------------------------'
    print

    #
    # Check inputs for validity
    #
    import sys, os
    
    if len(options.ps_name) < 1:
        log.info('Need PS set name')
        sys.exit(0)

    if not os.path.isfile(options.ps_xml):
        log.info('PS xml file does not exist: %s' % options.ps_xml)
        sys.exit(0)
        
    if not os.path.isfile(options.lv1xml):
        log.info('LV1 xml file does not exist: %s' % options.lv1xml)
        sys.exit(0)     

    if not os.path.isfile(options.hltxml):
        log.info('HLT xml file does not exist: %s' % options.hltxml)
        sys.exit(0)  
    
    #
    # Read LV1 and HLT xml files for trigger names, PS and PT
    #
    (l1_items, l2_chains, ef_chains) = readPsFile(options.ps_xml)

    #
    # Build look up table from trigger names to chain counters/L1 items
    #
    name2id = readXmlConfig(options.lv1xml, options.hltxml)
    
    #
    # Write out LV1 and HLT PS keys
    #    
    writeXmlPS(ps_lv1xml_out, ps_hltxml_out, l1_items, l2_chains, ef_chains, name2id, options.ps_name)

    #
    # Consistency check
    #
    CheckMenus(l1_items, l2_chains, ef_chains, name2id)

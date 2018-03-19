#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os

#-----------------------------------------------------------------------------
# Create and configure option parser
#

from optparse import OptionParser
p = OptionParser(usage="usage: processRuleBook.py <XML files with PS values> --target-lumi=<luminosity in 1E27 units>", version="1.0")

p.add_option('-l', '--target-lumi',  type  ="int",        default=None,  dest="lumi",  help="target luminosity for PS keys")
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

log = getLog('readRuleBook')

#-----------------------------------------------------------------------------
# Helper class: stores rule for one lumi point
#
class LumiPoint:
    def __init__(self, lumi):
    
        self.lumi        = int(lumi)
        self.name        = ''
        self.ps_slope    = None
        self.ps_offset   = None
        self.ps_has_plus = False

        self.pt_slope       = None # PT PS slope
        self.pt_offset      = None # PT PS offset
        self.default_off_pt = 0

        self.es_slope       = None # express stream PS slope
        self.es_offset      = None # express stream PS offset
        self.default_off_ps = -999.0

    def SetValue_PS(self, pars):
        if len(pars) != 2:            
            return False
        
        self.ps_slope  = float(pars[0])
        self.ps_offset = float(pars[1])

        if int(self.ps_slope) == 0 and int(self.ps_offset) == 0:
            self.ps_has_plus = pars[0].count('+') or pars[1].count('+');
            
        return True

    def SetValue_PT(self, pars):
        if len(pars) == 0:
            return True
        elif len(pars) != 2:            
            return False

        self.pt_slope  = float(pars[0])        
        self.pt_offset = float(pars[1])        
        return True

    def SetValue_ES(self, pars):
        if len(pars) == 0:
            return True
        elif len(pars) != 2:
            return False

        self.es_slope  = float(pars[0])
        self.es_offset = float(pars[1])
        return True

    def HasPlusPS(self):
        return self.ps_has_plus
    
    def ComputePS(self, target_lumi):
        if self.lumi < 1:
            return self.default_off_ps
        
        try:
            return float(self.ps_slope)*float(target_lumi)/float(self.lumi)+float(self.ps_offset)
        except:
            log.error('Problem for trigger %s - set PS=off' %self.name)
            return self.default_off_ps

    def ComputeES(self, target_lumi):
        if self.lumi < 1:
            return self.default_off_ps

        if self.es_slope != None:
            return float(self.es_slope)*float(target_lumi)/float(self.lumi)+float(self.es_offset)

        log.error('Problem for trigger %s - set ES=off' %self.name)
        return self.default_off_ps

    def ComputePT(self, target_lumi):
        if self.lumi < 1:
            return self.default_off_pt

        if self.pt_slope != None:
            return float(self.pt_slope)*float(target_lumi)/float(self.lumi)+float(self.pt_offset)

        log.error('Problem for trigger %s - set PT=off' %self.name)
        return self.default_off_pt

#-----------------------------------------------------------------------------
# Helper class: stores rules for one trigger
#
class TriggerRule:
    def __init__(self):
    
        self.trigger_name        = ""
        self.has_express         = False
        self.has_pt              = True
        self.lumi_points         = {}
        
    def ReadLine(self, line):
        
        input_list = line.strip(' \n').split(';')

        if len(input_list) < 3:
            log.warning('Skip incorrectly formatted input line: %s' % line)
            return False

        self.trigger_name = input_list[0].strip()

        lumi_list = input_list[1].strip("[ \t ]").split(":")
        ps_list   = input_list[2].strip("[ \t ]").split(":")
        pt_list   = []
        es_list   = []

        self.has_express = len(input_list) > 3
        self.has_pt      = len(input_list) > 4
        
        if self.has_express: es_list = input_list[3].strip("[ \t ]").split(":")
        if self.has_pt:      pt_list = input_list[4].strip("[ \t ]").split(":")

        if len(lumi_list) < 1:
            log.warning('Failed to parse luminosity values for: %s' % self.trigger_name)
            return False
        if len(lumi_list) != len(ps_list):
            log.warning('Lumi and PS lists have different size for: %s' % self.trigger_name)
            return False
        
        for i in range(0, len(lumi_list)):
            lumiP = LumiPoint(int(lumi_list[i]))
            lumiP.name = self.trigger_name
            
            if not lumiP.SetValue_PS(ps_list[i].strip("( )").split(",")):
                log.warning('Failed to read PS paramemets for: %s' % self.trigger_name)
                return False 
            
            if self.has_express:
                if i < len(es_list):
                    lumiP.SetValue_ES(es_list[i].strip("( )").split(","))
                    log.debug('Read express stream PS paramemets for: %s' % self.trigger_name)
                else:
                    lumiP.es_slope  = 0.0
                    lumiP.es_offset = 0.0
                    
            if self.has_pt:
                if i < len(pt_list):
                    lumiP.SetValue_PT(pt_list[i].strip("( )").split(","))
                    log.debug('Read PT paramemets for: %s' % self.trigger_name)
                else:
                    lumiP.pt_slope  = 0.0
                    lumiP.pt_offset = 0.0

            #
            # Save new luminosity point: dictionary key is integer luminosity
            #
            self.lumi_points[lumiP.lumi] = lumiP

        return True

    def FindLumiPoint(self, target_lumi):
        if len(self.lumi_points) == 0:
            log.warning('%s has no lumi points' % self.trigger_name)
            return None

        lumis = self.lumi_points.keys()

        #
        # Target lumi point is smaller than lowest luminosity point
        #  
        if target_lumi < min(lumis):
            log.debug('%s: use lowest lumi points: %d' % (self.trigger_name, min(lumis)))
            return self.lumi_points[min(lumis)]

        #
        # Target lumi point is greater than highest luminosity point
        #        
        if target_lumi > max(lumis):
            log.debug('%s: use highest lumi point: %d' % (self.trigger_name, max(lumis)))
            return self.lumi_points[max(lumis)]

        #
        # In this case: lumiA < lumiB < target_lumi < lumiC - use lumiB
        #
        lumis.sort(None, None, True)
        for lumi in lumis:
            log.debug('Lumi step: %d' %lumi)
            if target_lumi >= lumi:
                log.debug('Matched target lumi: %d' %target_lumi)
                return self.lumi_points[lumi]

        log.error('%s: failed to find lumi point: %d' % (self.trigger_name, target_lumi))
        return self.lumi_points[max(lumis)]        

    def GetLevel(self):
        if len(self.trigger_name) < 2:
            log.error('%s: failed to determine level '% self.trigger_name)
            return ''
        return self.trigger_name[0:2]

    def GetTriggerName(self):
        return self.trigger_name

#-----------------------------------------------------------------------------
# Read rulebook and fill trigger rules
#
def readRuleBook(file_path, triggers):

    file = open(file_path, 'r')

    for line in file: 
                
        if line.count('#') or not line.count(';'):
            continue

        rule = TriggerRule()

        if not rule.ReadLine(line):
            log.warning('Failed to parse line: %s' % line.rstrip('\n'))
            continue
        
        if rule.GetTriggerName() in triggers:
            log.warning('Replacing trigger rules for: %s' %rule.GetTriggerName())

        triggers[rule.GetTriggerName()] = rule
        
#-----------------------------------------------------------------------------
# Write XML files with PS, PT and express stream PS - use TriggerRateTool format
#
def writeXML(triggers, xml_path):

    output_text  = '<?xml version="1.0" encoding="ISO-8859-1"?>\n'
    output_text += '<?xml-stylesheet type="text/xsl" href="trigger_rates.xsl"?>\n'
    output_text += '<trigger>\n'

    levels = ["L1", "L2", "EF"]

    for level in levels:
        output_text += '  <level>\n'
        output_text += '    <lvl_name>'+level+'</lvl_name>\n'
        
        for t in triggers.values():

            if level != t.GetLevel():
                continue
            
            output_text += '    <signature>\n'
            output_text += '      <sig_name>%s</sig_name>\n' % t.GetTriggerName()

            lumiP = t.FindLumiPoint(options.lumi)
            if lumiP == None:
                log.error('Missing rule for trigger %s at lumi: %f' % (t.GetTriggerName(), options.lumi))
                continue
            
            if lumiP.HasPlusPS():
                output_text += '      <chain_prescale>+%3.1f</chain_prescale>\n' % int(lumiP.ComputePS(options.lumi))
            else:
                output_text += '      <chain_prescale>%3.1f</chain_prescale>\n'  % int(lumiP.ComputePS(options.lumi))
                
            if (level == 'L2' or level == 'EF'):
                output_text += '      <passthrough>%d</passthrough>\n' % int(lumiP.ComputePT(options.lumi))

            if level == 'EF':
                output_text += '      <express_prescale>%3.1f</express_prescale>\n' % int(lumiP.ComputeES(options.lumi))
                    
            output_text += '    </signature>\n'
        
        output_text += '  </level>\n'        
    output_text += '</trigger>\n'

    xml_file = open(xml_path,'w')
    xml_file.writelines(output_text)

#-----------------------------------------------------------------------------
# Minimal consistency check for trigger rules
#
def checkRules(triggers):

    counts = {}
    disabled_triggers = {}

    for t in triggers.values():
        
        if not t.GetLevel() in counts.keys():
            counts[t.GetLevel()] = 0

        counts[t.GetLevel()] = counts[t.GetLevel()]+1

        lumiP = t.FindLumiPoint(options.lumi)
        
        ps = lumiP.ComputePS(options.lumi)
        if ps < 1.0 and not t.GetTriggerName().count('Cosmics'):
            disabled_triggers[t.GetTriggerName()] = ps
            
    log.warning('This rulebook disables %d non-cosmic trigger(s)' %len(disabled_triggers))
    
    dis_keys = disabled_triggers.keys()
    dis_keys.sort()
    
    for k in dis_keys:
        print '   %s: PS=%d' % (k, disabled_triggers[k])

    for l, c in counts.iteritems():
        log.info('Number of %s triggers %d' %(l, c))

#-----------------------------------------------------------------------------
# Process and validate inputs
#
if len(args) < 1:
    log.warning('Need at least one input XML file')
    sys.exit(1)

if options.lumi == None or options.lumi < 1:
    log.warning('Invalid target luminosity value: '+str(options.lumi))
    sys.exit(1)

#
# Read trigger rules
#
trigger_rules = {}

for file in args:

    if not os.path.isfile(file):
        log.warning('Skip non existing rule file: %s' %file)
        continue

    readRuleBook(file, trigger_rules)

#
# Do minimal consistency check for trigger rules
#
checkRules(trigger_rules)

#
# Write out PS file
#
writeXML(trigger_rules, 'output_xml_%d.xml' % options.lumi)


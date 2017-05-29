#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os
import xml.etree.cElementTree as ET

#-----------------------------------------------------------------------------
# Create and configure option parser
#

from optparse import OptionParser
p = OptionParser(usage="usage: processRuleBook.py <XML files with PS values> --target-lumi=<luminosity in 1E30 units>", version="1.0")

p.add_option('-l', '--target-lumi',  type  ="int",        default=None,  dest="lumi",         help="target luminosity for PS keys")
p.add_option('-i', '--input-lumi',   type  ="int",        default=None,  dest="input_lumi",   help="input luminosity for rates file")
p.add_option('-r', '--rates-file',   type  ="str",        default=None,  dest="rates_file",   help="rates file")
p.add_option('-b', '--bw-file',      type  ="str",        default=None,  dest="bw_file",      help="bandwidth file")
p.add_option('-d', '--debug',        action="store_true", default=False, dest="debug",        help="print debug messages")
p.add_option('--primaries_only',  action="store_true", default=False, dest="primaries_only",        help="primaries only")

(options, args) = p.parse_args()

if options.input_lumi == 999:
    lumi_xml_ref = 10000
elif (options.input_lumi):
    lumi_xml_ref = float(options.input_lumi) # in 1e27 units
if options.rates_file and not options.input_lumi:
    print
    print "ERROR: Enter luminosity corresponding to your rates file or 999 for default"
    print "e.g. -i 10000 or -i 999 for default"
    print
    sys.exit()

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



def read_rates_file(rates_file):

    RATES_XML = ET.parse(rates_file)
    #    print RATES_XML
    RATES_INFO = RATES_XML.getroot()
    
    info_rates = {}
    
    for e in RATES_INFO.findall('level'):
        sigs = e.getiterator("signature")
        for sig in sigs:
            info_rates[sig.find("sig_name").text]=[float(sig.find("rate").text), float(sig.find("prescale").text), sig.find("lower_chain_name").text, float(sig.find("passthrough").text)]

                
    return info_rates

if (options.rates_file):
    read_rates = {}
    read_rates = read_rates_file(options.rates_file)
    #print read_rates



def correct_rates_PT(read_rates):
    
    corrected_rates = {}
    
    for item in read_rates.keys():

        EF_rate = 0
        corrected_rates[item] = [read_rates[item][0] , "None"]

        if read_rates[item][1] <0:
            continue

        if read_rates[item][3] > 0:
            if item.startswith('L2_'):
                for EF_chain in read_rates.keys():
                    if read_rates[EF_chain][2]==item:
                        L1_item = read_rates[item][2]
                        EF_rate = read_rates[EF_chain][0] - (read_rates[L1_item][0] / read_rates[item][3]) 
                        if EF_rate < 0.05 :
                            EF_rate = 0
                        corrected_rates[EF_chain] = [ EF_rate, "L2"]

            elif item.startswith("EF_"):
                L2_chain = read_rates[item][2]
                EF_rate = read_rates[item][0] - read_rates[L2_chain][0] / read_rates[item][3] 
                if EF_rate < 0.05 :
                    EF_rate = 0
                corrected_rates[item] = [ EF_rate, "EF"]
        
    return corrected_rates


if (options.rates_file):
    corrected_rate = {}
    corrected_rate = correct_rates_PT(read_rates)



def read_bw_file(file):

    bw_info = {}

    bw_file = open(file,"r")
    for line in bw_file:
        string_line = (line.strip("\n").strip()).split(":")
        bw_info["EF_"+string_line[0].strip().strip("'")] = string_line[1].strip().strip("'")
        
    return bw_info    

if (options.bw_file):
    read_bw = {}
    read_bw = read_bw_file(options.bw_file)
    #print read_bw

def Calculate_ES_Prescale(chain, PS, PT):

    if chain in read_bw.keys() :
    
        #rate = read_rates[chain][0]
        cor_rate = corrected_rate[chain][0]
        #PT_contribution = rate - cor_rate
        
        target_rate = float(read_bw_file(options.bw_file)[chain])
        
        EF_Chain = chain
        EF_Prescale = read_rates[EF_Chain][1]
        #EF_Rate = read_rates[EF_Chain][0]
        
        L2_Chain = read_rates[EF_Chain][2]
        L2_Prescale = read_rates[L2_Chain][1]
        L2_Rate = read_rates[L2_Chain][0]
        
        L1_Chain = read_rates[L2_Chain][2]
        L1_Prescale = read_rates[L1_Chain][1]
        L1_Rate = read_rates[L1_Chain][0]
        

        total_rate_noPT =  L1_Prescale* L2_Prescale*  EF_Prescale * cor_rate       

        total_PS = PS[L1_Chain] * PS[L2_Chain] *PS[EF_Chain] 
        
        total_rate_PT = 0
        
        if PT[L2_Chain] > 0 :
            total_rate_PT = L1_Rate * L1_Prescale / (PT[L2_Chain] * total_PS)
        
        elif PT[EF_Chain] > 0 :
            total_rate_PT = L2_Rate * L1_Prescale * L2_Prescale / (PT[EF_Chain] * total_PS)


    

        if not ( total_PS==0 or target_rate == 0 ):

            total_rate_noPT_scale = options.lumi * total_rate_noPT / (lumi_xml_ref * total_PS)

            total_rate_PT_scale = options.lumi * total_rate_PT / lumi_xml_ref
            
            total_rate_scale = total_rate_noPT_scale + total_rate_PT_scale
            
            ES_prescale = total_rate_scale / target_rate

#             print "******* %50s " % chain, "  %3.1f\t  "% total_rate_scale ,  "    \t ",total_PS , "   %3.1f\t   " %total_rate_PT , "  \t   ",PT[L2_Chain], "   \t   "  ,PT[EF_Chain],"   \t   ", target_rate,"   \t   ", ES_prescale   , "---------" , cor_rate, rate


            if ES_prescale < 1.:
                ES_prescale = 1.
                
        elif target_rate == 0:
            ES_prescale = 0.
        else:
            ES_prescale = -999.

        if target_rate > 0:
            print  "%50s" % chain,": ES Prescale: %5.1f -- Total Prescale: %8.1f " % (ES_prescale,total_PS) 


    else:
        ES_prescale = -999.


    return ES_prescale



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

    PS={}
    PT={}

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

            PS_ = int(lumiP.ComputePS(options.lumi))
            PS_toUse = 0
            
            if options.primaries_only:
                if PS_>1:
                    PS_toUse = -1 
                else:
                    PS_toUse = PS_
            else:
                PS_toUse = PS_
            print "+++",options.primaries_only, PS_toUse    
            PS[t.GetTriggerName()] = PS_toUse
                
            if lumiP.HasPlusPS():
                output_text += '      <chain_prescale>+%3.1f</chain_prescale>\n' % int(PS_toUse)
            else:
                output_text += '      <chain_prescale>%3.1f</chain_prescale>\n'  % int(PS_toUse)


                
            #print t.GetTriggerName(), int(lumiP.ComputePS(options.lumi))    
            if (level == 'L2' or level == 'EF'):
                PT_value = int(lumiP.ComputePT(options.lumi))
                output_text += '      <passthrough>%d</passthrough>\n' % PT_value
                PT[t.GetTriggerName()]=PT_value

            if level == 'EF':
                if options.bw_file and options.rates_file:
                    if t.GetTriggerName() not in read_bw.keys():
                        #print 'Chain',t.GetTriggerName(),'not in ES, setting ESP to -999'
                        output_text += '      <express_prescale>%3.1f</express_prescale>\n' % int(-999)
                    elif t.GetTriggerName() in read_rates.keys():
                        #print '*** ES prescale from Rates XML for',t.GetTriggerName()
                        output_text += '      <express_prescale>%3.1f</express_prescale>\n' % Calculate_ES_Prescale(t.GetTriggerName(),PS,PT)
                    else:
                        print '===> WARNING - Chain == %s \t \t == not in Rates XML, reading RuleBook ES Prescale instead' % t.GetTriggerName() 
                        output_text += '      <express_prescale>%3.1f</express_prescale>\n' % int(lumiP.ComputeES(options.lumi))
                else:
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

    list_of_triggers = []
    
    for t in triggers.values():

        list_of_triggers.append(t.GetTriggerName())
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


    if options.bw_file and options.rates_file:

        print
        print "***> Checking ES BW file and RuleBook for consistency..."

        got_inconsistency = False
        for ES_t in read_bw.keys():
            if ES_t not in list_of_triggers:
                print "===> WARNING - Chain", ES_t, "defined in bw file but not in RuleBook"
                got_inconsistency=True
        if not got_inconsistency:
            print "***> No inconsistency found"
        print "<***"
        print

if __name__ == "__main__":
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
    
    
    print
    print
    


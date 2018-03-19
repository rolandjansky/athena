# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#/usr/bin/env python
import os, sys
import xml.etree.cElementTree as ET
import logging

logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s %(levelname)s %(message)s')
log = logging.getLogger('addPS.py')

from optparse import OptionParser

op = OptionParser(usage = 'Usage: %prog [options] (all options required)')
op.add_option('-r', '--rules', action='store', type='string',
              dest='rules', default='', 
              help='Prescale rules file')
op.add_option('--l1xml', action='store', type='string', dest='l1xml',
              help='L1 XML file (in full format)')
op.add_option('--hltxml', action='store', type='string', dest='hltxml',
              help='HLT XML file (in full format)')
op.add_option('--target-lumi', action='store', type='float',
              dest='target_lumi',
              help='Target luminosity of the new prescale')
op.add_option('--current-lumi', action='store', type='float',
              dest='current_lumi',
              help='Current luminosity for which the input XML file was used')
op.add_option('--dump-default-rules', action='store_true', default=False, 
              dest='dump_default_rules',
              help='Dumps the default rules (no prescale up to e34) to default_rules.py (optional)')
op.add_option('-o', '--output', action='store', type='string', dest='output',
              help='Output file with new prescles (input to cost monitoring)')
op.add_option('--standby', action='store_true', default=False, 
              dest='standby',
              help='sets prescale for stand-by keys (L1)')
op.add_option('--vdM', action='store_true', default=False, 
              dest='vdM',
              help='sets prescale for van-der-Meerscan keys (HLT)')
op.add_option('--CaloNoise33', action='store_true', default=False, 
              dest='CaloNoise33',
              help='sets prescale for L1Calo noise setting of 3/3 (L1)')
op.add_option('--nB_paired', action='store', type='int', 
              dest='nB_paired', default=1,
              help='number of paired bunches')
op.add_option('--nB_unpaired', action='store', type='int', 
              dest='nB_unpaired', default=1,
              help='number of unpaired bunches')
op.add_option('--emergency', action='store_true', default=False, 
              dest='emergency',
              help='makes prescale sets w/o checks (USE WITH CARE)')

class PSRule:
    kScaleWithLumi = 0
    kFixedPrescale = 1
    def __init__(self, type, param_tuple):
        self.type = type
        self.min_lumi = 1000000
        self.factor = 1.0
        self.value = 1.
        if self.type == PSRule.kScaleWithLumi:
            self.min_lumi = param_tuple[0]
            self.factor = param_tuple[1]
            self.value = param_tuple[2] # PS value up to the min. luminosity
            self.scd_min_lumi = param_tuple[3]
            self.scd_factor = param_tuple[4]
            self.scd_value = param_tuple[5]
        elif self.type == PSRule.kFixedPrescale:
            self.value = param_tuple[0]
    def newPS(self, target_lumi, current_lumi, ps):
        if self.type == PSRule.kFixedPrescale:
            return ps
        elif self.type == PSRule.kScaleWithLumi:
            #get the factor for the standby set (1 id default)
            standby_fac = isStandby(self.name)

            #check for special values    
            specialPS = isSpecialCase(self.name, target_lumi)
            if (specialPS != -999.9): #-999.9 is default
                if (self.name.find("L1_")==0):
                    return int(specialPS*standby_fac)
                else:
                    return specialPS

            #point out inconsistencies of starting prescales + rules
            if current_lumi < self.min_lumi and ps > self.value and self.value >0:
                ml = current_lumi/ps
                log.warn('Prescale of %3.1f is already applied at %3.1f while min. luminosity is set to %3.1f' % \
                         (ps, current_lumi, self.min_lumi))
                log.warn("==> Update the rule: e.g. '%s': scaleWithLumi(%3.3f, %3.1f) or scaleWithLumi(*, *, %d)" % \
                         (self.name, ml, 1, ps))

            if target_lumi < self.min_lumi:
                if ps < self.value and self.name.find("ES:EF")!=0 and self.factor!=0:
                    if ps < 0:
                        log.info('chain %s is DISABLED online with PS %3.1f and has a value of %3.1f ' % (self.name, ps, self.value))
                        log.warn("==> Update the rule or original PS set of %s" % \
                                 (self.name))
                        if (opts.emergency):
                            log.warn("==> EMERGENCY mode, continue for now")
                        else:
                            log.warn("==> EXIT")
                            #sys.exit(1)
                    else: 
                        log.warn('Prescale %3.1f of %s is lower than minimal value of %3.1f ' % \
                                 (ps, self.name, self.value))
                        log.warn("==> Update the rule or original PS set of %s" % \
                                 (self.name))
                        if (opts.emergency):
                            log.warn("==> EMERGENCY mode, continue for now")
                        else:
                            log.warn("==> EXIT")
                            #sys.exit(1)
                return ps*standby_fac

            #here we are above the min-lumi for this chain
            #flat prescale above min lumi (if below, do not get here)
            if self.factor == 0:
                return self.value

            r = target_lumi/current_lumi

            #ret_prescale = ps*r*self.factor
            ret_prescale = r*self.factor
            #y-axis intercept < 0    
            if (self.value < 0):
                ret_prescale = ret_prescale + self.value/current_lumi
            
            #if calc prescale < minimal value
            if (max(1.,ret_prescale*ret_prescale) < self.value*self.value and (self.factor > 0) and (self.value > 0) and self.name.find("ES:EF")!=0):
                log.warn('!!Prescale of %3.1f for lumi %3.1f is smaller than minimal value of %3.1f given in rule for chain %s' % \
                         (ret_prescale, target_lumi, self.value, self.name))
                log.warn("==> Update the rule or the original prescal set, we will exit")
                #sys.exit(1)
                    
            #check wether
            #if (ps/current_lumi != self.factor)
            test_prescale = self.factor*current_lumi
            if (self.value < 0):
                test_prescale = test_prescale + self.value/current_lumi
            if (abs(max(test_prescale,1)-ps)>0.5 and self.name.find("ES:EF")!=0 and isSpecialCase(self.name, current_lumi)!=-999.9):
                if (self.name.find("L1_MBTS_1")!=0 and self.name.find("L1_ZDC")!=0):
                    log.warn('!!!Prescale of %3.1f for lumi %3.1f is not what we would have gotten with the rule for chain %s, calc instead: %3.1f' % \
                                          (ps, current_lumi, self.name, test_prescale))
                    log.warn("==> Update the rule or the original prescal set, we will exit")
                    #sys.exit(1)                
                
            if target_lumi > self.scd_min_lumi:
                #make this completely independent
                #get PS at snd_min_lumi
                #scd_ps = self.scd_min_lumi/current_lumi*self.factor
                if (self.scd_factor==0):
                    ret_prescale = self.scd_value
                else:
                    ret_prescale = self.scd_factor*target_lumi+self.scd_value
                
            if ((self.name).find('L1_') == 0):
                return int(max(1,ret_prescale*standby_fac))
            else:
                if (ret_prescale>0):
                    return max(1.,ret_prescale*standby_fac)
                else:
                    return min(-1.,ret_prescale*standby_fac)
                
def fixedPrescale(value):
    """Prescaling rule where prescale should scale with luminosity starting
from the specified minimum luminosity"""
    return PSRule(PSRule.kFixedPrescale, (value))

def scaleWithLumi(lumi_in_E27, factor=1.0, default_ps=1, scd_lumi=1000000, scd_factor=0, scd_value=0):
    """Prescaling rule where prescale should scale with luminosity starting
from the specified minimum luminosity"""
    return PSRule(PSRule.kScaleWithLumi, (lumi_in_E27, factor, default_ps, scd_lumi, scd_factor, scd_value))

def getPrescaleMaps(l1xml, hltxml):
    name2id = {}
    id2ps = {}
    l1map, l2map, efmap, esmap = {}, {}, {}, {}
    # L1
    doc = ET.parse(l1xml)
    l1top = doc.getroot()
    for e in l1top.findall('TriggerMenu/TriggerItem'):
        name = e.get('name')
        id = int(e.get('ctpid'))
        name2id[name] = id
    for e in l1top.findall('PrescaleSet/Prescale'):
        id = int(e.get('ctpid'))
        ps = int(e.text)
        id2ps[id] = ps
    for name, id in name2id.iteritems():
        l1map[name] = id2ps[id]
    # HLT
    doc = ET.parse(hltxml)
    hlttop = doc.getroot()
    for e in hlttop.findall('CHAIN_LIST/CHAIN'):
        name = e.get('chain_name')
        ps = float(e.get('prescale'))
        pt = float(e.get('pass_through'))
        level = e.get('level')
        # print (name, ps, pt, level)
        if level == 'L2': l2map[name] = (ps, pt)
        elif level == 'EF':
            efmap[name] = (ps, pt)
            for st in e.findall('STREAMTAG_LIST/STREAMTAG'):
                if st.get('stream') == 'express':
                    esmap[name] = int(st.get('prescale'))
    #
    return (l1map, l2map, efmap, esmap)

def toXml(psmaps, outxml):
    l1map = psmaps[0]
    l2map = psmaps[1]
    efmap = psmaps[2]
    esmap = psmaps[3]
    def writeSig(f, name, ps, pt):
        f.write('    <signature>\n')
        f.write('      <sig_name>%s</sig_name>\n' % name)
        if (name.find("L1")==0):
            f.write('      <chain_prescale>%d</chain_prescale>\n' % ps)
        else:
            f.write('      <chain_prescale>%3.1f</chain_prescale>\n' % ps)
        f.write('      <passthrough>%d</passthrough>\n' % pt)
        f.write('    </signature>\n')
#        f.write('      <sig_name>%s</sig_name>' % name)
#        f.write('      <chain_prescale>%d</chain_prescale>\n' % ps)
    def writeSigExpress(f, name, ps, pt, es):
        f.write('    <signature>\n')
        f.write('      <sig_name>%s</sig_name>\n' % name)
        f.write('      <chain_prescale>%3.1f</chain_prescale>\n' % ps)
        f.write('      <express_prescale>%3.1f</express_prescale>\n' % es)
        f.write('      <passthrough>%d</passthrough>\n' % pt)
        f.write('    </signature>\n')
#        f.write('      <sig_name>%s</sig_name>' % name)
#        f.write('      <chain_prescale>%d</chain_prescale>' % ps)
#        f.write('      <express_prescale>%d</express_prescale>\n' % es)

    fout = open(outxml, 'w')
    fout.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
    fout.write('<?xml-stylesheet type="text/xsl" href="trigger_rates.xsl"?>\n')
    fout.write('<trigger>\n')
    fout.write('  <level>\n')
    # L1
    fout.write('    <lvl_name>L1</lvl_name>\n')
    for name, ps in l1map.iteritems():
        writeSig(fout, name, ps, 0)
    fout.write('  </level>\n')
    # L2
    fout.write('  <level>\n')
    fout.write('    <lvl_name>L2</lvl_name>\n')
    for name, pspt in l2map.iteritems():
        writeSig(fout, name, pspt[0], pspt[1])
    fout.write('  </level>\n')
    # EF
    fout.write('  <level>\n')
    fout.write('    <lvl_name>EF</lvl_name>\n')
    for name, pspt in efmap.iteritems():
        #look for express prescale
        if (name in esmap):
            writeSigExpress(fout, name,  pspt[0], pspt[1], efmap[name][0])
        else:
            writeSig(fout, name,  pspt[0], pspt[1])
    fout.write('  </level>\n')
    # EF
    #fout.write('  <level>\n')
    #fout.write('    <lvl_name>ES</lvl_name>\n')
    #for name, ps in esmap.iteritems():
    #    writeSig(fout, name, ps, 0)
    #    # print '%04d : %s' % (ps, name)
    #fout.write('  </level>\n')
    fout.write('</trigger>\n')

def modifyPS(psmaps, rules, target_lumi, current_lumi):
    l1map = psmaps[0]
    l2map = psmaps[1]
    efmap = psmaps[2]
    esmap = psmaps[3]
    for (k, rule) in rules.iteritems():
        if k.find('L1_') == 0:
            if k in l1map.keys():
                l1map[k] = rule.newPS(target_lumi,current_lumi,l1map[k])
        elif k.find('L2_') == 0:
            if k in l2map.keys():
                pspt = (rule.newPS(target_lumi,current_lumi,l2map[k][0]),
                        l2map[k][1])
                l2map[k] = pspt
        elif k.find('ES:EF_') == 0:
            ch = k[3:]
            if ch in esmap.keys():
                esmap[ch] = rule.newPS(target_lumi,current_lumi,esmap[ch])
        elif k.find('EF_') == 0:
            if k in efmap.keys():
                pspt = (rule.newPS(target_lumi,current_lumi,efmap[k][0]),
                        efmap[k][1])
                efmap[k] = pspt
    pass

def readRules(filename):
    if len(filename) == 0: return {}
    if os.path.exists(filename):
        execfile(filename)
        if 'rules' in locals().keys():
            rs = locals()['rules']
            for k, v in rs.iteritems():
                v.name = k
            return rs
        else:
            log.error('No rules={...} specified in the rules file')
    else:
        log.error('Cannot find the rules file %s' % filename)
    return {}

def dumpDefaultRules(psmaps):
    def go(m, f, label):
        keys = m.keys()
        keys.sort()
        f.write('    # %s\n' % label)
        for k in keys:
            s = "'" + k + "'"
            f.write("    %-30s: scaleWithLumi(10000000, 1)\n" % s)
    f = open('default_rules.py', 'w')
    f.write('rules = {\n')
    go(psmaps[0], f, 'L1')
    go(psmaps[1], f, 'L2')
    go(psmaps[2], f, 'EF')
    go(psmaps[3], f, 'ES')
    f.write('    }\n')


#allocation: 
# MinBias 20
# gamma 30
# jets 25
# tau 20
# MET 15
# muon 30+10
def isSpecialCase(name, target_lumi): 
    #not sure about scaling with #nB ...
    if (name.find("L1_RD1_FILLED")==0): return 1400*opts.nB_paired

    if (name.find("L1_RD0_FIRSTEMPTY")==0): return 4200*opts.nB_paired

    if (name.find("L1_RD0_EMPTY")==0):
        empty_fac = (3564.-opts.nB_paired*4-opts.nB_unpaired)/(3564.-6.);
        prescale = int(500000*empty_fac/1000.)*1000
        return prescale

    if (name.find("L1_RD0_UNPAIRED")==0):
        return 14*opts.nB_unpaired

    if (name.find("L1_RD0_FILLED")==0):
        #L1 seed Min-Bias HLT chains, scale propto MB-PS (???)
        lumi_fac = isSpecialCase("L1_MBTS_1",target_lumi)
        if (lumi_fac<0):
            lumi_fac = 10*target_lumi/current_lumi
        return max(1,int(opts.nB_paired*lumi_fac))

    if (name.find("L1_MBTS_1")==0 and name.find("L1_MBTS_1_")<0):
        if target_lumi == 4 : return 2
        if target_lumi == 6 : return 4
        if target_lumi == 8 : return 5
        if target_lumi == 10 : return 7
        if target_lumi == 15 : return 11
        if target_lumi == 20 : return 16
        if target_lumi == 30 : return 20
        if target_lumi == 40 : return 30
        if target_lumi == 60 : return 55
        if target_lumi == 80 : return 70
        if target_lumi == 100 : return 200

    if (name.find("L1_ZDC")==0 and name.find("L1_ZDC_")<0):
        if target_lumi == 4 : return 2
        if target_lumi == 6 : return 4
        if target_lumi == 8 : return 5
        if target_lumi == 10 : return 7
        if target_lumi == 15 : return 11
        if target_lumi == 20 : return 16
        if target_lumi == 30 : return 20
        if target_lumi == 40 : return 30
        if target_lumi == 60 : return 55
        if target_lumi == 80 : return 70
        if target_lumi == 100 : return 200

    if (name.find("L1_TE10")==0 and name.find("L1_TE10_")<0 and name.find("L1_TE100")<0 ):
        if target_lumi == 4 : return 1
        if target_lumi == 6 : return 2
        if target_lumi == 8 : return 2
#        if target_lumi == 10 : return 3
#        if target_lumi == 20 : return 8
        if target_lumi == 100 : return 12

    if (name.find("L1_EM2")==0 and name.find("L1_EM2_")<0):
        if target_lumi == 100 : return 2
        if target_lumi == 150 : return 10
        if target_lumi == 200 : return 100
        if target_lumi == 300 : return 150
        if target_lumi == 400 : return 200

    if (name.find("L1_J5")==0 and name.find("L1_J5_")<0 and name.find("L1_J55")<0):
        if target_lumi == 200 : return 2
        if target_lumi == 300 : return 5
        if target_lumi == 400 : return 6

    if (name.find("L1_TAU5")==0 and name.find("L1_TAU5_")<0):
        if target_lumi == 300 : return 2
        if target_lumi == 400 : return 4

    if (name.find("L1_TE30")==0 and name.find("L1_TE30_")<0):
        if target_lumi == 300 : return 2
        if target_lumi == 400 : return 4

    if (name.find("L1_EM3")==0 and name.find("L1_EM3_")<0):
        if target_lumi == 200 : return 2
        if target_lumi == 300 : return 6
        if target_lumi == 400 : return 6

    if (name.find("L1_EM4")==0 and name.find("L1_EM4_")<0):
        if target_lumi == 300 : return 15
        if target_lumi == 400 : return 20

    if (name.find("L1_2EM2")==0 and name.find("L1_2EM2_")<0):
        if target_lumi == 300 : return 4
        if target_lumi == 400 : return 5

    if (name.find("ES:EF_L1ItemStramer_L1_EM2")==0):
        if target_lumi == 100 : return 300
        if target_lumi == 150 : return 90
        if target_lumi >= 200 : return 10

    if (name.find("ES:EF_L1ItemStramer_L1_J5")==0):
        if target_lumi == 200 : return 40

    # get this to 20 hz max
    if (name.find("L2_InDetMon_FS")==0):
        if (isSpecialCase("L1_MBTS_1",target_lumi) != -999.9): #-999.9 is default
            inrate = 68*target_lumi/isSpecialCase("L1_MBTS_1",target_lumi)
        else:
            inrate = 68*target_lumi/(10*target_lumi)
        #        print 'L2_InDetMin_FS', inrate, ' ',max(1,int(20./inrate))
        return max(1,int(inrate/20.))

    #have special cases here as MBTS_2 rate is 1.8 kHz (1.2 above this)
    if (target_lumi == 30):
        if (name.find("L2_vtxbeamspot_activeTE")==0 and not (opts.vdM)):
            return 75
        if (name.find("L2_trk2_FullScan")==0):
            return 200
        if (name.find("L2_mu4_MSonly_MB2_noL2_EFFS")==0):
            return 1.5

    # get this to 10% of MBTS_1 max
    if (name.find("L2_mbSpTrkMh_MB2")==0):
        if (target_lumi == 20 or target_lumi == 30):
            return isSpecialCase("L1_MBTS_1",target_lumi)/10.
        if (target_lumi == 40 or target_lumi == 60 or target_lumi == 80):
            return isSpecialCase("L1_MBTS_1",target_lumi)/(target_lumi/2.)
        if (target_lumi >= 100):
            return 0.9/0.05

    if (opts.vdM):
        if (name.find("L2_vtxbeamspot_activeTE")==0):
            if (target_lumi >= 20):
                return 2 #
            elif (target_lumi == 15):
                return 1.5 #
            else:
                return 1
        #disable other chains from MBTS_2 that we do not have event stripping @high rates
        if (name.find("L2_mbSpTrkMh_MB2")==0):
            return -1
#        if (name.find("L2_mu4_MSOnly_MB2_noL2_EFFS")==0):
#            return -1

    if (opts.CaloNoise33):
        if (name.find("L1_TAU5_EMPTY")==0) or (name.find("L1_J5_EMPTY")==0):
            return 100
        if (name.find("L1_TE10")==0 and name.find("TE10_")<0 and name.find("TE100")<0 ):
            return max(1000,200*target_lumi)

    return -999.9

def isStandby(name): 
    if (opts.standby):
        if (name.find("L1_RD0_FILLED")==0): return 1400
        if (name.find("L1_RD0_UNPAIRED")==0): return 100
        if (name.find("L1_RD0_EMPTY")==0): return 10
        if (name.find("L1_MBTS_2")==0) and (name.find("L1_MBTS_2_")<0): return 400
    return 1
        
if __name__ == '__main__':
    (opts, args) = op.parse_args()
    l1xml = opts.l1xml
    hltxml = opts.hltxml
    rules_file = opts.rules
    fname = opts.output
    target_lumi = opts.target_lumi
    current_lumi = opts.current_lumi

    print 'start now in addPS'
    #warn if target lumi < current lumi: don't extrapolate down, quit
    r = target_lumi/current_lumi
    if ( r < 0.1):
        print 'Target lumi below current lumi: cannot make prescale set!'
        sys.exit(1)
                           
    #
    if l1xml==None or hltxml==None or fname==None:
        op.print_help()
        print 'All options must be specified!'
        sys.exit(1)
    #
    rules = readRules(rules_file)
    #
    psmaps = getPrescaleMaps(l1xml, hltxml)
    if len(rules) > 0:
        log.info('Modify rules based on %s' % rules_file)
        modifyPS(psmaps, rules, target_lumi, current_lumi)
    doc = toXml(psmaps, fname)
    if opts.dump_default_rules:
        dumpDefaultRules(psmaps)
    

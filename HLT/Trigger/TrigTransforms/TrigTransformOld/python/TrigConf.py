#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: Alessandro Di Mattia <dimattia@mail.cern.ch>
# Module to get the trigger configuration from the cool database.

import sys,re,os
from TrigTransformOld import RunWithEnv
import xml.dom.minidom

from subprocess import *

class TrigConfiguration:
        def __init__(self,coolstr,fullHLTps,L1MenuFile=''):
                try:
                        release = coolstr.split('release')[1].split('smk')[0]
                        smk     = coolstr.split('smk')[1].split('hltps')[0]
                        hltps   = coolstr.split('hltps')[1].split('lvl1ps')[0]
                        lvl1ps  = coolstr.split('lvl1ps')[1]
                except Exception, x:
                        print 'Exception raised in TrigConfiguration: %s' % x
                        self.release       = None
                        self.smk           = None
                        self.hltps         = None
                        self.lvl1ps        = None
                        self.lvl1ps_min_lb = None
                        self.lvl1ps_max_lb = None
                        self.hltps_min_lb  = None
                        self.hltps_max_lb  = None
                        self.full_hltps    = None
                        self.bits_to_item  = {}
                        return

                self.release       = release.split()[0]
                self.smk           = smk.split()[0]
                self.hltps         = self.parse_ps_values(hltps)
                self.lvl1ps        = self.parse_ps_values(lvl1ps)
                self.lvl1ps_min_lb = self.parse_ps_range_min(lvl1ps)
                self.lvl1ps_max_lb = self.parse_ps_range_max(lvl1ps)
                self.hltps_min_lb  = self.parse_ps_range_min(hltps)
                self.hltps_max_lb  = self.parse_ps_range_max(hltps)
                self.full_hltps    = fullHLTps

                if self.release  =='unknown' or self.release  =='': 
                        self.release = None
                if self.smk      =='unknown' or self.smk      =='': 
                        self.smk     = None
                if self.lvl1ps[0]=='unknown' or self.lvl1ps[0]=='': 
                        self.lvl1ps  = None
                if self.hltps[0] =='unknown' or self.hltps[0] =='': 
                        self.hltps   = None
                matchps = re.compile('^(([0-9]+u[0-9]+)c*)+$')
                if not matchps.match(self.full_hltps): 
                        self.full_hltps = None
                self.bits_to_items = self.get_lvl1_configuration(L1MenuFile)



        def get_lvl1_configuration(self,L1MenuFile):
                if L1MenuFile!='':
                        lvl1_xml = xml.dom.minidom.parse(L1MenuFile)
                        bits_to_items = {}
                        for i in lvl1_xml.getElementsByTagName('TriggerItem'):
                                name = i.getAttribute('name')
                                bit  = int(i.getAttribute('ctpid'))
                                bits_to_items[bit] = name
                        return bits_to_items

                return {}


        def parse_ps_range_min(self,string=""):
                ps_sets = string.split(')')
                ps_range_min = []
                for i in range(len(ps_sets)-1):
                        t = ps_sets[i]
                        try:
                                ran = t.split('(')[1].split()[0]
                                low = ran.split('-')[0]
                                if low=='': low = '0'
                                ps_range_min.append(low)
                        except Exception, x:
                                print 'Exception in parse_ps_range_min: %s' % x
                                ps_range_min.append('0')
                return ps_range_min

        def parse_ps_range_max(self,string=""):
                ps_sets = string.split(')')
                ps_range_max = []
                for i in range(len(ps_sets)-1):
                        t = ps_sets[i]
                        try:
                                ran  = t.split('(')[1].split()[0]
                                high = ran.split('-')[1]
                                if high=='': high = '9999'
                                ps_range_max.append(high)
                        except Exception, x:
                                print 'Exception in parse_ps_range_max: %s' % x
                                ps_range_max.append('9999')
                return ps_range_max

        def parse_ps_values(self,string=""):
                ps_sets = string.split(')')
                ps_range_value = []
                for t in ps_sets:
                        try:
                                value = t.split('(')[0].split()[0]
                                ps_range_value.append(value)
                        except Exception, x:
                                print 'Exception (harmless?) in parse_ps_values: %s' % x                                
                return ps_range_value
                
        def parse_ps(self,dstr=""):
                ps_sets = dstr.split(')')
                if len(ps_sets)==1: return [ps_sets[0].split()[0]]
                
                ps_list = []#[ps_sets[0].split('(')[0].split()[0]]
                for t in ps_sets:
                        if len(t):
                                key = t.split('(')[0].split()[0]
                                ran = t.split('(')[1].split()[0]
                                low = int(ran.split('-')[0])
                                try: 
                                        high = int(ran.split('-')[1])
                                        ps_list += [key for l in range(low,high)]
                                        if(low<high): ps_list += [key]
                                except Exception, x:
                                        print 'Exception (harmless?) in parse_ps: %s' % x                                
                                        ps_list.append(key)

                return ps_list

        def str(self):
                rel        = "  Release       : %s\n" % self.release
                smk        = "  SMK           : %s\n" % self.smk
                ps1        = "  LVL1 PS       : %s\n" % self.lvl1ps
                ps1_min_lb = "  LVL1 PS MIN LB: %s\n" % self.lvl1ps_min_lb
                ps1_max_lb = "  LVL1 PS MAX LB: %s\n" % self.lvl1ps_max_lb
                ps2        = "  HLT PS        : %s\n" % self.hltps
                ps2_min_lb = "  HLT PS MIN LB : %s\n" % self.hltps_min_lb
                ps2_max_lb = "  HLT PS MAX LB : %s\n" % self.hltps_max_lb
                fullHLTps  = "  HLT full PS   : %s"   % self.full_hltps
                out = "Trigger configuration:\n%s%s%s%s%s%s%s%s%s" % \
        (rel,smk,ps1,ps1_min_lb,ps1_max_lb,ps2,ps2_min_lb,ps2_max_lb,fullHLTps)
                return out

        def Release(self):
                if not self.release: 
                        raise 'TrigConfiguration: release undefined!'
                return self.release

        def SMK(self):
                if not self.smk : 
                        raise 'TrigConfiguration: super master key undefined!'
                return self.smk
        
        def LVL1_PS(self,lumi_block=0):
                if not self.lvl1ps: 
                        raise 'TrigConfiguration: Lvl1 pre-scale sets undefined!'
                if lumi_block<=0: return self.lvl1ps[0]
                pos = 0
                for low,high in zip(self.lvl1ps_min_lb,self.lvl1ps_max_lb):
                        if lumi_block>=int(low) and lumi_block<=int(high): break
                        pos += 1
                try:
                        return self.lvl1ps[pos]
                except:
                        return self.lvl1ps[len(self.lvl1ps)]
                
        def HLT_PS(self,lumi_block=0):
                if not self.hltps: 
                        raise 'TrigConfiguration: HLT pre-scale sets undefined!'
                if lumi_block<=0: return self.hltps[0]
                pos = 0
                for low,high in zip(self.hltps_min_lb,self.hltps_max_lb):
                        if lumi_block>=int(low) and lumi_block<=int(high): break
                        pos += 1
                try:
                        return self.hltps[pos]
                except:
                        return self.hltps[len(self.hltps)]

        def FULL_HLT_PS(self):
                return self.full_hltps

        def L1ItemName(self,bit):
                if self.bits_to_items.has_key(bit):
                        return self.bits_to_items[bit]
                return ''


def bad_arguments(*args):
        print "TrigConf accepts 1 arguments plus two options , the run environment and the verbosity"
        print "                usage:TrigConf <run_number>  [release_conf]    [verbose]"
        print "                defaults:                    AtlasHLT,15.5.1     False"
        print "                examples: TrigConf.py 131202"
        print "                          TrigConf.get(131202,\"AtlasHLT,15.5.1\")"
        print
        print "arguments given: ", args
        sys.exit(1)
        

def get(run_number=0,*opts):

        #try to get optional parameters
        try:
                release = opts[0]
        except:
                release = ""

        try:
                verbose = opts[1]
        except:
                verbose = False


        # get the standard Cool configuration string
        L1MenuFile = ''
        LVL1xml = [f for f in os.listdir('.') if re.compile('L1Menu_').search(f)]
        for f in LVL1xml: os.remove(f)
        cmd = "AtlCoolTrigger.py -r " + str(run_number) + " -x"

        if release=="":
                proc = Popen([cmd],shell=True,stdout=PIPE,stderr=PIPE)
                try:
                        (stdout, stderr) = proc.communicate()
                except Exception, x:
                        print 'Exception raised running %s: %s' % (cmd. x)
                        print stderr

                code = proc.returncode
                
        else:
                (code, stdout, stderr) = RunWithEnv.run(release, cmd)
        
        if verbose: print stdout

        LVL1xml = [f for f in os.listdir('.') if re.compile('L1Menu_').search(f)]
        if len(LVL1xml)!=0: L1MenuFile = LVL1xml[0]
        #if code!=0: sys.exit(1)



        # get the patched Cool configuration
        cmd = "AtlCoolTrigger.py -r " + str(run_number)        + " -p hlt"
        
        if release=="":
                proc = Popen([cmd],shell=True,stdout=PIPE,stderr=PIPE)
                try:
                        (fullhltps, stderr) = proc.communicate()
                except Exception, x:
                        print 'Exception running command %s: %s' % (cmd, x)
                        print stderr
                        fullhltps = ''
                        pass

                code = proc.returncode
                
        else:
                (code, fullhltps, stderr) = RunWithEnv.run(release, cmd)
        
        if verbose: print fullhltps

        info = [l for l in stdout.split('\n') 
                  if l.find('release')                !=-1 and 
                     l.find(str(run_number))        !=-1 and
                     l.find('smk')                !=-1]

        if len(info)==0: keys = ''
        else: keys = info[0]
        
        #if len(fullhltps.split('\n'))>1:
        #    fullhltps = fullhltps.split('\n')[2]
            #fullhltps = fullhltps[fullhltps.find('AtlCoolTrigger.py'):]

        m = re.search('(([0-9]+u[0-9]+)c*)+',fullhltps)

        if m: full_hltps = fullhltps[m.start():m.end()]
        else: full_hltps = ''

        return TrigConfiguration(keys,full_hltps,L1MenuFile)



if __name__ == '__main__':        
        try:
                args = sys.argv[1:]
                assert(len(args)>0)
                assert(len(args)<4)
                num  = len(args) - 1

                if num==2:        out = get(args[0],args[1],args[2])
                elif num==1:        out = get(args[0],args[1])
                else:                 out = get(args[0]) 
        
        except (AssertionError, IndexError):
                bad_arguments(sys.argv[1:])
                sys.exit(1)
                
        except Exception, x:
                print "Execution error, is ATLAS environment set? Exception: %s" % x
                out = TrigConfiguration("","","")
        
        print out.str()

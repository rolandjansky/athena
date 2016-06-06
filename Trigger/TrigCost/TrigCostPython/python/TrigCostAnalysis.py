# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Elliot Lipeles, Univ. of Pennsylvania (2011)
# Tae Min Hong, Univ. of Pennsylvania (2011, 2012)
#
# This file defines general functions & objects used throughout the
# TrigCostPython package.  Some main classes are
#
# - CostChain
# - CostResult
# - CostResultCollection
# - LumiBlock
# - LumiBlockSet
# - others...
#

#print 'Hello world TrigCostAnalysis.py'

import string
import re
import math
#print '\tLoaded standard packages'

from  TrigCostPython import TrigCostTRP
#print '\tLoaded special TrigCostPython.TrigCostTRP  package'
#print '\tDone prelim import for TrigCostAnalysis.py'

#Method to search files in a bash variable.
#-------------------------------------------------------------------------#
def search_file(filename, search_path):
   """Given a search path, find file
   """

   import os
   from string import split

   file_found = 0
   paths = split(search_path, os.pathsep)
   for path in paths:
      if os.path.exists(os.path.join(path, filename)):
          file_found = 1
          break
   if file_found:
      return os.path.abspath(os.path.join(path, filename))
   else:
      return None

   if __name__ == '___main__':
      search_path = '/bin' + os.pathsep + '/usr/bin'  # ; on windows, : on unix
      find_file = search_file('ls',search_path)
   if find_file:
      print "File found at %s" % find_file
   else:
      print "File not found"

#--------------------------------------------------------------------------#


#==========================================================================
tokenize = re.compile(r'(\d+)|(\D+)').findall
def natural_sortkey(string):
    """ Sort so that EM3 & 2EM3 are juxtaposed """
    st3 = string[3:]    # Only consider after L1_, L2_, EF_
    tok = tokenize(st3) # Break up into list of pairs ('123','abc')
    tup = tuple(int(num) if num else alpha.lower() for num, alpha in tok)
    lst = filter(None, tup) # Get rid of empty entries
    return lst

#==========================================================================
def digits(x):
    """ Return amount of digits of x. """
    if x == 0: return 0
    return int(math.floor(math.log10(math.fabs(x))) + 1)

#==========================================================================
def sigfig(x, off=4):
    """ Return amount of decimals of x. """
    d = digits(x)
    if d > 0: d -= off + 1 # one extra for the decimal point
    else: d -= off
    return str( int(math.fabs(d)) ) if x != 0 else '0'

# TimM
# Remove . and - 
def TRPNameStrip(inputStr):
    outputStr = inputStr.replace("-", "_")
    outputStr = outputStr.replace(".", "_")
    outputStr = outputStr.replace(":", "_")
    return outputStr

class VarNotAvailable(Exception):
    pass

#---------------------------------------------------------------------
#
# Main chain class to store trigger configuration and trigger rates
#
#---------------------------------------------------------------------
class CostChain:
    def __init__(self):
        self.name      = ""
        self.level     = ""
        self.isgroup   = False
        self.fill      = -1
        self.run       = -1
        self.lb        = -1
        self.counter   = -1

        self.SetPrescale(0.)
        self.SetTAVCnt(0)
        self.SetTBPCnt(0)
        self.SetTAPCnt(0)
        self.SetTAVRate(0.)
        self.SetTBPRate(0.)
        self.SetTAPRate(0.)
        self.SetTBPXsec(0.)
        self.SetTAPXsec(0.)
        self.SetTAVXsec(0.)

    def Print(self, mode='noformat'):
        rate = self.GetRate()
        ps = self.GetPrescale()
        if mode == 'noformat':
            printstr = '%s cntr=%d [fill=%d run=%d/%d] rate=%.'+sigfig(rate)+'f ps=%.'+sigfig(ps)+'f tbp/tap/tav=%d/%d/%d'
            print (printstr % (self.GetName(), self.GetCounter(), self.GetFill(), self.GetRun(), self.GetLb(),
                               rate, ps, self.GetTBPCnt(), self.GetTAPCnt(), self.GetTAVCnt(),
                              ))
        else:
            printstr = '%s cntr=%4d [fill=%5s run=%10s] ' \
                   + 'rate=%10.'+sigfig(rate)+'f ' \
                   + 'ps=%10.'+sigfig(ps)+'f ' \
                   + 'tbp/tap/tav=%30s'
            print (printstr % (self.GetName(), self.GetCounter(),
                               str(self.GetFill()), '%d/%d' % (self.GetRun(), self.GetLb()),
                               rate, ps,
                               '%d/%d/%d' % (self.GetTBPCnt(), self.GetTAPCnt(), self.GetTAVCnt()),
                              ))

#	                + 'rav=%f rbp=%f rap=%f ' \
#	                + 'xav=%f xbp=%f xap=%f ' \
#	                   float(self.GetTAVRate()), float(self.GetTBPRate()), float(self.GetTAPRate()),
#	                   float(self.GetTAVXsec()), float(self.GetTBPXsec()), float(self.GetTAPXsec()),

    def GetAttrWithCheck(self,attr):
        try:
            return getattr(self,attr)
        except AttributeError,e:
            raise VarNotAvailable("Requested attribute "+attr+" not loaded, chain="+self.GetName())

    def GetAttrWithDefault(self,attr,default):
        if hasattr(self,attr):
            return getattr(self,attr)
        else:
            return default

    lvlmap = { 'L1':1, 'HLT':2 }

    def GetName(self)            : return self.name
    def IsGroup(self)            : return self.isgroup
    def GetRate(self)            : return self.GetAttrWithCheck("rate")
    def GetRateErr(self)         : return self.GetAttrWithCheck("rateerr")
    def GetLevel(self)           : return self.level
    def GetLevelNum(self)        : return CostChain.lvlmap[self.level]
    def GetPrescale(self)        :
        if hasattr(self,"prescale"):
            return getattr(self,"prescale")
        elif  hasattr(self,"chain_prescale"):
            return getattr(self,"chain_prescale")
        elif  hasattr(self,"chain_precale"):
            return getattr(self,"chain_precale")
        else :
           raise VarNotAvailable("Requested attribute prescale not loaded, chain="+self.GetName())
    def GetPassthrough(self)     : return self.GetAttrWithCheck("passthrough")
    def GetRerun(self)           : return self.GetAttrWithCheck("rerun")
    def GetLowerChain(self)      :
        lower_chain=self.GetAttrWithCheck("lowerchain")
        if lower_chain=='':
            raise VarNotAvailable("Empty lower chain for "+self.name)
        return lower_chain
    def GetFill(self)            : return self.fill
    def GetRun(self)             : return self.run
    def GetLb(self)              : return self.lb
    def GetCounter(self)         : return self.counter

    # gets only single seed lower chains and give '' for all other chains
    def GetLowerChainSingleSeed(self)      :
        if TrigCostTRP.NonChainTRP(self.name):
            return ''
        lower_chain=self.GetAttrWithDefault("lowerchain",'')
        if string.count(lower_chain,","):
            return ''
        return lower_chain

    def SetName(self,name)       : setattr(self,"name",name)
    def SetIsGroup(self,val)     : self.isgroup=val
    def SetRate(self,rate)       : setattr(self,"rate",rate)
    def SetLevel(self,lvl)       : setattr(self,"level",lvl)
    def SetPrescale(self,ps)     : setattr(self,"prescale",ps)
    def SetPassthrough(self,ps)  : setattr(self,"passthrough",ps)
    def SetRerun(self,ps)        : setattr(self,"rerun",ps)
    def SetLowerChain(self,name) : setattr(self,"lowerchain", name)
    def SetFill(self,fill)       : setattr(self,"fill",fill)
    def SetRun(self,run)         : setattr(self,"run",run)
    def SetLb(self,lb)           : setattr(self,"lb",lb)
    def SetCounter(self,counter) : setattr(self,"counter",counter)

    # from trig cost XML files
    def SetRateErr(self,rateerr) : setattr(self,"rateerr",rateerr)
    def SetEff(self,eff)         : setattr(self,"eff",eff)
    def SetEffErr(self,efferr)   : setattr(self,"eff",efferr)
    def SetPrescaledEff(self,eff): setattr(self,"prescaledeff",eff)
    def SetPSEffErr(self,efferr) : setattr(self,"psefferr",efferr)
    def SetNEvents(self,nevts)   : setattr(self,"nevts",nevts)
    def SetNEventsErr(self,val)  : setattr(self,"nevtserr",val)

    # for L1 counters
    def GetTAVCnt(self) : return self.GetAttrWithCheck("tavcnt")
    def GetTAPCnt(self) : return self.GetAttrWithCheck("tapcnt")
    def GetTBPCnt(self) : return self.GetAttrWithCheck("tbpcnt")
    def GetTAVOver(self): return self.GetAttrWithCheck("tavover")
    def GetTAPOver(self): return self.GetAttrWithCheck("tapover")
    def GetTBPOver(self): return self.GetAttrWithCheck("tbpover")

    def SetTAVCnt(self,val) : setattr(self,"tavcnt",val)
    def SetTAPCnt(self,val) : setattr(self,"tapcnt",val)
    def SetTBPCnt(self,val) : setattr(self,"tbpcnt",val)
    def SetTAVOver(self,val): setattr(self,"tavover",val)
    def SetTAPOver(self,val): setattr(self,"tapover",val)
    def SetTBPOver(self,val): setattr(self,"tbpover",val)

    # for rates & errors
    def GetTAVRate(self) : return self.GetAttrWithCheck("rate")
    def GetTAPRate(self) : return self.GetAttrWithCheck("taprate")
    def GetTBPRate(self) : return self.GetAttrWithCheck("tbprate")

    def GetTAVRateErr(self) : return self.GetAttrWithCheck("rateerr")
    def GetTAPRateErr(self) : return self.GetAttrWithCheck("taprateerr")
    def GetTBPRateErr(self) : return self.GetAttrWithCheck("tbprateerr")

    def SetTAVRate(self,val) : setattr(self,"rate",val) # this is the default rate for L1
    def SetTAPRate(self,val) : setattr(self,"taprate",val)
    def SetTBPRate(self,val) : setattr(self,"tbprate",val)

    def SetTAVRateErr(self,rateerr) : setattr(self,"rateerr",rateerr)
    def SetTAPRateErr(self,rateerr) : setattr(self,"taprateerr",rateerr)
    def SetTBPRateErr(self,rateerr) : setattr(self,"tbprateerr",rateerr)

    # for cross sections
    def GetTAVXsec(self) : return self.GetAttrWithCheck("xsec")
    def GetTAPXsec(self) : return self.GetAttrWithCheck("tapxsec")
    def GetTBPXsec(self) : return self.GetAttrWithCheck("tbpxsec")

    def SetTAVXsec(self,val) : setattr(self,"xsec",val) # this is the default cross-section
    def SetTAPXsec(self,val) : setattr(self,"tapxsec",val)
    def SetTBPXsec(self,val) : setattr(self,"tbpxsec",val)


#---------------------------------------------------------------------
#class CostGroup:
#    def __init__(self):
#        self.name      = ""
#        self.type      = ""
#        self.level     = ""
#
#    def Print(self):
#        print 'chain name:',self.GetName()
#        print '      rate:',self.GetRate()
#
#    def GetAttrWithCheck(self,attr):
#        try:
#            return getattr(self,attr)
#        except AttributeError,e:
#            raise Exception("Requested attribute "+attr+" not loaded")
#
#    def GetAttrWithDefault(self,attr,default):
#        if hasattr(self,attr):
#            return getattr(self,attr)
#        else:
#            return default
#
#    def GetName(self)            : return self.name
#    def GetType(self)            : return self.type
#    def GetRate(self)            : return self.GetAttrWithCheck("rate")
#    def GetLevel(self)           : return self.level
#
#    def SetName(self,name)       : setattr(self,"name",name)
#    def SetRate(self,rate)       : setattr(self,"rate",rate)
#    def SetLevel(self,lvl)       : setattr(self,"level",level)
#    def SetPrescale(self,ps)     : setattr(self,"prescale",ps)
#    def SetPassthrough(self,ps)  : setattr(self,"passthrough",ps)
#    def SetRerun(self,ps)        : setattr(self,"rerun",ps)
#
#    # from trig cost XML files
#    def SetRateErr(self,rateerr) : setattr(self,"rateerr",rateerr)
#    def SetEff(self,eff)         : setattr(self,"eff",eff)
#    def SetEffErr(self,efferr)   : setattr(self,"eff",efferr)
#    def SetPrescaledEff(self,eff): setattr(self,"prescaledeff",eff)
#    def SetPSEffErr(self,efferr) : setattr(self,"psefferr",efferr)
#    def SetNEvents(self,nevts)   : setattr(self,"nevts",nevts)
#    def SetNEventsErr(self,val)  : setattr(self,"nevtseff",val)
#

#---------------------------------------------------------------------
#
# Main class holding trigger cost results:
#   - dictionary of chains filled from different sources: XML, COOL, TRP, etc
#
#---------------------------------------------------------------------
class CostResult:
    # Accessors
    def GetChainNames(self,lvl="",forceSort=False):
        unsortedlist = []
        if lvl=="":
            unsortedlist = self.chains.keys()
        else:
            unsortedlist = [ ch.GetName() for ch in self.chains.values() \
                    if ch.GetLevel()==lvl ]
        return sorted(unsortedlist, key=natural_sortkey)

    def GetChains(self,lvl=""):
        if lvl=="":
            return self.chains.values()
        else:
            return [ ch for ch in self.chains.values() \
                     if ch.GetLevel()==lvl]

    def GetL1ChainNames(self): return self.GetChainNames('L1')
    def GetHLTChainNames(self): return self.GetChainNames('HLT')

    def GetL1Chains(self): return self.GetChains('L1')
    def GetHLTChains(self): return self.GetChains('HLT')

    #    def GetChains(self): return self.chains.values()
    def GetChain(self,chname):
        if self.HasChain(chname):
            return self.chains[chname]
        return None
    def SetChain(self,chname,ch): self.chains[chname
                                              ]=ch
    def HasChain(self,chname): return self.chains.has_key(chname)

    def Print(self):
        for chain in self.chains.values(): chain.Print()

    # init (defined member data)
    def __init__(self):
        # Data
        self.chains  = {}     # map of chain name -> CostChain
        self.name = ''
        self.sorted = 0
    def SetName(self, name): self.name = name
    def GetName(self): return self.name

    def SetSource(self, source): self.source = source
    def GetSource(self): return self.source


#---------------------------------------------------------------------
#
# Collection of results for multiple lumiblocks
#
#---------------------------------------------------------------------
class CostResultCollection:
    #
    # Accessors
    #
    def GetChainNames(self,lvl="",forceSort=False):
        lb = self.lbbeg
        if self.hashlt:
            if self.lbbeg_hlt > self.lbbeg:
                lb = self.lbbeg_hlt
        if self.results.has_key(lb):
            return self.results[lb].GetChainNames(lvl)
        return []
    def GetL1ChainNames(self): return self.GetChainNames('L1')
    def GetHLTChainNames(self): return self.GetChainNames('HLT')


    def FirstLB(self): return self.lbbeg
    def LastLB(self):  return self.lbend
    def LBRange(self): return range(self.lbbeg,self.lbend+1)

    def GetCostChain(self,lb,chname):
        if self.HasCostChain(lb,chname):
            return self.results[lb].GetChain(chname)
        return None

    def SetCostChain(self,lb,chain,ch):
        if not self.results.has_key(lb):
            self.results[lb] = CostResult()
        self.results[lb].SetChain(chain,ch)

    def GetCostResult(self,lb):
        return self.results[lb]

    def HasCostChain(self,lb,chname):
        if self.results.has_key(lb):
            return self.results[lb].HasChain(chname)
        return False

    # HLT version
    def FirstLBHLT(self): return self.lbbeg_hlt
    def LastLBHLT(self):  return self.lbend_hlt
    def LBRangeHLT(self): return range(self.lbbeg_hlt, self.lbend_hlt+1)

    def AddHLT(self, ratecoll_hlt):
        self.hashlt = True # Set HLT flags
        self.lbbeg_hlt = ratecoll_hlt.FirstLB()
        self.lbend_hlt = ratecoll_hlt.LastLB()
        for lb in ratecoll_hlt.LBRange():
            for chname in ratecoll_hlt.GetChainNames():
                chain = ratecoll_hlt.GetCostChain(lb, chname)
                self.SetCostChain(lb, chname, chain)

#	    def Print(self):
#	        print 'CostResultCollection', self.FirstLB(), '--', self.LastLB()
#	        for chname in self.GetChainNames():
#	            self.GetCostChain(self.FirstLB(),chname)

    def __repr__(self): self.Print()
    def Print(self, options=[]):
        if len(self.results)==0: return
        lbrange = self.LBRange()
        print "\nLB Range("+str(self.lbbeg)+","+str(self.lbend)+")"
        for lvl in ['L1','HLT']:
            print "\n-------------------------------------------------------------------------",
            print "\n%-45s %4s" % ("Rates for %s" % lvl, "LB="),
            for lb in lbrange: print "% 7d" % lb,
            print "\n-------------------------------------------------------------------------"
            for chname in self.GetChainNames(lvl):
                # Do not print if all zero...
                allzero = 'quiet' in options
                for lb in lbrange:
                    ch = self.GetCostChain(lb,chname)
                    if ch: allzero = allzero and ch.GetRate()==0
                    else:  allzero = False

                if allzero: continue
                print "\n%-50s" % (chname,),
                for lb in lbrange:
                    ch = self.GetCostChain(lb,chname)
                    if ch: print "% 7.2g" % (ch.GetRate(),),
                    else:  print "% 7s" % ('NA',),

        print "\nRuntime",
        if 'quiet' in options: print "info: The zero-rate chains are suppressed due to --quiet"
        else:                  print "hint: --quiet suppresses the zero-rate chains"
        return

    def GetName(self):      return self.name
    def GetRun(self):       return self.run
    def HasHLT(self):       return self.hashlt
    def SetName(self,name): self.name = name
    def SetRun(self,run):   self.run = run

    def __init__(self, results = {}):
        # Data
        self.results = results # map lb->->CostResults
        self.lbbeg = -1
        self.lbend = -1

        self.lbbeg_hlt = -1
        self.lbend_hlt = -1
        self.run = -1
        self.hashlt = False
        self.name = ''
        pass



#---------------------------------------------------------------------
#
# Configuration Information, loaded with TrigCostCool.GetConfig(...)
#
#---------------------------------------------------------------------
class PrescaleRange:
    def __init__(self):
        self.lbbeg  = 1
        self.lbend  = 9999999 # larger than ever happens
        self.l1key  = -1
        self.hltkey = -1

    def FirstLB(self): return self.lbbeg
    def LastLB(self) : return self.lbend
    def LBRange(self): return range(self.lbbeg,self.lbend+1)
    def Length(self) : return self.LastLB()-self.FirstLB()+1



class Prescale:
    def __init__(self):
        self.ps    = -1
        self.pt    = -1 # hlt only
        self.rerun = -1 # hlt only

class HLTChain:
    def __init__(self):
        self.ChainName      = ""
        self.ChainVersion   = -1
        self.ChainCounter   = -1
        self.TriggerLevel   = ""
        self.LowerChainName = ""


class RunConfig:
    def __init__(self):
        self.L1ChainName2CtpId   = {}
        self.ChainName2HLTChain  = {} # to HLTChain object (contains counter)

        self.L1CtpId2ChainName   = {}
        self.HLTCounter2HLTChain  = {} # to HLTChain object

        self.PrescaleRanges = []
        self.L1Prescales    = {} # map from l1key to prescales
        self.HLTPrescales    = {} # map from hltkey to prescales

        self.HLTChainName2Counter = {}
        self.HLTCounter2ChainName = {}

    def GetPrescaleRanges(self): return self.PrescaleRanges



#---------------------------------------------------------------------
#
# Info on Lumiblock : returned by TrigCostCool.GetLumiblocks()
#
#---------------------------------------------------------------------
class LumiBlock:
    orbitFreq  = 40.0790e6 / 3564.0

    def __init__(self):
        self.lb        = -1
        self.beg_time  = 0
        self.end_time  = 0
        self.l1_accept = 0
        self.l1a_turn  = 0
        self.tav_turn  = 0
        self.tap_turn  = 0
        self.tbp_turn  = 0

        self.avr_lumi  = 0 # average instantenous lumi:       LBAvInstLumi
        self.avr_evts  = 0 # average number of pp collisions: LBAvEvtsPerBX

        self.ncol      = 0 # number colliding bunches
        self.nb1       = 0 # number beam1 bunches
        self.nb2       = 0 # number beam2 bunches
        self.mu        = 0 # mean pileup
        self.colBCID   = set() # colliding pattern
        self.b1BCID    = set() # beam1 pattern
        self.b2BCID    = set() # beam2 pattern
        self.allBCID   = set() # beam1 or beam2 pattern

        self.fill      = -1  # LHC fill number
        self.stable    = -1  # Stable beam
        self.lhc       = '' # LHC machine mode
        self.beam      = '' # Beam mode
        self.ebeam     = -1  # Beam energy in GeV

        self.bgStartLB = 0
        self.bgBCIDList0 = []
        self.bgBCIDList1 = []
        self.bgBCIDList2 = []
        self.bgBCIDList3 = []
        self.bgBCIDList4 = []
        self.bgBCIDList5 = []
        self.bgBCIDList6 = []
        self.bgBCIDList7 = []
        self.bgLength0 = 0
        self.bgLength1 = 0
        self.bgLength2 = 0
        self.bgLength3 = 0
        self.bgLength4 = 0
        self.bgLength5 = 0
        self.bgLength6 = 0
        self.bgLength7 = 0
        self.bgName0 = ''
        self.bgName1 = ''
        self.bgName2 = ''
        self.bgName3 = ''
        self.bgName4 = ''
        self.bgName5 = ''
        self.bgName6 = ''
        self.bgName7 = ''

    def GetDT(self):
        return (self.end_time-self.beg_time)/1.0E9

    def GetIntLumi(self):
        return self.avr_lumi*self.tav_turn/self.orbitFreq

    def GetDeliveredLumi(self):
        return self.avr_lumi*self.GetDT()

    def AsString(self):
        from CoolConvUtilities import AtlCoolTool
        beg_ = AtlCoolTool.time.ctime(self.beg_time/1.0E9)
        end_ = AtlCoolTool.time.ctime(self.end_time/1.0E9)
        lumi = self.GetIntLumi()

        rval = 'lb %4d'%self.lb+' '+beg_+' '+end_+'\n'\
               '   l1_accept=%d'%self.l1_accept+'\n'+\
               '   l1a_turn =%d'%self.l1a_turn +'\n'+\
               '   tav_turn =%d'%self.tav_turn +'\n'+\
               '   tap_turn =%d'%self.tap_turn +'\n'+\
               '   tbp_turn =%d'%self.tbp_turn +'\n'+\
               '   avr_lumi =%f'%self.avr_lumi +'\n'+\
               '   avr_evts =%f'%self.avr_evts +'\n'+\
               '   int_lumi =%f'%lumi +'\n'

        return rval

class LumiBlockSet:

    def __repr__(self): self.Print()
    def Print(self, options=[]):
        lbrange = self.LBRange()
        print "-------------------------------------------------------------------------",
        print "\n%-45s %4s" % ("Beam conditions", "LB="),
        for lb in lbrange: print "% 7d" % lb,
        print "\n-------------------------------------------------------------------------",
        print "\n%-50s" % "# colliding bunches",
        for lb in lbrange: print "% 7d" % self.GetBunches(lb),
        print "\n%-50s" % "# beam1 bunch",
        for lb in lbrange: print "% 7d" % self.GetNbunch1(lb),
        print "\n%-50s" % "# beam2 bunch",
        for lb in lbrange: print "% 7d" % self.GetNbunch2(lb),
        print "\n%-50s" % "Fill number",
        for lb in lbrange: print "% 7d" % self.GetFillNumber(lb),
        print "\n%-50s" % "Stable beams?",
        for lb in lbrange: print "% 7s" % self.GetStableBeams(lb),
        print "\n%-50s" % "Beam energy (GeV)",
        for lb in lbrange: print "% 7.1f" % self.GetBeamEnergyGeV(lb),
        print "\n%-50s" % "LHC mode (abbreviated)",
        for lb in lbrange: print "% 7s" % self.GetLHCMode(lb).split(' ')[0],
        print "\n%-50s" % "Beam mode (abbreviated)",
        for lb in lbrange: print "% 7s" % self.GetBeamMode(lb).split(' ')[0].replace('INJECTION','INJ'),
        print
        return

    def GetLB(self,lb):   return self.lbs[lb]

    def FirstLB(self): return self.lbbeg
    def LastLB(self):  return self.lbend
    def LBRange(self): return range(self.lbbeg,self.lbend+1)

    def GetLBRange(self,lbbeg=-1,lbend=-1):
        return self.GetBegLB(lbbeg),self.GetEndLB(lbend)

    def GetBegLB(self,lbbeg=-1):
        if lbbeg==-1:
            if self.lbbeg==-1:
                return 0
            return self.lbbeg
        return lbbeg

    def GetEndLB(self,lbend=-1):
        if lbend==-1:
            if self.lbend==-1:
                return len(self.lbs)
            return self.lbend
        return lbend

    # returns inverse microbarns
    def GetDeliveredLumi(self,lbbeg=-1,lbend=-1):
        lbbeg, lbend = self.GetLBRange(lbbeg, lbend)
        sum = 0
        for lb in range(lbbeg,lbend+1):
            sum+=self.lbs[lb].GetDeliveredLumi()
        return sum

    # returns inverse microbarns
    def GetRecordedLumi(self,lbbeg=-1,lbend=-1):
        lbbeg, lbend = self.GetLBRange(lbbeg, lbend)
        sum = 0
        for lb in range(lbbeg,lbend+1):
            sum+=self.lbs[lb].GetIntLumi()
        return sum

    # returns seconds
    def GetTotalTime(self,lbbeg=-1,lbend=-1):
        lbbeg, lbend = self.GetLBRange(lbbeg, lbend)
        return (self.lbs[lbend].end_time-self.lbs[lbbeg].beg_time)/1e9

    # returns seconds
    def GetStartTime(self,lbbeg=-1):
        lbbeg = self.GetBegLB(lbbeg)
        return self.lbs[lbbeg].beg_time/1e9

    # returns seconds
    def GetEndTime(self,lbend=-1):
        lbend = self.GetEndLB(lbend)
        return self.lbs[lbend].end_time/1e9

    # returns in units of 1e30 cm^-2 s^-1
    def GetAvgInstantaneousLumiE30(self, lbbeg=-1, lbend=-1):
        return self.GetAvgInstantaneousLumi(lbbeg, lbend) / 1e30

    # returns inverse microbarns s^-1 =  1e30 cm^-2 s^-1
    def GetAvgInstantaneousLumi(self,lbbeg=-1,lbend=-1):
        denom = self.GetTotalTime(lbbeg, lbend)
        if denom > 0:
            return self.GetDeliveredLumi(lbbeg,lbend) / denom * 1e30
        return -1

    # returns average ratio of l1afterveto wrt l1afterps \todo
    def GetL1LiveFraction(self,lbbeg=-1,lbend=-1):
        return -1

    # bunch information
    def GetBunches(self,lb): return self.lbs[lb].ncol # num colliding bunches
    def GetNbunch1(self,lb): return self.lbs[lb].nb1  # num beam1 bunches
    def GetNbunch2(self,lb): return self.lbs[lb].nb2  # num beam2 bunches
    def GetBunchesBCID(self,lb): return self.lbs[lb].colBCID # set() of col bunch BCID
    def GetNbunch1BCID(self,lb): return self.lbs[lb].b1BCID  # set() of b1 bunch BCID
    def GetNbunch2BCID(self,lb): return self.lbs[lb].b2BCID  # set() of b2 bunch BCID

    # fill information
    def GetFillNumber(self,lb):      return self.lbs[lb].fill
    def GetStableBeams(self,lb):     return self.lbs[lb].stable
    def GetLHCMode(self,lb):         return self.lbs[lb].lhc
    def GetBeamMode(self,lb):        return self.lbs[lb].beam
    def GetBeamEnergyGeV(self,lb):   return self.lbs[lb].ebeam

    # returns in units of 1e30 cm^-2 s^-1
    def GetAvgInstantaneousBunchLumiE30(self,lb=-1):
        return self.GetAvgInstantaneousBunchLumi(lb) / 1e30

    def GetAvgInstantaneousBunchLumi(self,lb=-1):
        denom = self.GetBunches(self.GetBegLB(lb))
        if denom > 0:
            return self.GetAvgInstantaneousLumi(lb,lb) / denom
        return -1

    def GetLiveTime(self,lbbeg=-1,lbend=-1):
        lbbeg, lbend = self.GetLBRange(lbbeg, lbend)
        sum = 0
        for lb in range(lbbeg,lbend+1):
            if self.GetBunches(lb) > 0:
                sum += self.GetTotalTime(lb,lb)
        return sum

    def GetIntegratedTimeAveragedBunches(self,lbbeg=-1,lbend=-1): return self.GetTimeAveragedBunches(lbbeg,lbend,False)
    def GetLiveTimeAveragedBunches(self,lbbeg=-1,lbend=-1): return self.GetTimeAveragedBunches(lbbeg,lbend,True)
    def GetTimeAveragedBunches(self,lbbeg=-1,lbend=-1,doLive=True):
        lbbeg, lbend = self.GetLBRange(lbbeg, lbend)
        sum = 0
        denom = 0
        for lb in range(lbbeg,lbend+1):
            bunches = self.GetBunches(lb)
            if doLive and bunches <= 0:
                continue
            time = self.GetTotalTime(lb,lb)
            sum += bunches * time
            denom += time
        mean = 0
        if denom>0:
            mean = sum/denom
        return mean

    def GetMeanPileUp(self,lb=-1):
        # inelastic xsec: 71.5 mb
        # frequency of protons bunch going around the LHC: 11.24558 Hz
        # mu = 71.5 / 11.24558*bunchLumi = 6.358*bunchLumi
        lumiPerBunch=self.GetAvgInstantaneousBunchLumi(lb)
        self.lbs[lb].mu=lumiPerBunch * 6.358/1.0e30
        return self.lbs[lb].mu

    def __init__(self):
        self.lbs = {} # --> Collection of LumiBlock
        self.lbbeg = -1
        self.lbend = -1

class DatasetDescr :

    def __init__(self) :

        self.name = ''
        self.parent = ''
        self.path = ''
        self.energy = 0.0
        self.cross_section = 0.0
        self.gen_eff = 0.0
        self.collisions_per_evt = 0.0
        self.menu = ''
        self.tune = ''
        self.AtlasProject = ''
        self.AtlasVersion = ''
        self.year = 0
        self.month = 0
        self.day = 0
        self.version = ''
        self.comment = ''




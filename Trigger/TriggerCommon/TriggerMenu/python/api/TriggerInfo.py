# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__author__  = 'Javier Montejo'
__version__="$Revision: 1.00 $"
__doc__="Class containing all the information of an HLT chain"

import re
from TriggerMenu.api.TriggerEnums import TriggerType

class TriggerInfo:
    ''' Object containing all the HLT information related to a given period.
        Stores a list of TriggerChain objects and the functions to skim them
    '''
    def __init__(self,period):
        self.triggerChains = []
        self.period = period

        from TriggerDataAccess import getHLTlist
        HLTlist = getHLTlist(period)
        for hlt, l1, ps in HLTlist:
            self.triggerChains.append( TriggerChain(hlt, l1, ps))

    def reparse(self):
        self.triggerChains = [ TriggerChain(t.name, t.l1seed, t.prescale) for t in self.triggerChains ]

    def _getUnprescaled(self,triggerType, additionalTriggerType, matchPattern):
        return [x.name for x in self.triggerChains if x.isUnprescaled() and x.passType(triggerType, additionalTriggerType) and re.search(matchPattern, x.name)]

    def _getLowestUnprescaled(self,triggerType, additionalTriggerType, matchPattern):
        chainList = [ x for x in self.triggerChains if x.isUnprescaled() and x.passType(triggerType, additionalTriggerType) and re.search(matchPattern, x.name)]
        typeMap = {}
        for chain in chainList:
            if chain.triggerType not in typeMap:
                typeMap[chain.triggerType] = [chain]
                continue
            append = False
            for other in typeMap[chain.triggerType][:]:
                comp = chain.isLowerThan(other)
                if comp ==  0: 
                    append = False
                    break
                append = True
                if comp ==  1:    typeMap[chain.triggerType].remove(other)
            if append:
                typeMap[chain.triggerType].append(chain)
        return [x.name for t in typeMap.itervalues() for x in t ]


    def _getAllHLT(self,triggerType, additionalTriggerType, matchPattern):
        return {x.name: x.prescale for x in self.triggerChains if x.passType(triggerType, additionalTriggerType) and re.search(matchPattern, x.name)}


class TriggerLeg:
    types          = ('e','j','mu','tau','xe','g','ht')
    legpattern     = re.compile('([0-9]*)(%s)([0-9]+)' % '|'.join(types))
    detailpattern  = re.compile('(?:-?\d+)|(?:[^0-9|-]+)')
    bjetpattern    = re.compile('bmv')
    bphyspattern   = re.compile('b[A-Z]')
    exoticspattern = re.compile('llp|LLP|muvtx|hiptrt|LATE|NOMATCH')

    def __init__(self,legname, chainseed, chainname):
        self.legname = legname
        self.details = []
        self.l1seed = ""
        chainseed= chainseed.replace("L1_","")
        blocks = legname.split("_L1")

        for token in blocks[0].split("_"):
            m = self.legpattern.match(token)
            if m:
                count,legtype,thr = m.groups()
                self.count = int(count) if count else 1
                self.thr = int(thr)
                if legtype == 'e':
                    if self.count > 1: self.legtype = TriggerType.el_multi
                    else:              self.legtype = TriggerType.el_single
                elif legtype == 'mu':
                    if self.count > 1: self.legtype = TriggerType.mu_multi
                    else:              self.legtype = TriggerType.mu_single
                elif legtype == 'j':
                    if self.count > 1: self.legtype = TriggerType.j_multi
                    else:              self.legtype = TriggerType.j_single
                elif legtype == 'tau':
                    if self.count > 1: self.legtype = TriggerType.tau_multi
                    else:              self.legtype = TriggerType.tau_single
                elif legtype == 'g':
                    if self.count > 1: self.legtype = TriggerType.g_multi
                    else:              self.legtype = TriggerType.g_single
                elif legtype == 'xe':
                    self.legtype = TriggerType.xe
                elif legtype == 'ht':
                    self.legtype = TriggerType.ht
                else:
                    print "Unknown trigger type:",legtype
            else:
                if self.bjetpattern.match(token):
                    if self.legtype == TriggerType.j_single: self.legtype = TriggerType.bj_single
                    if self.legtype == TriggerType.j_multi:  self.legtype = TriggerType.bj_multi
                if self.bphyspattern.match(token):
                    self.legtype = TriggerType.mu_bphys
                if self.exoticspattern.search(token):
                    self.legtype = TriggerType.exotics
                self.details.append(token)

        for l1seed in blocks[1:]:
            if self.exoticspattern.search(l1seed):
                self.legtype = TriggerType.exotics
            if l1seed == chainseed: continue
            else: 
                assert self.l1seed=="", (self.l1seed, chainseed, chainname, blocks[1:])
                self.l1seed = l1seed
        if not self.l1seed: self.l1seed = chainseed

    def __repr__(self):
        return self.legname+" {0:b}".format(self.legtype)

    def isLegLowerThan(self, other, debug=False):
        ''' Returns -9 if none of them is lower than the other (e.g. different met flavour).
            Returns -1 if identical
            Returns  0 if other is lower than self.
            Returns  1 if self  is lower than other.
        '''
        if debug:
            print "DEBUG LEGS --------"
            print self.legname, other.legname
            print self.legtype, other.legtype
            print self.l1seed, other.l1seed
            print self.details, other.details
            print self.thr, other.thr
            print self.compareDetails(other, debug=True)
            print self.details == other.details
            print "DEBUG LEGS END --------"

        if self.legtype != other.legtype: return -9
        if self.compareDetails(other) == -1:
            if self.thr < other.thr: return 1
            if self.thr > other.thr: return 0
            else: return -1

        if self.compareDetails(other) == 1 and self.thr  <= other.thr: return 1
        if self.compareDetails(other) == 0 and other.thr <= self.thr:  return 0
        return -9

    def compareDetails(self, other, debug=False):
        ''' Returns -9 if none of them is lower than the other (e.g. different met flavour).
            Returns -1 if identical
            Returns  0 if other is lower than self.
            Returns  1 if self  is lower than other.
        '''
        from copy import deepcopy

        if debug: print "compareDetails:",len(self.details), len(other.details),(self.l1seed == other.l1seed),(self.details == other.details) 
        if len(self.details) != len(other.details): 
            if any([x.startswith("nod0") for x in self.details]):
                cloneself = deepcopy(self)
                cloneself.details = [ x for x in self.details if not x.startswith("nod0")]
                compno = cloneself.compareDetails(other,debug)
                if compno ==1 or compno == -1: 
                    #print "About to return 1",self.legname, other.legname
                    return 1
            if any([x.startswith("nod0") for x in other.details]):
                cloneother = deepcopy(other)
                cloneother.details = [ x for x in other.details if not x.startswith("nod0")]
                compno = self.compareDetails(cloneother,debug)
                if compno ==0 or compno == -1:
                    #print "About to return 0",self.legname, other.legname
                    return 0
            if any([x.startswith("cut") for x in self.details]):
                cloneself = deepcopy(self)
                cloneself.details = [ x for x in self.details if not x.startswith("cut")]
                compno = cloneself.compareDetails(other,debug)
                if compno ==0 or compno == -1: 
                    #print "About to return 0",self.legname, other.legname
                    return 0
            if any([x.startswith("cut") for x in other.details]):
                cloneother = deepcopy(other)
                cloneother.details = [ x for x in other.details if not x.startswith("cut")]
                compno = self.compareDetails(cloneother,debug)
                if compno ==1 or compno == -1:
                    #print "About to return 1",self.legname, other.legname
                    return 1
            return -9
        compl1seed  = self.compareTags(self.l1seed, other.l1seed, stringSubset=True, debug=debug)
        compdetails = self.compareTags("".join(self.details), "".join(other.details), debug=debug )
        if self.l1seed == other.l1seed:
            if self.details == other.details: return -1
            if debug: print "compareTags 1:",compdetails
            return compdetails

        if self.details == other.details:
            if debug: print "compareTags 2:",compl1seed
            return compl1seed

        if compl1seed == compdetails:
            return compl1seed
        return -9

    def compareTags(self, tag1, tag2, stringSubset=False,debug=False):
        def mycomp(x,y):
            ''' Return -9 for different strings, 
                -1 for identical strings/nums, 
                0/1 for high/low numbers or string subsets
            '''
            try: 
                x,y = int(x), int(y)
                if x < y: return 1
                elif x > y: return 0
                else: return -1
            except ValueError: 
                if x==y: return -1
                if x == y.replace("vloose","loose"): return 0
                if x == y.replace("vloose","loose").replace("loose","medium"): return 0
                if x == y.replace("vloose","loose").replace("loose","medium").replace("medium","tight"): return 0
                if y == x.replace("vloose","loose"): return 1
                if y == x.replace("vloose","loose").replace("loose","medium"): return 1
                if y == x.replace("vloose","loose").replace("loose","medium").replace("medium","tight"): return 1
                if stringSubset:
                    if x in y: return 1
                    if y in x: return 0
                return -9

        if tag1 == tag2: return -1
        #lower mv2 values are tighter, put a minus sign to trick it
        tag1 = tag1.replace("mv2c","mv2c-")
        tag2 = tag2.replace("mv2c","mv2c-")
        #only make a statement on the numerical values, with everything else identical
        reself  = self.detailpattern.findall(tag1)
        reother = self.detailpattern.findall(tag2)

        if len(reself) != len(reother): return -9
        thecomp = [mycomp(a,b) for a,b in zip(reself,reother)]
        if debug: print "thecomp:",thecomp,reself,reother
        if any([x == -9 for x in thecomp]): return -9
        if all([x !=0 for x in thecomp]) and any([x == 1 for x in thecomp]): return 1
        if all([x !=1 for x in thecomp]) and any([x == 0 for x in thecomp]): return 0
        return -9

    @classmethod
    def parse_legs(cls,name,l1seed,chainname):
        legsname = []
        name = name.replace("HLT_","")
        for token in name.split("_"):
            m = cls.legpattern.match(token)
            if m:
                legsname.append(token)
            elif legsname: 
                legsname[-1] += "_"+token
            else: #first token doesn't match
                #print "parse_legs: Problem parsing",name
                return []
        return [TriggerLeg(l,l1seed,chainname) for l in legsname]

class TriggerChain:
    l1types        = ('EM','J','MU','TAU','XE','XS','HT')
    l1pattern      = re.compile('([0-9]*)(%s)([0-9]+)' % '|'.join(l1types))

    def __init__(self,name,l1seed,prescale):
        self.name = name
        self.l1seed = l1seed
        tmplegs = TriggerLeg.parse_legs(name,l1seed,name)
        self.legs = self.splitAndOrderLegs(tmplegs)
        self.prescale = prescale
        self.triggerType = self.getTriggerType(self.legs, l1seed)

    def splitAndOrderLegs(self, legs):
        from copy import deepcopy
        newLegs = []
        for triggerType in TriggerType:
            for l in legs:
                if not l.legtype == triggerType: continue
                for i in range(l.count):
                    tmp = deepcopy(l)
                    tmp.count = 1
                    if tmp.legtype & TriggerType.el_multi:
                        tmp.legtype |=  TriggerType.el_single
                        tmp.legtype &= ~TriggerType.el_multi
                    elif tmp.legtype & TriggerType.mu_multi:
                        tmp.legtype |=  TriggerType.mu_single
                        tmp.legtype &= ~TriggerType.mu_multi
                    elif tmp.legtype & TriggerType.tau_multi:
                        tmp.legtype |=  TriggerType.tau_single
                        tmp.legtype &= ~TriggerType.tau_multi
                    elif tmp.legtype & TriggerType.j_multi:
                        tmp.legtype |=  TriggerType.j_single
                        tmp.legtype &= ~TriggerType.j_multi
                    elif tmp.legtype & TriggerType.bj_multi:
                        tmp.legtype |=  TriggerType.bj_single
                        tmp.legtype &= ~TriggerType.bj_multi
                    elif tmp.legtype & TriggerType.g_multi:
                        tmp.legtype |=  TriggerType.g_single
                        tmp.legtype &= ~TriggerType.g_multi
                    newLegs.append(tmp)
        return newLegs

    def getTriggerType(self, legs, l1seed):
        mtype = TriggerType.UNDEFINED
        for l in legs:
            if mtype & TriggerType.el and l.legtype & TriggerType.el:
                mtype |=  TriggerType.el_multi
                mtype &= ~TriggerType.el_single
            elif mtype & TriggerType.mu and l.legtype & TriggerType.mu:
                mtype |=  TriggerType.mu_multi
                mtype &= ~TriggerType.mu_single
            elif mtype & TriggerType.tau and l.legtype & TriggerType.tau:
                mtype |=  TriggerType.tau_multi
                mtype &= ~TriggerType.tau_single
            elif mtype & TriggerType.j and l.legtype & TriggerType.j:
                mtype |=  TriggerType.j_multi
                mtype &= ~TriggerType.j_single
            elif mtype & TriggerType.bj and l.legtype & TriggerType.bj:
                mtype |=  TriggerType.bj_multi
                mtype &= ~TriggerType.bj_single
            elif mtype & TriggerType.g and l.legtype & TriggerType.g:
                mtype |=  TriggerType.g_multi
                mtype &= ~TriggerType.g_single
            elif l.legtype & TriggerType.mu_bphys:
                mtype |=  TriggerType.mu_bphys
                mtype &= ~(TriggerType.mu_single | TriggerType.mu_multi)
            elif l.legtype & TriggerType.exotics:
                mtype |=  TriggerType.exotics
            else:
                mtype |= l.legtype

        l1seed= l1seed.replace("L1_","")
        for token in l1seed.split("_"):
            m = self.l1pattern.match(token)
            if m:
                count,legtype,thr = m.groups()
                count = int(count) if count else 1
                if legtype == 'EM':
                    assert(mtype & TriggerType.g or mtype & TriggerType.el or mtype & TriggerType.exotics), (legtype, mtype, self.name)
                elif legtype == 'MU':
                    if count > 1: mtype |= TriggerType.mu_multi
                    elif not mtype & TriggerType.mu_multi: mtype |= TriggerType.mu_single
                elif legtype == 'TAU':
                    assert(mtype & TriggerType.tau or mtype & TriggerType.exotics), (legtype, mtype, self.name)
                elif legtype == 'J':
                    if not mtype & TriggerType.bj and not mtype & TriggerType.j and not mtype & TriggerType.tau and not mtype & TriggerType.ht:
                        if count > 1: mtype |= TriggerType.j_multi
                        elif not mtype & TriggerType.j_multi: mtype |= TriggerType.j_single
                elif legtype == 'XE' or legtype == 'XS':
                    mtype |= TriggerType.xe
                elif legtype == 'HT':
                    mtype |= TriggerType.ht
                else:
                    print "Unknown trigger type:",legtype
        return mtype

    def isUnprescaled(self):
        return self.prescale == 1 

    def getType(self):
        return self.triggerType

    def passType(self, triggerType, additionalTriggerType):
        if self.triggerType == TriggerType.UNDEFINED: return False
        if self.triggerType == TriggerType.ALL:       return True
        match   = (self.triggerType & triggerType)
        if not match: return False
        tmpType = self.triggerType & ~triggerType

        try: 
            for t in additionalTriggerType:
                match   = (tmpType & t)
                if not match: return False
                tmpType = tmpType & ~t
        except TypeError: #Not iterable
            if additionalTriggerType!=TriggerType.UNDEFINED:
                match   = (tmpType & additionalTriggerType)
                if not match: return False
                tmpType = tmpType & ~additionalTriggerType

        return tmpType == TriggerType.UNDEFINED #After matches nothing remains

    def __repr__(self):
        print self.name, self.legs, "{0:b}".format(self.triggerType), self.prescale
        return ""

    def isLowerThan(self, other):
        ''' Returns -1 if none of them is lower than the other (e.g. asymmetric dilepton).
            Returns  0 if other is lower than self.
            Returns  1 if self  is lower than other.
        '''
        if self.triggerType != other.triggerType: return -1
        if len(self.legs) != len(other.legs): return -1
        comp = -1
        debug = False
        #if re.search("HLT_j55_gsc75_bmv2c1040_split_3j55_gsc75_boffperf_split", self.name): debug = True
        if debug: print "DEBUG:",self.name,other.name
        for selfleg, otherleg in zip(self.legs, other.legs):
            legcomp = selfleg.isLegLowerThan(otherleg, debug)
            if debug: print "DEBUG LEG return:", legcomp
            if legcomp == -9: return -1
            elif legcomp == -1: continue
            elif legcomp == 0 and comp == 1: return -1
            elif legcomp == 1 and comp == 0: return -1
            elif legcomp == 0 : comp = 0
            elif legcomp == 1 : comp = 1
        if debug: print "DEBUG FINAL:",comp
        return comp


def test():
    a = TriggerChain("HLT_j50_gsc65_bmv2c1040_split_3j50_gsc65_boffperf_split", "L1J100",1)
    print a
    print bin(a.getType())
    print a.passType(TriggerType.j_multi, TriggerType.UNDEFINED)
    print a.passType(TriggerType.j_multi | TriggerType.bj_single, TriggerType.UNDEFINED)
    print a.isUnprescaled()

if __name__ == "__main__":
    import sys
    sys.exit(test())

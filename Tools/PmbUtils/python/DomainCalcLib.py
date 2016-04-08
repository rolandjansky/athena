# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class DomainCalcLib(object):
    __calcs=None
    def __init__(self):
        self.__ensureInit()

    def getAllCalcNames(self,requireRTT=False,requireLogFiles=False,requireDomains=False):
        l=[]
        for i in range(len(DomainCalcLib.__calcs)):
            if requireRTT and DomainCalcLib.__calcs[i].getCDJobDefinitions("testkey")==None:
                continue
            if requireLogFiles and DomainCalcLib.__calcs[i].getLogFileNamingSchemeDefinitions()==None:
                continue
            if requireDomains and  DomainCalcLib.__calcs[i].defineDomains()==None:
                continue
            l+= [DomainCalcLib.__calcs[i].getName()]
        return l

    def getCalcByName(self,name):
        for i in range(len(DomainCalcLib.__calcs)):
            if DomainCalcLib.__calcs[i].getName()==name:
                return DomainCalcLib.__calcs[i]

    def __getAllPackageNames(self,name2packages):
        p_all=[]
        for n,ps in name2packages.items():
            for p in ps:
                if not p in p_all:
                    p_all+=[p]
        p_all.sort()
        return p_all

    def getAllPackageNames(self):
        return self.__getAllPackageNames(self.getCalcNamesToPackages())

    def getCalcNamesToPackages(self):
        n2p={}
        for i in range(len(DomainCalcLib.__calcs)):
            jobdefs=DomainCalcLib.__calcs[i].getCDJobDefinitions("fakedbkey")
            if not jobdefs: continue
            p=[]
            for jobdef in jobdefs:
                pkg=jobdef.getDBJobPackage()
                if not pkg in p:
                    p+=[pkg]
            p.sort()
            n2p[DomainCalcLib.__calcs[i].getName()]=p
        return n2p

    def getPackagesToCalcNames(self):
        n2p=self.getCalcNamesToPackages()
        p2n={}
        for n,ps in n2p.items():
            for p in ps:
                if not p in p2n.keys(): p2n[p]=[n]
                else: p2n[p]+=[n]
        return p2n

    def __ensureInit(self):
        if DomainCalcLib.__calcs!=None:
            return
        l=[]
        ###############################################################
        c=None
        try:
            from DomainCalc_MuonReco import DomainCalc_MuonReco
            c=DomainCalc_MuonReco()
            c.testvalid()
        except:
            print "WARNING: Problems importing DomainCalc_MuonReco"
            c=None
            raise
        if c!=None:
            l+=[c]
        ###############################################################
        c1=None
        c2=None
        try:
            from DomainCalc_Digi import DomainCalc_Digi
            c1=DomainCalc_Digi(pileup=False)
            c2=DomainCalc_Digi(pileup=True)
            c1.testvalid()
            c2.testvalid()
        except:
            print "WARNING: Problems importing DomainCalc_Digi"
            c1=None
            c2=None
            raise
        if c1!=None:
            l+=[c1]
        if c2!=None:
            l+=[c2]
        c=None
        try:
            from DomainCalc_Trig import DomainCalc_Trig
            c=DomainCalc_Trig()
            c.testvalid()
        except:
            print "WARNING: Problems importing DomainCalc_Trig"
            c=None
            raise
        if c!=None:
            l+=[c]
        ###############################################################
        #Disable for now, too hard to keep up to date.
        #c=None
        #try:
        #    from DomainCalc_RecJobTransforms import DomainCalc_RecJobTransforms
        #    c=DomainCalc_RecJobTransforms()
        #    c.testvalid()
        #except:
        #    print "WARNING: Problems importing DomainCalc_RecJobTransforms"
        #    c=None
        #    raise
        #if c!=None:
        #    l+=[c]
        ###############################################################
        c=None
        try:
            from DomainCalc_Sim import DomainCalc_Sim
            c=DomainCalc_Sim()
            c.testvalid()
        except:
            print "WARNING: Problems importing DomainCalc_Sim"
            c=None
            raise
        if c!=None:
            l+=[c]
        ###############################################################
        c1=None
        c2=None
        c3=None
        #one for logfiles, one for Cosmic BS RTT and one for MC RDO RTT
        try:
            from DomainCalc_RAWtoESDTrf import DomainCalc_RAWtoESDTrf
            c1=DomainCalc_RAWtoESDTrf()
            c2=DomainCalc_RAWtoESDTrf(userjobidsuf='bs',logfiles=False,name='rawtoesd_bs',prettyname='RAWtoESD BS')
            c3=DomainCalc_RAWtoESDTrf(userjobidsuf='rdo',logfiles=False,name='rawtoesd_rdo',prettyname='RAWtoESD RDO')
            c1.testvalid()
            c2.testvalid()
            c3.testvalid()
        except:
            print "WARNING: Problems importing DomainCalc_RAWtoESDTrf"
            c1=None
            c2=None
            c3=None
            raise
        if c1!=None:
            l+=[c1]
        if c2!=None:
            l+=[c2]
        if c3!=None:
            l+=[c3]
        ###############################################################
        c1=None
        c2=None
        c3=None
        #one for logfiles, one for BS_* userjobids and one for Zmumu_* userjobids
        try:
            from DomainCalc_MuCombPerf import DomainCalc_MuCombPerf
            c1=DomainCalc_MuCombPerf()
            c2=DomainCalc_MuCombPerf(userjobidprefix='BS',logfiles=False,name='mcp_bs',prettyname='MuonCombPerf BS')
            c3=DomainCalc_MuCombPerf(userjobidprefix='Zmumu',logfiles=False,name='mcp_zmumu',prettyname='MuonCombPerf Zmumu')
            c1.testvalid()
            c2.testvalid()
            c3.testvalid()
        except:
            print "WARNING: Problems importing DomainCalc_MuCombPerf"
            c1=None
            c2=None
            c3=None
            raise
        if c1!=None:
            l+=[c1]
        if c2!=None:
            l+=[c2]
        if c3!=None:
            l+=[c3]

        #Done:
        DomainCalcLib.__calcs = l

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# lumiCalculator.py
# utilities for calculation of integrated luminosity from information in COOL
# main class coolLumiCalc
# Richard Hawkings, started May 2007

import sys
from PyCool import cool

from CoolConvUtilities.AtlCoolLib import indirectOpen,RangeList
from DetectorStatus.DetStatusLib import DetStatusNames,DetStatusReq
from DetectorStatus.DetStatusCoolLib import statusCutsToRange

import LumiBlockComps.LumiQuery as LumiQuery

class RLBRange:
    """This class represents a range of lumiblocks within a single run"""
    def __init__(self,run,lb1,lb2):
        "Initialise to inclusive range lb1-lb2 in run run"
        self.run=run
        self.lb1=lb1
        self.lb2=lb2
        
    def __repr__(self):
        return 'Run %i LB [%i-%i]' % (self.run,self.lb1,self.lb2)

    def IOVRange(self):
        "Return IOV since,until corresponding to this lumiblock range"
        since=(self.run << 32)+self.lb1
        until=(self.run << 32)+self.lb2+1
        return (since,until)


class IOVCache:
    """An in-memory cache of items of data, each associated with an IOV
    Methods provided to add more data (specified with IOV) and to lookup the
    data for a particular time. Not optimised, suitable only for small amounts
    of data!"""
    def __init__(self):
        self.sinces=[]
        self.untils=[]
        self.data=[]
        
    def add(self,since,until,data):
        self.sinces+=[since]
        self.untils+=[until]
        self.data+=[data]

    def find(self,point):
        for i in range(0,len(self.sinces)):
            if (self.sinces[i]<=point and self.untils[i]>point):
                return self.data[i]
        return None

class lumiResult:
    """Class to hold luminosity calculation result"""
    def __init__(self,intL,l1acc,l2acc,l3acc,livetime,ngood,nbadstat):
        self.intL=intL
        self.l1acc=l1acc
        self.l2acc=l2acc
        self.l3acc=l3acc
        self.livetime=livetime
        self.ngood=ngood
        self.nbadstat=nbadstat
        
    def __add__(self,other):
        "addition of two lumiResults - accumulate totals"
        return lumiResult(self.intL+other.intL,self.l1acc+other.l1acc,self.l2acc+other.l2acc,self.l3acc+other.l3acc,self.livetime+other.livetime,self.ngood+other.ngood,self.nbadstat+other.nbadstat)
        
class coolLumiCalc:
    """Luminosity calculator from COOL - main utility class"""
    def __init__(self,cooldbconn,statusdbconn="",readoracle=False,loglevel=1,detStatus="",detStatusTag=""):
        """Initialisation of luminosity calculator
        Specify the COOL database string (e.g. COOLONL_TRIGGER/COMP200),
        COOL detector status connection string (if needed),
        whether to force Oracle (otherwise SQLite replicas might be used)
        and the detector status requirements and tag (if any).
        Status requirements given in the form 'SCTB G EMEC Y' etc
        """
        # open the COOL database instance (readonly)
        try:
            self.cooldb=indirectOpen(cooldbconn,True,readoracle,loglevel>1)
        except Exception, e:
            print e
            sys.exit(-1)
        # store other parameters
        self.loglevel=loglevel
        # setup detector status access if needed
        self.detstatus=detStatus
        self.detstatustag=detStatusTag
        if (self.detstatus!=""):
            # connect to detector status DB
            try:
                self.detstatusdb=indirectOpen(statusdbconn,True,readoracle,loglevel>1)
            except Exception,e:
                print e
                sys.exit(-1)

    def getLumiCache(self,since,until):
        "Return a lumicache (pairs of inst lumi, deltaT) for given range"
        lumicache=IOVCache()
        folderLUMI=self.cooldb.getFolder('/TRIGGER/LUMI/LBLEST')
        itr=folderLUMI.browseObjects(since,until-1,cool.ChannelSelection(0))
        while (itr.goToNext()):
            obj=itr.currentRef()
            # OnlineLumiDel is the integrated luminosity in the block
            # in units of nb^-1
            payload=obj.payload()
            instlumi=payload['OnlineLumiInst']
            if (instlumi>0):
                deltat=payload['OnlineLumiDel']/instlumi
            else:
                deltat=0
            lumicache.add(obj.since(),obj.until(),(instlumi,deltat))
        itr.close()
        return lumicache

    def calcFromList(self,triggername,lblist):
        """ Calculate the integrated luminosity for a list of LBs, returning
        a lumiResult object"""
        # setup the triggerlevel
        triggerlevel=self.triggerLevel(triggername)
        if triggerlevel is None: return None
        totalL=0
        totaltime=0.
        totalacc=3*[0]
        totalgoodblock=0
        totalbadblock=0
        # get counters folders
        folderLBCOUNTL1=self.cooldb.getFolder('/TRIGGER/LUMI/LVL1COUNTERS')
        folderLBCOUNTHLT=self.cooldb.getFolder('/TRIGGER/LUMI/HLTCOUNTERS')
        folderL1PRESCALE=self.cooldb.getFolder('/TRIGGER/LVL1/Prescales')
        
        for lbinfo in lblist:
            if (self.loglevel>0): print "Beginning calculation for",lbinfo
            # get the trigger configuration for this run
            runstat,chainnums,hltprescale=self._getChains(lbinfo.run,triggername,triggerlevel)
            if (self.loglevel>1): print "L1/2/3 chain numbers",chainnums[0],chainnums[1],chainnums[2]
            if (runstat):
                since,until=lbinfo.IOVRange()
                # check for detector status requirements
                if (self.detstatus!=""):
                    if (self.loglevel>0):
                        print "Applying detector status cuts: %s" % self.detstatus
                    gooddetstatus=statusCutsToRange(self.detstatusdb,'/GLOBAL/DETSTATUS/LBSUMM',since,until,self.detstatustag,self.detstatus)
                else:
                    gooddetstatus=RangeList(since,until)
                    
                if (self.loglevel>0): print "LumiB  L1-Acc  L2-Acc  L3-Acc   L1-pre   L2-pre   L3-pre LiveTime  IntL/nb-1"
                # get and cache the LVL1 prescales for this run
                l1precache=IOVCache()
                itr=folderL1PRESCALE.browseObjects(since,until-1,cool.ChannelSelection(chainnums[0]))
                while (itr.goToNext()):
                    obj=itr.currentRef()
                    l1precache.add(obj.since(),obj.until(),obj.payload()['Lvl1Prescale'])
                itr.close()
                # get and cache the luminosity estimates for this run
                # note these can have >1 LB intervals
                lumicache=self.getLumiCache(since,until)
                # loop through the LBs for this range
                # looping is driven by the LVL1COUNTERS folder which has
                # one entry for EACH lumiblock
                # assume that HLTCOUNTERS also have one entry for EACH block
                nblocks=0
                l1countitr=folderLBCOUNTL1.browseObjects(since,until-1,cool.ChannelSelection(chainnums[0]))
                if (triggerlevel>1):
                    l2countitr=folderLBCOUNTHLT.browseObjects(since,until-1,cool.ChannelSelection(chainnums[1]))
                if (triggerlevel>2):
                    l3countitr=folderLBCOUNTHLT.browseObjects(since,until-1,cool.ChannelSelection(chainnums[2]))
                while l1countitr.goToNext():
                    # access LVL1 information
                    l1countobj=l1countitr.currentRef()
                    lb=l1countobj.since() & 0xFFFFFFFF
                    l1payload=l1countobj.payload()
                    l1acc=l1payload['L1Accept']
                    # calculate livefraction from LVL1 ratios
                    # this needs to be improved to avoid rounding errors
                    if (l1payload['AfterPrescale']>0):
                        livefrac=float(l1payload['L1Accept'])/float(l1payload['AfterPrescale'])
                    else:
                        livefrac=1.
                    # access LVL2 information if needed
                    if (triggerlevel>1):
                        l2countitr.goToNext()
                        l2countobj=l2countitr.currentRef()
                        if (l2countobj.since()!=l1countobj.since()):
                            raise "L2/L1 counter synchronisation error"
                        l2payload=l2countobj.payload()
                        l2acc=l2payload['HLTAccept']
                    else:
                        l2acc=0
                    # access LVL3 information if needed
                    if (triggerlevel>2):
                        l3countitr.goToNext()
                        l3countobj=l3countitr.currentRef()
                        if (l3countobj.since()!=l1countobj.since()):
                            raise "L3/L1 counter synchronisation error"
                        l3payload=l3countobj.payload()
                        l3acc=l3payload['HLTAccept']
                    else:
                        l3acc=0
                    if (len(gooddetstatus.getAllowedRanges(l1countobj.since(),l1countobj.until()))>0):
                        # calculate intL for block
                        # lumi is being given in units of 10^33 cm^-2s^-1
                        # equivalent to 1 nb^-1s^-1
                        # instantaneous and time increment lumi
                        (lumi,deltat)=lumicache.find(l1countobj.since())
                        l1prescale=l1precache.find(l1countobj.since())
                        if (lumi is not None and l1prescale is not None):
                            # multiply by livetime in seconds to get
                            # intL in nb^-1
                            livetime=livefrac*deltat
                            intlumi=(lumi*livetime)/float(l1prescale*hltprescale[0]*hltprescale[1])
                            if (self.loglevel>1): print "%5i %7i %7i %7i %8i %8i %8i %8.2f %10.1f" % (lb,l1acc,l2acc,l3acc,l1prescale,hltprescale[0],hltprescale[1],livetime,intlumi)
                        else:
                            intlumi=0
                            print "%5i %7i %7i %7i <missing prescale or lumi>" %(lb,l1acc,l2acc,l3acc)
                        # accumulate statistics
                        totalacc[0]+=l1acc
                        totalacc[1]+=l2acc
                        totalacc[2]+=l3acc
                        totaltime+=livetime
                        totalL+=intlumi
                        totalgoodblock+=1
                    else:
                        totalbadblock+=1
                l1countitr.close()
            else:
                print "Trigger not defined for run",lbinfo.run
            if (self.loglevel>0): print "Rng-T %7i %7i %7i                            %8.2f %10.1f" % (totalacc[0],totalacc[1],totalacc[2],totaltime,totalL)
        return lumiResult(totalL,totalacc[0],totalacc[1],totalacc[2],totaltime,totalgoodblock,totalbadblock)

    def triggerLevel(self,trigname,quiet=False):
        """Extract the trigger level from the name (L1_x, L2_x or EF_x)
        Return 1 for unknown levels (after printing warning)"""
        if (trigname[:2]=='L1'): return 1
        if (trigname[:2]=='L2'): return 2
        if (trigname[:2]=='EF'): return 3
        if (not quiet): print 'WARNING: Trigger name',trigname,'does not define trigger level, assume L1'
        return 1

    def _getChains(self,run,triggername,triggerlevel):
        """Returns the trigger chain numbers for the specified trigger in this
        run, which are used as the channel indexes for the LVL1/Prescale
        and LUMI/LVL1COUNTERS and HLTCOUNTERS folders"""
        if (self.loglevel>1): print "Get chain numbers for run",run,triggername,"level",triggerlevel
        l1chain=-1
        l2chain=-1
        l3chain=-1
        l2prescale=1
        l3prescale=1
        badrun=False
        if (triggerlevel==3):
            lvl3name=triggername
            l3chain,l3prescale,lvl2name=self._getHLTChain(run,lvl3name,3)
            if (l3chain==-1):
                badrun=True
            else:
                l2chain,l2prescale,lvl1name=self._getHLTChain(run,lvl2name,2)
                if (l2chain==-1): badrun=True
        elif (triggerlevel==2):
            lvl2name=triggername
            l2chain,l2prescale,lvl1name=self._getHLTChain(run,lvl2name,2)
            if (l2chain==-1): badrun=True
        else:
            lvl1name=triggername
        # find LVL1 chain index
        if (not badrun):
            l1menu=self.cooldb.getFolder("/TRIGGER/LVL1/Menu")
            since,until=self.IOVFromRun(run)
            itr=l1menu.browseObjects(since,until-1,cool.ChannelSelection.all())
            while itr.goToNext():
                obj=itr.currentRef()
                if (obj.payload()['ItemName']==lvl1name):
                    l1chain=obj.channelId()
                    break
            if (l1chain==-1):
                print "LVL1 trigger %s not defined for run %i" % (lvl1name,run)
                badrun=True
            itr.close()
        return (not badrun,(l1chain,l2chain,l3chain),(l2prescale,l3prescale))

    def _getHLTChain(self,run,name,level):
        "access /TRIGGER/HLT/Menu folder to get chain counter for trigger name"
        hltmenu=self.cooldb.getFolder("/TRIGGER/HLT/Menu")
        since,until=self.IOVFromRun(run)
        itr=hltmenu.browseObjects(since,until-1,cool.ChannelSelection.all())
        chain=-1
        prescale=0
        lowername=""
        levelid=['L2','EF'][level-2]
        while itr.goToNext():
            obj=itr.currentRef()
            payload=obj.payload()
            if (payload['ChainName']==name and payload['TriggerLevel']==levelid):
                chain=payload['ChainCounter']
                if (level==3): chain+=10000
                prescale=payload['Prescale']
                lowername=payload['LowerChainName']
                break
        itr.close()
        if (self.loglevel>1): print "Trigger",name,"identifier chain",chain,"prescale",prescale,"lowername",lowername
        return (chain,prescale,lowername)

    def listTriggers(self,run,level=0):
        """Return a list of the available triggers for a particular run
        Restrict to particular level level argument!=0"""
        tlist=[]
        since,until=self.IOVFromRun(run)
        if (level<2):
            # read the LVL1 Menu folder
            l1menu=self.cooldb.getFolder("/TRIGGER/LVL1/Menu")
            itr=l1menu.browseObjects(since,until-1,cool.ChannelSelection.all())
            while itr.goToNext():
                obj=itr.currentRef()
                payload=obj.payload()
                item=payload['ItemName']
                if (item not in ['pass all','','-']):
                    tlist+=[payload['ItemName']]
            itr.close()
        if (level!=1):
            # read the HLTMenu folder
            hltmenu=self.cooldb.getFolder("/TRIGGER/HLT/Menu")
            itr=hltmenu.browseObjects(since,until-1,cool.ChannelSelection.all())
            while itr.goToNext():
                obj=itr.currentRef()
                payload=obj.payload()
                triglevel={'L2':2, 'EF':3}[payload['TriggerLevel']]
                if (triglevel==level or level==0):
                    tlist+=[payload['ChainName']]
            itr.close()
        return tlist

    def IOVFromRun(self,run):
        # form 1-run length interval of validity
        since=(run << 32)
        until=((run+1) << 32)
        return (since,until)

    def rlbFromFile(self,file,xml=False):
      if xml: return self.rlbFromXmlFile(file)
      else: return self.rlbFromRootFile(file)

    def rlbFromXmlFile(self,file):
      import os
      import ROOT
      from ROOT import gSystem
      CWD = os.getcwd()
      os.chdir(CWD)
      gSystem.Load('libGoodRunsListsLib')
      from ROOT import Root

      ## read the goodrunslist xml file
      reader = Root.TGoodRunsListReader(file)
      reader.Interpret()
      goodrunslist = reader.GetMergedGoodRunsList()
      goodrunslist.Summary(True)
      runs=goodrunslist.GetGoodRuns()
      
      rangelist=[]
      for i in range(len(runs)):
        run=runs[i]
        for j in range(len(run)):
             lumi_range=run[j]
             rangelist+=[RLBRange(run.GetRunNumber(),lumi_range.Begin(),lumi_range.End())]
      return rangelist
      
    def rlbFromRootFile(self,file):
        """Extract list of LBs from a ROOT file using LumiQuery and return
        as RLBRange objects"""
        lblist=LumiQuery.ListFromFile(file)
        rangelist=[]
        for lb in lblist:
            start=lb[0]
            stop=lb[1]
            if (start.run()==stop.run()):
                rangelist+=[RLBRange(start.run(),start.block(),stop.block())]
            else:
                print "ERROR: Multirun RLBRange not supported:",start,stop
        return rangelist

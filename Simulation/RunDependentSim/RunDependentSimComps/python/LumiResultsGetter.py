# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# module LumiResultsGetter
# Tool that gets all data for the RunDependentMC configuration.

import sys
from PyCool import cool

from CoolConvUtilities.AtlCoolLib import indirectOpen,RangeList
from DetectorStatus.DetStatusLib import DetStatusNames,DetStatusReq
from DetectorStatus.DetStatusCoolLib import statusCutsToRange

import LumiBlockComps.LumiQuery as LumiQuery
from  LumiBlockComps.LumiCalculator import RLBRange,IOVCache,lumiResult,coolLumiCalc
        
class coolLumiResultsGetter(coolLumiCalc):
    """Queries the DB to determine the RunDependentMC configuration for each luminosity block."""
    def __init__(self, cooldbconn, **kw):
        """Initialize the tool.
        cooldbconn = main database connection (to /TRIGGER/...) so should be COOLONL.
        kw =  argument list. Clients should stay up-to-date with CoolLumiCalc when they supply optional arguments. For more information, try
           help(LumiBlockComps.LumiCalculator.coolLumiCalc)
        child arguments: coolfolderlumi, useprescale
        """
        self.lumidbname=kw.pop('lumidb','COOLOFL_TRIGGER/COMP200')
        self.lumifoldername=kw.pop('coolfolderlumi','/TRIGGER/OFLLUMI/LBLESTOFL')
        self.lumitag = kw.pop('coolfoldertag','OflLumi-7TeV-002')
        self.lumimethod = kw.pop('lumimethod','ATLAS_PREFERRED')        
        self.useprescale=kw.pop('useprescale',False)
        
        kwkeep = kw.copy()
        kw.update({'readoracle':False,'loglevel':1,'detStatus':"",'detStatusTag':""})#supply defaults
        kw.update(kwkeep) #restore user choices
        print "Trying to open", cooldbconn
        coolLumiCalc.__init__(self, cooldbconn, **kw)
        #also need the online database
        try:
            print "Now trying to open", self.lumidbname
            self.cooldblumi=indirectOpen(self.lumidbname,True,kw.get('readoracle',False),kw.get('loglevel',1)>1)
        except Exception, e:
            print e
            sys.exit(-1)
        pass
    
    def getLumiMuCache(self,since,until):
        "Return a lumicache (pairs of inst lumi, eventsbx) for given range"
        lumicache=IOVCache()
        folderLUMI=self.cooldblumi.getFolder(self.lumifoldername)
        if (not folderLUMI.existsChannel(self.lumimethod)):
            if self.lumimethod != 'EXTERNAL': raise Error('This luminosity method is not supported by %s' % self.lumifoldername)
            chid=cool.ChannelSelection(998)
        else:
            chid = cool.ChannelSelection(folderLUMI.channelId(self.lumimethod))
        itr=folderLUMI.browseObjects(since,until-1,chid,self.lumitag)
        while (itr.goToNext()):
            obj=itr.currentRef()
            # OnlineLumiDel is the integrated luminosity in the block
            # in units of ub^-1/s (1e30 cm-2 s-1)
            payload=obj.payload()
            instlumi=payload['LBAvInstLumi']
            if (instlumi>0):
                evtsbx=payload['LBAvEvtsPerBX']
            else:
                evtsbx=0
                pass
            lumicache.add(obj.since(),obj.until(),(instlumi,evtsbx))
            pass
        itr.close()
        return lumicache
    def calcFromList(self,triggername,lblist):
        """ Calculate the integrated luminosity for a list of LBs, returning
        a list of (run,lb,lumiResult,evts) object tuples
        (note, this is the only difference in functionality between coolLumiCalc and coolLumiResultsGetter)
        """
        lumiResults = []
        # setup the triggerlevel
        triggerlevel=self.triggerLevel(triggername)
        if triggerlevel != 1:
            raise "Please use a L1 trigger for this tool."
        totalL=0
        totaltime=0.
        totalacc=3*[0]
        totalgoodblock=0
        totalbadblock=0
        # get counters, prescales LB timest folders
        folderLBCOUNTL1=self.cooldb.getFolder('/TRIGGER/LUMI/LVL1COUNTERS')
        folderL1PRESCALE=self.cooldb.getFolder('/TRIGGER/LVL1/Prescales')
        folderL1LB=self.cooldb.getFolder('/TRIGGER/LUMI/LBLB')
        # loop over LBList
        for lbinfo in lblist:            
            # get the trigger configuration for this run
            runstat,chainnums,hltprescale=self._getChains(lbinfo.run,triggername,triggerlevel)
            if (self.loglevel>1): print "L1 chain number", chainnums[0]
            if (runstat):
                since,until=lbinfo.IOVRange()
                # check for detector status requirements
                if (self.detstatus!=""):
                    if (self.loglevel>0):
                        print lbinfo, ": Applying detector status cuts: %s" % self.detstatus
                    gooddetstatus=statusCutsToRange(self.detstatusdb,'/GLOBAL/DETSTATUS/LBSUMM',since,until,self.detstatustag,self.detstatus)
                else:
                    gooddetstatus=RangeList(since,until)
                    
                if (self.loglevel>1): print "LumiB  L1-Acc  L1-pre  LiveTime  MeanInts IntL/ub-1"
                # get and cache the LVL1 prescales for this run; note these can have >1 LB intervals
                l1precache=IOVCache()
                itr=folderL1PRESCALE.browseObjects(since,until-1,cool.ChannelSelection(chainnums[0]))
                while (itr.goToNext()):
                    obj=itr.currentRef()
                    l1precache.add(obj.since(),obj.until(),obj.payload()['Lvl1Prescale'])
                itr.close()
                # get and cache the lumiosity estimates for this run; note these can have >1 LB intervals                
                lumicache=self.getLumiMuCache(since,until)                
                # loop through the LBs for this range
                # looping is driven by the LVL1COUNTERS folder which has 1 LB IOVs.
                # Should be matched by LBLB which is also 1 LB IOVs.
                nblocks=0
                l1countitr=folderLBCOUNTL1.browseObjects(since,until-1,cool.ChannelSelection(chainnums[0]))
                l1lbiter=folderL1LB.browseObjects(since,until-1,cool.ChannelSelection(0)) #just one channel in this folder.
                while l1countitr.goToNext():
                    # access LVL1 information
                    l1countobj=l1countitr.currentRef()
                    lb=l1countobj.since() & 0xFFFFFFFF
                    l1lbiter.goToNext()
                    lblbobj=l1lbiter.currentRef()
                    if (lblbobj.since()!=l1countobj.since()):
                        raise "L1 counter/lumiblock synchronisation error.  Cannot get length of Lumiblocks!"
                    l1payload=l1countobj.payload()
                    lblbpayload =lblbobj.payload()
                    l1acc=l1payload['L1Accept']
                    l1dt=(lblbpayload['EndTime']-lblbpayload['StartTime'])*1.0e-09
                    l1tstart=lblbpayload['StartTime']                    
                    # calculate livefraction from LVL1 (events accepted after mask and busy veto)/(events accepted by prescale)
                    # this will be a poor estimate when prescales are large.
                    if (l1payload['AfterPrescale']>0):
                        livefrac=float(l1payload['L1Accept'])/float(l1payload['AfterPrescale'])
                    else:
                        livefrac=1.
                        pass
                    l2acc=0
                    l3acc=0
                    if (len(gooddetstatus.getAllowedRanges(l1countobj.since(),l1countobj.until()))>0):
                        # calculate intL for block
                        # lumi is being given in units of 10^33 cm^-2s^-1
                        # equivalent to 1 nb^-1s^-1
                        # instantaneous and time increment lumi
                        try:
                            (lumi,evtsbx)=lumicache.find(l1countobj.since())
                        except TypeError:
                            if (self.lumimethod != 'EXTERNAL'): print "WARNING: No payload in", self.lumifoldername, "for run", lbinfo.run, "[", lb, "]!"
                            lumi=0
                            evtsbx=0
                            pass
                        l1prescale=l1precache.find(l1countobj.since())
                        if (lumi is not None and l1prescale is not None):
                            # multiply by livetime in seconds to get
                            # intL in nb^-1
                            # intlumi = nlumi * L1acc / (L1accAP * P )
                            livetime=livefrac*l1dt
                            if self.useprescale:
                                intlumi=(lumi*livetime)/(1.0 * l1prescale)
                                if not (intlumi >= 0): print "WARNING:",lbinfo.run,"[",lb,"]: bad lumi, prescale or livetime found:(IL,LV):", lumi,livetime
                            else:
                                intlumi=(lumi*livetime)
                                if not (intlumi >= 0): print "WARNING:",lbinfo.run,"[",lb,"]: bad lumi or livetime found:(IL,LV)", lumi,livetime
                                
                            if (self.loglevel>1): print "%5i %7i %8i %8.2f %8.7f %10.1f" % (lb,l1acc,l1prescale,livetime,evtsbx,intlumi)
                        elif (lumi is not None):
                            intlumi=(lumi*livetime)
                            if (self.loglevel>1): print "%5i %7i %8i %8s %8.7f %10.if <missing prescale>" %(lb,l1acc,"??",livetime,evtsbx,intlumi)
                            if not (intlumi >= 0): print "WARNING:",lbinfo.run,"[",lb,"]: bad lumi or livetime found:(IL,LV)", lumi,livetime
                        else:
                            intlumi=0
                            if (self.loglevel>1): print "%5i %7i %8i %8s %8s %10s <missing prescale>" %(lb,l1acc,"??",livetime,"??","??")
                        lumiResults.append( (lbinfo.run, int(lb), lumiResult(intlumi,l1acc,0,0,livetime,1,0), evtsbx, l1tstart ) )   
                        # accumulate statistics
                        totalacc[0]+=l1acc
                        totaltime+=livetime
                        totalL+=intlumi
                        totalgoodblock+=1
                    else: #this was a bad run detstatus
                        totalbadblock+=1
                        pass
                    pass
                l1countitr.close()
            else:
                print "WARNING: Trigger not defined for run",lbinfo.run
                pass
            if (self.loglevel>0): print "Running total after %24s:" % lbinfo, " %7i events; %8.2f seconds; %10.1f (nb^-1)" % (totalacc[0],totaltime,totalL)
            pass #end of loop over iovs
        return lumiResults
    

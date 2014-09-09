#!/usr/bin/env python
# detStatus_query.py - tool for querying detector status in COOL conditions DB
# Richard Hawkings, 5/2/07
# execute without arguments for help
import os,sys
from PyCool import cool,coral
from CoolConvUtilities import AtlCoolLib
from DetectorStatus import DetStatusLib

class queryStatus(AtlCoolLib.coolTool):
    def setup(self,args):
        # set values of non-optional parameters
        self.foldername=''
        self.tag=str(args[0])
        self.dets=args[1:]
        self.summ=False
        self.graphics=False
        self.pictfile=""
        # access to name encoding
        self.namelookup=DetStatusLib.DetStatusNames()

    def usage(self):
        """ Define the additional syntax for options """
        self._usage1()
        print 'COOL_tag det1 {det2 ..}'
        print 'det specifies a flag or flags to be checked (ALL=all flags)'
        print '--folder <folder>: Set foldername to be used'
        print '--summ : Produce summary statistics instead of listing all flags'
        print '--graphics : Display status matrix graphically'
        print '--pictfile=file : Write matrix to file (.eps,.gif etc)'
        self._usage2()

    def procopts(self,opts):
        "Process the additional non-standard options"
        for o,a in opts:
            if (o=='--folder'):
                self.foldername=str(a)
            elif (o=='--summ'):
                self.summ=True
            elif (o=='--graphics'):
                self.graphics=True
                self.summ=True
            elif (o=='--pictfile'):
                self.pictfile=str(a)
                self.graphics=True
                self.summ=True

    def execute(self):
        # find folder name from timestamp options
        if (self.foldername==''):
            folder=DetStatusLib.folderName(self.runLumi)
        else:
            folder=self.foldername
        # setup channel object
        (chanlist,chansel)=self.getChannels(self.dets)
        if (self.summ):
            # prepare for summary - get number of LBs in each folder
            lumiblocks=self.getLBs()
            runlist=lumiblocks.keys()
            runlist.sort()
            lbtot=0
            lboffset={}
            for run in runlist:
                lb=lumiblocks[run]
                lboffset[run]=lbtot
                lbtot+=lb
            statusbychan={}
            chanindex={}
            index=0
            for chan in chanlist:
                # statusbychan gives [nBLACK,nUNKNOWN,nRED,nYELLOW,nGREEN]
                statusbychan[chan]=[0,0,0,0,0]
                chanindex[chan]=index
                index+=1
            if (self.graphics):
                hist,canvas=self.initGraphics(len(chanlist),lbtot)
        print ">== Reading status from folder %s tag %s" % (folder,self.tag)
        try:
            cfolder=self.db.getFolder(folder)
            tag=self.tag
            if (tag=="HEAD"): tag=""
            objs=cfolder.browseObjects(self.since,self.until,chansel,tag)
            while objs.goToNext():
                obj=objs.currentRef()
                chan=obj.channelId()
                if chan not in chanlist: continue
                name=self.namelookup.name(obj.channelId())
                since=obj.since()
                until=obj.until()
                sincerun=since >> 32
                untilrun=until >> 32
                payload=obj.payload()
                code=payload['Code']
                if (code>=0): code=code & 3
                deadFrac=payload['deadFrac']
                deadThrust=payload['Thrust']
                if (self.summ):
                    # increment the counts of all appropriate runs
                    for (run,lbn) in lumiblocks.iteritems():
                        # slice represents IOV of LBs in this run
                        startslice=1+(run<<32)
                        endslice=1+(run<<32)+lbn
                        comstart=max(startslice,since)
                        comend=min(endslice,until)
                        nlb=comend-comstart
                        if nlb>0:
                            #print "Overlap %i LB for db [%i %i] [%i %i] in run %i with %i LB status %i %i" % (nlb,since >> 32, since & 0xFFFFFFFF, until >> 32, until & 0xFFFFFFFF, run,lbn,code,chan)
                            ((statusbychan[chan])[code+1])+=nlb
                            if (self.graphics):
                                for i in range(0,nlb):
                                    ixfill=i+lboffset[run]+(comstart & 0xFFFFFFFF)
                                    
                                    hist.Fill(chanindex[chan]+1,ixfill,[7,4,41,30,45][code+1])
                else:
                    # printout data
                    print name,":",
                    if (self.runLumi):
                        print '[',(since >> 32),',',(since % (1 << 32) ),'] - [',(until >> 32),',',(until % (1 << 32) ),' ]',
                    else:
                        print '{',AtlCoolLib.timeString(since),',',AtlCoolLib.timeString(until),'} ',
                    print '%s %5.3f %5.3f' % (DetStatusLib.colour(code),deadFrac,deadThrust),
                    if ('Comment' in payload.keys()):
                        print payload['Comment'],
                    if ('NConfig' in payload.keys()):
                        print "(%i/%i)" % (payload['NConfig'],payload['NWorking']),
                    print
        except Exception,e:
            print e
            print 'Problem accessing COOL data'
            sys.exit(1)
        # produce summary if required
        if (self.summ):
            print ">=== Status flags for runs %i to %i with total of %i LBs\n>=== Folder %s tag %s" % (self.runmin,self.runmax,lbtot,self.foldername,self.tag)
            print "Chan Flag  NMiss (frac) NBlck (frac) NUnkn (frac) NRed  (frac) NYlw  (frac) NGrn  (frac)"
            for chan in chanlist:
                counters=statusbychan[chan]
                cfrac=5*[0]
                nmiss=lbtot
                for i in range(0,5):
                    cfrac[i]=float(counters[i])/float(lbtot)
                    nmiss-=counters[i]
                print "%i %6s %6i %5.3f %6i %5.3f %6i %5.3f %6i %5.3f %6i %5.3f %6i %5.3f" % (chan,self.namelookup.name(chan),nmiss,float(nmiss)/float(lbtot),counters[0],cfrac[0],counters[1],cfrac[1],counters[2],cfrac[2],counters[3],cfrac[3],counters[4],cfrac[4])
        if (self.graphics):
            hist.Draw("COL")
            self.addAxis(canvas,chanindex,chanlist,runlist,lboffset,lbtot)
            canvas.Update()
            if (self.pictfile!=""):
                print "Writing picture to %s" % self.pictfile
                canvas.Print(self.pictfile)
            raw_input("Press return to remove canvas when done")
            
    def getLBs(self):
        "Get the number of lumiblocks in each run, using the LBLB folder"
        lbdata={}
        tdaqdbname='COOLONL_TRIGGER/COMP200'
        tdaqdb=AtlCoolLib.indirectOpen(tdaqdbname,True,True)
        if tdaqdb is None:
            print "ERROR - cannot access %s for run/LB mapping lookup" % tdaqdbname
        try:
            lbfolder=tdaqdb.getFolder('/TRIGGER/LUMI/LBLB')
            objs=lbfolder.browseObjects(self.since,self.until,cool.ChannelSelection(0))
            oldrun=0
            maxlb=0
            while objs.goToNext():
                obj=objs.currentRef()
                iovsince=obj.since()
                run=int(iovsince >> 32)
                lb=int(iovsince & 0xFFFFFFFF)
                if (run!=oldrun):
                    if oldrun!=0:
                        lbdata[oldrun]=maxlb
                    maxlb=0
                    oldrun=run
                if lb>maxlb: maxlb=lb
            if oldrun!=0: lbdata[oldrun]=maxlb
            
        except Exception,e:
            print e
            print "ERROR accessing data for run/LB mapping"
        return lbdata

    def getChannels(self,dets):
        "Return a list of the channels to be selected, and COOL selector"
        # COOL selector may give more channels
        chanlist=[]
        for det in dets:
            updet=det.upper()
            if updet in ['ALL','A']:
                chanlist+=self.namelookup.allNums()
            else:
                chanlist+=self.namelookup.nums(updet)
        chanlist.sort()
        chansel=cool.ChannelSelection(min(chanlist),max(chanlist))
        return (chanlist,chansel)

    def initGraphics(self,ndet,nlb):
        "Initialise graphics and setup histogram for status matrix"
        from ROOT import gROOT,TCanvas,TH2F,gStyle
        hist=TH2F("DetectorStatus","%s tag %s runs %i to %i (%i LBs)" % (self.foldername,self.tag,self.runmin,self.runmax,nlb),ndet+1,0.5,ndet+1.5,nlb,0.5,nlb+0.5)
        hist.Fill(ndet+1,1,49.)
        hist.SetNdivisions(0)
        gROOT.SetStyle("Plain")
        gStyle.SetPalette(0)
        gStyle.SetOptStat(0)
        gStyle.SetOptFit(0)
        canvas=TCanvas(self.foldername,self.foldername,0,0,500,600)
        return (hist,canvas)
    
    def addAxis(self,canvas,chanindex,chanlist,runlist,lboffset,lbtot):
        from ROOT import TLatex
        txt=TLatex()
        txt.SetNDC()
        txt.SetTextAngle(90.)
        txt.SetTextSize(0.012)
        for chan in chanlist:
            txt.DrawLatex(0.11+0.8*float(chanindex[chan])/(len(chanindex)+1),0.06,self.namelookup.name(chan))
        txt.SetTextAngle(0.)
        txt.SetTextSize(0.02)
        ndiv=1+len(lboffset)/20
        j=0
        for run in runlist:
            lb=lboffset[run]
            j+=1
            if (j==ndiv):
                txt.DrawLatex(0.91,0.11+0.8*float(lb)/lbtot,str(run))
                j=0
        
            
        
mytool=queryStatus('detStatus_query.py',True,3,99,['summ','graphics','folder=','pictfile='])


        

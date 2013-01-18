#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#  Author: Edward.Moyse@cern.ch (based on pmb-domain-analysertt.py by T. Kittelmann)
#  Initial version: Jan 2010

import os,sys,getopt

def usage(exitcode=0):
    if exitcode!=0:
        print ""
        print "Run with -h or --help for usage instructions."
        print ""
        sys.exit(exitcode)
    if not 'calcnames' in dir():
        from MuonRecRTT.CoolDozerAccessUtils import PhysMonDozerJobDef
        calcnames=PhysMonDozerJobDef().getJobs()
    print "Purpose:"
    print "  This script allows the user to investigate the per-alg performance of RTT jobs"
   
    sys.exit(exitcode)

# def getJobList(ndays,pretendDateStr,doBlackListing,rttType,rttCache,rttCMTCONFIG, jobcalc):
def getJobList(ndays,pretendDateStr,doBlackListing,rttType,rttCache,rttCMTCONFIG, coll, trendId):
    from PmbUtils.CoolDozerAccessUtils import CDTimeHelper, CDDataRetrievalHelper
    from PmbUtils.ValuePerDomainCalcUtils import Job,JobList,TimeLabel
    import os.path

    dbjobkey=os.path.join(rttType,'build',rttCMTCONFIG,rttCache)
    blacklist=[]
    # if jobcalc.getName()=='rdotoesd':
    #     #Fixme: To domain calc file
    #     trigjob='idmuoncalomuoncombegammajettautrigger'
    #     persjob='idmuoncalomuoncombegammajettautriggerpers'
    #     for i in [trigjob,persjob]:
    #         for d in ['2009-02-20','2009-02-21','2009-02-22','2009-02-26','2009-04-25',
    #                   '2009-04-26','2009-04-27','2009-04-28','2009-06-17','2009-06-18',
    #                   '2009-10-16','2009-10-17']:
    #             blacklist+=[(i,d)]
    #     for i in ['2009-08-14','2009-11-11','2009-11-12','2009-11-15','2009-11-23','2009-11-24','2009-11-26','2009-10-28','2009-10-30']:
    #         blacklist+=[(trigjob,i)]

    #figure out time internal
    if pretendDateStr==None:
        from PmbUtils.ValuePerDomainCalcUtils import TimeLabel
        pretendDateStr=TimeLabel(date=CDTimeHelper.now()).getLabel()
    until=CDTimeHelper.fromDateString(pretendDateStr)

    since=CDTimeHelper.nDaysBefore(ndays-1,until)

    #Retrieve from DB and put in joblist:
    # from PmbUtils.DomainCalc_MuonReco import DomainCalc_MuonReco
    # jobcalc=DomainCalc_MuonReco()
    # joblist = JobList(jobcalc)
    joblist=[]
    # jobs=CDDataRetrievalHelper.getJobsFromDB(jobcalc.getCDJobDefinitions(dbjobkey),since=since,until=until)
    from MuonRecRTT.CoolDozerAccessUtils import PhysMonDozerJobDef
    
    print "dbjobkey:",dbjobkey
    #CDDataRetrievalHelper.dumpDBContents()
    # collections = ["MooreTracks", "ConvertedMBoyTracks", "MuGirlRefittedTracks", "ConvertedMuIdCBTracks", "ConvertedStacoTracks"]
    jobdef=PhysMonDozerJobDef(dbjobkey=dbjobkey,dbjobpackage='MuonSpectrometer/MuonValidation/MuonRecValidation/MuonRecRTT', trendid=trendId,jobname=coll, dbChannelName=coll)
    jobs=CDDataRetrievalHelper.getJobsFromDB([jobdef] ,since=since,until=until)
    # print "jobs=",jobs
    for job in jobs:
        ok=True
        if doBlackListing:
            i=job.getIDStr()
            d=job.getTime().getLabel()
            for b in blacklist:
                if (i,d)==b:
                    ok=False
                    break
        if ok:
            joblist.append(job)
    return joblist

def dateFormatOk(datestr):
    from PmbUtils.CoolDozerAccessUtils import CDTimeHelper
    return CDTimeHelper.fromDateString(datestr) != 0

def plotResults(releases,collections= ["MooreTracks", "ConvertedMBoyTracks"],values=["Trk/Evt", "Ch2/dof/T"],title="MuonRecRTT Performance",filename="Results.png", rttCache="AtlasProduction", rttCMTCONFIG='i686-slc5-gcc43-opt', ndays=30, trendId="Zmumu_MboyAndMoreOnly"):
  print 'Plotting last ',ndays,' results for the following releases:',releases,', collections: ',collections,' and values: ',values
  
  doBlackListing=True
  metaJobList={}
  for release in releases:
    for coll in collections:
      joblist=getJobList(ndays,None,doBlackListing,release,rttCache,rttCMTCONFIG,coll,trendId)
      metaJobList[release]=joblist
  
  plotJobLists(filename, title, metaJobList, collections, values)

def plotJobLists(filename, title, metajoblist, collections, values):
    from PmbUtils.ValuePerDomainCalcUtils import PlottableResult, JobListPlotter, TimeLabel
    
    useReleaseInColname=True
    if (len(metajoblist)==1): useReleaseInColname=False

    useCollectionInColname=True
    if (len(collections)==1): useCollectionInColname=False
    
    # title = '%s ( %s events)' % (collection, int(firstSummary[collection].get('Events')) )
    # print 'Making plot for %s with %s summaries' % (title, len(summaries))
    
    # values = ["Trk/Evt", "Ch2/dof/T"]
    # collections = ["MooreTracks"]
    colnames=[]
    for release,joblist in metajoblist.iteritems():
      for value in values:
        for coll in collections:
          colname = ''
          if (useCollectionInColname): colname=coll
          colname= colname+" "+value
          if (useReleaseInColname): colname=colname+" "+release
          colnames.append( colname)
          
    print "Will be using the following colname",colnames
    
    result = PlottableResult(colnames, title)
    i=0
    for release,joblist in metajoblist.iteritems():
      # print release
      for job in joblist:
          i+=1
          for value in values:
            idstr= job.getIDStr()
            if idstr in collections:
              meas = job.getMeasurement(value)
              colname = ''
              if (useCollectionInColname): colname=coll
              colname= colname+" "+value
              if (useReleaseInColname): colname=colname+" "+release
              result.addPoint(colname=colname, value=meas.getValue(), time=job.getTime())
            else:
              print "Didn't find ",idstr
    # print "result.getNColumns()", result.getNColumns()
    # print "result.getTimes()", result.getTimes()
    JobListPlotter().produceTimeDevelopmentPlot(result,filename)

def main(argv):

    # doBlackListing=False
    # rttCache="AtlasProduction"
    # rttCMTCONFIG='i686-slc4-gcc34-opt'
    # 
    # 
    # joblist=getJobList(30,None,doBlackListing,'devval',rttCache,rttCMTCONFIG)
    # joblist2 = getJobList(30,None,doBlackListing,'15.6.X.Y', rttCache,rttCMTCONFIG)
    # 
    # metaJobList={'devval':joblist, '15.6.X.Y':joblist2}
    # 
    # # for job in joblist2:
    # #   joblist.append(job)
    # # for job in joblist:
    # #   print job.getTime()
    # #   print job.getMeasurement("Tracks")
    # 
    # plotSummariesForCollection("Results.png", metaJobList)
    releases = ['devval','15.6.X.Y']
    #  All values :
    # values=[
    # "Scat/Tk", 
    # "Ch2/dof/T",
    # "Trk/Evt", 
    # "Hole/Tk", 
    # "TrigEta/T", 
    # "Tracks",
    # "Eta/Trk",
    # "Phi/Trk", 
    # "Hit/Trk", 
    # "Events"
    # ]
    
    values=[
    "Scat/Tk", 
    "Ch2/dof/T",
    "Trk/Evt", 
    "Hole/Tk", 
    "TrigEta/T", 
    "Eta/Trk",
    "Phi/Trk", 
    "Hit/Trk", 
    ]
    
    plotResults(['devval'],collections= ["MooreTracks"],values=values,title="Moore Performance (devval)",filename="Moore Results.png")
    plotResults(['devval'],collections= ["ConvertedMBoyTracks"],values=values,title="Mboy Performance (devval)",filename="Muonboy Results.png")
    
    plotResults(releases,collections= ["MooreTracks"],values=["Tracks","Events"],title="Moore Performance",filename="Moore Results2.png")

if __name__ == "__main__":
    doprofile=False
    if doprofile:
        import cProfile
        cProfile.run("main(sys.argv[1:])", sort=1)
    else:
        main(sys.argv[1:])

#Ideas: Optional legend with numbers for pie-chart
#       Option to explode a given of the domains in the pie-chart.
#       Option for pretty-labels in plots?
#       Vertical grid lines in hist plots should be with one week/one month intervals.


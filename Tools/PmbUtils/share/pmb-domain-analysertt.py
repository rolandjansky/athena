#!/usr/bin/env python

#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: August 2009

import os,sys,getopt
defaultType='rawtoesd_bs'

__cachemap= {'prod':'AtlasProduction', 'tier0':'AtlasTier0','p1hlt':'AtlasP1HLT','cafhlt':'AtlasCAFHLT'}

def usage(exitcode=0):
    #assert False
    if exitcode!=0:
        print ""
        print "Run with -h or --help for usage instructions."
        print ""
        sys.exit(exitcode)
    if not 'calcnames' in dir():
        from PmbUtils.DomainCalcLib import DomainCalcLib
        calcnames=DomainCalcLib().getAllCalcNames(requireRTT=True)
    print "Purpose:"
    print "  This script allows the user to investigate the per-domain performance"
    print "  (mem or cpu) based on the output of a number of RTT jobs."
    print ""
    print "Usage:"
    print ""
    print "  -h,--help : Display this help"
    print ""
    print "  --numberofdays=X : Number of days to go back [default 30]"
    print "  --date=YYYY-MM-DD : If set, the script will return the same result as if it was"
    print "                      run on the specified date rather than today. Useful"
    print "                      for production of e.g. historical pie-charts."
    print ""
    print "  --table=X : Print table with results and save copy in file X"
    print "               - put % in front of filename to suppress output to stdout"
    print "               - use a filename of 'show' to just display to stdout"
    print ""
    print "  --histplot=X : Print history plot to image file X [extension determines image type]"
    print "  --hp-log : Use log-scale in history plot."
    print "  --hp-min=X : Set min of the vertical scale in history plot to X megabytes [default 0]."
    print "  --hp-max=X : Set max of the vertical scale in history plot to X megabytes [default auto]."
    print "  --nototal : Don't display an estimate of the total memory consumption."
    print "  --totaloffset=X : Offset the total by X mb. 0 Turns off offsetting and 'auto' makes"
    print "                    the program try to figure out a decent value by itself [default is auto]."
    print ""
    print "  --pie=X : Print pie-chart to image file X [extension determines image type]"
    print ""
    print "  --bar=filename:date1,date2,... : Produce bar chart in 'filename' comparing data at the given dates."
    print "                                   Dates must be in the YYYY-MM-DD format, and --numberofdays"
    print "                                   must be big enough that it includes all dates"
    print ""
    print "  --makestdoutput=dirname : Produce a variety of plots and tables in target directory (used for webpage)."
    print "  --stdoutputpostfix=X : Extra postfix for files created with makestdoutput (default none)"
    print ""
    print "  --jobs : Show data for the raw jobs instead of the derived per-domain results"
    print "  --cpu : Show cpu/evt usage rather than vmem."
    print "  --malloc : Show malloc/evt usage rather than vmem."
    print "  --memleak : Show memleak/evt rather than vmem."
    print "  --si2k : Show Si2k constant for machine where job ran."
    print "  --build=X : Look at data from build X. Example values are 'dev, 'devval', '15.3.X.Y-VAL', ... [default 'dev']"
    print "  --cache=X : Look at data from cache type X ('%s') [default 'prod']"%("', '".join(__cachemap.keys()))
    print "  --cmtconfig=X : Show data for cmtconfig X (default is i686-slc5-gcc43-opt)."
    print ""
    print "  --type=X : Type of jobs to look at Look at [default is "+defaultType+']'
    print "             Available types are:"
    for calcname in calcnames:
        print "                                  --type="+calcname
    print ""
    print "  --noblacklist : Turn off the default blacklisting of certain failing jobs."
    print ""
    print "  --domainvmemwarnings=email1,email2,... : Check for suspicious increases in domain vmem"
    print "                                           and send out warnings (for cron job). Put an entry 'show'"
    print "                                           to display the warnings."
    print "                                           Put to 'defaultlist' to send to the emails defined for the job"
    print "                                           type (aborting silently if none)."
    print "  --warnblockfile=X : Use file X to log emailed warnings and to block future duplicates sent."
    print "  --lastdata=X : Output the last few days of job data into X (needed for cron job)."
    print "  --dumpdb : Debug utility for dumping the contents of the RTT dozer DB and quitting."
    print "  --nodefaultaction : Disable the behaviour where running without arguments defaults to showing a table in stdout."
    print ""
    print "Examples of flags:"
    print ""
    print "  1) Print and store in mytable.txt a table summarising the mem-per-domain"
    print "      values based on RTT jobs going back as much as 20 days:"
    print ""
    print "      [cmdname] --table=mytable.txt --numberofdays=20"
    print ""
    print "  2) Make graph of the mem-per-domain evolution for the past 30 days, and store it in mygraph.png:"
    print ""
    print "      [cmdname] --histplot=mygraph.png --numberofdays=30"
    print ""
    print "  3) As in 2), but go back as far as possible, and also output a pie chart (using the last"
    print "     available data-point for each sub-domain). This time the output is requested as .svg:"
    print ""
    print "      [cmdname] --histplot=mygraph.svg --numberofdays=99999 --pie=mypie.svg"
    print ""
    print "  4) As in 2), but go back as far as possible, and also output a pie chart (using the last"
    print "     available data-point for each sub-domain). This time the output is requested as .svg:"
    print ""
    print "      [cmdname] --numberofdays=999999 --bar=mybarchart:2009-02-24,2009-05-08,2009-08-05"
    print ""
    print "Please send comments/questions/bug-reports to thomas.kittelmann@cern.ch"
    sys.exit(exitcode)

def getJobList(ndays,pretendDateStr,doBlackListing,rttType,rttCache,rttCMTCONFIG,jobcalc):
    from PmbUtils.CoolDozerAccessUtils import CDTimeHelper, CDDataRetrievalHelper
    from PmbUtils.ValuePerDomainCalcUtils import Job,JobList,TimeLabel
    import os.path

    dbjobkey=os.path.join(rttType,'build',rttCMTCONFIG,rttCache)
    blacklist=[]
    #if jobcalc.getName()=='rdotoesd':
    #    #Fixme: To domain calc file
    #    trigjob='idmuoncalomuoncombegammajettautrigger'
    #    persjob='idmuoncalomuoncombegammajettautriggerpers'
    #    for i in [trigjob,persjob]:
    #        for d in ['2009-02-20','2009-02-21','2009-02-22','2009-02-26','2009-04-25',
    #                  '2009-04-26','2009-04-27','2009-04-28','2009-06-17','2009-06-18',
    #                  '2009-10-16','2009-10-17']:
    #            blacklist+=[(i,d)]
    #    for i in ['2009-08-14','2009-11-11','2009-11-12','2009-11-15','2009-11-23','2009-11-24','2009-11-26','2009-10-28','2009-10-30',
    #              '2010-02-09','2010-01-12','2010-01-13','2010-01-14','2010-04-12','2010-04-13','2010-02-16']:
    #        blacklist+=[(trigjob,i)]
    #    blacklist+=[(persjob,'2009-06-25'),(None,'2010-03-08')]

    #figure out time internal
    if pretendDateStr==None:
        from PmbUtils.ValuePerDomainCalcUtils import TimeLabel
        pretendDateStr=TimeLabel(date=CDTimeHelper.now()).getLabel()
    until=CDTimeHelper.fromDateString(pretendDateStr)

    since=CDTimeHelper.nDaysBefore(ndays-1,until)

    #Retrieve from DB and put in joblist:
    joblist = JobList(jobcalc)
    jobs=CDDataRetrievalHelper.getJobsFromDB(jobcalc.getCDJobDefinitions(dbjobkey),since=since,until=until)
    for job in jobs:
        ok=True
        if doBlackListing:
            i=job.getIDStr()
            d=job.getTime().getLabel()
            for b in blacklist:
                if (i,d)==b or (b[0]==None and b[1]==d):
                    ok=False
                    break
        if ok:
            joblist.addJob(job)
    return joblist

def dateFormatOk(datestr):
    from PmbUtils.CoolDozerAccessUtils import CDTimeHelper
    return CDTimeHelper.fromDateString(datestr) != 0

def main(argv):

    from PmbUtils.DomainCalcLib import DomainCalcLib
    calclib = DomainCalcLib()
    calcnames=calclib.getAllCalcNames(requireRTT=True)

    #######################
    #  Parse command line #
    #######################

    if "-h" in argv or "--help" in argv:
        usage(0)
    try:
        opts, args = getopt.getopt(argv, "h", ["help", "numberofdays=","date=","table=","histplot=","pie=",
                                               "hp-log","hp-min=","hp-max=","nototal","totaloffset=",
                                               "cpu","malloc","memleak","si2k","jobs","bar=","noblacklist",'domainvmemwarnings=',
                                               'warnblockfile=','lastdata=',
                                               'build=','cache=','cmtconfig=','type=','makestdoutput=','stdoutputpostfix=','dumpdb',
                                               'nodefaultaction'])
    except getopt.GetoptError:
        usage(1)

    tableFile=None
    showTable=False
    historyFile=None
    pieFile=None
    ndays=30
    doLog=False
    doDBDump=False
    clipMinVal=None
    clipMaxVal=None
    doTotal=True
    totalOffset='auto'
    pretendDate=None
    doJobsRatherThanDomains=False
    doCPURatherThanVMEM=False
    doMallocRatherThanVMEM=False
    doMemleakRatherThanVMEM=False
    doSi2KRatherThanVMEM=False
    doBlackListing=True
    barChartDates=None
    barChartFileName=None
    domainVMEMWarnings=None
    warnBlockFile=None
    lastDataFile=None
    rttType=None
    rttCacheShort=None
    rttCMTCONFIG=None
    datatype=None
    optNoDefaultAction=False
    #defaultType
    optAddStdPlotsAndTablesDir=None
    optStdPlotsAndTablesPostFix=''
    #title=None gives a default title in all of the following
    displayJobTables=[]#list of (showonstdout,filename,fieldname,title)#title=None gives a default title
    displayDomainTables=[]#list of (showonstdout,filename,fieldname,dototal,title) #title=None gives a default title
    displayDomainPieCharts=[]#List of (filename,label,fieldname,title) # label=None means a pie-chart using the most recent label for all domains
    displayDomainHistPlots=[]#list of (filename,fieldname,totaloffset,clipmin,clipmax,title,domainoffsets) #clipmin/clipmax==None => ignored
    displayJobHistPlots=[]#list of (filename,fieldname,clipmin,clipmax,title) #clipmin/clipmax==None => ignored
    displaySingleFieldMultiLabelBarCharts=[]#list of (filename,fieldname,dojobs,totaloffset,timelabels,title)# timelabels=None for all, otherwise a list (strings or timelabels)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
        elif opt in ("--numberofdays"):
            ndays = int(arg)
        elif opt in ("--bar"):
            barChartDates=[]
            tmp=arg.split(':')
            if len(tmp)!=2:
                usage(1)
            barChartFileName=tmp[0]
            for datestr in tmp[1].split(','):
                if dateFormatOk(datestr):
                    barChartDates+=[datestr]
                else:
                    usage(1)
        elif opt in ("--table"):
            s=arg
            if len(s)<1:
                usage(1)
            if s=='show':
                showTable=True
            else:
                if s[0]=='%':
                    tableFile=s[1:]
                else:
                    tableFile=s
                    showTable=True
        elif opt in ("--histplot"):
            historyFile = arg
        elif opt in ("--pie"):
            pieFile = arg
        elif opt in ("--hp-log"):
            doLog = True
        elif opt in ("--dumpdb"):
            doDBDump = True
        elif opt in ("--hp-min"):
            clipMinVal=float(arg)
        elif opt in ("--hp-max"):
            clipMaxVal=float(arg)
        elif opt in ("--nototal"):
            doTotal=False
        elif opt in ("--cpu"):
            doCPURatherThanVMEM=True
        elif opt in ("--malloc"):
            doMallocRatherThanVMEM=True
        elif opt in ("--memleak"):
            doMemleakRatherThanVMEM=True
        elif opt in ("--si2k"):
            doSi2KRatherThanVMEM=True
        elif opt in ("--noblacklist"):
            doBlackListing=False
        elif opt in ("--jobs"):
            doJobsRatherThanDomains=True
        elif opt in ("--totaloffset"):
            totalOffset=int(arg) if arg!='auto' else 'auto'
        elif opt in ("--date"):
            pretendDate=arg
        elif opt in ("--domainvmemwarnings"):
            domainVMEMWarnings=arg
        elif opt in ("--warnblockfile"):
            warnBlockFile=arg
        elif opt in ("--lastdata"):
            lastDataFile=arg
        elif opt in ("--build"):
            if rttType!=None:
                usage(1)
            rttType=arg
        elif opt in ("--cache"):
            if rttCacheShort!=None:
                usage(1)
            rttCacheShort=arg
        elif opt in ("--cmtconfig="):
            if rttCMTCONFIG!=None:
                usage(1)
            rttCMTCONFIG=arg
        elif opt in ("--type"):
            if datatype!=None:
                usage(1)
            datatype=arg
        elif opt in ("--stdoutputpostfix"):
            if optStdPlotsAndTablesPostFix!='':
                print "Old:",optStdPlotsAndTablesPostFix
                usage(1)
            optStdPlotsAndTablesPostFix=str(arg)
        elif opt in ("--makestdoutput"):
            if optAddStdPlotsAndTablesDir!=None:
                usage(1)
            optAddStdPlotsAndTablesDir=os.path.expanduser(arg)
            if not os.path.isdir(optAddStdPlotsAndTablesDir):
                print "Unknown directory",optAddStdPlotsAndTablesDir
                usage(1)
        elif opt in ("--nodefaultaction'"):
            if optNoDefaultAction:
                usage(1)
            optNoDefaultAction=True

    nvars=0
    if doCPURatherThanVMEM: nvars+=1
    if doMallocRatherThanVMEM: nvars+=1
    if doMemleakRatherThanVMEM: nvars+=1
    if doSi2KRatherThanVMEM: nvars+=1
    if nvars>1:
        usage(1)

    if rttType==None:
        rttType='dev'
    if rttCacheShort==None:
        rttCacheShort='prod'

    if not rttCacheShort in __cachemap.keys():
        inverted_map=dict((val,key) for key,val in __cachemap.items())
        if rttCacheShort in inverted_map:
            rttCacheShort=inverted_map[rttCacheShort]
        else:
            usage(1)
    rttCache = __cachemap[rttCacheShort]

    if doDBDump:
        from PmbUtils.CoolDozerAccessUtils import CDDataRetrievalHelper
        CDDataRetrievalHelper.dumpDBContents()
        exit(0)

    if datatype==None:
        datatype=defaultType
    if not datatype in calcnames:
            print "Unknown argument to --type:",datatype
            usage(1)
    if rttCMTCONFIG==None:
        rttCMTCONFIG='i686-slc5-gcc43-opt'
    if len(args)!=0:
        usage(1)
    if ndays>9999:
        ndays = 9999
    if ndays<1:
        usage(1)
    if pretendDate!=None:
        if not dateFormatOk(pretendDate):
            usage(1)

    if barChartDates!=None and (barChartFileName==None or len(barChartDates)==0):
        usage(1)

    calc=calclib.getCalcByName(datatype)
    hasDomains = (len(calc.defineDomains())>0)

    if not hasDomains and domainVMEMWarnings!=None:
        print "No domains defined. Silently ignoring request for displaying/emailing warnings."
        #FIXME: We should really allow warnings to be displayed/emailed based on jobs as well!
        domainVMEMWarnings=None

    if domainVMEMWarnings=='defaultlist':
        domainVMEMWarnings=calc.getDefaultRTTEmailList(rttType,rttCache,rttCMTCONFIG)

    if tableFile==None and (not showTable) and historyFile==None and pieFile==None and \
           barChartDates==None and lastDataFile==None and domainVMEMWarnings==None and \
           optAddStdPlotsAndTablesDir==None:
        if optNoDefaultAction:
            print "Nothing to do and running with --nodefaultaction. Ending."
            exit(0)
        #Default action: Show domain (or jobs) table:
        showTable=True

    if not doJobsRatherThanDomains and not hasDomains:
        print "WARNING: No domains defined => Reverting to '--jobs' mode."
        doJobsRatherThanDomains=True
    if doJobsRatherThanDomains:
        doTotal=False
        if pieFile!=None:
            print "Can't produce pie-chart when running with --jobs"
            exit(1)

    #A few helper vars:
    varname = 'cpu/evt' if doCPURatherThanVMEM else ('malloc/evt' if doMallocRatherThanVMEM else ('memleak/evt' if doMemleakRatherThanVMEM else ('si2k' if doSi2KRatherThanVMEM else 'vmem')))
    varname2pretty = {'cpu/evt':'CPU/Evt','vmem':'VMem','malloc/evt':'Malloc/Evt','memleak/evt':"MemLeak/Evt", 'si2k':'Si2K Constant'}#Fixme: To ... somewhere else

    ###########################
    #  Retrieve data from DB  #
    ###########################

    #(fixme: could do this after the next steps, if we define the jobcalc out here):
    from PmbUtils.ValuePerDomainCalcUtils import JobList,JobListMultiOutputHelper
    joblist=getJobList(ndays,pretendDate,doBlackListing,rttType,rttCache,rttCMTCONFIG,calc)
    datatype_pretty=calc.getPrettyName()

    ######################################
    #  Add standard output if requested  #
    ######################################
    availablefields=joblist.getAllMeasurementNames()
    availabletimelabels = joblist.getTimes()
    if optAddStdPlotsAndTablesDir!=None:
        #A bit of helper infrastructure to construct file names and
        #make sure we don't overwrite anything by mistake:
        f=[]
        def absfilename(filelist,basename):
            if optStdPlotsAndTablesPostFix!='':
                basename=basename.replace('.txt','_'+optStdPlotsAndTablesPostFix+'.txt')
                basename=basename.replace('.png','_'+optStdPlotsAndTablesPostFix+'.png')
            basename=basename.replace('/','_')
            thefile=os.path.join(optAddStdPlotsAndTablesDir,basename)
            filelist+=[thefile]
            return thefile
        #Add a bunch of stuff for each field:
        for fieldname in availablefields:
            displayJobTables+=[(False,absfilename(f,'table_jobs_'+fieldname+'.txt'),fieldname,None)]
            if hasDomains:
                displayDomainTables+=[(False,absfilename(f,'table_domains_'+fieldname+'.txt'),fieldname,True,None)]
            if len(availabletimelabels)>1:
                displayJobHistPlots+=[(absfilename(f,'histplot_jobs_'+fieldname+'.png'),fieldname,None,None,None)]
                if hasDomains:
                    displayDomainHistPlots+=[(absfilename(f,'histplot_domains_'+fieldname+'.png'),fieldname,'auto',None,None,None,calc.domainOffsets())]
            #A pie-chart of last measured contribution for each domain:
            if hasDomains:
                displayDomainPieCharts+=[(absfilename(f,'pie_domains_'+fieldname+'.png'),None,fieldname,None)]

        #Check we didn't add an existing file.
        for thefile in f:
            if os.path.isfile(thefile):
                print "ERROR: File",thefile,"already exists. Aborting."
                usage(1)

    #####################################
    #  Add explicitly requested output  #
    #####################################

    if doJobsRatherThanDomains:
        prettyTitle=varname2pretty[varname]+' of '+datatype_pretty+' RTT jobs'
    else:
        prettyTitle=varname2pretty[varname]+' contribution in '+datatype_pretty+' RTT jobs'

    if tableFile!=None or showTable:
        if doJobsRatherThanDomains:
            displayJobTables+=[(showTable,tableFile,varname,prettyTitle)]
        else:
            displayDomainTables+=[(showTable,tableFile,varname,doTotal,prettyTitle)]
    if historyFile!=None:
        if doJobsRatherThanDomains:
            displayJobHistPlots+=[(historyFile,varname,clipMinVal,clipMaxVal,prettyTitle)]
        else:
            displayDomainHistPlots+=[(historyFile,varname,totalOffset,clipMinVal,clipMaxVal,prettyTitle,calc.domainOffsets())]
    if pieFile!=None:
        displayDomainPieCharts+=[(pieFile,None,varname,prettyTitle)]
    if barChartDates!=None:
        displaySingleFieldMultiLabelBarCharts+=[(barChartFileName,varname,doJobsRatherThanDomains,totalOffset,barChartDates,prettyTitle)]

    #############################
    #  Actually produce output  #
    #############################

    JobListMultiOutputHelper.produceOutput(joblist,
                                           jobTables=displayJobTables,
                                           domainTables=displayDomainTables,
                                           domainPieCharts=displayDomainPieCharts,
                                           domainHistPlots=displayDomainHistPlots,
                                           jobHistPlots=displayJobHistPlots,
                                           singleFieldMultiLabelBarCharts=displaySingleFieldMultiLabelBarCharts)

    if lastDataFile!=None:
        lasttimes=joblist.getTimes()
        if len(lasttimes)>5:
            lasttimes=lasttimes[len(lasttimes)-5:]
        pr_last = joblist.getPlottableResult_SpecificMeasurementPerJob('vmem',times=lasttimes)
        from PmbUtils.ValuePerDomainCalcUtils import JobListPrinter
        JobListPrinter().printTable(pr_last,toStdOut=False,outFile=os.path.expanduser(lastDataFile))

    ##################################################
    #  Possible display/mail warnings (for cron job) #
    ##################################################

    sendINFOMails=False
    if domainVMEMWarnings!=None:
        warnEmails=[]
        warnToStdOut=False
        warnDomain='vmem'
        warnReplyTo=None
        for e in domainVMEMWarnings.split(','):
            if e=='show': warnToStdOut=True
            elif e.startswith('REPLYTO:'):
                assert not warnReplyTo
                warnReplyTo=e.split(':')[1]
            else: warnEmails+=[e]

        infourl=None
        if warnEmails:
            infourl='http://atlas-pmb.web.cern.ch/atlas-pmb/rtt-mon/?build=%s&cmtconfig=%s&jobset=%s&cache=%s&var=%s' \
                     %(rttType,rttCMTCONFIG,datatype,rttCacheShort,warnDomain)

        from PmbUtils.CoolDozerAccessUtils import CDTimeHelper
        warnDate=CDTimeHelper.fromDateString(pretendDate) if pretendDate!=None else CDTimeHelper.now()
        import PmbUtils.WarningUtils
        warndata=PmbUtils.WarningUtils.constructWarnData(joblist,warnDomain,build=rttType,cache=rttCacheShort,cmtconfig=rttCMTCONFIG,domains=True)
        conciseChanges=warndata.getConciseChanges(thedate=warnDate,blockfile=warnBlockFile)
        if sendINFOMails or conciseChanges['haswarning']:
            level='WARNING' if conciseChanges['haswarning'] else 'INFO'
            idstr='%s/%s/%s/%s'%(datatype,rttType,rttCache,rttCMTCONFIG)
            title='%s %s changes in %s RTT jobs '%(level,warnDomain,idstr)
            lines=[]
            lines += ["Build   : %s"%idstr]
            if warnEmails:
                lines += ["Info    : %s"%infourl]
                lines += ["Log     : https://twiki.cern.ch/twiki/bin/view/Atlas/PMBMemoryPerDomainEvolution"]
            lines += ['']
            lines.extend(PmbUtils.WarningUtils.formatConciseChanges(conciseChanges,blockfile=warnBlockFile))
            lines+=['']
            import PmbUtils.CmtUtils
            lines.extend(PmbUtils.CmtUtils.formatTagDiffs(conciseChanges['date_transitions'],tags=[rttCache,rttType]))
            if warnToStdOut:
                print title+'\n'
                for l in lines:
                    print '  ',l
            if warnEmails and conciseChanges['new_reports']:
                import PmbUtils.MailUtils
                subject='[AUTO] '+title
                lines+=['']
                lines+=['[This is an automated message]']
                PmbUtils.MailUtils.sendEmail(warnEmails,subject,'Thomas.Kittelmann@cern.ch','  '+'\n  '.join(lines),replyto=warnReplyTo)

if __name__ == "__main__":
    main(sys.argv[1:])

#Ideas: Optional legend with numbers for pie-chart
#       Option to explode a given of the domains in the pie-chart.
#       Option for pretty-labels in plots?
#       Vertical grid lines in hist plots should be with one week/one month intervals.


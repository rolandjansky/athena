#!/usr/bin/env python

#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: August 2009

import os,sys,getopt

def usage(exitcode=0):
    if exitcode!=0:
        print ""
        print "Run with -h or --help for usage instructions."
        print ""
        sys.exit(exitcode)
    print "Purpose:"
    print "  This script loads a number of pickle files with VMEM related PerfMon info"
    print "  (likely created with pmb-domain-parselogfiles.py), analyses and presents"
    print "  the data in various ways."
    print ""
    print "Usage:"
    print ""
    print "  -h,--help : Display this help"
    print ""
    print "  --data=X:Y : [One for each input file] Add the file X with the"
    print "               label Y. The order of the --logfile arguments on the"
    print "               command line also defines the order of the data points"
    print "               in resulting plots and tables."
    print ""
    print " For now there are only a few simple options for producing output:"
    print ""
    print "  --jobsummary : Display a quick summary of the loaded jobs on stdout."
    print ""
    print "  --domainsummary : Display a quick summary of the gathered domain contributions on stdout."
    print ""
    print "  --makestdoutput=dirname : Produce a variety of plots and tables in target directory."
    print ""
    print "  --stage=X : (possible values ini,evt,fin,all,specialcpu,specialvmem. Default is evt)"
    print ""
    print ""
    print "Please send comments/questions/bug-reports to thomas.kittelmann@cern.ch and rousseau@lal.in2p3.fr"
    sys.exit(exitcode)

def main(argv):
    #######################
    #  Parse command line #
    #######################

    if "-h" in argv or "--help" in argv:
        usage()
    try:
        opts, args = getopt.getopt(argv, "", ["data=","jobsummary","domainsummary","makestdoutput=","stage="])
    except getopt.GetoptError:
        usage(1)

    #For all the totaloffset it goes that totaloffset==None => no total
    optAddJobSummaries=False
    optAddDomainSummaries=False
    optAddStdPlotsAndTablesDir=None
    displayJobTables=[]#list of (showonstdout,filename,fieldname,title)
    displayDomainTables=[]#list of (showonstdout,filename,fieldname,dototal,title)
    displayDomainPieCharts=[]#List of (filename,label,fieldname,title)
    displayDomainHistPlots=[]#list of (filename,fieldname,totaloffset,clipmin,clipmax,title,domainoffsets) #clipmin/clipmax==None => ignored
    displayJobHistPlots=[]#list of (filename,fieldname,clipmin,clipmax,title) #clipmin/clipmax==None => ignored
    displaySingleFieldMultiLabelBarCharts=[]#list of (filename,fieldname,dojobs,totaloffset,timelabel,title)
    readStage='evt'

    filesAndLabels=[]
    for opt, arg in opts:
        if opt in ("--data"):
            if arg and not ':' in arg:
                arg='%s:%s'%(arg,arg)
            l=arg.split(':')
            if len(l)!=2 or len(l[0])==0 or len(l[1])==0:
                print "ERROR: Wrong argument to --data"
                usage(1)
            l[1]=l[1].replace('%20',' ')
            filesAndLabels+=[(os.path.expanduser(l[0]),l[1])]
        elif opt in ("--jobsummary"):
            if optAddJobSummaries:
                usage(1)
            optAddJobSummaries=True
        elif opt in ("--domainsummary"):
            if optAddDomainSummaries:
                usage(1)
            optAddDomainSummaries=True
        elif opt in ("--makestdoutput"):
            if optAddStdPlotsAndTablesDir!=None:
                usage(1)
            optAddStdPlotsAndTablesDir=os.path.expanduser(arg)
            if not os.path.isdir(optAddStdPlotsAndTablesDir):
                print "Unknown directory",optAddStdPlotsAndTablesDir
                usage(1)
        elif opt in ("--stage"):
            if not arg in ['ini','evt','fin','all','specialcpu','specialvmem']:
                usage(1)
            readStage=arg

    if len(filesAndLabels)==0:
        print "ERROR: Specify at least one input file with --data!"
        usage(1)

    for f,l in filesAndLabels:
        if not os.path.isfile(f):
            print "ERROR: File not found:",f
            usage(1)

    ##########################
    #  Read the data files:  #
    ##########################

    import PmbUtils.LogFileParsingUtils
    jobs=[]
    sortval=0
    datatype=None
    for f,l in filesAndLabels:
        i=PmbUtils.LogFileParsingUtils.createJobsFromFileWithId2PerfMonSummary(f,l,sortval,stage=readStage)
        if i==None:
            print "ERROR: Problems loading data from ",f
            sys.exit(1)
        if datatype==None:
            datatype=i[0]
        elif datatype!=i[0]:
                print "ERROR: Incompatible data types in files!"
                sys.exit(1)
        jobs+=i[1]
        sortval+=1

    from PmbUtils.DomainCalcLib import DomainCalcLib
    calc=DomainCalcLib().getCalcByName(datatype)
    if calc==None:
        print "ERROR: Unsupported data type in files:",datatype
        sys.exit(1)

    from PmbUtils.ValuePerDomainCalcUtils import JobList,JobListMultiOutputHelper
    joblist = JobList(calc)
    for job in jobs:
        joblist.addJob(job)

    #############################################
    #  Add options from summary/stdplots flags  #
    #############################################

    if optAddJobSummaries==False and optAddDomainSummaries==False and optAddStdPlotsAndTablesDir==None:
        #To do something if ran without specific requests
        if calc.defineDomains()!=[]:
            optAddDomainSummaries=True
        else:
            optAddJobSummaries=True


    availablefields=joblist.getAllMeasurementNames()
    if len(availablefields)==0:
        print "ERROR: No fields found!"
        exit(1)

    availabletimelabels = joblist.getTimes()
    if len(availabletimelabels)==0:
        print "ERROR: No entries found!"
        exit(1)

    def title_job(varname):
        return varname+' of '+datatype+' jobs'
    def title_dom(varname):
        return varname+' contribution in '+datatype+' jobs'

    if optAddJobSummaries:
        for fieldname in availablefields:
            displayJobTables+=[(True,None,fieldname,title_job(fieldname))]

    if optAddDomainSummaries:
        for fieldname in availablefields:
            displayDomainTables+=[(True,None,fieldname,True,title_dom(fieldname))]

    html=None
    htmlFile=None
    if optAddStdPlotsAndTablesDir!=None:
        #A bit of helper infrastructure to construct file names and
        #make sure we don't overwrite anything by mistake:
        f=[]
        def absfilename(filelist,basename):
            basename=basename.replace('/','_')
            thefile=os.path.join(optAddStdPlotsAndTablesDir,basename)
            filelist+=[thefile]
            return thefile

        htmlFile=absfilename(f,'index.html')
        #stuff for constructing a little index.html for quickly putting the output on the web:
        html=['<html>','  <head>','    <title>Plots from PMBUtils</title>','  </head>','  <body>']
        #Add a bunch of stuff for each field:
        for fieldname in availablefields:
            tjob=title_job(fieldname)
            tdom=title_dom(fieldname)

            if len(availabletimelabels)>1:
                displayJobHistPlots+=[(absfilename(f,'histplot_'+datatype+'_jobs_'+fieldname+'.png'),
                                       fieldname,None,None,tjob)]
                #FIXME: How to figure out an appropriate total offset for the next two???
                displayDomainHistPlots+=[(absfilename(f,'histplot_'+datatype+'_domains_'+fieldname+'.png'),
                                          fieldname,None,None,None,tdom,calc.domainOffsets())]
            displayJobTables+=[(False,absfilename(f,'table_'+datatype+'_jobs_'+fieldname+'.txt'),fieldname,tjob)]
            displayDomainTables+=[(False,absfilename(f,'table_'+datatype+'_domains_'+fieldname+'.txt'),fieldname,True,tdom)]
            totaloffset='auto'
            totaloffset=0
            displaySingleFieldMultiLabelBarCharts+=[(absfilename(f,'bar_'+datatype+'_domains_'+fieldname+'.png'),
                                                     fieldname,False,totaloffset,None,tdom)]
            html+=['PUTIMAGE:%s'%displaySingleFieldMultiLabelBarCharts[-1][0]]
            html+=['INCLUDEFILE:%s'%displayJobTables[-1][1]]
            html+=['INCLUDEFILE:%s'%displayDomainTables[-1][1]]

            #A pie-chart for each field and each label:
            for tl in availabletimelabels:
                displayDomainPieCharts+=[(absfilename(f,'pie_'+datatype+'_domains_'+fieldname+'_'+tl.getLabel()+'.png'),
                                          tl,fieldname,tdom+' for '+tl.getLabel())]
            #TODO: Add the two other types of bar-charts when implemented!!

        #Check we didn't add an existing file.
        for thefile in f:
            if os.path.isfile(thefile):
                print "ERROR: File",thefile,"already exists. Aborting."
                usage(1)
        html+=['  </body>']
        html+=['</html>']

    #############################
    #  Actually produce output  #
    #############################

    displayDomainPieCharts=[]#FIXME
    JobListMultiOutputHelper.produceOutput(joblist,
                                           jobTables=displayJobTables,
                                           domainTables=displayDomainTables,
                                           domainPieCharts=displayDomainPieCharts,
                                           domainHistPlots=displayDomainHistPlots,
                                           jobHistPlots=displayJobHistPlots,
                                           singleFieldMultiLabelBarCharts=displaySingleFieldMultiLabelBarCharts)

    if html and htmlFile:
        actualhtml=[]
        for l in html:
            if l.startswith('INCLUDEFILE:'):
                actualhtml+=['  <pre>']
                for l2 in open(l[len('INCLUDEFILE:'):]):
                    actualhtml+=[l2.strip('\n')]
                actualhtml+=['  </pre>']
            elif l.startswith('PUTIMAGE:'):
                i=l[len('PUTIMAGE:'):]
                if i.startswith(optAddStdPlotsAndTablesDir): i=i[len(optAddStdPlotsAndTablesDir):]
                if i.startswith('/'): i=i[1:]
                actualhtml+=['<a href="%s"><img width="500" src="%s"/><a/><br/>'%(i,i)]
                #if imagemagick was installed we could make thumbnails and whatnot...
                #th=i.replace('.png','_th.png')
                #assert th!=i
                #import shutil
                #shutil.copy(i,th)
                #...
            else:
                actualhtml+=[l]
        open(htmlFile,"w").writelines('\n'.join(actualhtml)+'\n')
        
    return

if __name__ == "__main__":
    doprofile=False
    if doprofile:
        import cProfile
        cProfile.run("main(sys.argv[1:])", sort=1)
    else:
        main(sys.argv[1:])

#Todo: Better cmdline options, not just --standardvmemplots=dirname.
#      Finish the last two types of bar-charts.
#      Better titles.

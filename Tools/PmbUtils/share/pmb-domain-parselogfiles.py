#!/usr/bin/env python

#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: August 2009

import os,sys,getopt

#####################################
#  Parse command line/print usage:  #
#####################################

def usage(exitcode=0):
    if exitcode!=0:
        print ""
        print "Run with -h or --help for usage instructions."
        print ""
        sys.exit(exitcode)
    if not 'calclib' in dir():
        from PmbUtils.DomainCalcLib import DomainCalcLib
        calclib=DomainCalcLib()
    if not 'calcnames' in dir():
        calcnames=calclib.getAllCalcNames(requireLogFiles=True)
    print "Purpose:"
    print "  This script parses a number of logfiles for VMEM related PerfMon output and stores the"
    print "  results in a pickle file, which can then be analysed with pmb-domain-analysepickles.py."
    print ""
    print "Usage:"
    print ""
    print "  -h,--help : Display this help"
    print ""
    print "  --logfile=X : [REQUIRED] The script needs to run on a log file set."
    print "                Let X be one of the logfiles in a set, and the script will"
    print "                automatically look for the rest in the same directory."
    print ""
    for calcname in calcnames:
        print "                The file names in the "+calcname+" log file set must be:"
        print ""
        for f in calclib.getCalcByName(calcname).getLogFileNamingSchemeDefinitions():
            print "                        ",f
        print ""
    print "                 The log files can be gzipped (i.e. have an additional '.gz' postfix)."
    print ""
    print "  --output=X : Name of the output pickle file to store the results in."
    print ""
    print "  --debugsingle=X : Parse single log file X and display results on stdout."
    print ""
    print "Example usage:"
    print ""
    print "  1) Parse a number of logfiles from reco-on-data, and store the result in some file:"
    print ""
    print "      [cmdname] --logfile=/path/to/logs/rawtoesdtrf_core.log --output=somefile.dat"
    print ""
    print ""
    print "Please send comments/questions/bug-reports to thomas.kittelmann@cern.ch and rousseau@lal.in2p3.fr"
    sys.exit(exitcode)


def main(argv):

    from PmbUtils.DomainCalcLib import DomainCalcLib
    calclib = DomainCalcLib()
    calcnames=calclib.getAllCalcNames(requireLogFiles=True)

    #######################
    #  Parse command line #
    #######################

    if "-h" in argv or "--help" in argv:
        usage()
    try:
        opts, args = getopt.getopt(argv, "", ["logfile=","output=","debugsingle="])
    except getopt.GetoptError:
        usage(1)

    optLogFile=None
    optLogFileDir=None
    optOutFile=None
    optDebugSingleFiles=[]
    for opt, arg in opts:
        if opt in ("--logfile"):
            if optLogFile!=None:
                usage(1)
            optLogFile=os.path.expanduser(arg)
        elif opt in ("--output"):
            if optOutFile!=None:
                usage(1)
            optOutFile=os.path.expanduser(arg)
        elif opt in ("--debugsingle"):
            f=os.path.expanduser(arg)
            if not os.path.isfile(f):
                print "File not found:",f
                usage(1)
            optDebugSingleFiles+=[os.path.expanduser(arg)]
        else:
            usage(1)

    if (optLogFile==None or optOutFile==None) and len(optDebugSingleFiles)==0:
        print "ERROR: Missing arguments!"
        usage(1)

    if optLogFile!=None:
        if os.path.isfile(optOutFile):
            print "ERROR: File already exists:", optOutFile
            usage(1)

        absfilepath=os.path.abspath(optLogFile)
        if not os.path.isfile(absfilepath):
            print "ERROR: File not found:", absfilepath
            usage(1)
        optLogFile=os.path.basename(absfilepath)
        optLogFileDir=os.path.dirname(absfilepath)

        #Figure out which type of logfile set we are dealing with:
        found=False
        logfilesetType=None
        baselogfile2jobname=None

        calc=None
        for calcname in calcnames:
            if calc!=None:
                break
            tmpcalc=calclib.getCalcByName(calcname)
            for f in tmpcalc.getLogFileNamingSchemeDefinitions():
                if optLogFile in (f,f+'.gz'):
                    calc=tmpcalc
                    break
        if calc==None:
            print "ERROR: Logfile",optLogFile,"does not have recognised name!"
            usage(1)
        logfilesetType=calc.getName()
        baselogfile2jobname=calc.getLogFileNamingSchemeDefinitions()
        print " --> Logfile set type detected:",logfilesetType

        #Add all logfiles from set (nb: they might not all be present, which
        #we simply treat as a failed/missing job):
        logfile2jobname={}
        for baselogfile,jobname in baselogfile2jobname.items():
            f=os.path.join(optLogFileDir,baselogfile)
            fgz=f+'.gz'
            if os.path.isfile(f):
                print " --> Adding file",f
                logfile2jobname[f]=jobname
            elif os.path.isfile(fgz):
                print " --> Adding file",fgz
                logfile2jobname[fgz]=jobname
            else:
                print " --> Missing file",f+'[.gz]'

    #####################
    #  Actual parsing:  #
    #####################

    import PmbUtils.LogFileParsingUtils
    if optLogFile!=None:
        #from PmbUtils.LogFileParsingUtils import parseAthenaLogFilesForVMEM,outputId2VmemToFile
        parsedinfo=PmbUtils.LogFileParsingUtils.parseAthenaLogFilesForPerfMonSummaries(logfile2jobname,addTransformTimerInfo=True,extractMiscInfo=True)

    if len(optDebugSingleFiles)!=0:
        #from PmbUtils.LogFileParsingUtils import parseAthenaLogFileForVMEM
        res={}
        for f in optDebugSingleFiles:
            print "==> Parsing single file",f
            res[f]=PmbUtils.LogFileParsingUtils.parseall(f)
        for f in optDebugSingleFiles:
            print "==> Information found in",f,':'

            #unit=res[f][0]
            stages=res[f]['perfmon_summaries'][0].keys()
            stages.sort()
            if 'ini' in stages:
                stages.remove('ini')
                stages = ['ini']+stages
            for stage in stages:
                #for k,v in res[f].items():
                #    print 'res[%s]:'%k,v
                i=0
                for perfmonsum in res[f]['perfmon_summaries']:
                    i+=1
                    for var,data in perfmonsum[stage].items():
                        print '     %s (athena #%i) [%s] = %10.1f +/- %6.2f %s'%(var.rjust(10),i,stage,data['val'],data['err'],data['unit'])
            for ttn,tt in res[f]['transform_timings'].items():
                print ttn,tt
            for k,v in res[f]['misc'].items():
                print '   ',k.rjust(10),'=>',v


    #############
    #  Output:  #
    #############

    if optLogFile!=None:
        PmbUtils.LogFileParsingUtils.outputId2PerfMonSummaryToFile(parsedinfo,logfilesetType,optOutFile)

if __name__ == "__main__":
    doprofile=False
    if doprofile:
        import cProfile
        cProfile.run("main(sys.argv[1:])", sort=1)
    else:
        main(sys.argv[1:])

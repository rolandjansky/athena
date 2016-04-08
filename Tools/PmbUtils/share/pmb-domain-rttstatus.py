#!/usr/bin/env python

#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: September 2009
#  Major update: March 2010, August 2010

import os.path
import datetime

__status2colour = {'running':'blue','queued':'blue','success':'green','error':'red','post-processing problem':'orange','other':'cyan','missing':'gray'}

def parseArgs():

    from optparse import OptionParser
    parser = OptionParser()

    parser.add_option("--cache", dest="cachefile",
                      help="Speedup repeated calls by caching results in FILE", metavar="FILE")
    parser.add_option("-b", "--builds",
                      action="store_true", dest="builds", default=False,
                      help="List all available builds")
    parser.add_option("-p", "--packages",
                      action="store_true", dest="packages", default=False,
                      help="List all packages with RTT tests considered for domain breakdowns")
    parser.add_option("--buildlist",dest="buildfile", metavar="FILE",
                      help="Outputs list of builds in FILE [used by cron job].")
    parser.add_option("--menulist",dest="menufile", metavar="FILE",
                      help="Outputs lists used for the webpage menus in FILE [used by cron job].")
    parser.add_option("-j", "--jobtablesdir",dest="jobtablesdir", metavar="DIR",
                      help="Outputs html tables for all builds and tests in DIR.")

    (opt, args) = parser.parse_args()

    #Make sure we have no positional arguments and at least one action:
    if len(args)!=0:
        import string
        parser.error('Unrecognized arguments: '+string.join(args))

    def fixPath(f): return os.path.expanduser(f) if f else None
    opt.buildfile = fixPath(opt.buildfile)
    opt.jobtablesdir = fixPath(opt.jobtablesdir)
    opt.menufile = fixPath(opt.menufile)

    if not filter(None,[opt.packages,opt.builds,opt.buildfile,opt.jobtablesdir,opt.menufile]):
        #No actions
        parser.print_help()
        exit(1)

    if opt.jobtablesdir:
        if not os.path.isdir(opt.jobtablesdir) or not os.path.exists(opt.jobtablesdir):
            parser.error('--jobtablesdir: Unknown directory '+str(opt.jobtablesdir))

    return opt

def output(filename,lines):
    if not filename or filename=='show' or filename=='stdout':
        for l in lines:
            print l
    else:
        file = open(filename, 'w')
        for l in lines:
            file.write(l+'\n')
        file.close()

def produceTable(tablefile,domaincalc,info,build,cache,cmtconfig,cache_alt,pkg2testxmlinfo,timestamp):
    import PmbUtils.RTTScanUtils
    tablestyle=' style="font-size:80%;border:thin solid gray;background-color:rgb(240,240,240);text-align:left;white-space:nowrap;"'
    headerstyle=' style="text-align:center;background-color:rgb(200,200,255);"'
    subheaderstyle=' style="background-color:rgb(215,215,255);"'
    cdjobdefs=domaincalc.getCDJobDefinitions("fakedbkey")
    def addHeader(l,text):
        l+=['  <tr><th%s colspan="4">%s</th></tr>'%(headerstyle,text)]

    def link(text,ref=None,style=None,popup=None,col=None):
        if popup==None: popup=''
        if style==None: style=''
        if col==None: col=''
        if col:
            style+='color:%s;'%col
        if style:
            style=' style="%s"'%style
        if popup:
            popup=' title="%s"'%popup
        spanargs=style+popup
        if spanargs:
            text='<span%s>%s</span>'%(spanargs,text)
        return '<a href="%s">%s</a>'%(ref,text) if ref else text

    def getCDJobDefByJobName(jobname):
        for cdjobdef in cdjobdefs:
            if cdjobdef.getJobName()==jobname:
                return cdjobdef


    def createRTTLinks(date2jobinfo):
        global __status2colour
        l=[]
        for date in PmbUtils.RTTScanUtils.lastweek():
            n=(date.weekday()+1)%7
            if not date in date2jobinfo.keys():
                s='----'
                status='missing2'
                col=__status2colour['missing']
                url=None
            else:
                jobinfo=date2jobinfo[date]
                status=jobinfo['status']
                if status in __status2colour.keys(): col = __status2colour[status]
                else: col = __status2colour['other']
                url=PmbUtils.RTTScanUtils.getURLRunDir(jobinfo)
                assert jobinfo['nightly']==n
            l+=[link('rel%i'%n,ref=url,col=col,popup=status)]
        return '  '.join(l)
    l=[]
    l+=['<table%s>'%tablestyle]
    addHeader(l,'Job definitions')
    l+=[('  <tr%s><th>Job</th><th>Package</th><th>userJobId</th><th>RTT'+ \
         ' status <span style="font-size:70%%;">(checked LASTUPDATE[%s])</span></th></tr>')%(subheaderstyle,timestamp)]

    buildkey=os.path.join(build,cache,cmtconfig)
    nmissjobinfo=0
    for job in domaincalc.defineJobs():
        cdjobdef=getCDJobDefByJobName(job)
        if not cdjobdef:
            print "ERROR: Could not get job definition for",job
            continue
        package=cdjobdef.getDBJobPackage()
        (jobkey2testxmlinfo,userjobid2jobkey)=pkg2testxmlinfo[package]
        if not userjobid2jobkey:
            nmissjobinfo+=1
            continue
        userjobid=cdjobdef.getUserJobId()
        if not userjobid in userjobid2jobkey.keys():
            #only warn about out of date in dev branch:
            if 'dev' in build:
                print "ERROR: Could not get jobkey definition for userjobid %s and job %s in %s/%s"%(userjobid,job,buildkey,package)
            continue
        jobkey=userjobid2jobkey[userjobid]
        if not jobkey in jobkey2testxmlinfo.keys():
            print "ERROR: Could not get testxmlinfo definition for jobkey %s, userjobid %s and job %s"%(jobkey,userjobid,job)
            continue
        testxmlinfo=jobkey2testxmlinfo[jobkey]

        jobkey=testxmlinfo['userjobid']
        ln=None
        if userjobid in userjobid2jobkey.keys():
            ln=testxmlinfo['userjobid_lineno']
        ln='#L'+str(ln) if ln else ''

        jobnamelink=job
        packagelink=link(package,ref=cdjobdef.getSVNURLToPackage())
        joblink=link(userjobid,ref=cdjobdef.getSVNURLToXml()+ln)
        if jobkey in info[buildkey][package].keys():
            rttlinks=createRTTLinks(info[buildkey][package][jobkey])
            l+=['  <tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>'%(jobnamelink,packagelink,joblink,rttlinks)]

    if nmissjobinfo>0:
        print "WARNING: Did not find jobinfo %i %s jobs in %s"%(nmissjobinfo,domaincalc.getName(),os.path.join(build,cache,cmtconfig))

    if domaincalc.defineDomains():
        addHeader(l,'Domain definitions')
        #Domain calc:
        domaincalcurl=domaincalc.getSVNUrl(directToCalcDomainLine=True)
        domaincalcname=domaincalc.getDomainCalcFileName()
        l+=['  <tr><th colspan="4">Domains are defined in: %s</th></tr>'%link(domaincalcname,domaincalcurl)]
    l+=['</table>']
    output(tablefile,l)

def main():
    opt=parseArgs()

    #A few base things which we will almost always need:
    from PmbUtils.DomainCalcLib import DomainCalcLib
    import PmbUtils.RTTScanUtils

    calclib=DomainCalcLib()
    packages=calclib.getAllPackageNames()#All packages involved in http://atlas-pmb.web.cern.ch/atlas-pmb/rtt-mon/
    if opt.packages:
        print ' '.join(packages)

    #todo: stop here if only task is to list packages

    packages=['Rec*'] + packages#... and all packages of interest for reco shifters
    raw_info = PmbUtils.RTTScanUtils.getRunDirInfo(packages=packages,
                                                   cachefile=opt.cachefile,
                                                   timelimit_days=8,
                                                   silent=True)
    info=PmbUtils.RTTScanUtils.indexRunDirInfoByBuildPackageJob(raw_info)

    #Deal with remaining options:

    #we normally need a list of all builds (with 'prod' rather than 'offline'):
    builds=[]
    for b in info.keys():
        (build,cache,cmtconfig)=b.split('/')
        if cache=='offline': cache_alt='prod'
        else: cache_alt=cache
        builds+=[(build,cache,cmtconfig,cache_alt)]
    if opt.builds:
        for build,cache,cmtconfig,cache_alt in builds:
            print os.path.join(build,cache_alt,cmtconfig)

    if opt.buildfile or opt.menufile or opt.jobtablesdir:
        #first get some info about which tests goes with which builds:
        all_builds=[]
        all_calcnames=[]
        all_calcsandbuilds=[]
        pkg2calcnames = calclib.getPackagesToCalcNames()
        for (build,cache,cmtconfig,cache_alt) in builds:
            for package in info[os.path.join(build,cache,cmtconfig)].keys():
                if package in pkg2calcnames.keys():
                    for calcname in pkg2calcnames[package]:
                        if not (build,cache,cmtconfig,cache_alt) in all_builds:
                            all_builds+=[(build,cache,cmtconfig,cache_alt)]
                        if not calcname in all_calcnames:
                            all_calcnames += [calcname]
                        all_calcsandbuilds+=[(calcname,build,cache,cmtconfig,cache_alt)]
        all_builds.sort()
        all_calcnames.sort()
        all_calcsandbuilds.sort()

        if opt.buildfile:
            lines=[]
            for calcname,build,cache,cmtconfig,cache_alt in all_calcsandbuilds:
                lines+=["TESTANDBUILD:%s:%s:%s:%s"%(calcname,build,cache_alt,cmtconfig)]
            output(opt.buildfile,lines)

        if opt.menufile:
            lines=[]
            for build,cache,cmtconfig,cache_alt in all_builds:
                lines+=['B;%s;%s;%s'%(build,cache_alt,cmtconfig)]
            for calcname in all_calcnames:
                lines+=['T;%s'%(calcname)]
            for v in ['vmem','cpu','malloc']:#FIXME: HARDCODING
                lines+=['V;'+v]
            output(opt.menufile,lines)

        if opt.jobtablesdir:
            testxmlinfo = PmbUtils.RTTScanUtils.getTestXMLInfo(info)
            timestamp=datetime.datetime.utcnow().isoformat(' ')+' +0000'#FIXME: Should use same "now" for all tables!
            for calcname,build,cache,cmtconfig,cache_alt in all_calcsandbuilds:
                tablefile=os.path.join(opt.jobtablesdir,'table_%s_%s_%s_%s.html.snippet'%
                                       (calcname,build,cache_alt,cmtconfig))
                calc=calclib.getCalcByName(calcname)
                produceTable(tablefile,calc,info,build,cache,cmtconfig,cache_alt,testxmlinfo[os.path.join(build,cache,cmtconfig)],timestamp)

if __name__ == "__main__":
    import sys
    sys.exit(main())

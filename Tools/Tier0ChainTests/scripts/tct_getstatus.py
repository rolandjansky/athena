#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__author__ = 'Renaud Bruneliere <Renaud.Bruneliere@cern.ch>'
__doc__    = 'Print tct status. Usage tct_getstatus.py -h'

import os,sys,datetime

tctdir = '/afs/cern.ch/atlas/project/RTT/prod/Results/tct'
jobstatus = ['\033[31mfailed    \033[0m','\033[32msuccessful\033[0m']
castorstatus = ['\033[31mfiles not copied properly\033[0m','\033[32mall castor files ok\033[0m']
jobstatusNoColor = ['failed    ','successful']
castorstatusNoColor = ['files not copied properly','all castor files ok']

def parseCmdLine(args,itoday):
    # Parse options
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("-r","--release", dest="release",
                      help="Which release ? (15.5.X.Y,...)", default='15.5.X.Y')
    parser.add_option("-d","--day", dest="day",
                      help="Which release number rel_(0-6) ?",
                      default=('rel_%d' % itoday))
    parser.add_option("--dq", dest="dq",
                      help="Dump data quality results",
                      action='store_true',default=False)
    parser.add_option("--dump", dest="dump",
                      help="Dump results into a text file",
                      action='store_true',default=False)
    (config, args) = parser.parse_args(sys.argv[1:])
    for arg in args:
        print "gettctstatus.parseCmdLine(): option ",arg,"is not supported"
        pass
    return config

def getJobResults(workdir,config):
    # Loop over jobs
    for job in os.listdir(workdir):
        if not os.path.isdir(workdir+'/'+job): continue
        if not os.path.isfile('%s/%s/rttjobinfo.xml' % (workdir,job)): continue # check only rtt job folders
        if not len(os.listdir(workdir+'/'+job)):
            str = '%40s  \033[31mFolder not found\033[0m' % (job)
            strNoCol = '%40s  Folder not found' % (job)
            print str
            if config.dump: config.file.write(strNoCol+'\n')
            continue
        jobnum = (os.listdir(workdir+'/'+job))[0]
        if os.path.isfile('%s/%s/%s_log' % (workdir,job,job)):
            host = ''
            if os.path.isfile('%s/%s/%s_script_log' % (workdir,job,job)):
                host = os.popen('grep "Job was executed on host" %s/%s/%s_script_log' % (workdir,job,job)).readlines()
            if len(host): host = host[0].split()[5].strip(',')
            ldate = []
            if os.path.isfile('%s/%s/%s_script_log' % (workdir,job,job)):
                ldate = os.popen('grep "Started at" %s/%s/%s_script_log' % (workdir,job,job)).readlines()
            startdate='Unknown'
            if len(ldate):
                ldate = ldate[0].split()
                startdate = "%3s%2s %8s" % (ldate[3],ldate[4],ldate[5])
                pass
            teststr = 'ErrorCode=0 (OK)'
            ftest = os.popen('grep "%s" %s/%s/%s_log' % (teststr,workdir,job,job))
            itest = (len(ftest.readlines()) > 0)
            recostr = 'Py:RAWtoESD'
            freco = os.popen('grep "%s" %s/%s/%s_log' % (recostr,workdir,job,job))
            ireco = (len(freco.readlines()) > 0)
            castorstr = 'All files copied from castor are ok !'
            fcastor = os.popen('grep "%s" %s/%s/%s_log' % (castorstr,workdir,job,job))
            icastor = (len(fcastor.readlines()) > 0)
            str = '%51s  %14s  %11s  %9s' % (job,startdate,host,jobstatus[itest])
            strNoCol = '%51s  %14s  %11s  %9s' % (job,startdate,host,jobstatusNoColor[itest])
            if ireco and job != 'Reconstruct_ttbar_RDO_Collisions':
                str = str + '  %20s' % castorstatus[icastor]
                strNoCol = strNoCol + '  %20s' % castorstatusNoColor[icastor]
                pass
        else:
            strNoCol = str = '%40s  Not started yet' % (job)
            pass
        print str
        if config.dump: config.file.write(strNoCol+'\n')
        pass
    return

def getDQResults(workdir,config):
    # Loop over jobs
    for job in os.listdir(workdir):
        if not job.startswith('DQFlagsSummary_'): continue
        jobnum = (os.listdir(workdir+'/'+job))[0]
        if not os.path.isfile('%s/%s/%s_log' % (workdir,job,job)): continue
        dqtitlestr = "DQ results for %s:" % job.split('_')[1]
        dqstruline = '='*len(dqtitlestr)
        print "\n",dqtitlestr,'\n',dqstruline,'\n'
        if config.dump: config.file.write('\n'+dqtitlestr+'\n'+dqstruline+'\n\n')
        fdq = open('%s/%s/%s_log' % (workdir,job,job))
        if 'ComparisonWithYesterday' in job:
            printYesterdayComparison(fdq,config)
        elif 'ComparisonWithRelease' in job:
            printReleaseComparison(fdq,config)
        else:
            print "gettctstatus.getDQResults():",job,"is not defined !"
            pass
        pass
    return

def printYesterdayComparison(fdq,config):
    startanalyze = False
    for line in fdq:
        if 'top_level' in line: startanalyze = True
        if not startanalyze: continue
        if not (line.startswith('<top_level>') or line.startswith(' ')): continue
        if line.startswith('   '): continue
        lline = line.strip('\n').lstrip().split()
        if lline[0] == 'under': continue
        if 'Green' in lline[1]:
            test = '\033[32mGreen\033[0m'
        elif 'Red' in lline[1]:
            test = '\033[31mRed\033[0m'
        elif 'Undefined' in lline[1]:
            test = '\033[33mUndefined\033[0m'
            pass
        strline = '%20s  %10s' % (lline[0],test)
        strlineNoCol = '%20s  %10s' % (lline[0],lline[1])
        print strline
        if config.dump: config.file.write(strlineNoCol+'\n')
        pass
    return

def printReleaseComparison(fdq,config):
    startanalyze = False
    for line in fdq:
        if 'Difference in dq flag lists between today and yesterday' in line:
            startanalyze = True
            continue
        if not startanalyze: continue
        #print line
        llist = line.split('--- >')
        for i in range(len(llist)-1):
            l1 = llist[i].strip().lstrip().split()
            l2 = llist[i+1].strip().lstrip().split()
            iitem = 0
            for i,item in enumerate(l1):
                if item != '<': continue
                itemname = 'ERROR'
                if i+1 < len(l1): itemname = l1[i+1]
                itemflag1 = 'ERROR'
                if i+2 < len(l1): itemflag1 = l1[i+2]
                itemflag2 = 'ERROR'
                if 4*iitem+1 < len(l2): itemflag2 = l2[4*iitem+1]
                iitem = iitem + 1
                if 'Green' in itemflag1:
                    test1 = '\033[32mGreen\033[0m'
                elif 'Red' in itemflag1:
                    test1 = '\033[31mRed\033[0m'
                elif 'Undefined' in itemflag1:
                    test1 = '\033[33mUndefined\033[0m'
                elif 'Yellow' in itemflag1:
                    test1 = '\033[33mYellow\033[0m'
                elif 'Disabled' in itemflag1:
                    test1 = '\033[34mDisabled\033[0m'
                    pass
                if 'Green' in itemflag2:
                    test2 = '\033[32mGreen\033[0m'
                elif 'Red' in itemflag2:
                    test2 = '\033[31mRed\033[0m'
                elif 'Undefined' in itemflag2:
                    test2 = '\033[33mUndefined\033[0m'
                elif 'Yellow' in itemflag2:
                    test2 = '\033[33mYellow\033[0m'
                elif 'Disabled' in itemflag2:
                    test2 = '\033[34mDisabled\033[0m'
                    pass
                if test1 == test2: continue
                strline = '%40s  %20s -> %20s' % (itemname,test1,test2)
                strlineNoCol = '%40s  %10s -> %10s' % (itemname,itemflag1.strip().lstrip(),itemflag2.strip().lstrip())
                print strline
                if config.dump: config.file.write(strlineNoCol+'\n')
                pass
            pass
        break
    if not startanalyze:
        print "  No difference found"
        if config.dump: config.file.write('  No difference found\n')
        pass
    return

def main():
    itoday = datetime.date.today().isoweekday()
    if itoday == 7: itoday = 0
    config = parseCmdLine(sys.argv[1:],itoday)
    if config.dump: config.file = open('tctstatus.txt','w')
    title = "Summary for %s %s:" % (config.release,config.day)
    titleuline = "="*len(title)
    print "\n",title,"\n",titleuline,"\n"
    if config.dump: config.file.write("\n"+title+"\n"+titleuline+"\n\n")
    workdir = tctdir+'/'+config.day+'/'+config.release+'/build'
    workdir = workdir+'/'+(os.listdir(workdir))[0]
    workdir = workdir+'/'+(os.listdir(workdir))[0]
    workdir = workdir+'/Tier0ChainTests'
    getJobResults(workdir,config)
    if config.dq: getDQResults(workdir,config)
    if config.dump: config.file.close()
    return

if __name__ == '__main__':
    main()
    pass

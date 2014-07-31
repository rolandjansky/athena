#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys,datetime,time,shutil

__author__ = "Renaud Bruneliere <Renaud.Bruneliere@cern.ch>"
__doc__ = """Python script used by tct to run sequentially
the data quality web displaying jobs"""

def parseCmdLine(args):
    """ Parse input command line to optdict.
    To get the whole list of options type : tct_dqwebdisplay.py -h"""
    from optparse import OptionParser
    parser = OptionParser()
    # The following options are specific to tct_recotrf.py command
    parser.add_option("-d","--dry-run", dest="dryrun", help="Dry running",
                      action='store_true', default=False)
    parser.add_option("-m","--monitorfile", dest="monitorfile",
                      help="Input monitoring root file",
                      default="myMergedMonitoring_IDCosmic.root")
    parser.add_option("--dqconfig", dest="dqconfig",
                      help="DQ configuration",
                      default='Tier0ChainTests.LocalDQWebDisplayCosmics') 
    parser.add_option("-c","--hanconfig", dest="hanconfig",
                      help="Handy run configuration",
                      default='cosmics_prevrun.latest.hcfg')
    parser.add_option("--handir", dest="handir", help="Output han directory",
                      default='') 
    parser.add_option("--webdir", dest="webdir", help="Output web directory",
                      default='') 
    (config, args) = parser.parse_args(args)
    return config

def rundqdisplayjob(dqname,config,hanshortlist):
    print "\ntct_dqwebdisplay.py: start running on",dqname,"...",datetime.datetime.now()
    if dqname == 'Tier0ChainTests.DynamicDQWebDisplayCosmics' and config.hanconfig == '':
        print "tct_dqwebdisplay.py: WARNING: cannot run",dqname,'because hanconfig is empty'
        return
    isstaticcomp = (dqname == 'Tier0ChainTests.TCTDQWebDisplayCosmics')
    # check and build if missing output web directories
    dqimport = __import__(dqname)
    dqwebdisplay = getattr(dqimport,dqname.split('.')[1])
    dqconfig = dqwebdisplay.dqconfig
    timestamp = time.strftime('%Y%m%d%H%M%S')
    atlasarea = os.environ['AtlasArea'].split('/')
    dqdir = atlasarea[7]+'_'+atlasarea[8]+'_'+atlasarea[9]
    handir = dqconfig.hanResultsDir
    if config.handir != '': handir = config.handir
    if not os.path.isdir(handir): os.mkdir(handir)
    webdir = dqconfig.htmlDir
    if config.webdir != '': webdir = config.webdir
    if not os.path.isdir(webdir): os.mkdir(webdir)
    finalwebdir = webdir.replace('wwwtemp','www')
    if not os.path.isdir(finalwebdir): os.mkdir(finalwebdir)
    webdir=webdir+'/'+timestamp
    finalwebdir=finalwebdir+'/'+dqdir+'_'+dqconfig.config
    htmldir=(dqconfig.htmlWeb.replace('wwwtemp','www'))+'/'+dqdir+'_'+dqconfig.config
    if os.path.isdir(webdir):
        print "tct_dqwebdisplay.py: removing",webdir
        if not config.dryrun: os.system('rm -Rf %s' % webdir)
    if os.path.isdir(finalwebdir):
        print "tct_dqwebdisplay.py: removing",finalwebdir
        if not config.dryrun: os.system('rm -Rf %s' % finalwebdir)
        pass
    # preparation
    if os.path.isfile('MyCOOL_histo.db'): os.system('rm -f MyCOOL_histo.db')
    print "tct_dqwebdisplay.py: running tct_CreateDB.py"
    if not config.dryrun: os.system('tct_CreateDB.py') # create dummy cool database
    print "tct_dqwebdisplay.py: dqconfig.hcfg =",dqconfig.hcfg
    if not os.path.isfile(dqconfig.hcfg):
        hanconfigfile='/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/tct_reference_run.hcfg'
        print "tct_dqwebdisplay.py: Using han configuration fall-back file:",hanconfigfile
        os.system('ln -s %s %s' % (hanconfigfile,dqconfig.hcfg))
        pass
    if not os.path.isfile(dqconfig.hcfg_min10):
        hanconfigfile='/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/tct_reference_minutes10.hcfg'
        print "tct_dqwebdisplay.py: Using han configuration fall-back file:",hanconfigfile
        os.system('ln -s %s %s' % (hanconfigfile,dqconfig.hcfg_min10))
        pass
    if not os.path.isfile(dqconfig.hcfg_min30):
        hanconfigfile='/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/tct_reference_minutes30.hcfg'
        print "tct_dqwebdisplay.py: Using han configuration fall-back file:",hanconfigfile
        os.system('ln -s %s %s' % (hanconfigfile,dqconfig.hcfg_min30))
        pass
    ## Run command
    runcmd = "DQWebDisplay.py %s %s %s" % (config.monitorfile,dqname,timestamp)
    print "tct_dqwebdisplay.py: exact command is \"",runcmd,"\""
    print "tct_dqwebdisplay.py: starting DQWebDisplay.py...",datetime.datetime.now()
    if not config.dryrun:
        os.system(runcmd)
        for filename in os.popen('ls %s/%s*/*/*/run_*_han.root' % (handir,timestamp)):
            filename = filename.strip('\n')
            newname = (filename.split('/'))[len(filename.split('/'))-1]
            newlist = newname.split('_')
            newname = newname.replace(newlist[1],"")
            print "tct_dqwebdisplay.py: copying",filename,"to",newname
            print "tct_dqwebdisplay.py: current working dir is",os.getcwd()
            shutil.copyfile(filename,newname)
            pass
        print 'tct_dqwebdisplay.py: moving',webdir,'to',finalwebdir
        os.system('mv %sL %s' % (webdir,finalwebdir)) # final webdir reflects atlas project and release 
        pass
    if dqname == 'Tier0ChainTests.ReleaseDQWebDisplayCosmics' and os.path.isfile('run__han.root'):
        os.system('mv run__han.root run__han__release.root')
        pass
    ## make weblink file
    os.system('rm -f dqweblink_'+dqconfig.config+'.html')
    fdqweblink = open('dqweblink_'+dqconfig.config+'.html','w')
    sdqweblink = """
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Frameset//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transititional.dtd">
<html>
  <head>
    <title>Home | TCT Redirection page</title>
       <META http-equiv="refresh" content="1; URL=%s">
   </head>

<body>
You will be redirected automatically in 1 second to the <a href="%s">tct DQ webpage</a>.
</body>
</html>
EOF
"""
    fdqweblink.write(sdqweblink % (htmldir,htmldir))
    fdqweblink.close()
    print "tct_dqwebdisplay.py: ...ending",dqname,datetime.datetime.now()
    return

#
# Main job
#
print "tct_dqwebdisplay.py: starting",datetime.datetime.now()
workdir = rttdir = os.getcwd()
if "WORKDIR" in os.environ: workdir = os.environ["WORKDIR"]
print "tct_dqwebdisplay.py: working directory is",workdir
print "tct_dqwebdisplay.py: rttdir directory is",rttdir
os.chdir(workdir)
config = parseCmdLine(sys.argv[1:])
hanlist = config.hanconfig.split(',')
hanshortlist = []
# Copy han configuration files in workdir
for hanfile in hanlist:
    hanshort = (hanfile.split('/'))[len(hanfile.split('/'))-1]
    hanshortlist.append(hanshort)
    if rttdir != workdir: shutil.copyfile(hanfile,workdir+'/'+hanshort)
    pass
# Loop over different dq configurations
for dqname in config.dqconfig.split(','):
    rundqdisplayjob(dqname,config,hanshortlist)
    pass
# Loop over the different hcfg files and update Han configuration
print "tct_dqwebdisplay.py: starting Han replace running...",datetime.datetime.now()
runnumber = os.environ['RUNNUMBER']
cmd = 'tct_dqupdatereferences.sh %s %s %s %s'
for hanshort in hanshortlist:
    hanshortlatest = hanshort.replace('_prev','_')
    mycmd = cmd % (hanshortlatest,config.monitorfile,hanshort,runnumber)
    print "tct_dqwebdisplay.py: exact command is \"",mycmd,"\""
    if not config.dryrun: os.system(mycmd)
    pass
print "tct_dqwebdisplay.py: ...ending Han replace running",datetime.datetime.now()
# Copy output files back on afs rttdir
os.chdir(rttdir)
if rttdir != workdir:
    print "\ntct_dqwebdisplay.py: start copying output files to rttdir...",rttdir,datetime.datetime.now()
    print "tct_dqwebdisplay.py: rttdir =",rttdir
    for filename in os.listdir(workdir):
        print "tct_dqwebdisplay.py: copying",filename
        if not config.dryrun: shutil.copyfile(workdir+'/'+filename,rttdir+'/'+filename)
        pass
    print "tct_dqwebdisplay.py: ...output files copying to rttdir done ",datetime.datetime.now()
    pass
print "ErrorCode=0 (OK)"
print "\ntct_dqwebdisplay.py: ending",datetime.datetime.now()

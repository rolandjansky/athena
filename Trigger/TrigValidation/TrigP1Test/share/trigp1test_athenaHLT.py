#!/usr/bin/env python

import os,sys,subprocess,datetime,time,shutil

__author__ = "Renaud Bruneliere <Renaud.Bruneliere@cern.ch>, Stewart Martin-Haugh <smh@cern.ch>"
__doc__    = """Port of tct_reco_trf.py to TrigP1Test usage"""

castortct = '/eos/atlas/atlascerngroupdisk/proj-sit/tct/rec_input'
#castortct = '/castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/tct/rec_input'
tct_stage_svcclass = 'atlasgroupdisk'
fallbackfiles = {'IDCosmic':['/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/data09_cos.00135664.physics_IDCosmic.daq.RAW._lb0000._SFO-2._0001.data','/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/data09_cos.00135664.physics_IDCosmic.daq.RAW._lb0000._SFO-2._0002.data','/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/data09_cos.00135664.physics_IDCosmic.daq.RAW._lb0000._SFO-2._0003.data'],
                 'MinBias':['/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/data09_900GeV.00141749.physics_MinBias.merge.RAW._lb0022._0001.1','/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/data09_900GeV.00141749.physics_MinBias.merge.RAW._lb0023._0001.1','/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/data09_900GeV.00141749.physics_MinBias.merge.RAW._lb0024._0001.1'],
                 }
nfilestct = 10 # Maximum number of RAW data files stored on proj-sit for a given runNumber and stream
tct_stage_host     = 'castoratlast3'
tct_stage_svcclass = 'atlascerngroupdisk'
retcode = 0 # Exit code returned by script (if non-zero) and grepped by RTT

def checkTCTCastor(runnumber,trigStream,castordir,nfiles,lb):
    print ('trigp1test_athenaHLT.py : checkTCTCastor : starting...')
    test = False
    listlb = []
    if lb != "":
        if ',' in lb:
            listlb = [int(lb.split(',')[0]),int(lb.split(',')[1])]
        else:
            listlb = [int(lb)]
            pass
        pass
    if len(listlb):
        print ('trigp1test_athenaHLT.py : checkTCTCastor : check only files with lumiblocks within %s' % (listlb))
        pass
    if 'castor' in castortct and 'tzero' in castortct:
        fstrlist = os.popen('nsls -l %s/%s/physics_%s/' % (castortct,runnumber,trigStream))
    elif 'castor' in castortct:
        fstrlist = os.popen('xrd castoratlas ls %s/%s/physics_%s/' % (castortct,runnumber,trigStream))
    elif 'eos' in castortct:
        fstrlist = os.popen('xrd eosatlas ls %s/%s/physics_%s/' % (castortct,runnumber,trigStream))
    nGoodFiles = 0
    strlist = fstrlist.readlines()
    for istr,stream in enumerate(strlist):
        if strlist[istr].strip('\n') == "": continue
        strlist[istr] = strlist[istr].strip('\n').split()[len(strlist[istr].split())-1]
        if not trigStream in strlist[istr]: continue
        if len(listlb) == 1 and not ('_lb%0.4d' % listlb[0]) in strlist[istr]: continue
        if len(listlb) == 2:
            mylb = int(strlist[istr].split('_lb')[1].split('.')[0])
            if mylb < listlb[0] or mylb > listlb[1]: continue
            pass
        nGoodFiles += 1
        print ('trigp1test_athenaHLT.py : checkTCTCastor : %s is good file, nGoodFiles = %d' % (strlist[istr],nGoodFiles))
        pass
    if nGoodFiles >= nfiles: test = True 
    if nGoodFiles < nfiles and len(strlist) < nfilestct - nfiles: test = updateTCTCastor(runnumber,trigStream,castordir,nfiles,listlb)
    print ('trigp1test_athenaHLT.py : checkTCTCastor : test = %d ...end' % (test))
    return test

def updateTCTCastor(runnumber,trigStream,castordir,nfiles,listlb):
    test = True
    print ('trigp1test_athenaHLT.py : updateTCTCastor : starting...')
    init_stage_host     = os.environ['STAGE_HOST']
    init_stage_svcclass = os.environ['STAGE_SVCCLASS']
    if init_stage_svcclass == 't0atlas' and init_stage_host != 'castoratlas':
        init_stage_host = 'castoratlas'
    os.environ['STAGE_HOST']     = tct_stage_host
    os.environ['STAGE_SVCCLASS'] = tct_stage_svcclass
    #os.system('rfmkdir %s/%s' % (castortct,runnumber))
    #os.system('rfchmod 777 %s/%s' % (castortct,runnumber))
    #os.system('rfmkdir %s/%s/physics_%s' % (castortct,runnumber,trigStream))
    #os.system('rfchmod 777 %s/%s/physics_%s' % (castortct,runnumber,trigStream))
    if 'castor' in castortct:
        os.system('xrd castoratlas mkdir %s/%s' % (castortct,runnumber))
        os.system('xrd castoratlas chmod %s/%s 7 7 7' % (castortct,runnumber))
        os.system('xrd castoratlas mkdir %s/%s/physics_%s' % (castortct,runnumber,trigStream))
        os.system('xrd castoratlas chmod %s/%s/physics_%s 7 7 7' % (castortct,runnumber,trigStream))
    elif 'eos' in castortct:
        os.system('xrd eosatlas mkdir %s/%s' % (castortct,runnumber))
        os.system('xrd eosatlas chmod %s/%s 7 7 7' % (castortct,runnumber))
        os.system('xrd eosatlas mkdir %s/%s/physics_%s' % (castortct,runnumber,trigStream))
        os.system('xrd eosatlas chmod %s/%s/physics_%s 7 7 7' % (castortct,runnumber,trigStream))
    os.environ['STAGE_HOST']     = init_stage_host
    os.environ['STAGE_SVCCLASS'] = init_stage_svcclass
    if 'castor' in castordir and 'tzero' in castordir:
        ffilelist = os.popen('nsls -l %s' % (castordir))
    elif 'castor' in castordir:
        ffilelist = os.popen('xrd castoratlas ls %s' % (castordir))
    elif 'eos' in castordir:
        #ffilelist = os.popen('xrd eosatlas ls %s' % (castordir))
	ffilelist = os.popen('/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select ls %s' % (castordir))
    nGoodFiles = 0
    print "trigp1test_athenaHLT.py : updateTCTCastor : look for files in:", castordir
    for ifile,filestr in enumerate(ffilelist):
        filestr = filestr.strip('\n')
        if filestr == "": continue
        filename = os.path.basename(filestr.split()[len(filestr.split())-1])
        if not trigStream in filename: continue
        if len(listlb) == 1 and not ('_lb%0.4d' % listlb[0]) in filename: continue
        if len(listlb) == 2:
            mylb = int(filename.split('_lb')[1].split('.')[0])
            if mylb < listlb[0] or mylb > listlb[1]: continue
            pass        
        os.environ['STAGE_HOST']     = init_stage_host
        os.environ['STAGE_SVCCLASS'] = init_stage_svcclass
        os.system('echo STAGE_HOST=$STAGE_HOST STAGE_SVCCLASS=$STAGE_SVCCLASS')
        print ('trigp1test_athenaHLT.py : updateTCTCastor : copying %s/%s to %s' % (castordir,filename,os.getcwd()))
        if not config.dryrun_:
            if 'castor' in castordir and 'tzero' in castordir:
                os.system('time rfcp %s/%s .' % (castordir,filename))
            elif 'castor' in castordir:
                os.system('time xrdcp root://castoratlas/%s/%s .' % (castordir,filename))
            elif 'eos' in castordir:
                os.system('time xrdcp root://eosatlas/%s/%s .' % (castordir,filename))
        os.environ['STAGE_HOST']     = tct_stage_host
        os.environ['STAGE_SVCCLASS'] = tct_stage_svcclass
        os.system('echo STAGE_HOST=$STAGE_HOST STAGE_SVCCLASS=$STAGE_SVCCLASS')
        print ('trigp1test_athenaHLT.py : updateTCTCastor : copying %s to %s/%s/physics_%s' % (filename,castortct,runnumber,trigStream))
        if not config.dryrun_:
            if 'castor' in castortct and 'tzero' in castortct:
                os.system('time rfcp %s %s/%s/physics_%s' % (filename,castortct,runnumber,trigStream))
            elif 'castor' in castortct:
                os.system('time xrdcp %s root://castoratlas/%s/%s/physics_%s/' % (filename,castortct,runnumber,trigStream))
            elif 'eos' in castortct:
                os.system('time xrdcp %s root://eosatlas/%s/%s/physics_%s/' % (filename,castortct,runnumber,trigStream))
        if not config.dryrun_: os.system('rm -f %s' % (filename))
        nGoodFiles += 1
        if nGoodFiles == nfiles: break # take only the first 10 files
        pass
    os.environ['STAGE_HOST']     = init_stage_host
    os.environ['STAGE_SVCCLASS'] = init_stage_svcclass
    print ('trigp1test_athenaHLT.py : updateTCTCastor : ...end')
    return test

def copyTCTCastor(jobnum,runnumber,trigStream,nfiles,lb,castordir,usecastortct):
    init_stage_host     = os.environ['STAGE_HOST']
    init_stage_svcclass = os.environ['STAGE_SVCCLASS']
    if init_stage_svcclass == 't0atlas' and init_stage_host != 'castoratlas':
        init_stage_host = 'castoratlas'
    listlb = []
    if lb != "":
        if ',' in lb:
            listlb = [int(lb.split(',')[0]),int(lb.split(',')[1])]
        else:
            listlb = [int(lb)]
    if usecastortct or (castortct in castordir):
        os.environ['STAGE_HOST']     = tct_stage_host
        os.environ['STAGE_SVCCLASS'] = tct_stage_svcclass
    if usecastortct:
        mycastordir = '%s/%s/physics_%s' % (castortct,runnumber,trigStream)
    else:
        mycastordir = castordir
    print ('trigp1test_athenaHLT.py : copyTCTCastor : STAGE_HOST=%s' % (os.environ['STAGE_HOST']))
    print ('trigp1test_athenaHLT.py : copyTCTCastor : STAGE_SVCCLASS=%s' % (os.environ['STAGE_SVCCLASS']))    
    print ('trigp1test_athenaHLT.py : copyTCTCastor : look for files in %s' % (mycastordir))
    if 'castor' in mycastordir and 'tzero' in mycastordir:
        ffilelist = os.popen('nsls -l %s' % (mycastordir))
    elif 'castor' in mycastordir:
        ffilelist = os.popen('xrd castoratlas ls %s' % (mycastordir))
    elif 'eos' in mycastordir:
        ffilelist = os.popen('xrd eosatlas ls %s' % (mycastordir))
    filelist = ffilelist.readlines()
    if not len(filelist):
        print ('trigp1test_athenaHLT.py : copyTCTCastor : no files found in %s' % (mycastordir))
        print ('trigp1test_athenaHLT.py : copyTCTCastor : => run on fallbackfiles')
        if trigStream in fallbackfiles:
            myfallbackfiles = fallbackfiles[trigStream]
        else:
            myfallbackfiles = fallbackfiles['MinBias']
            pass
        for idat in range(nfiles):
            ifile = idat%(len(myfallbackfiles))
            if not config.dryrun_: os.system('ln -s %s %d.data' % (myfallbackfiles[ifile],idat))
            pass
        print "WARNING: Some files copied from castor were corrupted !"
        os.environ['STAGE_HOST']     = init_stage_host
        os.environ['STAGE_SVCCLASS'] = init_stage_svcclass
        return
    firstfile = jobnum*nfiles
    if (jobnum+1)*nfiles > len(filelist): firstfile = (jobnum%(len(filelist)/nfiles))*nfiles
    idat = 0
    nGoodFiles = 0
    for ifile,filestr in enumerate(filelist):
        filestr = filestr.strip('\n')
        if filestr == "": continue
        filename = os.path.basename(filestr.split()[len(filestr.split())-1])
        fullfilename = filename
        if not mycastordir in fullfilename: fullfilename = mycastordir + '/' + filename
        if len(listlb) == 1 and not ('_lb%0.4d' % listlb[0]) in filename:
            continue
        elif len(listlb) == 2:
            mylb = int(filename.split('_lb')[1].split('.')[0])
            if mylb < listlb[0] or mylb > listlb[1]: continue
        elif ifile < firstfile:
            continue
        if readRDO:
            print ('trigp1test_athenaHLT.py : copyTCTCastor : copying %s to RDO_%d.pool.root' % (fullfilename,idat))
        else:
            print ('trigp1test_athenaHLT.py : copyTCTCastor : copying %s to %d.data' % (fullfilename,idat))
        if not config.dryrun_: 
            if 'castor' in mycastordir and 'tzero' in mycastordir:
                os.system('time rfcp %s .' % (fullfilename))
            elif 'castor' in mycastordir:
                os.system('time xrdcp root://castoratlas/%s .' % (fullfilename))
            elif 'eos' in mycastordir:
                os.system('time xrdcp root://eosatlas/%s .' % (fullfilename))
        if not config.dryrun_: 
            if readRDO:
                os.system('ln -s %s RDO_%d.pool.root' % (filename,idat))
            else:
                os.system('ln -s %s %d.data' % (filename,idat))
        idat += 1
        nGoodFiles += 1
        if nGoodFiles == nfiles: break
        pass
    os.environ['STAGE_HOST']     = init_stage_host
    os.environ['STAGE_SVCCLASS'] = init_stage_svcclass
    print "All files copied from castor are ok !"
    return

def parseCmdLine(args):
    """ Parse input command line to optdict.
    To get the whole list of options type : trigp1test_athenaHLT.py -h"""
    from optparse import OptionParser
    parser = OptionParser()
    # The following options are specific to trigp1test_athenaHLT.py command
    parser.add_option("-d","--dry-run", dest="dryrun_", help="Dry running",
                      action='store_true', default=False)
    parser.add_option("-j","--jobnum", dest="jobnum_", help="Job number", type='int', default=0)
    parser.add_option("-r","--runnum", dest="runnum_", help="Run number", default='None') 
    parser.add_option("--lumiblock", dest="lb_", help="Lumi block(s)", default='')
    parser.add_option("--modifiers", dest="modifiers", help="Modifiers", default='')
    parser.add_option("-c","--castordir", dest="castordir_", help="Castor path to input data",
                      default="/castor/cern.ch/grid/atlas/tzero/prod1/perm/data09_900GeV/physics_MinBias/0141749/data09_900GeV.00141749.physics_MinBias.merge.RAW") 
    parser.add_option("--latestrun", dest="uselatestrun_", help="Do we use latest run available ?",
                      action='store_true', default=False)
    parser.add_option("--nfiles", dest="nfiles_", help="Number of input files",
                      type='int', default=3) 
    parser.add_option("--trigStream", dest="trigStream_", help="Trigger stream",default='') 
    # The following options are same as Reco_trf.py command
    parser.add_option("--maxEvents", dest="maxEvents", help="Maximal number of events",
                      type='int', default=500) 
    (config, args) = parser.parse_args(args)
    print 'config',config
    return config

#
# Main job
#
print "trigp1test_athenaHLT.py: starting",datetime.datetime.now()
workdir = rttdir = os.getcwd()
if "WORKDIR" in os.environ: workdir = os.environ["WORKDIR"]
print "trigp1test_athenaHLT.py: working directory is",workdir
os.chdir(workdir)
config = parseCmdLine(sys.argv[1:])
isr155xy = '15.5.X.Y' in os.environ['AtlasArea']
# To fix bug #85744: TCT: Fatal in <TROOT::InitSystem>: HOME directory not set
os.environ["ROOTENV_NO_HOME"] = '1' 

#
# Copy input files
#
print "trigp1test_athenaHLT.py: generate copy link script"
inputdir = config.castordir_
if not config.uselatestrun_: inputdir += '/'+config.runnum_
useTCTCastor = False
if not config.uselatestrun_:
    if not castortct in config.castordir_ and not 'eos' in config.castordir_:
        useTCTCastor = checkTCTCastor(config.runnum_,config.trigStream_,config.castordir_,config.nfiles_,config.lb_)
    print "trigp1test_athenaHLT.py: start copying files...",datetime.datetime.now()
    copyTCTCastor(config.jobnum_,config.runnum_,config.trigStream_,config.nfiles_,config.lb_,config.castordir_,useTCTCastor)
    print "trigp1test_athenaHLT.py: ...files copying done ",datetime.datetime.now()
else:
    copycmd = 'tct_generate_copyscript.py %d physics_%s %s %s %s %s' % (config.jobnum_,config.trigStream_,config.nfiles_,inputdir,str(config.uselatestrun_),config.lb_)
    os.system(copycmd)
    print "trigp1test_athenaHLT.py: start copying files...",datetime.datetime.now()
    if not config.dryrun_: os.system("./copylinkscript.sh")
    print "trigp1test_athenaHLT.py: ...files copying done ",datetime.datetime.now()
    #print "trigp1test_athenaHLT.py: set AtlCoolT0Setup.py"
    #os.system('/afs/cern.ch/user/a/atlcond/utils/AtlCoolT0Setup.py')
    pass

#
# List input files
#
print "trigp1test_athenaHLT.py: print list of input raw data files :"
os.system('ls -l -h *.data')
flistfiles = os.popen('ls -l *.data')
listfiles = flistfiles.readlines()
for il,line in enumerate(listfiles):
    listfiles[il] = line.strip('\n').split()[len(line.split())-1]
    pass

trigCmd = "athenaHLT.py -f \"" + str(listfiles) + "\" -c \"" + config.modifiers + "\" TriggerRelease/runHLT_standalone.py"
subprocess.call("echo " + trigCmd, shell=True)
subprocess.call(trigCmd, shell=True)

print "trigp1test_athenaHLT.py: delete input raw data files :"
for il,line in enumerate(listfiles):
    print "trigp1test_athenaHLT.py: remove",listfiles[il]
    os.system('rm -f %s' % listfiles[il])
    pass


#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys,subprocess,datetime,time,shutil

__author__ = "Renaud Bruneliere <Renaud.Bruneliere@cern.ch>"
__doc__    = """Python script used by tct to run the Reco_Trf.py transform"""

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
    print ('tct_recotrf.py : checkTCTCastor : starting...')
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
        print ('tct_recotrf.py : checkTCTCastor : check only files with lumiblocks within %s' % (listlb))
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
        print ('tct_recotrf.py : checkTCTCastor : %s is good file, nGoodFiles = %d' % (strlist[istr],nGoodFiles))
        pass
    if nGoodFiles >= nfiles: test = True 
    if nGoodFiles < nfiles and len(strlist) < nfilestct - nfiles: test = updateTCTCastor(runnumber,trigStream,castordir,nfiles,listlb)
    print ('tct_recotrf.py : checkTCTCastor : test = %d ...end' % (test))
    return test

def updateTCTCastor(runnumber,trigStream,castordir,nfiles,listlb):
    test = True
    print ('tct_recotrf.py : updateTCTCastor : starting...')
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
	ffilelist = os.popen('eos ls %s' % (castordir))
    nGoodFiles = 0
    print "tct_recotrf.py : updateTCTCastor : look for files in:", castordir
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
        print ('tct_recotrf.py : updateTCTCastor : copying %s/%s to %s' % (castordir,filename,os.getcwd()))
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
        print ('tct_recotrf.py : updateTCTCastor : copying %s to %s/%s/physics_%s' % (filename,castortct,runnumber,trigStream))
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
    print ('tct_recotrf.py : updateTCTCastor : ...end')
    return test

def copyTCTCastor(jobnum,runnumber,trigStream,nfiles,lb,castordir,usecastortct,readRDO=False):
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
    print ('tct_recotrf.py : copyTCTCastor : STAGE_HOST=%s' % (os.environ['STAGE_HOST']))
    print ('tct_recotrf.py : copyTCTCastor : STAGE_SVCCLASS=%s' % (os.environ['STAGE_SVCCLASS']))    
    print ('tct_recotrf.py : copyTCTCastor : look for files in %s' % (mycastordir))
    if 'castor' in mycastordir and 'tzero' in mycastordir:
        ffilelist = os.popen('nsls -l %s' % (mycastordir))
    elif 'castor' in mycastordir:
        ffilelist = os.popen('xrd castoratlas ls %s' % (mycastordir))
    elif 'eos' in mycastordir:
        ffilelist = os.popen('xrd eosatlas ls %s' % (mycastordir))
    filelist = ffilelist.readlines()
    if not len(filelist):
        print ('tct_recotrf.py : copyTCTCastor : no files found in %s' % (mycastordir))
        print ('tct_recotrf.py : copyTCTCastor : => run on fallbackfiles')
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
            print ('tct_recotrf.py : copyTCTCastor : copying %s to RDO_%d.pool.root' % (fullfilename,idat))
        else:
            print ('tct_recotrf.py : copyTCTCastor : copying %s to %d.data' % (fullfilename,idat))
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
    To get the whole list of options type : tct_recotrf.py -h"""
    from optparse import OptionParser
    parser = OptionParser()
    # The following options are specific to tct_recotrf.py command
    parser.add_option("-d","--dry-run", dest="dryrun_", help="Dry running",
                      action='store_true', default=False)
    parser.add_option("-j","--jobnum", dest="jobnum_", help="Job number", type='int', default=0)
    parser.add_option("-r","--runnum", dest="runnum_", help="Run number", default='None') 
    parser.add_option("--lumiblock", dest="lb_", help="Lumi block(s)", default='')
    parser.add_option("-c","--castordir", dest="castordir_", help="Castor path to input data",
                      default="/castor/cern.ch/grid/atlas/tzero/prod1/perm/data09_900GeV/physics_MinBias/0141749/data09_900GeV.00141749.physics_MinBias.merge.RAW") 
    parser.add_option("--latestrun", dest="uselatestrun_", help="Do we use latest run available ?",
                      action='store_true', default=False)
    parser.add_option("--nfiles", dest="nfiles_", help="Number of input files",
                      type='int', default=3) 
    # The following options are same as Reco_trf.py command
    parser.add_option("--maxEvents", dest="maxEvents", help="Maximal number of events",
                      type='int', default=500) 
    parser.add_option("--trigStream", dest="trigStream_", help="Trigger stream",default='') 
    parser.add_option("--AMI", dest="AMI", help="AMI tag", default='')
    parser.add_option("--AMITag", dest="AMITag", help="AMI tag", default='')
    parser.add_option("--autoConfiguration", dest="autoConfiguration", help="autoConfiguration", default='')
    parser.add_option("--geometryVersion", dest="geometryVersion", help="geometryVersion", default='')
    parser.add_option("--conditionsTag", dest="conditionsTag", help="conditionsTag", default='')
    parser.add_option("--ignoreErrors", dest="ignoreErrors", help="ignoreErrors", action='store_true', default='False')
    parser.add_option("--ignorePatterns", dest="ignorePatterns", help="ignorePatterns", default='')
    #parser.add_option("--ignoreerrors", dest="ignoreerrors", help="ignoreErrors", action='store_true', default='NONE')
    #parser.add_option("--beamType", dest="beamType", help="Beam type", default='')
    parser.add_option("--preExec", dest="preExec", help="Pre-execute options (overwrite)", default='')
    parser.add_option("--preExec_r2e", dest="preExec_r2e", help="Pre-execute in RAWtoESD options (overwrite)", default='')
    parser.add_option("--append_preExec", dest="append_preExec", help="Pre-execute options (append)", default='')
    parser.add_option("--preInclude", dest="preInclude", help="Pre-include options", default='')
    parser.add_option("--postInclude", dest="postInclude", help="Post-include options", default='')
    parser.add_option("--postExec", dest="postExec", help="postExec options", default='')
    parser.add_option("--postExec_e2a", dest="postExec_e2a", help="postExec_e2a options", default='')
    parser.add_option("--triggerConfig", dest="triggerConfig", help="Trigger configuration", default='')
    parser.add_option("--RunNumber", dest="RunNumber", help="Run number", default='')
    parser.add_option("--steering", dest="steering", help="transform steering", default='')
    parser.add_option("--athenaopts", dest="athenaopts", help="athena options", default='')
    parser.add_option("--HI", dest="doHI_", help="Run with Heavy ions settings",
                      action='store_true', default=False)
    parser.add_option("--HIP", dest="doHIP_", help="Run with proton-lead settings",
                      action='store_true', default=False)
    parser.add_option("--readRDO", dest="readRDO_", help="Read input RDO files",
                      action='store_true', default=False)
    parser.add_option("--dropHIST", dest="dropHIST_", help="Drop DQ histogram from outputs",
                      action='store_true', default=False)
    parser.add_option("--dropDESDs", dest="dropDESDs_", help="Drop DESD files from outputs",
                      action='store_true', default=False)
    parser.add_option("--dropDRAWs", dest="dropDRAWs_", help="Drop DRAW files from outputs",
                      action='store_true', default=False)
    parser.add_option("--dropNTUPs", dest="dropNTUPs_", help="Drop NTUP files from outputs",
                      action='store_true', default=False)
    parser.add_option("--dropDAODs", dest="dropDAODs_", help="Drop DAOD files from outputs",
                      action='store_true', default=False)
    #parser.add_option("--uploadtoami", dest="uploadtoami", help="Upload performance data to AMI", type='float', default=0.0)
    (config, args) = parser.parse_args(args)
    print 'config',config
    return config

def generateRecoTrfCmd(config):
    """ Generate from command line options the list of arguments
    used by Reco_trf.py """
    recocmd = "Reco_tf.py "
    nfiles = config.nfiles_
    nfilescheck = 0
    for ifile in range(nfiles):
        if not config.readRDO_ and not os.path.isfile('%d.data' % ifile): break
        if config.readRDO_ and not os.path.isfile('RDO_%d.pool.root' % ifile): break
        nfilescheck += 1
    if nfilescheck < nfiles:
        print 'tct_recotrf.py : generateRecoTrfCmd : found only',nfilescheck,'data files => set it as number of files!'
        nfiles = nfilescheck
    if config.readRDO_:
        config.inputRDOFile = ("RDO_%d.pool.root,"*nfiles) % tuple(range(nfiles))
        config.inputRDOFile = config.inputRDOFile.strip(',')
    else:
        config.inputBSFile = ("%d.data,"*nfiles) % tuple(range(nfiles))
        config.inputBSFile = config.inputBSFile.strip(',')
    config.outputESDFile = "myESD_%s_%d.pool.root" % (config.trigStream_,config.jobnum_)
    if not config.dropHIST_:
        config.outputHISTFile = "myMergedMonitoring_%s_%d.root" % (config.trigStream_,config.jobnum_)
    ### Updated 27.11.2016
    desdlist = ['DESDM_IDALIGN','DESDM_SGLEL','DESDM_SLTTMU',
                'DESDM_EGAMMA','DESDM_MCP','DESDM_CALJET',
                'DESDM_PHOJET','DESDM_TILEMU','DESDM_EXOTHIP']
    drawlist = ['DRAW_EGZ','DRAW_TAUMUH','DRAW_ZMUMU','DRAW_RPVLL', 'DRAW_TOPSLMU']
    ### DAOD still missing some outputs due to ATLASRECTS-3427
    #daodlist = ['DAOD_IDTIDE','DAOD_IDTRKVALID','DAOD_SCTVALID','DAOD_IDNCB']
    daodlist = ['DAOD_IDTIDE','DAOD_IDTRKVALID']
    ntuplist = []  # Obsolete
    if config.doHI_:
        desdlist = []
        #desdlist = ['DESD_HIRARE']
        drawlist = []
        #ntuplist = ['NTUP_HI','NTUP_TRKVALID']
        ntuplist = []
        config.outputAODFile = "myAOD_%s_%d.pool.root" % (config.trigStream_,config.jobnum_)
        config.outputTAGFile = "myTAG_%s_%d.root" % (config.trigStream_,config.jobnum_)
    elif config.doHIP_:
        desdlist = []
        drawlist = []
        #ntuplist = ['NTUP_HI','NTUP_MINBIAS','NTUP_TRKVALID']
        ntuplist = []
        config.outputAODFile = "myAOD_%s_%d.pool.root" % (config.trigStream_,config.jobnum_)
        config.outputTAGFile = "myTAG_%s_%d.root" % (config.trigStream_,config.jobnum_)
    else:
        config.outputAODFile = "myAOD_%s_%d.AOD.pool.root" % (config.trigStream_,config.jobnum_)
        config.outputTAGFile = "myTAG_%s_%d.root" % (config.trigStream_,config.jobnum_)
    if not config.dropDAODs_:
        for dtype in daodlist:
            setattr(config,'output%sFile' % dtype,
                    "%s_%s_%d.AOD.pool.root" % (dtype.split('_')[1],config.trigStream_,config.jobnum_))
    #if config.beamType == 'cosmics':
    #    desdlist = ['DESD_PIXELCOMM','DESD_IDCOMM','DESD_CALOCOMM','DESD_MUONCOMM','DESD_TILECOMM']
    #    pass
    if not config.dropDESDs_:
        for dtype in desdlist:
            setattr(config,'output%sFile' % dtype,
                    "%s_%s_%d.pool.root" % (dtype.split('_')[1],config.trigStream_,config.jobnum_))
    if not config.dropDRAWs_:
        for dtype in drawlist:
            setattr(config,'output%sFile' % dtype,
                    "%s_%s_%d.data" % (dtype.split('_')[1],config.trigStream_,config.jobnum_))
    if not config.dropNTUPs_:
        #config.outputNTUP_TRIGFile = "TRIG_%s_%d.root" % (config.trigStream_,config.jobnum_)
        if not config.doHI_ and not config.doHIP_ :
            config.outputNTUP_SCTFile = "SCT_%s_%d.root" % (config.trigStream_,config.jobnum_)
            config.outputNTUP_BKGDFile = "BKGD_%s_%d.root" % (config.trigStream_,config.jobnum_)
        for ntype in ntuplist:
            setattr(config,'output%sFile' % ntype,
                    "%s_%s_%d.NTUP.root" % (ntype.split('_')[1],config.trigStream_,config.jobnum_))
    if config.ignoreErrors=="ALL":
        config.ignoreErrors = "True"
    for arg in vars(config):
        if arg.endswith('_') or getattr(config,arg) == '': continue
        recocmd += " --"+arg+"="+str(getattr(config,arg))
    return recocmd

#
# Main job
#
print "tct_recotrf.py: starting",datetime.datetime.now()
workdir = rttdir = os.getcwd()
if "WORKDIR" in os.environ: workdir = os.environ["WORKDIR"]
print "tct_recotrf.py: working directory is",workdir
os.chdir(workdir)
config = parseCmdLine(sys.argv[1:])
isr155xy = '15.5.X.Y' in os.environ['AtlasArea']
# To fix bug #85744: TCT: Fatal in <TROOT::InitSystem>: HOME directory not set
os.environ["ROOTENV_NO_HOME"] = '1' 

#
# Copy input files
#
print "tct_recotrf.py: generate copy link script"
inputdir = config.castordir_
if not config.uselatestrun_: inputdir += '/'+config.runnum_
useTCTCastor = False
if not config.uselatestrun_:
    if not castortct in config.castordir_ and not 'eos' in config.castordir_:
        useTCTCastor = checkTCTCastor(config.runnum_,config.trigStream_,config.castordir_,config.nfiles_,config.lb_)
    print "tct_recotrf.py: start copying files...",datetime.datetime.now()
    copyTCTCastor(config.jobnum_,config.runnum_,config.trigStream_,config.nfiles_,config.lb_,config.castordir_,useTCTCastor,config.readRDO_)
    print "tct_recotrf.py: ...files copying done ",datetime.datetime.now()
else:
    copycmd = 'tct_generate_copyscript.py %d physics_%s %s %s %s %s' % (config.jobnum_,config.trigStream_,config.nfiles_,inputdir,str(config.uselatestrun_),config.lb_)
    os.system(copycmd)
    print "tct_recotrf.py: start copying files...",datetime.datetime.now()
    if not config.dryrun_: os.system("./copylinkscript.sh")
    print "tct_recotrf.py: ...files copying done ",datetime.datetime.now()
    pass
if not config.readRDO_:
    print "tct_recotrf.py: set AtlCoolT0Setup.py"
    os.system('/afs/cern.ch/user/a/atlcond/utils/AtlCoolT0Setup.py')
    pass

#
# List input files
#
print "tct_recotrf.py: print list of input raw data files :"
os.system('ls -l -h *.data')
flistfiles = os.popen('ls -l *.data')
listfiles = flistfiles.readlines()
for il,line in enumerate(listfiles):
    listfiles[il] = line.strip('\n').split()[len(line.split())-1]
    pass

#
# Run job transform
#

# Environment needed for uploadtoami  
os.environ['TZAMIPW'] = '/afs/cern.ch/atlas/project/RTT/prod/RTTUserFiles/vukotic/conn.txt'

recocmd = generateRecoTrfCmd(config)
print "\ntct_recotrf.py: exact Reco_Trf.py command is \"",recocmd,"\""
print "\ntct_recotrf.py: starting Reco_Trf.py...",datetime.datetime.now()
startTime = time.time()
if not config.dryrun_:
    os.environ['DEFAULT_AF_CACHE_FNAME'] = ''
    retcode = subprocess.call(recocmd, shell=True)
    if retcode: raise SystemExit(retcode)
    print "tct_recotrf.py: return code is set to",retcode
    pass
endTime = time.time()
print "tct_recotrf.py: ...ending Reco_Trf.py. ",datetime.datetime.now()
print "tct_recotrf.py: DeltaTime = ",str(endTime-startTime),"\n"

#
# Delete input files
#
print "tct_recotrf.py: delete input raw data files :"
for il,line in enumerate(listfiles):
    print "tct_recotrf.py: remove",listfiles[il]
    os.system('rm -f %s' % listfiles[il])
    pass

#
# Copy files back to rttdir if different from workdir
#
os.chdir(rttdir)
if rttdir != workdir:
    print "\ntct_recotrf.py: start copying output files to rttdir...",rttdir,datetime.datetime.now()
    print "tct_recotrf.py: rttdir =",rttdir
    for filename in os.listdir(workdir):
        test = filename.endswith('.data') or filename in listfiles
        if test: continue
        if not os.path.isfile(workdir+'/'+filename):
            print "tct_recotrf.py:",(workdir+'/'+filename),"is not a file, skip it"
            continue
        print "tct_recotrf.py: copying",filename
        if not config.dryrun_: shutil.copyfile(workdir+'/'+filename,rttdir+'/'+filename)
        pass
    print "tct_recotrf.py: ...output files copying to rttdir done ",datetime.datetime.now()
    pass
print "\ntct_recotrf.py: ending",datetime.datetime.now()

#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Pythonized version of AlpGen steering executables 
#    written by Zach Marshall <zach.marshall@cern.ch>
#  Attempts to remove path-dependence of AlpGen so that
#    it can be used safely in the production system.

import os,sys,subprocess,shutil,time,glob
from AthenaCommon import Logging

def SimpleRun( proc_name='wjet' , events=10000 , nwarm = [4,1000000] , energy=3500. , jets=[2,0] , seed = 1234 , special1 = None , special2 = None , special3 = None, inputgridfile = None, configOnly=False):
    log = Logging.logging.getLogger('AlpGenUtils')
    #input parameters: 
    #   number of events per iteration (number of iterations?)
    #   number of events generated after warmup (configure warmup based on above?)
    #   beam energy
    #   random number seed
    #   number of jets
    error = -1

    if configOnly:
      log.info(' Config only requested. ')

    alpgenpath=os.environ['ALPGENPATH']
    if alpgenpath=='':
        log.error('AlpGen Setup Failed! Please set ALPGENPATH correctly!')
        return error

    app_path = alpgenpath+'/'+proc_name+'work/'+proc_name+'gen'
    if not os.access(app_path,os.R_OK):
        log.error('AlpGen module for process'+str(proc_name)+'not found at expected location:')
        log.error('   '+str(app_path))
        return error
    
    SkipWarmUp = False

    if inputgridfile is not None:
        value,key = inputgridfile.split('.tar.gz',1)
        gridname = value
        if len(value) <1 : 
            log.fatal('Grid file is not correct, please provide a grid file.')
            raise RunTimeError('Corrupted grid file.')

        gridfile = glob.glob(gridname+".grid")

        if gridfile[0]:
           SkipWarmUp = True
           os.symlink(gridfile[0],'alpout.grid1')
        
        
    tmp_card = """alpout          ! label for files
0               ! start with: 0=new grid, 1=previous warmup grid, 2=previous generation grid
"""
    
    if SkipWarmUp:
        log.info('Grid File provided: '+gridfile[0]+' ... Skipping Warm-Up!')
        nwarm[1]=0
        nwarm[0]=0
        tmp_card = """alpout          ! label for files
1               ! start with: 0=new grid, 1=previous warmup grid, 2=previous generation grid
"""
    else:
        tmp_card = """alpout          ! label for files
0               ! start with: 0=new grid, 1=previous warmup grid, 2=previous generation grid
"""

    tmp_card += '%i %i       ! Nevents/iteration,  N(warm-up iterations) \n'%(nwarm[1],nwarm[0])
    tmp_card += '%i          ! Nevents generated after warm-up \n'%(events)
    tmp_card += 'njets %i \n'%(int(jets[0]))
    tmp_card += 'ebeam %f         ! E beam\n'%(float(energy))
    if not 'ndns' in special1: tmp_card += 'ndns 9               ! PDF CTEQ6L1 \n'
    else:
        log.info('Spotted ndns setting in your special input - will not use the default CTEQ6L1 PDF.')

    tmp_card += """ih2 1                ! LHC
iqopt 1              ! Qscale, 1 is generator default for all the processes 
qfac  1              ! Qscale factor 
ickkw 1              ! enable jet-parton matching, determine scale of alpha_s 
iewopt 3             ! EW parameter scheme (3=MW 80.419, MZ 91.188, GF 1.16639e-5)
mt 172.5             ! top mass
ktfac 1              ! ckkw alphs scale 
ptjmin 15.           ! jet min pT
ptbmin 20.           ! b-quark min pT
ptcmin 20.           ! c-quark min pT
ptlmin 0.            ! lepton min pt                  
ptphmin 20.          ! photon min pt
metmin 0.0           ! missing et cut 
etajmax 6.0          ! parton max eta
etabmax 6.0          ! max eta for b-quarks
etacmax 6.0          ! max eta for c-quarks
etalmax 10.0         ! lepton max eta   
etaphmax 6.0         ! max eta for photons
drjmin 0.7           ! min dR between jets
drlmin 0.0           ! min delta r between leptons  
cluopt 1             ! kt scale option. 1:kt propto pt, 2:kt propto mt
"""

    if seed>100000:
        log.warning('Using seed '+str(seed)+'>100000 - not recommended by the Alpgen authors.')

    tmp_card += 'iseed1 %i       ! first  random seed for weighting process \n'%(int(seed))
    tmp_card += 'iseed2 %i       ! second random seed for weighting process \n'%((int(seed)+123))
    tmp_card += 'iseed3 %i       ! first  random seed for unweighting process \n'%((int(seed)+456))
    tmp_card += 'iseed4 %i       ! second random seed for unweighting process \n'%((int(seed)+789))

    # Get the right PDF loaded up
    if not os.access('cteq6l1.tbl',os.R_OK):
        log.info('Grabbing PDF set CTEQ6L1')
        os.symlink(alpgenpath+'/alplib/pdfdat/ctq61/cteq6l1.tbl','cteq6l1.tbl')

    pid = os.getpid()
    
    alp_card = open( 'input_card.'+str(pid) , 'w' )
    alp_card.write('1               ! imode\n')
    alp_card.write( tmp_card )
    if special1 is not None: alp_card.write( special1 )
    alp_card.close()

    if not configOnly:
        log.info('Running mode 1 generation')
        gen_step_1 = subprocess.Popen(app_path+' < input_card.'+str(pid),shell=True)
        gen_step_1.wait()
    else:
        log.info('Would now run mode 1 generation')
        shutil.copy('input_card.'+str(pid),'input_card.mode_1.dat')
        log.info(app_path+' < input_card.mode_1.dat')

    alp_card = open( 'input_card.'+str(pid) , 'w' )
    alp_card.write('2               ! imode\n')
    alp_card.write( tmp_card )
    if special2 is not None: alp_card.write( special2 )
    alp_card.close()

    if not configOnly:
        log.info('Running mode 2 generation')
        gen_step_2 = subprocess.Popen(app_path+' < input_card.'+str(pid),shell=True)
        gen_step_2.wait()
    else:
        log.info('Would now run mode 2 generation')
        shutil.copy('input_card.'+str(pid),'input_card.mode_2.dat')
        log.info(app_path+' < input_card.mode_2.dat')
        # don't need to do anything else, so return
        return

    log.info('Moving output into place')
    shutil.move('alpout.unw','alpgen.XXXXXX.TXT.v1._00001.events')

    oldparamfile = open('alpout_unw.par','r')
    newparamfile = open('alpgen.XXXXXX.TXT.v1._00001.dat','w')
    for line in oldparamfile.readlines():
        if '***** end parameters' in line:
            if special3 is None or '501' not in special3:
                newparamfile.write("501    20.        ! min ETCLUS used for parton-jet matching (Normally ETCLUS = ptjmin + 5)\n")
            if special3 is None or '502 ' not in special3:
                newparamfile.write("502    0.7        ! min RCLUS value for parton-jet matching\n")
            if special3 is None or '503' not in special3:
                newparamfile.write("503    6.         ! max ETACLUS value for parton-jet matching\n")
            if special3 is None or '504' not in special3:
                newparamfile.write("504    %i          ! 0 inclusive 1 exclusive\n"%(jets[1]))
            if special3 is not None:
                newparamfile.write(special3)        
        newparamfile.write(line)

    oldparamfile.close()
    newparamfile.close()

    shutil.copy('alpgen.XXXXXX.TXT.v1._00001.dat','input_card.final_pass.dat')
    log.info('Param card that would be passed to athena in input_card.final_pass.dat')

    log.info('Zipping up output for use in athena')
    zip_up = subprocess.Popen(['tar','cvzf','alpgen.XXXXXX.TXT.v1.tgz','alpgen.XXXXXX.TXT.v1._00001.dat','alpgen.XXXXXX.TXT.v1._00001.events'])
    zip_up.wait()

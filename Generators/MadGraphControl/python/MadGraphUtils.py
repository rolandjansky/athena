#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Pythonized version of MadGraph steering executables
#    written by Zach Marshall <zach.marshall@cern.ch>
#    updates for aMC@NLO by Josh McFayden <mcfayden@cern.ch>
#  Attempts to remove path-dependence of MadGraph

import os,sys,time,subprocess,shutil,glob,re,difflib
from AthenaCommon import Logging
mglog = Logging.logging.getLogger('MadGraphUtils')


def setup_path_protection():
    if 'GFORTRAN_TMPDIR' in os.environ:
        return
    if 'TMPDIR' in os.environ:
        os.environ['GFORTRAN_TMPDIR']=os.environ['TMPDIR']
        return
    if 'TMP' in os.environ:
        os.environ['GFORTRAN_TMPDIR']=os.environ['TMP']
        return

def new_process(card_loc='proc_card_mg5.dat',grid_pack=None):
    """ Generate a new process in madgraph.  Note that
    you can pass *either* a process card location or a
    string that contains your process card, and either should
    work for the same command.
    """
    try:
        from __main__ import opts
        if opts.config_only:
            mglog.info('Athena running on config only mode: not executing MadGraph')
            return
    except:
        pass

    # Don't run if generating events from gridpack
    if is_gen_from_gridpack(grid_pack):
        return grid_pack


    if '\n' in card_loc:
        # Actually just sent the process card contents - let's make a card
        mglog.info('Writing process card')
        a_card = open( 'proc_card_mg5.dat' , 'w' )
        a_card.write(card_loc)
        a_card.close()
        return new_process()

    madpath=os.environ['MADPATH']
    # Just in case
    setup_path_protection()

    mgexec='/bin/mg5_aMC'

    # Check to allow older MadGraph versions
    if not os.access(madpath+mgexec,os.R_OK):
        foundOld=False
        if os.access(madpath+'/bin/mg5',os.R_OK):
            mglog.warning('No mg5_aMC executable found in '+madpath+'/bin, but (depreciated) mg5 executable was found, will try with this...')
            foundOld=True
            mgexec='/bin/mg5'

        if not foundOld:
            raise RuntimeError('No mg5_aMC or mg5 executables found in '+madpath+'/bin')

    # Check if we have to use MG4 proc_card.dat or MG5 proc_card.dat
    thedir = ''
    if not os.access(card_loc,os.R_OK):
        raise RuntimeError('No process card found at '+card_loc)
    else:
        mglog.info('Assuming that '+card_loc+' is a well-formatted process card')
        proc_peek = open(card_loc,'r')
        for l in proc_peek.readlines():
            # Look for an output line 
            if 'output' not in l.split('#')[0].split(): continue
            # Check how many things before the options start
            tmplist = l.split('#')[0].split(' -')[0]
            # if two things, second is the directory
            if len(tmplist.split())==2: thedir = tmplist.split()[1]
            # if three things, third is the directory (second is the format)
            elif len(tmplist.split())==3: thedir = tmplist.split()[2]
            # See if we got a directory
            if ''!=thedir:
                mglog.info('Saw that you asked for a special output directory: '+str(thedir))
            break
        proc_peek.close()

    mglog.info('Started process generation at '+str(time.asctime()))

    generate = subprocess.Popen([madpath+mgexec,card_loc],stdin=subprocess.PIPE)
    generate.wait()

    mglog.info('Finished process generation at '+str(time.asctime()))

    if ''==thedir:
        directories = os.listdir( os.getcwd() )
        for adir in sorted(directories):
            if 'PROC' in adir: thedir=adir

    if not os.access('%s/SubProcesses/subproc.mg'%thedir,os.R_OK):
        raise RuntimeError('No diagrams for this process in dir='+str(thedir))
    return thedir


def get_default_runcard(proc_dir='PROC_mssm_0'):
    try:
        from __main__ import opts
        if opts.config_only:
            mglog.info('Athena running on config only mode: grabbing run card the old way, as there will be no proc dir')
            mglog.info('Fetching default LO run_card.dat')
            if os.access(os.environ['MADPATH']+'/Template/LO/Cards/run_card.dat',os.R_OK):
                shutil.copy(os.environ['MADPATH']+'/Template/LO/Cards/run_card.dat','run_card.SM.dat')
                return 'run_card.SM.dat'
            elif os.access(os.environ['MADPATH']+'/Template/Cards/run_card.dat',os.R_OK):
                shutil.copy(os.environ['MADPATH']+'/Template/Cards/run_card.dat','run_card.SM.dat')
                return 'run_card.SM.dat'
            else:
                raise RuntimeError('Cannot find default LO run_card.dat!')
    except:
        pass

    # Get the run card from the installation
    run_card_loc=proc_dir+'/Cards/run_card.dat'
    mglog.info('Fetching default run_card.dat from '+str(run_card_loc))
    if os.access(run_card_loc,os.R_OK):
        shutil.copy(run_card_loc,'run_card.SM.dat')
        return 'run_card.SM.dat'
    else:
        run_card_loc=proc_dir+'/Cards/run_card_default.dat'
        mglog.info('Fetching default run_card.dat from '+str(run_card_loc))
        if os.access(run_card_loc,os.R_OK):
            shutil.copy(run_card_loc,'run_card.SM.dat')
            return 'run_card.SM.dat'
        else:
            raise RuntimeError('Cannot find default run_card.dat or run_card_default.dat! I was looking here: %s'%run_card_loc)
    
    
def generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir='PROC_mssm_0',grid_pack=False,gridpack_compile=False,cluster_type=None,cluster_queue=None,cluster_temp_path=None,extlhapath=None,madspin_card_loc=None,required_accuracy=0.01,gridpack_dir=None,nevents=None,random_seed=None):
    try:
        from __main__ import opts
        if opts.config_only:
            mglog.info('Athena running on config only mode: not executing MadGraph')
            return
    except:
        pass

    # Just in case
    setup_path_protection()

    if is_gen_from_gridpack(grid_pack):
        if gridpack_dir and nevents and random_seed:
            mglog.info('Running event generation from gridpack (using smarter mode from generate() function)')
            generate_from_gridpack(run_name=run_name,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=random_seed,card_check=proc_dir,param_card=param_card_loc,madspin_card=madspin_card_loc,extlhapath=extlhapath) 
            return
        else:
            mglog.info('Detected gridpack mode for generating events but asssuming old configuration (using sepatate generate_from_gridpack() call)')
            return


    version = getMadGraphVersion() # avoiding code duplication

    # If we need to get the cards...
    if run_card_loc is not None and not os.access(run_card_loc,os.R_OK):
        if run_card_loc.count('/')>1:
            mglog.warning('Several / found in run card name '+str(run_card_loc)+' but card not found.')
            mglog.warning('Will try to retrieve the card via get files, without the path portion of the name.')
            run_card_loc = run_card_loc.split('/')[-1]
        mglog.info('Getting run card '+str(run_card_loc)+' via get_files')
        get_run_card = subprocess.Popen(['get_files','-data',run_card_loc])
        get_run_card.wait()
        if not os.access(run_card_loc,os.R_OK):
            mglog.error('Could not find run card '+str(run_card_loc))
            return 1
    if param_card_loc is not None and not os.access(param_card_loc,os.R_OK):
        if param_card_loc.count('/')>1:
            mglog.warning('Several / found in param card name '+str(param_card_loc)+' but card not found.')
            mglog.warning('Will try to retrieve the card via get files, without the path portion of the name.')
            param_card_loc = param_card_loc.split('/')[-1]
        mglog.info('Getting param card '+str(param_card_loc)+' via get_files')
        get_param_card = subprocess.Popen(['get_files','-data',param_card_loc])
        get_param_card.wait()
        if not os.access(param_card_loc,os.R_OK):
            mglog.error('Could not find param card '+str(param_card_loc))
            return 1

    # Check if process is NLO or LO
    isNLO=is_NLO_run(proc_dir=proc_dir)
           
    if grid_pack:
        #Running in gridpack mode
        mglog.info('Started generating gridpack at '+str(time.asctime()))
        mglog.warning(' >>>>>> THIS KIND OF JOB SHOULD ONLY BE RUN LOCALLY - NOT IN GRID JOBS <<<<<<')

    
        if not isNLO:
            oldcard = open(run_card_loc,'r')
            newcard = open(run_card_loc+'.tmp','w')
    
            for line in oldcard:
                if ' = gridpack ' in line:
                    newcard.write('   .%s.     = gridpack  !True = setting up the grid pack\n'%('true'))
                    mglog.info('   .%s.     = gridpack  !True = setting up the grid pack'%('true'))
                else:
                    newcard.write(line)
            oldcard.close()
            newcard.close()
            shutil.move(run_card_loc+'.tmp',run_card_loc)
        else:

            oldcard = open(run_card_loc,'r')
            newcard = open(run_card_loc+'.tmp','w')
    
            req_acc=required_accuracy
            for line in oldcard:
                if ' = nevents ' in line:
                    newcard.write(' 0 = nevents    ! Number of unweighted events requested \n')
                    mglog.info('Setting nevents = 0.')
                elif ' = req_acc ' in line:
                    newcard.write(' %f = req_acc    ! Required accuracy (-1=auto determined from nevents)\n'%(req_acc))
                    mglog.info('Setting req_acc = %f'%(req_acc))
                else:
                    newcard.write(line)
            oldcard.close()
            newcard.close()
            shutil.move(run_card_loc+'.tmp',run_card_loc)

        print "run_card.dat:",run_card_loc
        runCard = subprocess.Popen(['cat',run_card_loc])
        runCard.wait()


    else:
        #Running in on-the-fly mode
        mglog.info('Started generating at '+str(time.asctime()))


    mglog.info('Run '+str(run_name)+' will be performed in mode '+str(mode)+' with '+str(njobs)+' jobs in parallel.')


    if param_card_loc is None: mglog.info('Will use the default parameter card')


    if madspin_card_loc:
        if int(version.split('.')[0]) < 2:
            mglog.error('Attempting to run MadSpin with old MadGraph version - please upgrade to MadGraph5_aMC@NLO (v2+).')
            return 1
            
        mglog.info('Decaying with MadSpin. Moving card (%s) into place.'%(madspin_card_loc))
        shutil.copyfile(madspin_card_loc,proc_dir+'/Cards/madspin_card.dat')


    # Ensure that things are set up normally
    if not os.access(run_card_loc,os.R_OK):
        mglog.error('No run card found at '+run_card_loc)
        return 1
    if param_card_loc is not None and not os.access(param_card_loc,os.R_OK):
        mglog.error('No param card found at '+param_card_loc)
        return 1
    if not os.access(proc_dir,os.R_OK):
        mglog.error('No process directory found at '+proc_dir)
        return 1
    if not os.access(proc_dir+'/bin/generate_events',os.R_OK):
        mglog.error('No generate_events module found in '+proc_dir)
        return 1
    try:
        os.remove(proc_dir+'/Cards/run_card.dat')
        if param_card_loc is not None: os.remove(proc_dir+'/Cards/param_card.dat')
    except:
        pass
    try:
        shutil.copyfile(run_card_loc,proc_dir+'/Cards/run_card.dat')
        if param_card_loc is not None: shutil.copyfile(param_card_loc,proc_dir+'/Cards/param_card.dat')
    except Exception as e:
        mglog.error('Trouble moving the cards into place!!')
        mglog.error('Issued exception '+str(e.args)+' '+str(e))
        mglog.error('Working from '+str(os.getcwd()))
        return 1

    (LHAPATH,origLHAPATH,origLHAPDF_DATA_PATH) = setupLHAPDF(isNLO, version=version, proc_dir=proc_dir, extlhapath=extlhapath) 

            
    mglog.info('For your information, the libraries available are (should include LHAPDF):')
    mglog.info( sorted( os.listdir( proc_dir+'/lib/' ) ) )

    setupFastjet(isNLO, proc_dir=proc_dir)

    mglog.info('Now I will hack the make files a bit.  Apologies, but there seems to be no good way around this.')
    shutil.copyfile(proc_dir+'/Source/make_opts',proc_dir+'/Source/make_opts_old')
    old_opts = open(proc_dir+'/Source/make_opts_old','r')
    new_opts = open(proc_dir+'/Source/make_opts','w')
    for aline in old_opts:
        if 'FC=g' in aline:
            mglog.info('Configuring the fancy gfortran compiler instead of g77 / f77')
            new_opts.write('  FC=gfortran\n')
        elif 'FFLAGS+= -ffixed-line-length-132' in aline and 'i686' in os.environ['CMTCONFIG']:
            mglog.info('Setting you up for a 32-bit compilation')
            new_opts.write('FFLAGS+= -ffixed-line-length-132 -m32\n')
        else:
            new_opts.write(aline)
    old_opts.close()
    new_opts.close()
    mglog.info('Make file hacking complete.')

    currdir=os.getcwd()
    os.chdir(proc_dir)

    athenaMP = False
    if 'ATHENA_PROC_NUMBER' in os.environ:
        njobs = int(os.environ['ATHENA_PROC_NUMBER'])
        mglog.info('Lucky you - you are running on a full node queue.  Will re-configure for '+str(njobs)+' jobs.')
        mode = 2
        athenaMP = True
        
    if njobs>1 and mode==0: mglog.warning('Inconsistent configuration between mode '+str(mode)+' and number of jobs '+str(njobs))
    if njobs>1 and not isNLO:
    
        if not grid_pack and not athenaMP:
            mglog.warning('Running parallel generation.  This should not happen for a grid job, to be a good citizen.')
        elif not athenaMP:
            mglog.info('Running parallel gridpack generation.  We already told you to not do this in a grid job...')


        if cluster_type is not None and cluster_queue is not None:
            mglog.info('Setting up parallel running system settings')
            
            
            config_card='Cards/me5_configuration.txt'
            oldcard = open(config_card,'r')
            newcard = open(config_card+'.tmp','w')                                                                                             
 
            for line in oldcard:                                                             
                if 'cluster_type = ' in line:                                                
                    newcard.write('cluster_type = %s \n'%(cluster_type))                        
                    mglog.info('Setting cluster type = %s in %s'%(cluster_type,config_card))                   
                elif 'cluster_queue = ' in line: 
                    if not cluster_queue:
                        cluster_queue='None'
                    newcard.write('cluster_queue = %s \n'%(cluster_queue))                      
                    mglog.info('Setting cluster queue = %s in %s'%(cluster_queue,config_card))                 
                else:                                                                        
                    newcard.write(line)                                                      
            oldcard.close()                                                                  
            newcard.close()                                                                  
            shutil.move(config_card+'.tmp',config_card)     
                                                                                             
            mglog.info('New me5_configuration.txt card:')                                    
            configCard = subprocess.Popen(['cat',config_card])             
            configCard.wait()                                                                      

            if cluster_type=='pbs': 
                mglog.info('Modifying bin/internal/cluster.py for PBS cluster running')
                os.system("sed -i \"s:text += prog:text += './'+prog:g\" bin/internal/cluster.py")                 
    
        generate = subprocess.Popen(['bin/generate_events',str(mode),str(njobs),str(run_name)],stdin=subprocess.PIPE)
        generate.wait()

    elif not isNLO:   
        
        mglog.info('Running serial generation.  This will take a bit more time than parallel generation.')
        generate = subprocess.Popen(['bin/generate_events','0',str(run_name)],stdin=subprocess.PIPE)
        generate.wait()

    elif isNLO:
        
        ### Editing config card
        config_card_loc='Cards/amcatnlo_configuration.txt'
        oldcard = open(config_card_loc,'r')
        newcard = open(config_card_loc+'.tmp','w')
        
        # Make sure params only set once
        run_mode_set=False
        auto_html_set=False
        cltype_set=False
        clqueue_set=False
        nbcore_set=False
        tmppath_set=False

        for line in oldcard:
            if 'run_mode =' in line:
                if not run_mode_set:
                    mglog.info('Setting run_mode = %i'%(mode))
                    newcard.write('run_mode = %i \n'%(mode))
                    run_mode_set=True
            elif 'automatic_html_opening =' in line:
                if not auto_html_set:
                    mglog.info('Setting automatic_html_opening = %s'%('False'))
                    newcard.write('automatic_html_opening = %s \n'%('False'))
                    auto_html_set=True
            elif 'cluster_type = ' in line and mode == 1:
                if not cltype_set:
                    mglog.info('Setting cluster type = %s in %s'%(cluster_type,config_card_loc))   
                    newcard.write('cluster_type = %s \n'%(cluster_type))                       
                    cltype_set=True
            elif 'cluster_queue = ' in line and mode == 1:                                               
                if not clqueue_set:
                    mglog.info('Setting cluster queue = %s in %s'%(cluster_queue,config_card_loc)) 
                    newcard.write('cluster_queue = %s \n'%(cluster_queue))                     
                    clqueue_set=True
            elif 'cluster_temp_path = ' in line and mode == 1 and cluster_temp_path:                                               
                if not tmppath_set:
                    mglog.info('Setting cluster temp path = %s in %s'%(cluster_temp_path,config_card_loc)) 
                    newcard.write('cluster_temp_path = %s \n'%(cluster_temp_path))                     
                    tmppath_set=True
            elif 'nb_core = ' in line:                                               
                if not nbcore_set:
                    mglog.info('Setting number of cores = %i in %s'%(njobs,config_card_loc)) 
                    newcard.write('nb_core = %i \n'%(njobs))                     
                    nbcore_set=True
            else:
                newcard.write(line)
        oldcard.close()
        newcard.close()
        shutil.move(config_card_loc+'.tmp',config_card_loc)
        
        print "amcatnlo_configuration.txt:",config_card_loc
        configCard = subprocess.Popen(['cat',config_card_loc])
        configCard.wait()
    
    
        #generate = subprocess.Popen(['bin/aMCatNLO','launch','-p','-f'],stdin=subprocess.PIPE)
        #generate.wait()
        
        mglog.info('Removing Cards/shower_card.dat to ensure we get parton level events only')
        remove_shower = subprocess.Popen(['rm','Cards/shower_card.dat'])
        remove_shower.wait()

            
        
        mygenerate = subprocess.Popen(['bin/generate_events','--name='+str(run_name)],stdin=subprocess.PIPE, stderr=subprocess.STDOUT)
        mygenerate.wait()



    if grid_pack:


        if not isNLO:
            ### LO RUN ###
            gridpack_name=(run_name+'_gridpack.tar.gz')
            mglog.info('Tidying up gridpack (%s)...'%gridpack_name)
            
            os.chdir(currdir)
            if madspin_card_loc:
                shutil.copy((proc_dir+'/'+run_name+'_decayed_1_gridpack.tar.gz'),gridpack_name)
            else:
                shutil.copy((proc_dir+'/'+run_name+'_gridpack.tar.gz'),gridpack_name)
            
            if gridpack_compile:
                mkdir = subprocess.Popen(['mkdir','tmp%i/'%os.getpid()])
                mkdir.wait()
                os.chdir('tmp%i/'%os.getpid())
                mglog.info('untar gridpack')
                untar = subprocess.Popen(['tar','xvzf',('../'+gridpack_name)])
                untar.wait()
                mglog.info('compile and clean up')
                os.chdir('madevent/')
                compile = subprocess.Popen(['./bin/compile'])
                compile.wait()
                clean = subprocess.Popen(['./bin/clean4grid'])
                clean.wait()
                os.chdir('../')
                mglog.info('remove old tarball')
                remove_old = subprocess.Popen(['rm',('../'+gridpack_name)])
                remove_old.wait()
                mglog.info('Package up new tarball')
                tar = subprocess.Popen(['tar','cvzf','../'+gridpack_name,'--exclude=lib/PDFsets','.']) 
                tar.wait() 

                os.chdir('../')
                mglog.info('Remove temporary directory')
                remove_tmp = subprocess.Popen(['rm','-fr','tmp%i/'%os.getpid()])
                remove_tmp.wait()
                mglog.info('Tidying up complete!')

        else:


            ### NLO RUN ###

            # In case a MadSpin card was specified setup MadSpin
            if madspin_card_loc:
                # modify run card for nevents 250
                modify_run_card(settings={'nevents': 250})
                # Generate some events -> trigger MadSpin code generation, compilation and setup (overweight estimation)
                run = subprocess.Popen(['./bin/generate_events','--parton','--nocompile','--only_generation','-f','--name='+str(run_name)], stdin=subprocess.PIPE)
                run.wait()
                # The folders with the generated events and decayed events are removed when running from the gridpack


            gridpack_name=(run_name+'_gridpack.tar.gz')
            mglog.info('Creating gridpack (%s)...'%gridpack_name)
            
            os.chdir('../')
            mglog.info('Package up proc_dir')
            os.rename(proc_dir,gridpack_dir) 
            tar = subprocess.Popen(['tar','czf',gridpack_name,gridpack_dir,'--exclude=lib/PDFsets']) 
            tar.wait() 
            os.rename(gridpack_dir,proc_dir) 




        raise RuntimeError('Gridpack sucessfully created, exiting the transform. IGNORE ERRORS if running gridpack generation!')


    if madspin_card_loc:
        if os.path.exists('Events/'+run_name+'_decayed_1'):
            if not isNLO:
                shutil.move('Events/'+run_name+'_decayed_1/unweighted_events.lhe.gz','Events/'+run_name+'/unweighted_events.lhe.gz')
                mglog.info('Moving MadSpin events from %s to %s.'%('Events/'+run_name+'_decayed_1/unweighted_events.lhe.gz','Events/'+run_name+'/unweighted_events.lhe.gz'))
            else:
                shutil.move('Events/'+run_name+'_decayed_1/events.lhe.gz','Events/'+run_name+'/events.lhe.gz')
                mglog.info('Moving MadSpin events from %s to %s.'%('Events/'+run_name+'_decayed_1/events.lhe.gz','Events/'+run_name+'/events.lhe.gz'))

        else:
            mglog.error('MadSpin was run but can\'t find output folder %s.'%('Events/'+run_name+'_decayed_1/'))
            raise RuntimeError('MadSpin was run but can\'t find output folder %s.'%('Events/'+run_name+'_decayed_1/'))

    elif isNLO:

        mglog.info('Moving generated events to be in correct format for arrange_output().')
        mglog.info('Unzipping generated events.')
        unzip = subprocess.Popen(['gunzip','-f','Events/'+run_name+'/events.lhe.gz'])
        unzip.wait()
        
        mglog.info('Moving file over to '+'Events/'+run_name+'/unweighted_events.lhe')
        shutil.move('Events/'+run_name+'/events.lhe','Events/'+run_name+'/unweighted_events.lhe')
        
        mglog.info('Re-zipping into dataset name '+'Events/'+run_name+'/unweighted_events.lhe.gz')
        rezip = subprocess.Popen(['gzip','Events/'+run_name+'/unweighted_events.lhe'])
        rezip.wait()
        
    os.chdir(currdir)

    resetLHAPDF(origLHAPATH=origLHAPATH,origLHAPDF_DATA_PATH=origLHAPDF_DATA_PATH)

    mglog.info('Finished at '+str(time.asctime()))
    return 0


def generate_from_gridpack(run_name='Test',gridpack_dir='madevent/',nevents=-1,random_seed=-1,card_check=None,param_card=None,madspin_card=None,extlhapath=None):

    # Just in case
    setup_path_protection()

    isNLO=is_NLO_run(proc_dir=gridpack_dir)
    LHAPATH=os.environ['LHAPATH'].split(':')[0]

    version = getMadGraphVersion()
    (LHAPATH,origLHAPATH,origLHAPDF_DATA_PATH) = setupLHAPDF(isNLO, version=version, proc_dir=gridpack_dir, extlhapath=extlhapath) 

    setupFastjet(isNLO, proc_dir=gridpack_dir)

    if param_card is not None:
        # only copy param_card if name of destination directory differs
        if param_card != gridpack_dir + '/Cards/' + param_card.split('/')[-1]:
            shutil.copy( param_card , gridpack_dir+'/Cards' )
        mglog.info( 'Moved param card into place: '+str(param_card) )

    # Work in progress...
    #if card_check:
    #    mglog.info('Checking that cards in %s and %s are consistent.'%(gridpack_dir,card_check))
    #    file_grid=open(gridpack_dir+'/Cards/proc_card_mg5.dat','r')
    #    file_proc=open('proc_card_mg5.dat','r')
    #
    #    for line in difflib.unified_diff(file_grid.readlines(), file_proc.readlines(), 
    #                                     fromfile='file1', tofile='file2', lineterm=''):
    #        print line


    mglog.info('Generating events from gridpack')
    
    # Ensure that things are set up normally
    if not os.path.exists(gridpack_dir):
        raise RuntimeError('Gridpack directory not found at '+gridpack_dir)


    mglog.info('>>>> FOUND GRIDPACK <<<<  <- This will be used for generation')
    mglog.info('Generation of '+str(nevents)+' events will be performed using the supplied gridpack with random seed '+str(random_seed))
    mglog.info('Started generating events at '+str(time.asctime()))



    #Remove addmasses if it's there
    if os.access(gridpack_dir+'/bin/internal/addmasses.py',os.R_OK):
        os.remove(gridpack_dir+'/bin/internal/addmasses.py')
        
    currdir=os.getcwd()
    
    if not isNLO:
        ### LO RUN ###
        if not os.access(gridpack_dir+'bin/run.sh',os.R_OK):
            mglog.error('bin/run.sh not found at '+gridpack_dir)
            return 1
        else:
            mglog.info('Found '+gridpack_dir+' bin/run.sh, starting generation.')

        mglog.info('For your information, ls of '+currdir+':')
        mglog.info( sorted( os.listdir( currdir ) ) )
        mglog.info('For your information, ls of '+gridpack_dir+':')
        mglog.info( sorted( os.listdir( gridpack_dir ) ) )
        
        generate = subprocess.Popen([gridpack_dir+'/bin/run.sh',str(nevents),str(random_seed)],stdin=subprocess.PIPE)
        generate.wait()
        
    else:
        ### NLO RUN ###
        if not os.access(gridpack_dir+'bin/generate_events',os.R_OK):
            mglog.error('bin/generate_events not found at '+gridpack_dir)
            return 1
        else:
            mglog.info('Found '+gridpack_dir+'bin/generate_events, starting generation.')
    
        
        ### Editing run card
        run_card_loc=gridpack_dir+'Cards/run_card.dat'
        oldruncard = open(run_card_loc,'r')
        newruncard = open(run_card_loc+'.tmp','w')
        req_acc=0.01
        for line in oldruncard:
            if ' = nevents ' in line:
                newruncard.write(' %i = nevents    ! Number of unweighted events requested \n'%(nevents))
                mglog.info('Setting nevents = %i.'%(nevents))
            elif ' = iseed ' in line: 
                newruncard.write(' %i = iseed      ! rnd seed (0=assigned automatically=default)) \n'%(random_seed)) 
                mglog.info('Setting random number seed = %i.'%(random_seed))

            else:
                newruncard.write(line)
        oldruncard.close()
        newruncard.close()
        shutil.move(run_card_loc+'.tmp',run_card_loc)
        
        print "run_card.dat:",run_card_loc
        runCard = subprocess.Popen(['cat',run_card_loc])
        runCard.wait()
    
        ### Editing config card
        ### Editing config card
        config_card_loc=gridpack_dir+'/Cards/amcatnlo_configuration.txt'
        oldcard = open(config_card_loc,'r')
        newcard = open(config_card_loc+'.tmp','w')
        
        # Make sure params only set once
        run_mode_set=False
        auto_html_set=False
        cltype_set=False
        clqueue_set=False
        nbcore_set=False

        for line in oldcard:
            if 'run_mode =' in line:
                if not run_mode_set:
                    mglog.info('Setting run_mode = %i'%(0))
                    newcard.write('run_mode = %i \n'%(0))
                    run_mode_set=True
            elif 'automatic_html_opening =' in line:
                if not auto_html_set:
                    mglog.info('Setting automatic_html_opening = %s'%('False'))
                    newcard.write('automatic_html_opening = %s \n'%('False'))
                    auto_html_set=True
            elif 'nb_core = ' in line:                                               
                if not nbcore_set:
                    mglog.info('Setting number of cores = %i in %s'%(1,config_card_loc)) 
                    newcard.write('nb_core = %i \n'%(1))                     
                    nbcore_set=True
            else:
                newcard.write(line)
        oldcard.close()
        newcard.close()
        shutil.move(config_card_loc+'.tmp',config_card_loc)
        
        print "amcatnlo_configuration.txt:",config_card_loc
        configCard = subprocess.Popen(['cat',config_card_loc])
        configCard.wait()

        #genxs_loc=gridpack_dir+'/bin/internal/gen_crossxhtml.py'
        #oldcard = open(genxs_loc,'r')
        #newcard = open(genxs_loc+'.tmp','w')
        #for line in oldcard:
        #    if 'if \'lhe\' in self.parton:' in line:
        #        mglog.info('Fixing %s card'%(genxs_loc))
        #        newcard.write('            if \'lhe\' in self.parton: \n')
        #        newcard.write('                link=\'\'\n')
        #    else:
        #        newcard.write(line)
        #oldcard.close()
        #newcard.close()
        #shutil.move(genxs_loc+'.tmp',genxs_loc)
    
        mglog.info('For your information, ls of '+currdir+':')
        mglog.info( sorted( os.listdir( currdir ) ) )
        mglog.info('For your information, ls of '+gridpack_dir+'/Events/:')
        mglog.info( sorted( os.listdir( gridpack_dir+'/Events/' ) ) )
    
        if os.access(gridpack_dir+'/Events/'+run_name, os.F_OK):
            mglog.info('Removing %s/Events/%s directory from gridpack generation.'%(gridpack_dir,run_name))
            shutil.rmtree(gridpack_dir+'/Events/'+run_name)
    
        # Delete events generated when setting up MadSpin during gridpack generation
        if os.access(gridpack_dir+'/Events/'+run_name+'_decayed_1', os.F_OK):
            mglog.info('Removing %s/Events/%s_decayed_1 directory from gridpack generation.'%(gridpack_dir,run_name))
            shutil.rmtree(gridpack_dir+'/Events/'+run_name+'_decayed_1')

        mglog.info('For your information, ls of '+gridpack_dir+'/Events/:')
        mglog.info( sorted( os.listdir( gridpack_dir+'/Events/' ) ) )
    
        mglog.info('Copying make_opts from Template')   
        shutil.copy(os.environ['MADPATH']+'/Template/LO/Source/make_opts',gridpack_dir+'/Source/')   
       
        generate = subprocess.Popen([gridpack_dir+'/bin/generate_events','--parton','--nocompile','--only_generation','-f','--name=%s'%run_name],stdin=subprocess.PIPE)
        generate.wait()

    mglog.info('Copying generated events to %s.'%currdir)

    if madspin_card:
        #if os.path.exists(gridpack_dir+'Events/'+run_name+'_decayed_1'): 
        if not isNLO:
            LOdir='Events/GridRun_%i'%random_seed+'_decayed_1'
            if os.path.exists(gridpack_dir+'/'+LOdir):
                shutil.copy(gridpack_dir+'/'+LOdir+'/unweighted_events.lhe.gz','events.lhe.gz') 
            else: 
                mglog.error('MadSpin was run but can\'t find output folder %s.'%LOdir)
                raise RuntimeError('MadSpin was run but can\'t find output folder %s.'%LOdir)
        else:
            NLOdir='Events/'+run_name+'_decayed_1'
            if os.path.exists(gridpack_dir+'/'+NLOdir):
                shutil.copy(gridpack_dir+'/'+NLOdir+'/events.lhe.gz','events.lhe.gz') 
            else: 
                mglog.error('MadSpin was run but can\'t find output folder %s.'%NLOdir)
                raise RuntimeError('MadSpin was run but can\'t find output folder %s.'%NLOdir)



    else: 

        if not os.path.exists(gridpack_dir+'Events/GridRun_%i/'%random_seed):
            shutil.copy(gridpack_dir+'/Events/'+run_name+'/events.lhe.gz','events.lhe.gz') 

 
        

    mglog.info('For your information, ls of '+currdir+':')
    mglog.info( sorted( os.listdir( currdir ) ) )
 
    mglog.info('Moving generated events to be in correct format for arrange_output().')
    mglog.info('Unzipping generated events.')
    unzip = subprocess.Popen(['gunzip','-f','events.lhe.gz'])
    unzip.wait()

    mglog.info('Moving file over to '+gridpack_dir+'/Events/'+run_name+'/unweighted_events.lhe')
    mkdir = subprocess.Popen(['mkdir','-p',(gridpack_dir+'/Events/'+run_name)])
    mkdir.wait()   
    shutil.move('events.lhe',gridpack_dir+'/Events/'+run_name+'/unweighted_events.lhe')

    mglog.info('Re-zipping into dataset name '+gridpack_dir+'/Events/'+run_name+'/unweighted_events.lhe.gz')
    rezip = subprocess.Popen(['gzip',gridpack_dir+'/Events/'+run_name+'/unweighted_events.lhe'])
    rezip.wait()

    os.chdir(currdir)

    mglog.info('Finished at '+str(time.asctime()))

    resetLHAPDF(origLHAPATH=origLHAPATH,origLHAPDF_DATA_PATH=origLHAPDF_DATA_PATH)

    return 0


def getMadGraphVersion():

    # also need to find out the version (copied from generate)
    madpath=os.environ['MADPATH']
    version=None    
    version_file = open(os.environ['MADPATH']+'/VERSION','r')
    
    for line in version_file:
        if 'version' in line:
            version=line.split('=')[1].strip()
    version_file.close()

    if not version:
        mglog.error('Failed to find MadGraph/MadGraph5_aMC@NLO version')
        # return 1
        sys.exit(1)
    else:
        mglog.info('Found MadGraph/MadGraph5_aMC@NLO version %i.%i.%i'%(int(version.split('.')[0]),int(version.split('.')[1]),int(version.split('.')[2])))

    return(version)


def setupFastjet(isNLO, proc_dir=None):

    mglog.info('Path to fastjet install dir:%s'%os.environ['FASTJETPATH'])


    getfjconfig = subprocess.Popen(['get_files','-data','fastjet-config'])
    getfjconfig.wait() 
    #Get custom fastjet-config 
    if not os.access(os.getcwd()+'/fastjet-config',os.X_OK):
        mglog.error('Failed to get fastjet-config from MadGraphControl')
        return 1
    fastjetconfig = os.getcwd()+'/fastjet-config'
    
    mglog.info('fastjet-config --version:      %s'%str(subprocess.Popen([fastjetconfig, '--version'],stdout = subprocess.PIPE).stdout.read().strip()))
    mglog.info('fastjet-config --prefix:       %s'%str(subprocess.Popen([fastjetconfig, '--prefix'],stdout = subprocess.PIPE).stdout.read().strip()))
        
    if not isNLO:
        config_card=proc_dir+'/Cards/me5_configuration.txt'
    else:
        config_card=proc_dir+'/Cards/amcatnlo_configuration.txt'
        
    oldcard = open(config_card,'r')
    newcard = open(config_card+'.tmp','w')
     
    for line in oldcard:
        if 'fastjet = ' in line:                                                
            newcard.write('fastjet = %s \n'%(fastjetconfig))
            mglog.info('Setting fastjet = %s in %s'%(fastjetconfig,config_card))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()
    shutil.move(config_card+'.tmp',config_card)
    #mglog.info('New %s card:'%config_card)
    #configCard = subprocess.Popen(['cat',config_card])             
    #configCard.wait()

    return


def setupLHAPDF(isNLO, version=None, proc_dir=None, extlhapath=None):

    origLHAPATH=os.environ['LHAPATH']
    origLHAPDF_DATA_PATH=os.environ['LHAPDF_DATA_PATH']


    LHAPATH=os.environ['LHAPATH'].split(':')[0]
    if len(os.environ['LHAPATH'].split(':')) >=2 :
        LHADATAPATH=os.environ['LHAPATH'].split(':')[1]
    else:
        LHADATAPATH=os.environ['LHAPATH'].split(':')[0]

    if isNLO:
        os.environ['LHAPDF_DATA_PATH']=LHADATAPATH

    mglog.info('Path to LHAPDF install dir:%s'%LHAPATH)
    mglog.info('Path to LHAPDF data dir: %s'%LHADATAPATH)
    if not os.path.isdir(LHADATAPATH):
        mglog.error('LHAPDF data dir: %s is not accesible'%LHADATAPATH)
        return 1
    if not os.path.isdir(LHAPATH):
        mglog.error('LHAPDF path dir: %s is not accesible'%LHAPATH)
        return 1

    # Dealing with LHAPDF (Only need to edit configuration file for 2.1.1 onwards)
    if int(version.split('.')[0]) >= 2 and ( int(version.split('.')[1]) > 1 or ( int(version.split('.')[1]) == 1 and int(version.split('.')[2]) > 0) ):

        if extlhapath:
            lhapdfconfig=extlhapath
            if not os.access(lhapdfconfig,os.X_OK):
                mglog.error('Failed to find valid external lhapdf-config at %s'%lhapdfconfig)
                return 1
            LHADATAPATH=subprocess.Popen([lhapdfconfig, '--datadir'],stdout = subprocess.PIPE).stdout.read().strip()
            mglog.info('Changing LHAPDF_DATA_PATH to %s'%LHADATAPATH)
            os.environ['LHAPDF_DATA_PATH']=LHADATAPATH
        else:
            getlhaconfig = subprocess.Popen(['get_files','-data','lhapdf-config'])
            getlhaconfig.wait() 
            #Get custom lhapdf-config 
            if not os.access(os.getcwd()+'/lhapdf-config',os.X_OK):
                mglog.error('Failed to get lhapdf-config from MadGraphControl')
                return 1
            lhapdfconfig = os.getcwd()+'/lhapdf-config'

        mglog.info('lhapdf-config --version:      %s'%str(subprocess.Popen([lhapdfconfig, '--version'],stdout = subprocess.PIPE).stdout.read().strip()))
        mglog.info('lhapdf-config --prefix:       %s'%str(subprocess.Popen([lhapdfconfig, '--prefix'],stdout = subprocess.PIPE).stdout.read().strip()))
        mglog.info('lhapdf-config --libdir:       %s'%str(subprocess.Popen([lhapdfconfig, '--libdir'],stdout = subprocess.PIPE).stdout.read().strip()))
        mglog.info('lhapdf-config --datadir:      %s'%str(subprocess.Popen([lhapdfconfig, '--datadir'],stdout = subprocess.PIPE).stdout.read().strip()))
        mglog.info('lhapdf-config --pdfsets-path: %s'%str(subprocess.Popen([lhapdfconfig, '--pdfsets-path'],stdout = subprocess.PIPE).stdout.read().strip()))


        if not isNLO:
            config_card=proc_dir+'/Cards/me5_configuration.txt'
        else:
            config_card=proc_dir+'/Cards/amcatnlo_configuration.txt'

        oldcard = open(config_card,'r')
        newcard = open(config_card+'.tmp','w')
     
        for line in oldcard:
            if 'lhapdf = ' in line:                                                
                newcard.write('lhapdf = %s \n'%(lhapdfconfig))
                mglog.info('Setting lhapdf = %s in %s'%(lhapdfconfig,config_card))
            else:
                newcard.write(line)
        oldcard.close()
        newcard.close()
        shutil.move(config_card+'.tmp',config_card)
        #mglog.info('New me5_configuration.txt card:')
        #configCard = subprocess.Popen(['cat',config_card])             
        #configCard.wait()

        mglog.info('Creating links for LHAPDF')
        if os.path.islink(proc_dir+'/lib/PDFsets'):
            os.unlink(proc_dir+'/lib/PDFsets')
        elif os.path.isdir(proc_dir+'/lib/PDFsets'):
            shutil.rmtree(proc_dir+'/lib/PDFsets')
        os.symlink(LHADATAPATH,proc_dir+'/lib/PDFsets')
        mglog.info('Available PDFs are:')
        mglog.info( sorted( [ x for x in os.listdir(proc_dir+'/lib/PDFsets') if not ".tar.gz" in x ] ) )
        
    else:
        # Nasty fixes for MG5v1:
        # Check first for external LHAPDF
        if extlhapath:
            mglog.info('Using external LHAPDF.')
            # Store LHAPATH that comes with release before editing
            releaselhapath=LHAPATH
            # Need to fix path to be absolute before symlinking
            if os.path.isabs(extlhapath):
                LHAPATH=extlhapath
            else:
                LHAPATH=os.environ['TestArea']+'/'+extlhapath
                mglog.info('Modifying extlhapath to be absolute - assuming installation is in '+os.environ['TestArea'])
            mglog.info('External LHAPATH is:')
            mglog.info('  '+LHAPATH)
        else:
            mglog.info('Using Athena release LHAPDF, LHAPATH is:')
            mglog.info('  '+LHAPATH)
                        
        mglog.info('Creating links for LHAPDF')
        os.symlink(LHAPATH,proc_dir+'/lib/PDFsets')
        mglog.info('Available PDFs are:')
        mglog.info( sorted( os.listdir( proc_dir+'/lib/PDFsets/' ) ) )
    
        if not extlhapath:
            mglog.info( 'Linking in libraries based on the path '+LHAPATH+' with config '+os.environ['CMTCONFIG'] )
            lhalibpath = LHAPATH.split('share/')[0]+os.environ['CMTCONFIG']+'/lib/'
        else:
            useReleaseLibs=False
            lhalibpath = LHAPATH.split('share/')[0]+'/lib/'
            mglog.info( 'Try linking in libraries from '+lhalibpath)
            if not os.access( lhalibpath+'libLHAPDF.a',os.R_OK):
                mglog.warning('libLHAPDF.a not found in '+lhalibpath)
                useReleaseLibs=True
            if not os.access( lhalibpath+'libLHAPDF.so',os.R_OK):
                mglog.warning('libLHAPDF.so not found in '+lhalibpath)
                useReleaseLibs=True
    
            if useReleaseLibs:
                mglog.info('Resorting to Athena release LHAPDF libraries even though external LHAPATH provided.') 
                mglog.info('Try finding libraries in Athena release based on CMTCONFIG '+os.environ['CMTCONFIG']+' and path:')
                mglog.info('  '+releaselhapath)
                lhalibpath = releaselhapath.split(
                    'share/')[0]+os.environ['CMTCONFIG']+'/lib/'
                
        os.symlink( lhalibpath+'libLHAPDF.a',proc_dir+'/lib/libLHAPDF.a')
        os.symlink( lhalibpath+'libLHAPDF.so',proc_dir+'/lib/libLHAPDF.so')

    return (LHAPATH,origLHAPATH,origLHAPDF_DATA_PATH)


def resetLHAPDF(origLHAPATH='',origLHAPDF_DATA_PATH=''):
    mglog.info('Restoring original LHAPDF env variables:')
    os.environ['LHAPATH']=origLHAPATH
    os.environ['LHAPDF_DATA_PATH']=origLHAPDF_DATA_PATH
    mglog.info('LHAPATH=%s'%os.environ['LHAPATH'])
    mglog.info('LHAPDF_DATA_PATH=%s'%os.environ['LHAPDF_DATA_PATH'])


def add_lifetimes(process_dir=None,threshold=None):
    """ Add lifetimes to the generated LHE file.  Should be
    called after generate_events is called.
    """
    try:
        from __main__ import opts
        if opts.config_only:
            mglog.info('Athena running on config only mode: not executing MadGraph')
            return
    except:
        pass

    from glob import glob
    if process_dir==None:
        from glob import glob
        if len(glob('*PROC*'))<1:
            mglog.error('Process directory could not be found!')
        else:
            process_dir = glob('*PROC*')[-1]
    me_exec=process_dir+'/bin/madevent'
    if len(glob(process_dir+'/Events/*'))<1:
        mglog.error('Process dir %s does not contain events?'%process_dir)
    run = glob(process_dir+'/Events/*')[0].split('/')[-1]

    # Note : This slightly clunky implementation is needed for the time being
    # See : https://answers.launchpad.net/mg5amcnlo/+question/267904

    tof_c = open('time_of_flight_exec_card','w')
    tof_c.write('add_time_of_flight '+run+((' --threshold='+str(threshold)) if threshold is not None else ''))
    tof_c.close()

    mglog.info('Started adding time of flight info '+str(time.asctime()))

    generate = subprocess.Popen([me_exec,'time_of_flight_exec_card'],stdin=subprocess.PIPE)
    generate.wait()

    mglog.info('Finished adding time of flight information at '+str(time.asctime()))

    return True


def arrange_output(run_name='Test',proc_dir='PROC_mssm_0',outputDS='madgraph_OTF._00001.events.tar.gz',lhe_version=None,saveProcDir=False):
    try:
        from __main__ import opts
        if opts.config_only:
            mglog.info('Athena running on config only mode: not executing MadGraph')
            return
    except:
        pass

    # NLO is not *really* the question here, we need to know if we should look for weighted or
    #  unweighted events in the output directory.  MadSpin (above) only seems to give weighted
    #  results for now?
    #isNLO=is_NLO_run(proc_dir=proc_dir)
    hasUnweighted = os.access(proc_dir+'/Events/'+run_name+'/unweighted_events.lhe.gz',os.R_OK)

    # Clean up in case a link or file was already there
    if os.path.exists(os.getcwd()+'/events.lhe'): os.remove(os.getcwd()+'/events.lhe')

    mglog.info('Unzipping generated events.')
    if hasUnweighted:
        unzip = subprocess.Popen(['gunzip','-f',proc_dir+'/Events/'+run_name+'/unweighted_events.lhe.gz'])
        unzip.wait()
    else:
        unzip = subprocess.Popen(['gunzip','-f',proc_dir+'/Events/'+run_name+'/events.lhe.gz'])
        unzip.wait()

    mglog.info('Putting a copy in place for the transform.')
    if hasUnweighted:
        orig_input = open(proc_dir+'/Events/'+run_name+'/unweighted_events.lhe','r')
        mod_output = open(os.getcwd()+'/events.lhe','w')
    else:
        orig_input = open(proc_dir+'/Events/'+run_name+'/events.lhe','r')
        mod_output = open(os.getcwd()+'/events.lhe','w')


    #Removing empty lines in LHE
    nEmpty=0
    for line in orig_input.readlines():
        if line.strip():
            mod_output.write(line)
        else:
            nEmpty=nEmpty+1
    orig_input.close()
    mod_output.close()

    mglog.info('Removed %i empty lines from LHEF.'%nEmpty)

    if lhe_version:            
        mod_output2 = open(os.getcwd()+'/events.lhe','r')
        test=mod_output2.readline()
        if 'version="' in test:
            mglog.info('Applying LHE version hack')
            final_file = open(os.getcwd()+'/events.lhe.copy','w')
            final_file.write('<LesHouchesEvents version="%i.0">\n'%lhe_version)
            shutil.copyfileobj(mod_output2, final_file)
            final_file.close()
            shutil.copy(os.getcwd()+'/events.lhe.copy',os.getcwd()+'/events.lhe')
        mod_output2.close()


    # Actually move over the dataset - this first part is horrible...
    variables = {}
    if os.access('runargs.generate.py',os.R_OK): 
        execfile('runargs.generate.py',variables) 
    else: 
        execfile('runargs.Generate.py',variables) 

    outputTXTFile = None
    if 'runArgs' in variables and hasattr(variables['runArgs'],'outputTXTFile'):
        outputTXTFile = variables['runArgs'].outputTXTFile

    if outputTXTFile is None: outputDS = 'tmp_'+outputDS
    else:                     outputDS = outputTXTFile

    mglog.info('Moving file over to '+outputDS.split('.tar.gz')[0]+'.events')

    shutil.move(os.getcwd()+'/events.lhe',outputDS.split('.tar.gz')[0]+'.events')

    mglog.info('Re-zipping into dataset name '+outputDS)
    rezip = subprocess.Popen(['tar','cvzf',outputDS,outputDS.split('.tar.gz')[0]+'.events'])
    rezip.wait()

    if not saveProcDir:
        mglog.info('Blasting away the process directory')
        shutil.rmtree(proc_dir,ignore_errors=True)

    mglog.info('All done with output arranging!')
    return outputDS


def helpful_definitions():
    return """
# Define multiparticle labels
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
define pb = g u c d s b u~ c~ d~ s~ b~
define jb = g u c d s b u~ c~ d~ s~ b~
define l+ = e+ mu+
define l- = e- mu-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
define fu = u c e+ mu+ ta+
define fu~ = u~ c~ e- mu- ta-
define fd = d s ve~ vm~ vt~
define fd~ = d~ s~ ve vm vt
define susystrong = go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~
define susyweak = el- el+ er- er+ mul- mul+ mur- mur+ ta1- ta1+ ta2- ta2+ n1 n2 n3 n4 x1- x1+ x2- x2+ sve sve~ svm svm~ svt svt~ 
define susylq = ul ur dl dr cl cr sl sr
define susylq~ = ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~
define susysq = ul ur dl dr cl cr sl sr t1 t2 b1 b2
define susysq~ = ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~
define susysl = el- el+ er- er+ mul- mul+ mur- mur+ ta1- ta1+ ta2- ta2+
define susyv = sve svm svt
define susyv~ = sve~ svm~ svt~
"""

def strong_process_dict( njets = 1 , gentype = 'GG' ):
    header = """
import model mssm"""
    header += helpful_definitions()
    header += """
# Specify process(es) to run

"""
    footer = """
# Output processes to MadEvent directory
output -f
"""

    jetloop = [ '' ]
    if 0<njets: jetloop += [ 'j' ]
    if 1<njets: jetloop += [ 'j j' ]
    if 2<njets: jetloop += [ 'j j j' ]
    if 3<njets: jetloop += [ 'j j j j' ]
    if 4<njets: jetloop += [ 'j j j j j' ]
    if 5<njets: jetloop += [ 'j j j j j j' ]
    if 6<njets: jetloop += [ 'j j j j j j j' ]
    if 7<njets: jetloop += [ 'j j j j j j j j' ]
    if 8<njets:
        mglog.error('That is just ridiculous.  Do not generate more than eight jets off of your main process in MadGraph.  The world will implode.')
        return {}

    starter = 'generate'
    squarks = ['ul','dl','cl','sl','ur','dr','cr','sr']
    ss_string = ''
    sg_string = ''
    ssb_string = ''
    gg_string = ''
    n1n1_string = ''
    n2n3_string = ''
    c1n2_string = ''
    cc_string = ''
    bb_string = ''
    tt_string = ''
    Scharm_string = ''
    first = True
    for jets in jetloop:
        bb_string += '%s p p > b1 b1~ %s $ go ul ur dl dr cl cr sl sr t1 t2 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b2~ \n'%(starter,jets)
        tt_string += '%s p p > t1 t1~ %s $ go ul ur dl dr cl cr sl sr t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ b1~ t2~ b2~ \n'%(starter,jets)
        n1n1_string += '%s p p > n1 n1 %s $ susystrong \n'%(starter,jets)
        n2n3_string += '%s p p > n2 n3 %s $ susystrong \n'%(starter,jets)
        gg_string += '%s p p > go go %s $ susysq \n'%(starter,jets)
        for sign1 in ['+','-']:
            c1n2_string += '%s p p > x1%s n2 %s $ susystrong \n'%(starter,sign1,jets)
            for sign2 in ['+','-']:
                cc_string += '%s p p > x1%s x1%s %s $ susystrong \n'%(starter,sign1,sign2,jets)
                starter = 'add process'
        if first: starter = 'generate'
        for s in ['cl','cr']:
            for t in ['cl','cr']:
                Scharm_string += '%s p p > %s %s~ %s $ go \n'%(starter,s,t,jets)
                starter = 'add process'
        if first: starter = 'generate'
        for s in squarks:
            sg_string += '%s p p > %s go %s \n'%(starter,s,jets)
            sg_string += '%s p p > %s~ go %s \n'%(starter,s,jets)
            for t in squarks:
                if 'phot' in gentype:
                    ss_string += '%s p p > %s %s~ a %s $ go \n'%(starter,s,t,jets)
                    starter = 'add process'
                else:
                    ss_string += '%s p p > %s %s~ %s $ go \n'%(starter,s,t,jets)
                    starter = 'add process'
                ssb_string += '%s p p > %s %s~ %s \n'%(starter,s,t,jets)
        first = False

    processes = {
        'SS' : ss_string ,
        'SSphot' : ss_string,
        'GG' : gg_string ,
        'SG' : sg_string ,
        'GS' : sg_string ,
        'CC' : cc_string ,
        'BB' : bb_string ,
        'Scharm' : Scharm_string,
        'TT' : tt_string ,
        'N2N3' : n2n3_string ,
        'N1N1' : n1n1_string ,
        'C1N2' : c1n2_string ,
        'SSB' : ssb_string ,
        'ALL' : ss_string+'\n'+gg_string+'\n'+ssb_string+'\n'+sg_string
      }

    if not gentype in processes.keys():
        mglog.error('No idea how to deal with the simplified models for'+gentype+'.  Sorry!')
        return ''
    if processes[gentype] == '':
        mglog.error('No processes found for the set up you requested: '+str(gentype))
        return ''
    mglog.info('Will run MadGraph over the process:')
    mglog.info(str(processes[gentype]))


    return header+processes[gentype]+footer


def get_variations( gentype , masses , syst_mod , xqcut = None ):
    if xqcut is None:
        xqcut=500 # default
        if 'Scharm'==gentype:
            if masses['1000004']<xqcut*4.: xqcut = masses['1000004']*0.25
        elif 'N2N3'==gentype:
            if masses['1000025']<xqcut*4.: xqcut = masses['1000025']*0.25
        elif 'N1N1'==gentype:
            if masses['1000022']<xqcut*4.: xqcut = masses['1000022']*0.22
        elif 'C1N2'==gentype:
            if masses['1000024']<xqcut*4.: xqcut = masses['1000024']*0.25
        elif 'Stau'==gentype:
            if masses['1000015']<xqcut*4.: xqcut = masses['1000015']*0.25
        elif 'SlepSlep'==gentype:
            if masses['1000011']<xqcut*4.: xqcut = masses['1000011']*0.25
        elif 'T2' in gentype:
            if masses['2000006']<xqcut*4.: xqcut = masses['2000006']*0.25
        else:
            if 'G' in gentype or 'ALL' in gentype:
                if masses['1000021']<xqcut*4.: xqcut = masses['1000021']*0.25
            if 'S' in gentype or 'ALL' in gentype:
                if masses['1000002']<xqcut*4.: xqcut = masses['1000002']*0.25
            if 'T' in gentype:
                if masses['1000006']<xqcut*4.: xqcut = masses['1000006']*0.25
            if 'B' in gentype:
                if masses['1000005']<xqcut*4.: xqcut = masses['1000005']*0.25
            if 'C' in gentype:
                if masses['1000024']<xqcut*4.: xqcut = masses['1000024']*0.25
            if 'D' in gentype:
                if masses['2000001']<xqcut*4.: xqcut = masses['2000001']*0.25
        if syst_mod is not None and 'qup' in syst_mod.lower(): xqcut = xqcut*2.
        elif syst_mod is not None and 'qdown' in syst_mod.lower(): xqcut = xqcut*0.5
    mglog.info('For matching, will use xqcut of '+str(xqcut))

    alpsfact = 1.0
    scalefact = 1.0
    if syst_mod is not None and 'alpsfactup' in syst_mod.lower():
        alpsfact = 2.0
    elif syst_mod is not None and 'alpsfactdown' in syst_mod.lower():
        alpsfact = 0.5

    if syst_mod is not None and 'scalefactup' in syst_mod.lower(): scalefact = 2.0
    elif syst_mod is not None and 'scalefactdown' in syst_mod.lower(): scalefact = 0.5

    return xqcut , alpsfact , scalefact


def SUSY_StrongSM_Generation(runArgs = None, gentype='SS',decaytype='direct',masses=None,\
                             nevts=None, njets=1, syst_mod=None,\
                             SLHAonly=False, keepOutput=False, SLHAexactCopy=False,\
                             writeGridpack=False,gridpackDirName=None,getnewruncard=False,MSSMCalc=False,pdlabel="'cteq6l1'",\
                             lhaid=10042,madspin_card=None):
    # Set beam energy
    beamEnergy = 6500.
    if hasattr(runArgs,'ecmEnergy'): beamEnergy = runArgs.ecmEnergy * 0.5

    # Set random seed
    rand_seed=1234
    if hasattr(runArgs, "randomSeed"): rand_seed=runArgs.randomSeed

    # Sensible defaults for number of events
    if nevts is None: nevts = 10000.

    if not os.environ.has_key('MADGRAPH_DATA'):
        os.environ['MADGRAPH_DATA']=os.getcwd()
        mglog.warning('Setting your MADGRAPH_DATA environmental variable to the working directory')
    # Set up production and decay strings
    if nevts<1000 or nevts>10000000:
        mglog.error('Bad idea to generate '+str(nevts)+' events.  MadGraph wont complain, but the job will never end.  Bailing out now.')
        return -1,''

    process = strong_process_dict(njets,gentype)

    xqcut , alpsfact , scalefact = get_variations( gentype , masses , syst_mod )

    if not SLHAonly and (writeGridpack or gridpackDirName is None):
        # Generate the new process!
        thedir = new_process(card_loc=process)
        if 1==thedir:
            mglog.error('Error in process generation!')
            return -1,''
        mglog.info('Using process directory '+str(thedir))

    if MSSMCalc:
        # Grab the param card and move the new masses into place
        build_param_card(param_card_old='param_card.SM.SG.dat',param_card_new='LH.dat',masses=masses)
    
        mglog.info('Running MSSMCalc')
        runMSSMCalc = subprocess.Popen([ os.environ['MADPATH']+'/Calculators/mssm/MSSMCalc'])
        runMSSMCalc.wait()
        if not os.access('param_card.dat',os.R_OK):
            mglog.error('Problem generating param card!!  Will bail out...')
            return -1,''
    else:
        # Grab the param card and move the new masses into place
        if SLHAexactCopy:
            str_masses = '_'.join(sorted(masses.values()))
            str_param_card = 'param_card.SM.%s.%s.dat'%(gentype,decaytype,str_masses)
            proc_paramcard = subprocess.Popen(['get_files','-data',str_param_card])
            proc_paramcard.wait()
            if not os.access(str_param_card,os.R_OK):
                mglog.info('Could not get param card '+str_param_card)
        else:
            build_param_card(param_card_old='param_card.SM.%s.%s.dat'%(gentype,decaytype),param_card_new='param_card.dat',masses=masses)

    if SLHAonly:
        mglog.info('Not running generation - only setting up SLHA file')
        return [xqcut,'dummy']

    # Generate events!
    if gridpackDirName is not None:
        if writeGridpack==False:
            mglog.info('Generating events from gridpack')
            if generate_from_gridpack(run_name='Test',gridpack_dir=gridpackDirName,nevents=int(nevts),random_seed=rand_seed,param_card='param_card.dat'):
                mglog.error('Error generating events!')
                return -1
            thedir=gridpackDirName
        else:
            mglog.error('Wrong combination of arguments! writeGridpack='+str(writeGridpack)+' gridpackDirName='+str(gridpackDirName))
            return -1
    else:

        # Grab the run card and move it into place
        if 'phot' in gentype:
            build_run_card(run_card_old='run_card.SMphot.dat',run_card_new='run_card.dat',
                       xqcut=xqcut,nevts=nevts,rand_seed=rand_seed,beamEnergy=beamEnergy, scalefact=scalefact, alpsfact=alpsfact)
        else:
           if getnewruncard==True:
               extras = { 'ktdurham':xqcut , 'lhe_version':'2.0' , 'cut_decays':'F' , 'pdlabel':pdlabel , 'lhaid':lhaid , 'drjj':0.0 }
               build_run_card(run_card_old=get_default_runcard(thedir),run_card_new='run_card.dat',xqcut=0,
                                      nevts=nevts,rand_seed=rand_seed,beamEnergy=beamEnergy, scalefact=scalefact, alpsfact=alpsfact,extras=extras)
           else:
               build_run_card(run_card_old='run_card.SM.dat',run_card_new='run_card.dat',
                           xqcut=xqcut,nevts=nevts,rand_seed=rand_seed,beamEnergy=beamEnergy, scalefact=scalefact, alpsfact=alpsfact)

        # Now do the actual event generation
        if generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir=thedir,grid_pack=writeGridpack,madspin_card_loc=madspin_card):
            mglog.error('Error generating events!')
            return -1

    # Move output files into the appropriate place, with the appropriate name
    the_spot = arrange_output(run_name='Test',proc_dir=thedir,outputDS='madgraph_OTF._00001.events.tar.gz',saveProcDir=keepOutput)
    if the_spot == '':
        mglog.error('Error arranging output dataset!')
        return -1

    mglog.info('All done generating events!!')
    return [xqcut,the_spot]


def SUSY_SM_Generation(runArgs = None, process='', gentype='SS',decaytype='direct',masses=None,\
                       nevts=None, syst_mod=None,xqcut=None, SLHAonly=False, keepOutput=False, SLHAexactCopy=False,\
                       writeGridpack=False,gridpackDirName=None,MSSMCalc=False,pdlabel="'cteq6l1'",lhaid=10042,\
                       madspin_card=None,decays={},extras=None):
    # Set beam energy
    beamEnergy = 6500.
    if hasattr(runArgs,'ecmEnergy'): beamEnergy = runArgs.ecmEnergy * 0.5

    # Set random seed
    rand_seed=1234
    if hasattr(runArgs, "randomSeed"): rand_seed=runArgs.randomSeed

    # Sensible defaults for number of events
    if nevts is None: nevts = 10000.

    if not os.environ.has_key('MADGRAPH_DATA'):
        os.environ['MADGRAPH_DATA']=os.getcwd()
        mglog.warning('Setting your MADGRAPH_DATA environmental variable to the working directory')
    # Set up production and decay strings
    if nevts<1000 or nevts>10000000:
        mglog.error('Bad idea to generate '+str(nevts)+' events.  MadGraph wont complain, but the job will never end.  Bailing out now.')
        return -1,''

    xqcut , alpsfact , scalefact = get_variations( gentype , masses , syst_mod , xqcut=xqcut )

    if not SLHAonly and (writeGridpack or gridpackDirName is None):
        # Generate the new process!
        if 'import model' in process:
            mglog.info('Assuming that you have specified the model in your process string already')
            full_proc = ''
            for l in process.split('\n'):
                if 'import model' in l:
                    full_proc += l+'\n'
                    break
            full_proc+=helpful_definitions()
            for l in process.split('\n'):
                if 'import model' not in l:
                    full_proc += l+'\n'
            full_proc+="""
# Output processes to MadEvent directory
output -f
"""
        else:
            full_proc = """
import model mssm
"""+helpful_definitions()+"""
# Specify process(es) to run

"""+process+"""
# Output processes to MadEvent directory
output -f
"""
        thedir = new_process(card_loc=full_proc)
        if 1==thedir:
            mglog.error('Error in process generation!')
            return -1,''
        mglog.info('Using process directory '+str(thedir))

    if MSSMCalc:
        # Grab the param card and move the new masses into place
        build_param_card(param_card_old='param_card.SM.SG.dat',param_card_new='LH.dat',masses=masses,decays=decays)

        mglog.info('Running MSSMCalc')
        runMSSMCalc = subprocess.Popen([ os.environ['MADPATH']+'/Calculators/mssm/MSSMCalc'])
        runMSSMCalc.wait()
        if not os.access('param_card.dat',os.R_OK):
            mglog.error('Problem generating param card!!  Will bail out...')
            return -1,''
    else:
        # Grab the param card and move the new masses into place
        if SLHAexactCopy:
            str_masses = '_'.join(sorted(masses.values()))
            str_param_card = 'param_card.SM.%s.%s.dat'%(gentype,decaytype)
            proc_paramcard = subprocess.Popen(['get_files','-data',str_param_card])
            proc_paramcard.wait()
            if not os.access(str_param_card,os.R_OK):
                mglog.info('Could not get param card '+str_param_card)
        else:
            build_param_card(param_card_old='param_card.SM.%s.%s.dat'%(gentype,decaytype),param_card_new='param_card.dat',masses=masses,decays=decays)

    if SLHAonly:
        mglog.info('Not running generation - only setting up SLHA file')
        return [xqcut,'dummy']

    # Set up the extras dictionary
    if extras is None:
        extras = { 'ktdurham':xqcut , 'lhe_version':'2.0' , 'cut_decays':'F' , 'pdlabel':pdlabel , 'lhaid':lhaid , 'drjj':0.0 }
    else:
        if 'drjj' not in extras: extras['drjj']=0.0
        if 'lhe_version' not in extras: extras['lhe_version']='2.0'
        if 'cut_decays' not in extras: extras['cut_decays']='F'
        if ('pdlabel' in extras and pdlabel is not None) or\
           ('lhaid' in extras and lhaid is not None) or\
           ('ktdurham' in extras and xqcut is not None):
            mglog.error('Tried to define variables in two places.  Please pass pdlabel, lhaid, and ktduram ONLY through either the extras dictionary OR the function parameters')
            return -1

        if 'pdlabel' not in extras: extras['pdlabel']=pdlabel
        if 'lhaid' not in extras: extras['lhaid']=lhaid
        if 'ktdurham' not in extras: extras['ktdurham']=xqcut

    # Generate events!
    if gridpackDirName is not None:
        if writeGridpack==False:
            mglog.info('Generating events from gridpack')
            if generate_from_gridpack(run_name='Test',gridpack_dir=gridpackDirName,nevents=int(nevts),random_seed=rand_seed,param_card='param_card.dat'):
                mglog.error('Error generating events!')
                return -1
            thedir=gridpackDirName
        else:
            mglog.error('Wrong combination of arguments! writeGridpack='+str(writeGridpack)+' gridpackDirName='+str(gridpackDirName))
            return -1
    else:
        # Grab the run card and move it into place
        build_run_card(run_card_old=get_default_runcard(thedir),run_card_new='run_card.dat',xqcut=0,
                       nevts=nevts,rand_seed=rand_seed,beamEnergy=beamEnergy, scalefact=scalefact, alpsfact=alpsfact,extras=extras)
        if generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir=thedir,grid_pack=writeGridpack,madspin_card_loc=madspin_card):
            mglog.error('Error generating events!')
            return -1

    # Move output files into the appropriate place, with the appropriate name
    the_spot = arrange_output(run_name='Test',proc_dir=thedir,outputDS='madgraph_OTF._00001.events.tar.gz',saveProcDir=keepOutput)
    if the_spot == '':
        mglog.error('Error arranging output dataset!')
        return -1

    mglog.info('All done generating events!!')
    return [xqcut,the_spot]


def build_param_card(param_card_old=None,param_card_new='param_card.dat',masses={},decays={},extras={},params={}):
    """Build a new param_card.dat from an existing one.
    Params should be a dictionary of dictionaries. The first key is the block name, and the second in the param name.
    Eventually params will replace the other arguments, but we need to keep them for backward compatibility for now."""
    # Grab the old param card and move it into place
    paramcard = subprocess.Popen(['get_files','-data',param_card_old])
    paramcard.wait()
    if not os.access(param_card_old,os.R_OK):
        mglog.info('Could not get param card '+param_card_old)
    if os.access(param_card_new,os.R_OK):
        mglog.error('Old param card at'+str(param_card_new)+' in the current directory. Dont want to clobber it. Please move it first.')
        return -1

    #ensure all blocknames and paramnames are upper case
    for blockName in params:
       params[blockName.upper()] = params.pop(blockName)
       for paramName in params[blockName.upper()]:
          params[blockName.upper()][paramName.upper()] = params[blockName.upper()].pop(paramName)

    oldcard = open(param_card_old,'r')
    newcard = open(param_card_new,'w')
    decayEdit = False #only becomes true in a DECAY block when specifying the BR
    blockName = ""
    doneParams = {} #tracks which params have been done
    for line in oldcard:
        if line.strip().upper().startswith('BLOCK') or line.strip().upper().startswith('DECAY')\
                    and len(line.strip().split()) > 1: 
            if decayEdit and blockName == 'DECAY': decayEdit = False # Start a new DECAY block
            pos = 0 if line.strip().startswith('DECAY') else 1
            blockName = line.strip().upper().split()[pos]
        if decayEdit: continue #skipping these lines because we are in an edit of the DECAY BR

        akey = None
        if blockName != 'DECAY' and len(line.strip().split()) > 0:
            akey = line.strip().split()[0]
        elif blockName == 'DECAY' and len(line.strip().split()) > 1:
            akey = line.strip().split()[1]
        if akey==None:
           newcard.write(line)
           continue

        if akey != None and blockName == 'MASS'  and akey in masses:
            newcard.write('   %s    %s  # \n'%(akey,str(masses[akey])))
            mglog.info('   %s    %s  #'%(akey,str(masses[akey])))
            decayEdit = False
            # Error checking
            if 'MASS' in params:
                mglog.error('Conflicting use of mass and params')
                return -1
            continue
        elif akey != None and blockName == 'BSM'   and akey in extras:
            newcard.write('   %s    %s  # \n'%(akey,str(extras[akey])))
            mglog.info('   %s    %s  #'%(akey,str(extras[akey])))
            decayEdit = False
            # Error checking
            if 'BSM' in params:
                mglog.error('Conflicting use of extras for BSM and params')
                return -1
            continue
        elif akey != None and blockName == 'DECAY' and akey in decays:
            for newline in decays[akey].splitlines():
                newcard.write(newline+'\n')
                mglog.info(newline)
            decayEdit = True
            # Error checking
            if 'DECAY' in params:
                mglog.error('Conflicting use of decay and params')
                return -1
            continue

        #check if we have params for this block 
        if not params.has_key(blockName):
           newcard.write(line)
           continue
        blockParams = params[blockName]

        # look for a string key, which would follow a # 
        stringkey = None
        if '#' in line: #ignores comment lines
           stringkey = line.strip()[line.strip().find('#')+1:].strip()
           if len(stringkey.split()) > 0: stringkey = stringkey.split()[0].upper()

        if not akey in blockParams and not (stringkey != None and stringkey in blockParams):
           newcard.write(line)
           continue

        if akey in blockParams and (stringkey != None and stringkey in blockParams):
           mglog.error('Conflicting use of numeric and string keys %s and %s' % (akey,stringkey))
           return -1
        theParam = blockParams.get(akey,blockParams[stringkey])
        if not blockName in doneParams: doneParams[blockName] = {}
        if akey in blockParams: doneParams[blockName][akey]=True
        elif stringkey != None and stringkey in blockParams: doneParams[blockName][stringkey]=True

        #do special case of DECAY block
        if blockName=="DECAY":
           if theParam.splitlines()[0].split()[0]=="DECAY":
               #specifying the full decay block
               for newline in theParam.splitlines():
                    newcard.write(newline+'\n')
                    mglog.info(newline)
               decayEdit = True
           else: #just updating the total width
              newcard.write('DECAY   %s    %s  # %s\n'%(akey,str(theParam),line.strip()[line.strip().find('#')+1:] if line.strip().find('#')>0 else ""))
              mglog.info('DECAY   %s    %s  # %s\n'%(akey,str(theParam),line.strip()[line.strip().find('#')+1:] if line.strip().find('#')>0 else ""))
        # second special case of QNUMBERS
        elif blockName=='QNUMBERS':
           #specifying the full QNUMBERS block
           for newline in theParam.splitlines():
                newcard.write(newline+'\n')
                mglog.info(newline)
           decayEdit = True
        else: #just updating the parameter
           newcard.write('   %s    %s  # %s\n'%(akey,str(theParam),line.strip()[line.strip().find('#')+1:] if line.strip().find('#')>0 else ""))
           mglog.info('   %s    %s  # %s\n'%(akey,str(theParam),line.strip()[line.strip().find('#')+1:] if line.strip().find('#')>0 else ""))
        # Done editing the line!

    #check that all specified parameters have been updated (helps to catch typos)
    for blockName in params:
       if not blockName in doneParams:
          mglog.error('Did not find any of the parameters for block %s in param_card' % blockName)
          return -1
       for paramName in params[blockName]:
          if not paramName in doneParams[blockName]:
            mglog.error('Was not able to replace parameter %s in param_card' % paramName)
            return -1

    # Close up and return
    oldcard.close()
    newcard.close()
    return param_card_new

def build_run_card(run_card_old='run_card.SM.dat',run_card_new='run_card.dat',
                   xqcut=0.,nevts=60000,rand_seed=1234,beamEnergy=4000.,
                   scalefact=1.,alpsfact=1.,extras={}):
    """Build a new run_card.dat from an existing one.
    Extras is a dictionary of keys (no spaces needed) and
    values to replace as well.
    """
    # Grab the old run card and move it into place
    runcard = subprocess.Popen(['get_files','-data',run_card_old])
    runcard.wait()
    if not os.access(run_card_old,os.R_OK):
        mglog.info('Could not get run card '+run_card_old)
    if os.access(run_card_new,os.R_OK):
        mglog.error('Old run card in the current directory. Dont want to clobber it. Please move it first.')
        return -1
    oldcard = open(run_card_old,'r')
    newcard = open(run_card_new,'w')
    for line in oldcard:
        if '= xqcut ' in line:
            newcard.write('%f   = xqcut   ! minimum kt jet measure between partons \n'%(xqcut))
        elif ' nevents ' in line:
            newcard.write('  %i       = nevents ! Number of unweighted events requested \n'%(nevts))
        elif ' iseed ' in line:
            newcard.write('   %i      = iseed   ! rnd seed (0=assigned automatically=default)) \n'%(rand_seed))
        elif ' ebeam1 ' in line:
            newcard.write('   %i      = ebeam1  ! beam 1 energy in GeV \n'%(int(beamEnergy)))
        elif ' ebeam2 ' in line:
            newcard.write('   %i      = ebeam2  ! beam 2 energy in GeV \n'%(int(beamEnergy)))
        elif ' scalefact  ' in line:
            newcard.write(' %3.2f     = scalefact        ! scale factor for event-by-event scales \n'%(scalefact))
        elif ' alpsfact  ' in line:
            newcard.write(' %3.2f     = alpsfact         ! scale factor for QCD emission vx \n'%(alpsfact))
        else:
            for ak in extras:
                excludeList=['xqcut','nevents','iseed','ebeam1','ebeam2','scalefact','alpsfact']
                if ak in excludeList:
                    mglog.error('You are trying to set "%s" with the extras parameter in build_run_card, this must be set in the build_run_card arguments instead.'%ak)
                    raise RuntimeError('You are trying to set "%s" with the extras parameter in build_run_card, this must be set in the build_run_card arguments instead.'%ak)
                    return -1

                #if '='+ak.strip() in line.replace(' ',''):
                if '='+ak.strip() in line.replace(' ','') and (len(line.strip().split(ak.strip())[1])==0 or line.split(ak.strip())[1][0]==" "):
                    newcard.write( ' '+str(extras[ak])+'    ='+'='.join(line.split('=')[1:]) )
                    break
            else: # Fell through the loop
                newcard.write(line)
    oldcard.close()
    newcard.close()
    return run_card_new

# New helper function - this is a bit of duplication with build_run_card but modify_run_card
# is a bit more lightweight. build_run_card could be rewritten to call modify_run_card
# with the explicit arguments included in the dict.
def modify_run_card(run_card='Cards/run_card.dat',
                     run_card_backup='Cards/run_card_backup.dat',
                     settings={}, delete_backup = False):
    mglog.info('Modifying run card located at '+run_card+'.')
    if delete_backup:
        mglog.info('Deleting original run card.')
    else:
        mglog.info('Keeping backup of original run card at '+run_card_backup+'.')
    print(settings)
    if os.path.isfile(run_card_backup): os.unlink(run_card_backup) # delete old backup
    os.rename(run_card, run_card_backup) # change name of original card

    oldCard = open(run_card_backup, 'r')
    newCard = open(run_card, 'w')
    for line in iter(oldCard):
        if not line.strip().startswith('#'): # line commented out
            command = line.split('!', 1)[0]
            comment = line.split('!', 1)[1] if '!' in line else ''
            if '=' in command:
                setting = command.split('=')[-1] #.strip()
                stripped_setting = setting.strip()
                oldValue = command.split('=')[0] #.strip()
            if stripped_setting in settings:
                line = oldValue.replace(oldValue.strip(), str(settings[stripped_setting]))+'='+setting
                if comment != '': line += '  !' + comment
                mglog.info('Setting '+stripped_setting+' = '+str(settings[stripped_setting])+'.')
        newCard.write(line)

    # close files
    oldCard.close()
    newCard.close()
    mglog.info('Finished modification of run card.')
    if delete_backup: os.unlink(run_card_backup)

def print_cards():
    if os.access('proc_card_mg5.dat',os.R_OK):
        mglog.info("proc_card_mg5.dat:")
        procCard = subprocess.Popen(['cat','proc_card_mg5.dat'])
        procCard.wait()
    else:
        mglog.warning('No proc_card_mg5.dat found')

    if os.access('run_card.dat',os.R_OK):
        mglog.info("run_card.dat:")
        runCard = subprocess.Popen(['cat','run_card.dat'])
        runCard.wait()
    else:
        mglog.warning('No run_card.dat found')


def is_gen_from_gridpack(grid_pack=None):
    variables = {}
    if os.access('runargs.generate.py',os.R_OK): 
        execfile('runargs.generate.py',variables) 
    else: 
        execfile('runargs.Generate.py',variables) 

    outputTXTFile = None
    if 'runArgs' in variables and hasattr(variables['runArgs'],'inputGenConfFile') and grid_pack:
        return True

    return False

def is_NLO_run(proc_dir='PROC_mssm_0'):
    isNLO=False
    #proc_card_loc=proc_dir+'/Cards/proc_card_mg5.dat'
    #proccard = open(proc_card_loc,'r')
    #for line in proccard:
    #    #This is probably better but needs checking:
    #    #m = re.search('\[.*QCD.*\]', name)
    #    #if m and m.group():       
    #    if 'generate' in line and ('[QCD]' in line or '[real=QCD]' in line):
    #        mglog.info('Found NLO generation from this line in proc_card.dat: %s'%(line.strip()))
    #        isNLO=True
    #        break
    #proccard.close()


    lo_config_card_loc=proc_dir+'/Cards/me5_configuration.txt'
    nlo_config_card_loc=proc_dir+'/Cards/amcatnlo_configuration.txt'

    if os.access(lo_config_card_loc,os.R_OK) and not os.access(nlo_config_card_loc,os.R_OK):     
        isNLO=False
    elif os.access(nlo_config_card_loc,os.R_OK) and not os.access(lo_config_card_loc,os.R_OK):     
        isNLO=True
    else:
        mglog.error("Neither configuration card found. Unable to determine LO or NLO process!")
        RuntimeError('escaping')

    return isNLO

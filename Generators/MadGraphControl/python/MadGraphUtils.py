#! /usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Pythonized version of MadGraph steering executables
#    written by Zach Marshall <zach.marshall@cern.ch>
#    updates for aMC@NLO by Josh McFayden <mcfayden@cern.ch>
#  Attempts to remove path-dependence of MadGraph

import os,sys,time,subprocess,shutil,glob,re,difflib,stat
from AthenaCommon import Logging
mglog = Logging.logging.getLogger('MadGraphUtils')

MADGRAPH_PDFSETTING=None
from MadGraphUtilsHelpers import *
from MadGraphSystematicsUtils import *

def setup_path_protection():
    # Addition for models directory
    if 'PYTHONPATH' in os.environ:
        if not 'Generators/madgraph/models' in os.environ['PYTHONPATH']:
            os.environ['PYTHONPATH'] += ':/cvmfs/atlas.cern.ch/repo/sw/Generators/madgraph/models/latest'
    # Make sure that gfortran doesn't write to somewhere it shouldn't
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

    import os
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
    generate.communicate()

    mglog.info('Finished process generation at '+str(time.asctime()))

    if ''==thedir:
        directories = os.listdir( os.getcwd() )
        for adir in sorted(directories):
            if 'PROC' in adir: thedir=adir

    if not os.access('%s/SubProcesses/subproc.mg'%thedir,os.R_OK):
        raise RuntimeError('No diagrams for this process in dir='+str(thedir))

    # Special catch related to path setting and using afs
    import os
    needed_options = ['ninja','collier','fastjet','lhapdf','syscalc_path']
    in_config = open(os.environ['MADPATH']+'/input/mg5_configuration.txt','r')
    option_paths = {}
    for l in in_config.readlines():
        for o in needed_options:
            if o+' =' in l.split('#')[0]:
                old_path = l.split('#')[0].split('=')[1].strip().split('MCGenerators')[1]
                old_path = old_path[ old_path.find('/') : ]
                if o =='lhapdf' and 'LHAPATH' in os.environ:
                    # Patch for LHAPDF version
                    version = os.environ['LHAPATH'].split('lhapdf/')[1].split('/')[0]
                    old_version = old_path.split('lhapdf/')[1].split('/')[0]
                    old_path = old_path.replace(old_version,version)
                if o=='ninja':
                    # Patch for stupid naming problem
                    old_path.replace('gosam_contrib','gosam-contrib')
                option_paths[o] = os.environ['MADPATH'].split('madgraph5amc')[0]+old_path
    in_config.close()
    for o in needed_options:
        if not o in option_paths: mglog.warning('Path for option '+o+' not found in original config')

    mglog.info('Modifying config paths to avoid use of afs:')
    mglog.info(option_paths)

    # Get the original configuration information
    if not is_NLO_run(proc_dir=thedir):
        config_card=thedir+'/Cards/me5_configuration.txt'
    else:
        config_card=thedir+'/Cards/amcatnlo_configuration.txt'
    with file(config_card, 'r') as original: data = original.readlines()
    with file(config_card, 'w') as modified:
        for l in data:
            written = False
            for o in needed_options:
                if o+' =' in l.split('#')[0]:
                    modified.write( o +' = '+option_paths[o]+'\n' )
                    written = True
                    break
            if not written:
                modified.write(l)
    # Done modifying paths

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
    if os.access(run_card_loc,os.R_OK):
        mglog.info('Copying default run_card.dat from '+str(run_card_loc))
        shutil.copy(run_card_loc,'run_card.tmp.dat')
        return 'run_card.tmp.dat'
    else:
        run_card_loc=proc_dir+'/Cards/run_card_default.dat'
        mglog.info('Fetching default run_card.dat from '+str(run_card_loc))
        if os.access(run_card_loc,os.R_OK):
            shutil.copy(run_card_loc,'run_card.tmp.dat')
            return 'run_card.tmp.dat'
        else:
            raise RuntimeError('Cannot find default run_card.dat or run_card_default.dat! I was looking here: %s'%run_card_loc)


def generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir='PROC_mssm_0',grid_pack=False,gridpack_compile=False,cluster_type=None,cluster_queue=None,cluster_nb_retry=None,cluster_temp_path=None,extlhapath=None,madspin_card_loc=None,required_accuracy=0.01,gridpack_dir=None,nevents=None,random_seed=None,reweight_card_loc=None,bias_module=None):
    try:
        from __main__ import opts
        if opts.config_only:
            mglog.info('Athena running on config only mode: not executing MadGraph')
            return
    except:
        pass

    # Just in case
    setup_path_protection()

    # check that mode chosen and number of jobs requested are consistent
    mode_njobs_consistency_check(mode,njobs,cluster_type)

    if is_gen_from_gridpack(grid_pack):
        if gridpack_dir and nevents!=None and random_seed!=None:
            mglog.info('Running event generation from gridpack (using smarter mode from generate() function)')
            generate_from_gridpack(run_name=run_name,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=random_seed,card_check=proc_dir,param_card=param_card_loc,madspin_card=madspin_card_loc,extlhapath=extlhapath,gridpack_compile=gridpack_compile,reweight_card=reweight_card_loc)
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
    if reweight_card_loc is not None and not os.access(reweight_card_loc,os.R_OK):
        mglog.error('Could not find reweight card '+str(reweight_card_loc))
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
                    newcard.write(' 1000 = nevents    ! Number of unweighted events requested \n')
                    mglog.info('Setting nevents = 1000.')
                elif ' = req_acc ' in line:
                    newcard.write(' %f = req_acc    ! Required accuracy (-1=auto determined from nevents)\n'%(req_acc))
                    mglog.info('Setting req_acc = %f'%(req_acc))
                else:
                    newcard.write(line)
            oldcard.close()
            newcard.close()
            shutil.move(run_card_loc+'.tmp',run_card_loc)

        mglog.info( "run_card.dat: "+run_card_loc )
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

    if reweight_card_loc:
        if int(version.split('.')[0]) < 3 and not ( int(version.split('.')[0]) >= 2 and int(version.split('.')[1]) >= 4 ):
            mglog.error('Attempting to run reweighting with old MadGraph version - please upgrade to MadGraph5_aMC@NLO v2.4+.')
            return 1
        mglog.info('Running reweighting module. Moving card (%s) into place.'%(reweight_card_loc))
        shutil.copyfile(reweight_card_loc,proc_dir+'/Cards/reweight_card.dat')


    # Ensure that things are set up normally
    if not os.access(run_card_loc,os.R_OK):
        mglog.error('No run card found at '+run_card_loc)
        return 1
    if param_card_loc is not None and not os.access(param_card_loc,os.R_OK):
        mglog.error('No param card found at '+param_card_loc)
        return 1
    if reweight_card_loc is not None and not os.access(reweight_card_loc,os.R_OK):
        mglog.error('No reweight card found at '+reweight_card_loc)
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

    allow_links = True
    if cluster_type is not None:
        if 'condor' in cluster_type.lower():
            mglog.warning('Condor clusters do not allow links.  Will do more copying rather than linking')
            allow_links = False

    (LHAPATH,origLHAPATH,origLHAPDF_DATA_PATH) = setupLHAPDF(isNLO, version=version, proc_dir=proc_dir, extlhapath=extlhapath, allow_links=allow_links)


    mglog.info('For your information, the libraries available are (should include LHAPDF):')
    mglog.info( sorted( os.listdir( proc_dir+'/lib/' ) ) )

    setupFastjet(isNLO, proc_dir=proc_dir)
    if bias_module!=None:
        setup_bias_module(bias_module,run_card_loc,proc_dir,grid_pack)


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

    print_cards(run_card=run_card_loc,param_card=(param_card_loc if param_card_loc is not None else proc_dir+'/Cards/param_card.dat'))

    currdir=os.getcwd()
    os.chdir(proc_dir)

    athenaMP = False
    if 'ATHENA_PROC_NUMBER' in os.environ:
        njobs = int(os.environ['ATHENA_PROC_NUMBER'])
        mglog.info('Lucky you - you are running on a full node queue.  Will re-configure for '+str(njobs)+' jobs.')
        mode = 2
        athenaMP = True
    if mode!=0 and not isNLO:

        if not grid_pack and not athenaMP:
            mglog.warning('Running parallel generation.  This should not happen for a grid job, to be a good citizen.')
        elif not athenaMP:
            mglog.info('Running parallel gridpack generation.  We already told you to not do this in a grid job...')


        if mode==1:
            mglog.info('Setting up parallel running system settings')

            setNCores(process_dir=os.getcwd(), Ncores=njobs)

            config_card='Cards/me5_configuration.txt'
            oldcard = open(config_card,'r')
            newcard = open(config_card+'.tmp','w')

            for line in oldcard:
                if cluster_type!=None and 'cluster_type = ' in line:
                    newcard.write('cluster_type = %s \n'%(cluster_type))
                    mglog.info('Setting cluster type = %s in %s'%(cluster_type,config_card))
                elif cluster_queue!=None and 'cluster_queue = ' in line:
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

        run_card_consistency_check(isNLO=isNLO)
        generate = subprocess.Popen(['bin/generate_events',str(mode),str(njobs),str(run_name)],stdin=subprocess.PIPE)
        generate.communicate()

    elif not isNLO:

        setNCores(process_dir=os.getcwd(), Ncores=njobs)
        run_card_consistency_check(isNLO=isNLO)
        mglog.info('Running serial generation.  This will take a bit more time than parallel generation.')
        generate = subprocess.Popen(['bin/generate_events','0',str(run_name)],stdin=subprocess.PIPE)
        generate.communicate()

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
        cluster_nb_retry_set=False

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
            elif 'cluster_type = ' in line and mode == 1 and cluster_type:
                if not cltype_set:
                    mglog.info('Setting cluster type = %s in %s'%(cluster_type,config_card_loc))
                    newcard.write('cluster_type = %s \n'%(cluster_type))
                    cltype_set=True
            elif 'cluster_queue = ' in line and mode == 1 and cluster_queue:
                if not clqueue_set:
                    mglog.info('Setting cluster queue = %s in %s'%(cluster_queue,config_card_loc))
                    newcard.write('cluster_queue = %s \n'%(cluster_queue))
                    clqueue_set=True
            elif 'cluster_nb_retry = ' in line and mode == 1 and cluster_nb_retry:
                if not cluster_nb_retry_set:
                    mglog.info('Setting cluster no. retries = %i in %s'%(cluster_nb_retry,config_card_loc))
                    newcard.write('cluster_nb_retry = %i \n'%(cluster_nb_retry))
                    cluster_nb_retry_set=True
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

        mglog.info( "amcatnlo_configuration.txt: "+config_card_loc )
        configCard = subprocess.Popen(['cat',config_card_loc])
        configCard.wait()


        mglog.info('Removing Cards/shower_card.dat to ensure we get parton level events only')
        remove_shower = subprocess.Popen(['rm','Cards/shower_card.dat'])
        remove_shower.wait()


        run_card_consistency_check(isNLO=isNLO)
        mygenerate = subprocess.Popen(['bin/generate_events','--name='+str(run_name)],stdin=subprocess.PIPE, stderr=subprocess.STDOUT)
        mygenerate.communicate()



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

            run_card_consistency_check(isNLO=isNLO)

            ### NLO RUN ###


            gridpack_name=(run_name+'_gridpack.tar.gz')
            mglog.info('Creating gridpack (%s)...'%gridpack_name)

            os.chdir('../')
            mglog.info('Package up proc_dir')
            os.rename(proc_dir,gridpack_dir)
            tar = subprocess.Popen(['tar','czf',gridpack_name,gridpack_dir,'--exclude=lib/PDFsets'])
            tar.wait()
            os.rename(gridpack_dir,proc_dir)


        raise RuntimeError('Gridpack sucessfully created, exiting the transform. IGNORE ERRORS if running gridpack generation!')

    os.chdir(currdir)

    resetLHAPDF(origLHAPATH=origLHAPATH,origLHAPDF_DATA_PATH=origLHAPDF_DATA_PATH)

    mglog.info('Finished at '+str(time.asctime()))
    return 0


def generate_from_gridpack(run_name='Test',gridpack_dir='madevent/',nevents=-1,random_seed=-1,card_check=None,param_card=None,madspin_card=None,reweight_card=None,extlhapath=None, gridpack_compile=None):

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

    if reweight_card is not None:
        if os.path.exists(reweight_card):
            shutil.copy( reweight_card , gridpack_dir+'/Cards/reweight_card.dat' )
            mglog.info( 'Moved reweight card into place: '+str(reweight_card) )
        else:
            mglog.info( 'Did not find reweight card '+str(reweight_card)+', using the one provided by gridpack' )


    print_cards(run_card=gridpack_dir+'/Cards/run_card.dat',param_card=gridpack_dir+'/Cards/param_card.dat')


    mglog.info('Generating events from gridpack')

    # Ensure that things are set up normally
    if not os.path.exists(gridpack_dir):
        raise RuntimeError('Gridpack directory not found at '+gridpack_dir)


    mglog.info('>>>> FOUND GRIDPACK <<<<  <- This will be used for generation')
    mglog.info('Generation of '+str(int(nevents))+' events will be performed using the supplied gridpack with random seed '+str(random_seed))
    mglog.info('Started generating events at '+str(time.asctime()))



    #Remove addmasses if it's there
    if os.access(gridpack_dir+'/bin/internal/addmasses.py',os.R_OK):
        os.remove(gridpack_dir+'/bin/internal/addmasses.py')

    currdir=os.getcwd()

    if not isNLO:
        ### LO RUN ###
        if not os.access(gridpack_dir+'/bin/run.sh',os.R_OK):
            mglog.error('/bin/run.sh not found at '+gridpack_dir)
            return 1
        else:
            mglog.info('Found '+gridpack_dir+' bin/run.sh, starting generation.')
        # hack script to add reweighting and systematics, if required
        hack_gridpack_script(gridpack_dir,reweight_card,madspin_card)

        setNCores(process_dir=gridpack_dir)

        mglog.info('For your information, ls of '+currdir+':')
        mglog.info( sorted( os.listdir( currdir ) ) )
        mglog.info('For your information, ls of '+gridpack_dir+':')
        mglog.info( sorted( os.listdir( gridpack_dir ) ) )

        run_card_consistency_check(isNLO=isNLO,path=gridpack_dir)
        generate = subprocess.Popen([gridpack_dir+'/bin/run.sh',str(int(nevents)),str(int(random_seed))],stdin=subprocess.PIPE)
        generate.communicate()

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

        mglog.info( "run_card.dat: "+run_card_loc )
        runCard = subprocess.Popen(['cat',run_card_loc])
        runCard.wait()

        ### Editing config card
        config_card_loc=gridpack_dir+'/Cards/amcatnlo_configuration.txt'

        # Make sure params only set once
        cltype_set=False
        clqueue_set=False

        setNCores(process_dir=gridpack_dir)

        mglog.info( "amcatnlo_configuration.txt: "+config_card_loc )
        configCard = subprocess.Popen(['cat',config_card_loc])
        configCard.wait()

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


        if not gridpack_compile:
            mglog.info('Copying make_opts from Template')
            shutil.copy(os.environ['MADPATH']+'/Template/LO/Source/make_opts',gridpack_dir+'/Source/')

            run_card_consistency_check(isNLO=isNLO,path=gridpack_dir)
            generate = subprocess.Popen([gridpack_dir+'/bin/generate_events','--parton','--nocompile','--only_generation','-f','--name=%s'%run_name],stdin=subprocess.PIPE)
            generate.communicate()
        else:
            mglog.info('Allowing recompilation of gridpack')
            if os.path.islink(gridpack_dir+'/lib/libLHAPDF.a'):
                mglog.info('Unlinking '+gridpack_dir+'/lib/libLHAPDF.a')
                os.unlink(gridpack_dir+'/lib/libLHAPDF.a')

            run_card_consistency_check(isNLO=isNLO,path=gridpack_dir)
            generate = subprocess.Popen([gridpack_dir+'/bin/generate_events','--parton','--only_generation','-f','--name=%s'%run_name],stdin=subprocess.PIPE)
            generate.communicate()


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

    return

def get_LHAPDF_DATA_PATH():
    LHAPATH=os.environ['LHAPATH'].split(':')[0]
    if len(os.environ['LHAPATH'].split(':')) >=2 :
        LHADATAPATH=os.environ['LHAPATH'].split(':')[1]
    else:
        LHADATAPATH=os.environ['LHAPATH'].split(':')[0]
    return LHADATAPATH

# function to get lhapdf id and name from either id or name
def get_lhapdf_id_and_name(pdf):
    pdfname=''
    pdfid=-999
    LHADATAPATH=get_LHAPDF_DATA_PATH()
    pdflist = open(LHADATAPATH+'/pdfsets.index','r')
    if isinstance(pdf,int) or pdf.isdigit():
        pdf=int(pdf)
        pdfid=pdf
        for line in pdflist:
            splitline=line.split()
            if int(splitline[0]) == pdfid:
                pdfname=splitline[1]
                break
    else:
        pdfname=pdf
        for line in pdflist:
            splitline=line.split()
            if splitline[1] == pdfname:
                pdfid=int(splitline[0])
                break
    pdflist.close()

    if pdfname=='':
        err='Couldn\'t find PDF name associated to ID %i in %s.'%(pdfid,LHADATAPATH+'/pdfsets.index')
        mglog.error(err)
        raise RuntimeError(err)
    if pdfid<0:
        err='Couldn\'t find PDF ID associated to name %s in %s.'%(pdfname,LHADATAPATH+'/pdfsets.index')
        mglog.error(err)
        raise RuntimeError(err)

    return pdfid,pdfname

def setupLHAPDF(isNLO, version=None, proc_dir=None, extlhapath=None, allow_links=True):

    origLHAPATH=os.environ['LHAPATH']
    origLHAPDF_DATA_PATH=os.environ['LHAPDF_DATA_PATH']


    LHAPATH=os.environ['LHAPATH'].split(':')[0]
    if len(os.environ['LHAPATH'].split(':')) >=2 :
        LHADATAPATH=os.environ['LHAPATH'].split(':')[1]
    else:
        LHADATAPATH=os.environ['LHAPATH'].split(':')[0]

    pdfname=''
    pdfid=-999

    ### Reading LHAPDF ID from run card
    run_card_loc=proc_dir+'/Cards/run_card.dat'
    mydict=getDictFromCard(run_card_loc)

    if mydict["pdlabel"].replace("'","") == 'lhapdf':
        #Make local LHAPDF dir
        mglog.info('creating local LHAPDF dir: MGC_LHAPDF/')
        if os.path.islink('MGC_LHAPDF/'):
            os.unlink('MGC_LHAPDF/')
        elif os.path.isdir('MGC_LHAPDF/'):
            shutil.rmtree('MGC_LHAPDF/')

        newMGCLHA='MGC_LHAPDF/'

        mkdir = subprocess.Popen(['mkdir','-p',newMGCLHA])
        mkdir.wait()

        pdfs_used=[ int(x) for x in mydict['lhaid'].replace(' ',',').split(',') ]
        # included systematics pdfs here
        if 'sys_pdf' in mydict:
            sys_pdf=mydict['sys_pdf'].replace('&&',' ').split()
            for s in sys_pdf:
                if s.isdigit():
                    idx=int(s)
                    if idx>1000: # the sys_pdf syntax is such that small numbers are used to specify the subpdf index
                        pdfs_used.append(idx)
                else:
                    pdfs_used.append(s)
        if 'systematics_arguments' in mydict:
            systematics_arguments=parse_systematics_arguments(mydict['systematics_arguments'])
            if 'pdf' in systematics_arguments:
                sys_pdf=systematics_arguments['pdf'].replace(',',' ').replace('@',' ').split()
                for s in sys_pdf:
                    if s.isdigit():
                        idx=int(s)
                        if idx>1000: # the sys_pdf syntax is such that small numbers are used to specify the subpdf index
                            pdfs_used.append(idx)
                    else:
                        pdfs_used.append(s)
        for pdf in pdfs_used:
            if isinstance(pdf,str) and (pdf.lower()=='errorset' or pdf.lower()=='central'):
                continue
            # new function to get both lhapdf id and name
            pdfid,pdfname=get_lhapdf_id_and_name(pdf)
            mglog.info("Found LHAPDF ID=%i, name=%s!"%(pdfid,pdfname))

            if not os.path.exists(newMGCLHA+pdfname) and not os.path.lexists(newMGCLHA+pdfname):
                if not os.path.exists(LHADATAPATH+'/'+pdfname):
                    mglog.warning('PDF not installed at '+LHADATAPATH+'/'+pdfname)
                if allow_links:
                    mglog.info('linking '+LHADATAPATH+'/'+pdfname+' --> '+newMGCLHA+pdfname)
                    os.symlink(LHADATAPATH+'/'+pdfname,newMGCLHA+pdfname)
                else:
                    mglog.info('copying '+LHADATAPATH+'/'+pdfname+' --> '+newMGCLHA+pdfname)
                    shutil.copytree(LHADATAPATH+'/'+pdfname,newMGCLHA+pdfname)

        if allow_links:
            mglog.info('linking '+LHADATAPATH+'/pdfsets.index --> '+newMGCLHA+'pdfsets.index')
            os.symlink(LHADATAPATH+'/pdfsets.index',newMGCLHA+'pdfsets.index')

            atlasLHADATAPATH=LHADATAPATH.replace('sft.cern.ch/lcg/external/lhapdfsets/current','atlas.cern.ch/repo/sw/Generators/lhapdfsets/current')
            mglog.info('linking '+atlasLHADATAPATH+'/lhapdf.conf --> '+newMGCLHA+'lhapdf.conf')
            os.symlink(atlasLHADATAPATH+'/lhapdf.conf',newMGCLHA+'lhapdf.conf')
        else:
            mglog.info('copying '+LHADATAPATH+'/pdfsets.index --> '+newMGCLHA+'pdfsets.index')
            shutil.copy2(LHADATAPATH+'/pdfsets.index',newMGCLHA+'pdfsets.index')

            atlasLHADATAPATH=LHADATAPATH.replace('sft.cern.ch/lcg/external/lhapdfsets/current','atlas.cern.ch/repo/sw/Generators/lhapdfsets/current')
            mglog.info('copying '+atlasLHADATAPATH+'/lhapdf.conf -->'+newMGCLHA+'lhapdf.conf')
            shutil.copy2(atlasLHADATAPATH+'/lhapdf.conf',newMGCLHA+'lhapdf.conf')


        LHADATAPATH=os.getcwd()+'/MGC_LHAPDF'

    else:
        mglog.info('Not using LHAPDF')
        return (LHAPATH,origLHAPATH,origLHAPDF_DATA_PATH)


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

        mglog.info('Creating links for LHAPDF')
        if os.path.islink(proc_dir+'/lib/PDFsets'):
            os.unlink(proc_dir+'/lib/PDFsets')
        elif os.path.isdir(proc_dir+'/lib/PDFsets'):
            shutil.rmtree(proc_dir+'/lib/PDFsets')
        if allow_links:
            os.symlink(LHADATAPATH,proc_dir+'/lib/PDFsets')
        else:
            shutil.copytree(LHADATAPATH,proc_dir+'/lib/PDFsets')
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
        if allow_links:
            os.symlink(LHAPATH,proc_dir+'/lib/PDFsets')
        else:
            shutil.copytree(LHAPATH,proc_dir+'/lib/PDFsets')
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

        if allow_links:
            os.symlink( lhalibpath+'libLHAPDF.a',proc_dir+'/lib/libLHAPDF.a')
            os.symlink( lhalibpath+'libLHAPDF.so',proc_dir+'/lib/libLHAPDF.so')
        else:
            shutil.copy2( lhalibpath+'libLHAPDF.a',proc_dir+'/lib/libLHAPDF.a')
            shutil.copy2( lhalibpath+'libLHAPDF.so',proc_dir+'/lib/libLHAPDF.so')

    return (LHAPATH,origLHAPATH,origLHAPDF_DATA_PATH)

# Function to set the number of cores and the running mode in the run card
def setNCores(process_dir=None, Ncores=None):
    if process_dir is None:
        mglog.warning('Cannot setNCores because no process dir was provided')
        return
    my_Ncores = Ncores
    if Ncores is None and 'ATHENA_PROC_NUMBER' in os.environ:
        my_Ncores = int(os.environ['ATHENA_PROC_NUMBER'])
    if my_Ncores is None:
        mglog.info('Setting up for serial run')
        my_Ncores = 1

    if not is_NLO_run(proc_dir=process_dir):
        config_card=process_dir+'/Cards/me5_configuration.txt'
    else:
        config_card=process_dir+'/Cards/amcatnlo_configuration.txt'

    import fileinput
    # core configuration with the one we need
    oldcard = open(config_card,'r')
    newcard = open(config_card+'.tmp','w')
    for line in oldcard.readlines():
        if 'nb_core = ' in line:
            mglog.info('Setting number of cores = %i in %s'%(my_Ncores,config_card))
            newcard.write('nb_core = %i \n'%(my_Ncores))
        elif 'run_mode = ' in line:
            mglog.info('Setting run mode = %i in %s'%(0 if my_Ncores==1 else 2,config_card))
            newcard.write('run_mode = %i \n'%(0 if my_Ncores==1 else 2))
        elif 'automatic_html_opening =' in line:
            mglog.info('Setting automatic_html_opening = %s'%('False'))
            newcard.write('automatic_html_opening = %s \n'%('False'))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()
    shutil.move(config_card+'.tmp',config_card)


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
    generate.communicate()

    mglog.info('Finished adding time of flight information at '+str(time.asctime()))

    # Put the file back where we found it
    lhe = glob(process_dir+'/Events/*/*lhe')[0]
    rezip = subprocess.Popen(['gzip',lhe])
    rezip.wait()

    return True

def run_madspin(madspin_card=None,runArgs=None,saveProcDir=False,run_name='Test'):
    """ Run madspin on the generated LHE file.  Should be
    run when you have inputGeneratorFile set.
    """
    try:
        from __main__ import opts
        if opts.config_only:
            mglog.info('Athena running on config only mode: not executing MadGraph')
            return
    except:
        pass
    madpath=os.environ['MADPATH']
    process_dir=new_process(SUSY_model()+"""
generate p p > go go
output -f"""
        )

    tarball = runArgs.inputGeneratorFile
    from glob import glob
    if not os.path.exists(os.getcwd()+'/'+tarball):
        mglog.error(' %s does not contain events?'%(os.getcwd()))
    else:
        mglog.info('Unzipping generated events.')
        unzip = subprocess.Popen(['gunzip','-f',os.getcwd()+'/'+tarball])
        unzip.wait()
        mglog.info('Moving file over to '+process_dir+'/Events/'+run_name+'/unweighted_events.lhe')
        mkdir = subprocess.Popen(['mkdir','-p',(process_dir+'/Events/')])
        mkdir.wait()
        mkdir = subprocess.Popen(['mkdir','-p',(process_dir+'/Events/'+run_name)])
        mkdir.wait()
        shutil.move(os.getcwd()+'/'+tarball.replace('tar.gz','events'),process_dir+'/Events/'+run_name+'/unweighted_events.lhe')
    if len(glob(process_dir+'/Events/*'))<1:
        mglog.error('Process dir %s does not contain events?'%process_dir)
    run = glob(process_dir+'/Events/*')[0].split('/')[-1]
    from glob import glob
    me_exec=process_dir+'/bin/madevent'
    shutil.copyfile(madspin_card,process_dir+'/Cards/madspin_card.dat')

    ms_c = open('madspin_decay_cmd','w')
    ms_c.write('decay_events '+run_name)
    ms_c.close()

    mglog.info('Started madspin at '+str(time.asctime()))

    generate = subprocess.Popen([me_exec,'madspin_decay_cmd'],stdin=subprocess.PIPE)
    generate.communicate()

    mglog.info('Finished running madspin at '+str(time.asctime()))
    shutil.move(process_dir+'/Events/'+run_name+'_decayed_1/unweighted_events.lhe.gz',process_dir+'/Events/'+run_name+'/events.lhe.gz')
    mglog.info('Moving MadSpin events from %s to %s.'%(process_dir+'/Events/'+run_name+'_decayed_1/unweighted_events.lhe.gz',process_dir+'/Events/'+run_name+'/events.lhe.gz'))


    # Move output files into the appropriate place, with the appropriate name
    the_spot = arrange_output(run_name=run_name,proc_dir=process_dir,outputDS='madgraph_OTF._00001.events.tar.gz',saveProcDir=saveProcDir)
    if the_spot == '':
        mglog.error('Error arranging output dataset!')
        return -1

    mglog.info('All done generating events!!')

    return


def arrange_output(run_name='Test',proc_dir='PROC_mssm_0',outputDS='madgraph_OTF._00001.events.tar.gz',lhe_version=None,saveProcDir=False,runArgs=None,madspin_card_loc=None):
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


    hasRunMadSpin=False
    madspinDirs=sorted(glob.glob(proc_dir+'/Events/'+run_name+'_decayed_*/'))
    if len(madspinDirs): hasRunMadSpin=True

    if madspin_card_loc or hasRunMadSpin:
        if len(madspinDirs):
            if hasUnweighted:
                # so this is a bit of a mess now...
                # if madspin is run from an NLO grid pack the correct lhe events are at both
                #      madevent/Events/run_01/unweighted_events.lhe.gz
                # and  madevent/Events/run_01_decayed_1/events.lhe.gz
                # so there are unweighted events but not in the madspinDir...
                if os.path.exists(madspinDirs[-1]+'/unweighted_events.lhe.gz'):
                    shutil.move(madspinDirs[-1]+'/unweighted_events.lhe.gz',proc_dir+'/Events/'+run_name+'/unweighted_events.lhe.gz')
                    mglog.info('Moving MadSpin events from %s to %s.'%(madspinDirs[-1]+'/unweighted_events.lhe.gz',proc_dir+'/Events/'+run_name+'/unweighted_events.lhe.gz'))
                elif os.path.exists(madspinDirs[-1]+'/events.lhe.gz'):
                    shutil.move(madspinDirs[-1]+'/events.lhe.gz',proc_dir+'/Events/'+run_name+'/unweighted_events.lhe.gz')
                    mglog.info('Moving MadSpin events from %s to %s.'%(madspinDirs[-1]+'/events.lhe.gz',proc_dir+'/Events/'+run_name+'/unweighted_events.lhe.gz'))
                else:
                    raise RuntimeError('MadSpin was run but can\'t find files :(')


            else:
                shutil.move(madspinDirs[-1]+'/events.lhe.gz',proc_dir+'/Events/'+run_name+'/events.lhe.gz')
                mglog.info('Moving MadSpin events from %s to %s.'%(madspinDirs[-1]+'/events.lhe.gz',proc_dir+'/Events/'+run_name+'/events.lhe.gz'))

        else:
            mglog.error('MadSpin was run but can\'t find output folder %s.'%(proc_dir+'/Events/'+run_name+'_decayed_1/'))
            raise RuntimeError('MadSpin was run but can\'t find output folder %s.'%(proc_dir+'/Events/'+run_name+'_decayed_1/'))



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
        orig_input = proc_dir+'/Events/'+run_name+'/unweighted_events.lhe'
        mod_output = open(os.getcwd()+'/events.lhe','w')
    else:
        orig_input = proc_dir+'/Events/'+run_name+'/events.lhe'
        mod_output = open(os.getcwd()+'/events.lhe','w')


    #Removing empty lines in LHE
    nEmpty=0
    # Try to do this in a way that uses less memory...
    # Note that I could do this instead with unix commands:
    #grep -cvP '\S' orig_input
    #sed -i '/^$/d' orig_input
    with open(orig_input,'r') as fileobject:
        for line in fileobject:
            if line.strip():
                mod_output.write(line)
            else:
                nEmpty=nEmpty+1
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
    outputTXTFile = None
    if runArgs is None:
        # If they didn't pass in runArgs, then we have to do this by hand
        variables = {}
        if os.access('runargs.generate.py',os.R_OK):
            execfile('runargs.generate.py',variables)
        elif os.access('runargs.Generate.py',os.R_OK):
            execfile('runargs.Generate.py',variables)

        if 'runArgs' in variables and hasattr(variables['runArgs'],'outputTXTFile'):
            outputTXTFile = variables['runArgs'].outputTXTFile
    else:
        if hasattr(runArgs,'outputTXTFile'):
            outputTXTFile = runArgs.outputTXTFile

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

        if os.path.isdir('MGC_LHAPDF/'):
            shutil.rmtree('MGC_LHAPDF/',ignore_errors=True)

    # shortening the outputDS in the case of an output TXT file
    if outputTXTFile is not None:
        outputDS = outputDS.split('.TXT')[0]
    # Do some fixing up for them
    if runArgs is not None:
        mglog.debug('Setting inputGenerator file to '+outputDS)
        runArgs.inputGeneratorFile=outputDS

    mglog.info('All done with output arranging!')
    return outputDS

def setup_bias_module(bias_module,run_card_loc,proc_dir,grid_pack):
    version = getMadGraphVersion()
    if int(version.split('.')[0])<2 or int(version.split('.')[0])==2 and int(version.split('.')[1])<5:
        mglog.error('Sorry, the bias module is available only from MG5_aMC v2.5.0')
        return 1
    if grid_pack and (int(version.split('.')[0])<2 or int(version.split('.')[0])==2 and int(version.split('.')[1])<6):
        mglog.error('Sorry, the bias module works with gridpacks only from MG5_aMC v2.6.0')
        return 1
    if isinstance(bias_module,tuple):
        mglog.info('Using bias module '+bias_module[0])
        the_run_card = open(run_card_loc,'r')
        for line in the_run_card:
            if 'bias_module' in line and not bias_module[0] in line:
                mglog.error('You need to add the bias module '+bias_module[0]+' to the run card to actually run it')
                return 1
        the_run_card.close()
        if len(bias_module)!=3:
            mglog.error('Please give a 3-tuple of strings containing bias module name, bias module, and makefile. Alternatively, give path to bias module tarball.')
            return 1
        bias_module_newpath=proc_dir+'/Source/BIAS/'+bias_module[0]
        os.makedirs(bias_module_newpath)
        bias_module_file=open(bias_module_newpath+'/'+bias_module[0]+'.f','w')
        bias_module_file.write(bias_module[1])
        bias_module_file.close()
        bias_module_make_file=open(bias_module_newpath+'/Makefile','w')
        bias_module_make_file.write(bias_module[2])
        bias_module_make_file.close()
    else:
        mglog.info('Using bias module '+bias_module)
        bias_module_name=bias_module.split('/')[-1].replace('.gz','')
        bias_module_name=bias_module_name.replace('.tar','')
        the_run_card = open(run_card_loc,'r')
        for line in the_run_card:
            if 'bias_module' in line and not bias_module_name in line:
                mglog.error('You need to add the bias module '+bias_module_name+' to the run card to actually run it')
                return 1
        the_run_card.close()

        if os.path.exists(bias_module+'.tar.gz'):
            bias_module_path=bias_module+'.tar.gz'
        elif os.path.exists(bias_module+'.gz'):
            bias_module_path=bias_module+'.gz'
        elif os.path.exists(bias_module):
            bias_module_path=bias_module
        else:
            mglog.error('Did not find bias module '+bias_module+' , this path should point to folder or tarball.  Alternatively give a tuple of strings containing module name, module, and makefile')
            return 1
        bias_module_newpath=proc_dir+'/Source/BIAS/'+bias_module_path.split('/')[-1]
        mglog.info('Copying bias module into place: '+bias_module_newpath)
        shutil.copy(bias_module_path,bias_module_newpath)
        mglog.info('Unpacking bias module')
        if bias_module_newpath.endswith('.tar.gz'):
            untar = subprocess.Popen(['tar','xvzf',bias_module_newpath,'--directory='+proc_dir+'/Source/BIAS/'])
            untar.wait()
        elif bias_module_path.endswith('.gz'):
            gunzip = subprocess.Popen(['gunzip',bias_module_newpath])
            gunzip.wait()


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
    header = SUSY_model()
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
        my_mass=2000 # default
        if 'Scharm'==gentype:
            my_mass = min([masses[x] for x in ['1000004','2000004'] if x in masses ])
        elif 'N2N3'==gentype:
            my_mass = min([masses[x] for x in ['1000023','1000025'] if x in masses ])
        elif 'N1N1'==gentype:
            my_mass = min([masses[x] for x in ['1000022'] if x in masses ])
        elif 'C1N2'==gentype:
            my_mass = min([masses[x] for x in ['1000023','1000024'] if x in masses ])
        elif 'Stau'==gentype:
            my_mass = min([masses[x] for x in ['1000015','2000015'] if x in masses ])
        elif 'StauStau'==gentype:
            my_mass = min([masses[x] for x in ['1000015','2000015'] if x in masses ])
        elif 'SlepSlep'==gentype:
            my_mass = min([masses[x] for x in ['1000011','1000013','1000015','2000011','2000013','2000015'] if x in masses ])
        elif 'T2' in gentype:
            my_mass = min([masses[x] for x in ['2000006'] if x in masses ])
        else:
            if 'G' in gentype or 'ALL' in gentype:
                my_mass = masses['1000021']
            if 'S' in gentype or 'ALL' in gentype:
                my_mass = min([masses[x] for x in ['1000001','1000002','1000003','1000004','2000001','2000002','2000003','2000004'] if x in masses])
            if 'T' in gentype:
                my_mass = masses['1000006']
            if 'B' in gentype:
                my_mass = masses['1000005']
            if 'C' in gentype:
                my_mass = masses['1000024']
            if 'D' in gentype:
                my_mass = masses['2000001']
        xqcut = min(my_mass*0.25,500)
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

    return abs(xqcut) , alpsfact , scalefact


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

    # Ensure that the param card is compatible with the model that's being used
    shutil.move('param_card.dat','original_param_card.dat')
    update_param_card_blocks( process_dir=thedir , from_param_card='original_param_card.dat' , to_param_card='param_card.dat' )

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
    the_spot = arrange_output(run_name='Test',proc_dir=thedir,outputDS='madgraph_OTF._00001.events.tar.gz',saveProcDir=keepOutput,runArgs=runArgs)
    if the_spot == '':
        mglog.error('Error arranging output dataset!')
        return -1

    mglog.info('All done generating events!!')
    return [xqcut,the_spot]


def SUSY_SM_Generation(runArgs = None, process='', gentype='SS',decaytype='direct',masses=None,\
                       nevts=None, syst_mod=None,xqcut=None, SLHAonly=False, keepOutput=False, SLHAexactCopy=False,\
                       writeGridpack=False,gridpackDirName=None,MSSMCalc=False,pdlabel="'cteq6l1'",lhaid=10042,\
                       madspin_card=None,decays={},extras=None,paramCardPrefix='param_card.SM'):
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

    thedir = gridpackDirName
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
            full_proc = SUSY_model()+helpful_definitions()+"""
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
            build_param_card(param_card_old='%s.%s.%s.dat'%(paramCardPrefix,gentype,decaytype),param_card_new='param_card.dat',masses=masses,decays=decays)
    # Ensure that the param card is compatible with the model that's being used
    shutil.move('param_card.dat','original_param_card.dat')
    update_param_card_blocks( process_dir=thedir , from_param_card='original_param_card.dat' , to_param_card='param_card.dat' )

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
    the_spot = arrange_output(run_name='Test',proc_dir=thedir,outputDS='madgraph_OTF._00001.events.tar.gz',saveProcDir=keepOutput,runArgs=runArgs)
    if the_spot == '':
        mglog.error('Error arranging output dataset!')
        return -1

    mglog.info('All done generating events!!')
    return [xqcut,the_spot]


def update_lhe_file(lhe_file_old,param_card_old=None,lhe_file_new=None,masses={},delete_old_lhe=True):
    """Build a new LHE file from an old one and an updated param card.
    The masses of some particles can be changed via the masses dictionary.  No particles that appear in the events
    may have their masses changed.
    If the param card is provided, the decay block in the LHE file will be replaced with the one in the param card.
    By default, the old LHE file is removed.
    If None is provided as a new LHE file name, the new file will replace the old one."""
    # If we want to just use a temp file, then put in a little temp holder
    lhe_file_new_tmp = lhe_file_new if lhe_file_new is not None else lhe_file_old+'.tmp'
    # Make sure the LHE file is there
    if not os.access(lhe_file_old,os.R_OK):
        mglog.error('Could not access old LHE file at '+str(lhe_file_old)+'. Please check the file location.')
        return -1
    # Grab the old param card
    paramcard = subprocess.Popen(['get_files','-data',param_card_old])
    paramcard.wait()
    if not os.access(param_card_old,os.R_OK):
        mglog.info('Could not get param card '+param_card_old)
    # Don't overwrite old param cards
    if os.access(lhe_file_new_tmp,os.R_OK):
        mglog.error('Old file at'+str(lhe_file_new_tmp)+' in the current directory. Dont want to clobber it. Please move it first.')
        return -1

    oldparam = open(param_card_old,'r')
    newlhe = open(lhe_file_new_tmp,'w')
    blockName = None
    decayEdit = False
    eventRead = False
    particles_in_events = []
    # Decay block ends with </slha>

    with open(lhe_file_old,'r') as fileobject:
        for line in fileobject:
            if decayEdit and not '</slha>' in line: continue
            if decayEdit and '</slha>' in line:
                decayEdit = False
            if line.strip().upper().startswith('BLOCK') or line.strip().upper().startswith('DECAY')\
                        and len(line.strip().split()) > 1:
                pos = 0 if line.strip().startswith('DECAY') else 1
                blockName = line.strip().upper().split()[pos]

            akey = None
            if blockName != 'DECAY' and len(line.strip().split()) > 0:
                akey = line.strip().split()[0]
            elif blockName == 'DECAY' and len(line.strip().split()) > 1:
                akey = line.strip().split()[1]

            # Replace the masses with those in the dictionary
            if akey != None and blockName == 'MASS'  and akey in masses:
                newlhe.write('   %s    %s  # \n'%(akey,str(masses[akey])))
                mglog.info('   %s    %s  #'%(akey,str(masses[akey])))
                decayEdit = False
                continue

            # Replace the entire decay section of the LHE file with the one from the param card
            if blockName == 'DECAY' and param_card_old is not None:
                # We are now reading the decay blocks!  Take them from the param card
                oldparam = open(param_card_old,'r')
                newDecays = False
                for old_line in oldparam.readlines():
                    newBlockName = None
                    if old_line.strip().upper().startswith('DECAY') and len(old_line.strip().split()) > 1:
                        newBlockName = line.strip().upper().split()[pos]
                    if newDecays:
                        newlhe.write(old_line)
                    elif newBlockName == 'DECAY':
                        newDecays = True
                        newlhe.write(old_line)
                oldparam.close()
                # Done adding the decays
                decayEdit = True
                blockName = None
                continue

            # Keep a record of the particles that are in the events
            if not eventRead and '<event>' in line: eventRead = True
            if eventRead:
                if len(line.split())==11:
                    aparticle = line.split()[0]
                    if not aparticle in particles_in_events: particles_in_events += [aparticle]

            # Otherwise write the line again
            newlhe.write(line)

    # Check that none of the particles that we were setting the masses of appear in the LHE events
    for akey in masses:
        if akey in particles_in_events:
            mglog.error('Attempted to change mass of a particle that was in an LHE event!  This is not allowed!')
            return -1

    # Close up and return
    newlhe.close()

    # Move the new file to the old file location
    if lhe_file_new is None:
        os.remove(lhe_file_old)
        shutil.move(lhe_file_new_tmp,lhe_file_old)
        lhe_file_new_tmp = lhe_file_old
    # Delete the old file if requested
    elif delete_old_lhe:
        os.remove(lhe_file_old)

    return lhe_file_new_tmp



def build_param_card(param_card_old=None,param_card_new='param_card.dat',masses={},decays={},extras={},params={}):
    """Build a new param_card.dat from an existing one.
    Params should be a dictionary of dictionaries. The first key is the block name, and the second in the param name.
    Eventually params will replace the other arguments, but we need to keep them for backward compatibility for now."""
    # Grab the old param card and move it into place
    if os.access(param_card_old,os.R_OK):
        mglog.info('Found old param card at '+param_card_old)
    else:
        paramcard = subprocess.Popen(['get_files','-data',param_card_old])
        paramcard.wait()
    if not os.access(param_card_old,os.R_OK):
        mglog.info('Could not get param card '+param_card_old)
    if os.access(param_card_new,os.R_OK):
        mglog.error('Old param card at '+str(param_card_new)+' in the current directory. Dont want to clobber it. Please move it first.')
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
                   scalefact=-1.,alpsfact=-1.,extras={}):
    """Build a new run_card.dat from an existing one.
    Extras is a dictionary of keys (no spaces needed) and
    values to replace as well.
    """
    # Handle scalefact setting -- old setup makes this a little clunky
    if 'scalefact' in extras:
        if scalefact>0 and scalefact!=extras['scalefact']:
            mglog.error('Scalefact set in both extras (%1.2f) and arguments to build_run_card (%1.2f)'%(extras['scalefact'],scalefact))
            mglog.error('and not equal! Do not know what to do, so giving up')
            raise RuntimeError('Inconsistent setting of scalefact')
        elif scalefact<0:
            scalefact=extras['scalefact']
            del extras['scalefact']
    # Otherwise set the default value
    elif scalefact<=0:
        scalefact=1.

    # Handle alpsfact setting -- old setup makes this a little clunky
    if 'alpsfact' in extras:
        if alpsfact>0 and alpsfact!=extras['alpsfact']:
            mglog.error('Alpsfact set in both extras (%1.2f) and arguments to build_run_card (%1.2f)'%(extras['alpsfact'],alpsfact))
            mglog.error('and not equal! Do not know what to do, so giving up')
            raise RuntimeError('Inconsistent setting of alpsfact')
        elif alpsfact<0:
            alpsfact=extras['alpsfact']
            del extras['alpsfact']
    # Otherwise set the default value
    elif alpsfact<=0:
        alpsfact=1.

    # guess NLO from run card -- not the most robust way to do this but otherwise arguments of build_run_card would need to change
    isNLO=isNLO_from_run_card(run_card_old)
    # add gobal PDF and scale uncertainty config to extras, except PDF or weights for syscal config are explictly set
    setup_pdf_and_systematic_weights(MADGRAPH_PDFSETTING,extras,isNLO)

    # Grab the old run card and move it into place
    # Get the run card from the installation
    if os.access(run_card_old,os.R_OK):
        mglog.info('Found old card at '+run_card_old)
    else:
        runcard = subprocess.Popen(['get_files','-data',run_card_old])
        runcard.wait()

    if not os.access(run_card_old,os.R_OK):
        mglog.info('Could not get run card '+run_card_old)
    if os.access(run_card_new,os.R_OK):
        mglog.error('Old run card in the current directory. Dont want to clobber it. Please move it first.')
        return -1

    oldcard = open(run_card_old,'r')
    newcard = open(run_card_new,'w')
    used_options = []
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
                actual_line=line.split('!')[0]
                comment=''
                if len(line.split('!'))>1:
                    comment=line.split('!')[1]
                if '='+ak.strip() in actual_line.replace(' ','') and (len(actual_line.strip().split(ak.strip())[1])==0 or actual_line.split(ak.strip())[1][0]==" "):
                    newline=' '+str(extras[ak])+'    ='+actual_line.split('=')[-1]+'!'+comment
                    newcard.write(newline )
                    used_options += [ ak ]
                    break
                # if a setting is partly upper case in the run_card but the user gave a lower case name
                # (e.g. reweight_PDF is written in the run card but reweight_pdf given by user)
                # it will still be processed properly
                elif '='+ak.strip() in actual_line.lower().replace(' ','') and (len(actual_line.lower().strip().split(ak.strip())[1])==0 or actual_line.lower().split(ak.strip())[1][0]==" "):
                    newline=' '+str(extras[ak])+'    ='+actual_line.split('=')[-1]+'!'+comment
                    newcard.write(newline )
                    used_options += [ ak ]
                    break

            else: # Fell through the loop
                newcard.write(line)
    # Clean up options that weren't used
    newcard.write('\n')
    for ak in extras:
        if ak in used_options: continue
        mglog.warning('Option '+ak+' was not in the default run_card.  Adding by hand a setting to '+str(extras[ak]) )
        newcard.write( ' '+str(extras[ak])+'   = '+ak+'\n')
    # Close up
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
    used_settings = []
    for line in iter(oldCard):
        if not line.strip().startswith('#'): # line commented out
            command = line.split('!', 1)[0]
            comment = line.split('!', 1)[1] if '!' in line else ''
            if '=' in command:
                setting = command.split('=')[-1] #.strip()
                stripped_setting = setting.strip()
                oldValue = '='.join(command.split('=')[:-1])
                if stripped_setting in settings:
                    # if setting set to 'None' it will be removed from run_card
                    if settings[stripped_setting]==None:
                        line=''
                        mglog.info('Removing '+stripped_setting+'.')
                        used_settings += [ stripped_setting ]
                    else:
                        line = oldValue.replace(oldValue.strip(), str(settings[stripped_setting]))+'='+setting
                        if comment != '': line += '  !' + comment
                        mglog.info('Setting '+stripped_setting+' = '+str(settings[stripped_setting])+'.')
                        used_settings += [ stripped_setting ]
        newCard.write(line)

    # Clean up unused options
    for asetting in settings:
        if asetting in used_settings: continue
        if settings[asetting]==None: continue
        mglog.warning('Option '+asetting+' was not in the default run_card.  Adding by hand a setting to '+str(settings[asetting]) )
        newCard.write( ' '+str(settings[asetting])+'   = '+str(asetting)+'\n')
    # close files
    oldCard.close()
    newCard.close()
    mglog.info('Finished modification of run card.')
    if delete_backup: os.unlink(run_card_backup)


# Update a param card default template with blocks from a different param card
def update_param_card_blocks( process_dir , from_param_card , to_param_card ):
    # Make sure the files exist where we expect them to
    if not os.access(process_dir+'/Cards/param_card.dat',os.R_OK):
        mglog.error('update_param_card_blocks: Could not find param card in '+process_dir)
        raise RuntimeError('update_param_card_blocks: No param card')
    if not os.access(from_param_card,os.R_OK):
        mglog.error('update_param_card_blocks: Could not find param card '+from_param_card)
        raise RuntimeError('update_param_card_blocks: No param card')
    # Open up the old param cards and read in their lines
    proc_param_card_f = open(process_dir+'/Cards/param_card.dat','r')
    proc_param_card_l = proc_param_card_f.readlines()
    from_param_card_f = open(from_param_card,'r')
    from_param_card_l = from_param_card_f.readlines()
    # What blocks are in the old card?
    from_blocks = [x.split()[1] for x in from_param_card_l if 'block ' in x.lower()]
    # What decays are in the old card?
    from_decays = [x.split()[1] for x in from_param_card_l if 'decay ' in x.lower()]
    # Build a dictionary of blocks and decays
    from_block_dict = {}
    from_decay_dict = {}
    line_number=0
    sys.stdout.flush()

    while line_number<len(from_param_card_l):
        if from_param_card_l[line_number].lower().startswith('block '):
            my_block = from_param_card_l[line_number].split()[1].lower()
            holder=from_param_card_l[line_number]
            line_number += 1
            while line_number<len(from_param_card_l) and not from_param_card_l[line_number].lower().startswith('block ') and not from_param_card_l[line_number].lower().startswith('decay '):
                holder += from_param_card_l[line_number]
                line_number += 1
            from_block_dict[my_block] = holder
        elif from_param_card_l[line_number].lower().startswith('decay '):
            my_decay = from_param_card_l[line_number].split()[1]
            holder=from_param_card_l[line_number]
            line_number += 1
            while line_number<len(from_param_card_l) and not from_param_card_l[line_number].lower().startswith('block ') and not from_param_card_l[line_number].lower().startswith('decay '):
                holder += from_param_card_l[line_number]
                line_number += 1
            from_decay_dict[my_decay] = holder
        else:
            line_number += 1
    # Now write our new param card
    to_param_card_f = open(to_param_card,'w')
    line_number=0
    sys.stdout.flush()

    while line_number<len(proc_param_card_l):
        if proc_param_card_l[line_number].lower().startswith('block ') and proc_param_card_l[line_number].split()[1].lower() in from_block_dict:
            my_block = proc_param_card_l[line_number].split()[1].lower()
            # Use the above block.  Watch for lines that we didn't have that should now be there!
            to_param_card_f.write(from_block_dict[my_block])
            # Get the list of parameters in this block that are being set
            block_pars = [l.split()[0] for l in from_block_dict[my_block].split('\n') if len(l.split('#')[0].strip())>0]
            # Advance to the next line of the default card
            line_number += 1
            # Advance to the next point in the file that we care about
            while line_number<len(proc_param_card_l) and not proc_param_card_l[line_number].lower().startswith('block ') and not proc_param_card_l[line_number].lower().startswith('decay '):
                if len(proc_param_card_l[line_number].lower().split('#')[0].strip())>0 and my_block not in ['alpha']:
                    my_par = proc_param_card_l[line_number].lower().split('#')[0].strip().split()[0]
                    if not my_par in block_pars:
                        to_param_card_f.write(proc_param_card_l[line_number])
                line_number += 1
        elif proc_param_card_l[line_number].lower().startswith('decay ') and proc_param_card_l[line_number].split()[1] in from_decay_dict:
            # Use the above decay - safe to take the whole thing
            to_param_card_f.write(from_decay_dict[proc_param_card_l[line_number].split()[1]])
            line_number += 1
            # Advance to the next point in the file that we care about
            while line_number<len(proc_param_card_l) and not proc_param_card_l[line_number].lower().startswith('block ') and not proc_param_card_l[line_number].lower().startswith('decay '):
                line_number += 1
        else:
            to_param_card_f.write(proc_param_card_l[line_number])
            line_number += 1
    # Close out
    to_param_card_f.close()
    from_param_card_f.close()
    proc_param_card_f.close()


def print_cards(proc_card='proc_card_mg5.dat',run_card='run_card.dat',param_card='param_card.dat'):
    if os.access(proc_card,os.R_OK):
        mglog.info("proc_card:")
        procCard = subprocess.Popen(['cat',proc_card])
        procCard.wait()
    else:
        mglog.warning('No proc_card: '+proc_card+' found')

    if os.access(run_card,os.R_OK):
        mglog.info("run_card:")
        runCard = subprocess.Popen(['cat',run_card])
        runCard.wait()
    else:
        mglog.warning('No run_card: '+run_card+' found')

    if os.access(param_card,os.R_OK):
        mglog.info("param_card:")
        runCard = subprocess.Popen(['cat',param_card])
        runCard.wait()
    else:
        mglog.warning('No param_card: '+param_card+' found')


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

def run_card_consistency_check(isNLO=False,path='.'):
    cardpath=path+'/Cards/run_card.dat'
    mydict=getDictFromCard(cardpath)

    for k,v in mydict.iteritems():
        mglog.info( '"%s" = %s'%(k,v) )

    if not isNLO:
        #Check CKKW-L setting
        if float(mydict['ktdurham']) > 0 and int(mydict['ickkw']) != 0:
            log='Bad combination of settings for CKKW-L merging! ktdurham=%s and ickkw=%s.'%(mydict['ktdurham'],mydict['ickkw'])
            mglog.error(log)
            raise RuntimeError(log)

        version = getMadGraphVersion() # avoiding code duplication
        # Only needed for 2.5.0 and later
        if int(version.split('.')[0])>=2 and int(version.split('.')[1])>=5:
            #event_norm must be "sum" for use_syst to work
            if mydict['use_syst'].replace('.','').lower() in ['t','true']:
                if 'event_norm' not in mydict or mydict['event_norm']!="sum":
                    modify_run_card(cardpath,cardpath.replace('.dat','_backup.dat'),{'event_norm':'sum'})

        # Check if user is trying to use deprecated syscalc arguments with the other systematics script
        if is_version_or_newer([2,6,2]) and (not 'systematics_program' in mydict or mydict['systematics_program']=='systematics'):
            syscalc_settings=['sys_pdf', 'sys_scalefact', 'sys_alpsfact', 'sys_matchscale']
            found_syscalc_setting=False
            for s in syscalc_settings:
                if s in mydict:
                    mglog.warning('Using syscalc setting %s with new systematics script. Systematics script is default from 2.6.2 and steered differently (https://cp3.irmp.ucl.ac.be/projects/madgraph/wiki/Systematics#Systematicspythonmodule)'%(s))
                    found_syscalc_setting=True
            if found_syscalc_setting:
                syst_arguments=convertSysCalcArguments(mydict)
                mglog.info('Converted syscalc arguments to systematics arguments: '+syst_arguments)
                syst_settings_update={'systematics_arguments':syst_arguments}
                for s in syscalc_settings:
                    syst_settings_update[s]=None
                modify_run_card(cardpath,cardpath.replace('.dat','_syscalc.dat'),syst_settings_update)


    # usually the pdf and systematics should be set during build_run_card
    # but check again in case the user did not call the function or provides a different card here
    mglog.info('Checking PDF and systematics settings')
    if not base_fragment_setup_check(MADGRAPH_PDFSETTING,mydict,isNLO):
        # still need to set pdf and systematics
        syst_settings=get_pdf_and_systematic_settings(MADGRAPH_PDFSETTING,isNLO)
        modify_run_card(cardpath,cardpath.replace('.dat','_before_syst.dat'),syst_settings)

    mglog.info('Finished checking run card - All OK!')
    return

def mode_njobs_consistency_check(mode,njobs,cluster_type):
    if cluster_type!=None and mode!=1:
        raise RuntimeError('Set a non-None cluster type (cluster_type='+str(cluster_type)+') but did not set run mode to 1 but to '+str(mode))
    elif njobs<1:
        raise RuntimeError('Requested nonsensical number of jobs: njobs='+str(njobs))

    if mode==0:
        if njobs==1:
            return True
        else:
            raise RuntimeError('Requested mode 0 (single core) but more than 1 job: njobs='+str(njobs))
    elif mode==1:
        return True
    elif mode==2:
        if int(njobs)>1:
            return True
        else:
            raise RuntimeError('Requested mode 2 (multi core) but only 1 job: njobs='+str(njobs))
    else:
        raise RuntimeError('MadGraph run mode has to be 0, 1, or 2 (= single-core, cluster, multi-core mode)')

def hack_gridpack_script(gridpack_dir,reweight_card,madspin_card):
    need_to_add_rwgt=reweight_card!=None

    run_card_dict=getDictFromCard(gridpack_dir+'/Cards/run_card.dat',lowercase=True)

    systematics_program=None
    if settingIsTrue(run_card_dict['use_syst']):
        if is_version_or_newer([2,6,2]):
            systematics_program='systematics'
        else:
            systematics_program='syscalc'
        run_card_dict['systematics_program']
        if checkSettingExists('systematics_program',run_card_dict):
            if checkSetting('systematics_program','systematics',run_card_dict):
                systematics_program='systematics'
            if checkSetting('systematics_program','syscalc',run_card_dict):
                systematics_program='syscalc'
            if checkSetting('systematics_program','none',run_card_dict):
                systematics_program=None
    need_to_add_syst=systematics_program!=None

    systematics_arguments=''
    if checkSettingExists('systematics_arguments',run_card_dict):
        sys_dict=parse_systematics_arguments(run_card_dict['systematics_arguments'])
        for s in sys_dict:
            systematics_arguments+=' --'+s+'='+sys_dict[s]

    # add systematics calculation and reweighting to run.sh
    runscript=gridpack_dir+'/bin/run.sh'
    oldscript = open(runscript,'r')
    newscript = open(runscript+'.tmp','w')
    # in older MG versions the gridpack is run with the command below
    gridrun_line_old='./bin/gridrun $num_events $seed'
    syst_line_old='./bin/madevent syscalc GridRun_${seed} -f\n'
    if systematics_program=='systematics':
        #syst_line_old='./bin/madevent systematics GridRun_${seed} -f\n'
        syst_line_old='python bin/internal/systematics.py Events/GridRun_123456/unweighted_events.lhe.gz Events/GridRun_123456/unweighted_events.lhe.gz '+systematics_arguments+'\n'
    reweight_line_old='./bin/madevent reweight GridRun_${seed} -f\n'
    # in new versions it is run like this
    gridrun_line_new='${DIR}/bin/gridrun $num_events $seed $gran'
    syst_line_new='${DIR}/bin/madevent syscalc GridRun_${seed} -f\n'
    if systematics_program=='systematics':
#        syst_line_new='${DIR}/bin/madevent systematics GridRun_${seed} -f\n'
        syst_line_new='python ${DIR}/bin/internal/systematics.py  ${DIR}/Events/GridRun_123456/unweighted_events.lhe.gz ${DIR}/Events/GridRun_123456/unweighted_events.lhe.gz '+systematics_arguments+'\n'
    reweight_line_new='${DIR}/bin/madevent reweight GridRun_${seed} -f\n'

    for line in oldscript:
        if (need_to_add_rwgt or need_to_add_syst) and gridrun_line_old in line:
            if madspin_card:
                # renaming madspin card deactivates madspin -- madspin should be run after reweighting and systematics
                newscript.write('mv Cards/madspin_card.dat Cards/madspin_card_backup.dat\n')
            newscript.write(line)
            # run systematics
            if need_to_add_syst:
                newscript.write(syst_line_old)
                need_to_add_syst=False
            # reweight
            if need_to_add_rwgt:
                newscript.write(reweight_line_old)
                need_to_add_rwgt=False
            if madspin_card:
                # move madspin card back in place and run madspin
                newscript.write('mv Cards/madspin_card_backup.dat Cards/madspin_card.dat\n')
                newscript.write('./bin/madevent decay_events GridRun_${seed} -f\n')
        elif (need_to_add_rwgt or need_to_add_syst) and gridrun_line_new in line:
            if madspin_card:
                # renaming madspin card deactivates madspin -- madspin should be run after reweighting
                newscript.write('mv $DIR/Cards/madspin_card.dat $DIR/Cards/madspin_card_backup.dat\n')
            newscript.write(line)
            # run systematics
            if need_to_add_syst:
                newscript.write(syst_line_new)
                need_to_add_syst=False
            # reweight
            if need_to_add_rwgt:
                newscript.write(reweight_line_new)
                need_to_add_rwgt=False
            if madspin_card:
                # move madspin card back in place and run madspin
                newscript.write('mv $DIR/Cards/madspin_card_backup.dat $DIR/Cards/madspin_card.dat\n')
                newscript.write('$DIR/bin/madevent decay_events GridRun_${seed} -f\n')

        else:
            newscript.write(line)
    oldscript.close()
    newscript.close()
    mglog.info('created '+runscript+'.tmp')

    if reweight_card and need_to_add_rwgt:
        raise RuntimeError('Could not add reweighting to gridpack script: '+runscript+' maybe line to generate events changed')
    shutil.move(runscript+'.tmp',runscript)
    st = os.stat(runscript)
    os.chmod(runscript, st.st_mode | stat.S_IEXEC)


def SUSY_model():
    if not 'MADPATH' in os.environ:
        mglog.warning('No MADPATH in your environment - not sure what the right SUSY model is')
        return 'import model mssm\n'
    if os.access(os.environ['MADPATH']+'/models/mssm',os.R_OK):
        mglog.info('Using mssm model')
        return 'import model mssm\n'
    if os.access(os.environ['MADPATH']+'/models/MSSM_SLHA2',os.R_OK):
        mglog.info('Using MSSM_SLHA2 model')
        return 'import model MSSM_SLHA2\n'
    mglog.warning('No idea what to do - models not found!')
    return 'import model mssm\n'


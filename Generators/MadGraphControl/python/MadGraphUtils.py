#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Pythonized version of MadGraph steering executables
#    written by Zach Marshall <zach.marshall@cern.ch>
#  Attempts to remove path-dependence of MadGraph

import os,sys,time,subprocess,shutil,glob
from AthenaCommon import Logging
mglog = Logging.logging.getLogger('MadGraphUtils')

def new_process(card_loc='proc_card_mg5.dat'):
    try:
        from __main__ import opts
        if opts.config_only:
            mglog.info('Athena running on config only mode: not executing MadGraph')
            return
    except:
        pass

    madpath=os.environ['MADPATH']

    if not os.access(madpath+'/bin/mg5',os.R_OK):
        mglog.error('No mg5 executable found in '+madpath+'/bin')
        return 1

    # Check if we have to use MG4 proc_card.dat or MG5 proc_card.dat
    if not os.access(card_loc,os.R_OK):
        mglog.error('No process card found at '+card_loc)
        return 1
    else:
        mglog.info('Assuming that '+card_loc+' is a well-formatted process card')

    mglog.info('Started at '+str(time.asctime()))

    generate = subprocess.Popen([madpath+'/bin/mg5',card_loc],stdin=subprocess.PIPE)
    generate.wait()

    mglog.info('Finished at '+str(time.asctime()))

    directories = os.listdir( os.getcwd() )
    thedir = ''
    for adir in sorted(directories):
        if 'PROC' in adir: thedir=adir

    if not os.access('%s/SubProcesses/subproc.mg'%thedir,os.R_OK):
        mglog.error('No diagrams for this process!')
        return 1
    return thedir

def generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir='PROC_mssm_0',grid_pack=False,cluster_type=None,cluster_queue=None,extlhapath=None):
    try:
        from __main__ import opts
        if opts.config_only:
            mglog.info('Athena running on config only mode: not executing MadGraph')
            return
    except:
        pass

    madpath=os.environ['MADPATH']
    if grid_pack:
        mglog.info('Started generating gridpack at '+str(time.asctime()))
        mglog.warning(' >>>>>> THIS KIND OF JOB SHOULD ONLY BE RUN LOCALLY - NOT IN GRID JOBS <<<<<<')

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
        mglog.info('Started generating at '+str(time.asctime()))
    mglog.info('Run '+str(run_name)+' will be performed in mode '+str(mode)+' with '+str(njobs)+' jobs in parallel.')

    if param_card_loc is None: mglog.info('Will use the default parameter card')

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

    # For dealing with LHAPDF -sets
    # Check first for external LHAPDF
    if extlhapath:
        mglog.info('Using external LHAPDF.')
        # Store LHAPATH that comes with release before editing
        releaselhapath=os.environ['LHAPATH']
        # Need to fix path to be absolute before symlinking
        if os.path.isabs(extlhapath):
            os.environ['LHAPATH']=extlhapath
        else:
            os.environ['LHAPATH']=os.environ['TestArea']+'/'+extlhapath
            mglog.info('Modifying extlhapath to be absolute - assuming installation is in '+os.environ['TestArea'])
        mglog.info('External LHAPATH is:')
        mglog.info('  '+os.environ['LHAPATH'])
    else:
        mglog.info('Using Athena release LHAPDF, LHAPATH is:')
        mglog.info('  '+os.environ['LHAPATH'])
                    
    mglog.info('Creating links for LHA running')
    os.symlink(os.environ['LHAPATH'],proc_dir+'/lib/PDFsets')
    mglog.info('Available PDFs are:')
    mglog.info( sorted( os.listdir( proc_dir+'/lib/PDFsets/' ) ) )

    if not extlhapath:
        mglog.info( 'Linking in libraries based on the path '+os.environ['LHAPATH']+' with config '+os.environ['CMTCONFIG'] )
        lhalibpath = os.environ['LHAPATH'].split('share/')[0]+os.environ['CMTCONFIG']+'/lib/'
    else:
        useReleaseLibs=False
        lhalibpath = os.environ['LHAPATH'].split('share/')[0]+'/lib/'
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

            
    mglog.info('For your information, the libraries available are (should include LHAPDF):')
    mglog.info( sorted( os.listdir( proc_dir+'/lib/' ) ) )


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
        njobs = os.environ['ATHENA_PROC_NUMBER']
        mglog.info('Lucky you - you are running on a full node queue.  Will re-configure for '+str(njobs)+' jobs.')
        mode = 2
        athenaMP = True
        
    if njobs>1 and mode==0: mglog.warning('Inconsistent configuration between mode '+str(mode)+' and number of jobs '+str(njobs))
    if njobs>1:
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
    else:
        mglog.info('Running serial generation.  This will take a bit more time than parallel generation.')
        generate = subprocess.Popen(['bin/generate_events','0',str(run_name)],stdin=subprocess.PIPE)
        generate.wait()

    if grid_pack:
        gridpack_name=(run_name+'_gridpack.tar.gz')
        mglog.info('Tidying up gridpack (%s)...'%gridpack_name)

        os.chdir(currdir)
        shutil.copy((proc_dir+'/'+run_name+'_gridpack.tar.gz'),gridpack_name)

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
        tar = subprocess.Popen(['tar','cvzfh',('../'+gridpack_name),'.'])
        tar.wait()
        os.chdir('../')
        mglog.info('Remove temporary directory')
        remove_tmp = subprocess.Popen(['rm','-fr','tmp%i/'%os.getpid()])
        remove_tmp.wait()
        mglog.info('Tidying up complete!')

    os.chdir(currdir)

    mglog.info('Finished at '+str(time.asctime()))
    return 0

def generate_from_gridpack(run_name='Test',gridpack_dir='madevent/',nevents=-1,random_seed=-1):

    mglog.info('Generating events from gridpack')
    
    # Ensure that things are set up normally
    if not os.path.exists(gridpack_dir):
        mglog.error('Gridpack directory not found at '+gridpack_dir)
        return 1

    mglog.info('>>>> FOUND GRIDPACK <<<<  <- This will be used for generation')
    mglog.info('Generation of '+str(nevents)+' events will be performed using the supplied gridpack with random seed '+str(random_seed))
    mglog.info('Started generating at '+str(time.asctime()))

    currdir=os.getcwd()
    
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

    mglog.info('For your information, ls of '+currdir+':')
    mglog.info( sorted( os.listdir( currdir ) ) )
 
    mglog.info('Moving generated events to be in correct format for arrange_output().')
    mglog.info('Unzipping generated events.')
    unzip = subprocess.Popen(['gunzip','events.lhe.gz'])
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

    return 0


def arrange_output(run_name='Test',proc_dir='PROC_mssm_0',outputDS='madgraph.000001.Example._00001.events.tar.gz',skip_events=0):
    try:
        from __main__ import opts
        if opts.config_only:
            mglog.info('Athena running on config only mode: not executing MadGraph')
            return
    except:
        pass

    mglog.info('Unzipping generated events.')
    unzip = subprocess.Popen(['gunzip',proc_dir+'/Events/'+run_name+'/unweighted_events.lhe.gz'])
    unzip.wait()

    mglog.info('Putting a copy in place for the transform.')
    orig_input = open(proc_dir+'/Events/'+run_name+'/unweighted_events.lhe','r')
    mod_output = open(os.getcwd()+'/events.lhe','w')

    skipped = False
    fillerEvent = """<event>
 6  36  0.1133000E-02  0.2112863E+03  0.7957747E-01  0.1147102E+00
       21   -1    0    0  501  502  0.00000000000E+00  0.00000000000E+00  0.19080438510E+03  0.19080438510E+03  0.00000000000E+00 0. -1.
       21   -1    0    0  502  503  0.00000000000E+00  0.00000000000E+00 -0.20776970980E+03  0.20776970980E+03  0.00000000000E+00 0. -1.
  2000002    2    1    2  501    0  0.49986411532E+02 -0.10875498693E+02 -0.17155263876E+02  0.18728766873E+03  0.17934728872E+03 0.  0.
        2    1    3    3  501    0  0.39289087124E+02 -0.87637873906E+02 -0.17731830657E+02  0.97664973907E+02  0.00000000000E+00 0.  1.
  1000022    1    3    3    0    0  0.10697324408E+02  0.76762375213E+02  0.57656678129E+00  0.89622694822E+02  0.45000000000E+02 0.  1.
 -2000002    1    1    2    0  503 -0.49986411532E+02  0.10875498693E+02  0.18993917224E+00  0.21128642618E+03  0.20500000000E+03 0.  0.
#  0.1400000E+05  0.1400000E+05  0.2112863E+03
</event>
"""
    for line in orig_input.readlines():
        if not '<event>' in line or skipped: mod_output.write(line)
        else:
            mglog.info('Adding events for skipping '+str(skip_events)+' events in Pythia')
            for ev in xrange(0,skip_events):
                mod_output.write(fillerEvent)
            skipped = True
            mod_output.write(line)

    mglog.info('Moving file over to '+outputDS.split('.tar.gz')[0])
    shutil.move(proc_dir+'/Events/'+run_name+'/unweighted_events.lhe',outputDS.split('.tar.gz')[0])

    mglog.info('Re-zipping into dataset name '+outputDS)
    rezip = subprocess.Popen(['tar','cvzf',outputDS,outputDS.split('.tar.gz')[0]])
    rezip.wait()

    mglog.info('All done with output arranging!')
    return 0

def get_decays(gentype='SS',decaytype='direct',ptype=None):
    flipdict = { 'u':'d' , 'd':'u' , 's':'c' , 'c':'s' }
    Wdict = [ ['u','d~','-'] , ['d','u~','+'] , ['s','c~','+'] , ['c','s~','-'] ]
    if 'direct' in decaytype:
        if 'SS'==gentype:
            squark1 = ptype[0][0]
            if len(ptype[0])>2: squark1 += '~'
            squark2 = ptype[1][0]
            if len(ptype[1])>2: squark2 += '~'
            return [', %s > %s n1 , %s > %s n1'%(ptype[0],squark1,ptype[1],squark2)]
        if 'GG'==gentype:
            decaylist = []
            if not 'BB' in decaytype:
                for s in [ 'u','d','c','s' ]:
                    for t in [ 'u','d','c','s' ]:
                        decaylist += [', go > %s %s~ n1 , go > %s %s~ n1'%(s,s,t,t)]
            else:
                decaylist = [', go > b b~ n1 , go > b b~ n1' ]
            return list(set(decaylist))
        if 'CC'==gentype:
            return [', %s > W%s n1 , %s > W%s n1'%(ptype[0],ptype[0][2],ptype[1],ptype[1][2])]
        if 'Scharm'==gentype:
            a1 = '~' if len(ptype[0])>2 else ''
            a2 = '~' if len(ptype[1])>2 else ''
            return [', %s > c%s n1 , %s > c%s n1'%(ptype[0],a1,ptype[1],a2)]
        if 'BB'==gentype:
            a1 = '~' if len(ptype[0])>2 else ''
            a2 = '~' if len(ptype[1])>2 else ''
            return [', %s > b%s n1 , %s > b%s n1'%(ptype[0],a1,ptype[1],a2)]
        if 'TT'==gentype and 'CC' in decaytype:
            a1 = '~' if len(ptype[0])>2 else ''
            a2 = '~' if len(ptype[1])>2 else ''
            return [', %s > c%s n1 , %s > c%s n1'%(ptype[0],a1,ptype[1],a2)]
        if 'TT'==gentype and 'TT' in decaytype:
            a1 = '~' if len(ptype[0])>2 else ''
            a2 = '~' if len(ptype[1])>2 else ''
            b1 = '-' if len(ptype[0])>2 else '+'
            b2 = '-' if len(ptype[1])>2 else '+'
            return [', (%s > t%s n1 , (t%s > W%s b%s, W%s > fu%s fd%s)), (%s > t%s n1, (t%s > W%s b%s, W%s > fu%s fd%s))'%(ptype[0],a1,a1,b1,a1,b1,a1,a2,ptype[1],a2,a2,b2,a2,b2,a2,a1)]
    if 'mixed' in decaytype:
        if 'BB'==gentype:
            if 'Higgs' in decaytype:
                a1 = '~' if len(ptype[0])>2 else ''
                a2 = '~' if len(ptype[1])>2 else ''
                decaylist = []
                decaylist += [', (b1 > n2 b , n2 > h1 n1) , (b1~ > b~ n1) ']
                decaylist += [', (b1~ > n2 b~ , n2 > h1 n1) , (b1 > b n1) ']
                return decaylist
        if 'TT'==gentype:
            a1 = '~' if len(ptype[0])>2 else ''
            a2 = '~' if len(ptype[1])>2 else ''
            b1 = '-' if len(ptype[0])>2 else '+'
            b2 = '-' if len(ptype[1])>2 else '+'
            decaylist = []
            if 'BT2' in decaytype:
                if 'offshell' in decaytype:
                    decaylist += [', (%s > t%s n2 , (t%s > W%s b%s, W%s > fu%s fd%s)), %s > b%s x1%s'%(ptype[0],a1,a1,b1,a1,b1,a1,a2,ptype[1],a2,b2)]
                    decaylist += [', %s > b%s x1%s , (%s > t%s n2 , (t%s > W%s b%s, W%s > fu%s fd%s))'%(ptype[0],a1,b1,ptype[1],a2,a2,b2,a2,b2,a2,a1)]
                else:
                    decaylist += [', (%s > t%s n2 , (t%s > W%s b%s, W%s > fu%s fd%s)), (%s > b%s x1%s , (x1%s > W%s n1, W%s > fu%s fd%s))'%(ptype[0],a1,a1,b1,a1,b1,a1,a2,ptype[1],a2,b2,b2,b2,b2,a2,a1)]
                    decaylist += [', (%s > b%s x1%s , (x1%s > W%s n1, W%s > fu%s fd%s)) , (%s > t%s n2 , (t%s > W%s b%s, W%s > fu%s fd%s))'%(ptype[0],a1,b1,b1,b1,b1,a1,a2,ptype[1],a2,a2,b2,a2,b2,a2,a1)]    
                return decaylist
            elif 'BT' in decaytype:
                if 'offshell' in decaytype:
                    decaylist += [', (%s > t%s n1 , (t%s > W%s b%s, W%s > fu%s fd%s)), %s > b%s x1%s'%(ptype[0],a1,a1,b1,a1,b1,a1,a2,ptype[1],a2,b2)]
                    decaylist += [', %s > b%s x1%s , (%s > t%s n1 , (t%s > W%s b%s, W%s > fu%s fd%s))'%(ptype[0],a1,b1,ptype[1],a2,a2,b2,a2,b2,a2,a1)]
                else:
                    decaylist += [', (%s > t%s n1 , (t%s > W%s b%s, W%s > fu%s fd%s)), (%s > b%s x1%s , (x1%s > W%s n1, W%s > fu%s fd%s))'%(ptype[0],a1,a1,b1,a1,b1,a1,a2,ptype[1],a2,b2,b2,b2,b2,a2,a1)]
                    decaylist += [', (%s > b%s x1%s , (x1%s > W%s n1, W%s > fu%s fd%s)) , (%s > t%s n1 , (t%s > W%s b%s, W%s > fu%s fd%s))'%(ptype[0],a1,b1,b1,b1,b1,a1,a2,ptype[1],a2,a2,b2,a2,b2,a2,a1)]    
                return decaylist
            elif 'TT' in decaytpe:
                decaylist += [', (%s > t%s n1 , (t%s > W%s b%s, W%s > fu%s fd%s)), (%s > t%s n2 , (t%s > W%s b%s, W%s > fu%s fd%s))'%(ptype[0],a1,a1,b1,a1,b1,a1,a2,ptype[1],a2,a2,b2,a2,b2,a2,a1)]
                decaylist += [', (%s > t%s n2 , (t%s > W%s b%s, W%s > fu%s fd%s)), (%s > t%s n1 , (t%s > W%s b%s, W%s > fu%s fd%s))'%(ptype[0],a1,a1,b1,a1,b1,a1,a2,ptype[1],a2,a2,b2,a2,b2,a2,a1)]
                return decaylist
                
    if 'onestep' in decaytype:
        if 'SS'==gentype and 'CC' in decaytype:
            if 'r' in ptype[0] or 'r' in ptype[1]: return []
            squark1 = flipdict[ ptype[0][0] ]
            if len(ptype[0])>2: squark1 += '~'
            squark2 = flipdict[ ptype[1][0] ]
            if len(ptype[1])>2: squark2 += '~'
            s1='+'
            if ptype[0][0] in ['d','s'] and len(ptype[0])<=2:  s1='-'
            elif ptype[0][0] in ['u','c'] and len(ptype[0])>2: s1='-'
            s2='+'
            if ptype[1][0] in ['d','s'] and len(ptype[1])<=2:  s2='-'
            elif ptype[1][0] in ['u','c'] and len(ptype[1])>2: s2='-'
            #return [ ', ( %s > %s x1%s , x1%s > W%s n1 ) , ( %s > %s x1%s , x1%s > W%s n1 )'%(ptype[0],squark1,s1,s1,s1,ptype[1],squark2,s2,s2,s2)]
            return [ ', %s > %s x1%s , %s > %s x1%s'%(ptype[0],squark1,s1,ptype[1],squark2,s2)]
        if 'GG'==gentype and 'CC' in decaytype:
            decaylist = []
            for s in Wdict:
                for t in Wdict:
                    #decaylist += [', ( go > %s %s x1%s , x1%s > W%s n1 ) , ( go > %s %s x1%s , x1%s > W%s n1 )'%(s[0],s[1],s[2],s[2],s[2],t[0],t[1],t[2],t[2],t[2])]
                    decaylist += [', go > %s %s x1%s , go > %s %s x1%s'%(s[0],s[1],s[2],t[0],t[1],t[2])]
            return decaylist
        if 'SS'==gentype and ('CN' in decaytype or 'NC' in decaytype):
            thelist = []
            squark1C = flipdict[ ptype[0][0] ]
            if len(ptype[0])>2: squark1C += '~'
            squark2C = flipdict[ ptype[1][0] ]
            if len(ptype[1])>2: squark2C += '~'
            s1='+'
            if ptype[0][0] in ['d','s'] and len(ptype[0])<=2:  s1='-'
            elif ptype[0][0] in ['u','c'] and len(ptype[0])>2: s1='-'
            s2='+'
            if ptype[1][0] in ['d','s'] and len(ptype[1])<=2:  s2='-'
            elif ptype[1][0] in ['u','c'] and len(ptype[1])>2: s2='-'
            squark1N = ptype[0][0]
            if len(ptype[0])>2: squark1N += '~'
            squark2N = ptype[1][0]
            if len(ptype[1])>2: squark2N += '~'
            #if not 'r' in ptype[0]: thelist += [', ( %s > %s x1%s , x1%s > W%s n1 ) , ( %s > %s n2 , n2 > Z n1 )'%(ptype[0],squark1C,s1,s1,s1,ptype[1],squark2N)]
            #if not 'r' in ptype[1]: thelist += [', ( %s > %s n2 , n2 > Z n1 ) , ( %s > %s x1%s , x1%s > W%s n1 )'%(ptype[0],squark1N,ptype[1],squark2C,s2,s2,s2)]
            if not 'r' in ptype[0]: thelist += [', %s > %s x1%s , %s > %s n2'%(ptype[0],squark1C,s1,ptype[1],squark2N)]
            if not 'r' in ptype[1]: thelist += [', %s > %s n2 , %s > %s x1%s'%(ptype[0],squark1N,ptype[1],squark2C,s2)]
            return thelist
        if 'GG'==gentype and ('CN' in decaytype or 'NC' in decaytype):
            decaylist = []
            for s in [ 'u','d','c','s' ]:
                for t in Wdict:
                    #decaylist += [', ( go > %s %s x1%s , x1%s > W%s n1 ) , ( go > %s %s~ n2 , n2 > Z n1 )'%(t[0],t[1],t[2],t[2],t[2],s,s)]
                    decaylist += [', go > %s %s x1%s , go > %s %s~ n2'%(t[0],t[1],t[2],s,s)]
            return decaylist
        if 'TT'==gentype:
            a1 = '~' if len(ptype[0])>2 else ''
            a2 = '~' if len(ptype[1])>2 else ''
            b1 = '-' if len(ptype[0])>2 else '+'
            b2 = '-' if len(ptype[1])>2 else '+'
            if 'BB' in decaytype:
                if 'offshell' in decaytype:
                    return [', %s > b%s x1%s , %s > b%s x1%s'%(ptype[0],a1,b1,ptype[1],a2,b2)]
                else:
                    return [', (%s > b%s x1%s , (x1%s > W%s n1, W%s > fu%s fd%s)), (%s > b%s x1%s, (x1%s > W%s n1, W%s > fu%s fd%s))'%(ptype[0],a1,b1,b1,b1,b1,a1,a2,ptype[1],a2,b2,b2,b2,b2,a2,a1)]
            elif 'TT' in decaytype:
                return [', (%s > t%s n2 , (t%s > W%s b%s, W%s > fu%s fd%s)), (%s > t%s n2, (t%s > W%s b%s, W%s > fu%s fd%s))'%(ptype[0],a1,a1,b1,a1,b1,a1,a2,ptype[1],a2,a2,b2,a2,b2,a2,a1)]
        
    if 'twostep' in decaytype:
        # two steps through charginos / neutralinos, where only the first decay will be done in MadGraph
        if 'SS'==gentype and 'CC' in decaytype:
            if 'r' in ptype[0] or 'r' in ptype[1]: return []
            squark1 = flipdict[ ptype[0][0] ]
            if len(ptype[0])>2: squark1 += '~'
            squark2 = flipdict[ ptype[1][0] ]
            if len(ptype[1])>2: squark2 += '~'
            s1='+'
            if ptype[0][0] in ['d','s'] and len(ptype[0])<=2:  s1='-'
            elif ptype[0][0] in ['u','c'] and len(ptype[0])>2: s1='-'
            s2='+'
            if ptype[1][0] in ['d','s'] and len(ptype[1])<=2:  s2='-'
            elif ptype[1][0] in ['u','c'] and len(ptype[1])>2: s2='-'
            return [ ', %s > %s x1%s , %s > %s x1%s'%(ptype[0],squark1,s1,ptype[1],squark2,s2)]
        if 'GG'==gentype and 'CC' in decaytype:
            decaylist = []
            for s in Wdict:
                for t in Wdict:
                    decaylist += [', go > %s %s x1%s , go > %s %s x1%s'%(s[0],s[1],s[2],t[0],t[1],t[2])]
            return decaylist
        if 'SS'==gentype and ('CN' in decaytype or 'NC' in decaytype):
            thelist = []
            if 'r' in ptype[0] or 'r' in ptype[1]: return []
            squark1C = flipdict[ ptype[0][0] ]
            if len(ptype[0])>2: squark1C += '~'
            squark2C = flipdict[ ptype[1][0] ]
            if len(ptype[1])>2: squark2C += '~'
            s1='+'
            if ptype[0][0] in ['d','s'] and len(ptype[0])<=2:  s1='-'
            elif ptype[0][0] in ['u','c'] and len(ptype[0])>2: s1='-'
            s2='+'
            if ptype[1][0] in ['d','s'] and len(ptype[1])<=2:  s2='-'
            elif ptype[1][0] in ['u','c'] and len(ptype[1])>2: s2='-'
            squark1N = ptype[0][0]+('~' if len(ptype[0])>2 else '')
            squark2N = ptype[1][0]+('~' if len(ptype[1])>2 else '')
            thelist += [', %s > %s x1%s , %s > %s n2'%(ptype[0],squark1C,s1,ptype[1],squark2N)]
            thelist += [', %s > %s n2 , %s > %s x1%s'%(ptype[0],squark1N,ptype[1],squark2C,s2)]
            return thelist
        if 'GG'==gentype and ('CN' in decaytype or 'NC' in decaytype):
            decaylist = []
            for s in [ 'u','d','c','s' ]:
                for t in Wdict:
                    decaylist += [', go > %s %s x1%s , go > %s %s~ n2'%(t[0],t[1],t[2],s,s)]
            return decaylist
        # Plain two-step
        if 'GG'==gentype:
            decaylist = []
            for s in Wdict:
                for t in Wdict:
                    decaylist += [', ( go > %s %s x1%s , ( x1%s > W%s n2 , n2 > Z n1 ) ) , ( go > %s %s x1%s , ( x1%s > W%s n2 > Z n1 ) )'%(s[0],s[1],s[2],s[2],s[2],t[0],t[1],t[2],t[2],t[2])]
            return decaylist
        if 'SS'==gentype:
            if 'r' in ptype[0] or 'r' in ptype[1]: return []
            squark1 = flipdict[ ptype[0][0] ]
            if len(ptype[0])>2: squark1 += '~'
            squark2 = flipdict[ ptype[1][0] ]
            if len(ptype[1])>2: squark2 += '~'
            s1='+'
            if ptype[0][0] in ['d','s'] and len(ptype[0])<=2:  s1='-'
            elif ptype[0][0] in ['u','c'] and len(ptype[0])>2: s1='-'
            s2='+'
            if ptype[1][0] in ['d','s'] and len(ptype[1])<=2:  s2='-'
            elif ptype[1][0] in ['u','c'] and len(ptype[1])>2: s2='-'
            return [ ', ( %s > %s x1%s , ( x1%s > W%s n2 , n2 > Z n1 ) ) , ( %s > %s x1%s , ( x1%s > W%s n2 , ( n2 > Z n1 ) )'%(ptype[0],squark1,s1,s1,s1,ptype[1],squark2,s2,s2,s2)]

    mglog.warning('Requested decays not yet implemented!!  '+str(gentype)+' '+str(decaytype))
    return []




def SUSY_StrongSM_Generation(run_number=100000,gentype='SS',decaytype='direct',masses=None,stringy='100_500_900',\
                             nevts=60000,rand_seed=1234, njets=1, use_decays=True, skip_events=0, syst_mod=None,\
                             SLHAonly=False, beamEnergy=3500., keepOutput=False, decays={}, SLHAexactCopy=False,
                             writeGridpack=False,gridpackDirName=None):
    if not os.environ.has_key('MADGRAPH_DATA'):
        os.environ['MADGRAPH_DATA']=os.getcwd()
        mglog.warning('Setting your MADGRAPH_DATA environmental variable to the working directory')
    # Set up production and decay strings
    if nevts<1000 or nevts>10000000:
        mglog.error('Bad idea to generate '+str(nevts)+' events.  MadGraph wont complain, but the job will never end.  Bailing out now.')
        return -1,''
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
        mglog.fatal('That is just ridiculous.  Do not generate more than eight jets off of your main process in MadGraph.  The world will implode.')
        return -1,''

    starter = 'generate'
    squarks = ['ul','dl','cl','sl','ur','dr','cr','sr']
    counter = 0
    ss_string = ''
    sg_string = ''
    if 'S' in gentype:
        for jets in jetloop:
            for s in squarks:
                sg_string += '%s p p > %s go %s \n'%(starter,s,jets)
                sg_string += '%s p p > %s~ go %s \n'%(starter,s,jets)
                for t in squarks:
                    if not use_decays:
                        if 'phot' in gentype:
                            ss_string += '%s p p > %s %s~ a %s $ go \n'%(starter,s,t,jets)
                            counter = counter+1
                            starter = 'add process'
                            continue
                        else:
                            ss_string += '%s p p > %s %s~ %s $ go \n'%(starter,s,t,jets)
                            counter = counter+1
                            starter = 'add process'
                            continue                            
                    decaylist = get_decays(gentype='SS',decaytype=decaytype,ptype=[s,t+'~'])
                    for decay in decaylist:
                        ss_string += '%s p p > %s %s~ %s $ go %s \n'%(starter,s,t,jets,decay)
                        counter = counter+1
                        starter = 'add process'

    starter = 'generate'
    counter = 0
    gg_string = ''
    if 'GG'==gentype:
        for jets in jetloop:
            if not use_decays:
                gg_string += '%s p p > go go %s $ ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,jets)
                counter = counter+1
                starter = 'add process'
                continue
            decaylist = get_decays(gentype='GG',decaytype=decaytype,ptype=['go','go'])
            for decay in decaylist:
                gg_string += '%s p p > go go %s $ ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ %s \n'%(starter,jets,decay)
                counter = counter+1
                starter = 'add process'

    starter = 'generate'
    counter = 0
    cc_string = ''
    if 'CC'==gentype:
        for jets in jetloop:
            for sign1 in ['+','-']:
                for sign2 in ['+','-']:
                    if not use_decays:
                        cc_string += '%s p p > x1%s x1%s %s $ go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,sign1,sign2,jets)
                        counter = counter+1
                        starter = 'add process'
                        continue
                    decaylist = get_decays(gentype='CC',decaytype=decaytype,ptype=['x1%s'%(sign1),'x1%s'%(sign2)])[0]
                    for decay in decaylist:
                        cc_string += '%s p p > x1%s x1%s %s $ go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ %s \n'%(starter,sign1,sign2,jets,decay)
                        counter = counter+1
                        starter = 'add process'
    starter = 'generate'
    counter = 0
    n2n3_string = ''
    if 'N2N3'==gentype:
        for jets in jetloop:
            n2n3_string += '%s p p > n2 n3 %s $ go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,jets)
            counter = counter+1
            starter = 'add process'
    starter = 'generate'
    counter = 0
    c1n2_string = ''
    if 'C1N2'==gentype:
      for sign in ['+','-']:
        for jets in jetloop:
            c1n2_string += '%s p p > x1%s n2 %s $ go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,sign,jets)
            counter = counter+1
            starter = 'add process'
    starter = 'generate'
    counter = 0
    bb_string = ''
    if 'BB'==gentype:
        for jets in jetloop:
            if not use_decays:
                bb_string += '%s p p > b1 b1~ %s $ go ul ur dl dr cl cr sl sr t1 t2 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b2~ \n'%(starter,jets)
                counter = counter+1
                starter = 'add process'
                continue
            decaylist = get_decays(gentype='BB',decaytype=decaytype,ptype=['b1','b1~'])
            for decay in decaylist:
                bb_string += '%s p p > b1 b1~ %s %s \n'%(starter,jets,decay)
                counter = counter+1
                starter = 'add process'
    starter = 'generate'
    counter = 0
    Scharm_string = ''
    if 'Scharm'==gentype:
        squarks = ['cl','cr']
        for jets in jetloop:
            for s in squarks:
                for t in squarks:
                    Scharm_string += '%s p p > %s %s~ %s $ go \n'%(starter,s,t,jets)
                    counter = counter+1
                    starter = 'add process'
    starter = 'generate'
    counter = 0
    tt_string = ''
    if 'TT'==gentype:
        for jets in jetloop:
            if not use_decays:
                tt_string += '%s p p > t1 t1~ %s $ go ul ur dl dr cl cr sl sr t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ b1~ t2~ b2~ \n'%(starter,jets)
                counter = counter+1
                starter = 'add process'
                continue
            decaylist = get_decays(gentype='TT',decaytype=decaytype,ptype=['t1','t1~'])
            for decay in decaylist:
                tt_string += '%s p p > t1 t1~ %s %s $ go ul ur dl dr cl cr sl sr t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ b1~ t2~ b2~ \n'%(starter,jets,decay)
                counter = counter+1
                starter = 'add process'

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
        'C1N2' : c1n2_string ,
      }

    if not SLHAonly:
        if not gentype in processes.keys():
            mglog.error('No idea how to deal with the simplified models for'+gentype+'.  Sorry!')
            return -1,''
        if processes[gentype] == '':
            mglog.error('No processes found for the set up you requested: '+str(gentype)+' '+str(decaytype)+' '+str(masses))
            return -1,''
        mglog.info('Will run MadGraph over the process:')
        mglog.info(str(processes[gentype]))

    xqcut=500 # default


    if 'Scharm'==gentype:
        if masses['1000004']<xqcut*4.: xqcut = masses['1000004']*0.25
    elif 'N2N3'==gentype:
        if masses['1000025']<xqcut*4.: xqcut = masses['1000025']*0.25
    elif 'C1N2'==gentype:
        if masses['1000024']<xqcut*4.: xqcut = masses['1000024']*0.25
    elif 'T' in gentype:
        if masses['1000006']<xqcut*4.: xqcut = masses['1000006']*0.25
        if use_decays: xqcut = 10
    elif 'B' in gentype:
        if masses['1000005']<xqcut*4.: xqcut = masses['1000005']*0.25
    elif 'S' in gentype:
        if masses['1000002']<xqcut*4.: xqcut = masses['1000002']*0.25
    elif 'G' in gentype:
        if masses['1000021']<xqcut*4.: xqcut = masses['1000021']*0.25        
    elif 'C' in gentype:
        if masses['1000024']<xqcut*4.: xqcut = masses['1000024']*0.25


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

    if not SLHAonly:
        proccard = subprocess.Popen(['get_files','-data','proc_card_mg5.SM.dat'])
        proccard.wait()

        if not os.access('proc_card_mg5.SM.dat',os.R_OK):
            mglog.error('Could not retrieve process card template')
            return -1,''
        if os.access('proc_card_mg5.dat',os.R_OK):
            mglog.error('Old process card in the current directory.  Dont want to clobber it.  Please move it first.')
            return -1,''
        oldcard = open('proc_card_mg5.SM.dat','r')
        newcard = open('proc_card_mg5.dat','w')
        for line in oldcard.readlines():
            if 'PROCESS' in line: newcard.write(processes[gentype])
            else: newcard.write(line)
        oldcard.close()
        newcard.close()

        # Generate the new process!
        thedir = new_process(card_loc='proc_card_mg5.dat')
        if 1==thedir:
            mglog.error('Error in process generation!')
            return -1,''

        # See if things are where we expected them to be...
        if not os.access(thedir,os.R_OK):
            mglog.error('Could not find process directory '+thedir+' !!  Bailing out!!')
            return -1,''
        else:
            mglog.info('Using process directory '+thedir)

    # Grab the param card and move the new masses into place
    offshellness = ''
    #if 'onestepCC' in decay:
    #    if '1000022' in masses and '1000024' in masses:
    #        if masses['1000024']-masses['1000022']<100:
    #            # Offshell decay!
    #            offshellness = 'offshell.'
    #if gentype=='TT' and decaytype=='onestepBB':
    #    if '1000022' in masses and '1000024' in masses:
    #        if masses['1000024']-masses['1000022']<81.0:
    #            offshellness = 'offshell.'

    if SLHAexactCopy:
        str_masses = '_'.join(sorted(masses.values()))
        str_param_card = 'param_card.SM.%s.%s.dat'%(gentype,decaytype,str_masses)
        proc_paramcard = subprocess.Popen(['get_files','-data',str_param_card])
        proc_paramcard.wait()
        if not os.access(str_param_card,os.R_OK):
            mglog.info('Could not get param card '+str_param_card)
    else:
        build_param_card(param_card_old='param_card.SM.%s.%s.%sdat'%(gentype,decaytype,offshellness),param_card_new='param_card.dat',masses=masses,decays=decays)

    if SLHAonly:
        mglog.info('Not running generation - only setting up SLHA file')
        return [xqcut,'madgraph.%i.madgraph_SM_%s_%s_%s._00001.events.tar.gz'%(run_number,gentype,decaytype,stringy)]

    # Grab the run card and move it into place
    if 'phot' in gentype:
        build_run_card(run_card_old='run_card.SMphot.dat',run_card_new='run_card.dat',
                   xqcut=xqcut,nevts=nevts,rand_seed=rand_seed,beamEnergy=beamEnergy, scalefact=scalefact, alpsfact=alpsfact)

    else:
        build_run_card(run_card_old='run_card.SM.dat',run_card_new='run_card.dat',
                       xqcut=xqcut,nevts=nevts,rand_seed=rand_seed,beamEnergy=beamEnergy, scalefact=scalefact, alpsfact=alpsfact)

    # Generate events!
    if gridpackDirName is not None:
        if writeGridpack==False:
            mglog.info('Generateing events from gridpack')
            if generate_from_gridpack(run_name='Test',gridpack_dir=gridpackDirName,nevents=int(nevts),random_seed=rand_seed):
                mglog.error('Error generating events!')
                return -1
            thedir=gridpackDirName
        else:
            mglog.error('Wrong combination of arguments! writeGridpack='+str(writeGridpack)+' gridpackDirName='+str(gridpackDirName))
            return -1
    else:
        if generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir=thedir,grid_pack=writeGridpack):
            mglog.error('Error generating events!')
            return -1

    # Move output files into the appropriate place, with the appropriate name
    if arrange_output(run_name='Test',proc_dir=thedir,outputDS='madgraph.%i.madgraph_SM_%s_%s_%s._00001.events.tar.gz'%(run_number,gentype,decaytype,stringy),skip_events=skip_events):
        mglog.error('Error arranging output dataset!')
        return -1

    if not keepOutput:
        mglog.info('Removing process directory...')
        shutil.rmtree(thedir,ignore_errors=True)

    mglog.info('All done generating events!!')
    return [xqcut,'madgraph.%i.madgraph_SM_%s_%s_%s._00001.events.tar.gz'%(run_number,gentype,decaytype,stringy)]


def SUSY_StrongSM_SG_Generation(run_number=100000,gentype='SS',masses=None,stringy='100_500_900',
                                nevts=60000,rand_seed=1234, njets=1, skip_events=0, syst_mod=None,
                                SLHAonly=False, beamEnergy=3500.,keepOutput=False, writeGridpack=False,gridpackDirName=None):
    if not os.environ.has_key('MADGRAPH_DATA'):
        os.environ['MADGRAPH_DATA']=os.getcwd()
        mglog.warning('Setting your MADGRAPH_DATA environmental variable to the working directory')
    # Set up production and decay strings
    if nevts<1000 or nevts>10000000:
        mglog.error('Bad idea to generate '+str(nevts)+' events.  MadGraph wont complain, but the job will never end.  Bailing out now.')
        return -1,''

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
        return -1,''

    starter = 'generate'
    squarks = ['ul','dl','cl','sl','ur','dr','cr','sr']
    counter = 0
    ss_string = ''
    sg_string = ''
    ssb_string = ''
    if 'S' in gentype or 'ALL' in gentype:
        for jets in jetloop:
            for s in squarks:
                sg_string += '%s p p > %s go %s $ go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,s,jets)
                counter = counter+1
                sg_string += '%s p p > %s~ go %s $ go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,s,jets)

                counter = counter+1
                for t in squarks:
                    ssb_string += '%s p p > %s %s~ %s $ go \n'%(starter,s,t,jets)
                    counter = counter+1
                    ss_string += '%s p p > %s %s %s $ go \n'%(starter,s,t,jets)
                    counter = counter+1
                    starter = 'add process'

    starter = 'generate'
    counter = 0
    gg_string = ''
    if 'GG'==gentype or 'ALL' in gentype:
        for jets in jetloop:
            gg_string += '%s p p > go go %s $ ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,jets)
            counter = counter+1
            starter = 'add process'

    processes = {
        'SS' : ss_string ,
        'GG' : gg_string ,
        'SSB' : ssb_string ,
        'SG' : sg_string ,
        'GS' : sg_string ,
        'ALL' : ss_string+'\n'+gg_string+'\n'+ssb_string+'\n'+sg_string
      }

    if not SLHAonly:
        if not gentype in processes.keys():
            mglog.error('No idea how to deal with the simplified models for '+gentype+'.  Sorry!')
            return -1,''
        mglog.info('Will run MadGraph over the process:')
        mglog.info(str(processes[gentype]))

    xqcut=500 # default
    if 'G' in gentype or 'ALL' in gentype:
        if masses['1000021']<xqcut*4.: xqcut = masses['1000021']*0.25
    if 'S' in gentype or 'ALL' in gentype:
        if masses['1000002']<xqcut*4.: xqcut = masses['1000002']*0.25
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

    if not SLHAonly:
        proccard = subprocess.Popen(['get_files','-data','proc_card_mg5.SM.dat'])
        proccard.wait()

        if not os.access('proc_card_mg5.SM.dat',os.R_OK):
            mglog.error('Could not retrieve process card template')
            return -1,''
        if os.access('proc_card_mg5.dat',os.R_OK):
            mglog.error('Old process card in the current directory.  Dont want to clobber it.  Please move it first.')
            return -1,''
        oldcard = open('proc_card_mg5.SM.dat','r')
        newcard = open('proc_card_mg5.dat','w')
        for line in oldcard.readlines():
            if 'PROCESS' in line: newcard.write(processes[gentype])
            else: newcard.write(line)
        oldcard.close()
        newcard.close()

        # Generate the new process!
        thedir = new_process(card_loc='proc_card_mg5.dat')
        if 1==thedir:
            mglog.error('Error in process generation!')
            return -1,''

        # See if things are where we expected them to be...
        if not os.access(thedir,os.R_OK):
            mglog.error('Could not find process directory',thedir,'!!  Bailing out!!')
            return -1,''
        else:
            mglog.info('Using process directory '+thedir)

    # Grab the param card and move the new masses into place
    build_param_card(param_card_old='param_card.SM.SG.dat',param_card_new='LH.dat',masses=masses,decays={})

    mglog.info('Running MSSMCalc')
    runMSSMCalc = subprocess.Popen([ os.environ['MADPATH']+'/Calculators/mssm/MSSMCalc'])
    runMSSMCalc.wait()
    if not os.access('param_card.dat',os.R_OK):
        mglog.error('Problem generating param card!!  Will bail out...')
        return -1,''

    if SLHAonly:
        mglog.info('Not running generation - only setting up SLHA file')
        return [xqcut,'madgraph.%i.madgraph_SM_SG_%s_%s_%s._00001.events.tar.gz'%(run_number,gentype,'direct',stringy)]

    # Grab the run card and move it into place
    build_run_card(run_card_old='run_card.SM.dat',run_card_new='run_card.dat',
                   xqcut=xqcut,nevts=nevts,rand_seed=rand_seed,beamEnergy=beamEnergy,scalefact=scalefact,alpsfact=alpsfact)

    # Generate events!
    if gridpackDirName is not None:
        if writeGridpack==False:
            mglog.info('Generateing events from gridpack')
            if generate_from_gridpack(run_name='Test',gridpack_dir=gridpackDirName,nevents=int(nevts),random_seed=rand_seed):
                mglog.error('Error generating events!')
                return -1
            thedir=gridpackDirName
        else:
            mglog.error('Wrong combination of arguments! writeGridpack='+str(writeGridpack)+' gridpackDirName='+str(gridpackDirName))
            return -1
    else:
        if generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir=thedir,grid_pack=writeGridpack):
            mglog.error('Error generating events!')
            return -1

    # Move output files into the appropriate place, with the appropriate name
    if arrange_output(run_name='Test',proc_dir=thedir,outputDS='madgraph.%i.madgraph_SM_SG_%s_%s_%s._00001.events.tar.gz'%(run_number,gentype,'direct',stringy),skip_events=skip_events):
        mglog.error('Error arranging output dataset!')
        return -1

    if not keepOutput:
        mglog.info('Removing process directory...')
        shutil.rmtree(thedir,ignore_errors=True)

    mglog.info('All done with generation!!')
    return [xqcut,'madgraph.%i.madgraph_SM_SG_%s_%s_%s._00001.events.tar.gz'%(run_number,gentype,'direct',stringy)]



def SUSY_StrongSM_SG2NC_Generation(run_number=100000,gentype='SS',masses=None,stringy='100_500_900',
                                nevts=60000,rand_seed=1234, njets=1, skip_events=0, syst_mod=None,
                                SLHAonly=False, beamEnergy=3500.,keepOutput=False):
    if not os.environ.has_key('MADGRAPH_DATA'):
        os.environ['MADGRAPH_DATA']=os.getcwd()
        mglog.warning('Setting your MADGRAPH_DATA environmental variable to the working directory')
    # Set up production and decay strings
    if nevts<1000 or nevts>100000:
        mglog.error('Bad idea to generate '+str(nevts)+' events.  MadGraph will complain.  Bailing out now.')
        return -1,''

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
        return -1,''

    starter = 'generate'
    squarks = ['ul','dl','cl','sl','ur','dr','cr','sr']
    counter = 0
    ss_string = ''
    sg_string = ''
    ssb_string = ''
    if 'S' in gentype:
        for jets in jetloop:
            for s in squarks:
                sg_string += '%s p p > %s go %s $ go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,s,jets)
                counter = counter+1
                sg_string += '%s p p > %s~ go %s $ go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,s,jets)
                counter = counter+1
                for t in squarks:
                    ssb_string += '%s p p > %s %s~ %s $ go \n'%(starter,s,t,jets)
                    counter = counter+1
                    ss_string += '%s p p > %s %s %s $ go \n'%(starter,s,t,jets)
                    counter = counter+1
                    starter = 'add process'

    starter = 'generate'
    counter = 0
    gg_string = ''
    if 'GG'==gentype or 'ALL' in gentype:
        for jets in jetloop:
            gg_string += '%s p p > go go %s $ ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,jets)
            counter = counter+1
            starter = 'add process'


    starter = 'generate'
    squarks = ['ul','dl','cl','sl','ur','dr','cr','sr']
    counter = 0
    ss_string = ''
    sg_string = ''
    ssb_string = ''
    if 'S' in gentype:
        for jets in jetloop:
            for s in squarks:
                sg_string += '%s p p > %s go %s $ go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,s,jets)
                counter = counter+1
                sg_string += '%s p p > %s~ go %s $ go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,s,jets)
                counter = counter+1
                for t in squarks:
                    ssb_string += '%s p p > %s %s~ %s $ go \n'%(starter,s,t,jets)
                    counter = counter+1
                    ss_string += '%s p p > %s %s %s $ go \n'%(starter,s,t,jets)
                    counter = counter+1
                    starter = 'add process'

    starter = 'generate'
    counter = 0
    gg_string = ''
    if 'GG'==gentype:
        for jets in jetloop:
            gg_string += '%s p p > go go %s $ ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~ \n'%(starter,jets)
            counter = counter+1
            starter = 'add process'

    starter = 'generate'
    squarks = ['ul','dl','cl','sl','ur','dr','cr','sr']
    counter = 0
    ss_string = ''
    sg_string = ''
    ssb_string = ''
    gg_string = ''
    for jets in jetloop:
        for s in squarks:
            sg_string += '%s p p > %s go %s \n'%(starter,s,jets)
            counter = counter+1
            sg_string += '%s p p > %s~ go %s \n'%(starter,s,jets)
            counter = counter+1
            for t in squarks:
                ssb_string += '%s p p > %s %s~ %s \n'%(starter,s,t,jets)
                counter = counter+1
                ss_string += '%s p p > %s %s %s \n'%(starter,s,t,jets)
                counter = counter+1
                starter = 'add process'
        gg_string += '%s p p > go go %s \n'%(starter,jets)
        counter = counter+1
        starter = 'add process'

        

    processes = {
        'SS' : ss_string ,
        'GG' : gg_string ,
        'SSB' : ssb_string ,
        'SG' : sg_string ,
        'GS' : sg_string ,
        'ALL' : ss_string+'\n'+gg_string+'\n'+ssb_string+'\n'+sg_string
      }

    if not SLHAonly:
        if not gentype in processes.keys():
            mglog.error('No idea how to deal with the simplified models for '+gentype+'.  Sorry!')
            return -1,''
        mglog.info('Will run MadGraph over the process:')
        mglog.info(str(processes[gentype]))

    xqcut=500 # default
    if 'G' in gentype or 'ALL' in gentype:
        if masses['1000021']<xqcut*4.: xqcut = masses['1000021']*0.25
    if 'S' in gentype or 'ALL' in gentype:
        if masses['1000002']<xqcut*4.: xqcut = masses['1000002']*0.25
    if syst_mod is not None and 'qup' in syst_mod.lower(): xqcut = xqcut*2.
    elif syst_mod is not None and 'qdown' in syst_mod.lower(): xqcut = xqcut*0.5
    mglog.info('For matching, will use xqcut of '+str(xqcut))

    if not SLHAonly:
        proccard = subprocess.Popen(['get_files','-data','proc_card_mg5.SM.dat'])
        proccard.wait()

        if not os.access('proc_card_mg5.SM.dat',os.R_OK):
            mglog.error('Could not retrieve process card template')
            return -1,''
        if os.access('proc_card_mg5.dat',os.R_OK):
            mglog.error('Old process card in the current directory.  Dont want to clobber it.  Please move it first.')
            return -1,''
        oldcard = open('proc_card_mg5.SM.dat','r')
        newcard = open('proc_card_mg5.dat','w')
        for line in oldcard.readlines():
            if 'PROCESS' in line: newcard.write(processes[gentype])
            else: newcard.write(line)
        oldcard.close()
        newcard.close()

        # Generate the new process!
        thedir = new_process(card_loc='proc_card_mg5.dat')
        if 1==thedir:
            mglog.error('Error in process generation!')
            return -1,''

        # See if things are where we expected them to be...
        if not os.access(thedir,os.R_OK):
            mglog.error('Could not find process directory',thedir,'!!  Bailing out!!')
            return -1,''
        else:
            mglog.info('Using process directory '+thedir)

    # Grab the param card and move the new masses into place
    build_param_card(param_card_old='param_card.SM.SG.dat',param_card_new='LHA.dat',masses=masses,decays={})

    mglog.info('Running MSSMCalc')
    runMSSMCalc = subprocess.Popen([ os.environ['MADPATH']+'/Calculators/mssm/MSSMCalc'])
    runMSSMCalc.wait()
    if not os.access('param_card.dat',os.R_OK):
        mglog.error('Problem generating param card!!  Will bail out...')
        return -1,''

    if SLHAonly:
        mglog.info('Not running generation - only setting up SLHA file')
        return [xqcut,'madgraph.%i.madgraph_SM_SG2NC_%s_%s_%s._00001.events.tar.gz'%(run_number,gentype,'direct',stringy)]

    # Grab the run card and move it into place
    build_run_card(run_card_old='run_card.SM.dat',run_card_new='run_card.dat',
                   xqcut=xqcut,nevts=nevts,rand_seed=rand_seed,beamEnergy=beamEnergy,scalefact=scalefact,alpsfact=alpsfact)

    # Generate events!
    if generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir=thedir):
        mglog.error('Error generating events!')
        return -1

    # Move output files into the appropriate place, with the appropriate name
    if arrange_output(run_name='Test',proc_dir=thedir,outputDS='madgraph.%i.madgraph_SM_SG2NC_%s_%s_%s._00001.events.tar.gz'%(run_number,gentype,'direct',stringy),skip_events=skip_events):
        mglog.error('Error arranging output dataset!')
        return -1

    if not keepOutput:
        mglog.info('Removing process directory...')
        shutil.rmtree(thedir,ignore_errors=True)

    mglog.info('All done with generation!!')
    return [xqcut,'madgraph.%i.madgraph_SM_SG2NC_%s_%s_%s._00001.events.tar.gz'%(run_number,gentype,'direct',stringy)]

def build_param_card(param_card_old=None,param_card_new='param_card.dat',masses={},decays={}):
    """Build a new param_card.dat from an existing one."""
    # Grab the old param card and move it into place
    paramcard = subprocess.Popen(['get_files','-data',param_card_old])
    paramcard.wait()
    if not os.access(param_card_old,os.R_OK):
        mglog.info('Could not get param card '+param_card_old)
    if os.access(param_card_new,os.R_OK):
        mglog.error('Old param card in the current directory. Dont want to clobber it. Please move it first.')
        return -1
    oldcard = open(param_card_old,'r')
    newcard = open(param_card_new,'w')
    edit = False
    blockName = ""
    for line in oldcard:
        if (line.strip().startswith('BLOCK') or 
            line.strip().startswith('DECAY')) and len(line.strip().split()) > 1: 
            if edit and blockName == 'DECAY': edit = False # Start a new DECAY block
            pos = 0 if line.strip().startswith('DECAY') else 1
            blockName = line.strip().split()[pos]
        pdgid = None
        if blockName == 'MASS' and len(line.strip().split()) > 0:
            pdgid = line.strip().split()[0]
        if blockName == 'DECAY' and len(line.strip().split()) > 1:
            pdgid = line.strip().split()[1]
        if pdgid != None and blockName == 'MASS' and masses.has_key(pdgid):
            newcard.write('   %s    %s  # \n'%(pdgid,str(masses[pdgid])))
            mglog.info('   %s    %s  #'%(pdgid,str(masses[pdgid])))
            edit = False
        elif pdgid != None and blockName == 'DECAY' and decays.has_key(pdgid):
            for newline in decays[pdgid].splitlines():
                newcard.write(newline+'\n')
                mglog.info(newline)
            edit = True
        elif not edit:
            newcard.write(line)
            #mglog.info(line)
    oldcard.close()
    newcard.close()
    return param_card_new

def build_run_card(run_card_old='run_card.SM.dat',run_card_new='run_card.dat',
                   xqcut=500.,nevts=60000,rand_seed=1234,beamEnergy=4000.,
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
        if ' xqcut ' in line:
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
                if (' '+ak.strip()+' ') in line and '=' in line:
                    newcard.write( ' '+str(extras[ak])+'    ='+line.split('=')[1] )
            else: # Fell through the loop
                newcard.write(line)
    oldcard.close()
    newcard.close()
    return run_card_new

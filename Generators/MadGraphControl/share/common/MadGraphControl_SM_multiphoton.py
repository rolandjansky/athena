from MadGraphControl.MadGraphUtils import *

# General settings
nevents=4*runArgs.maxEvents if runArgs.maxEvents>0 else 20000
mode=0
nJobs=1
gridpack_dir=None
gridpack_mode=False
cluster_type=None
cluster_queue=None

# Merging settings
maxjetflavor=5
ickkw=0
nJetMax=1
ktdurham=30      # 25ns change 30 -> 20
dparameter=0.2   # 25ns change 0.4 -> 0.1

if '3photons' in  str(runArgs.jobConfig):
    mgproc="""
generate p p > a a a @0
add process p p > a a a j @1
"""
    name='3photon'
    process="pp>a a a"
elif '4photons' in str(runArgs.jobConfig):
    mgproc="""
generate p p > a a a a @0
add process p p > a a a a j @1
"""
    name='4photon'
    process="pp>a a a a"
else: 
    raise RuntimeError("JO %s not recognized: " %runArgs.jobConfig)

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
"""+mgproc+"""
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version'    : '3.0',
           'cut_decays'     : 'F', 
           'pdlabel'        : "'lhapdf'",
           'lhaid'          : 260000,  
           'maxjetflavor'   : maxjetflavor,
           'asrwgtflavor'   : maxjetflavor,
           'ickkw'          : 0,
           'ptj'            : 20,      
           'pta'            : 10,
           'ptb'            : 20,      
           'mmll'           : 0,      
           'mmjj'           : 0,
           'drjj'           : 0.1,
           'drbb'           : 0.1,
           'drll'           : 0,
           'drjl'           : 0.,       
           'draj'           : 0.05,
           'dral'           : 0.,
           'drab'           : 0.05,
           'draa'           : 0.05,
           'ptl'            : 10,
           'etaa'           : 5,
           'etal'           : 10,
           'etab'           : 6,
           'etaj'           : 6,
           'ktdurham'       : ktdurham,    
           'dparameter'     : dparameter  }
##########################################################################################
process_dir = new_process(grid_pack=gridpack_dir)
build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)

print_cards()
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,njobs=nJobs,proc_dir=process_dir,
         grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,cluster_type=cluster_type,cluster_queue=cluster_queue,
         nevents=nevents,random_seed=runArgs.randomSeed)

arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')

if 'ATHENA_PROC_NUMBER' in os.environ:
    print 'Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.'
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot                                                                                                                                              
    if not hasattr(opts,'nprocs'): print 'Did not see option!'
    else: opts.nprocs = 0
    print opts

#### Shower 
evgenConfig.description = 'MadGraph SM production of 3 and 4 photons with up to one additional jet'
evgenConfig.keywords+=['photon','SM']
evgenConfig.inputfilecheck = stringy
evgenConfig.contact  = [ "amoroso@cern.ch", "adam.jinaru@cern.ch" ]
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")    
include("Pythia8_i/Pythia8_MadGraph.py")

PYTHIA8_nJetMax=nJetMax
PYTHIA8_TMS=ktdurham
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=process                                                      
PYTHIA8_nQuarksMerge=maxjetflavor
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")                         


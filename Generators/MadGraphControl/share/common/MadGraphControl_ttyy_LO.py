from MadGraphControl.MadGraphUtils import *



# General settings
nevents=int(1.1*runArgs.maxEvents)
mode=0
cluster_type = 'None'
cluster_queue = 'None'
nJobs=1
gridpack_dir=None
gridpack_mode=False

# MG particle cuts
mllCut=-1
ptjCut=0
ptlCut=0
xptlCut=5
ptaCut=20
drajCut=0.2
dralCut=0.2
etajCut=-1
etabCut=-1
myyCut=90
myyMaxCut=175
maxjetflavor=5
dyn_scale = '3'


# Shower/merging settings
parton_shower='PYTHIA8'

### DSID lists (extensions can include systematics samples)
allhad=[345869]
noallhad=[345868]

# allhad channel and noallhad channel
if runArgs.runNumber in allhad:
    mgproc="generate p p > t t~ > uc ds~ b ds uc~ b~ a a QCD=2 QED=6"



    name='ttgamgam_allhad'
    process="pp > ttyy"
    keyword=['SM', 'ttgammagamma', 'allHadronic']

    nevents=50000
    gridpack_mode=True
    gridpack_dir='madevent/'


    mode=1
    cluster_type = 'condor'
    cluster_queue = 'None'
    nJobs=100



elif runArgs.runNumber in noallhad:
    mgproc ="""generate p p > t t~ > l+ vl b ds uc~ b~ a a QCD=2 QED=6 @1\n"""
    mgproc +="""add process p p > t t~ > uc ds~ b l- vl~ b~ a a QCD=2 QED=6\n"""
    mgproc +="""add process p p > t t~ > l+ vl l- vl~ b b~ a a QCD=2 QED=6"""
    name='ttgamgam_noallhad'
    process="pp > ttyy"
    keyword=['SM', 'ttgammagamma', 'leptonic']

    nevents= 50000
    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type = 'condor'
    cluster_queue = 'None'
    nJobs=100

else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
define uc = u c
define uc~ = u~ c~
define ds = d s
define ds~ = d~ s~
"""+mgproc+"""
output -f""")
fcard.close()

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")


process_dir = new_process(grid_pack=gridpack_dir)

lhaid=247000
pdflabel='lhapdf'
#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version'   :'3.0',
           'pdlabel'       :"'lhapdf'",
           'lhaid'         : lhaid,
           'pdflabel'      : "'"+pdflabel+"'",
           'maxjetflavor'  : maxjetflavor,
           'parton_shower' : parton_shower,
           'mll_sf'        : mllCut,
           'pta'           : ptaCut,
           'ptl'           : ptlCut,
           'xptl'          : xptlCut,
           'ptj'           : ptjCut,
           'etal'          : 5.0,
           'etaj'          : 5.0,
           'etaa'          : 5.0,
           'etaj'          : etajCut,
           'etab'          : etabCut,
           'drjj'          : 0.0,
           'drjl'          : 0.0,
           'drll'          : 0.0,
           'draa'          : 0.0,
           'draj'          : 0.2,
           'dral'          : 0.2,
           'mmaa'          : myyCut,
           'mmaamax'       : myyMaxCut,
           'dynamical_scale_choice': dyn_scale,
           'use_syst'      :'T',
           'sys_scalefact' : '1 0.5 2',
           'sys_pdf'       : 'NNPDF23_lo_as_0130_qed'
           }


runName='run_01'





print  "checking process_dir for old card: ", process_dir
print "old run card is:", get_default_runcard(proc_dir=process_dir)


build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)


print_cards()


generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode, njobs=nJobs,grid_pack=gridpack_mode, gridpack_dir=gridpack_dir, proc_dir=process_dir,run_name=runName, nevents=nevents, cluster_type=cluster_type,cluster_queue=cluster_queue, random_seed=runArgs.randomSeed)


arrange_output(run_name=runName,proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)

############################
# Shower JOs will go here

import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts

evgenConfig.generators = ['MadGraph', 'Pythia8', 'EvtGen']
evgenConfig.contact  = [ "a.hasib@cern.ch" ]
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=keyword
evgenConfig.inputfilecheck = stringy
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")


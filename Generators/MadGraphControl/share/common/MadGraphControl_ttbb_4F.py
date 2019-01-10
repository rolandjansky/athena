from MadGraphControl.MadGraphUtils import *
import fileinput
import subprocess
from AthenaCommon import Logging
JOlog = Logging.logging.getLogger('ttbb4FJobOption')
JOlog.info(" Loading MadGraphControl file for tt+bb 4F ")

# General settings
nevents=1.1*runArgs.maxEvents if runArgs.maxEvents>0 else 5500
#nevents       = 1.1*runArgs.maxEvents
mode          = 0
gridpack_dir  = None
gridpack_mode = False

# MG Particle cuts                                                                                                
mllcut=0
bwcut=50

# Shower/merging settings                                                                                                
maxjetflavor=4
parton_shower='PYTHIA8'

# DSID dictionary - Will allow expansion into a control file in the future
# key  - DSID
# arg1 - alphaS values in Pythi8 shower
ttbb_4fl={410265:["0.127"],410268:["0.127"],# singlelep - A14 nominal alphaS value
          410266:["0.127"],410269:["0.127"],# dileptonic- A14 nominal alphaS value
          410267:["0.127"],410270:["0.127"]} # allhadronic - A14 nominal alphaS value

# We can define the madspin decay pattern based on DSID
singlelep = [410265,410268]
dilepton  = [410266,410269]
allhad    = [410267,410270]

# Using gridpacks, so make sure there is a check on the config file
if hasattr(runArgs,'inputGenConfFile'):
    evgenConfig.inputconfcheck = "ttbb"

if runArgs.runNumber in ttbb_4fl:
    name          = 'ttbb4F'
    process       = "pp>ttbb"
    gridpack_mode = True
    gridpack_dir  = 'madevent/'

else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

msdecay   = ""
nleptons  = -2

if runArgs.runNumber in singlelep:
    nleptons = 1
    nevents  = 2.5*nevents # Filter efficiency (~1/0.45)
    msdecay  = "decay t > w+ b, w+ > all all \ndecay t~ > w- b~, w- > all all"
elif runArgs.runNumber in dilepton:
    nleptons = 2
    msdecay  = "define lv = e+ mu+ ta+ ve vm vt e- mu- ta- ve~ vm~ vt~ \ndecay t > w+ b, w+ > lv lv \ndecay t~ > w- b~, w- > lv lv"
elif runArgs.runNumber in allhad:
    nleptons = 0
    msdecay  = "decay t > w+ b, w+ > j j \ndecay t~ > w- b~, w- > j j"
else:
    raise RuntimeError("Do not know which decay is required for this DSID")

JOlog.info(" MadSpin decay defined as "+msdecay)

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model loop_sm
define p = p
define j = p
generate p p > t t~ b b~ [QCD]
output pp_ttbb_4F_NLO_13TeV_PY8_MadSpin
""")
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")


lhaid=260400
pdflabel='lhapdf'

#Fetch default LO run_card.dat and set parameters                                                                             
extras = { 'lhaid'         : lhaid,
           'pdlabel'       : "'"+pdflabel+"'",
           'maxjetflavor'  : maxjetflavor,
           'parton_shower' : parton_shower,
           'mll_sf'        : mllcut,
           'mll'           : mllcut,
           'bwcutoff'      : bwcut}

doSyst=True

if doSyst:
    lhe_version=3
    extras.update({'reweight_scale': '.true.',
                   'rw_Rscale_down':  0.5,
                   'rw_Rscale_up'  :  2.0,
                   'rw_Fscale_down':  0.5,
                   'rw_Fscale_up'  :  2.0,
                   'reweight_PDF'  : '.true.',
                   'PDF_set_min'   : 260401,
                   'PDF_set_max'   : 260500})

else:
    lhe_version=1
    extras.update({'reweight_scale': '.false.',
                   'reweight_PDF'  : '.false.'})


# Location is dependent on the method
madspin_card_loc = 'madspin_card.dat'

if not hasattr(runArgs, 'inputGenConfFile'):
    fMadSpinCard = open('madspin_card.dat','w')
    fMadSpinCard.write('import Events/Test/events.lhe.gz\n')
    fMadSpinCard.write('set ms_dir MadSpin\n')
else:
    os.unlink(gridpack_dir+'Cards/madspin_card.dat')
    fMadSpinCard = open(gridpack_dir+'Cards/madspin_card.dat','w')
    fMadSpinCard.write('import '+gridpack_dir+'Events/Test/events.lhe.gz\n')
# We cannot use this directory if we wish to use different ME for the decay
# Removing this line will ensure that the MS decay is correct for the process
# and is uncorrelated to the actual gridpack production in this case
#    fMadSpinCard.write('set ms_dir '+gridpack_dir+'MadSpin\n')
    fMadSpinCard.write('set seed '+str(10000000+int(runArgs.randomSeed))+'\n')
fMadSpinCard.write('set BW_cut '+str(int(bwcut))+'\n')
fMadSpinCard.write('''set Nevents_for_max_weigth 250 # number of events for the estimate of the max. weight (default: 75)
set max_weight_ps_point 1000  # number of PS to estimate the maximum for each event (default: 400)
%s
launch'''%(msdecay))
fMadSpinCard.close()



process_dir = new_process(grid_pack=gridpack_dir)

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)

get_param_file = subprocess.Popen(['get_files','-data', 'aMcAtNlo_param_card_tt.dat'])
get_param_file.wait()
import os, shutil
if not os.path.exists('aMcAtNlo_param_card_tt.dat'):
    raise RuntimeError("Cannot find aMcAtNlo_param_card_tt.dat")

scaleToCopy      = 'setscales_ttbb4F_muRgeom.f'
scaleDestination = process_dir+'/SubProcesses/setscales.f'
scalefile        = subprocess.Popen(['get_files','-data',scaleToCopy])
scalefile.wait()

if not os.access(scaleToCopy,os.R_OK):
  raise RuntimeError("ERROR: Could not get %s"%(scaleToCopy))
shutil.copy(scaleToCopy,scaleDestination)

print_cards()

generate(required_accuracy=0.001,run_card_loc='run_card.dat',param_card_loc='aMcAtNlo_param_card_tt.dat',proc_dir=process_dir,mode=mode,madspin_card_loc=madspin_card_loc,grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,gridpack_compile=True,random_seed=runArgs.randomSeed,nevents=nevents)
outputDS=arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz',lhe_version=lhe_version,saveProcDir=True)

# Shower
evgenConfig.description    = 'aMcAtNlo_ttbb_4F control'
evgenConfig.contact        = ['maria.moreno.llacer@cern.ch']
evgenConfig.keywords      += ['ttbar','top']
runArgs.inputGeneratorFile=outputDS

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")
#Removing shower weights
#include("Pythia8_i/Pythia8_ShowerWeights.py")

testSeq.TestHepMC.MaxVtxDisp = 100000000

if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot                                                                                              
    if not hasattr(opts,'nprocs'):
        mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts
    
# Matching settings
genSeq.Pythia8.Commands += ["SpaceShower:alphaSvalue    = "+ttbb_4fl[runArgs.runNumber][0], # Dependent on DSID
                            "TimeShower:alphaSvalue     = "+ttbb_4fl[runArgs.runNumber][0] # Dependent on DSID
                            ]

# Bit more logging info
JOlog.info("SpaceShower:alphaSvalue   -> "+ttbb_4fl[runArgs.runNumber][0])
JOlog.info("TimeShower:alphaSvalue   -> "+ttbb_4fl[runArgs.runNumber][0])

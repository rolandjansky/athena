from MadGraphControl.MadGraphUtils import *
import fileinput
import subprocess
from AthenaCommon import Logging
JOlog = Logging.logging.getLogger('FxFxJobOption')
JOlog.info(" Loading MadGraphControl file for tt012 FxFx ")

# General settings
nevents       = 5.0*runArgs.maxEvents # 25% efficiency for FxFx matching plus a safety factor
mode          = 0
gridpack_dir  = None
gridpack_mode = False

# MG Particle cuts                                                                                                
mllcut=0
# Shower/merging settings                                                                                                
maxjetflavor=5
parton_shower='PYTHIA8'

# DSID dictionary - Will allow expansion into a control file in the future
# key  - DSID
# arg1 - muQ (qCut) for Pythia8 FxFx
# arg2 - alphaS values in Pythi8 shower
ttbar_5fl={410452:["70.0","0.118"], # nonallhad 70 - alphaS lower value
           410453:["70.0","0.118"], # dilepton 70 - alphaS lower value
           410454:["70.0","0.118"], # allhadronic 70 - alphaS lower value
           410455:["30.0","0.118"], # nonallhad 30 - alphaS lower value
           410456:["30.0","0.118"], # dilepton 30 - alphaS lower value
           410457:["30.0","0.118"], # allhadronic 30 - alphaS lower value
           410458:["70.0","0.127"], # nonallhad 70 - A14 nominal alphaS value
           410459:["70.0","0.127"], # dilepton 70 - A14 nominal alphaS value
           410460:["70.0","0.127"], # allhadronic 70 - A14 nominal alphaS value
           410461:["30.0","0.127"], # nonallhad 30 - A14 nominal alphaS value
           410462:["30.0","0.127"], # dilepton 30 - A14 nominal alphaS value
           410463:["30.0","0.127"], # allhadronic 30 - A14 nominal alphaS value
           410448:["70.0","0.118"], # nonallhad 70 - alphaS lower value
           410449:["30.0","0.118"]} # nonallhad 30 - alphaS lower value

# We can define the madspin decay pattern based on DSID
nonallhad = [410452, 410455, 410458, 410461, 410448, 410449]
dilepton  = [410453, 410456, 410459, 410462]
allhad    = [410454, 410457, 410460, 410463]

# Using gridpacks, so make sure there is a check on the config file
if hasattr(runArgs,'inputGenConfFile'):
    evgenConfig.inputconfcheck = "tt012_FxFx"

    JOlog.info("Hacking jobstatus.pkl file to remove local filesystem dependency")
    import pickle,shutil
    data=None
    with open('madevent/SubProcesses/job_status.pkl', 'rb') as f:
        data = pickle.load(f)

    for i in data:
        i['dirname']=i['dirname'].replace('/home/giuli/TTBAR_TRY/run/pp_tt_012jFxFx_NLO_13TeV_PY8_MadSpin/','madevent/')
   
    output = open('myfile.pkl', 'wb')
    pickle.dump(data, output)
    output.close()
    shutil.move('myfile.pkl','madevent/SubProcesses/job_status.pkl')
    
if runArgs.runNumber in ttbar_5fl:
    name          = 'ttbarjets'
    process       = "pp>tt"
    gridpack_mode = True
    gridpack_dir  = 'madevent/'

else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

msdecay   = ""
nleptons  = -2

if runArgs.runNumber in nonallhad:
    nleptons = -1
    nevents  = 4.0*nevents # Filter efficiency
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
import model loop_sm-no_b_mass
define p = p b b~
define j = p
generate p p > t t~ [QCD] @0
add process p p > t t~ j [QCD] @1
add process p p > t t~ j j [QCD] @2
output pp_tt_012jFxFx_NLO_13TeV_PY8_MadSpin
""")
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")


lhaid=260000
pdflabel='lhapdf'

#Fetch default LO run_card.dat and set parameters                                                                             
extras = { 'lhaid'         : lhaid,
           'pdlabel'       : "'"+pdflabel+"'",
           'maxjetflavor'  : maxjetflavor,
           'parton_shower' : parton_shower,
           'mll_sf'        : mllcut,
           'mll'           : mllcut}
doSyst=True

if doSyst:
    lhe_version=3
    extras.update({'reweight_scale': '.true.',
                   'rw_Rscale_down':  0.5,
                   'rw_Rscale_up'  :  2.0,
                   'rw_Fscale_down':  0.5,
                   'rw_Fscale_up'  :  2.0,
                   'reweight_PDF'  : '.true.',
                   'PDF_set_min'   : 260001,
                   'PDF_set_max'   : 260100,
                   'ickkw'         :  3,
                   'jetradius'     :  1.0,
                   'ptj'           :  15,
                   'etaj'          :  5,
                   'isoEM'         :'True'})

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
fMadSpinCard.write('''set Nevents_for_max_weigth 250 # number of events for the estimate of the max. weight (default: 75)
set max_weight_ps_point 1000  # number of PS to estimate the maximum for each event (default: 400)
set BW_cut 50
%s
launch'''%(msdecay))
fMadSpinCard.close()


process_dir = new_process(grid_pack=gridpack_dir)

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)

get_param_file = subprocess.Popen(['get_files','-data', 'aMcAtNlo_param_card_loop_sm-no_b_mass.dat'])
get_param_file.wait()
import os, shutil
if not os.path.exists('aMcAtNlo_param_card_loop_sm-no_b_mass.dat'):
    raise RuntimeError("Cannot find aMcAtNlo_param_card_loop_sm-no_b_mass.dat")

print_cards()

generate(required_accuracy=0.001,run_card_loc='run_card.dat',param_card_loc='aMcAtNlo_param_card_loop_sm-no_b_mass.dat',proc_dir=process_dir,mode=mode,madspin_card_loc=madspin_card_loc,grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,gridpack_compile=True,random_seed=runArgs.randomSeed,nevents=nevents)
outputDS=arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz',lhe_version=lhe_version,saveProcDir=True)

# Shower
evgenConfig.description    = 'aMcAtNlo_ttbar_fxfx control'
evgenConfig.contact        = ['francesco.giuli@cern.ch','mcfayden@cern.ch','ian.connelly@cern.ch']
evgenConfig.keywords      += ['ttbar','jets','top']
runArgs.inputGeneratorFile=outputDS

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")

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
genSeq.Pythia8.Commands += ["JetMatching:merge           = on",
                            "JetMatching:scheme          = 1",
                            "JetMatching:setMad          = off",
                            "JetMatching:qCut            = "+ttbar_5fl[runArgs.runNumber][0], # Dependent on DSID
                            "JetMatching:coneRadius      = 1.0",
                            "JetMatching:etaJetMax       = 10.0",
                            "JetMatching:doFxFx          = on",
                            "JetMatching:qCutME          = 15.0", # Dependent on DSID
                            "JetMatching:nJetMax         = 2",
                             "SpaceShower:alphaSvalue    = "+ttbar_5fl[runArgs.runNumber][1], # Dependent on DSID
                             "TimeShower:alphaSvalue     = "+ttbar_5fl[runArgs.runNumber][1] # Dependent on DSID
                             ]

# Pythia8_i settings                                                                                                                        
genSeq.Pythia8.UserHook = 'JetMatchingMadgraph'
genSeq.Pythia8.FxFxXS = True

# Bit more logging info
JOlog.info("JetMatching:qCut   -> "+ttbar_5fl[runArgs.runNumber][0])
JOlog.info("SpaceShower:alphaSvalue   -> "+ttbar_5fl[runArgs.runNumber][1])
JOlog.info("TimeShower:alphaSvalue   -> "+ttbar_5fl[runArgs.runNumber][1])
JOlog.info("JetMatching:qCutME -> 15 GeV")
JOlog.info("aMcAtNlo:ptj       -> 15 GeV")

# --------------------------------------------------------------
# Apply TTbarWToLeptonFilter
# --------------------------------------------------------------
# Primarily needed for non-all hadronic decays
# nleptons == -2 means inclusive decay ergo no filtering on leptons

if nleptons != -2:
    include("GeneratorFilters/TTbarWToLeptonFilter.py")
    filtSeq.TTbarWToLeptonFilter.NumLeptons = nleptons
    filtSeq.TTbarWToLeptonFilter.Ptcut      = 0.0

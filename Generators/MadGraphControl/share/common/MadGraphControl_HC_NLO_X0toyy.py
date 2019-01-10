from MadGraphControl.MadGraphUtils import *

mode=0

resonanceMass = float(runArgs.jobConfig[0].split('_')[4])
resonanceWidthPercent = runArgs.jobConfig[0].split('_')[5].split('.py')[0] #Currently of the form e.g. W10p or NW

resonanceWidth = 0

if resonanceWidthPercent == "NW":
    resonanceWidth = 0.0042  #GeV
else:
    resonanceWidthPercent = float(resonanceWidthPercent[1:-1]) * 0.01 #Converting string to percent of mass
    resonanceWidth = resonanceWidthPercent * resonanceMass

if resonanceWidth == 0:
    raise RuntimeError("Resonance width not correctly interpreted from jO name")

#---------------------------------------------------------------------------------------------------
# Setting EFT parameters in HC model
#---------------------------------------------------------------------------------------------------
parameters={
    'frblock':{
        'Lambda':'1.000000e+03',
        'cosa':  '1.000000e+00',
        'kSM':   '1.000000e+00',
        'kHaa':  '1.000000e+00',
        'kHll':  '0.000000e+00',
        'kAll':  '0.000000e+00',
        'kAaa':  '0.000000e+00',
        'kHza':  '0.000000e+00',
        'kAza':  '0.000000e+00',
        'kHgg':  '1.000000e+00',
        'kAgg':  '0.000000e+00',
        'kHzz':  '0.000000e+00',
        'kAzz':  '0.000000e+00',
        'kHww':  '0.000000e+00',
        'kAww':  '0.000000e+00',
        'kHda':  '0.000000e+00',
        'kHdz':  '0.000000e+00',
        'kHdwR': '0.000000e+00',
        'kHdwI': '0.000000e+00'}
    }



#---------------------------------------------------------------------------------------------------
# Setting X0 mass and width for param_card.dat
#---------------------------------------------------------------------------------------------------
higgsMass  = {'25': '%e'%resonanceMass}               #Mass
higgsDecay = {'25':'DECAY 25 %e'%resonanceWidth} #Width


#---------------------------------------------------------------------------------------------------
# Generating gg -> X0 -> aa  in HC model
#---------------------------------------------------------------------------------------------------
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model HC_NLO_MOD_GammaGamma-heft
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
generate p p > x0 > a a / t [QCD]
output -f""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

process_dir = new_process()


#---------------------------------------------------------------------------------------------------
# Setting some parameters for run_card.dat
#---------------------------------------------------------------------------------------------------
#Fetch default NLO run_card.dat and set parameters
extras = {'lhe_version':'3.0',
          'pdlabel'    : "'lhapdf'",
          'lhaid'      : '260000',
          'parton_shower' :'PYTHIA8',
          'reweight_scale':'.false.',
          'bwcutoff'   : '1000'}

#---------------------------------------------------------------------------------------------------
# Setting the number of generated events to 'safefactor' times maxEvents,
# to avoid crashing due to not having enough events
# Also putting protection to avoid from crashing when maxEvents=-1
#---------------------------------------------------------------------------------------------------
safefactor = 1.1
nevents    = 10000*safefactor
if runArgs.maxEvents > 0:
    nevents=runArgs.maxEvents*safefactor



#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the param_card
# Build a new param_card.dat from an existing one
# Used values given in "parameters" for EFT parameters, if not set there, default values are used
# Higgs mass is set to 125 GeV by "higgsMass"
#---------------------------------------------------------------------------------------------------

build_param_card(param_card_old=process_dir+'/Cards/param_card.dat',param_card_new='param_card_new.dat',masses=higgsMass,decays=higgsDecay,params=parameters)

#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the run_card
# Build a new run_card.dat from an existing one
# Using the values given in "extras" above for the selected parameters when setting up the run_card
# If not set in "extras", default values are used
#---------------------------------------------------------------------------------------------------
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)


print_cards()



runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=mode,proc_dir=process_dir,run_name=runName)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3)

#### Shower

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")


evgenConfig.generators = ["aMcAtNlo", "Pythia8", "EvtGen"]
evgenConfig.description = 'X0 to gamma gamma in NLO HC model'
evgenConfig.keywords+=['BSMHiggs','diphoton']
evgenConfig.inputfilecheck = runName
evgenConfig.contact = ['Martin White <martin.white@adelaide.edu.au>']

runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

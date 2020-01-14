# Generator transform pre-include
#  Gets us ready for on-the-fly SUSY SM generation

# Simple variable setups
njets = -1
masses = {}
decays = {}

# Event multipliers for getting more events out of madgraph to feed through athena (esp. for filters)
evt_multiplier = 2.0

# Useful definitions
squarks = []
squarksl = []
for anum in [1,2,3,4]:
    squarks += [str(1000000+anum),str(-1000000-anum),str(2000000+anum),str(-2000000-anum)]
    squarksl += [str(1000000+anum),str(-1000000-anum)]
dict_index_syst = {0:'scalefactup',
                   1:'scalefactdown',
                   2:'alpsfactup',
                   3:'alpsfactdown',
                   4:'moreFSR',
                   5:'lessFSR',
                   6:'qup',
                   7:'qdown'}

# Basic settings for production and filters
SLHAonly = False
syst_mod = None
xqcut = None
madspin_card = None

try:
    from MadGraphControl.MadGraphUtils import getMadGraphVersion
    version = getMadGraphVersion()
    if int(version.split('.')[1])<5 and int(version.split('.')[0])<3:
        extras = {}
    else:
        extras = {'event_norm':'sum', # Fix normalization issues when SysCalc runs
                  }
except ImportError:
    print "WARNING: you are running a really old version of MadGraph now!"
    extras={}


if hasattr(runArgs,'EventMultiplier'): evt_multiplier=runArgs.EventMultiplier
if runArgs.maxEvents>0:
    nevts=runArgs.maxEvents*evt_multiplier
else:
    nevts=5000*evt_multiplier
evt_multiplier=-1

# in case someone needs to be able to keep the output directory for testing
keepOutput = False

# fixing LHE files after madspin?  do that here.
fixEventWeightsForBridgeMode=False


if 'py1up' in runArgs.jobConfig[0]:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var1Up_EvtGen_Common.py")
elif 'py1dw' in runArgs.jobConfig[0]:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var1Down_EvtGen_Common.py")
elif 'py2up' in runArgs.jobConfig[0]:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var2Up_EvtGen_Common.py")
elif 'py2dw' in runArgs.jobConfig[0]:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var2Down_EvtGen_Common.py")
elif 'py3aup' in runArgs.jobConfig[0]:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3aUp_EvtGen_Common.py")
elif 'py3adw' in runArgs.jobConfig[0]:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3aDown_EvtGen_Common.py")
elif 'py3bup' in runArgs.jobConfig[0]:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3bUp_EvtGen_Common.py")
elif 'py3bdw' in runArgs.jobConfig[0]:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3bDown_EvtGen_Common.py")
elif 'py3cup' in runArgs.jobConfig[0]:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3cUp_EvtGen_Common.py")
elif 'py3cdw' in runArgs.jobConfig[0]:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3cDown_EvtGen_Common.py")
else:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

include("Pythia8_i/Pythia8_MadGraph.py")

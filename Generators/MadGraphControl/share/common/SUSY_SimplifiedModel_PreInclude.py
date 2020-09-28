# Generator transform pre-include
#  Gets us ready for on-the-fly SUSY SM generation

# Simple variable setups
param_blocks = {} # For general params
decoupled_mass = '4.5E9'
masses = {}
for p in ['1000001','1000002','1000003','1000004','1000005','1000006','2000001','2000002','2000003','2000004','2000005','2000006','1000021',\
          '1000023','1000024','1000025','1000011','1000013','1000015','2000011','2000013','2000015','1000012','1000014','1000016','1000022',\
          '1000035','1000037','35','36','37']: # Note that gravitino is non-standard
    masses[p]=decoupled_mass
decays = {}

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
syst_mod = None
ktdurham = None # Only set if you want a non-standard setting (1/4 heavy mass)
madspin_card = None
param_card = None # Only set if you *can't* just modify the default param card to get your settings (e.g. pMSSM)

# Default run settings
run_settings = {'event_norm':'sum',
                'drjj':0.0,
                'lhe_version':'3.0',
                'cut_decays':'F',
                'pdgs_for_merging_cut': '1, 2, 3, 4, 21', # Terrible default in MG
                'ickkw':0,
                'xqcut':0} # use CKKW-L merging (yes, this is a weird setting)
# Set up default PDF and systematic settings (note: action in import module)
from MadGraphControl.MadGraph_NNPDF30NLO_Base_Fragment import *

# Setting for writing out a gridpack
writeGridpack = False

# Event multipliers for getting more events out of madgraph to feed through athena (esp. for filters)
evt_multiplier=-1

# in case someone needs to be able to keep the output directory for testing
keepOutput = False

# fixing LHE files after madspin?  do that here.
fixEventWeightsForBridgeMode=False

# In case you want to keep lifetimes in the LHE files
add_lifetimes_lhe = False

from MadGraphControl.MadGraphUtilsHelpers import get_physics_short
phys_short = get_physics_short()
if 'py1up' in phys_short:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var1Up_EvtGen_Common.py")
elif 'py1dw' in phys_short:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var1Down_EvtGen_Common.py")
elif 'py2up' in phys_short:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var2Up_EvtGen_Common.py")
elif 'py2dw' in phys_short:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var2Down_EvtGen_Common.py")
elif 'py3aup' in phys_short:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3aUp_EvtGen_Common.py")
elif 'py3adw' in phys_short:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3aDown_EvtGen_Common.py")
elif 'py3bup' in phys_short:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3bUp_EvtGen_Common.py")
elif 'py3bdw' in phys_short:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3bDown_EvtGen_Common.py")
elif 'py3cup' in phys_short:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3cUp_EvtGen_Common.py")
elif 'py3cdw' in phys_short:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3cDown_EvtGen_Common.py")
else:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

include("Pythia8_i/Pythia8_MadGraph.py")

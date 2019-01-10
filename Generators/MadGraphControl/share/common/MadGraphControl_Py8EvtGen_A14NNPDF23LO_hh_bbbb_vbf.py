from MadGraphControl.MadGraphUtils import *

#---------------------------------------------------------------------------------------------------
# Setting higgs mass to 125 GeV for param_card.dat
#---------------------------------------------------------------------------------------------------
higgsMass={'25':'1.250000e+02'} #MH

#---------------------------------------------------------------------------------------------------
# Setting some parameters for run_card.dat
#---------------------------------------------------------------------------------------------------
extras = { 'lhe_version':'2.0', 
           'cut_decays':'F', 
           'pdlabel':"'nn23lo1'",
           'scale':'125',
           'dsqrt_q2fact1':'125',
           'dsqrt_q2fact2':'125',
           }

#---------------------------------------------------------------------------------------------------
# Generating di-higgs through Heavy Higgs resonance with MadGraph
# Due to the parameters settings above, the resonant contribution is eliminated
# Thus, the non-resonant hh production with different trilinear Higgs couplings is enabled
#---------------------------------------------------------------------------------------------------
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
    import model sm
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    import model /cvmfs/atlas.cern.ch/repo/sw/Generators/madgraph/models/latest/HHVBF_UFO
    generate p p > h h j j QCD=0 
    output -f""")
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

#---------------------------------------------------------------------------------------------------
# Setting the number of generated events to 30 times maxEvents,
# to avoid crashing due to not having enough events
# Also putting protection to avoid from crashing when maxEvents=-1
#---------------------------------------------------------------------------------------------------
safefactor=30
nevents=5000*safefactor
if runArgs.maxEvents > 0:
    nevents=runArgs.maxEvents*safefactor

process_dir = new_process()
#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the run_card
# Build a new run_card.dat from an existing one
# Using the values given in "extras" above for the selected parameters when setting up the run_card
# If not set in "extras", default values are used 
#---------------------------------------------------------------------------------------------------
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0,extras=extras)



#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the param_card
# Build a new param_card.dat from an existing one
# Used values given in "parameters" for MHH and WHH, if not set there, default values are used
# Higgs mass is set to 125 GeV by "higgsMass"
#---------------------------------------------------------------------------------------------------
build_param_card(param_card_old='%s/Cards/param_card.dat' % process_dir,param_card_new='param_card_new.dat',masses=higgsMass,params=parameters)
   
print_cards()
    
runName='run_01'     

generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=0,proc_dir=process_dir,run_name=runName)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

#---------------------------------------------------------------------------------------------------
# Pythia8 Showering with A14_NNPDF23LO
#---------------------------------------------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_LHEF.py")

#---------------------------------------------------------------------------------------------------
# EVGEN Configuration
#---------------------------------------------------------------------------------------------------
evgenConfig.generators = ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.description = "Non-resonant di-Higgs production through vector-boson-fusion (VBF) which decays  to bbbb."
evgenConfig.keywords = ["BSM", "BSMHiggs", "nonResonant", "VBF", "bottom"]
evgenConfig.contact = ['John Alison <john.alison@cern.ch>']
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

#---------------------------------------------------------------------------------------------------
# Decaying hh to bbbb with Pythia8
# Preferred way to select particular decay channels without modifying the Higgs
# width or cross section is to turn off all decays but those of interest, not to
# modify the branching ratios
#---------------------------------------------------------------------------------------------------
genSeq.Pythia8.Commands += ["25:oneChannel = on 1.0 100 5 -5 "] #bb final state                                                                               

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = DEBUG

from MadGraphControl.MadGraphUtils import *

joboptparts = runArgs.jobConfig[0].strip('\.py').split('_')
decay = joboptparts[4]
mass = float(joboptparts[2].lstrip('X').rstrip('tohh'))

mode=0

#-----------------------------------------
# Setting mHH and WHH for param_card.dat
#-----------------------------------------
parameters={'1560':str(mass),      #MHH 
            '1561':'4.000000e-03'} #WHH

#----------------------------------------------------
# Setting higgs mass to 125 GeV for param_card.dat
#----------------------------------------------------
higgsMass={'25':'1.250000e+02'} #MH

#-----------------------------------------------
# Setting some parameters for run_card.dat
#-----------------------------------------------
extras = { 'lhe_version':'2.0', 
           'cut_decays':'F', 
           'pdlabel':"'nn23lo1'",
           'scale':'125',
           'dsqrt_q2fact1':'125',
           'dsqrt_q2fact2':'125',      
           'parton_shower':'PYTHIA8',
           'ptj':'0',
           'ptb':'0',
           'pta':'0',
           'ptjmax':'-1',
           'ptbmax':'-1',
           'ptamax':'-1',
           'etaj':'-1',
           'etab':'-1',
           'etaa':'-1',
           'etajmin':'0',
           'etabmin':'0',
           'etaamin':'0',
           'mmaa':'0',
           'mmaamax':'-1',
           'mmbb':'0',
           'mmbbmax':'-1',
           'drjj':'0',
           'drbb':'0',
           'draa':'0',
           'drbj':'0',
           'draj':'0',
           'drab':'0',
           'drjjmax':'-1',
           'drbbmax':'-1',
           'draamax':'-1',
           'drbjmax':'-1',
           'drajmax':'-1',
           'drabmax':'-1' }

#---------------------------------------------------------------------------------------------------
# Generating di-higgs through Heavy Higgs resonance with MadGraph
#---------------------------------------------------------------------------------------------------
fcard = open('proc_card_mg5.dat','w')
if runArgs.runNumber in range(345190,345200):
    fcard.write("""
    import model sm
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    import model HeavyHiggsTHDM
    generate p p > h h 
    output -f""")
    fcard.close()
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

beamEnergy=6500
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

runName='run_01'

process_dir = new_process(card_loc='proc_card_mg5.dat')

#---------------------------------------------------------------------------------------------------
# Filter efficiency is low.
# Thus, setting the number of generated events to 30 times maxEvents,
# to avoid crashing due to not having enough events
# Also putting protection to avoid from crashing when maxEvents=-1
#---------------------------------------------------------------------------------------------------
safefactor=30
nevents=5000*safefactor
if runArgs.maxEvents > 0:
    nevents=runArgs.maxEvents*safefactor


#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the param_card
# Build a new param_card.dat from an existing one
# Used values given in "parameters" for MHH and WHH, if not set there, default values are used
# Higgs mass is set to 125 GeV by "higgsMass"
#---------------------------------------------------------------------------------------------------

build_param_card(param_card_old='param_card.HeavyScalar.dat',param_card_new='param_card_new.dat',masses=higgsMass,extras=parameters)


#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the run_card
 # Build a new run_card.dat from an existing one
# Using the values given in "extras" above for the selected parameters when setting up the run_card
# If not set in "extras", default values are used 
#---------------------------------------------------------------------------------------------------

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir), run_card_new='run_card.dat', xqcut=0,
               nevts=nevents, rand_seed=runArgs.randomSeed, beamEnergy=beamEnergy,
               extras=extras)

print_cards()

generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=mode,proc_dir=process_dir,run_name=runName)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

#---------------------------------------------------------------------------------------------------
# Pythia8 Showering with A14_NNPDF23LO
#---------------------------------------------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_LHEF.py")
include("Pythia8_i/Pythia8_MadGraph.py")

#---------------------------------------------------------------------------------------------------
# EVGEN Configuration
#---------------------------------------------------------------------------------------------------
evgenConfig.generators     = ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.description    = "Di-Higgs production through %s GeV Heavy Higgs resonance which decays to bbtautau %s."%(mass,decay)
evgenConfig.keywords       = ["hh","BSM", "BSMHiggs", "resonance", "tau", "bottom"]
evgenConfig.contact        = ['Marisilvia Donadelli <Marisilvia.Donadelli@cern.ch>']
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile = runName+'._00001.events.tar.gz'

#---------------------------------------------------------------------------------------------------

# Decaying hh to bbtautau with Pythia8

#---------------------------------------------------------------------------------------------------

genSeq.Pythia8.Commands += ["25:onMode=off",
                            "25:onIfAny=5 15" ]

#Will comment this part by now:
# Require Higgs(25) -> b(5) b~(-5) tau(15) tau~(-15) independent of status codes (recommended)
#from GeneratorFilters.GeneratorFiltersConf import ParentTwoChildrenFilter
#filtSeq += ParentTwoChildrenFilter("HiggsToBBTauTauFilter")
#filtSeq.HiggsToBBTauTauFilter.PDGParent = [25]
#filtSeq.HiggsToBBTauTauFilter.PDGChild = [5,15]

#---------------------------------------------------------------------------------------------------
# Filter for bbtautau
#---------------------------------------------------------------------------------------------------
#include("GeneratorFilters/XtoVVDecayFilterExtended.py")
from GeneratorFilters.GeneratorFiltersConf import XtoVVDecayFilterExtended
filtSeq += XtoVVDecayFilterExtended("BBTauTauFilter")
filtSeq.BBTauTauFilter.PDGGrandParent = 1560
filtSeq.BBTauTauFilter.PDGParent = 25
filtSeq.BBTauTauFilter.StatusParent = 22
filtSeq.BBTauTauFilter.PDGChild1 = [5]
filtSeq.BBTauTauFilter.PDGChild2 = [15]

#---------------------------------------------------------------------------------------------------
# Filter for lephad
#---------------------------------------------------------------------------------------------------
if runArgs.runNumber in range(345190,345195):
    filtSeq += XtoVVDecayFilterExtended("TauTauLepHadFilter")
    filtSeq.TauTauLepHadFilter.PDGGrandParent = 25
    filtSeq.TauTauLepHadFilter.PDGParent = 15
    filtSeq.TauTauLepHadFilter.StatusParent = 2
    filtSeq.TauTauLepHadFilter.PDGChild1 = [11,13]
    filtSeq.TauTauLepHadFilter.PDGChild2 = [111,130,211,221,223,310,311,321,323]

#---------------------------------------------------------------------------------------------------
# Filter for hadhad
#---------------------------------------------------------------------------------------------------
if runArgs.runNumber in range(345195,345200):
    filtSeq += XtoVVDecayFilterExtended("TauTauHadHadFilter")
    filtSeq.TauTauHadHadFilter.PDGGrandParent = 25
    filtSeq.TauTauHadHadFilter.PDGParent = 15
    filtSeq.TauTauHadHadFilter.StatusParent = 2
    filtSeq.TauTauHadHadFilter.PDGChild1 = [111,130,211,221,223,310,311,321,323]
    filtSeq.TauTauHadHadFilter.PDGChild2 = [111,130,211,221,223,310,311,321,323]

#---------------------------------------------------------------------------------------------------
# Filter for 2 leptons (inc tau(had)) with pt cuts on e/mu and tau(had)
#---------------------------------------------------------------------------------------------------
from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
filtSeq += MultiElecMuTauFilter("LepTauPtFilter")
filtSeq.LepTauPtFilter.IncludeHadTaus = True
filtSeq.LepTauPtFilter.NLeptons = 2
filtSeq.LepTauPtFilter.MinPt = 13000.
filtSeq.LepTauPtFilter.MinVisPtHadTau = 15000.
filtSeq.LepTauPtFilter.MaxEta = 5.

if runArgs.runNumber in range(345190,345195):
    filtSeq.Expression = "BBTauTauFilter and TauTauLepHadFilter and LepTauPtFilter"

if runArgs.runNumber in range(345195,345200):
    filtSeq.Expression = "BBTauTauFilter and TauTauHadHadFilter and LepTauPtFilter"

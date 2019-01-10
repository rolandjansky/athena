from MadGraphControl.MadGraphUtils import *

minevents=5000
if runArgs.runNumber == 345750:
    minevents=500

nevents=max(minevents,runArgs.maxEvents)
mode=0

# merging parameters
maxjetflavor=5
ktdurham=30
dparameter=0.2
nJetMax=4
process="pp>z"

# MG Particle cuts
mbbcut=20

### DSID lists (extensions can include systematics samples)
test_Np0=[345746]
test_Np1=[345747]
test_Np2=[345748]
test_Np3=[345749]
test_Np4=[345750]


if runArgs.runNumber in test_Np0:
    mg_proc="generate p p > z, z > b b~ @0"
    nevents=int(nevents*100)
elif runArgs.runNumber in test_Np1:    
    mg_proc="generate p p > z j, z > b b~ @1"
    nevents=int(nevents*50)
elif runArgs.runNumber in test_Np2:
    mg_proc="generate p p > z j j, z > b b~ @2"
    nevents=int(nevents*15)
elif runArgs.runNumber in test_Np3:
    mg_proc="generate p p > z j j j, z > b b~ @3"
    nevents=int(nevents*10)
elif runArgs.runNumber in test_Np4:
    mg_proc="generate p p > z j j j j, z > b b~ @4"
    nevents=int(nevents*8)
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
"""+mg_proc+"""
output -f""")
fcard.close()



beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version' : '3.0', 
           'cut_decays'  : 'F', 
           'pdlabel'     : "'lhapdf'",
           'lhaid'          : 247000,  
           'maxjetflavor'   : maxjetflavor,
           'asrwgtflavor'   : maxjetflavor,
           'ptj'            : 20,      
           'ptb'            : 20,      
           'mmbb'           : mbbcut, 
           'drjj'       : 0.0,
           'ktdurham'   : ktdurham,
           'dparameter' : dparameter}
process_dir = new_process()
build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat',xqcut=0,
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

    
print_cards()
    
runName='run_01'     


generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,run_name=runName)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

#### Shower 
evgenConfig.description = 'MadGraph_Zbb'
evgenConfig.keywords+=['Z','jets','drellYan']
evgenConfig.inputfilecheck = runName
evgenConfig.minevents = minevents
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'


PYTHIA8_nJetMax=nJetMax
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=process
PYTHIA8_TMS=ktdurham
PYTHIA8_nQuarksMerge=maxjetflavor

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")
genSeq.Pythia8.Commands+=["Merging:mayRemoveDecayProducts=on"]

include("GeneratorFilters/AntiKt4TruthJets_pileup.py")

if not hasattr(filtSeq, "VBFForwardJetsFilter" ):
  from GeneratorFilters.GeneratorFiltersConf import VBFForwardJetsFilter
  filtSeq += VBFForwardJetsFilter()

filtSeq.VBFForwardJetsFilter.JetMinPt=15.*GeV
filtSeq.VBFForwardJetsFilter.JetMaxEta=5.0
filtSeq.VBFForwardJetsFilter.NJets=2
filtSeq.VBFForwardJetsFilter.Jet1MinPt=15.*GeV
filtSeq.VBFForwardJetsFilter.Jet1MaxEta=5.0
filtSeq.VBFForwardJetsFilter.Jet2MinPt=15.*GeV
filtSeq.VBFForwardJetsFilter.Jet2MaxEta=5.0
filtSeq.VBFForwardJetsFilter.UseOppositeSignEtaJet1Jet2=False
filtSeq.VBFForwardJetsFilter.TruthJetContainer="AntiKt4TruthJets"
filtSeq.VBFForwardJetsFilter.LGMinPt=15.*GeV
filtSeq.VBFForwardJetsFilter.LGMaxEta=2.5
filtSeq.VBFForwardJetsFilter.DeltaRJLG=0.05
filtSeq.VBFForwardJetsFilter.RatioPtJLG=0.3

# medium cut:
filtSeq.VBFForwardJetsFilter.MassJJ = 300.*GeV
filtSeq.VBFForwardJetsFilter.DeltaEtaJJ = 3.0

from MadGraphControl.MadGraphUtils import *

nevents=15000
mode=0

# merging parameters
ktdurham=30
dparameter=0.4
nJetMax=1
process='pp>tt~'
maxjetflavor=5

### DSID lists (extensions can include systematics samples)
test_Np01=[429207]
test_Np0  =[429205]
test_Np1  =[429206]


if runArgs.runNumber in test_Np01:
    mg_proc="""generate p p > t t~ @0
add process p p > t t~ j @1
"""
elif runArgs.runNumber in test_Np0:
    mg_proc="generate p p > t t~ @0"
elif runArgs.runNumber in test_Np1:    
    mg_proc="generate p p > t t~ j @1"
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm-no_b_mass
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~ b b~
"""+mg_proc+"""
output -f""")
fcard.close()



beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


process_dir = new_process()

#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version' : '2.0', 
           'cut_decays'  : 'F', 
           'pdlabel'     : "'nn23lo1'",
           'ickkw'       : 0,
           'drjj'        : 0.0,
           'maxjetflavor': maxjetflavor,
           'ktdurham'    : ktdurham,
           'dparameter'  : dparameter,
           'use_syst'    : 'False'}
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',xqcut=0,
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

    
print_cards()
    
runName='run_01'     



generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,run_name=runName)
outputDS=arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)  
   


#### Shower 
evgenConfig.description = 'MadGraph_ttbar'
evgenConfig.keywords+=['ttbar','jets']
evgenConfig.inputfilecheck = outputDS.split('.tar.gz')[0]
runArgs.inputGeneratorFile=outputDS

PYTHIA8_nJetMax=nJetMax
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=process
PYTHIA8_TMS=ktdurham
PYTHIA8_nQuarksMerge=maxjetflavor
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")
#Needed for ttbar:
genSeq.Pythia8.Commands+=["Merging:mayRemoveDecayProducts=on"]

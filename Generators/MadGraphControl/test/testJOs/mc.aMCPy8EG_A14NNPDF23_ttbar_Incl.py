from MadGraphControl.MadGraphUtils import *

nevents=10000
mode=0


### DSID lists (extensions can include systematics samples)
test=[999999]

fcard = open('proc_card_mg5.dat','w')
if runArgs.runNumber in test:
    fcard.write("""
    import model loop_sm
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    generate p p > t t~ [QCD]
    output -f""")
    fcard.close()
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)



beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")



process_dir = new_process()

#Fetch default NLO run_card.dat and set parameters
extras = { 'pdlabel'       :"'lhapdf'",
           'lhaid'         :"11000",
           #'pdlabel'       :"'nn23nlo'",
           'parton_shower' :'PYTHIA8',
           'reweight_scale':'.false.'}
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)


print_cards()
    
runName='run_01'     

generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,run_name=runName)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)  
  


#### Shower

evgenConfig.description = 'aMcAtNlo_ttbar'
evgenConfig.keywords+=['ttbar','jets']
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'


include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")

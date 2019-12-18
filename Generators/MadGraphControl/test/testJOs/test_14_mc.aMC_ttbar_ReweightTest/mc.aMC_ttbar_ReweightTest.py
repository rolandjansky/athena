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
    generate p p > w+ [QCD]
    output -f""")
    fcard.close()
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)



beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")




print_cards()
    
runName='run_01'     


process_dir = new_process()

#Fetch default NLO run_card.dat and set parameters
extras = {}
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

reweight_card_loc='reweight_card.dat'
reweight_card = open(reweight_card_loc,'w')
reweight_card.write("""
set aewm1 100
launch
set aewm1 200
launch
set aewm1 300""")

reweight_card.close()

generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,run_name=runName,reweight_card_loc=reweight_card_loc)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)  
  

evgenConfig.generators = ["aMcAtNlo"]

############################
# Shower JOs will go here

theApp.finalize()
theApp.exit()

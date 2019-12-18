from MadGraphControl.MadGraphUtils import *

nevents=10000
mode=0


### DSID lists (extensions can include systematics samples)
test=[999999]

fcard = open('proc_card_mg5.dat','w')
if runArgs.runNumber in test:
    fcard.write("""
    import model sm
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    generate p p > t t~
    output -f""")
    fcard.close()

else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)



beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


runName='run_01'     


process_dir = new_process()

#Fetch default LO run_card.dat and set parameters
extras = {'lhe_version'  :'3.0', 
          'cut_decays'   :'F', 
          'pdlabel'      : "'lhapdf'",
          'lhaid'        : 247000,
          'use_syst'     : 'True',
          'sys_scalefact':'1.0 0.5 2.0',
          'sys_pdf'      : 'NNPDF23_lo_as_0130_qed'}

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

    
print_cards()
    
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,run_name=runName)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)
  

############################
# Shower JOs will go here
evgenConfig.generators = ["MadGraph"]
theApp.finalize()
theApp.exit()


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


runName='run_01'     


process_dir = new_process()

#Fetch default LO run_card.dat and set parameters
extras = {'lhe_version'    :'2.0', 
          'pdlabel'        : "'lhapdf'",
          'lhaid'          : 260000,
          'parton_shower'  :'PYTHIA8', 
          'reweight_scale' :'1',
          'rw_rscale'      :'1.0 0.5 2.0',
          'rw_fscale'      :'1.0 0.5 2.0',
          'reweight_PDF'   : 'True',
          'PDF_set_min'    : 260001,
          'PDF_set_max'    : 260100,
          'store_rwgt_info':'True'}

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)



madspin_card_loc='madspin_card.dat'                                                                                                                                    

mscard = open(madspin_card_loc,'w')                                                                                                                                    
mscard.write("""#************************************************************
#*                        MadSpin                           *
#*                                                          *
#*    P. Artoisenet, R. Frederix, R. Rietkerk, O. Mattelaer *
#*                                                          *
#*    Part of the MadGraph5_aMC@NLO Framework:              *
#*    The MadGraph5_aMC@NLO Development Team - Find us at   *
#*    https://server06.fynu.ucl.ac.be/projects/madgraph     *
#*                                                          *
#************************************************************
#Some options (uncomment to apply)
#
# set seed 1
# set Nevents_for_max_weigth 75 # number of events for the estimate of the max. weight
# set BW_cut 15                # cut on how far the particle can be off-shell
 set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
#
set seed %i
# specify the decay for the final state particles
decay t > w+ b, w+ > all all
decay t~ > w- b~, w- > all all
decay w+ > all all
decay w- > all all
decay z > all all
# running the actual code
launch"""%runArgs.randomSeed)                                                                                                                                              
mscard.close()

    
print_cards()
    
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,run_name=runName,madspin_card_loc=madspin_card_loc)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)  
   

############################
# Shower JOs will go here
theApp.finalize()
theApp.exit()


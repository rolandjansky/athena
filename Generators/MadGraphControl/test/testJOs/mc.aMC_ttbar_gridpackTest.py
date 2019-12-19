from MadGraphControl.MadGraphUtils import *

# General settings
nevents=10000
gridpack_dir=None
gridpack_mode=False
runName='run_01'

### DSID lists (extensions can include systematics samples)
ttbar_5fl=[999999]

if runArgs.runNumber in ttbar_5fl:
    mgproc="generate p p > t t~ [QCD] @0"
    name='ttbar'
    process="pp>tt~"

    gridpack_mode=True
    gridpack_dir='madevent/'

else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model loop_sm-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
"""+mgproc+"""
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


process_dir = new_process(grid_pack=gridpack_dir)

#Fetch default LO run_card.dat and set parameters
extras = {'pdlabel'      : "'lhapdf'",
          'lhaid'        : 260000,
          'parton_shower':'PYTHIA8', 
          'reweight_PDF' : 'True',
          'PDF_set_min'  : 260001,
          'PDF_set_max'  : 260100}

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)

print_cards()

madspin_card_loc='madspin_card.dat'

if not hasattr(runArgs, 'inputGenConfFile'):
    fMadSpinCard = open('madspin_card.dat','w')
    fMadSpinCard.write('import Events/'+runName+'/events.lhe.gz\n')                  
    fMadSpinCard.write('set ms_dir MadSpin\n')                                        
else:                                                                               
    os.unlink(gridpack_dir+'Cards/madspin_card.dat')                                  
    fMadSpinCard = open(gridpack_dir+'Cards/madspin_card.dat','w')                    
    fMadSpinCard.write('import '+gridpack_dir+'Events/'+runName+'/events.lhe.gz\n')  
    fMadSpinCard.write('set ms_dir '+gridpack_dir+'MadSpin\n')                        
    fMadSpinCard.write('set seed '+str(10000000+int(runArgs.randomSeed))+'\n')        
fMadSpinCard.write('''set Nevents_for_max_weigth 250 # number of events for the estimate of the max. weight (default: 75)
set max_weight_ps_point 1000  # number of PS to estimate the maximum for each event (default: 400)
decay t > w+ b, w+ > all all
decay t~ > w- b~, w- > all all
launch''')
fMadSpinCard.close()  

#mscard.write("""#************************************************************
##*                        MadSpin                           *
##*                                                          *
##*    P. Artoisenet, R. Frederix, R. Rietkerk, O. Mattelaer *
##*                                                          *
##*    Part of the MadGraph5_aMC@NLO Framework:              *
##*    The MadGraph5_aMC@NLO Development Team - Find us at   *
##*    https://server06.fynu.ucl.ac.be/projects/madgraph     *
##*                                                          *
##************************************************************
##Some options (uncomment to apply)
##
## set seed 1
## set Nevents_for_max_weigth 75 # number of events for the estimate of the max. weight
## set BW_cut 15                # cut on how far the particle can be off-shell
# set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
##
#set seed %i
## specify the decay for the final state particles
#decay t > w+ b, w+ > all all
#decay t~ > w- b~, w- > all all
#decay w+ > all all
#decay w- > all all
#decay z > all all
## running the actual code
#launch"""%runArgs.randomSeed)                                                                                                                                              
#mscard.close()

    
print_cards()
    
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=0,njobs=1,proc_dir=process_dir,run_name=runName,madspin_card_loc=madspin_card_loc,
         grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=runArgs.randomSeed)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)



############################
# Shower JOs will go here
theApp.finalize()
theApp.exit()


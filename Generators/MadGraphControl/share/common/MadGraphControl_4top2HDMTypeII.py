from MadGraphControl.MadGraphUtils import *

nevents=6000

### DSID lists
DSIDDic = {304777:'400',304778:'500',304779:'600',304780:'700',304781:'800',304782:'900',304783:'1000'}
mass = DSIDDic[runArgs.runNumber]

#STEP - compute the process
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
    import model 2HDMtypeII
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    generate p p > t t~ h2 QED=99 QCD=99
    output out/mh2_"""+mass+"""/""")
fcard.close()

rcard='MadGraph_run_card_2HDM.dat'
# getting run cards
from PyJobTransformsCore.trfutil import get_files

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version':'2.0', 
           'cut_decays':'F', 
           'pdlabel':"'cteq6l1'",
           'parton_shower':'PYTHIA8' }
build_run_card(run_card_old=rcard,run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

madspin_card_loc='madspin_card.dat'                                                                                                                                    

mscard = open(madspin_card_loc,'w')                                                                                                                                    
mscard.write("""
 set max_weight_ps_point 400  # number of PS to estimate the maximum for each event                                                                                        
#                                                                                                                                                                          
set seed %i                                                                                                                                                                
# specify the decay for the final state particles                                                                                                                          
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
decay t > w+ b, (w+ > all all)
decay t~ > w- b~, (w- > all all)
decay h2 > t t~, (t > w+ b, w+ > all all), (t~ > w- b~, w- > all all)
# running the actual code                                                                                                                                                  
launch"""%runArgs.randomSeed)                                                                                                                                              
mscard.close()
    
print_cards()
    
runName='run_01'     
##Generate the process using the previous widths
process_dir = new_process(card_loc='proc_card_mg5.dat')
generate(run_card_loc='run_card.dat', 
         param_card_loc='MadGraph_param_card_2HDMm'+mass+'.dat',
         mode=0, 
         njobs=1, 
         run_name=runName, 
         madspin_card_loc='madspin_card.dat',
         proc_dir=process_dir)

evgenConfig.generators = ["MadGraph"]

############################
# Shower JOs will go here
procDir='out/mh2_'+mass+'/'
outputDS=runName+'._00001.events.tar.gz'

arrange_output(run_name=runName,proc_dir=procDir,outputDS=runName+'._00001.events.tar.gz', lhe_version=1.0)  

evgenConfig.description = '2HDMtypeII alignment limit,m='+mass+'GeV,all decays'
evgenConfig.process = "p p > t t~ h2 > t t~ t t~; t > w b, w > all all"
evgenConfig.keywords+=['top','BSMHiggs','jets','exotic','BSM']
evgenConfig.inputfilecheck = runName
evgenConfig.contact  = ["alberto.gascon.bravo@desy.de"]
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

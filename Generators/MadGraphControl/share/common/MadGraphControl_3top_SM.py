from MadGraphControl.MadGraphUtils import *

# General settings
minevents=5000
nevents=20000 
mode=0

DSID_3top_SM =[304014] 


if runArgs.runNumber in DSID_3top_SM:
    mgproc="""generate p p > t t~ t w- QED=3 \nadd process p p > t t~ t~ w+ QED=3 \nadd process p p > t t~ t j QED=4 \nadd process p p > t t~ t~ j QED=4"""
    name="3topSM"
    keyword=['SM','top']

else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm
define p = g u c d s u~ c~ d~ s~
define p = p b b~
define j = g u c d s u~ c~ d~ s~
define j = j b b~
"""+mgproc+"""


output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


pdflabel="nn23lo1"


#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version'  : '2.0',
           'pdlabel'      : "'"+pdflabel+"'" }

build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,
               extras=extras)

print_cards()
process_dir = new_process()
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir)
arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')


#### Shower 
evgenConfig.process = "3top_SM"
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=keyword 
evgenConfig.inputfilecheck = stringy
evgenConfig.minevents = minevents
evgenConfig.contact= ["lserkin@cern.ch", "michele.pinamonti@gmail.com"]

runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

from MadGraphControl.MadGraphUtils import *

# General settings
minevents=5000
nevents=20000
mode=0

DSID_ttbarWW =[410081]


if runArgs.runNumber in DSID_ttbarWW:
    mgproc="""generate p p > t t~ W+ W-"""
    name="ttbarWW"
    keyword=['SM','top','ttVV']

else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
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

process_dir = new_process()
build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,
               extras=extras)

print_cards()
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir)
arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')


#### Shower 
evgenConfig.generators += ["MadGraph", "Pythia8"]
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=keyword 
evgenConfig.inputfilecheck = stringy
evgenConfig.minevents = minevents
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

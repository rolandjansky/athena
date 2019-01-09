from MadGraphControl.MadGraphUtils import *
import math
import re

#Take mass and coupling from jobOptions name
reMass = int(re.findall(r'\d+',re.findall(r'4bM\d+',runArgs.jobConfig[0])[0])[1])
reCoupling = int(re.findall(r'\d+', runArgs.jobConfig[0])[-1])

#Madraph Settings
mode=0
process='pp>bb~'
mzp=reMass
gta=1.0
gb=reCoupling*0.1
wzp = (gb*gb*mzp) /  (math.pi*8)

# merging parameters
ktdurham=30
dparameter=0.4
nJetMax=2
maxjetflavor=5

#Defining process, two additional jets possible
mg_proc="""generate p p > zp , zp > b b~ @0
add process p p > zp j , zp > b b~ @1
add process p p > zp j j , zp > b b~ @2
"""

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model ThirdZp_UFO
define p = p b b~
define j = j b b~
"""+mg_proc+"""
output -f""")
fcard.close()



beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


process_dir = new_process()
           
#'pdlabel'     : "'nn23lo1'",

#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version' : '2.0', 
           'cut_decays'  : 'F', 
           'pdlabel'     : "'lhapdf'",
           'lhaid'       : "263000",
           'ickkw'       : 0,
           'drjj'        : 0.0,
           'maxjetflavor': maxjetflavor,
           'ktdurham'    : ktdurham,
           'dparameter'  : dparameter,
           'use_syst'    : 'False'}
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',xqcut=0,
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

    
print_cards()

params = {}
params['MASS']={'mzp': str(mzp)}
params['DECAY']= {'wzp': str(wzp)}
params['NPCOUP']={'gta' : str(gta) , 'gb' : str(gb)}
 
runName='run_01'    
build_param_card(param_card_old=process_dir+'/Cards/param_card.dat',param_card_new='param_card_new.dat',params=params)
print_cards()

generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=mode,proc_dir=process_dir,run_name=runName)

## Interfacing output/input with pythia
outputDS = arrange_output(run_name=runName,proc_dir=process_dir,lhe_version=3,saveProcDir=True)  
runArgs.inputGeneratorFile=outputDS


#### Shower and matching/merging
PYTHIA8_nJetMax=nJetMax
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=process
PYTHIA8_TMS=ktdurham
PYTHIA8_nQuarksMerge=maxjetflavor
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")

#Information Strings
evgenConfig.description = 'MadGraph Zprime->bb with up to 2 additional jets'
evgenConfig.keywords=[ 'BSM', 'Zprime', 'heavyBoson', 'resonance']
evgenConfig.contact=['hector.de.la.torre.perez@cern.ch']

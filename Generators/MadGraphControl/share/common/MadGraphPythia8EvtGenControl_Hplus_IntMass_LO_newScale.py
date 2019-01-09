#--------------------------------------------------------------
# on-the-fly generation of H+ MG5 events, montoya@cern.ch
#--------------------------------------------------------------
from MadGraphControl.MadGraphUtils import *
#
gridpack=False
nevents=10000
mode=0

### DSID lists (extensions can include systematics samples) -> To be filled
decay_taunu=[]
for x in xrange(344250,344255): decay_taunu.append(x)


fcard = open('proc_card_mg5.dat','w')
if runArgs.runNumber in decay_taunu:
    fcard.write("""
    set group_subprocesses Auto
    set ignore_six_quark_processes False   
    set loop_optimized_output True
    set complex_mass_scheme False
    import model sm
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    define l+ = e+ mu+
    define l- = e- mu-
    define vl = ve vm vt
    define vl~ = ve~ vm~ vt~
    import model 2HDMtypeII
    generate p p > W+ b H- b~ / h1 h3 h2
    output -f""")
    fcard.close()
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

#--------------------------------------------------------------
# Charge Higgs, and all other masses in GeV
#--------------------------------------------------------------
###
mhc_str = str(runArgs.jobConfig[0])  # JOB OPTION NAME MUST CONTAIN THE MASS WE WANT TO SIMULATE IN FORMAT LIKE: *_H400_*
mhc=0
int(s) 
for s in mhc_str.split("_"):
    ss=s.replace("H","")  
    if ss.isdigit():
        mhc = int(ss)        
if mhc==0:
   raise RuntimeError("Charged Higgs mass not set, mhc=0, check joOption name %s"%mhc_str)
###
import math
mh1=1.250e+02                 
mh2=math.sqrt(math.pow(mhc,2)+math.pow(8.0399e+01,2)) 
mh3=mh2
do_newtheory=1
#--------------------------------------------------------------
# Scale -> fixed at (mt+mhc)/3
#--------------------------------------------------------------
if do_newtheory:
   scale=(mhc/3)
else:
   scale=(mhc+1.725e+02)/3


extras = { 'lhe_version':'3.0',                        # These two are not present in NLO cards, 
           'cut_decays':'F',                           # lhe version fixed by arrange_output function   
           'pdlabel':"'nn23lo'",
           'parton_shower':'PYTHIA8',
           'fixed_ren_scale':'T',
           'fixed_fac_scale':'T',
           'muR_ref_fixed':str(scale), 
           'muF1_ref_fixed':str(scale), 
           'muF2_ref_fixed':str(scale) 
           }
process_dir = new_process()
build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

#Build param_card.dat from UFO, and set Higgs masses
#write_param_card is part of the 2HDMTypeII model
import os
import sys
sys.path.append( os.environ['MADPATH']+'/models/2HDMtypeII/' )
from write_param_card import ParamCardWriter
ParamCardWriter('param_card.TMP.dat')
masses = {'25':str(mh1)+'  #  mh1',
          '35':str(mh2)+'  #  mh2',
          '36':str(mh2)+'  #  mh2',
          '37':str(mhc)+'  #  mhc'
          }
build_param_card(param_card_old='param_card.TMP.dat',param_card_new='param_card.dat',masses=masses) 

print_cards()

runName='run_01'     

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=mode,proc_dir=process_dir,run_name=runName)

arrange_output(run_name=runName,lhe_version=1,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')                                                                          

#### Shower     
#
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

evgenConfig.description = 'Madgraph Intermeidate mass charged Higgs'
evgenConfig.keywords+=['MSSM']                                                                                                                
evgenConfig.contact = ['H. Hadavand']

evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

dotaunu=False
for s in mhc_str.split("_"):
    ss=s.replace(".py","")  
    print ss
    if ss=='taunu' and runArgs.runNumber in decay_taunu:
        dotaunu=True

if dotaunu==False:
    raise RuntimeError("No decay mode was identified, check jobOption name %s, and/or runNumber %i."%(runArgs.jobConfig[0],runArgs.runNumber))

if dotaunu:
    genSeq.Pythia8.Commands += ["Higgs:useBSM = on",
                                "37:onMode = off",                   # turn off all mhc decays
                                "37:onIfMatch = 15 16"]              # switch on H+ to taunu
    

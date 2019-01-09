#--------------------------------------------------------------
# on-the-fly generation of H+ MG5 events, montoya@cern.ch
# mc16: pawel.bruckman.de.renstrom@cern.ch
#--------------------------------------------------------------
from MadGraphControl.MadGraphUtils import *
#
gridpack=False
#nevents=10000
nevents=20*(runArgs.maxEvents)
mode=0

###
mhc_str = str(runArgs.jobConfig[0])  # get the main JO name string
dotaunu=False
for s in mhc_str.split("_"):
    ss=s.replace(".py","")
    print ss
    if ss=='taunu':
        dotaunu=True


fcard = open('proc_card_mg5.dat','w')
if dotaunu:
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
# JOB OPTION NAME MUST CONTAIN THE MASS WE WANT TO SIMULATE IN FORMAT LIKE: *_H400_*
mhc=0
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

#--------------------------------------------------------------
# Scale -> fixed at 125 GeV  (recommended by Marco Zaro)
#--------------------------------------------------------------
scale=1.25e+02


extras = { 'lhe_version':'3.0',                        # These two are not present in NLO cards, 
           'cut_decays':'F',                           # lhe version fixed by arrange_output function   
           'pdlabel'      : "'lhapdf'",
           'lhaid'        : 262400,
##           'parton_shower':'PYTHIA8',
           'fixed_ren_scale':'T',
           'fixed_fac_scale':'T',
           'scale':str(scale),                         # fixed ren scale
           'dsqrt_q2fact1':str(scale),                 # fixed fact scale for pdf1
           'dsqrt_q2fact2':str(scale),                 # fixed fact scale for pdf2
           'use_syst'     : 'True',
           'sys_scalefact':'1.0 0.5 2.0',
           'sys_pdf'      : 'NNPDF30_lo_as_0118_nf_4'
           }
process_dir = new_process()
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

#Build param_card.dat from UFO, and set Higgs masses
#write_param_card is part of the 2HDMTypeII model
import os

masses = {'25':str(mh1)+'  #  mh1',
          '35':str(mh2)+'  #  mh2',
          '36':str(mh2)+'  #  mh2',
          '37':str(mhc)+'  #  mhc'
          }
#build_param_card(param_card_old='param_card.TMP.dat',param_card_new='param_card.dat',masses=masses) 
build_param_card(param_card_old=process_dir+'/Cards/param_card.dat',param_card_new='param_card.dat',masses=masses)

print_cards()

runName='run_01'     

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=mode,proc_dir=process_dir,run_name=runName)

outputDS=arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)

#### Shower 
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

evgenConfig.description = 'Madgraph Intermeidate mass charged Higgs'
evgenConfig.keywords+=['MSSM']
#evgenConfig.contact = ['H. Hadavand']
evgenConfig.contact = ['Pawel Bruckman <pawel.bruckman.de.renstrom@cern.ch>']

#evgenConfig.inputfilecheck = runName
#runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
runArgs.inputGeneratorFile=outputDS


if dotaunu==False:
    raise RuntimeError("No decay mode was identified, check jobOption name %s, and/or runNumber %i."%(runArgs.jobConfig[0],runArgs.runNumber))

if dotaunu:
    genSeq.Pythia8.Commands += ["Higgs:useBSM = on",
                                "37:onMode = off",                   # turn off all mhc decays
                                "37:onIfMatch = 15 16"]              # switch on H+ to taunu
    include ('GeneratorFilters/HplusTauNu_Filter_Fragment.py')         # filter for tau, jets, leptons, MET

#! /usr/bin/env pythonrun
from MadGraphControl.MadGraphUtils import *

#---------------------------------------------------------------------------------------------------                                               
# Set parameters                                                                                                                                   
#---------------------------------------------------------------------------------------------------                                               
lhe_version = 3.0




extras = {'auto_ptj_mjj':'False',
          'maxjetflavor':'4',
          'event_norm':'sum',
          'pdlabel':'lhapdf',
          'lhaid':315200,
          'cut_decays':'F', 
          'dynamical_scale_choice':3,
          'use_syst' :'T',
          'sys_scalefact' :'None',
          'sys_alpsfact' : 'None',
          'sys_pdf' : 'NNPDF31_lo_as_0130',
          }

runName='run_01'     
parameters = {'DECAY':{'Wv1':width},'MASS':{'Mv1':mass}}
#---------------------------------------------------------------------------------------------------                                               
# MadGraph process                                                                                                                                 
#---------------------------------------------------------------------------------------------------                                               
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model Top-Philic_UFO_V1
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
generate p p > t t~ v1/v1, v1 > t t~
output -f""")
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
  beamEnergy = runArgs.ecmEnergy / 2.
else:
  raise RuntimeError("No center of mass energy found.")

# this could be changed if event multipliers were ever needed
nevents=runArgs.maxEvents

if (nevents <0):
  nevents=5000

process_dir = new_process()

#---------------------------------------------------------------------------------------------------                                               
# Build run card                                                                                                                                   
#---------------------------------------------------------------------------------------------------                                               
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

#---------------------------------------------------------------------------------------------------                                               
# Build parameter card                                                                                                                             
#---------------------------------------------------------------------------------------------------                                               
build_param_card(param_card_old='MadGraph_param_card_4topres.dat',param_card_new='param_card.dat',params=parameters)

print_cards()

mode=0
runName='run_01'
generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=mode,proc_dir=process_dir,run_name=runName)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=lhe_version)





# Some more information
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
evgenConfig.description = "pp -> ttg, g->tt signal point"
evgenConfig.keywords = ["exotic", "BSM", "RandallSundrum", "warpedED"]
evgenConfig.contact = ["James Ferrando <james.ferrando@desy.de>"]
evgenConfig.process = "pp>ttg> tttt"  # e.g. pp>G*>WW>qqqq

# Finally, run the parton shower...
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

# ...and pull in MadGraph-specific stuff
include("Pythia8_i/Pythia8_MadGraph.py")


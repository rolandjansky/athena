###############################################################
#
# Job options file for Evgen MadGraph Pythia8 Monopole Generation
# A. Lionti, 2015-09-22
#==============================================================

#--------------------------------------------------------------
# MadGraph on the fly
#--------------------------------------------------------------
from MadGraphControl.MadGraphUtils import *

if hasattr(runArgs,'ecmEnergy'):
  beamEnergy = runArgs.ecmEnergy / 2.
else:
  beamEnergy = 6500.

safefactor=1.1 #generate extra 10% events in case any fail showering
nevents = 5000*safefactor
if runArgs.maxEvents > 0: nevents = runArgs.maxEvents*safefactor


masses = {}
masses['4110000'] = float(mass)


# writing proc card for MG
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model mono_spinhalf
generate p p > mm+ mm-
output -f
""")
fcard.close()

#Process_dir
process_dir = new_process()

#create the param_card
if os.access('param_card.dat',os.R_OK):
  print("Deleting old param_card.dat")
  os.remove('param_card.dat')
param_card = "%s/Cards/param_card.dat" % process_dir
	
param_card_extras={'GCH':{'GCH':'%s' % gcharge} }

if( build_param_card(param_card_old=param_card,param_card_new='param_card.dat',masses=masses,params=param_card_extras) == -1):
  raise RuntimeError("Could not create param_card.dat")
	
#create the run card:i
run_card_extras = {'lhe_version':'1.0' , 'ptheavy':'%s' % ptcut}

if os.access('run_card.dat',os.R_OK):
  print("Deleting old run_card.dat")
  os.remove('run_card.dat')
run_card = get_default_runcard(proc_dir=process_dir)
if build_run_card(run_card_old=run_card,run_card_new='run_card.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=run_card_extras)==-1:
  raise RuntimeError("Could not create run_card.dat")


# generating events in MG
generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir=process_dir)

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraphPythia8EvtGen_A14NNPDF23LO_DYMonopole'
	
arrange_output(run_name='Test',proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')

evgenConfig.inputfilecheck = stringy		
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'


#--------------------------------------------------------------
# General MC15 configuration
#--------------------------------------------------------------
include( "Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py" )
include( "Pythia8_i/Pythia8_MadGraph.py" )

#--------------------------------------------------------------
# Configuration for EvgenJobTransforms
#--------------------------------------------------------------
evgenConfig.description = "Drell-Yan magnetic monopole generation for Mass=%s, Gcharge=%s with MadGraph+Pythia8 and the A14_NNPDF23LO tune in MC15" % (mass,gcharge)
evgenConfig.keywords = ["exotic", "magneticMonopole", "drellYan", "BSM"]
evgenConfig.contact = ["anlionti@cern.ch"]
evgenConfig.process = "p p -> mm+ mm-"
evgenConfig.specialConfig = 'MASS=%s;GCHARGE=%s;preInclude=SimulationJobOptions/preInclude.Monopole.py' % (mass,gcharge)

#--------------------------------------------------------------
# Edit PDGTABLE.MeV with monopole mass
#--------------------------------------------------------------
ALINE1="M 4110000                          %s.E+03       +0.0E+00 -0.0E+00 Monopole        0" % (mass)
ALINE2="W 4110000                          0.E+00         +0.0E+00 -0.0E+00 Monopole        0"

import os
import sys

pdgmod = os.path.isfile('PDGTABLE.MeV')
if pdgmod is True:
    os.remove('PDGTABLE.MeV')
os.system('get_files -data PDGTABLE.MeV')
f=open('PDGTABLE.MeV','a')
f.writelines(str(ALINE1))
f.writelines('\n')
f.writelines(str(ALINE2))
f.writelines('\n')
f.close()

del ALINE1
del ALINE2

#--------------------------------------------------------------
# Edit G4particle_whitelist.txt with monopole
#--------------------------------------------------------------

ALINE1="4110000   mm  %s.E+06 (Mev/c) lepton %s" % (mass,gcharge)
ALINE2="-4110000  mmbar  %s.E+06 (Mev/c) lepton -%s" % (mass,gcharge)

import os
import sys

pdgmod = os.path.isfile('G4particle_whitelist.txt')
if pdgmod is True:
    os.remove('G4particle_whitelist.txt')
os.system('get_files -data G4particle_whitelist.txt')
f=open('G4particle_whitelist.txt','a')
f.writelines(str(ALINE1))
f.writelines('\n')
f.writelines(str(ALINE2))
f.writelines('\n')
f.close()

del ALINE1
del ALINE2


































































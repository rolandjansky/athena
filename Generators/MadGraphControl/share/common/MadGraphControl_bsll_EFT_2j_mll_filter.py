import re
import os
import math
import subprocess

from MadGraphControl.MadGraphUtils import *

nevents = 1500000


JOname = runArgs.jobConfig[0]





fcard = open('proc_card_mg5.dat', 'w')
fcard.write("""
import model bsll_CI --modelname
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
generate p p > mu+ mu- 
add process p p > mu+ mu- j 
add process p p > mu+ mu- j j 
output -f""")
fcard.close()



evgenLog.info('DiMuon Filter and DiLepton Mass Filter is applied')

include ( 'GeneratorFilters/MultiMuonFilter.py' )
filtSeq.MultiMuonFilter = filtSeq.MultiMuonFilter
filtSeq.MultiMuonFilter.Ptcut = 10000.
filtSeq.MultiMuonFilter.Etacut = 2.5
filtSeq.MultiMuonFilter.NMuons = 2

include ( 'GeneratorFilters/DiLeptonMassFilter.py' )
filtSeq.DiLeptonMassFilter.MinPt = 10000.
filtSeq.DiLeptonMassFilter.MaxEta = 2.5
filtSeq.DiLeptonMassFilter.MinMass = mll_min
filtSeq.DiLeptonMassFilter.MaxMass = mll_max
filtSeq.DiLeptonMassFilter.AllowSameCharge = False

filtSeq.Expression = "MultiMuonFilter and DiLeptonMassFilter"    



beamEnergy = -999
if hasattr(runArgs, 'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

process_dir = new_process()
extras = {'pdlabel': "'lhapdf'",
          'lhaid'      : '247000',
          'ickkw'      : '0',
          'ktdurham'   : '400.0',
          'drjj'   : '0.4',
          'ptj'   : '20',
          'ptl'   : '10',
          'maxjetflavor': '5'}

try:
    os.remove('run_card.dat')
except OSError:
    pass

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir), run_card_new='run_card.dat',
               nevts=nevents, rand_seed=runArgs.randomSeed, beamEnergy=beamEnergy, extras=extras)

if os.path.exists("param_card.dat"):
    os.remove("param_card.dat")


param_card_name = 'param_card_EFT_bsll_5FS.dat'
param_card = subprocess.Popen(['get_files', '-data', param_card_name])
param_card.wait()
if not os.access(param_card_name, os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open(param_card_name, 'r')

    newcard = open('param_card.dat', 'w')
    cmubs = -1.0

    for line in oldcard:
        if '# cmubs' in line:
            newcard.write('      1 %e # cmubs\n' % (cmubs))
        elif '# celbs' in line:
            newcard.write('      2 0.000000e+00 # celbs\n')
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

print_cards()

runName = 'run_01'
process_dir = new_process()
generate(run_card_loc='run_card.dat',
         param_card_loc='param_card.dat',
         mode=0,
         proc_dir=process_dir,
		 nevents=nevents,
		 random_seed=runArgs.randomSeed,
         run_name=runName)
		 
arrange_output(run_name=runName, proc_dir=process_dir, outputDS=runName + '._00001.events.tar.gz', lhe_version=3,
               saveProcDir=True)

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

PYTHIA8_nJetMax=2                                                                                                                                                       
PYTHIA8_Dparameter=0.4                                                                                                                                                 
PYTHIA8_Process="pp>mu+mu-"                                                                                                                                                       
PYTHIA8_TMS=400                                                                                                                                                          
PYTHIA8_nQuarksMerge=5                                 
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")



evgenConfig.description = 'Production of b->sll EFT, Lambda = 1 TeV'
evgenConfig.keywords += ['BSM', 'exotic']
evgenConfig.generators += ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.process = 'pp -> llb'
evgenConfig.minevents = 5000
evgenConfig.tune = 'A14 NNPDF23LO'
evgenConfig.contact = ["Yoav Afik <yafik@cern.ch>"]
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile = runName + '._00001.events.tar.gz'

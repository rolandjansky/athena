from MadGraphControl.MadGraphUtils import *

#
## 2 inputs parameters, mass of Zp (zpm), 1 coupling gzpmul
##
## Example 1: 15.0GeV Zp with coupling to muon only pp --> 2mu+Zp --> 4mu
##
## zpm=15.0
## gzpmul=3.000000e-01
##
gzpel=0.000000e+00
gzper=0.000000e+00
gzpmur=gzpmul
gzptal=0.000000e+00
gzptar=0.000000e+00
maxjetflavor=5
## safe factor applied to nevents, to account for the filter efficiency
safefactor=1.1

fcard = open('proc_card_mg5.dat', 'w')
fcard.write("""
import model Leptophilic_UFO
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
set group_subprocesses Auto
set ignore_six_quark_processes False
set gauge unitary
set complex_mass_scheme False
generate p p > mu+ mu- Zp, Zp > mu+ mu-
output -f
""")
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
  beamEnergy = runArgs.ecmEnergy / 2.
else: 
  raise RuntimeError("No center of mass energy found.")

#---------------------------------------------------------------------------
# MG5 run Card
#---------------------------------------------------------------------------
#
# ptj default value 20GeV, using ktdurham just in case its lower than 20GeV 
#
extras = {
    'pdlabel'      : "'lhapdf'",
    'lhaid'        : '260000',
    'lhe_version'  : '3.0',
    'maxjetflavor' : maxjetflavor,
    'asrwgtflavor' : maxjetflavor,
    'cut_decays'   : 'F',
    'use_syst'     : 'T',
    'sys_scalefact' :'1 0.5 2',
    'sys_alpsfact' : 'None',
    'sys_matchscale' : 'None',
    'sys_pdf'      : 'None',#NNPDF23_lo_as_0130_qed'
}

process_dir = new_process()

build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat',
               nevts=int(runArgs.maxEvents*safefactor),rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy, extras=extras)

#---------------------------------------------------------------------------
# MG5 param Card
#---------------------------------------------------------------------------
paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_Leptophilic_muOnly.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_Leptophilic_muOnly.dat',os.R_OK):
    print 'ERROR: Could not get param card'
## params is a dictionary of dictionaries (each dictionary is a 
## separate block)
## parameters for newcoup
params={}
ncoups={}
ncoups['GZpeL']=gzpel
ncoups['GZpeR']=gzper
ncoups['GZpmuL']=gzpmul
ncoups['GZpmuR']=gzpmur
ncoups['GZptaL']=gzptal
ncoups['GZptaR']=gzptar
params['ZPINPUTS']=ncoups
## mass parameters
masses={}
masses['999888']=zpm

str_param_card='param_card.dat'
build_param_card(param_card_old='MadGraph_param_card_Leptophilic_muOnly.dat',
                 param_card_new=str_param_card,
                 masses=masses,
                 params=params)  

print_cards()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc=str_param_card,mode=2,njobs=1,run_name=runName,proc_dir=process_dir)

outputDS=arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz', saveProcDir=True)                                                                         
import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts                                                                                                                                                                       
#### Shower                                                                                                                                                             
evgenConfig.description = "Leptophilic Zprime, https://arxiv.org/pdf/1411.7394.pdf"
evgenConfig.keywords = ["BSM","Zprime","4muon"]
evgenConfig.process = "pp --> 2mu+Zp --> 4mu around Z peak"
evgenConfig.contact = ["Tiesheng Dai <tiesheng.dai@cern.ch>"]

runArgs.inputGeneratorFile=outputDS

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

genSeq.Pythia8.Commands += [
                            '23:onMode = off'
                           ]


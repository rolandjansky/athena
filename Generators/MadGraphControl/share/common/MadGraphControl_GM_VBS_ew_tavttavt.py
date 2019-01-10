from MadGraphControl.MadGraphUtils import *

nevents=36000
mode=0

### DSID lists
DSIDDic = {305788:'200',305789:'300',305790:'400',305791:'500',305792:'600',305793:'700',305794:'800',305795:'900',305796:'1000'}
mass = DSIDDic[runArgs.runNumber]

### Cards
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    define vl = ve vm vt
    define vl~ = ve~ vm~ vt~
    import model GM_UFO
    generate    p p > H5pp  > w+ w+ j j QCD=0, w+ > l+ vl , w+ > ta+ vt  @1
    add process p p > H5pp~ > w- w- j j QCD=0, w- > l- vl~, w- > ta- vt~ @2
    output -f
    """)
fcard.close()

extras = {
    'pdlabel':"'lhapdf'",
    'lhaid':"247000",
    'ptj':"15",
    'pta':"0",
    'ptb':"0",
    'etal':"2.7",
    'drjj':"0",
    'drll':"0",
    'draa':"0",
    'draj':"0",
    'drjl':"0",
    'dral':"0",
    'sys_pdf':"NNPDF23_lo_as_0130_qed.LHgrid" }
    
build_run_card(run_card_old='MadGraph_run_card_GM.dat',run_card_new='run_card.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=runArgs.ecmEnergy/2.,extras=extras)

print_cards()
    
runName='GM_VBS_H5pp_ew_lvlv_'+mass+'_ISHdot5_qcd0'

process_dir = new_process(card_loc='proc_card_mg5.dat')

evgenConfig.generators = ["MadGraph"]

generate(run_card_loc='run_card.dat',param_card_loc='MadGraph_param_card_GMm'+mass+'.dat',mode=mode,proc_dir=process_dir,run_name=runName)

############################
# Shower JOs will go here

import os
if 'ATHENA_PROC_NUMBER' in os.environ:
   njobs = os.environ.pop('ATHENA_PROC_NUMBER')
   # Try to modify the opts underfoot
   if not hasattr(opts,'nprocs'): print 'Warning: Did not see option!'
   else: opts.nprocs = 0
   print opts

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

include("Pythia8_i/Pythia8_A14_NNPDF23LO_Common.py")
include("Pythia8_i/Pythia8_LHEF.py")
include("Pythia8_i/Pythia8_EvtGen.py")
evgenConfig.generators += ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.contact = ["Jerome Claude <jerome.claude@cern.ch>"]
genSeq.Pythia8.Commands += [
    "255:all H++ H-- 1 6 0 500",
    "Init:showAllParticleData = on",
    "Next:numberShowLHA = 10",
    "Next:numberShowEvent = 10"]
evgenConfig.description = 'MadGraph_GM_VBS_H5pp_ISH_0.5_ta+l+X'
evgenConfig.keywords+=['BSM','VBS','Higgs','WW','leptonic']
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'                     

############################
# Filter

include('GeneratorFilters/MultiLeptonFilter.py')
MultiLeptonFilter = filtSeq.MultiLeptonFilter
MultiLeptonFilter.Ptcut = 5000.
MultiLeptonFilter.Etacut = 2.7
MultiLeptonFilter.NLeptons = 2


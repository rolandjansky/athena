from MadGraphControl.MadGraphUtils import *

nevents=45000
mode=0

### DSID lists
DSIDDic = {307181:'200',307182:'300',307183:'400',307184:'500',307185:'600',307186:'700',307187:'800',307188:'900',307189:'1000'}
mass = DSIDDic[runArgs.runNumber]

### Cards
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
    define l+ = e+ mu+ 
    define l- = e- mu- 
    define vl = ve vm 
    define vl~ = ve~ vm~
    import model GM_UFO
    generate    p p > H5p  > w+ z j j QCD=0, w+ > ta+ vt , z > l+ l- @1
    add process p p > H5p~ > w- z j j QCD=0, w- > ta- vt~, z > l+ l- @2
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
    
runName='GM_VBS_H5p_ew_tauvtll_'+mass+'_ISHdot5_qcd0'

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
evgenConfig.contact = ["Benjamin Freund <benjamin.freund@cern.ch>"]
genSeq.Pythia8.Commands += [
    "Init:showAllParticleData = on",
    "Next:numberShowLHA = 10",
    "Next:numberShowEvent = 10"]
evgenConfig.description = 'MadGraph_GM_VBS_H5p_ISH_0.5_ta+l+l'
evgenConfig.keywords+=['BSM','VBS','Higgs','WZ','leptonic']
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'                     

############################
# Filter

include('GeneratorFilters/MultiLeptonFilter.py')
MultiLeptonFilter = filtSeq.MultiLeptonFilter
MultiLeptonFilter.Ptcut = 3500.
MultiLeptonFilter.Etacut = 2.7
MultiLeptonFilter.NLeptons = 3

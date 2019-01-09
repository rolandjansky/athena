from MadGraphControl.MadGraphUtils import *

nevents=36000
mode=0

### DSID lists
DSIDDic = {307779:'200',307780:'250',307781:'300',307782:'350',307783:'400',307784:'450',307785:'500',307786:'600',307787:'700',307788:'800',307789:'900',307790:'1000'}
mass = DSIDDic[runArgs.runNumber]

### Cards
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    define vl = ve vm vt
    define vl~ = ve~ vm~ vt~
    import model GM_UFO
    generate    p p > H5z j j QCD=0, (H5z > w+ w-, w+ > l+ vl,  w- > l- vl~)
    output -f
    """)
fcard.close()

extras = {
    'pdlabel':"'lhapdf'",
    'lhaid':"262000",
    'ptj':"15",
    'pta':"0",
    'ptb':"0",
    'ptl':"15",
    'etaj':"5",
    'etal':"2.7",
    'drjj':"1",
    'drll':"0",
    'draa':"0",
    'draj':"0",
    'drjl':"0",
    'dral':"0",
    'mmjj':"150",
    'sys_pdf':"NNPDF30_lo_as_0118.LHgrid" }
    
build_run_card(run_card_old='MadGraph_run_card_GM.dat',run_card_new='run_card.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=runArgs.ecmEnergy/2.,extras=extras)

print_cards()
    
runName='GM_VBF_H5z_WWlvlv_ew_m'+mass

process_dir = new_process(card_loc='proc_card_mg5.dat')

evgenConfig.generators = ["MadGraph"]

generate(run_card_loc='run_card.dat',param_card_loc='MadGraph_param_card_GMVBF_WWlvlv_m'+mass+'.dat',mode=mode,proc_dir=process_dir,run_name=runName)

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

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")
evgenConfig.generators += ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.contact = ["Guangyi Zhang <g.zhang@cern.ch>"]
genSeq.Pythia8.Commands += [
    "Init:showAllParticleData = on",
    "Next:numberShowLHA = 10",
    "Next:numberShowEvent = 10"]
evgenConfig.description = 'MadGraph_GM_VBF_H5z'
evgenConfig.keywords+=['BSM','VBF','WW','leptonic']
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'                     

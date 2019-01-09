from MadGraphControl.MadGraphUtils import *
mode=0

### DSID lists (extensions can include systematics samples)
name = runArgs.jobConfig[0]
dsid = int(name.split(".")[1])
test = [dsid]

fcard = open('proc_card_mg5.dat','w')
if runArgs.runNumber in test:
    fcard.write("""
    import model sm
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    generate p p > j j @0
    add process p p > j j j @1
    add process p p > j j j j @2
    output -f""")
    fcard.close()
else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

process_dir = new_process()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

# HT slice boundaries
HTDict = {0:0,1:100,2:200,3:300,4:500,5:700,6:1000,7:1500,8:2000,9:4000,10:6000,11:8000,12:13000}
evt_multiplier = 50

if runArgs.maxEvents > 0:
    nevents=runArgs.maxEvents*evt_multiplier
else:
    nevents=5000*evt_multiplier

name_info = name.split("CKKW")[1].split(".py")[0].split("_HT")
slice = int(name_info[1]) 
ktdurham = int(name_info[0])

# Merging info
maxjetflavor=4
njetmax=2
dparameter=0.4
processname="pp>jj"

#Fetch default LO run_card.dat and set parameters
extras = { 'run_tag'      : "'CMS_NNPDF30NLO'",
           'pdlabel'      : "'lhapdf'",
           'lhaid'        : 260000,
           'ickkw'        : 0,
           'pdfwgt'       : 'T',
           'asrwgtflavor' : maxjetflavor,
           'lhe_version'  : 3.0,
           'ptj'          : 0.0,
           'drjj'         : 0.0 ,
           'drjl'         : 0.4,
           'maxjetflavor' : maxjetflavor,
           'ktdurham'     : ktdurham,
           'dparameter'   : dparameter,
           'htjmin'       : HTDict[slice-1],
           'htjmax'       : HTDict[slice],
           'use_syst'     :'T',
           'sys_scalefact' :'1 0.5 2',
           'sys_alpsfact' : 'None',
           'sys_matchscale' : 'None',
           'sys_pdf'      : 'None',#NNPDF23_lo_as_0130_qed',
#           'dynamical_scale_choice' : 0,
           'auto_ptj_mjj' : 'F'
           }

runName='run_01'
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras,xqcut=10)

print_cards()

generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,run_name=runName)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

# Commands for CKKW Matching in Pythia
PYTHIA8_nJetMax=njetmax
PYTHIA8_TMS=ktdurham
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=processname
PYTHIA8_nQuarksMerge=maxjetflavor

include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")

# Leading jet pT filter
include("GeneratorFilters/JetFilterAkt4.py")
ptcut = 1.5*ktdurham
filtSeq.QCDTruthJetFilter.MinPt = ptcut*GeV

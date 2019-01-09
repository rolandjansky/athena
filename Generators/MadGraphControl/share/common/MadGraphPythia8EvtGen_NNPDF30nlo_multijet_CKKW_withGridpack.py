from MadGraphControl.MadGraphUtils import *
mode=0

name = runArgs.jobConfig[0]
dsid = int(name.split(".")[1])
test = [dsid]

gridpack_dir='madevent/'
gridpack_mode=True
runName=str(dsid)


fcard = open('proc_card_mg5.dat','w')
if runArgs.runNumber in test:
    fcard.write("""
    import model sm-no_b_mass
    define p = g u c d s b u~ c~ d~ s~ b~
    define j = g u c d s b u~ c~ d~ s~ b~
    generate p p > j j @0
    add process p p > j j j @1
    add process p p > j j j j @2
    output -f""")
    fcard.close()
else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

process_dir = new_process(grid_pack="madevent/")

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

# HT slice boundaries
HTDict = {0:0,1:100,2:200,3:300,4:500,5:700,6:1000,7:1500,8:2000,9:4000,10:6000,11:8000,12:13000}
multDict = {0:1.1,1:1.2,2:2.2,3:4.0,4:5.0,5:6.0,6:7.0,7:8.0,8:9.0,9:10.0,10:10.0,11:10.0,12:10.0}

name_info = name.split("CKKW")[1].split(".py")[0].split("_HT")
slice = int(name_info[1]) 
mergingscale = int(name_info[0])
ktdurham = mergingscale/2

evt_multiplier = multDict[slice]

if runArgs.maxEvents > 0:
    nevents=runArgs.maxEvents*evt_multiplier
else:
    nevents=5000*evt_multiplier


# Merging info
maxjetflavor=5
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
           'use_syst' :'T',
           'sys_scalefact' :'0.5 1 2',
           'sys_alpsfact' : 'None',
           'sys_matchscale' : 'None',
           'dynamical_scale_choice' : 1,
           'auto_ptj_mjj' : 'F'
           }

runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras,xqcut=0)

print_cards()

generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,run_name=runName,grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=runArgs.randomSeed,required_accuracy=0.001)


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(runName)
outputDS=arrange_output(run_name=runName,proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)
runArgs.inputGeneratorFile=outputDS 


include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

# Commands for CKKW Matching in Pythia
PYTHIA8_nJetMax=njetmax
PYTHIA8_TMS=mergingscale
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=processname
PYTHIA8_nQuarksMerge=maxjetflavor

include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")
genSeq.Pythia8.Commands += ["Merging:usePythiaQRenHard = off",
                            "Merging:usePythiaQFacHard = off"]


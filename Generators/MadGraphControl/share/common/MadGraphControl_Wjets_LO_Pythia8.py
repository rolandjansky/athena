from MadGraphControl.MadGraphUtils import *

# General settings
nevents=150000
mode=0
nJobs=1
gridpack_dir=None
gridpack_mode=False
cluster_type=None
cluster_queue=None


# Merging settings
maxjetflavor=5
ickkw=0
nJetMax=4
ktdurham=30
dparameter=0.4


### DSID lists (extensions can include systematics samples)
Wenu_5fl_Np0=[361520]
Wenu_5fl_Np1=[361521]
Wenu_5fl_Np2=[361522]
Wenu_5fl_Np3=[361523]
Wenu_5fl_Np4=[361524]
Wmunu_5fl_Np0=[361525]
Wmunu_5fl_Np1=[361526]
Wmunu_5fl_Np2=[361527]
Wmunu_5fl_Np3=[361528]
Wmunu_5fl_Np4=[361529]
Wtaunu_5fl_Np0=[361530]
Wtaunu_5fl_Np1=[361531]
Wtaunu_5fl_Np2=[361532]
Wtaunu_5fl_Np3=[361533]
Wtaunu_5fl_Np4=[361534]



### Electron
if runArgs.runNumber in Wenu_5fl_Np0:
    mgproc="generate p p > eall veall @0"
    name='Wenu_Np0'
    process="pp>LEPTONS,NEUTRINOS"
elif runArgs.runNumber in Wenu_5fl_Np1:
    mgproc="generate p p > eall veall j @1"
    name='Wenu_Np1'
    process="pp>LEPTONS,NEUTRINOS"
elif runArgs.runNumber in Wenu_5fl_Np2:
    mgproc="generate p p > eall veall j j @2"
    name='Wenu_Np2'
    process="pp>LEPTONS,NEUTRINOS"
elif runArgs.runNumber in Wenu_5fl_Np3:
    mgproc="generate p p > eall veall j j j @3"
    name='Wenu_Np3'
    process="pp>LEPTONS,NEUTRINOS"
elif runArgs.runNumber in Wenu_5fl_Np4:
    mgproc="generate p p > eall veall j j j j @4"
    name='Wenu_Np4'
    process="pp>LEPTONS,NEUTRINOS"  
    nevents=10000

### Muon 
elif runArgs.runNumber in Wmunu_5fl_Np0:
    mgproc="generate p p > muall vmall @0"
    name='Wmunu_Np0'
    process="pp>LEPTONS,NEUTRINOS"
elif runArgs.runNumber in Wmunu_5fl_Np1:
    mgproc="generate p p > muall vmall j @1"
    name='Wmunu_Np1'
    process="pp>LEPTONS,NEUTRINOS"
elif runArgs.runNumber in Wmunu_5fl_Np2:
    mgproc="generate p p > muall vmall j j @2"
    name='Wmunu_Np2'
    process="pp>LEPTONS,NEUTRINOS"
elif runArgs.runNumber in Wmunu_5fl_Np3:
    mgproc="generate p p > muall vmall j j j @3"
    name='Wmunu_Np3'
    process="pp>LEPTONS,NEUTRINOS"
elif runArgs.runNumber in Wmunu_5fl_Np4:
    mgproc="generate p p > muall vmall j j j j @4"
    name='Wmunu_Np4'
    process="pp>LEPTONS,NEUTRINOS" 
    nevents=10000
    
### Tau
elif runArgs.runNumber in Wtaunu_5fl_Np0:
    mgproc="generate p p > taall vtall @0"
    name='Wtaunu_Np0'
    process="pp>LEPTONS,NEUTRINOS"
elif runArgs.runNumber in Wtaunu_5fl_Np1:
    mgproc="generate p p > taall vtall j @1"
    name='Wtaunu_Np1'
    process="pp>LEPTONS,NEUTRINOS"
elif runArgs.runNumber in Wtaunu_5fl_Np2:
    mgproc="generate p p > taall vtall j j @2"
    name='Wtaunu_Np2'
    process="pp>LEPTONS,NEUTRINOS"
elif runArgs.runNumber in Wtaunu_5fl_Np3:
    mgproc="generate p p > taall vtall j j j @3"
    name='Wtaunu_Np3'
    process="pp>LEPTONS,NEUTRINOS"
elif runArgs.runNumber in Wtaunu_5fl_Np4:
    mgproc="generate p p > taall vtall j j j j @4"
    name='Wtaunu_Np4'
    process="pp>LEPTONS,NEUTRINOS"   
    nevents=10000
    
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm-no_b_mass
define eall = e+ e-
define veall = ve ve~
define muall = mu+ mu-
define vmall = vm vm~
define taall = ta+ ta-
define vtall = vt vt~
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
"""+mgproc+"""
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


process_dir = new_process(grid_pack=gridpack_dir)

#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version'  : '2.0',
           'cut_decays'   : 'F', 
           'pdlabel'      : "'nn23lo1'",
#           'pdlabel'      : "'lhapdf'",
#           'lhaid'        : 247000,
           'maxjetflavor' : maxjetflavor,
           'asrwgtflavor' : maxjetflavor,
           'ickkw'        : 0,
           'ptj'          : 20,
           'ptb'          : 20,
           'mmjj'         : 0,
           'drjj'         : 0,
           'drll'         : 0,
           'drjl'         : 0.4,
           'ptl'          : 0,
           'etal'         : 10,
           'etab'         : 6,
           'etaj'         : 6,
           'ktdurham'     : ktdurham,    
           'dparameter'   : dparameter,
           'use_syst'     : 'F' }

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)
print_cards()

generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,njobs=nJobs,proc_dir=process_dir,
         grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,cluster_type=cluster_type,cluster_queue=cluster_queue,
         nevents=nevents,random_seed=runArgs.randomSeed)

arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')



#### Shower 
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=['Z','electron','jets','drellYan']
evgenConfig.inputfilecheck = stringy
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")    
include("Pythia8_i/Pythia8_MadGraph.py")


PYTHIA8_nJetMax=nJetMax
PYTHIA8_TMS=ktdurham
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=process                                                      
PYTHIA8_nQuarksMerge=maxjetflavor
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")                   

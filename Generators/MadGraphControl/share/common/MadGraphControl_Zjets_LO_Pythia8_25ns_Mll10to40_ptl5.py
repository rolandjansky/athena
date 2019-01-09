from MadGraphControl.MadGraphUtils import *

# General settings
nevents=150000
mode=0
nJobs=1
gridpack_dir=None
gridpack_mode=False
cluster_type=None
cluster_queue=None

# MG Particle cuts
mllcut=10
mmllmax=40
ptl=5

# Merging settings
maxjetflavor=5
ickkw=0
nJetMax=4
ktdurham=30      # 25ns change 30 -> 20
dparameter=0.2   # 25ns change 0.4 -> 0.1


### DSID lists (extensions can include filters, systematics samples, etc.)
Zee_5fl_NpX=[]     # all Z->ee slices should go here

Zmumu_5fl_NpX=[ 363725,363726,363727,363728,363729,363730,363731,363732,
                363733,363734,363735,363736,363737,363738,363739,363740,
                363741,363742,363743,363744,363745,363746,363747,363748 ]   # all Z->mumu slices should go here

Ztautau_5fl_NpX=[] # all Z->tautau slices should go here

Znunu_5fl_NpX=[]   # all Z->nunu slices should go here

nevents_forHT={'lowHT':100000,'midlowerHT':80000,'midHT':70000,'midupperHT':25000,
               'highlowerHT':15000,'highHT':2000}

### Electrons
if runArgs.runNumber in Zee_5fl_NpX:
    mgproc="""
generate p p > e+ e- @0
add process p p > e+ e- j @1
add process p p > e+ e- j j @2
add process p p > e+ e- j j j @3
add process p p > e+ e- j j j j @4
"""
    name='Zee_NpX'
    process="pp>e+e-"
    
    nevents=nevents_forHT[HTslice]
    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20

### Muons    
elif runArgs.runNumber in Zmumu_5fl_NpX:
    mgproc="""
generate p p > mu+ mu- @0
add process p p > mu+ mu- j @1
add process p p > mu+ mu- j j @2
add process p p > mu+ mu- j j j @3
add process p p > mu+ mu- j j j j @4
"""
    name='Zmumu_NpX'
    process="pp>mu+mu-"
    
    nevents=nevents_forHT[HTslice]
    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20
    
### Taus
elif runArgs.runNumber in Ztautau_5fl_NpX:
    mgproc="""
generate p p > ta+ ta- @0
add process p p > ta+ ta- j @1
add process p p > ta+ ta- j j @2
add process p p > ta+ ta- j j j @3
add process p p > ta+ ta- j j j j @4
"""
    name='Ztautau_NpX'
    process="pp>ta+ta-"
    
    nevents=nevents_forHT[HTslice]
    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20


### Neutrinos
elif runArgs.runNumber in Znunu_5fl_NpX:
    mgproc="""
generate p p > vl vl~ @0
add process p p > vl vl~ j @1
add process p p > vl vl~ j j @2
add process p p > vl vl~ j j j @3
add process p p > vl vl~ j j j j @4
"""
    name='Znunu_NpX'
    process="pp>NEUTRINOS"
    
    nevents=nevents_forHT[HTslice]
    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20
    
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm-no_b_mass
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

#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version'    : '3.0',
           'cut_decays'     : 'F', 
           'pdlabel'        : "'lhapdf'",
           'lhaid'          : 260000,  
           'maxjetflavor'   : maxjetflavor,
           'asrwgtflavor'   : maxjetflavor,
           'ickkw'          : 0,
           'ptj'            : 20,      
           'ptb'            : 20,      
           'mmll'           : mllcut,  
           'mmllmax'        : mmllmax,    
           'ihtmin'         : ihtmin,
           'ihtmax'         : ihtmax,
           'mmjj'           : 0,
           'drjj'           : 0,
           'drll'           : 0,
           'drjl'           : 0.,       
           'ptl'            : ptl,
           'etal'           : 10,
           'etab'           : 6,
           'etaj'           : 6,
           'ktdurham'       : ktdurham,    
           'dparameter'     : dparameter  }
##########################################################################################
process_dir = new_process(grid_pack=gridpack_dir)
build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat', 
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

genSeq.Pythia8.Commands += ["Merging:unorderedASscalePrescrip = 0"]

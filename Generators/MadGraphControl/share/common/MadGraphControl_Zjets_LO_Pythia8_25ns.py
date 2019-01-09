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
mllcut=40

# Merging settings
maxjetflavor=5
ickkw=0
nJetMax=4
ktdurham=30      
dparameter=0.2

### DSID lists (extensions can include filters, systematics samples, etc.)
Zee_5fl_NpX=[ 363147,363148,363149,363150,363151,363152,363153,363154,
              363155,363156,363157,363158,363159,363160,363161,363162,
              363163,363164,363165,363166,363167,363168,363169,363170 ]     

Zmumu_5fl_NpX=[ 363123,363124,363125,363126,363127,363128,363129,363130,
                363131,363132,363133,363134,363135,363136,363137,363138,
                363139,363140,363141,363142,363143,363144,363145,363146 ]   

Ztautau_5fl_NpX=[ 363171,363172,363173,363174,363175,363176,363177,363178,
                  363179,363180,363181,363182,363183,363184,363185,363186,
                  363187,363188,363189,363190,363191,363192,363193,363194 ] 

Znunu_5fl_NpX=[ 363195,363196,363197,363198,363199,363200,363201,363202,
                363203,363204,363205,363206,363207,363208,363209,363210,
                363211,363212,363213,363214,363215,363216,363217,363218 ]   

nevents_forHT={'lowHT':200000,'lowhighHT':70000,'midlowHT':50000,'midHT':20000,'midhighHT':15000,'highHT':8000}

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
    
    nevents=nevents_forHT[HTrange]
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
    
    nevents=nevents_forHT[HTrange]
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
    
    nevents=nevents_forHT[HTrange]
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
    
    nevents=nevents_forHT[HTrange]
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
           'ihtmin'         : ihtmin,
           'ihtmax'         : ihtmax,
           'mmjj'           : 0,
           'drjj'           : 0,
           'drll'           : 0,
           'drjl'           : 0.,       
           'ptl'            : 0,
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
evgenConfig.contact  = [ "nicolas.gilberto.gutierrez.ortiz@cern.ch" ]
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=['Z','electron','jets','drellYan']
evgenConfig.inputfilecheck = stringy
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'
if hasattr(runArgs,'outputTXTFile'):                                                                                                                         
    runArgs.inputGeneratorFile=runArgs.outputTXTFile
    evgenConfig.inputfilecheck=runArgs.outputTXTFile.split('.tar.gz')[0]  

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")    
include("Pythia8_i/Pythia8_MadGraph.py")

PYTHIA8_nJetMax=nJetMax
PYTHIA8_TMS=ktdurham
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=process                                                      
PYTHIA8_nQuarksMerge=maxjetflavor
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")                         

genSeq.Pythia8.Commands += ["Merging:unorderedASscalePrescrip = 0"]

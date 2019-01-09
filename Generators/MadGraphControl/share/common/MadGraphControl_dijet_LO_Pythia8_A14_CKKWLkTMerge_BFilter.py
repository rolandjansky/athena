from MadGraphControl.MadGraphUtils import *

# General settings
nevents=250000
mode=0
nJobs=1
gridpack_dir=None
gridpack_mode=False
cluster_type=None 
cluster_queue=None

alpsfact = 1.0
scalefact = 1.0

lhaid=260000

# MG Particle cuts
mllcut=-1
ptmin=0
ptmax=-1

# MG merging settings
maxjetflavor=5
ickkw=0

# Pythia8 merging settings
nJetMax=1
ktdurham=15
dparameter=0.2 #NEW 0.4->0.2


model="sm-no_b_mass"
pjdef="""
define p = p b b~
define j = p
"""

### DSID lists (extensions can include systematics samples)
dijet_5fl=[300227]
dijet_4fl=[300228]

if runArgs.runNumber in dijet_5fl:
    mgproc="""
generate p p > j j @0
add process p p > j j j @1
"""
    name='dijet'
    process="pp>jj"
    lhaid=260000
elif runArgs.runNumber in dijet_4fl:
    model="sm"
    mgproc="""
generate p p > b b~ @0
add process p p > b b~ j @1
"""
    name='dibjet'
    process="pp>bb~"
    pjdef=""
    maxjetflavor=4
    lhaid=260400
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model """+model+"""
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
"""+pjdef+"""
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
extras = { 'lhe_version'  : '3.0',
           'cut_decays'   : 'F', 
           'pdlabel'      : "'lhapdf'",
           'lhaid'        : lhaid,         
           'maxjetflavor' : maxjetflavor,
           'asrwgtflavor' : maxjetflavor,
           'ickkw'        : 0,
           'ptj'          : 0,
           'ptb'          : 0,
           'mmjj'         : 0,
           'drjj'         : 0,
           'drll'         : 0,
           'drjl'         : 0.4,
           'ptl'          : 0,
           'etal'         : 10,
           'etaj'         : 10,
           'ktdurham'     : ktdurham,    
           'dparameter'   : dparameter,
           'use_syst'     : 'F',
           'dynamical_scale_choice': 3
             }

process_dir = new_process(grid_pack=gridpack_dir)
build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               scalefact=scalefact,alpsfact=alpsfact,extras=extras)

print_cards()

generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,njobs=nJobs,proc_dir=process_dir,
         grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,cluster_type=cluster_type,cluster_queue=cluster_queue,
         nevents=nevents,random_seed=runArgs.randomSeed)

arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')

#### Shower 
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.contact = [ 'Gavin Hesketh <gavin.hesketh@ucl.ac.uk>' ]
evgenConfig.keywords+=["inclusive","bottom","bbbar"]
evgenConfig.inputfilecheck = stringy
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'


if 'ATHENA_PROC_NUMBER' in os.environ:
    print 'Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.'
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): print 'Did not see option!'
    else: opts.nprocs = 0
    print opts
   


include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

PYTHIA8_nJetMax=nJetMax
PYTHIA8_TMS=ktdurham
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=process
PYTHIA8_nQuarksMerge=maxjetflavor
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")

#NEW
#genSeq.Pythia8.Commands += ["Merging:unorderedASscalePrescrip = 0"]

#for testing rapidity ordering
#genSeq.Pythia8.Commands += ["SpaceShower:rapidityOrder = on"]


# B-hadron filter
include("GeneratorFilters/BHadronFilter.py")
filtSeq += HeavyFlavorBHadronFilter
HeavyFlavorBHadronFilter.RequestCharm=0
HeavyFlavorBHadronFilter.RequestBottom=1
HeavyFlavorBHadronFilter.Request_cQuark=0
HeavyFlavorBHadronFilter.Request_bQuark=0
HeavyFlavorBHadronFilter.BottomPtMin=13000.
HeavyFlavorBHadronFilter.BottomEtaMax=2.7

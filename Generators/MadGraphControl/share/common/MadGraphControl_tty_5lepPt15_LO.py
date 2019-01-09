from MadGraphControl.MadGraphUtils import *
# General settings
nevents=int(1.1*runArgs.maxEvents)
mode=2
# MG Particle cuts

ptjCut=0
ptlCut=0
ptlMaxCut=15
ptaCut=15
drajCut=0.2
dralCut=0.2
etajCut=-1
etabCut=-1
maxjetflavor=5
dyn_scale = '3'


### DSID lists (extensions can include systematics samples)
DSID = runArgs.runNumber

if DSID == 999999:
    mgproc="""generate p p > t t~ > uc ds~ b ds uc~ b~ a QCD=2 QED=6"""
    name='ttgamma_allhadronic'
    keyword=['SM','top', 'ttV', 'photon', 'jets', 'allHadronic']

elif DSID == 345867:
    mgproc="""generate p p > t t~ > l+ vl  b ds uc~ b~ a  QCD=2 QED=5 @1\n"""
    mgproc +="""add process p p > t t~ > uc ds~ b l- vl~ b~ a  QCD=2 QED=5\n"""
    mgproc +="""add process p p > t t~ > l+ vl l- vl~ b  b~ a"""
    name='ttgamma_nonallhadronic'
    keyword=['SM','top', 'ttV', 'photon', 'lepton','jets']

else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)
stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
define w = w+ w-
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
define uc = u c
define uc~ = u~ c~
define ds = d s
define ds~ = d~ s~
"""+mgproc+"""
output -f
""")
fcard.close()
beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

lhaid=247000
pdflabel='lhapdf'
#Fetch default LO run_card.dat and set parameters
extras = {'lhe_version'   :'3.0',
           'lhaid'         : lhaid,
           'pdlabel'       : "'"+pdflabel+"'",
           'maxjetflavor'  : maxjetflavor,
           'ptl'           : ptlCut,
           'xptl'          : 5,
           'ptlmax'        : ptlMaxCut,
           'pta'           : ptaCut,
           'ptj'           : 0.0,
           'etal'          : 5.0,
           'etaa'          : 5.0,
           'etaj'          : etajCut,
           'etab'          : etabCut,
           'drjj'          : 0.0,
           'drjl'          : 0.0,
           'drll'          : 0.0,
           'draa'          : 0.0,
           'draj'          : 0.2,
           'dral'          : 0.2,
           'dynamical_scale_choice': dyn_scale,
           'use_syst'      : 'T',
           'sys_scalefact' : '1 0.5 2',
           'sys_pdf'       : 'NNPDF23_lo_as_0130_qed'}

process_dir = new_process()
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
                extras=extras)
print_cards()
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir)
outputDS=arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.tar.gz',lhe_version=3)

# Go to serial mode for Pythia8
if 'ATHENA_PROC_NUMBER' in os.environ:
    print 'Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.'
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): print 'Did not see option!'
    else: opts.nprocs = 0
    print opts

##shower
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=keyword
evgenConfig.contact = ["mazoza.ghneimat@cern.ch","Shu.Li@cern.ch","atlas-generators-madgraphcontrol@cern.ch"]
runArgs.inputGeneratorFile=outputDS
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

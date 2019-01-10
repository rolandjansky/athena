from MadGraphControl.MadGraphUtils import *

multiplier=250
# General settings
minevents=200
if hasattr(runArgs,"maxEvents"):
    minevents=runArgs.maxEvents


run_name='run_01'
mode=0
gridpack_dir=None
gridpack_mode=False

# MG Particle cuts
mllcut=-1

# Shower/merging settings
maxjetflavor=5
parton_shower='PYTHIA8'
nJetMax=2
qCut=25.

qCut25=[363845,364663]
qCut20=[363770]
qCut50=[363772]

if runArgs.runNumber in qCut20:
    qCut=20.
if runArgs.runNumber in qCut50:
    qCut=50.

### DSID lists (extensions can include systematics samples)

Zee=[363845,363770,363772,364663]
Zmumu=[363846,364664]
Wenu=[429739]


if runArgs.runNumber in Zee:
    mgproc="""
generate p p > e+ e- [QCD] @0
add process p p > e+ e- j [QCD] @1
add process p p > e+ e- j j [QCD] @2
"""
    name='Zee'
    keyword=['SM','Z'] 
    mllcut=40
    nJetMax=2
    gridpack_mode=True
    gridpack_dir='pp_Zeejets_012_13TeV_finalGridpackTest/'
elif runArgs.runNumber in Zmumu:
    mgproc="""
generate p p > mu+ mu- [QCD] @0
add process p p > mu+ mu- j [QCD] @1
add process p p > mu+ mu- j j [QCD] @2
"""
    name='Zmumu'
    keyword=['SM','Z'] 
    mllcut=40
    nJetMax=2
    gridpack_mode=True
    gridpack_dir='pp_Zmumujets_012_13TeV_finalGridpackTest/'

elif runArgs.runNumber in Wenu:          
    mgproc="""                        
define l = e+ e-                      
define vl = ve ve~                    
generate p p > l vl [QCD] @0          
add process p p > l vl j [QCD] @1     
add process p p > l vl j j [QCD] @2   
output W_2jets_FxFx_13TeV             
"""                                   
    name='Wenuw'                      
    keyword=['SM','W']                
    nevents=20000                     
    nJetMax=2                         
    gridpack_mode=True                
    gridpack_dir='pp_Wenuejets_012_13TeV/'  
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


nevents=minevents*multiplier
print "Asking MadGraph5_aMC@NLO to generate",nevents,"events."


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model loop_sm-no_b_mass
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


lhaid=230000
pdflabel='lhapdf'
pdfErrSize=100
lhe_version=3

#Fetch default LO run_card.dat and set parameters

extras = { 'lhaid'         : lhaid,
           'pdlabel'       : "'"+pdflabel+"'",
           'maxjetflavor'  : maxjetflavor,
           'parton_shower' : parton_shower,
           'ickkw'         : 3,
           'reweight_scale': '.true.',
           'rw_Rscale_down':  0.5,
           'rw_Rscale_up'  :  2.0,
           'rw_Fscale_down':  0.5,  
           'rw_Fscale_up'  :  2.0, 
           'reweight_PDF'  : '.true.',
           'PDF_set_min'   : lhaid+1, 
           'PDF_set_max'   : lhaid+pdfErrSize, 
           'jetradius'     : 1.0,
           'ptj'           : 8, 
           'etaj'          : -1,
           'mll_sf'        : mllcut,
           'mll'           : mllcut}




#process_dir = new_process(grid_pack=gridpack_dir)
process_dir = gridpack_dir

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)

print_cards()

param_card_loc='param_card.Torrielli.dat'
paramcard = subprocess.Popen(['get_files','-data',param_card_loc])
paramcard.wait()
if not os.access(param_card_loc,os.R_OK):
    raise RuntimeError("ERROR: Could not get %s"%param_card_loc)

generate(run_card_loc='run_card.dat',param_card_loc=param_card_loc,mode=mode,proc_dir=process_dir,
         run_name=run_name,grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=runArgs.randomSeed,gridpack_compile=True)
#arrange_output(run_name=run_name,proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz',lhe_version=lhe_version)
outputDS=arrange_output(run_name=run_name,proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz',lhe_version=lhe_version)



if 'ATHENA_PROC_NUMBER' in os.environ:
    print 'Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.'
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): print 'Did not see option!'
    else: opts.nprocs = 0
    print opts


#### Shower 
evgenConfig.description = 'aMcAtNloPythia8EvtGen_'+str(name)
evgenConfig.keywords+=keyword 
#evgenConfig.inputfilecheck = stringy
#runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'
runArgs.inputGeneratorFile=outputDS


include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py") 
include("Pythia8_i/Pythia8_aMcAtNlo.py")

PYTHIA8_nJetMax=nJetMax
PYTHIA8_qCut=qCut
include("Pythia8_i/Pythia8_FxFx.py")



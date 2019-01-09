from MadGraphControl.MadGraphUtils import *

# General settings
minevents=5000
nevents=30000
mode=0
nJobs=1
gridpack_dir=None
gridpack_mode=False
cluster_type=None 
cluster_queue=None

alpsfact = 1.0
scalefact = 1.0

# MG Particle cuts
mllcut=-1

# MG merging settings
maxjetflavor=5
ickkw=0

# Pythia8 merging settings
nJetMax=2
ktdurham=30
dparameter=0.2

### DSID lists (extensions can include systematics samples)
ttbar_5fl_Nominal=[407200,407201,407202,407203,407204]

if runArgs.runNumber in ttbar_5fl_Nominal:
    mgproc="""
generate p p > t t~ @0
add process p p > t t~ j @1
add process p p > t t~ j j @2
"""
    name='ttbar_Np012'
    process="pp>tt~"
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

if runArgs.runNumber % 5 == 1:
    nevents=500000
    minevents=5000
elif runArgs.runNumber % 5 == 2:
    nevents=350000
    minevents=1000
elif runArgs.runNumber % 5 == 3:
    nevents=350000
    minevents=1000
elif runArgs.runNumber % 5 == 4:
    nevents=500000
    minevents=500

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

madspin_card_loc='madspin_card.dat'
mscard = open(madspin_card_loc,'w')
mscard.write("""#************************************************************
#*                        MadSpin                           *
#*                                                          *
#*    P. Artoisenet, R. Frederix, R. Rietkerk, O. Mattelaer *
#*                                                          *
#*    Part of the MadGraph5_aMC@NLO Framework:              *
#*    The MadGraph5_aMC@NLO Development Team - Find us at   *
#*    https://server06.fynu.ucl.ac.be/projects/madgraph     *
#*                                                          *
#************************************************************                                                                                                              
#Some options (uncomment to apply)
# 
# set seed 1
# set Nevents_for_max_weigth 75 # number of events for the estimate of the max. weight
# set BW_cut 15                # cut on how far the particle can be off-shell
 set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
# 
set seed %i
# specify the decay for the final state particles
decay t > w+ b, w+ > all all
decay t~ > w- b~, w- > all all
decay w+ > all all
decay w- > all all
decay z > all all
# running the actual code
launch"""%runArgs.randomSeed)
mscard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version'  : '3.0',
           'cut_decays'   : 'F', 
           'pdlabel'      : "'lhapdf'",
           'lhaid'        : 260000,         
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
           'use_syst'     : 'T',
           'sys_matchscale': ktdurham,
           'sys_alpsfact' : '1 0.5 2',
           'sys_scalefact' : '1 0.5 2',
           'sys_pdf'      : 'NNPDF30_nlo_as_0118',
           'dynamical_scale_choice': 3
             }


process_dir = new_process(grid_pack=gridpack_dir)
build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               scalefact=scalefact,alpsfact=alpsfact,extras=extras)

print_cards()

generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,njobs=nJobs,proc_dir=process_dir,
         grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,cluster_type=cluster_type,cluster_queue=cluster_queue,
         nevents=nevents,random_seed=runArgs.randomSeed,madspin_card_loc=madspin_card_loc)

arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')

#### Shower 
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=['ttbar','electron','muon','jets']
evgenConfig.inputfilecheck = stringy
evgenConfig.minevents = minevents
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

include("Pythia8_i/Pythia8_MadGraph.py")

PYTHIA8_nJetMax=nJetMax
PYTHIA8_TMS=ktdurham
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=process
PYTHIA8_nQuarksMerge=maxjetflavor
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")
genSeq.Pythia8.Commands+=["Merging:mayRemoveDecayProducts=on"]
genSeq.Pythia8.Commands += ["Merging:unorderedASscalePrescrip = 0"]

## Add semi- and di- lepton filter
include('GeneratorFilters/TTbarWToLeptonFilter.py')
filtSeq.TTbarWToLeptonFilter.NumLeptons = -1
filtSeq.TTbarWToLeptonFilter.Ptcut = 0.

# Add phase space filters
include('GeneratorFilters/MissingEtFilter.py')
include('GeneratorFilters/HTFilter.py')
filtSeq.HTFilter.MinJetPt = 35.*GeV # Min pT to consider jet in HT
filtSeq.HTFilter.MaxJetEta = 2.5 # Max eta to consider jet in HT
filtSeq.HTFilter.UseLeptonsFromWZTau = True # Include e/mu from the MC event in the HT
filtSeq.HTFilter.MinLeptonPt = 25.*GeV # Min pT to consider muon in HT
filtSeq.HTFilter.MaxLeptonEta = 2.5 # Max eta to consider muon in HT
if runArgs.runNumber % 5 == 0:
    filtSeq.MissingEtFilter.METCut = 0*GeV # Min MET to keep event
    filtSeq.MissingEtFilterUpperCut.METCut = 200*GeV # Max MET to keep event
    filtSeq.HTFilter.MinHT = 0.*GeV # Min HT to keep event
    filtSeq.HTFilter.MaxHT = 500.*GeV # Max HT to keep event
elif runArgs.runNumber % 5 == 1:
    filtSeq.MissingEtFilter.METCut = 0*GeV # Min MET to keep event
    filtSeq.MissingEtFilterUpperCut.METCut = 200*GeV # Max MET to keep event
    filtSeq.HTFilter.MinHT = 500.*GeV # Min HT to keep event
    filtSeq.HTFilter.MaxHT = 700.*GeV # Max HT to keep event
elif runArgs.runNumber % 5 == 2:
    filtSeq.MissingEtFilter.METCut = 0*GeV # Min MET to keep event
    filtSeq.MissingEtFilterUpperCut.METCut = 200*GeV # Max MET to keep event
    filtSeq.HTFilter.MinHT = 700.*GeV # Min HT to keep event
    filtSeq.HTFilter.MaxHT = 1000.*GeV # Max HT to keep event
elif runArgs.runNumber % 5 == 3:
    filtSeq.MissingEtFilter.METCut = 200*GeV # Min MET to keep event
    filtSeq.MissingEtFilterUpperCut.METCut = 100000*GeV # Max MET to keep event
    filtSeq.HTFilter.MinHT = 0.*GeV # Min HT to keep event
    filtSeq.HTFilter.MaxHT = 1000.*GeV # Max HT to keep event
elif runArgs.runNumber % 5 == 4:
    filtSeq.MissingEtFilter.METCut = 0*GeV # Min MET to keep event
    filtSeq.MissingEtFilterUpperCut.METCut = 100000*GeV # Max MET to keep event
    filtSeq.HTFilter.MinHT = 1000.*GeV # Min HT to keep event
    filtSeq.HTFilter.MaxHT = 100000.*GeV # Max HT to keep event

filtSeq.Expression = "(TTbarWToLeptonFilter and HTFilter and MissingEtFilter and not MissingEtFilterUpperCut)"



from MadGraphControl.MadGraphUtils import *

nevents=10000
mode=0
gridpack_dir='madevent/'


#---------------------------------------------------------------------------
# Process type based on runNumber:
#---------------------------------------------------------------------------
if runArgs.runNumber==364460:
   mgproc="""generate p p > vl vl~ a j j QCD=0 QED=5"""
   name='nunugammajj'
   process='pp>nunua'
   keyword=['SM','Z','photon','neutrino','jets']
   description = 'MadGraph Z->vvg plus two EWK jets'
   ptgmin=130
elif runArgs.runNumber==364458:
   mgproc="""generate p p > e+ e- a j j QCD=0 QED=5"""
   name='eegammajj'
   process='pp>e+e-a'
   keyword=['SM','Z','photon','electron','jets']
   description = 'MadGraph Z->eeg plus two EWK jets'
   ptgmin=10
elif runArgs.runNumber==364459:
   mgproc="""generate p p > mu+ mu- a j j QCD=0 QED=5"""
   name='mumugammajj'
   process='pp>mu+mu-a'
   keyword=['SM','Z','photon','muon','jets']
   description = 'MadGraph Z->mumug plus two EWK jets'
   ptgmin=10

else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

print("Debug::I have choosen process:"+name)
#---------------------------------------------------------------------------
# MG5 Proc card
#---------------------------------------------------------------------------
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
"""+mgproc+"""
output -f
""")
fcard.close()


#----------------------------------------------------------------------------
# Run Number
#----------------------------------------------------------------------------
if not hasattr(runArgs,'runNumber'):
    raise RunTimeError("No run number found.")

#----------------------------------------------------------------------------
# Random Seed
#----------------------------------------------------------------------------
randomSeed = 0
if hasattr(runArgs,'randomSeed'): randomSeed = runArgs.randomSeed

#----------------------------------------------------------------------------
# Beam energy
#----------------------------------------------------------------------------
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = int(runArgs.ecmEnergy) / 2.
else:
    raise RunTimeError("No center of mass energy found.")

#---------------------------------------------------------------------------
# Number of Events
#---------------------------------------------------------------------------
safefactor = 1.1
if hasattr(runArgs,'maxEvents') and runArgs.maxEvents > 0:  nevents = int(int(runArgs.maxEvents)*safefactor)
else: nevents = int(nevents*safefactor)

#---------------------------------------------------------------------------
# Number of Events
#---------------------------------------------------------------------------
skip_events=0
if hasattr(runArgs,'skipEvents'): skip_events=runArgs.skipEvents

#---------------------------------------------------------------------------
# MG5 Run Card
#---------------------------------------------------------------------------
extras = { 'lhe_version' : '2.0',
           'req_acc_FO'    :'0.001', 
           'cut_decays'  : 'T', 
           'pdlabel'     :"'lhapdf'",
           'lhaid'       :"262000",
           'auto_ptj_mjj': 'F',
           'gridpack' : '.true.',
           'ptj':"15",
           'ptb':"15",
           'pta':"10",
           'ptl':"10",
           'etaj':"5.5",
           'etab':"5.5",
           'etal':"3.0",
           'etaa':"3.0",
           'drjj':"0.1",
           'drll':"0.1",
           'draa':"0",
           'draj':"0.1",
           'drjl':"0.1",
           'dral':"0.1",
           'mmjj':"0",
           'mmll':"40",
           'ptgmin'      : ptgmin,
           'epsgamma':'0.1',
           'R0gamma' :'0.1', 
           'xn'    :'2',
           'isoEM' :'True', 
           'bwcutoff'    :'15',
           'maxjetflavor': '5',
           'asrwgtflavor': '5',
           'use_syst'    : 'False'}
           
doSyst=True

if doSyst:
    lhe_version=3
    extras.update({'reweight_scale': '.true.',
                   'rw_Rscale_down':  0.5,
                   'rw_Rscale_up'  :  2.0,
                   'rw_Fscale_down':  0.5,
                   'rw_Fscale_up'  :  2.0,
                   'reweight_PDF'  : '.true.',
                   'PDF_set_min'   : 262001,
                   'PDF_set_max'   : 262100})
else:
    lhe_version=1
    extras.update({'reweight_scale': '.false.',
                   'reweight_PDF'  : '.false.'})
           

process_dir = new_process(grid_pack=gridpack_dir)
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),
               run_card_new='run_card.dat', 
               xqcut=0,
               nevts=nevents,
               rand_seed=randomSeed,
               beamEnergy=beamEnergy,
               extras=extras)

print_cards()


#---------------------------------------------------------------------------
# MG5 + Pythia8 setup and process (lhe) generation
#---------------------------------------------------------------------------
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,run_name=name,grid_pack=True,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=runArgs.randomSeed)


#--------------------------------------------------------------------------------------------------------------------
#--------------------------------------------------------------------------------------------------------------------

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)
arrange_output(run_name=name,
               proc_dir=process_dir,
               outputDS=name+'._00001.events.tar.gz'
#               outputDS=stringy+'._00001.events.tar.gz',
               )
 


#### Shower
# PYTHIA8_nJetMax=0
# PYTHIA8_Process=process
# PYTHIA8_nQuarksMerge=5
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")



evgenConfig.generators = ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.inputfilecheck = name
#evgenConfig.inputfilecheck = stringy

evgenConfig.contact = ['Louis Helary <Louis.Helary@cern.ch>']
evgenConfig.keywords+=keyword
evgenConfig.description = description

#runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'
runArgs.inputGeneratorFile=name+'._00001.events.tar.gz'

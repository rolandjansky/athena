from MadGraphControl.MadGraphUtils import *

nevents=5000
mode=0

#---------------------------------------------------------------------------
# MG5 Proc card
#---------------------------------------------------------------------------
fcard = open('proc_card_mg5.dat','w')
if runArgs.runNumber==363724:
    fcard.write("""
    import model sm
    define p = g u c d s b u~ c~ d~ s~ b~
    define j = g u c d s b u~ c~ d~ s~ b~
    define l+ = e+ mu+
    define l- = e- mu-
    define vl = vt
    define vl~ = vt~
    generate    p p > l+ l- vl vl~ j j QED=6 QCD=0
    output -f
    """)
    fcard.close()
    name="ZZllvv2jetEWK"


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
if hasattr(runArgs,'maxEvents') and runArgs.maxEvents > 0:  nevents = int(runArgs.maxEvents)*safefactor
else: nevents = nevents*safefactor

#---------------------------------------------------------------------------
# Number of Events
#---------------------------------------------------------------------------
skip_events=0
if hasattr(runArgs,'skipEvents'): skip_events=runArgs.skipEvents

#---------------------------------------------------------------------------
# MG5 Run Card
#---------------------------------------------------------------------------
extras = {
    'pdlabel':"'lhapdf'",
    'lhaid':"262000",
    'asrwgtflavor':"5",
    'lhe_version':"3.0",
    'ptj':"15",
    'ptb':"15",
    'pta':"0",
    'ptl':"4",
    'misset':"10",
    'etaj':"5",
    'etab':"5",
    'etal':"2.8",
    'drjj':"0",
    'drll':"0",
    'draa':"0",
    'draj':"0",
    'drjl':"0",
    'dral':"0",
    'mmjj':"0",
    'mmll':"40",
    'maxjetflavor':"5" ,
    'sys_pdf':"NNPDF30_lo_as_0118.LHgrid" }

process_dir = new_process()
build_run_card(run_card_old=get_default_runcard(process_dir),
               run_card_new='run_card.dat',
               nevts=nevents,
               rand_seed=randomSeed,
               beamEnergy=beamEnergy,
               extras=extras)

#---------------------------------------------------------------------------
# MG5 + Pythia8 setup and process (lhe) generation
#---------------------------------------------------------------------------
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,run_name=name)


#--------------------------------------------------------------------------------------------------------------------
#--------------------------------------------------------------------------------------------------------------------

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)
arrange_output(run_name=name,
               proc_dir=process_dir,
               outputDS=stringy+'._00001.events.tar.gz',
               )

#### Shower

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

evgenConfig.generators = ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.description = 'MadGraph ZZ->llvv plus two EWK jets'
evgenConfig.keywords+=['SM','ZZ','2lepton','2jet','VBS']
evgenConfig.inputfilecheck = stringy
evgenConfig.contact = ['Guangyi Zhang <g.zhang@cern.ch>']

runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

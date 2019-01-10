from MadGraphControl.MadGraphUtils import *

nevents=10000
minevents=5000
mode=0
gridpack_dir='madevent/'
qCut=20


#---------------------------------------------------------------------------
# Process type based on runNumber:
#---------------------------------------------------------------------------
if runArgs.runNumber==345775 or runArgs.runNumber==345776 or runArgs.runNumber==345777 or runArgs.runNumber==345778:
    process1='generate p p > e+ e- a [QCD] @0'
    process2='add process p p > e+ e- a j [QCD] @1'
    evgenConfig.keywords = ['Z','photon','electron','jets']
    description='MadGraph_aMC@NLO Z->eeg 0,1jets@NLO'
    ptgmin=10    
    name='eegammajj'
    if runArgs.runNumber==345775:
        nevents=15000
        minevents=5000
        description+=" pty>10 GeV, 0<HT<125 GeV"
        evgenConfig.inputconfcheck="eegamma_HT0_125_13TeV"
    elif runArgs.runNumber==345776:
        nevents=15000
        minevents=5000
       	description+=" pty>10 GeV, HT>125 GeV, mjj<150 GeV"
        evgenConfig.inputconfcheck="eegamma_HTGT125_MjjLT150"
    elif runArgs.runNumber==345777:
        nevents=65000 
        minevents=5000
        description+=" pty>10 GeV, HT>125 GeV, 150<mjj<500 GeV"
        evgenConfig.inputconfcheck="eegamma_HTGT125_MjjLT500"
    elif runArgs.runNumber==345778:
        nevents=65000
        minevents=500
        description+=" pty>10 GeV, HT>125 GeV, mjj>500 GeV"
        evgenConfig.inputconfcheck="eegamma_HTGT125_MjjGT500"

elif runArgs.runNumber==345779 or runArgs.runNumber==345780 or runArgs.runNumber==345781 or runArgs.runNumber==345782:
    process1='generate p p > mu+ mu- a [QCD] @0'
    process2='add process p p > mu+ mu- a j [QCD] @1'
    evgenConfig.keywords = ['Z','photon','muon','jets']
    description='MadGraph_aMC@NLO Z->mumug 0,1jets@NLO'
    ptgmin=10
    name='mumugammajj'
    if runArgs.runNumber==345779:
        nevents=15000
        minevents=5000
       	description+=" pty>10 GeV, 0<HT<125 GeV"
        evgenConfig.inputconfcheck="mumugamma_HT0_125_13TeV"
    elif runArgs.runNumber==345780:
        nevents=15000
        minevents=5000
        description+=" pty>10 GeV, HT>125 GeV, mjj<150 GeV"
        evgenConfig.inputconfcheck="mumugamma_HTGT125_MjjLT150_13TeV"
    elif runArgs.runNumber==345781:
        nevents=65000
        minevents=5000
        description+=" pty>10 GeV, HT>125 GeV, 150<mjj<500 GeV"
        evgenConfig.inputconfcheck="mumugamma_HTGT125_MjjLT500_13TeV"
    elif runArgs.runNumber==345782:
        nevents=65000
        minevents=500
        description+=" pty>10 GeV, HT>125 GeV, mjj>500 GeV"
        evgenConfig.inputconfcheck="mumugamma_HTGT125_MjjGT500_13TeV"

        
elif runArgs.runNumber==345783:
    process1='generate p p > ta+ ta- a [QCD] @0'
    process2='add process p p > ta+ ta- a j [QCD] @1'
    evgenConfig.keywords = ['Z','photon','tau','jets']
    description='MadGraph_aMC@NLO Z->tautaug 0,1jets@NLO'
    ptgmin=10
    nevents=15000
    minevents=5000
    name='tautaugammajj'
    description+=" pty>10 GeV"
    evgenConfig.inputconfcheck="tautaugamma_pty_10_13TeV"

elif runArgs.runNumber==345784:
    process1='generate p p > vl vl~ a [QCD] @0'
    process2='add process p p > vl vl~ a j [QCD] @1'
    evgenConfig.keywords = ['Z','photon','neutrino','jets']
    description='MadGraph_aMC@NLO Z->nunug 0,1jets@NLO'
    ptgmin=140
    nevents=15000
    minevents=5000    
    name='nunugammajj'
    description+=" pty>10 GeV"
    evgenConfig.inputconfcheck="nunugamma_pty_140_13TeV"

else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

#print("Debug::I have choosen process:"+name)
print("Debug::MadGrahControl_Zg_01_NLO_FxFx::Generation Parameters :: RunNumber =",runArgs.runNumber," :: Name =",name," :: Description =",description," :: ptgmin =",ptgmin," :: nevents=",nevents," :: minevents=",minevents," :: process=",process1," :: ",process2) 
#---------------------------------------------------------------------------
# MG5 Proc card
#---------------------------------------------------------------------------
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model loop_sm-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
"""+process1+"""
"""+process2+"""
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
skip_events=0
if hasattr(runArgs,'skipEvents'): skip_events=runArgs.skipEvents

#---------------------------------------------------------------------------
# MG5 Run Card
#---------------------------------------------------------------------------
extras = { 'cut_decays'    : 'T',
           'ickkw'         :'3',
           'parton_shower' :'PYTHIA8',
           'pdlabel'       :"'lhapdf'",
           'lhaid'         :"260000",
           'auto_ptj_mjj'  :'F',
           'gridpack'      :'.true.',
           'ptj'           :"10",
           'etaj'          :"5.5",
           'drjj'          :"0.1",
           'ptgmin'        :ptgmin,
           'epsgamma'      :'0.1',
           'R0gamma'       :'0.1',
           'xn'            :'2',
           'isoEM'         :'True',
           'etagamma'      :'3.0',
           'bwcutoff'      :'15',
           'maxjetflavor'  :'5',
           'asrwgtflavor'  :'5',
           'use_syst'      :'True',
           'reweight_scale':'True',
           'rw_Rscale_down':0.5,  
           'rw_Rscale_up'  :2.0,
           'rw_Fscale_down':0.5,
           'rw_Fscale_up'  :2.0,
           'reweight_PDF'  :'True',
           'PDF_set_min'   :260001,
           'PDF_set_max'   :260100
}


if runArgs.runNumber<345784 and runArgs.runNumber>=345775:
    extras.update({'ptl'       :"10",
                   'etal'          :"3.0",
                   'drll'          :"0.1",
                   'drll_sf'       :"0.1",
                   'drjl'          :"0.1", 
                   'mmll'          :"10",
                   'mll_sf'        :"10"})
                   
                   
           

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
# Shower
#--------------------------------------------------------------------------------------------------------------------

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)
arrange_output(run_name=name,
               proc_dir=process_dir,
               outputDS=name+'._00001.events.tar.gz'
#               outputDS=stringy+'._00001.events.tar.gz',
               )
 

PYTHIA8_qCut=qCut
PYTHIA8_nJetMax=1

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")
include("Pythia8_i/Pythia8_FxFx.py")
genSeq.Pythia8.Commands += ['JetMatching:qCutME        = 10.0']

print("Debug::MadGrahControl_Zg_01_NLO_FxFx::Shower Parameters :: name =",name," :: proc_dir =",process_dir," :: qCut =",PYTHIA8_qCut," :: PYTHIA8_nJetMax =",PYTHIA8_nJetMax)

#--------------------------------------------------------------------------------------------------------------------
# Mjj filtering:
#--------------------------------------------------------------------------------------------------------------------
if(runArgs.runNumber==345776 or runArgs.runNumber==345777 or runArgs.runNumber==345778 or runArgs.runNumber==345780 or runArgs.runNumber==345781 or runArgs.runNumber==345782):
    include("GeneratorFilters/VBFMjjIntervalFilter.py")
    filtSeq.VBFMjjIntervalFilter.RapidityAcceptance = 5.5
    filtSeq.VBFMjjIntervalFilter.MinSecondJetPT = 20.*GeV
    filtSeq.VBFMjjIntervalFilter.MinOverlapPT = 15.*GeV
    filtSeq.VBFMjjIntervalFilter.TruthJetContainerName = "AntiKt4TruthJets"
    filtSeq.VBFMjjIntervalFilter.NoJetProbability  = -1.0
    filtSeq.VBFMjjIntervalFilter.OneJetProbability = -1.0
    filtSeq.VBFMjjIntervalFilter.LowMjjProbability = 1.0
    filtSeq.VBFMjjIntervalFilter.HighMjjProbability = 1.0
    filtSeq.VBFMjjIntervalFilter.LowMjj = 150.*GeV
    filtSeq.VBFMjjIntervalFilter.TruncateAtLowMjj = True
    filtSeq.VBFMjjIntervalFilter.HighMjj = 500.*GeV
    filtSeq.VBFMjjIntervalFilter.TruncateAtHighMjj = True
    filtSeq.VBFMjjIntervalFilter.PhotonJetOverlapRemoval = True
    filtSeq.VBFMjjIntervalFilter.ElectronJetOverlapRemoval = True
    filtSeq.VBFMjjIntervalFilter.TauJetOverlapRemoval = True
       
    if(runArgs.runNumber==345776 or runArgs.runNumber==345780):
           filtSeq.VBFMjjIntervalFilter.NoJetProbability  = 1.0
           filtSeq.VBFMjjIntervalFilter.OneJetProbability = 1.0
           filtSeq.VBFMjjIntervalFilter.LowMjj = 0*GeV
           filtSeq.VBFMjjIntervalFilter.TruncateAtLowMjj = False
           filtSeq.VBFMjjIntervalFilter.HighMjj = 150.*GeV
           filtSeq.VBFMjjIntervalFilter.TruncateAtHighMjj = True

    elif(runArgs.runNumber==345778 or runArgs.runNumber==345782):
           filtSeq.VBFMjjIntervalFilter.LowMjj = 500*GeV
           filtSeq.VBFMjjIntervalFilter.TruncateAtLowMjj = True
           filtSeq.VBFMjjIntervalFilter.HighMjj = 13000.*GeV
           filtSeq.VBFMjjIntervalFilter.TruncateAtHighMjj = False

    print("Debug::MadGrahControl_Zg_01_NLO_FxFx::Filter parameters :: NoJetP=",filtSeq.VBFMjjIntervalFilter.NoJetProbability," :: OneJetP=",filtSeq.VBFMjjIntervalFilter.OneJetProbability," :: LowMjjVal=",filtSeq.VBFMjjIntervalFilter.LowMjj," :: TruncateAtLowMjj=",filtSeq.VBFMjjIntervalFilter.TruncateAtLowMjj," :: HighMjjVal=",filtSeq.VBFMjjIntervalFilter.HighMjj," :: TruncateAtHighMjj=",filtSeq.VBFMjjIntervalFilter.TruncateAtHighMjj)




#--------------------------------------------------------------------------------------------------------------------
# Sample information:
#--------------------------------------------------------------------------------------------------------------------

evgenConfig.generators = ["aMcAtNlo", "Pythia8"]

evgenConfig.contact = ['Louis Helary <Louis.Helary@cern.ch>']
evgenConfig.description = description
runArgs.inputGeneratorFile=name+'._00001.events.tar.gz'
evgenConfig.minevents=minevents
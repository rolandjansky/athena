################################################################
# This is job options for 
# Madgraph LO Z->bb + 2 or more jets inclusive sample.
# Designed for VBF H->bb analysis background sample production.
################################################################


from MadGraphControl.MadGraphUtils import *

################################################################
# Madgraph @ LO
################################################################
nevents=5000
mode=0

DSID_QCDZbbjj =[342195]
DSID_EWKZbbjj =[342196]

if runArgs.runNumber in DSID_QCDZbbjj:
    # lowest QED order -> the electro-weak production will not be included here
    mgproc="""generate p p > z j j, z > b b~"""
    name="QCDZbbjj"
    keyword=['SM','Z','bottom']
elif runArgs.runNumber in DSID_EWKZbbjj:
    mgproc="""generate p p > z j j QCD=0, z > b b~"""
    name="EWKZbbjj"
    keyword=['SM','Z','bottom']
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
"""+mgproc+"""
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

# 69+/-1% for this generation condition
# safety factor 2 should be OK
safefactor=2
nevents=nevents*safefactor
if runArgs.maxEvents > 0:
    nevents=runArgs.maxEvents*safefactor


pdflabel="nn23lo1"

extras = { 'lhe_version'  : '2.0',
           'pdlabel'      : "'"+pdflabel+"'",
           'ptj'          : 20,
           'ptb'          : 0,
           'etab'         : 5.0,
           'etaj'         : 5.0
           }

build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)

print_cards()
process_dir = new_process()
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir)
arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')



################################################################
# Showering (for inclusive sample)
################################################################
evgenConfig.generators += ["MadGraph", "Pythia8"]
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=keyword 
evgenConfig.inputfilecheck = stringy
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")



################################################################
# Filtering (four jets based on truth particle jets)
################################################################
include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
include("GeneratorFilters/AntiKt6TruthJets_pileup.py")

if not hasattr( filtSeq, "QCDTruthMultiJetFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import QCDTruthMultiJetFilter
    filtSeq += QCDTruthMultiJetFilter()
    pass

QCDTruthMultiJetFilter = filtSeq.QCDTruthMultiJetFilter
QCDTruthMultiJetFilter.Njet = 4
QCDTruthMultiJetFilter.NjetMinPt = 20.*GeV #
QCDTruthMultiJetFilter.MinLeadJetPt = 20.*GeV #
QCDTruthMultiJetFilter.MaxLeadJetPt = runArgs.ecmEnergy*GeV # LHC kinematic limit...
QCDTruthMultiJetFilter.MaxEta = 5.0
QCDTruthMultiJetFilter.TruthJetContainer = "AntiKt4TruthJets"
QCDTruthMultiJetFilter.DoShape = False

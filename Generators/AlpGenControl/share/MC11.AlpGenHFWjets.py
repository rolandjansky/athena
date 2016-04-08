# Set up for generic alpgen generation
#  This is the main job option to call

import sys

UseHerwig=True
rand_seed=1
ecm = 3500.

from AthenaCommon import Logging

log = Logging.logging.getLogger('AlpGenGenericGeneration')

if not hasattr(runArgs,'runNumber'):
    log.fatal('No run number found.  Generation will fail.  No idea how to decode this.')
    raise RunTimeError('No run number found.')

if not hasattr(runArgs,'randomSeed'):
    log.error('Random seed not set.  Will use 1.')
else:
    rand_seed=runArgs.randomSeed

if not hasattr(runArgs,'skipEvents'):
    log.info('No skip events argument found.  Good!')
else:
    log.fatal('Skip events argument found.  No idea how to deal with that yet.  Bailing out.')
    raise RunTimeError('Skip events argument found.')

if not hasattr(runArgs,'ecmEnergy'):
    log.info('No center of mass energy found.  Will use the default of 7 TeV.')
else:
    log.info('Ignoring the center of mass energy you specified and using 7 TeV.')

# Set up run based on run number
run_num=int(runArgs.runNumber)
process=''
special1=None
nwarm = 4

# W+jets samples (thanks to L. Mijovic)
if run_num>=147001 and run_num<=147024:
    log.info('Recognized W+jets run number.  Will generate for run '+str(run_num))
    process = 'wjet'
    jets = [(run_num-147001)%6,1] # Exclusive matching to the last number of partons
    if jets[0]==5: jets[1]=0

    #               10 min  30 min  55 min  9 hrs    20 hrs
    #               20 hrs     ???      ???
    event_numbers=[1000000,5000000,5000000,30000000,12000000,
                   10000000,10000000,10000000]
    ncwarm = [2,2,3,3,4,4,4,4]
    nwarm = ncwarm[jets[0]]
    events_alpgen = event_numbers[jets[0]]

    if run_num>=147013: UseHerwig=False    

    events_athena = 5000
    if jets[0]==4: events_athena=750 # Should work
    if jets[0]==5: events_athena=200 # Should work
    if jets[0]==6: events_athena=50
    if jets[0]==7: events_athena=100

    W_mode = int((run_num-147001)/6)%2+1
    special1 = 'iwdecmode %i          ! W decay mode (1: el, 2: mu)\n '%(W_mode)

if process=='':
    log.fatal('Unknown run number!  Bailing out!')
    raise RunTimeError('Unknown run number.')

# Special configuration for Pythia showering
if not UseHerwig:
        special1 += """xclu   0.26          ! lambda for alpha_s in CKKW scale evaluation
ipclu 1         ! nloop for alphas in CKKW scale evaluation
"""

from AlpGenControl.AlpGenUtils import SimpleRun

log.info('Running AlpGen on process '+process+' with ECM='+str(ecm)+' with '+str(nwarm)+' warm up rounds')
log.info('Will generate '+str(events_alpgen)+' events with jet params '+str(jets)+' and random seed '+str(rand_seed))
if special1 is not None:
    log.info('Special settings:')
    log.info(special1)

SimpleRun( proc_name=process , events=events_alpgen , nwarm=nwarm , energy=ecm , jets=jets , seed = rand_seed , special1=special1 , special2=special1 )

if hasattr(runArgs,'inputGeneratorFile'):
    log.info('About to clobber old input generator file argument, '+str(runArgs.inputGeneratorFile))
runArgs.inputGeneratorFile='alpgen.XXXXXX.TXT.v1'

log.info('Passing events on to athena for generation of '+str(events_athena)+' events')

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg") 

if not UseHerwig:
    include ( "MC11JobOptions/MC11_PythiaPerugia2011C_Common.py" )

    Pythia.PythiaCommand += ["pyinit user alpgen",
                             "pydat1 parj 90 20000.",
                             "pydat3 mdcy 15 1 0",
                             "pypars mstp 143 1",
                             "pydat1 parj 90 20000",   # prevent photon emission in parton showers
                             "pydat1 mstj 26 0"        # turn off B mixing (done by EvtGen)
                             ]
else: 
    include ( "MC11JobOptions/MC11_AlpgenJimmy_Common_7TeV.py" ) 
 
    Herwig.HerwigCommand += [ "iproc alpgen",
                              "taudec TAUOLA",
                              "mixing 0",  # Switch off B-mixing
                            ]

include ( "MC11JobOptions/MC11_Tauola_Fragment.py" )
include ( "MC11JobOptions/MC11_Photos_Fragment.py" )

from MC11JobOptions.AlpgenEvgenConfig import evgenConfig
evgenConfig.inputfilebase = 'alpgen.XXXXXX.TXT.v1'
evgenConfig.efficiency = 0.90000
evgenConfig.minevents = events_athena

#--------------------------------------------------------------
# EVTGEN
#--------------------------------------------------------------

# This code sets up EvtGen for inclusive decays
from HepMCTools.HepMCToolsConf import TranslateParticleCodes
topAlg += TranslateParticleCodes()
from EvtGen_i.EvtGen_iConf import EvtInclusiveDecay
topAlg += EvtInclusiveDecay()

TranslateParticleCodes = topAlg.TranslateParticleCodes
TranslateParticleCodes.OutputLevel = 3
if UseHerwig:
    TranslateParticleCodes.translateToPDTFrom = "HERWIG"
else:
    TranslateParticleCodes.translateToPDTFrom = "PYTHIA"
TranslateParticleCodes.translateFromPDTTo = "EVTGEN"
TranslateParticleCodes.test = [0,10000000]

import os
os.system('get_files -data inclusive.dec')
os.system('get_files -data inclusive.pdt')

EvtInclusiveDecay = topAlg.EvtInclusiveDecay
EvtInclusiveDecay.OutputLevel = 3
## If you want to force decays you need to uncomment the line below and replace
## the user decay file with your own
####EvtInclusiveDecay.userDecayFile = "DstarP2D0PiP_D02KpiPlusAnti.DEC"
EvtInclusiveDecay.pdtFile = "inclusive.pdt"
EvtInclusiveDecay.decayFile = "inclusive.dec"

# Build jets for the filter
try:
     from JetRec.JetGetters import *
     a4alg=make_StandardJetGetter('AntiKt',0.4,'Truth').jetAlgorithmHandle()
except Exception, e:
     pass

#--------------------------------------------------------------
# Filter on the B and charm hadrons
#--------------------------------------------------------------
from GeneratorFilters.GeneratorFiltersConf import HeavyFlavorHadronFilter
topAlg += HeavyFlavorHadronFilter()
topAlg.HeavyFlavorHadronFilter.OutputLevel=INFO
topAlg.HeavyFlavorHadronFilter.RequestCharm=TRUE
topAlg.HeavyFlavorHadronFilter.RequestBottom=TRUE
topAlg.HeavyFlavorHadronFilter.Request_cQuark=TRUE
topAlg.HeavyFlavorHadronFilter.Request_bQuark=TRUE
topAlg.HeavyFlavorHadronFilter.RequestSpecificPDGID=FALSE
topAlg.HeavyFlavorHadronFilter.RequireTruthJet=TRUE
topAlg.HeavyFlavorHadronFilter.TruthContainerName="AntiKt4TruthJets"
topAlg.HeavyFlavorHadronFilter.JetPtMin=8.0*GeV
topAlg.HeavyFlavorHadronFilter.JetEtaMax=5.0
topAlg.HeavyFlavorHadronFilter.BottomPtMin=3.0*GeV
topAlg.HeavyFlavorHadronFilter.CharmPtMin=3.0*GeV
topAlg.HeavyFlavorHadronFilter.BottomEtaMax=2.7
topAlg.HeavyFlavorHadronFilter.CharmEtaMax=2.7
topAlg.HeavyFlavorHadronFilter.DeltaRFromTruth=0.3
try:
   StreamEVGEN.RequireAlgs +=  [ "HeavyFlavorHadronFilter" ]
except Exception, e:
   pass


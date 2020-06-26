###############################################################
#
#        Generation of Lambda_b0 -> Lambda_0 mu+ mu-
#
#==============================================================

#--------------------------------------------------------------
# Global job-driving options
#--------------------------------------------------------------

EvtMax = 1000
#EvtMax = MYNOEVENTS

antiParticle = False

filterLVL1trigger    = True
filterLVL2trigger    = True
pTcutLVL1trigger     = 6000
pTcutLVL2trigger     = 4000
filterHadrons        = True
bQuarksBSignalFilter = False

fastGeneration      = False
doNotWritePOOL      = False
readRndFromFile     = False
outLevelBChains     = DEBUG
ntupleType          = "AANT"
bothLambdaBFinsel   = False
forceHadronization  = False
repeatHadronization = 20.
oldTypeSetup        = False

#--------------------------------------------------------------
# General application configuration options
#--------------------------------------------------------------

if oldTypeSetup:
    theApp.setup( MONTECARLO )
else:
    import AthenaCommon.AtlasUnixGeneratorJob

from PartPropSvc.PartPropSvcConf import PartPropSvc
svcMgr += PartPropSvc()
    

#--------------------------------------------------------------
# Private application configuration options
#--------------------------------------------------------------
svcMgr.AuditorSvc.Auditors  = [ "ChronoAuditor" ]

#--------------------------------------------------------------
# Algorithms 
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topAlg=AlgSequence()

from PythiaB.PythiaBConf import PythiaB
topAlg += PythiaB()
PythiaB = topAlg.PythiaB

from EvtGen_i.EvtGen_iConf import EvtDecay
topAlg += EvtDecay()
EvtDecay = topAlg.EvtDecay

from GeneratorFilters.GeneratorFiltersConf import BSignalFilter
topAlg += BSignalFilter()
BSignalFilter = topAlg.BSignalFilter

theApp.ExtSvc += [ "AtRndmGenSvc" ]

#--------------------------------------------------------------
# Number of events to be accepted,
# rewritten if use B job submition script
# RunNumber, FirstEvent rewritten if use B job submition script
#--------------------------------------------------------------

if oldTypeSetup:
    EventSelector = Service ("EventSelector")

theApp.EvtMax = EvtMax
svcMgr.EventSelector.RunNumber   = 18900
#EventSelector.RunNumber   = MYDSN
svcMgr.EventSelector.FirstEvent  = 0
#EventSelector.FirstEvent  = MYSKIP

#--------------------------------------------------------------
#User random number seeds - re-written if use B job submition script
#--------------------------------------------------------------

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc()
svcMgr.AtRndmGenSvc.Seeds = [ "PYTHIA 4789899 1890000001", "PYTHIA_INIT 820021 3347532", "EVTGEN 4789899 1890000001" ]
#AtRndmGenSvc.Seeds = [ "PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532", "EVTGEN 4789899 821000366" ]
#AtRndmGenSvc.Seeds = [ "PYTHIA 4789899 MYDSNMYRAN", "PYTHIA_INIT 820021 2347532", "EVTGEN 4789899 MYDSNMYRAN" ]
if readRndFromFile:
    svcMgr.AtRndmGenSvc.ReadFromFile = true;

#--------------------------------------------------------------
# Output level (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------

svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages 

#--------------------------------------------------------------
# Parameters specific to PythiaB
#--------------------------------------------------------------

include( "EvtGen_i/StopPytWeakBdecays.py" )

# Apply Lambda_b0 user-finsel selection
PythiaB.ForceDecayChannel = "LambdabmumuLambda"
if antiParticle:
    PythiaB.DecayChannelParameters = [-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
else:
    PythiaB.DecayChannelParameters = [+1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

if bothLambdaBFinsel:
    PythiaB.DecayChannelParameters = [ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

#--------------------------------------------------------------
# Pythia parameters optimal for beauty production
#--------------------------------------------------------------

#include( "PythiaB/Btune.py" )

# 'msel 5' is only for fast tests! for correct b-producion you should use 'msel 1'
if fastGeneration:
    PythiaB.PythiaCommand += ["pysubs ckin 3 6.", "pysubs msel 5"]
else:
    PythiaB.PythiaCommand += ["pysubs ckin 3 6.", "pysubs msel 1"]

# Ignore almost all pythia errors
PythiaB.PythiaCommand += [ "pydat1 mstu 22 1000" ]

# Selections on b quarks
if antiParticle:
    PythiaB.cutbq = ["0. 0. or 9. 3.5"]
else:
    PythiaB.cutbq = ["9. 3.5 or 0. 0."]
#PythiaB.cutbq = ["0. 1000. or 0. 1000."]

# Number of repeated hadronization mhadr
PythiaB.mhadr = repeatHadronization

# Forced hadronizaition to Lambda_b0
if forceHadronization:
    PythiaB.forceHadronization = "Lambdab"
    if antiParticle:
        PythiaB.HadronizationParameters = [-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    else:
        PythiaB.HadronizationParameters = [+1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

# For how many events store B-chain in NTUPLE
BSignalFilter.OutputLevel  = outLevelBChains
BSignalFilter.SignaltoNtup = EvtMax

# Level 1 Muon Cuts
BSignalFilter.LVL1MuonCutOn = False
BSignalFilter.LVL1MuonCutPT = pTcutLVL1trigger
BSignalFilter.LVL1MuonCutEta = 2.5

# Level 2 lepton cuts - these will only function if LVL1 trigger used
BSignalFilter.LVL2MuonCutOn = False
BSignalFilter.LVL2MuonCutPT = pTcutLVL2trigger
BSignalFilter.LVL2MuonCutEta = 2.5

# Hadronic tracks cuts
BSignalFilter.Cuts_Final_hadrons_switch = False
BSignalFilter.Cuts_Final_hadrons_pT = 500.0
BSignalFilter.Cuts_Final_hadrons_eta = 2.5

# Switches for trigger levels
if filterLVL1trigger:
    BSignalFilter.LVL1MuonCutOn = True
if filterLVL2trigger:
    BSignalFilter.LVL2MuonCutOn = True
if filterHadrons:
    BSignalFilter.Cuts_Final_hadrons_switch = True
    if antiParticle:
        BSignalFilter.BParticle_cuts = -5122
    else:
        BSignalFilter.BParticle_cuts = 5122

if bQuarksBSignalFilter:
    BSignalFilter.StoreBQuarks = True

#--------------------------------------------------------------
# Your control of EvtGen
#--------------------------------------------------------------

# If you do not define a name to your Decay-table file here 
# then EvtGen will work with a default decay table DECAY.DEC
EvtDecay.OutputLevel = outLevelBChains
if antiParticle:
    EvtDecay.userDecayTableName = "MYDECAY_antiLb2Lll.DEC"
else:
    EvtDecay.userDecayTableName = "MYDECAY_Lb2Lll.DEC"

#--------------------------------------------------------------
# Histogram & Ntuple Persistency
#--------------------------------------------------------------

if ntupleType == "ROOT":
    theApp.HistogramPersistency = "ROOT"
    svcMgr.NTupleSvc = Service( "NTupleSvc" )
    svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='athena.evgen.ntup' OPT='NEW'" ]

if ntupleType == "HBOOK":
    theApp.Dlls += [ "HbookCnv" ]
    theApp.HistogramPersistency = "HBOOK"
    HbookHistSvc = Service( "HbookHistSvc" )
    HbookHistSvc.NPAWC = 1500000
    #HbookHistSvc.RecordLength = 1024
    #HbookHistSvc.IQUEST10 = 65000
    svcMgr.NTupleSvc = Service( "NTupleSvc" )
    svcMgr.NTupleSvc.NPAWC = 15000000
    svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='athena.evgen.ntup' OPT='NEW'" ]

if ntupleType == "AANT":
    theApp.Dlls += [ "AnalysisTools" ]
    THistSvc = Service ( "THistSvc" )
    THistSvc.Output = ["AANT DATAFILE='athena.evgen.ntup' OPT='NEW'"]
    theApp.TopAlg += [ "AANTupleStream" ]
    AANTupleStream = Algorithm( "AANTupleStream" )
    AANTupleStream.ExtraRefNames = [ "" ]
    AANTupleStream.OutputName = 'athena.evgen.ntup'
    AANTupleStream.ExistDataHeader = False
    AANTupleStream.OutputLevel = VERBOSE

HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.root"

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------

if not doNotWritePOOL:
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

    Stream1 = AthenaPoolOutputStream( "Stream1" )
    Stream1.OutputFile = "athena.evgen.root"
    # 2101 = EventInfo
    # 133273 = MCTruth (HepMC)
    Stream1.ItemList += [ "2101#*", "133273#*" ]
    Stream1.AcceptAlgs = [ "BSignalFilter" ]

#==============================================================
#
# End of job options file
#
###############################################################

#############################################################################
#
#             Generation of B events   B->MYSIGNAL
#             
#            Adopt MYDECAY.DEC according to your channel
#
#############################################################################
# B-event generation using PythiaB and EvtGen
# Evtent generation, hadronization and Strong B-decays are 
# provided by Pythia, weak decays are done in EvtGen
#
# 
# The Generator is under the development - to varios users applications 
# Following functions are already available:
# Generation of B events  bb->X  
# Generation of B->any exlusive decay channel selected by User
# Repeated hadronization can be activated to speed-up b-production
# The only selection of the events is a b-quark selection
# No LVL1 LVL2 OFFLINE selections in this releas - under prepartion
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from PartPropSvc.PartPropSvcConf import PartPropSvc
svcMgr += PartPropSvc()


#--------------------------------------------------------------
# Private Application Configuration options
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

theApp.ExtSvc += ["AtRndmGenSvc"]
#--------------------------------------------------------------
# Number of events to be accepted !! (default is 10)
# re-written if use B job submition script
# RunNumber, FirstEvent re-written if use B job submition script
#--------------------------------------------------------------
#--------------------------------------------------------------
#User random number seeds - re-written if use B job submition script
#--------------------------------------------------------------
# Following line introduced 18.April 2006 to allow AtRndmGenSvc. control EvtGen rndms
# At the same time the EvtGenRandomSeed was commented 
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc()
svcMgr.AtRndmGenSvc.Seeds = ["PYTHIA 5489899 989240512", "PYTHIA_INIT 820021 2347532", "EVTGEN 4789899 821000366" ]
# AtRndmGenSvc.ReadFromFile = true;
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel               = 2
#--------------------------------------------------------------
# Number of events to be accepted !! (default is 10)
# re-written if use B job submition script
# RunNumber, FirstEvent re-written if use B job submition script
#--------------------------------------------------------------
theApp.EvtMax = 500 
svcMgr.EventSelector.RunNumber   = 1
svcMgr.EventSelector.FirstEvent  = 1
#--------------------------------------------------------------				 
#              PARAMETERS  SPECIFIC  TO   PYTHIAB
#--------------------------------------------------------------
include( "EvtGen_i/StopPytWeakBdecays.py" )
#--------------------------------------------------------------
# --------  PYTHIA PARAMETERS OPTIMAL FOR BEAUTY PRODUCTION  --
#--------------------------------------------------------------
#  'msel 5' is only for fast tests! 
#  for correct b-producion you should use 'msel 1'
#include( "PythiaB/Btune.py" )

PythiaB.PythiaCommand += ["pysubs ckin 3 15.",
				 "pysubs msel 1"]
#  ------------- Selections on b  quarks   -------------
PythiaB.cutbq = ["0. 102.5 and 10. 2.5"]
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  14. 
#  ------------- For how many events store B-chain in NTUPLE -------------
BSignalFilter.SignaltoNtup = 10
BSignalFilter.LVL1MuonCutOn = TRUE
BSignalFilter.LVL1MuonCutPT = 6000.0
BSignalFilter.LVL1MuonCutEta = 2.5
BSignalFilter.LVL2MuonCutOn = TRUE
BSignalFilter.LVL2ElectronCutOn = FALSE
BSignalFilter.LVL2MuonCutPT = 3000.0
BSignalFilter.LVL2MuonCutEta = 2.5
BSignalFilter.Cuts_Final_hadrons_switch = TRUE
BSignalFilter.Cuts_Final_hadrons_pT = 500.0
BSignalFilter.Cuts_Final_hadrons_eta = 2.5
#--------------------------------------------------------------
# YOUR CONTROL OF EVTGEN 
#--------------------------------------------------------------
# If you do not define a name to your Decay-table file here 
# then EvtGen will work with a default decay table DECAY.DEC
EvtDecay.userDecayTableName = "Bs2Jpsiphi.DEC"

# EvtGen random seed interface. Does not affect PYTHIA seeds.
# This is a TEMPORARY FIX as it does not abide by ATLAS software rules.
# James R Catmore, 16/2/2004
# Followign line commented 17.April 2006 to allow AtRndmGenSvc
# EvtDecay.EvtGenRandomSeed = 645754;

#--------------------------------------------------------------
# Histogram & Ntuple Persistency
#--------------------------------------------------------------
# Change the following line to "RootHistCnv" for ROOT persistency
#theApp.Dlls                += [ "RootHistCnv" ]
# Change the following line to "ROOT" for ROOT persistency
theApp.HistogramPersistency = "ROOT"
#--------------------------------------------------------------
# NTuple output file
#--------------------------------------------------------------
#RootHistSvc = Service( "RootHistSvc" )
#HbookHistSvc.NPAWC = 1500000
#HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
#HistogramPersistencySvc.OutputFile  = "histo.root"
svcMgr.NTupleSvc.Output = [ "FILE1  DATAFILE='pythiaB.root'  OPT='NEW'  TYP='ROOT'" ]

###############################################################
# Add POOL persistency
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile = "pythiaB.pool.root"
# 2101 = EventInfo
# 133273 = MCTruth (HepMC)
Stream1.ItemList += [ "2101#*", "133273#*" ]
Stream1.AcceptAlgs = ["BSignalFilter"]

#==============================================================
#
# End of job options file
#
###############################################################

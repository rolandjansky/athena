#############################################################################
#
#             Generation of B events   bb->X
#             
#
# B-event generation using PythiaB and EvtGen
# Evtent generation, hadronization and Strong B-decays are 
# provided by Pythia, weak decays are done in EvtGen
#
# 
# The Generator is under the development - to include varios users applications 
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
theApp.EvtMax = 12
svcMgr.EventSelector.RunNumber   = 1
svcMgr.EventSelector.FirstEvent  = 1
#--------------------------------------------------------------
#User random number seeds - re-written if use B job submition script
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc()
svcMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532", "EVTGEN 4789899 821000366"]
# svcMgr.AtRndmGenSvc.ReadFromFile = true;
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel               = 2
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

PythiaB.PythiaCommand += ["pysubs ckin 3 6.",
				 "pysubs msel 1"]
#  ------------- Selections on b  quarks   -------------
PythiaB.cutbq = ["7. 4.5 or 7. 4.5"]
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  3. 
#  ------------- For how many events store B-chain in NTUPLE -------------
BSignalFilter.SignaltoNtup = 10
#--------------------------------------------------------------
# YOUR CONTROL OF EVTGEN 
#--------------------------------------------------------------
# If you do not define a name to your Decay-table file here 
# then EvtGen will work with a default decay table DECAY.DEC
#EvtDecay.userDecayTableName = "MYDECAY.DEC";
#EvtGen random seed interface. Does not affect PYTHIA seeds.
#This is a TEMPORARY FIX as it does not abide by ATLAS software rules.
#James R Catmore, 16/2/2004
#EvtDecay.EvtGenRandomSeed = 645754;

###############################################################

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
Stream1.ForceRead=TRUE
Stream1.OutputFile = "pythiaB.pool.root"
# 2101 = EventInfo
# 133273 = MCTruth (HepMC)
Stream1.ItemList += [ "2101#*", "133273#*" ]

include("AthenaSealSvc/AthenaSealSvc_joboptions.py" )
AthenaSealSvc.CheckDictionary = TRUE

#==============================================================
#
# End of job options file
#
###############################################################

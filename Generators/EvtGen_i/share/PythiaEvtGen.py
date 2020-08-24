#############################################################################
#
#             Generation of any Pythia event (here 81,82 for example)
#             
#                 with all weak B-decays done in EvtGen 
#
#############################################################################
#
# Evtent generation, hadronization and Strong B-decays are 
# provided by Pythia, weak decays are done in EvtGen
#
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
job=AlgSequence()

from Pythia_i.Pythia_iConf import Pythia
job += Pythia()
Pythia = job.Pythia

from EvtGen_i.EvtGen_iConf import EvtDecay
job += EvtDecay()
EvtDecay = job.EvtDecay

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
svcMgr.MessageSvc.OutputLevel               = DEBUG
#--------------------------------------------------------------				 
#              PARAMETERS  SPECIFIC  TO   PYTHIA
#--------------------------------------------------------------
from PythiaB.PythiaBConf import PythiaB
#job += PythiaB()
#PythiaB = job.PythiaB

include( "EvtGen_i/StopPytWeakBdecays.py" )

Pythia.PythiaCommand += ["pysubs msel 5"]
#--------------------------------------------------------------
# YOUR CONTROL OF EVTGEN 
#--------------------------------------------------------------
# If you do not define a name to your Decay-table file here 
# then EvtGen will work with a default decay table DECAY.DEC
# EvtDecay.userDecayTableName = "MYDECAY.DEC";
# EvtGen random seed interface. Does not affect PYTHIA seeds.
# This is a TEMPORARY FIX as it does not abide by ATLAS software rules.
# James R Catmore, 16/2/2004
# EvtDecay.EvtGenRandomSeed = 645754;

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
Stream1.OutputFile = "pythiaB.pool.root"
# 2101 = EventInfo
# 133273 = MCTruth (HepMC)  
Stream1.ItemList += [ "2101#*", "133273#*" ]

#==============================================================
#
# End of job options file
#
###############################################################

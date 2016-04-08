###############################################################
#
# Job options file
# Generate events with Matchig
# Example: gg/gb -> t[b]H+, t->bqq, H+->taunu with mH+=300 GeV
#
# Martin Flechl, 29/1/2009
# mflechl@cern.ch
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

# make sure we are loading the ParticleProperty service
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = INFO

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed
theApp.EvtMax = 10
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532"]

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from Pythia_i.Pythia_iConf import Pythia
job += Pythia()
job.Pythia.SetAtlasDefaults = False
job.Pythia.PythiaCommand = ["pyinit user matchig", "pyinit pylistf 1","pyinit dumpr 1 1",
                        "pysubs msel 0", "pysubs msub 161 1", "pysubs msub 401 1",
                        "pysubs msub 402 1", "pypars mstp 129 1000", "pypars mstp 81 0"]

job.Pythia.PythiaCommand += ["pydat1 paru 141 30.",                 #tan beta
                             "pydat2 pmas 37 1 300."]               #H+ mass

#uncomment when using TAUOLA/PHOTOS
#job.Pythia.PythiaCommand += ["pydat1 parj 90 20000",   # Turn off lepton radiation
#                             "pydat3 mdcy 15 1 0"]     # Turn off tau decays

#top decay
job.Pythia.PythiaCommand += ["pydat3 mdme 41 1 0",
                             "pydat3 mdme 42 1 0",
                             "pydat3 mdme 43 1 0",
                             "pydat3 mdme 44 1 0",
                             "pydat3 mdme 45 1 0",
                             "pydat3 mdme 46 1 1", # t->Wb
#                             "pydat3 mdme 46 1 4", # t->Wb    #for light H+
                             "pydat3 mdme 48 1 0",
                             "pydat3 mdme 49 1 0", # t->H+b
#                             "pydat3 mdme 49 1 5", # t->H+b   #for light H+
                             "pydat3 mdme 50 1 0",
                             "pydat3 mdme 51 1 0",
                             "pydat3 mdme 52 1 0",
                             "pydat3 mdme 53 1 0",
                             "pydat3 mdme 54 1 0",
                             "pydat3 mdme 55 1 0"]

#W decay
job.Pythia.PythiaCommand += ["pydat3 mdme 190 1 1", # to q q'
                             "pydat3 mdme 191 1 1", # to q q'
                             "pydat3 mdme 192 1 0",
                             "pydat3 mdme 194 1 1", # to q q'
                             "pydat3 mdme 195 1 1", # to q q'
                             "pydat3 mdme 196 1 0",
                             "pydat3 mdme 198 1 0",
                             "pydat3 mdme 199 1 1", # to q q'
                             "pydat3 mdme 200 1 0",
                             "pydat3 mdme 206 1 0", #enu
                             "pydat3 mdme 207 1 0", #munu
                             "pydat3 mdme 208 1 0"] #taunu

#H+ decay
job.Pythia.PythiaCommand += ["pydat3 mdme 503 1 0",
                             "pydat3 mdme 504 1 0",
                             "pydat3 mdme 505 1 0",
                             "pydat3 mdme 507 1 0",
                             "pydat3 mdme 508 1 0",
                             "pydat3 mdme 509 1 1", #H+ -> tau nu
                             "pydat3 mdme 511 1 0",
                             "pydat3 mdme 512 1 0",
                             "pydat3 mdme 513 1 0",
                             "pydat3 mdme 514 1 0",
                             "pydat3 mdme 515 1 0",
                             "pydat3 mdme 516 1 0",
                             "pydat3 mdme 517 1 0",
                             "pydat3 mdme 518 1 0",
                             "pydat3 mdme 519 1 0",
                             "pydat3 mdme 520 1 0",
                             "pydat3 mdme 521 1 0",
                             "pydat3 mdme 522 1 0",
                             "pydat3 mdme 523 1 0",
                             "pydat3 mdme 524 1 0",
                             "pydat3 mdme 525 1 0",
                             "pydat3 mdme 526 1 0",
                             "pydat3 mdme 527 1 0",
                             "pydat3 mdme 528 1 0",
                             "pydat3 mdme 529 1 0"]

#tau decay
job.Pythia.PythiaCommand += ["pydat3 mdme 89 1 0", #enu
                             "pydat3 mdme 90 1 0"] #munu

from TruthExamples.TruthExamplesConf import PrintMC
job += PrintMC()
job.PrintMC.FirstEvent = 1
job.PrintMC.LastEvent = 3

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.OutputFile = "matchig.pool.root"
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]

#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------

#include( 'CBNT_Athena/CBNT_AthenaAware_jobOptions.py' )
#include( 'CBNT_Athena/CBNT_EventInfo_jobOptions.py' )                     
#include( 'RecExCommon/CBNT_Truth_jobOptions.py' )
 
#theApp.CreateSvc += ['AthenaSealSvc']
#ServiceMgr += CfgMgr.AthenaSealSvc()

#---------------------------------------------------------------
# AANT persistency
#---------------------------------------------------------------
#if not 'AANT_OUTPUT' in dir():
#   AANT_OUTPUT = "matchig.aan.root"
#   pass
#if not hasattr(ServiceMgr, 'THistSvc'):
#   from GaudiSvc.GaudiSvcConf import THistSvc
#   ServiceMgr += THistSvc()
#ServiceMgr.THistSvc.Output += ["AANT DATAFILE='%s' OPT='RECREATE'" % AANT_OUTPUT]

#from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
#topSequence += AANTupleStream(
#   ExistDataHeader = False,
#   WriteInputDataHeader = False,
#   OutputName = AANT_OUTPUT,
#   OutputLevel = WARNING
#)

#---------------------------------------------------------------

#==============================================================
#
# End of job options file
#
###############################################################

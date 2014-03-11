
# dijet production with pythia
from AthenaCommon.AlgSequence import AlgSequence 
topAlg = AlgSequence("TopAlg") 

from Pythia_i.Pythia_iConf import Pythia
topAlg += Pythia()

Pythia = topAlg.Pythia

theApp.EvtMax = 50

Pythia.PythiaCommand = [ "pysubs msel 0",
                         "pysubs ckin 3 560.",
                         "pysubs ckin 4 1120.",
                         "pysubs msub 11 1",
                         "pysubs msub 12 1",
                         "pysubs msub 13 1",
                         "pysubs msub 68 1",
                         "pysubs msub 28 1",
                         "pysubs msub 53 1",
                         "pypars mstp 82 4",
                         "pyinit pylisti 12",
                         "pyinit pylistf 1",
                         "pystat 1 3 4 5",
                         "pyinit dumpr 1 5" ]

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel               = 3

from TruthExamples.TruthExamplesConf import PrintMC
topAlg += PrintMC()
topAlg.PrintMC.McEventKey = "GEN_EVENT"
topAlg.PrintMC.VerboseOutput = TRUE
topAlg.PrintMC.PrintStyle = "Barcode"
topAlg.PrintMC.FirstEvent = 1
topAlg.PrintMC.LastEvent =  5

# reconstruct some jets
from RecExConfig.RecFlags  import rec
rec.doTruth = True

from JetRec.JetRecFlags import jetFlags
jetFlags.inputFileType = "GEN"

from JetRec.JetGetters import *
make_StandardJetGetter('Cone',0.7,'Truth')
make_StandardJetGetter('Cone',0.4,'Truth')
make_StandardJetGetter('Kt',0.6,'Truth')
make_StandardJetGetter('Kt',0.4,'Truth')

make_StandardJetGetter('SISCone',0.7,'Truth')
make_StandardJetGetter('SISCone',0.4,'Truth')
make_StandardJetGetter('AntiKt',0.6,'Truth')
make_StandardJetGetter('AntiKt',0.4,'Truth')

# helper alg to print PID
from ValgrindRTTJobs.ValgrindRTTJobsConf import ValgrindHelperAlg
topAlg+=ValgrindHelperAlg()

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

## tweak the default commit interval for montecarlo needs
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.AthenaPoolCnvSvc.CommitInterval = 10

from PoolSvc.PoolSvcConf import PoolSvc
from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc

Stream1 = AthenaPoolOutputStream("Stream1")
Stream1.ItemList += ["EventInfo#*", "McEventCollection#*" ]
# Stream1.ForceRead=True
Stream1.OutputFile = "McEvent.root"

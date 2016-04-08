## Special setup for event generation
include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")
## Other includes needed by the physics file
include("PartPropSvc/PartPropSvc.py")

theApp.EvtMax = 5

from AthenaCommon.Logging import logging
svcMgr.MessageSvc.OutputLevel = INFO
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

## Random seeds
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc()
AtRndmGenSvc.Seeds = ["PYTHIA 330020611 841000366", "PYTHIA_INIT 824021 3247532"]

## PYTHIA in MadGraph mode
from Pythia_i.Pythia_iConf import Pythia
topAlg += Pythia()
topAlg.Pythia.PythiaCommand = ["pyinit user madgraph",
                               "pypars mstp 81 21",
                               "pystat 1 3 4 5",
                               "pyinit dumpr 1 5"]

## Print out events
from TruthExamples.TruthExamplesConf import PrintMC
topAlg += PrintMC()

## Ntuple service output
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
from PoolSvc.PoolSvcConf import PoolSvc
from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc

## Output data stream config
stream1 = AthenaPoolOutputStream("StreamEVGEN")
stream1.ItemList += ["EventInfo#*", "McEventCollection#*"]
stream1.OutputFile = "test.pool.root"

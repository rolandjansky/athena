###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------

# IMPORTANT!!! The following line adds the correct meta data to Pythia output!
import AthenaCommon.AtlasUnixGeneratorJob
#--------------------------------------------------------------

from AthenaCommon.Logging import logging

evgenLog = logging.getLogger('csc_evgen')

# special setup for event generation
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

# remember output level I set

evgenMsgLevel = INFO

# other includes needed by the physics file
include( "PartPropSvc/PartPropSvc.py" )

from RngComps.RngCompsConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc()

# get a handle on topalg
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

svcMgr.MessageSvc.OutputLevel = evgenMsgLevel
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
atRndmGenSvc = svcMgr.AtRndmGenSvc
AtRndmGenSvc.Seeds = ["PYTHIA 330020611 841000366", "PYTHIA_INIT 824021 3247532"]

from Pythia_i.Pythia_iConf import Pythia
topAlg += Pythia()

from TruthExamples.TruthExamplesConf import PrintMC
topAlg += PrintMC()
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 5
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
Pythia = topAlg.Pythia
Pythia.PythiaCommand = ["pyinit user lhef"]
Pythia.PythiaCommand +=  [
    "pypars mstp 81 21",
    "pystat 1 3 4 5",
    "pyinit dumpr 1 5"]
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
from PoolSvc.PoolSvcConf import PoolSvc
from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc

Stream1 = AthenaPoolOutputStream("StreamEVGEN")
Stream1.ItemList += ["EventInfo#*",
                     "McEventCollection#*" ]
Stream1.OutputFile = "test.pool.root"
#==============================================================
#
# End of job options file
#
###############################################################

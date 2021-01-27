#import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA8 4789899 989240512",
                                 "PYTHIA8_INIT 820021 2347532"]

svcMgr.MessageSvc.OutputLevel = INFO

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from Pythia8_i.Pythia8_iConf import Pythia8_i

topSequence+=Pythia8_i()
#topSequence.Pythia8_i.CollisionEnergy = 7000

topSequence.Pythia8_i.HDF5File = "/eos/home-a/afreegar/hdf5_files/Z1-g0-repack.hdf5"


# Printing service
from TruthIO.TruthIOConf import DumpMC
topSequence += DumpMC()

# ----------------------------------------------------------------------
# Ntuple service output
# ----------------------------------------------------------------------
#import AthenaPoolCnvSvc.WriteAthenaPool
#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

#stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
#stream1.WritingTool = "AthenaOutputStreamTool"
#stream1.OutputFile = "epos_events.pool.root"
#stream1.ForceRead = TRUE;
#stream1.TakeItemsFromInput = True
#stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]


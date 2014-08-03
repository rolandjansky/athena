import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AppMgr import theApp

include( "PartPropSvc/PartPropSvc.py" )

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr

#* load relevant libraries
ServiceMgr.AuditorSvc.Auditors  = [ "ChronoAuditor" ]
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()

ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 85909879 9707499", "PYTHIA_INIT 87928165 11160529"]

#--------------------------------------------------------------
# Persistency services
#--------------------------------------------------------------
# you shouldn't need to change this
ServiceMgr.EventPersistencySvc.CnvServices     = [ "McCnvSvc" ];  
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# OUTPUT PRINTOUT LEVEL
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
# you can override this for individual modules if necessary
ServiceMgr.MessageSvc.OutputLevel               = 3
#--------------------------------------------------------------
#GENERATORS SETUP (select by uncommenting/commenting)
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from Pythia_i.Pythia_iConf import Pythia
topSequence += Pythia()

#-------------------------------------------------------------
# define your physics process here
# Declare the Pythia to be the Generator to be used
topSequence.Pythia.PythiaCommand += ["pysubs msel 6"]
#selection of the subprocess gg -> ttbar
topSequence.Pythia.PythiaCommand += ["pysubs msub 81 1"]
topSequence.Pythia.PythiaCommand += ["pysubs msub 82 1"]

#-------------------------------------------------------------

#-------------------------------------------------------------
# Pool Persistency
#-------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

# EventInfo Converters
include( "EventAthenaPool/EventAthenaPool_joboptions.py")
PoolSvc = Service( "PoolSvc" )
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.ItemList+=["EventInfo#*"];        	 
Stream1.ItemList+=[ "McEventCollection#*" ]
Stream1.ForceRead=TRUE   
Stream1.OutputFile = "McEvent.root"
#PoolSvc.Output    = "McEvent.root"
#==============================================================

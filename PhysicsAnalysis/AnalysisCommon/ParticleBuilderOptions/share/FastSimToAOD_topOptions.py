#
# Run Atlfast on generated events and produce AOD
#
# if you switch ON the Fast TrackParticle, you MUST select a detector description version:
# DetDescrVersion="ATLAS-DC3-02"
# Because the Fast TrackParticle Creator needs a well defined detector description
#
#  usage: athena -c 'INPUT=["myGEN.root"]; OUTPUT="myAOD.root"; EVTMAX=100' FastSimToAOD_jobOptions.py
#
#          INPUT  : list of input file names
#          OUTPUT : output file name
#          EVTMAX : number of events
#


##############################
# get user options
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
#EventSelector.BackNavigation = True

# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc

# input file name
if not 'INPUT' in dir():
    #INPUT = [ "rfio:/castor/cern.ch/atlas/project/dc2/preprod/evgen804/dc2.002896.pyt_h130_llll.evgen804/data/dc2.002896.pyt_h130_llll.evgen804._0001.pool.root" ]
    #INPUT = [ "ESD.pool.root" ]
    INPUT = [ "McEvent.root" ]

# number of events
if not 'EVTMAX' in dir():
    EVTMAX = 10

# output file name
if not 'OUTPUT' in dir():
    OUTPUT = "AOD_FastSim.pool.root"


##############################
# run Atlfast

# read GEN Pool
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# Input file name
ServiceMgr.EventSelector.InputCollections = INPUT

# number of events
theApp.EvtMax = EVTMAX

include( "AtlfastAlgs/Atlfast_MakeAOD.py" )

# flags for AOD
Stream1.OutputFile = OUTPUT

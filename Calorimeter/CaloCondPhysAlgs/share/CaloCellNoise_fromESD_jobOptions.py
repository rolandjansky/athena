# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
###############################################################
#
# Job options file to run simple algorithm computing noise per cell
#
#==============================================================
from AthenaCommon.DetFlags import DetFlags

from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('data')

DetFlags.detdescr.all_setOn()
DetFlags.Muon_setOff()
DetFlags.ALFA_setOff()

include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "PartPropSvc/PartPropSvc.py" )
#
# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp
#
# Pool input
#
PoolESDInput= [
"rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_900GeV/physics_CosmicCalo/00176051/data11_900GeV.00176051.physics_CosmicCalo.recon.ESD.x81/data11_900GeV.00176051.physics_CosmicCalo.recon.ESD.x81._lb0002._SFO-ALL._0001.1",
"rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_900GeV/physics_CosmicCalo/00176051/data11_900GeV.00176051.physics_CosmicCalo.recon.ESD.x81/data11_900GeV.00176051.physics_CosmicCalo.recon.ESD.x81._lb0003._SFO-ALL._0001.1",
"rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_900GeV/physics_CosmicCalo/00176051/data11_900GeV.00176051.physics_CosmicCalo.recon.ESD.x81/data11_900GeV.00176051.physics_CosmicCalo.recon.ESD.x81._lb0004._SFO-ALL._0001.1",
]
svcMgr.EventSelector.InputCollections = PoolESDInput
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = PoolESDInput
from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.Input.Files = athenaCommonFlags.FilesInput()


# the Tile, LAr and Calo detector description package
#from AthenaCommon.GlobalFlags import jobproperties
#jobproperties.Global.DetDescrVersion='ATLAS-GEO-16-00-00'


from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "TileConditions/TileConditions_jobOptions.py" ) 
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
LArADC2MeVCondAlgDefault()

from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
CaloNoiseCondAlg ('totalNoise')
CaloNoiseCondAlg ('electronicNoise')

# Trigger decision tool

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList = ['ds']
# set up trigger config service 
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt 


from CaloCondPhysAlgs.CaloCondPhysAlgsConf import CaloCellNoiseAlg
myNoise = CaloCellNoiseAlg("CaloCellNoiseAlg")
myNoise.doMC = False
myNoise.readNtuple = False
myNoise.doFit = True
# uncomment to activate trigger decision-  example below is using L1_RD1_EMPTY, checked against 16.6.1 release
#myNoise.TrigDecisionTool = tdt
#myNoise.TriggerChain = "L1_RD1_EMPTY"
myNoise.doLumiFit = False

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += myNoise


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
myNoise.OutputLevel        = INFO

if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  = ["file1 DATAFILE='cellnoise.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 20000
theApp.EvtSel = "EventSelector"


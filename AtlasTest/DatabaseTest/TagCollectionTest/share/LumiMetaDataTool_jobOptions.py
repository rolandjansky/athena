DetDescrVersion = "ATLAS-GEO-10-00-01" 
ConditionsTag   = "COMCOND-REPPST-006-01 "     

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'                 # For 2 TeV data    
#globalflags.DataSource = 'geant4'              # For Monte Carlo
globalflags.ConditionsTag = ConditionsTag
globalflags.DetDescrVersion = DetDescrVersion                 
#globalflags.InputFormat = 'pool'
globalflags.print_JobProperties()
#from IOVDbSvc.CondDB import conddb

theApp.EvtMax = 20

import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
ToolSvc = Service( "ToolSvc" )

from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
svcMgr.PoolSvc.ReadCatalog = [ "XMLFileCatalog_file:/afs/cern.ch/atlas/offline/test/POOLCollectionTools/PoolFileCatalog.xml" ]

# add LumiBlockMetaDataTool to ToolSvc and configure
from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )
LumiBlockMetaDataTool.calcLumi = True # False by default

# add ToolSvc.LumiBlockMetaDataTool to MetaDataSvc
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import MetaDataSvc
svcMgr += MetaDataSvc( "MetaDataSvc" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool ]

svcMgr.EventSelector.InputCollections =  ["PFN:/afs/cern.ch/atlas/offline/test/POOLCollectionTools/TAG.143392._000002.piece.root"]; # The input file name
#svcMgr.EventSelector.InputCollections =  ["PFN:/afs/cern.ch/user/c/cranshaw/scratch2/data10_7TeV.00152166.physics_L1Calo.merge.TAG.r1297_p161_tid143392_00/test.root"]; # The input file name
svcMgr.EventSelector.CollectionType = "ExplicitROOT"
svcMgr.EventSelector.Query="LumiBlockN<220"

svcMgr.MessageSvc.OutputLevel = DEBUG
svcMgr.MessageSvc.debugLimit = 500000




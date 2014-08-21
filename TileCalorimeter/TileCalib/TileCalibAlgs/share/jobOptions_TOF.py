##########################
# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()

import AthenaPoolCnvSvc.ReadAthenaPool

theAuditorSvc.Auditors  += [ "NameAuditor"]

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()  #Switched off to avoid geometry
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOn()
DetFlags.Print()

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-CommNF-00-00-00'
log.info( "DetDescrVersion = %s" % (jobproperties.Global.DetDescrVersion()) )
 
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
log.info( "GeoModelSvc.AtlasVersion = %s" % (GeoModelSvc.AtlasVersion) )
GeoModelSvc.IgnoreTagDifference = True

###############################################################################

include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include ("EventCommonAthenaPool/EventCommonAthenaPool_joboptions.py")
include( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )

###############################################################################

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TileCalibAlgs.TileCalibAlgsConf import TileTopCalibAlg
topSequence += TileTopCalibAlg("TileTopCalib")
TileTopCalib = topSequence.TileTopCalib


from TileCalibAlgs.TileCalibAlgsConf import TileTOFTool
TileTOF = TileTOFTool(name        = 'TileTOF',
                                  OutputLevel        = 3);

m_runNumber = 20919
m_runType = 1

ToolSvc += TileTOF;
 
TileTopCalib.TileCalibTools += [ TileTOF ];

print TileTopCalib

# The AOD input file
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0001._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0002._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0003._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0004._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0005._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0006._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0007._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0008._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0009._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0010._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0011._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0012._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0013._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0014._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0015._lumi0000._0001.1"]
ServiceMgr.EventSelector.InputCollections += ["rfio:/castor/cern.ch/grid/atlas/t0/perm/M4cosmics/0020919/ESD/M4.0020919.debug.L1TT-b00000010.ESD.v13002507.part0016._lumi0000._0001.1"]


# Number of Events to process
theApp.EvtMax = 1000000

# DUMP
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True  #true will dump data store contents

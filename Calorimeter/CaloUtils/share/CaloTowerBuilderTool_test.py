# $Id$
#
# @file CaloUtils/share/CaloTowerBuilderTool_test.py
# @author scott snyder <snyder@bnl.gov>
# @date Jul, 2013
# @brief Regression tests for CaloTowerBuilderTool.
#


from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.LAr_setOn()
DetFlags.detdescr.Tile_setOn()


import sys
import string
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
ServiceMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]
from AtlasGeoModel import LArGM
from AtlasGeoModel import TileGM

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc().GlobalTag = 'OFLCOND-SDR-BS14T-IBL-06'


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from CaloUtils import CaloUtilsConf

builder_tool = CaloUtilsConf.CaloTowerBuilderTool \
  (IncludedCalos = ['LAREM', 'LARHEC', 'TILE'])
ToolSvc += builder_tool
     
topSequence += CaloUtilsConf.CaloTowerBuilderToolTestAlg ('towertest')

theApp.EvtMax = 1

theApp.getHandle().Dlls += [ 'CaloUtils' ]

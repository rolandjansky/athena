include.block ( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py" )

###################################################################
#
# Job options file for setting up TT-Cell Map in POOL/COOL
#
#==================================================================

from PoolSvc.PoolSvcConf import PoolSvc
ServiceMgr += PoolSvc()
if not "prfile:poolcond/PoolCat_oflcond.xml" in ServiceMgr.PoolSvc.ReadCatalog :
    ServiceMgr.PoolSvc.ReadCatalog+= [ "prfile:poolcond/PoolCat_oflcond.xml" ]

from AthenaCommon.Logging import logging
logger = logging.getLogger( 'LArTTCellMap_ATLAS_jobOptions.py' )

if "LArDBConnection" not in dir() :
  LArDBConnection= ""

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
  LArDB = "LAR" 
else:
  LArDB = "LAR_OFL"

if LArDBConnection!="" :
     LArDB=""

from IOVDbSvc.CondDB import conddb
conddb.addFolder(LArDB,"/LAR/Identifier/LArTTCellMapAtlas"+LArDBConnection )

# read correct mapping for old geometries where information cannot be found from global tag
DetDescr = globalflags.DetDescrVersion()
if DetDescr.startswith("ATLAS-CSC-01") or DetDescr.startswith("ATLAS-DC"):
    conddb.addOverride("/LAR/Identifier/LArTTCellMapAtlas","LARIdentifierLArTTCellMapAtlas-DC3-02")



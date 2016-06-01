# IOVRecExCommissionSR1.py - setup IOVDbSvc for ID commissioning in SR1
# commissioning with real data, but temporarily using COOL1.2.x CMCPROD db
# setup for sqlite replica expected to be found in sqlite130/CMCPROD.db
#

include.block ( "IOVDbSvc/IOVRecExCommissionSR1.py" )

IOVCoolDBTech="sqlite"
IOVCoolDBProd="CMCPROD"
IOVCoolDBUser="ATLAS_COOL_READER"
IOVCoolDBPwd="COOLRED4PRO"
#
include("IOVDbSvc/IOVCondDB.py")

# include comcond catalog via DATAPATH
PoolSvc = Service( "PoolSvc" )
PoolSvc.ReadCatalog+= [ "prfile:poolcond/PoolCat_comcond.xml" ]

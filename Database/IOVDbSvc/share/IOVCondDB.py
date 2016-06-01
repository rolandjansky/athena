# IOVCondDB.py
# common setup for COOL conditions database for reconstruction
# included by specific IOV<app>.py (app=RecExCommon etc)
# 6/12/06:
# This method is now obsolete, should use automatic configuration via
# include of IOVDbSvc/CondDBSetup.py from AtlasGeoModel/GeoModelInit.py
#
include.block ( "IOVDbSvc/IOVCondDB.py" )
#
# setup access to the conditions database
include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
include( "IOVDbSvc/IOVDbSvc_jobOptions.py" )

# set the default connection (now to local SQLite file - will normally only
# be used for conditions writing)
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=mycool.db;X:"+IOVCoolDBProd

# connection with password:
#IOVDbSvc.dbConnection="impl=cool;techno=oracle;schema=ATLAS_COOL_INDET;ATLAS_COOLPROD:"+IOVCoolDBProd+":"+IOVCoolDBUser+":"+IOVCoolDBPwd
# old default to Lisbon CondDB
#IOVDbSvc.dbConnection = "impl=conddb;techno=mysql;atlasdbdev.cern.ch:conditions_dc2:rdrCnDC2sDB:"

# setup access strings to each COOL database schema on COOLPROD in CondDBCool
# these can be used in folder access strings as e.g.
# IOVDbSvc.Folders+= [ CondDBCool.INDET+"/INDET/BEAMPOS" ]

# helper function to define access strings for Oracle or SQlite replica
def IOVDbAcc(tech,dbname,user):
  if (tech=="oracle"):
    res="impl=cool;techno=oracle;schema=ATLAS_"+user+";ATLAS_COOLPROD:"+dbname+":ATLAS_COOL_READER"
  elif (tech=="sqlite"):
    res="impl=cool;techno=sqlite;schema=sqlite200/"+dbname+".db;X:"+dbname
  elif (tech=="logical"):
    res="impl=cool;techno=logical;schema="+user+";X:"+dbname
  else:
    res=""
  return "<dbConnection>"+res+"</dbConnection>"

class CondDBCool:
  INDET=IOVDbAcc(IOVCoolDBTech,IOVCoolDBProd,"COOL_INDET")
  PIXEL=IOVDbAcc(IOVCoolDBTech,IOVCoolDBProd,"COOL_PIXEL")
  SCT=IOVDbAcc(IOVCoolDBTech,IOVCoolDBProd,"COOL_SCT")
  TRT=IOVDbAcc(IOVCoolDBTech,IOVCoolDBProd,"COOL_TRT")
  LAR=IOVDbAcc(IOVCoolDBTech,IOVCoolDBProd,"COOL_LAR")
  TILE=IOVDbAcc(IOVCoolDBTech,IOVCoolDBProd,"COOL_TILE")
  MUON=IOVDbAcc(IOVCoolDBTech,IOVCoolDBProd,"COOL_MUON")
  MUONALIGN=IOVDbAcc(IOVCoolDBTech,IOVCoolDBProd,"COOL_MUONALIGN")
  MDT=IOVDbAcc(IOVCoolDBTech,IOVCoolDBProd,"COOL_MDT")
  TDAQ=IOVDbAcc(IOVCoolDBTech,IOVCoolDBProd,"COOL_TDAQ")
  DCS=IOVDbAcc(IOVCoolDBTech,IOVCoolDBProd,"COOL_DCS")
  GLOBAL=IOVDbAcc(IOVCoolDBTech,IOVCoolDBProd,"COOL_GLOBAL")
  LOCAL="<dbConnection>impl=cool;techno=sqlite;schema=mycool.db;X:"+IOVCoolDBProd+"</dbConnection> "

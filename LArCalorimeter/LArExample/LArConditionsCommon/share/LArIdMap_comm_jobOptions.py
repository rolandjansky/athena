include.block ( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

###################################################################
#
# Job options file for setting up Identifier Map in POOL/COOL
#
#==================================================================

from LArConditionsCommon.LArCondFlags import larCondFlags 

LArDBConnection = ""
LArDB = "LAR"

if larCondFlags.LArDBConnection.statusOn  :
  LArDBConnection = larCondFlags.LArDBConnection()
  LArDB = ""

from IOVDbSvc.CondDB import conddb

conddb.addFolder(LArDB,"/LAR/Identifier/OnOffIdMap"+LArDBConnection)
conddb.addFolder(LArDB,"/LAR/Identifier/CalibIdMap"+LArDBConnection)
conddb.addFolder(LArDB,"/LAR/Identifier/FebRodMap"+LArDBConnection)

    

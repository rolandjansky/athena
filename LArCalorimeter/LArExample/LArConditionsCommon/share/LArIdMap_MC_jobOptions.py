include.block ( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

###################################################################
#
# Job options file for setting up Identifier Map in POOL/COOL
#
#==================================================================

from LArConditionsCommon.LArCondFlags import larCondFlags 

larCondFlags.config_idmap_MC() 

LArDBConnection = ""
LArDB = "LAR_OFL"

if larCondFlags.LArDBConnection.statusOn  :
  LArDBConnection = larCondFlags.LArDBConnection()
  LArDB = ""

from IOVDbSvc.CondDB import conddb

conddb.addFolder(LArDB,"/LAR/Identifier/OnOffIdMap"+LArDBConnection )
larCondFlags.addTag("/LAR/Identifier/OnOffIdMap", conddb)

conddb.addFolder(LArDB,"/LAR/Identifier/CalibIdMap"+LArDBConnection )
larCondFlags.addTag("/LAR/Identifier/CalibIdMap", conddb)

conddb.addFolder(LArDB,"/LAR/Identifier/FebRodMap"+LArDBConnection )
larCondFlags.addTag("/LAR/Identifier/FebRodMap", conddb)


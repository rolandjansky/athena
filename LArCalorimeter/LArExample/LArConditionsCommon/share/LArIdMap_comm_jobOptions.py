include.block ( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

###################################################################
#
# Job options file for setting up Identifier Map in POOL/COOL
#
#==================================================================

from LArConditionsCommon.LArCondFlags import larCondFlags 
from LArRecUtils.LArRecUtilsConf import LArOnOffMappingAlg, LArFebRodMappingAlg, LArCalibLineMappingAlg
from AthenaCommon.AlgSequence import AthSequencer

LArDBConnection = ""
LArDB = "LAR"

if larCondFlags.LArDBConnection.statusOn  :
  LArDBConnection = larCondFlags.LArDBConnection()
  LArDB = ""

from IOVDbSvc.CondDB import conddb

condSeq = AthSequencer("AthCondSeq")

if not hasattr( condSeq, 'LArOnOffMappingAlg' ):
  conddb.addFolder(LArDB,"/LAR/Identifier/OnOffIdMap"+LArDBConnection,className="AthenaAttributeList")
  larCondFlags.addTag("/LAR/Identifier/OnOffIdMap", conddb)
  condSeq+=LArOnOffMappingAlg(ReadKey="/LAR/Identifier/OnOffIdMap")

if not hasattr( condSeq, "LArCalibLineMappingAlg"):
  conddb.addFolder(LArDB,"/LAR/Identifier/CalibIdMap"+LArDBConnection,className="AthenaAttributeList")
  larCondFlags.addTag("/LAR/Identifier/CalibIdMap", conddb)
  condSeq+=LArCalibLineMappingAlg(ReadKey="/LAR/Identifier/CalibIdMap")

if not hasattr( condSeq, "LArFebRodMappingAlg"):
  conddb.addFolder(LArDB,"/LAR/Identifier/FebRodMap"+LArDBConnection,className="AthenaAttributeList")
  larCondFlags.addTag("/LAR/Identifier/FebRodMap", conddb)
  condSeq+=LArFebRodMappingAlg(ReadKey="/LAR/Identifier/FebRodMap")

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

conddb.addFolder(LArDB,"/LAR/Identifier/OnOffIdMap"+LArDBConnection,className="AthenaAttributeList")
condSeq+=LArOnOffMappingAlg(ReadKey="/LAR/Identifier/OnOffIdMap")

conddb.addFolder(LArDB,"/LAR/Identifier/CalibIdMap"+LArDBConnection,className="AthenaAttributeList")
condSeq+=LArCalibLineMappingAlg(ReadKey="/LAR/Identifier/CalibIdMap")

conddb.addFolder(LArDB,"/LAR/Identifier/FebRodMap"+LArDBConnection,className="AthenaAttributeList")
condSeq+=LArFebRodMappingAlg(ReadKey="/LAR/Identifier/FebRodMap")
    

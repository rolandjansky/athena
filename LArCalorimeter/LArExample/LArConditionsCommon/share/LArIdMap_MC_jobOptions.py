include.block ( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

###################################################################
#
# Job options file for setting up Identifier Map in POOL/COOL
#
#==================================================================

from LArConditionsCommon.LArCondFlags import larCondFlags 
from AthenaCommon.AlgSequence import AthSequencer
from LArRecUtils.LArRecUtilsConf import LArOnOffMappingAlg, LArFebRodMappingAlg, LArCalibLineMappingAlg

larCondFlags.config_idmap_MC() 

isPileUpDigi = False
from PyUtils.moduleExists import moduleExists
if moduleExists ('Digitization'):
  from Digitization.DigitizationFlags import digitizationFlags
  isPileUpDigi = digitizationFlags.doXingByXingPileUp()

LArDBConnection = ""
LArDB = "LAR_OFL"

if larCondFlags.LArDBConnection.statusOn  :
  LArDBConnection = larCondFlags.LArDBConnection()
  LArDB = ""

from IOVDbSvc.CondDB import conddb
condSeq = AthSequencer("AthCondSeq")

if not hasattr(condSeq,"LArOnOffMappingAlg"):
  conddb.addFolder(LArDB,"/LAR/Identifier/OnOffIdMap"+LArDBConnection,className="AthenaAttributeList")
  larCondFlags.addTag("/LAR/Identifier/OnOffIdMap", conddb)
  condSeq+=LArOnOffMappingAlg(ReadKey="/LAR/Identifier/OnOffIdMap")

if not isPileUpDigi and not hasattr(condSeq,"LArCalibLineMappingAlg"):
  conddb.addFolder(LArDB,"/LAR/Identifier/CalibIdMap"+LArDBConnection,className="AthenaAttributeList")
  larCondFlags.addTag("/LAR/Identifier/CalibIdMap", conddb)
  condSeq+=LArCalibLineMappingAlg(ReadKey="/LAR/Identifier/CalibIdMap")

if not isPileUpDigi and not hasattr(condSeq,"LArFebRodMappingAlg"):
  conddb.addFolder(LArDB,"/LAR/Identifier/FebRodMap"+LArDBConnection,className="AthenaAttributeList")
  larCondFlags.addTag("/LAR/Identifier/FebRodMap", conddb)
  condSeq+=LArFebRodMappingAlg(ReadKey="/LAR/Identifier/FebRodMap")


include.block("LArRecUtils/LArOnOffMappingAlg.py")
from AthenaCommon.AlgSequence import AthSequencer
from IOVDbSvc.CondDB import conddb
conddb.addFolder("LAR_OFL","/LAR/Identifier/OnOffIdMap",className="AthenaAttributeList")
condSequence = AthSequencer("AthCondSeq")  
from LArRecUtils.LArRecUtilsConf import LArOnOffMappingAlg

condSequence+=LArOnOffMappingAlg(ReadKey="/LAR/Identifier/OnOffIdMap",OutputLevel=DEBUG)



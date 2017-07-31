include.block("LArRecUtils/LArOnOffMappingAlg.py")

from IOVDbSvc.CondDB import conddb
conddb.addFolder("LAR","/LAR/Identifier/OnOffIdMap",className="AthenaAttributeList")
                            
condSequence = AthSequencer("AthCondSeq")  
from LArRecUtils.LArRecUtilsConf import LArOnOffMappingAlg

condSequence+=LArOnOffMappingAlg(ReadKey="/LAR/Identifier/OnOffIdMap",OutputLevel=DEBUG)



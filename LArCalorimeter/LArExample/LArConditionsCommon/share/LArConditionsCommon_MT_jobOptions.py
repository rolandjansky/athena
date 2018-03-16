include.block ( "LArConditionsCommon/LArConditionsCommon_MT_jobOptions.py" )

from LArConditionsCommon.LArCondFlags import larCondFlags 
from IOVDbSvc.CondDB import conddb

if not larCondFlags.LoadElecCalib.is_locked():
    larCondFlags.LoadElecCalib.set_Value(rec.readRDO()) 


#Import conditions algos
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LAruA2MeVFlat_ as LAruA2MeVCondAlg 
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArDAC2uAFlat_ as LArDAC2uACondAlg 
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArRampFlat_ as LArRampCondAlg 
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArPedestalFlat_ as LArPedestalCondAlg 
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArHVScaleCorrFlat_ as LArHVScaleCorrCondAlg 
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArMphysOverMcalFlat_ as LArMphysOverMcalCondAlg 

from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArOFCFlat_ as LArOFCCondAlg 
from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArShapeFlat_ as LArShapeCondAlg 


from AthenaCommon.AlgSequence import AthSequencer
condSequence = AthSequencer("AthCondSeq")
for obj,calg in (("Pedestal",LArPedestalCondAlg),("uA2MeV",LAruA2MeVCondAlg),("DAC2uA",LArDAC2uACondAlg),("Ramp",LArRampCondAlg),("MphysOverMcal", LArMphysOverMcalCondAlg),("HVScaleCorr",LArHVScaleCorrCondAlg),("OFC",LArOFCCondAlg),("Shape",LArShapeCondAlg)):
    conddb.addFolder("LAR_ONL","/LAR/ElecCalibFlat/"+obj,className="CondAttrListCollection")
    condSequence+=calg(ReadKey="/LAR/ElecCalibFlat/"+obj,WriteKey="LAr"+obj)


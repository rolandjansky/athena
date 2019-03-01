from eflowRec.eflowRecFlags import jobproperties

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#Jet/MET algorithms - EM input and LC output

#Same as for Run2 so can use EM version
from eflowRec.eflowPreparationGetter_HLLHC import eflowPreparationGetter_HLLHC
CaloObjectBuilderGetter = eflowPreparationGetter_HLLHC()

#Need HLLHC specific items, so use HLLHC version
from eflowRec.eflowCaloObjectBuilderGetter_HLLHC import eflowCaloObjectBuilderGetter_HLLHC
ObjectBuilderToolsGetter = eflowCaloObjectBuilderGetter_HLLHC()

#Need HLLHC specific items, so use HLLHC version
from eflowRec.eflowObjectBuilderGetter_HLLHC import eflowObjectBuilderGetter_HLLHC
ObjectBuilderGetter = eflowObjectBuilderGetter_HLLHC()

if not jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
    from eflowRec.eflowRecConf import eflowVertexInformationSetter
    eflowVertexInformationSetter = eflowVertexInformationSetter("eflowVertextInformationSetter_EM")
    topSequence += eflowVertexInformationSetter

    from eflowRec.eflowRecConf import eflowOverlapRemoval
    eflowOverlapRemoval = eflowOverlapRemoval("eflowOverlapRemoval_EM")
    topSequence += eflowOverlapRemoval

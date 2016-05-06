from eflowRec.eflowRecFlags import jobproperties

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#Jet/MET algorithms - EM input and LC output

from eflowRec.eflowPreparationGetter import eflowPreparationGetter
CaloObjectBuilderGetter = eflowPreparationGetter()

from eflowRec.eflowCaloObjectBuilderGetter import eflowCaloObjectBuilderGetter
ObjectBuilderToolsGetter = eflowCaloObjectBuilderGetter()

from eflowRec.eflowObjectBuilderGetter import eflowObjectBuilderGetter
ObjectBuilderGetter = eflowObjectBuilderGetter()

if not jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
    from eflowRec.eflowRecConf import eflowVertexInformationSetter
    eflowVertexInformationSetter = eflowVertexInformationSetter("eflowVertextInformationSetter_EM")
    topSequence += eflowVertexInformationSetter

    from eflowRec.eflowRecConf import eflowOverlapRemoval
    eflowOverlapRemoval = eflowOverlapRemoval("eflowOverlapRemoval_EM")
    topSequence += eflowOverlapRemoval

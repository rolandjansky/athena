from eflowRec.eflowRecFlags import jobproperties

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#Jet/MET algorithms - EM input and LC output

from eflowRec.eflowCaloObjectBuilderGetter import eflowCaloObjectBuilderGetter
CaloObjectBuilderGetter = eflowCaloObjectBuilderGetter()

from eflowRec.eflowObjectBuilderToolsGetter import eflowObjectBuilderToolsGetter
ObjectBuilderToolsGetter = eflowObjectBuilderToolsGetter()

from eflowRec.eflowRecConf import eflowVertexInformationSetter
eflowVertexInformationSetter = eflowVertexInformationSetter("eflowVertextInformationSetter_EM")
topSequence += eflowVertexInformationSetter

from eflowRec.eflowRecConf import eflowOverlapRemoval
eflowOverlapRemoval = eflowOverlapRemoval("eflowOverlapRemoval_EM")
topSequence += eflowOverlapRemoval

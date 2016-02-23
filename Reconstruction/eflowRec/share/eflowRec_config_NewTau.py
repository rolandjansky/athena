from eflowRec.eflowRecFlags import jobproperties

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from eflowRec.eflowTauProcessingToolsGetter import eflowTauProcessingToolsGetter
TauProcessingToolsGetter = eflowTauProcessingToolsGetter()

from eflowRec.eflowRecConf import eflowVertexInformationSetter
eflowVertexInformationSetter = eflowVertexInformationSetter("eflowTauVertexInformationSetter")

from eflowRec.eflowRecConf import eflowPi0Tagger
eflowPi0Tagger = eflowPi0Tagger()
topSequence += eflowPi0Tagger

eflowVertexInformationSetter.eflowObjects_inputName="eflowObjects_tauMode"
topSequence += eflowVertexInformationSetter 

from eflowRec.eflowRecConf import eflowPFOCreator
eflowPFOCreator = eflowPFOCreator("eflowTauPFOCreator")
topSequence += eflowPFOCreator

from eflowRec.eflowRecFlags import jobproperties

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

if jobproperties.eflowRecFlags.runTauMode == True:
    from eflowRec.eflowRecConf import eflowTauSeeder
    eflowTauSeeder = eflowTauSeeder()
    topSequence += eflowTauSeeder

from eflowRec.eflowPreparationGetter import eflowPreparationGetter
CaloObjectBuilderGetter = eflowPreparationGetter()

from eflowRec.eflowCaloObjectBuilderGetter import eflowCaloObjectBuilderGetter
ObjectBuilderToolsGetter = eflowCaloObjectBuilderGetter()

from eflowRec.eflowObjectBuilderGetter import eflowObjectBuilderGetter
ObjectBuilderGetter = eflowObjectBuilderGetter()

if jobproperties.eflowRecFlags.runTauMode == False:
    from eflowRec.eflowRecConf import eflowQuantities
    Quantities = eflowQuantities()

    elif jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
        Quantities.EflowObjectsName="eflowObjects01"

    topSequence+=Quantities

from eflowRec.eflowRecConf import eflowVertexInformationSetter
eflowVertexInformationSetter = eflowVertexInformationSetter()

if jobproperties.eflowRecFlags.runTauMode == True:
    from eflowRec.eflowRecConf import eflowPi0Tagger
    eflowPi0Tagger = eflowPi0Tagger()
    topSequence += eflowPi0Tagger
    
    eflowVertexInformationSetter.eflowObjects_inputName="eflowObjects_tauMode"

elif jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
    eflowVertexInformationSetter.eflowObjects_inputName="eflowObjects01"
else:
    eflowVertexInformationSetter.eflowObjects_inputName="eflowObjects_FullMode"

topSequence += eflowVertexInformationSetter

from eflowRec.eflowRecConf import eflowOverlapRemoval
eflowOverlapRemoval = eflowOverlapRemoval()

topSequence += eflowOverlapRemoval

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging  # loads logger 
import traceback # to allow printout of trace back
from RecExConfig.Configured import Configured # import base class 

def setup_eflowObjectBulderTools(Configured, nameModifier,mlog):

    if nameModifier != "EM" and nameModifier != "LC":
        mlog.error("Invalid calorimeter scale was specified : should be LC or EM, but was "+nameModifier)
        return False

    try:
        from eflowRec.eflowRecConf import eflowObjectBuilder_Tools
        ObjectBuilder_Tools=eflowObjectBuilder_Tools("eflowObjectBuilder_Tools_"+nameModifier)
    except:
        mlog.error("could not import eflowRec.eflowObjectBuilderTools")
        print traceback.format_exc()
        return False

    Configured._eflowObjectBuilderToolsHandle = ObjectBuilder_Tools

    from eflowRec.eflowRecFlags import jobproperties

    ObjectBuilder_Tools.PrivateToolList  = []

    if jobproperties.eflowRecFlags.eflowAlgType == "CellLevel" or jobproperties.eflowRecFlags.eflowAlgType == "EOverP" :
        try:
            from eflowRec.eflowCellLevelSubtractionToolDefault import eflowCellLevelSubtractionToolDefault
            CellLevelSubtractionTool = eflowCellLevelSubtractionToolDefault("eflowCellLevelSubtractionTool_"+nameModifier)
        except:
            mlog.error("could not import eflowRec.eflowCellLevelSubtractionTool")
            print traceback.format_exc()
            return False
        
        # Toggle e/p mode
        if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
            CellLevelSubtractionTool.CalcEOverP = True
            
        ObjectBuilder_Tools.PrivateToolList += [CellLevelSubtractionTool]
        
    if jobproperties.eflowRecFlags.useSplitShowers == True and jobproperties.eflowRecFlags.eflowAlgType != "EOverP":
        if nameModifier == "EM":  
            try:
                from eflowRec.eflowRecoverSplitShowersToolDefault import eflowRecoverSplitShowersToolDefault
                RecoverSplitShowersToolDefault=eflowRecoverSplitShowersToolDefault("eflowRecoverSplitShowers_"+nameModifier)
            except:
                mlog.error("could not import eflowRec.eflowRecoverSplitShowersToolDefault")
                print traceback.format_exc()
                return False
            ObjectBuilder_Tools.PrivateToolList += [RecoverSplitShowersToolDefault]

        elif nameModifier == "LC":
            try:
                from eflowRec.eflowRecoverSplitShowersToolDefault_LC import eflowRecoverSplitShowersToolDefault_LC
                RecoverSplitShowersToolDefault_LC=eflowRecoverSplitShowersToolDefault_LC("eflowRecoverSplitShowers_"+nameModifier)
            except:
                mlog.error("could not import eflowRec.eflowRecoverSplitShowersToolDefault")
                print traceback.format_exc()
                return False
            ObjectBuilder_Tools.PrivateToolList += [RecoverSplitShowersToolDefault_LC]

    try:
        from eflowRec.eflowMomentCalculatorToolDefault import eflowMomentCalculatorToolDefault
        MomentCalculatorTool = eflowMomentCalculatorToolDefault("eflowMomentCalculatorTool_"+nameModifier)
    except:
        mlog.error("could not import eflowRec.eflowMomentCalculatorTool")
        print traceback.format_exc()
        return False

    if nameModifier == "LC":
        MomentCalculatorTool.LCMode = True

    ObjectBuilder_Tools.PrivateToolList += [MomentCalculatorTool]

    if nameModifier == "EM":        
        try:
            from eflowRec.eflowLCCalibToolDefault import eflowLCCalibToolDefault
            LCCalibTool = eflowLCCalibToolDefault("eflowLCCalibTool_"+nameModifier)
        except:
            mlog.error("could not import eflowRec.eflowLCCalibTool")
            print traceback.format_exc()
            return False

        ObjectBuilder_Tools.PrivateToolList += [LCCalibTool]
        
    try:
        from eflowRec.eflowObjectCreatorToolDefault import eflowObjectCreatorToolDefault
        ObjectCreatorTool=eflowObjectCreatorToolDefault("eflowObjectCreatorTool_"+nameModifier)
    except:
        mlog.error("could not import eflowRec.eflowObjectCreatorTool")
        print traceback.format_exc()
        return False

    if "LC" == nameModifier:
        ObjectCreatorTool.PFOOutputName="JetETMiss_LCPFO"
        ObjectCreatorTool.LCMode=True
        ObjectBuilder_Tools.EflowClustersOutputName="PFOClusters_JetETMiss_LC"
        ObjectBuilder_Tools.EflowCaloObjectsName="eflowCaloObjects_LC"

    ObjectBuilder_Tools.PrivateToolList += [ObjectCreatorTool]

    if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
        ObjectCreatorTool.PFOOutputName = 'EOverP'
        ObjectBuilder_Tools.EflowClustersOutputName="eflowCluster01"
        _output = { _outputType : "eflowObjects01" }
        
    from RecExConfig.ObjKeyStore import objKeyStore
    objKeyStore.addTransient(Configured.outputType(),Configured.outputKey())
        
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += ObjectBuilder_Tools

    return True

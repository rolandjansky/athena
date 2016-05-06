# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging  # loads logger 
import traceback # to allow printout of trace back
from RecExConfig.Configured import Configured # import base class 

def setup_eflowObjectBuilderTools(Configured, nameModifier,mlog):

    if nameModifier != "EM" and nameModifier != "LC":
        mlog.error("Invalid calorimeter scale was specified : should be LC or EM, but was "+nameModifier)
        return False

    try:
        from eflowRec.eflowRecConf import eflowCaloObjectBuilder
        ObjectBuilder_Tools=eflowCaloObjectBuilder("eflowCaloObjectBuilder_"+nameModifier)
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

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += ObjectBuilder_Tools

    return True

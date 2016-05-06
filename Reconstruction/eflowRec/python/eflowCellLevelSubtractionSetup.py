# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.Configured import Configured # import base class 

def setup_eflowCellLevelSubtraction(Configured, nameModifier, mlog):

    Configured.SubtractionSigmaCut = 1.5
    Configured.ConsistencySigmaCut = 1.0

    if "EM" == nameModifier:

        try:
            from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_JetETMiss
            CellEOverPTool=eflowCellEOverPTool_mc12_JetETMiss()
        except:
            mlog.error("could not import eflowRec.eflowCellEOverPTool_mc12")
            print traceback.format_exc()
            return False
        
        Configured.eflowCellEOverPTool=CellEOverPTool

    elif "LC" == nameModifier:
        try:
            from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_LC
            CellEOverPTool=eflowCellEOverPTool_mc12_LC()
        except:
            mlog.error("could not import eflowRec.eflowCellEOverPTool_mc12_LC")
            print traceback.format_exc()
            return False

        Configured.eflowCellEOverPTool=CellEOverPTool

    from eflowRec.eflowRecFlags import jobproperties
    if jobproperties.eflowRecFlags.useUpdated2015ChargedShowerSubtraction == False:
        Configured.useUpdated2015ChargedShowerSubtraction = False

    return True

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 
import traceback # to allow printout of trace back
from RecExConfig.Configured import Configured # import base class 

def setup_eflowMomentCalculatorTool(Configured, nameModifier,mlog):
    
    if nameModifier != "EM" and nameModifier != "LC":
        mlog.error("Invalid calorimeter scale was specified : should be LC or EM, but was "+nameModifier)
        return False

    try:
        from eflowRec.eflowRecConf import eflowClusterCollectionTool
        eflowClusterCollectionTool = eflowClusterCollectionTool("eflowClusterCollectionTool_MomentCalculator_"+nameModifier)
    except:
        mlog.error("could not import eflowRec. eflowClusterCollectionTool")
        print traceback.format_exc()
        return False
    
    Configured.eflowRecClusterCollectionTool = eflowClusterCollectionTool

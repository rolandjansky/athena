# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger                           
import traceback # to allow printout of trace back
from RecExConfig.Configured import Configured # import base class 

def setup_eflowCaloObjectBulder(Configured, nameModifier,mlog):

    if nameModifier != "EM" and nameModifier != "LC":
        mlog.error("Invalid calorimeter scale was specified : should be LC or EM, but was "+nameModifier)
        return False

    try:
        from eflowRec.eflowRecConf import eflowCaloObjectBuilder
        eflowCaloObjectBuilderAlgorithm=eflowCaloObjectBuilder("eflow"+nameModifier+"CaloObjectBuilder")
    except:
        mlog.error("could not import eflowRec.eflowCaloObjectBuilder")
        print traceback.format_exc()
        return False

    Configured._eflowCaloObjectBuilderHandle = eflowCaloObjectBuilderAlgorithm

    if "EM" == nameModifier:

        try:
            from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_JetETMiss
            CellEOverPTool=eflowCellEOverPTool_mc12_JetETMiss()
        except:
            mlog.error("could not import eflowRec.eflowCellEOverPTool_mc12")
            print traceback.format_exc()
            return False
        
        eflowCaloObjectBuilderAlgorithm.eflowCellEOverPTool=CellEOverPTool

    elif "LC" == nameModifier:
        try:
            from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_LC
            CellEOverPTool=eflowCellEOverPTool_mc12_LC()
        except:
            mlog.error("could not import eflowRec.eflowCellEOverPTool_mc12_LC")
            print traceback.format_exc()
            return False
        
        eflowCaloObjectBuilderAlgorithm.eflowCellEOverPTool=CellEOverPTool


    try:
            from eflowRec.eflowTrackToCaloTrackExtrapolatorTool import eflowTrackToCaloTrackExtrapolatorToolDefault
            TrackToCaloTrackExtrapolatorToolDefault=eflowTrackToCaloTrackExtrapolatorToolDefault()
    except:
            mlog.error("could not import eflowRec.eflowTrackToCaloTrackExtrapolatorToolDefault")
            print traceback.format_exc()
            return False

    eflowCaloObjectBuilderAlgorithm.eflowTrackToCaloTrackExtrapolatorTool=TrackToCaloTrackExtrapolatorToolDefault

    # sets output key of C++ algorithm equal to the python side 
    eflowCaloObjectBuilderAlgorithm.EflowCaloObjectsOutputName=Configured.outputKey()

    from eflowRec.eflowRecFlags import jobproperties
    
    if "EM" == nameModifier:
        eflowCaloObjectBuilderAlgorithm.ClustersName = "CaloTopoCluster"
    elif "LC" == nameModifier:
        eflowCaloObjectBuilderAlgorithm.ClustersName = "CaloCalTopoCluster"

    if True == jobproperties.eflowRecFlags.useLeptons:
        eflowCaloObjectBuilderAlgorithm.useLeptons = True
        if True == jobproperties.eflowRecFlags.storeLeptonCells:
            eflowCaloObjectBuilderAlgorithm.storeLeptonCells = True
            if "LC" == nameModifier:
                eflowCaloObjectBuilderAlgorithm.eflowLeptonCellsName="eflowRec_leptonCellContainer_LC"
    else:
        eflowCaloObjectBuilderAlgorithm.useLeptons = False

    if "LC" == nameModifier:
        eflowCaloObjectBuilderAlgorithm.EflowCaloObjectsOutputName="eflowCaloObjects_LC"
        eflowCaloObjectBuilderAlgorithm.eflowElectronsName="eflowRec_selectedElectrons_LC"
         
    from RecExConfig.ObjKeyStore import objKeyStore
    objKeyStore.addTransient(Configured.outputType(),Configured.outputKey())

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += eflowCaloObjectBuilderAlgorithm ;

    return True

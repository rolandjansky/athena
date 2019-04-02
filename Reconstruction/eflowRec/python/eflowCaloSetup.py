# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger                           
import traceback # to allow printout of trace back
from RecExConfig.Configured import Configured # import base class 

def setup_eflowCaloObjectCreator(Configured, nameModifier,mlog):

    if nameModifier != "EM" and nameModifier != "EM_HLLHC" and nameModifier != "LC":
        mlog.error("Invalid calorimeter scale was specified : should be LC or EM or EM_HLLHC, but was "+nameModifier)
        return False

    try:
        from eflowRec.eflowRecConf import eflowPreparation
        eflowPreparationAlgorithm=eflowPreparation("eflow"+nameModifier+"CaloObjectBuilder")
    except:
        mlog.error("could not import eflowRec.eflowPreparation")
        print traceback.format_exc()
        return False

    from eflowRec.eflowRecFlags import jobproperties;
    if True == jobproperties.eflowRecFlags.fixMuonLogic:
        eflowPreparation.FixMuonLogic=True
    
    Configured._eflowPreparationHandle = eflowPreparationAlgorithm

    try:
        from eflowRec.eflowRecConf import eflowTrackCaloExtensionTool
        TrackCaloExtensionTool=eflowTrackCaloExtensionTool()
    except:
        mlog.error("could not import eflowRec.eflowTrackCaloExtensionTool")
        print traceback.format_exc()
        return False

    eflowPreparationAlgorithm.TrackExtrapolatorTool = TrackCaloExtensionTool

    # sets output key of C++ algorithm equal to the python side 
    eflowPreparationAlgorithm.EflowCaloObjectsOutputName=Configured.outputKey()

    from eflowRec.eflowRecFlags import jobproperties
    
    if "EM" == nameModifier or "EM_HLLHC" == nameModifier:
        eflowPreparationAlgorithm.ClustersName = "CaloTopoCluster"
        eflowPreparationAlgorithm.CalClustersName = "CaloCalTopoClusters"
    elif "LC" == nameModifier:
        eflowPreparationAlgorithm.ClustersName = "CaloCalTopoClusters"
        eflowPreparationAlgorithm.CalClustersName = ""

    if True == jobproperties.eflowRecFlags.useLeptons:
        eflowPreparationAlgorithm.useLeptons = True
        if True == jobproperties.eflowRecFlags.storeLeptonCells:
            eflowPreparationAlgorithm.storeLeptonCells = True
            if "LC" == nameModifier:
                eflowPreparationAlgorithm.eflowLeptonCellsName="eflowRec_leptonCellContainer_LC"
    else:
        eflowPreparationAlgorithm.useLeptons = False

    if "LC" == nameModifier:
        eflowPreparationAlgorithm.EflowCaloObjectsOutputName="eflowCaloObjects_LC"
        eflowPreparationAlgorithm.eflowElectronsName="eflowRec_selectedElectrons_LC"

    try:
        from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
        TrackSelectionTool = InDet__InDetTrackSelectionTool()
    except:
        mlog.error("could not import InDetTrackSelectionTool.InDet__InDetTrackSelectionTool")
        print traceback.format_exc()
        return False

    try:
             from AthenaCommon.AppMgr import ToolSvc
    except:
             mlog.error("could not import ToolSvc")
             print traceback.format_exc()
             return False

    ToolSvc += TrackSelectionTool

    TrackSelectionTool.CutLevel = "TightPrimary"
    TrackSelectionTool.minPt = 500.0
    if "EM_HLLHC" == nameModifier:
        TrackSelectionTool.maxAbsEta=4.0

    eflowPreparationAlgorithm.TrackSelectionTool = TrackSelectionTool

    from RecExConfig.ObjKeyStore import objKeyStore
    objKeyStore.addTransient(Configured.outputType(),Configured.outputKey())

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += eflowPreparationAlgorithm ;

    return True

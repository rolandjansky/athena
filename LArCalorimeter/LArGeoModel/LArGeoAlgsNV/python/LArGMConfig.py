# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LArGMCfg(configFlags):
    
    result=GeoModelCfg(configFlags)

    doAlignment=configFlags.LAr.doAlign
    activateCondAlgs = not configFlags.GeoModel.Align.LegacyConditionsAccess

    tool = CompFactory.LArDetectorToolNV(ApplyAlignments=doAlignment, EnableMBTS=configFlags.Detector.GeometryMBTS)
    if configFlags.Common.ProductionStep != ProductionStep.Simulation and configFlags.Common.ProductionStep != ProductionStep.FastChain:
        tool.GeometryConfig = "RECO"

    result.getPrimary().DetectorTools += [ tool ]

    if doAlignment:
        if configFlags.Input.isMC:
            #Monte Carlo case:
            result.merge(addFolders(configFlags,"/LAR/Align","LAR_OFL",className="DetCondKeyTrans"))
            result.merge(addFolders(configFlags,"/LAR/LArCellPositionShift","LAR_OFL",className="CaloRec::CaloCellPositionShift"))
        else:
            if configFlags.Overlay.DataOverlay:
                #Data overlay
                result.merge(addFolders(configFlags, "/LAR/Align", "LAR_ONL",className="DetCondKeyTrans"))
                result.merge(addFolders(configFlags, "/LAR/LArCellPositionShift", "LAR_OFL", tag="LArCellPositionShift-ideal", db="OFLP200",className="CaloRec::CaloCellPositionShift"))
            else:
                #Regular offline data processing
                result.merge(addFolders(configFlags,"/LAR/Align","LAR_ONL",className="DetCondKeyTrans"))
                result.merge(addFolders(configFlags,"/LAR/LArCellPositionShift","LAR_ONL",className="CaloRec::CaloCellPositionShift"))

        if activateCondAlgs:
            result.addCondAlgo(CompFactory.LArAlignCondAlg())
            result.addCondAlgo(CompFactory.CaloAlignCondAlg())
            AthReadAlg_ExtraInputs = []
            caloCellsInInput = configFlags.Input.Files != ["_ATHENA_GENERIC_INPUTFILE_NAME_"] and "CaloCellContainer" in [i.split('#')[0] for i in configFlags.Input.TypedCollections]
            sCellsInInput = False
            caloCellKeys = []
            if caloCellsInInput:
                from SGComps.AddressRemappingConfig import AddressRemappingCfg
                result.merge(AddressRemappingCfg())

                caloCellKeys = [i.split('#')[1] for i in configFlags.Input.TypedCollections if "CaloCellContainer"==i.split('#')[0] ]
                for key in caloCellKeys:
                    if key != 'AllCalo':
                        sCellsInInput = True

            AthReadAlg_ExtraInputs.append(('CaloDetDescrManager', 'ConditionStore+CaloDetDescrManager'))            
            if configFlags.GeoModel.Run == 'RUN3' and configFlags.Detector.GeometryTile or sCellsInInput:
                result.addCondAlgo(CompFactory.CaloSuperCellAlignCondAlg())
                AthReadAlg_ExtraInputs.append(('CaloSuperCellDetDescrManager', 'ConditionStore+CaloSuperCellDetDescrManager'))


            if caloCellsInInput:
                for key in caloCellKeys:
                    AthReadAlg=CompFactory.AthReadAlg
                    AthReadAlg_CaloCellCont = AthReadAlg (f'AthReadAlg_{key}',
                                                          Key = f'CaloCellContainer/{key}',
                                                          Aliases = [],
                                                          ExtraInputs = AthReadAlg_ExtraInputs)
                    result.addCondAlgo(AthReadAlg_CaloCellCont)
    else:
        # Build unalinged CaloDetDescrManager instance in the Condition Store
        if activateCondAlgs:
            result.addCondAlgo(CompFactory.CaloAlignCondAlg(LArAlignmentStore="",CaloCellPositionShiftFolder=""))
            if configFlags.GeoModel.Run == 'RUN3' and configFlags.Detector.GeometryTile:
                result.addCondAlgo(CompFactory.CaloSuperCellAlignCondAlg())
            
    return result

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = LArGMCfg(ConfigFlags)
    f=open('LArGMCfg.pkl','wb')
    acc.store(f)
    f.close()

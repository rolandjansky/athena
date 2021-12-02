# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from OutputStreamAthenaPool.OutputStreamConfig import addToAOD, addToESD

def HIGlobalRecCfg(flags):
    """Configures Heavy IOn Global quantities """
    acc = ComponentAccumulator()

    # load dependencies
    from CaloRec.CaloRecoConfig import CaloRecoCfg  
    acc.merge(CaloRecoCfg(flags))
    from CaloRec.CaloTowerMakerConfig import CaloTowerMakerCfg
    towerMaker = acc.getPrimaryAndMerge(CaloTowerMakerCfg(flags))
    input = towerMaker.TowerContainerName


    eventShapeTool = CompFactory.HIEventShapeFillerTool( 
            EventShapeMapTool = CompFactory.HIEventShapeMapTool()
    )
    shapeKey = "HIEventShape"
    eventShapeMakerAlg = CompFactory.HIEventShapeMaker(
                    InputTowerKey = input,
                    NaviTowerKey = input,
                    OutputContainerKey = shapeKey,
                    HIEventShapeFillerTool = eventShapeTool,
                    OrderOfFlowHarmonics = 7)
    output = [ f"xAOD::HIEventShapeContainer#{shapeKey}", f"xAOD::HIEventShapeAuxContainer#{shapeKey}Aux."]

    # TODO configure MBTS &FWD conversion once available


    if flags.HeavyIon.Global.doEventShapeSummary:
        summaryKey = "CaloSums"
        summaryTool = CompFactory.HIEventShapeSummaryTool(  SubCalos =  ['FCal','EMCal','HCal','ALL'],
                                                            Samplings = ['FCAL0','FCAL1','FCAL2'],
                                                            DoPositiveNegativeSides= False )

        eventShapeMakerAlg.SummaryTool = summaryTool
        eventShapeMakerAlg.SummaryContainerKey = summaryKey

        output.extend([ f"xAOD::HIEventShapeContainer#{summaryKey}", f"xAOD::HIEventShapeAuxContainer#{summaryKey}Aux."])

    acc.merge(addToESD(flags, output))
    acc.merge(addToAOD(flags, output))
                    
    acc.addEventAlgo( eventShapeMakerAlg )


    return acc
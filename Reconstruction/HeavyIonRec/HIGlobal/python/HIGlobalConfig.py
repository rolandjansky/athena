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
                    HIEventShapeFillerTool = eventShapeTool)
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

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    flags.Input.Files = [defaultTestFiles.d + "/RecJobTransformTests/data18_hi.00367384.physics_HardProbes.daq.RAW._lb0145._SFO-8._0001.data"]
    flags.Exec.MaxEvents=5
    flags.Concurrency.NumThreads=1

    flags.fillFromArgs() # enable unit tests to switch only parts of reco: python -m HIRecConfig.HIRecConfig HeavyIon.doGlobal = 0 and so on
    flags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(flags)
    from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
    readBSAcc = ByteStreamReadCfg(flags)
    acc.merge(HIGlobalRecCfg(flags))
    from AthenaCommon.Constants import DEBUG
    acc.getEventAlgo("HIEventShapeMaker").OutputLevel=DEBUG
    status = acc.run()
    if status.isFailure():
        import sys
        sys.exit(-1)
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def LArMonitoringConfig(ConfigFlags,CONFIG,STREAM):
    
    acc=ComponentAccumulator()
    
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(ConfigFlags))

    LArRawDataReadingAlg=CompFactory.LArRawDataReadingAlg
    acc.addEventAlgo(LArRawDataReadingAlg())

    if "LArMon" in CONFIG:

        from LArROD.LArFebErrorSummaryMakerConfig import LArFebErrorSummaryMakerCfg
        acc.merge(LArFebErrorSummaryMakerCfg(ConfigFlags))

        from LArMonitoring.LArFEBMonAlg import LArFEBMonConfig
        acc.merge(LArFEBMonConfig(ConfigFlags))

        if STREAM == 'NONE': # coverage only in NoTrigSel
           from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
           acc.merge(CaloNoiseCondAlgCfg(ConfigFlags,"electronicNoise"))
           from LArBadChannelTool.LArBadChannelConfig import LArBadChannelCfg
           acc.merge(LArBadChannelCfg(ConfigFlags))
           from LArMonitoring.LArCoverageAlg import LArCoverageConfig
           acc.merge(LArCoverageConfig(ConfigFlags))


        # adding NoisyRO monitoring, needs CaloCells
        from LArROD.LArRawChannelBuilderAlgConfig import LArRawChannelBuilderAlgCfg
        acc.merge(LArRawChannelBuilderAlgCfg(ConfigFlags))
        from LArCellRec.LArCellBuilderConfig import LArCellBuilderCfg,LArCellCorrectorCfg
        larCellBuilder     = acc.popToolsAndMerge(LArCellBuilderCfg(ConfigFlags))
        larCellCorrectors  = acc.popToolsAndMerge(LArCellCorrectorCfg(ConfigFlags))
        cellFinalizer  = CompFactory.CaloCellContainerFinalizerTool()
        cellMakerTools=[larCellBuilder,cellFinalizer]+larCellCorrectors
        cellAlgo=CompFactory.CaloCellMaker(CaloCellMakerToolNames=cellMakerTools,
                                     CaloCellsOutputName="AllCalo")

        acc.addEventAlgo(cellAlgo,primary=True)

        from LArCellRec.LArNoisyROSummaryConfig import LArNoisyROSummaryCfg
        acc.merge(LArNoisyROSummaryCfg(ConfigFlags))

        from LArMonitoring.LArNoisyROMonAlg import LArNoisyROMonConfig 
        acc.merge(LArNoisyROMonConfig(ConfigFlags))

        from LArMonitoring.LArDigitMonAlg import LArDigitMonConfig 
        acc.merge(LArDigitMonConfig(ConfigFlags))

        from LArMonitoring.LArRawChannelMonAlg import LArRawChannelMonConfig 
        acc.merge(LArRawChannelMonConfig(ConfigFlags))

        if 'Main' == STREAM or 'express' == STREAM or 'L1Topo' == STREAM or 'HardProbes' == STREAM or 'MinBias' == STREAM or 'NONE' == STREAM:

           # include collision time, 
           # and cluster collision time, but need to run the clustering
           from LArMonitoring.LArCollisionTimeMonAlg import LArCollisionTimeMonConfig
           acc.merge(LArCollisionTimeMonConfig(ConfigFlags))

        if 'ZeroBias' == STREAM or 'CosmicCalo' == STREAM:

           #include noise correlation 
           from LArMonitoring.LArNoiseCorrelationMonAlg import LArNoiseCorrelationMonConfig
           acc.merge(LArNoiseCorrelationMonConfig(ConfigFlags))

        if 'ZeroBias' == STREAM or 'CosmicCalo' == STREAM  or 'NONE' == STREAM:
           #cosmics
           from LArMonitoring.LArCosmicsMonAlg import LArCosmicsMonConfig
           acc.merge(LArCosmicsMonConfig(ConfigFlags))

        return acc

    if "DSPMon" in CONFIG:

        from LArROD.LArRawChannelBuilderAlgConfig import LArRawChannelBuilderAlgCfg
        acc.merge(LArRawChannelBuilderAlgCfg(ConfigFlags))

        from LArBadChannelTool.LArBadChannelConfig import LArBadChannelCfg
        acc.merge(LArBadChannelCfg(ConfigFlags))

        from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
        acc.merge(CaloNoiseCondAlgCfg(ConfigFlags,"totalNoise"))

        from LArMonitoring.LArFEBMonAlg import LArFEBMonConfig
        acc.merge(LArFEBMonConfig(ConfigFlags))

        from LArMonitoring.LArRODMonAlg import LArRODMonConfig
        acc.merge(LArRODMonConfig(ConfigFlags))

        return acc

    if "CaloMon" in CONFIG:

        from LArROD.LArRawChannelBuilderAlgConfig import LArRawChannelBuilderAlgCfg
        acc.merge(LArRawChannelBuilderAlgCfg(ConfigFlags))
        from LArCellRec.LArCellBuilderConfig import LArCellBuilderCfg,LArCellCorrectorCfg
        larCellBuilder     = acc.popToolsAndMerge(LArCellBuilderCfg(ConfigFlags))
        larCellCorrectors  = acc.popToolsAndMerge(LArCellCorrectorCfg(ConfigFlags))
        cellFinalizer  = CompFactory.CaloCellContainerFinalizerTool()
        cellMakerTools=[larCellBuilder,cellFinalizer]+larCellCorrectors
        cellAlgo=CompFactory.CaloCellMaker(CaloCellMakerToolNames=cellMakerTools,
                                     CaloCellsOutputName="AllCalo")

        acc.addEventAlgo(cellAlgo,primary=True)

        from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
        acc.merge(CaloNoiseCondAlgCfg(ConfigFlags,"electronicNoise"))

        from LArBadChannelTool.LArBadChannelConfig import LArBadChannelCfg
        acc.merge(LArBadChannelCfg(ConfigFlags))

        from CaloMonitoring.LArCellMonAlg import LArCellMonConfig
        acc.merge(LArCellMonConfig(ConfigFlags))

        return acc

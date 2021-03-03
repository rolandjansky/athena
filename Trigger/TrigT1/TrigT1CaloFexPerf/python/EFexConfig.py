# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.Logging import logging
def enableEfexAlgorithms( algSequence, SuperCellContainer='SCell', 
                          doEle=True, doTau=True, logLevel = logging.INFO):

    log = logging.getLogger( 'TrigT1CaloFexPerf.EFexConfig' )
    log.setLevel(logLevel)

    from TrigT1CaloFexPerf.L1PerfControlFlags import L1Phase1PerfFlags as perfFlags

    # the CaloNoiseTool as global tool
    from AthenaCommon.AppMgr import ToolSvc
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    ToolSvc += CaloNoiseToolDefault()

    # eFEX clusters 
    if doEle:
        from TrigT1CaloFexPerf.TrigT1CaloFexPerfConf import LVL1__EFexEMAlgorithm
        log.info("Enable eFEX EM algorithm")
        emAlg = LVL1__EFexEMAlgorithm( name="EFexEMAlgorithm",
                                        InputSuperCellContainer = SuperCellContainer,
                                        OutputClusterName="SClusterCl",
                                        UseTileCells = perfFlags.Calo.UseAllCalo(),
                                        CleanCellContainerSkim=perfFlags.Calo.ApplySCQual(), 
                                        QualBitMask=perfFlags.Calo.QualBitMask(), 
                                        ApplyBaseLineSelection=False )

        emAlg.DefaultClusterTool.UseTileCells = perfFlags.Calo.UseAllCalo()
        emAlg.DefaultClusterTool.CleanCellContainer = perfFlags.Calo.ApplySCQual()
        emAlg.DefaultClusterTool.QualBitMask = perfFlags.Calo.QualBitMask()
        algSequence += emAlg

    # eFEX taus
    if doTau:
        from TrigT1CaloFexPerf.TrigT1CaloFexPerfConf import LVL1__EFexTauAlgorithm
        log.info("Enable eFEX Tau algorithm")
        algSequence += LVL1__EFexTauAlgorithm( name="EFexTauAlgorithm",
                                                InputSuperCellContainer=SuperCellContainer,
                                                OutputClusterName="SClusterTau",
                                                CleanCellContainer=perfFlags.Calo.ApplySCQual(),
                                                CleanCellContainerSkim=False,
                                                QualBitMask=perfFlags.Calo.QualBitMask() )



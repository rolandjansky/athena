# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

def enableEfexAlgorithms( algSequence,
                          SuperCellContainer='SCell', 
                          useTDR=False, 
                          doLArFex=False, 
                          doEle=True, 
                          doTau=True, 
                          ApplySCQual=True, 
                          SCBitMask=0x40,
                          logLevel = logging.INFO):

    log = logging.getLogger( 'TrigT1CaloFexSim.L1Sim.eFEX' )
    log.setLevel(logLevel)

    log.info("Enable eFEX algorithms")

    # the CaloNoiseTool as global tool
    from AthenaCommon.AppMgr import ToolSvc
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    theCaloNoiseTool=CaloNoiseToolDefault()
    ToolSvc+=theCaloNoiseTool

    from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import TrigT1CaloEFex
    # eFEX clusters with or without BCID based energy correction 
    if(useTDR and doEle):
        log.debug("Adding TDR version of TrigT1CaloEFex/TrigT1CaloEFexCl to AthAlgSeq")
        algSequence += TrigT1CaloEFex( name="TrigT1CaloEFexCl",
                                       CleanCellContainerSkim=ApplySCQual,
                                       QualBitMask = SCBitMask, 
                                       OutputClusterName="SClusterCl",
                                       SuperCellContainer=SuperCellContainer,
                                       EnergyWeightedCluster=True)

    elif doEle:
        log.debug("Adding new version of TrigT1CaloEFex/TrigT1CaloEFexCl to AthAlgSeq")
        algSequence += TrigT1CaloEFex( name="TrigT1CaloEFexCl",
                                       CleanCellContainer=ApplySCQual, 
                                       QualBitMask=SCBitMask, 
                                       OutputClusterName="SClusterCl",
                                       SuperCellContainer=SuperCellContainer,
                                       ClusterEnergyThreshold=4.0,
                                       ApplyBaseLineSelection=False,
                                       EnergyWeightedCluster=False)

    # Schedule the tau algorihtm
    if doTau:
        from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import TrigT1CaloRun3TauFex
        algSequence += TrigT1CaloRun3TauFex( name="TrigT1CaloEFexTau",
                                             CleanCellContainer=ApplySCQual,
                                             CleanCellContainerSkim=False,
                                             QualBitMask=SCBitMask,
                                             SuperCellContainer=SuperCellContainer,
                                             OutputClusterName="SClusterTau" )


    # LArFex algorithms for different sigma
    if( doLArFex ):
        from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import LArFex
        for sigma in [ 1, 2, 3 ]:
            log.debug("Adding LArFex/%iSig to AthAlgSeq" % sigma)
            algSequence += LArFex( name="%iSig" % sigma,
                                   EtInSigma = 3.0,
                                   CaloNoiseTool = theCaloNoiseTool,
                                   EtInSigmaSelect = sigma,
                                   OutputClusterName = "LArLayer1Vars%iSig" % sigma)
            

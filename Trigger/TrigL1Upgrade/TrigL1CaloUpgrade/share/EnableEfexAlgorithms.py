
def enableEfexAlgorithms(SuperCellContainer='SCell', useTDR=False, doLArFex=False, doEle=True, doTau=True):

    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'TrigT1CaloFexSim.L1Sim.eFEX' )
    log.setLevel(logging.DEBUG)

    log.info("Enable eFEX algorithms")

    # the CaloNoiseTool as global tool
    from AthenaCommon.AppMgr import ToolSvc
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    theCaloNoiseTool=CaloNoiseToolDefault()
    ToolSvc+=theCaloNoiseTool

    # add the algorithms to the sequence of all algorithms
    algseq = CfgMgr.AthSequencer("AthAlgSeq")

    # eFEX clusters with or without BCID based energy correction 
    if(useTDR and doEle):
        log.debug("Adding TDR version of TrigT1CaloEFex/TrigT1CaloEFexCl to AthAlgSeq")
        algseq += CfgMgr.TrigT1CaloEFex( name="TrigT1CaloEFexCl",
                                         CleanCellContainer=True,
                                         OutputClusterName="SClusterCl",
                                         SuperCellContainer=SuperCellContainer,
                                         EnergyWeightedCluster=True)

    elif doEle:
        log.debug("Adding new version of TrigT1CaloEFex/TrigT1CaloEFexCl to AthAlgSeq")
        algseq += CfgMgr.TrigT1CaloEFex( name="TrigT1CaloEFexCl",
                                         CleanCellContainer=False,
                                         OutputClusterName="SClusterCl",
                                         SuperCellContainer=SuperCellContainer,
                                         EnergyWeightedCluster=False)

    # Schedule the tau algorihtm
    if doTau:
        algseq += CfgMgr.TrigT1CaloRun3TauFex(name="TrigT1CaloEFexTau",
                                              CleanCellContainer=False,
                                              SuperCellContainer=SuperCellContainer,
                                              OutputClusterName="SClusterTau"
                                              )


    # LArFex algorithms for different sigma
    if( doLArFex ):
        for sigma in [ 1, 2, 3 ]:
            log.debug("Adding LArFex/%iSig to AthAlgSeq" % sigma)
            algseq += CfgMgr.LArFex( name="%iSig" % sigma,
                                     EtInSigma = 3.0,
                                     CaloNoiseTool = theCaloNoiseTool,
                                     EtInSigmaSelect = sigma,
                                     OutputClusterName = "LArLayer1Vars%iSig" % sigma)
            

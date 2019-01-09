#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

def metCellRecoSequence(RoIs):
    #################################################
    # Useful imports
    #################################################
    from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,WARNING

    from math import pi 
    from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc#, TestCaloDataAccess
    from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.Constants import VERBOSE,DEBUG,INFO

    from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq

    #################################################
    # Declare reco sequence. This will be returned at the end of the function
    #################################################
    metCellRecoSequence = seqAND("metCellRecoSequence")


    #################################################
    # Set up monitoring for CaloDataAccess
    #################################################
    mon = GenericMonitoringTool("CaloDataAccessSvcMon")
    mon.Histograms += [defineHistogram( "TIME_locking_LAr_RoI", title="Time spent in unlocking the LAr collection", xbins=100, xmin=0, xmax=100 ),
                       defineHistogram( "roiROBs_LAr", title="Number of ROBs unpacked in RoI requests", xbins=20, xmin=0, xmax=20 ),
                       defineHistogram( "TIME_locking_LAr_FullDet", title="Time spent in unlocking the LAr collection", xbins=100, xmin=0, xmax=100 ),
                       defineHistogram( "roiEta_LAr,roiPhi_LAr", type="TH2F", title="Geometric usage", xbins=50, xmin=-5, xmax=5, ybins=64, ymin=-pi, ymax=pi )]

    svcMgr += TrigCaloDataAccessSvc()
    svcMgr.TrigCaloDataAccessSvc.MonTool = mon
    svcMgr.TrigCaloDataAccessSvc.OutputLevel=WARNING

    #################################################
    # Get cells
    #################################################

    from TrigT2CaloCommon.CaloDef import algoHLTCaloCell
    cellMakerAlgo = algoHLTCaloCell(inputEDM=RoIs, outputEDM='CaloCells')
    metCellRecoSequence += cellMakerAlgo


    #################################################
    # Add EFMissingETAlg and associated tools
    #################################################
    from TrigEFMissingET.TrigEFMissingETConf import EFMissingETAlgMT, EFMissingETFromCellsMT, EFMissingETFromHelper
    metAlg = EFMissingETAlgMT( name="EFMET" )
    metAlg.OutputLevel=WARNING
    helperTool = EFMissingETFromHelper("theHelperTool") 
    metAlg.HelperTool= helperTool 
    metAlg.METContainerKey = "HLT_MET"
    
        #///////////////////////////////////////////
        # Setup monitoring for EFMissingETAlg
        #///////////////////////////////////////////
    metMon = GenericMonitoringTool("METMonTool")
    metMon.Histograms = [ defineHistogram( "TIME_Total", title="Time spent Alg", xbins=100, xmin=0, xmax=100 ),
                          defineHistogram( "TIME_Loop", title="Time spent in Tools loop", xbins=100, xmin=0, xmax=100 )]
    from TrigEFMissingET.TrigEFMissingETMonitoring import ( hEx_log, hEy_log, hEz_log, hMET_log, hSumEt_log,
                                                            hMET_lin, hSumEt_lin,
                                                            hXS, hMETPhi, hMETStatus,
                                                            hCompEx, hCompEy, hCompEz, hCompEt, hCompSumEt, hCompSumE,
                                                            hCompEt_lin, hCompSumEt_lin )

    metMon.Histograms  = [ hEx_log, hEy_log, hEz_log, hMET_log, hSumEt_log ]
    metMon.Histograms += [ hMET_lin, hSumEt_lin ]
    metMon.Histograms += [ hXS, hMETPhi, hMETStatus]
    metMon.Histograms += [ hCompEx, hCompEy, hCompEz, hCompEt, hCompSumEt, hCompSumE ]
    metMon.Histograms += [ hCompEt_lin, hCompSumEt_lin ]
    metAlg.MonTool = metMon

        #///////////////////////////////////////////
        # Add EFMissingETFromCells tool
        #///////////////////////////////////////////
    from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromCellsMT
    cellTool = EFMissingETFromCellsMT( name="METFromCellsTool" )
    cellTool.CellsCollection = cellMakerAlgo.CellsName
    
    metAlg.METTools.append(cellTool)

    metCellRecoSequence += metAlg

    seqOut = metAlg.METContainerKey
    return (metCellRecoSequence, seqOut)


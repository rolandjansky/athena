#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#


    #################################################
    # Useful imports
    #################################################
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,WARNING

from math import pi 

from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq


def metCellAthSequence(ConfigFlags):
    from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    InputMakerAlg= clusterFSInputMaker()
    (recoSequence, sequenceOut) = metCellRecoSequence()

    MetAthSequence =  seqAND("MetAthSequence",[InputMakerAlg, recoSequence ])
    return (MetAthSequence, InputMakerAlg, sequenceOut)

    
def metCellRecoSequence():

    from TrigT2CaloCommon.CaloDef import HLTFSCellMakerRecoSequence
    (metCellRecoSequence, CellsName) = HLTFSCellMakerRecoSequence()


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
    metMon.Histograms = [ defineHistogram( "TIME_Total", path='EXPERT', title="Time spent Alg", xbins=100, xmin=0, xmax=100 ),
                          defineHistogram( "TIME_Loop", path='EXPERT', title="Time spent in Tools loop", xbins=100, xmin=0, xmax=100 )]
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

    ### WARNING: this setting does not work for the scheduler: the variable is set, but the scheduler retrieves the default one
    cellTool.CellsCollection = CellsName

    
    metAlg.METTools.append(cellTool)

    metCellRecoSequence += metAlg

    seqOut = metAlg.METContainerKey
    return (metCellRecoSequence, seqOut)


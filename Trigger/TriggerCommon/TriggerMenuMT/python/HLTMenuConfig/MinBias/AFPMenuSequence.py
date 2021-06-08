# 
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
#

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR, seqAND
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
from AthenaCommon.GlobalFlags import globalflags
from TrigEDMConfig.TriggerEDMRun3 import recordable

def AFPTrkRecoSequence():

    # Create inputs using input maker
    AFPInputMaker = InputMakerForRoI("IM_AFPTrackingFS")
    AFPInputMaker.RoITool = ViewCreatorInitialROITool()
    AFPInputMaker.RoIs = "AFPRoIs"

    # get AFP_GeometryTool 
    from AthenaConfiguration.ComponentFactory import CompFactory
    AFP_Geometry_tool = CompFactory.getComp("AFP_GeometryTool")("AFP_Geometry_tool")

    from AFP_ByteStream2RawCnv.AFP_ByteStream2RawCnvConf import AFP_RawDataProvider
    AFP_Raw = AFP_RawDataProvider()

    from AFP_Raw2Digi.AFP_Raw2DigiConf import AFP_Raw2Digi
    AFP_R2D = AFP_Raw2Digi()

    # select between a real data or a simulation mode - Simulation = 0, RealData = 1
    if globalflags.DataSource()=='data':
        AFP_SIDLocReco_DataType = 1
    else:
        AFP_SIDLocReco_DataType = 0

    AFP_SIDLocReco_ListAlgoSID = ["SIDBasicKalman"]
    AFP_SIDLocReco_AmpThresh = 5000.
    
    # SID part
    from AFP_LocReco.AFP_LocRecoConf import AFP_SIDLocReco
    AFP_SID = AFP_SIDLocReco("AFP_SIDLocReco",
                            AFP_Geometry = AFP_Geometry_tool,
                            vecListAlgoSID = AFP_SIDLocReco_ListAlgoSID,
                            DataType = AFP_SIDLocReco_DataType,
                            AmpThresh = AFP_SIDLocReco_AmpThresh, 
                            AFPTrackContainerKey = recordable('HLT_AFPTrackContainer'))
    
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool("MonTool_AFP_SIDLocReco")

    monTool.defineHistogram( 'HitsSize', path='EXPERT', type='TH1F', title='SID hits size',xbins=50, xmin=0, xmax=50 )

    monTool.defineHistogram( 'TrkSize', path='EXPERT', type='TH1F', title='AFP tracks size',xbins=50, xmin=0, xmax=100 )
    
    monTool.defineHistogram( 'TrkStationID', path='EXPERT', type='TH1F', title='Track station ID',xbins=4, xmin=0, xmax=4 )
    monTool.defineHistogram( 'TrkXLocal', path='EXPERT', type='TH1F', title='Track xLocal',xbins=100, xmin=-50, xmax=50 )
    monTool.defineHistogram( 'TrkYLocal', path='EXPERT', type='TH1F', title='Track yLocal',xbins=100, xmin=-30, xmax=30 )
    monTool.defineHistogram( 'TrkZLocal', path='EXPERT', type='TH1F', title='Track zLocal',xbins=100, xmin=-50, xmax=50 )
    
    monTool.defineHistogram( 'TrkXSlope', path='EXPERT', type='TH1F', title='Track x slope',xbins=100, xmin=-0.5, xmax=0.5 )
    monTool.defineHistogram( 'TrkYSlope', path='EXPERT', type='TH1F', title='Track y slope',xbins=100, xmin=-0.5, xmax=0.5 )
    monTool.defineHistogram( 'TrkNHoles', path='EXPERT', type='TH1F', title='Track number of holes',xbins=0, xmin=0, xmax=200 )
    monTool.defineHistogram( 'TrkChi2', path='EXPERT', type='TH1F', title='Track chi2',xbins=50, xmin=0, xmax=10 )

    AFP_SID.MonTool = monTool
    
    # TID part
    # from AFP_LocReco.AFP_LocRecoConf import AFP_TDLocReco
    # AFP_TD = AFP_TDLocReco("AFP_TDLocReco")

    # if globalflags.DataSource()=='data':
    #     AFP_TDLocReco.DataType = 1
    # else:
    #     AFP_TDLocReco.DataType = 0

    # # select algo for timing reco and parameters
    # AFP_TDLocReco.ListAlgoTD = ["TDZVertex"]
    # AFP_TDLocReco.AmpThresh = 10.

    # Pass-through hypo
    def trigStreamerAFPHypoTool(chain_dict):
        return conf2toConfigurable(CompFactory.TrigStreamerHypoTool(chain_dict["chainName"]))
    
    hypo = conf2toConfigurable(CompFactory.TrigStreamerHypoAlg("AFPPassThroughHypo"))
    trigHypoToolGen = trigStreamerAFPHypoTool

    # Build the entire sequence (input maker+reco+dummy hypo)
    AFPRecoSeq = parOR("AFPRecoSeq", [AFP_Raw, AFP_R2D, AFP_SID])
    AFPSequence = seqAND("AFPSequence", [AFPInputMaker, AFPRecoSeq])

    return MenuSequence(Sequence = AFPSequence,
                        Maker    = AFPInputMaker,
                        Hypo     = hypo,
                        HypoToolGen = trigHypoToolGen)

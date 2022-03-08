#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''
@file TgcRawDataMonitorAlgorithm.py
@author M.Aoki
@date 2019-10-03
@brief Python configuration for the Run III AthenaMonitoring package for TGC 
'''

def TgcRawDataMonitoringConfig(inputFlags):
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.Enums import Format
    result = ComponentAccumulator()

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    result.merge(MagneticFieldSvcCfg(inputFlags))
    result.merge(AtlasGeometryCfg(inputFlags))
    
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TgcRawDataMonitorCfg')

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(inputFlags))

    tgcRawDataMonitorTool = CompFactory.TgcRawDataMonitorTool("TgcRawDataMonitorTool")

    muonSelectionTool = CompFactory.CP.MuonSelectionTool("TgcMonMuonSelectionTool")
    muonSelectionTool.MuQuality = 0 # tight:0 medium:1

    tgcRawDataMonAlg = helper.addAlgorithm(CompFactory.TgcRawDataMonitorAlgorithm,'TgcRawDataMonAlg',
                                           TrackExtrapolator = extrapolator,
                                           TgcRawDataMonitorTool = tgcRawDataMonitorTool,
                                           MuonSelectionTool = muonSelectionTool )

    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    result.merge( TrackingGeometryCondAlgCfg(inputFlags ) )

    doGapByGapHitOcc = True
    doGapByGapEffMap = False
    doHitResiduals = True

    tgcRawDataMonAlg.FillGapByGapHistograms = (doGapByGapHitOcc or doGapByGapEffMap or doHitResiduals)

    tgcRawDataMonAlg.PrintAvailableMuonTriggers = False

    tgcRawDataMonAlg.MonitorTriggerMultiplicity = False
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_MU4_Run3,Mul:1,HLT:HLT_mu4_l2io_L1MU4,RPC:1,TGC:1;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_MU6_Run3,Mul:1,HLT:HLT_mu6_L1MU6,RPC:2,TGC:3F;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_MU10_Run3,Mul:1,HLT:HLT_mu10_L1MU10,RPC:3,TGC:6F;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_MU20_Run3,Mul:1,HLT:HLT_mu24_L1MU20,RPC:6,TGC:12FCH;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_MU3V_Run3,Mul:1,HLT:HLT_mu4_l2io_L1MU3V,RPC:1,TGC:1;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_MU5VF_Run3,Mul:1,HLT:HLT_mu20_L1MU5VF,RPC:2,TGC:3F;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_MU8F_Run3,Mul:1,HLT:HLT_mu10_L1MU8F,RPC:3,TGC:6F;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_MU14FCH_Run3,Mul:1,HLT:HLT_mu50_L1MU14FCH,RPC:6,TGC:12FCH;"

    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_2MU4_Run3,Mul:2,HLT:HLT_2mu4_L12MU4,RPC:1,TGC:1;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_2MU6_Run3,Mul:2,HLT:HLT_2mu6_L12MU6,RPC:2,TGC:3F;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_2MU10_Run3,Mul:2,HLT:HLT_2mu14_L12MU10,RPC:3,TGC:6F;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_2MU3V_Run3,Mul:2,HLT:HLT_2mu4_L12MU3V,RPC:1,TGC:1;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_2MU5VF_Run3,Mul:2,HLT:HLT_2mu6_L12MU5VF,RPC:2,TGC:3F;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_2MU8F_Run3,Mul:2,HLT:HLT_2mu14_L12MU8F,RPC:3,TGC:6F;"

    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_3MU4_Run3,Mul:3,HLT:HLT_3mu4_bJpsi_L13MU4,RPC:1,TGC:1;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_3MU6_Run3,Mul:3,HLT:HLT_3mu6_L13MU6,RPC:2,TGC:3F;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_3MU3V_Run3,Mul:3,HLT:HLT_3mu4_bJpsi_L13MU3V,RPC:1,TGC:1;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_3MU5VF_Run3,Mul:3,HLT:HLT_3mu6_L13MU5VF,RPC:2,TGC:3F;"

    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_4MU3V_Run3,Mul:4,HLT:HLT_4mu4_L14MU3V,RPC:1,TGC:1;"
    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_4MU4_Run3,Mul:4,HLT:HLT_4mu4_L14MU4,RPC:1,TGC:1;"

    tgcRawDataMonAlg.CtpDecisionMoniorList += "Tit:L1_MU10BOM_Run3,Mul:1,HLT:HLT_2mu10_l2mt_L1MU10BOM,RPC:4M,TGC:99;"

    tgcRawDataMonAlg.MonitorThresholdPatterns = False
    tgcRawDataMonAlg.ThrPatternList = "MU4,MU6,MU10,MU11,MU20,MU21,"
    tgcRawDataMonAlg.ThrPatternList += "MU3V,MU3VF,MU3VC,MU5VF,MU8F,MU8VF,MU8FC,MU9VF,MU9VFC,MU14FCH,MU14FCHR,MU15VFCH,MU15VFCHR,MU18VFCH,MU10BOM,MU12BOM,MU8FH,MU20FC,MU12FCH,MU4BOM,MU4BO,MU14EOF,MU8EOF,MU3EOF,"

    tgcRawDataMonAlg.TagAndProbe = True
    tgcRawDataMonAlg.TagAndProbeZmumu = False
    tgcRawDataMonAlg.UseNonMuonTriggers = False

    if not inputFlags.DQ.triggerDataAvailable:
        tgcRawDataMonAlg.MuonRoIContainerName = ''

    if inputFlags.Input.Format is Format.BS or 'TGC_MeasurementsAllBCs' in inputFlags.Input.Collections:
        tgcRawDataMonAlg.AnaTgcPrd=True
    if inputFlags.Input.Format is Format.BS or ('TrigT1CoinDataCollection' in inputFlags.Input.Collections and
                                                'TrigT1CoinDataCollectionNextBC' in inputFlags.Input.Collections and
                                                'TrigT1CoinDataCollectionPriorBC' in inputFlags.Input.Collections ):
        tgcRawDataMonAlg.AnaTgcCoin=True
    
    mainDir = 'Muon/MuonRawDataMonitoring/TGC/'
    import math

    myGroup = helper.addGroup(tgcRawDataMonAlg,'TgcRawDataMonitor',mainDir)

    ################################################################################################################
    commonPath = 'Common/'
    myGroupCommon = helper.addGroup(tgcRawDataMonAlg, 'TgcRawDataMonitor_Common', mainDir)
    myGroupCommon.defineHistogram('mon_lb;TgcMon_LuminosityBlock',title='Luminosity Block;Luminosity Block;Number of events',
                                  path=commonPath,type='TH1F',xbins=100,xmin=0,xmax=10,opt='kAddBinsDynamically')
    myGroupCommon.defineHistogram('mon_bcid;TgcMon_BCID',title='BCID;BCID;Number of events',
                                  path=commonPath,type='TH1F',xbins=100,xmin=0,xmax=10,opt='kAddBinsDynamically')
    myGroupCommon.defineHistogram('mon_pileup;TgcMon_Pileup',title='Pileup;Pileup;Number of events',
                                  path=commonPath,type='TH1F',xbins=100,xmin=0,xmax=10,opt='kAddBinsDynamically')
    
    ################################################################################################################
    muonPath = 'Muon/'
    myGroupMuon = helper.addGroup(tgcRawDataMonAlg, 'TgcRawDataMonitor_Muon', mainDir)
    myGroupMuon.defineHistogram('oflmuon_num;TgcMon_OfflineMuon_num',title='OfflineMuon_num;OfflineMuon num per event;Number of events',
                                  path=muonPath,type='TH1F',xbins=21,xmin=-0.5,xmax=20.5)
    myGroupMuon.defineHistogram('oflmuon_muonType;TgcMon_OfflineMuon_muonType',title='OfflineMuon_muonType;OfflineMuon muonType;Number of events',
                                  path=muonPath,type='TH1F',xbins=21,xmin=-0.5,xmax=20.5)
    myGroupMuon.defineHistogram('oflmuon_author;TgcMon_OfflineMuon_author',title='OfflineMuon_author;OfflineMuon author;Number of events',
                                  path=muonPath,type='TH1F',xbins=21,xmin=-0.5,xmax=20.5)
    myGroupMuon.defineHistogram('oflmuon_quality;TgcMon_OfflineMuon_quality',title='OfflineMuon_quality;OfflineMuon quality;Number of events',
                                  path=muonPath,type='TH1F',xbins=21,xmin=-0.5,xmax=20.5)
    myGroupMuon.defineHistogram('oflmuon_pt;TgcMon_OfflineMuon_Pt',title='OfflineMuon_Pt;OfflineMuon pT [GeV];Number of events',
                                  path=muonPath,type='TH1F',xbins=100,xmin=0,xmax=100)
    myGroupMuon.defineHistogram('oflmuon_eta,oflmuon_phi;TgcMon_OfflineMuon_EtaVsPhi',title='OfflineMuon_EtaVsPhi;OfflineMuon Eta;OfflineMuon Phi',
                                  path=muonPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)


    ################################################################################################################
    trigThrPatternsPath = 'TrigPatterns/'
    for monTrig in tgcRawDataMonAlg.ThrPatternList.split(','):
        monTrigGroup = helper.addGroup(tgcRawDataMonAlg, 'TgcRawDataMonitor'+monTrig, mainDir)
        monTrigGroup.defineHistogram('l1item_roi_eta_'+monTrig+',l1item_roi_phi_'+monTrig+';L1Item_MuonRoI_EtaVsPhi_'+monTrig,title='L1Item_MuonRoI_EtaVsPhi_'+monTrig+';MuonRoI Eta;MuonRoI Phi',
                                     path=trigThrPatternsPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
        monTrigGroup.defineHistogram('l1item_roi_eta_'+monTrig+',l1item_roi_thrNumber_'+monTrig+';L1Item_MuonRoI_EtaVsThrNumber_'+monTrig,title='L1Item_MuonRoI_EtaVsThrNumber_'+monTrig+';MuonRoI Eta;MuonRoI Threshold Number',
                                     path=trigThrPatternsPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=15,ymin=0.5,ymax=15.5)
        monTrigGroup.defineHistogram('l1item_roi_eta_'+monTrig+',l1item_roi_ismorecand_'+monTrig+';L1Item_MuonRoI_EtaVsIsMoreCand_'+monTrig,title='L1Item_MuonRoI_EtaVsIsMoreCand_'+monTrig+';MuonRoI Eta;MuonRoI IsMoreCand Flag',
                                     path=trigThrPatternsPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=3,ymin=-1.5,ymax=1.5)
        monTrigGroup.defineHistogram('l1item_roi_eta_'+monTrig+',l1item_roi_bw3coin_'+monTrig+';L1Item_MuonRoI_EtaVsBW3Coin_'+monTrig,title='L1Item_MuonRoI_EtaVsBW3Coin_'+monTrig+';MuonRoI Eta;MuonRoI BW3Coin Flag',
                                     path=trigThrPatternsPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=3,ymin=-1.5,ymax=1.5)
        monTrigGroup.defineHistogram('l1item_roi_eta_'+monTrig+',l1item_roi_innercoin_'+monTrig+';L1Item_MuonRoI_EtaVsInnerCoin_'+monTrig,title='L1Item_MuonRoI_EtaVsInnerCoin_'+monTrig+';MuonRoI Eta;MuonRoI InnerCoin Flag',
                                     path=trigThrPatternsPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=3,ymin=-1.5,ymax=1.5)
        monTrigGroup.defineHistogram('l1item_roi_eta_'+monTrig+',l1item_roi_goodmf_'+monTrig+';L1Item_MuonRoI_EtaVsGoodMF_'+monTrig,title='L1Item_MuonRoI_EtaVsGoodMF_'+monTrig+';MuonRoI Eta;MuonRoI GoodMF Flag',
                                     path=trigThrPatternsPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=3,ymin=-1.5,ymax=1.5)

        monTrigGroup.defineHistogram('muon_passed_l1item_'+monTrig+',muon_pt_rpc;L1Item_MuonRoI_Eff_Pt_RPC_'+monTrig,title='L1Item_MuonRoI_Eff_Pt_RPC_'+monTrig+';Offline muon pT [GeV];Efficiency',
                                type='TEfficiency',path=trigThrPatternsPath,xbins=50,xmin=0,xmax=50)
        monTrigGroup.defineHistogram('muon_passed_l1item_'+monTrig+',muon_pt_tgc;L1Item_MuonRoI_Eff_Pt_TGC_'+monTrig,title='L1Item_MuonRoI_Eff_Pt_TGC_'+monTrig+';Offline muon pT [GeV];Efficiency',
                                type='TEfficiency',path=trigThrPatternsPath,xbins=50,xmin=0,xmax=50)
        monTrigGroup.defineHistogram('muon_passed_l1item_'+monTrig+',muon_phi_rpc;L1Item_MuonRoI_Eff_Phi_RPC_'+monTrig,title='L1Item_MuonRoI_Eff_Phi_RPC_'+monTrig+';Offline muon phi [rad.];Efficiency',
                                type='TEfficiency',path=trigThrPatternsPath,xbins=32,xmin=-math.pi,xmax=math.pi)
        monTrigGroup.defineHistogram('muon_passed_l1item_'+monTrig+',muon_phi_tgc;L1Item_MuonRoI_Eff_Phi_TGC_'+monTrig,title='L1Item_MuonRoI_Eff_Phi_TGC_'+monTrig+';Offline muon phi [rad.];Efficiency',
                                type='TEfficiency',path=trigThrPatternsPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        monTrigGroup.defineHistogram('muon_passed_l1item_'+monTrig+',muon_eta;L1Item_MuonRoI_Eff_Eta_'+monTrig,title='L1Item_MuonRoI_Eff_Eta_'+monTrig+';Offline muon eta;Efficiency',
                                type='TEfficiency',path=trigThrPatternsPath,xbins=100,xmin=-2.5,xmax=2.5)
        monTrigGroup.defineHistogram('muon_passed_l1item_'+monTrig+',muon_eta,muon_phi;L1Item_MuonRoI_Eff_EtaVsPhi_'+monTrig,title='L1Item_MuonRoI_Eff_EtaVsPhi_'+monTrig+';Offline muon eta; Offline muon phi',
                                type='TEfficiency',path=trigThrPatternsPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)


    ################################################################################################################
    trigMultiPath = 'TrigMultiplicity/'
    for monTrig in tgcRawDataMonAlg.CtpDecisionMoniorList.split(';'):
        tmp = monTrig.split(',')[0]
        objname = tmp.replace('Tit:','')
        monTrigGroup = helper.addGroup(tgcRawDataMonAlg, 'TgcRawDataMonitor'+objname, mainDir)

        monTrigGroup.defineHistogram('ctpMultiplicity;'+objname+'_ctpMultiplicity',title=objname+' ctpMultiplicity;Ctp Output Multiplicity;Number of events',
                                path=trigMultiPath,xbins=11,xmin=-0.5,xmax=10.5)
        monTrigGroup.defineHistogram('rawMultiplicity;'+objname+'_rawMultiplicity',title=objname+' rawMultiplicity;Raw Input Multiplicity;Number of events',
                                path=trigMultiPath,xbins=11,xmin=-0.5,xmax=10.5)
        monTrigGroup.defineHistogram('countDiff;'+objname+'_L1CountDiff',title=objname+' L1CountDiff;Event-by-event N(CTP out)-N(CTP in);Number of events',
                                path=trigMultiPath,xbins=11,xmin=-5.5,xmax=5.5)

        monTrigGroup.defineHistogram('roiMatching_CTPin,roiMatching_CTPout;'+objname+'_RoiMatching',title=objname+'_RoiMatching;Input;Output',
                                path=trigMultiPath,type='TH2F',xbins=2,xmin=-0.5,xmax=1.5,ybins=2,ymin=-0.5,ymax=1.5,xlabels=['NotExist','Exist'],ylabels=['NotExist','Exist'])

        monTrigGroup.defineHistogram('Eta,Phi;'+objname+'_L1Count_EtaVsPhi_inOk_outOk',title=objname+'_L1Count_EtaVsPhi_inOk_outOk;MuonRoI Eta;MuonRoI Phi',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
        monTrigGroup.defineHistogram('Eta,dRmin;'+objname+'_L1Count_EtaVsdRmin_inOk_outOk',title=objname+'_L1Count_EtaVsdRmin_inOk_outOk;MuonRoI Eta;Closest dR between Muon RoIs',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=11,ymin=-0.1,ymax=1.0)
        monTrigGroup.defineHistogram('dRmin;'+objname+'_roi_dRmin_inOk_outOk',title=objname+'_roi_dRmin_inOk_outOk;Closest dR between Muon RoIs;Number of events',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH1F',xbins=11,xmin=-0.1,xmax=1.0)
        monTrigGroup.defineHistogram('ThrNum;'+objname+'_roi_ThrNum_inOk_outOk',title=objname+'_roi_ThrNum_inOk_outOk;Threshold number (positive for barrel, negative for endcap);Number of events',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH1F',xbins=22,xmin=-15.5,xmax=6.5)
        monTrigGroup.defineHistogram('Charge;'+objname+'_roi_Charge_inOk_outOk',title=objname+'_roi_Charge_inOk_outOk;Muon charge;Number of events',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('BW3Coin;'+objname+'_roi_BW3Coin_inOk_outOk',title=objname+'_roi_BW3Coin_inOk_outOk;TGC BW3Coin flag;Number of events',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('InnerCoin;'+objname+'_roi_InnerCoin_inOk_outOk',title=objname+'_roi_InnerCoin_inOk_outOk;TGC InnerCoin flag;Number of events',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('GoodMF;'+objname+'_roi_GoodMF_inOk_outOk',title=objname+'_roi_GoodMF_inOk_outOk;TGC GoodMF flag;Number of events',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('IsMoreCandInRoI;'+objname+'_roi_IsMoreCandInRoI_inOk_outOk',title=objname+'_roi_IsMoreCandInRoI_inOk_outOk;RPC IsMoreCandInRoI flag;Number of events',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('PhiOverlap;'+objname+'_roi_PhiOverlap_inOk_outOk',title=objname+'_roi_PhiOverlap_inOk_outOk;PhiOverlap flag;Number of events',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('EtaOverlap;'+objname+'_roi_EtaOverlap_inOk_outOk',title=objname+'_roi_EtaOverlap_inOk_outOk;EtaOverlap flag;Number of events',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH1F',xbins=2,xmin=-0.5,xmax=1.5)
        monTrigGroup.defineHistogram('dRmin,PhiOverlap;'+objname+'_roi_dRminVsPhiOverlap_inOk_outOk',
                                title=objname+'_roi_dRminVsPhiOverlap_inOk_outOk;Closest dR between Muon RoIs;PhiOverlap flag',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH2F',xbins=11,xmin=-0.1,xmax=1.0,ybins=3,ymin=-1.5,ymax=1.5)
        monTrigGroup.defineHistogram('isVetoed;'+objname+'_roi_isVetoed_inOk_outOk',title=objname+'_roi_isVetoed_inOk_outOk;isVetoed flag;Number of events',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH1F',xbins=2,xmin=-0.5,xmax=1.5)
        monTrigGroup.defineHistogram('dRmin,isVetoed;'+objname+'_roi_dRminVsisVetoed_inOk_outOk',
                                title=objname+'_roi_dRminVsisVetoed_inOk_outOk;Closest dR between Muon RoIs;isVetoed flag',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH2F',xbins=11,xmin=-0.1,xmax=1.0,ybins=2,ymin=-0.5,ymax=1.5)
        monTrigGroup.defineHistogram('Eta,isVetoed;'+objname+'_roi_EtaVsisVetoed_inOk_outOk',
                                title=objname+'_roi_EtaVsisVetoed_inOk_outOk;MuonRoI Eta;isVetoed flag',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=2,ymin=-0.5,ymax=1.5)
        monTrigGroup.defineHistogram('Eta,pTdiff;'+objname+'_L1Count_EtaVspTdiff_inOk_outOk',title=objname+'_L1Count_EtaVspTdiff_inOk_outOk;MuonRoI Eta;pT difference',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=31,ymin=-15.5,ymax=15.5)
        monTrigGroup.defineHistogram('pTdiff;'+objname+'_roi_pTdiff_inOk_outOk',title=objname+'_roi_pTdiff_inOk_outOk;pT difference;Number of events',
                                cutmask='inOk_outOk',path=trigMultiPath,type='TH1F',xbins=31,xmin=-15.5,xmax=15.5)


        monTrigGroup.defineHistogram('Eta,Phi;'+objname+'_L1Count_EtaVsPhi_inOk_outNg',title=objname+'_L1Count_EtaVsPhi_inOk_outNg;MuonRoI Eta;MuonRoI Phi',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
        monTrigGroup.defineHistogram('Eta,dRmin;'+objname+'_L1Count_EtaVsdRmin_inOk_outNg',title=objname+'_L1Count_EtaVsdRmin_inOk_outNg;MuonRoI Eta;Closest dR between Muon RoIs',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=11,ymin=-0.1,ymax=1.0)
        monTrigGroup.defineHistogram('dRmin;'+objname+'_roi_dRmin_inOk_outNg',title=objname+'_roi_dRmin_inOk_outNg;Closest dR between Muon RoIs;Number of events',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH1F',xbins=11,xmin=-0.1,xmax=1.0)
        monTrigGroup.defineHistogram('ThrNum;'+objname+'_roi_ThrNum_inOk_outNg',title=objname+'_roi_ThrNum_inOk_outNg;Threshold number (positive for barrel, negative for endcap);Number of events',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH1F',xbins=22,xmin=-15.5,xmax=6.5)
        monTrigGroup.defineHistogram('Charge;'+objname+'_roi_Charge_inOk_outNg',title=objname+'_roi_Charge_inOk_outNg;Muon charge;Number of events',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('BW3Coin;'+objname+'_roi_BW3Coin_inOk_outNg',title=objname+'_roi_BW3Coin_inOk_outNg;TGC BW3Coin flag;Number of events',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('InnerCoin;'+objname+'_roi_InnerCoin_inOk_outNg',title=objname+'_roi_InnerCoin_inOk_outNg;TGC InnerCoin flag;Number of events',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('GoodMF;'+objname+'_roi_GoodMF_inOk_outNg',title=objname+'_roi_GoodMF_inOk_outNg;TGC GoodMF flag;Number of events',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('IsMoreCandInRoI;'+objname+'_roi_IsMoreCandInRoI_inOk_outNg',title=objname+'_roi_IsMoreCandInRoI_inOk_outNg;RPC IsMoreCandInRoI flag;Number of events',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('PhiOverlap;'+objname+'_roi_PhiOverlap_inOk_outNg',title=objname+'_roi_PhiOverlap_inOk_outNg;PhiOverlap flag;Number of events',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('EtaOverlap;'+objname+'_roi_EtaOverlap_inOk_outNg',title=objname+'_roi_EtaOverlap_inOk_outNg;EtaOverlap flag;Number of events',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH1F',xbins=2,xmin=-0.5,xmax=1.5)
        monTrigGroup.defineHistogram('dRmin,PhiOverlap;'+objname+'_roi_dRminVsPhiOverlap_inOk_outNg',
                                title=objname+'_roi_dRminVsPhiOverlap_inOk_outNg;Closest dR between Muon RoIs;PhiOverlap flag',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH2F',xbins=11,xmin=-0.1,xmax=1.0,ybins=3,ymin=-1.5,ymax=1.5)
        monTrigGroup.defineHistogram('isVetoed;'+objname+'_roi_isVetoed_inOk_outNg',title=objname+'_roi_isVetoed_inOk_outNg;isVetoed flag;Number of events',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH1F',xbins=2,xmin=-0.5,xmax=1.5)
        monTrigGroup.defineHistogram('dRmin,isVetoed;'+objname+'_roi_dRminVsisVetoed_inOk_outNg',
                                title=objname+'_roi_dRminVsisVetoed_inOk_outNg;Closest dR between Muon RoIs;isVetoed flag',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH2F',xbins=11,xmin=-0.1,xmax=1.0,ybins=2,ymin=-0.5,ymax=1.5)
        monTrigGroup.defineHistogram('Eta,isVetoed;'+objname+'_roi_EtaVsisVetoed_inOk_outNg',
                                title=objname+'_roi_EtaVsisVetoed_inOk_outNg;MuonRoI Eta;isVetoed flag',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=2,ymin=-0.5,ymax=1.5)
        monTrigGroup.defineHistogram('Eta,pTdiff;'+objname+'_L1Count_EtaVspTdiff_inOk_outNg',title=objname+'_L1Count_EtaVspTdiff_inOk_outNg;MuonRoI Eta;pT difference',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=31,ymin=-15.5,ymax=15.5)
        monTrigGroup.defineHistogram('pTdiff;'+objname+'_roi_pTdiff_inOk_outNg',title=objname+'_roi_pTdiff_inOk_outNg;pT difference;Number of events',
                                cutmask='inOk_outNg',path=trigMultiPath,type='TH1F',xbins=31,xmin=-15.5,xmax=15.5)


        monTrigGroup.defineHistogram('Eta,Phi;'+objname+'_L1Count_EtaVsPhi_inNg_outOk',title=objname+'_L1Count_EtaVsPhi_inNg_outOk;MuonRoI Eta;MuonRoI Phi',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
        monTrigGroup.defineHistogram('Eta,dRmin;'+objname+'_L1Count_EtaVsdRmin_inNg_outOk',title=objname+'_L1Count_EtaVsdRmin_inNg_outOk;MuonRoI Eta;Closest dR between Muon RoIs',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=11,ymin=-0.1,ymax=1.0)
        monTrigGroup.defineHistogram('dRmin;'+objname+'_roi_dRmin_inNg_outOk',title=objname+'_roi_dRmin_inNg_outOk;Closest dR between Muon RoIs;Number of events',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH1F',xbins=11,xmin=-0.1,xmax=1.0)
        monTrigGroup.defineHistogram('ThrNum;'+objname+'_roi_ThrNum_inNg_outOk',title=objname+'_roi_ThrNum_inNg_outOk;Threshold number (positive for barrel, negative for endcap);Number of events',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH1F',xbins=22,xmin=-15.5,xmax=6.5)
        monTrigGroup.defineHistogram('Charge;'+objname+'_roi_Charge_inNg_outOk',title=objname+'_roi_Charge_inNg_outOk;Muon charge;Number of events',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('BW3Coin;'+objname+'_roi_BW3Coin_inNg_outOk',title=objname+'_roi_BW3Coin_inNg_outOk;TGC BW3Coin flag;Number of events',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('InnerCoin;'+objname+'_roi_InnerCoin_inNg_outOk',title=objname+'_roi_InnerCoin_inNg_outOk;TGC InnerCoin flag;Number of events',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('GoodMF;'+objname+'_roi_GoodMF_inNg_outOk',title=objname+'_roi_GoodMF_inNg_outOk;TGC GoodMF flag;Number of events',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('IsMoreCandInRoI;'+objname+'_roi_IsMoreCandInRoI_inNg_outOk',title=objname+'_roi_IsMoreCandInRoI_inNg_outOk;RPC IsMoreCandInRoI flag;Number of events',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('PhiOverlap;'+objname+'_roi_PhiOverlap_inNg_outOk',title=objname+'_roi_PhiOverlap_inNg_outOk;PhiOverlap flag;Number of events',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH1F',xbins=3,xmin=-1.5,xmax=1.5)
        monTrigGroup.defineHistogram('EtaOverlap;'+objname+'_roi_EtaOverlap_inNg_outOk',title=objname+'_roi_EtaOverlap_inNg_outOk;EtaOverlap flag;Number of events',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH1F',xbins=2,xmin=-0.5,xmax=1.5)
        monTrigGroup.defineHistogram('dRmin,PhiOverlap;'+objname+'_roi_dRminVsPhiOverlap_inNg_outOk',
                                title=objname+'_roi_dRminVsPhiOverlap_inNg_outOk;Closest dR between Muon RoIs;PhiOverlap flag',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH2F',xbins=11,xmin=-0.1,xmax=1.0,ybins=3,ymin=-1.5,ymax=1.5)
        monTrigGroup.defineHistogram('isVetoed;'+objname+'_roi_isVetoed_inNg_outOk',title=objname+'_roi_isVetoed_inNg_outOk;isVetoed flag;Number of events',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH1F',xbins=2,xmin=-0.5,xmax=1.5)
        monTrigGroup.defineHistogram('dRmin,isVetoed;'+objname+'_roi_dRminVsisVetoed_inNg_outOk',
                                title=objname+'_roi_dRminVsisVetoed_inNg_outOk;Closest dR between Muon RoIs;isVetoed flag',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH2F',xbins=11,xmin=-0.1,xmax=1.0,ybins=2,ymin=-0.5,ymax=1.5)
        monTrigGroup.defineHistogram('Eta,isVetoed;'+objname+'_roi_EtaVsisVetoed_inNg_outOk',
                                title=objname+'_roi_EtaVsisVetoed_inNg_outOk;MuonRoI Eta;isVetoed flag',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=2,ymin=-0.5,ymax=1.5)
        monTrigGroup.defineHistogram('Eta,pTdiff;'+objname+'_L1Count_EtaVspTdiff_inNg_outOk',title=objname+'_L1Count_EtaVspTdiff_inNg_outOk;MuonRoI Eta;pT difference',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH2F',xbins=100,xmin=-2.5,xmax=2.5,ybins=31,ymin=-15.5,ymax=15.5)
        monTrigGroup.defineHistogram('pTdiff;'+objname+'_roi_pTdiff_inNg_outOk',title=objname+'_roi_pTdiff_inNg_outOk;pT difference;Number of events',
                                cutmask='inNg_outOk',path=trigMultiPath,type='TH1F',xbins=31,xmin=-15.5,xmax=15.5)



    ################################################################################################################
    trigPath = 'Trig/'
    myGroup.defineHistogram('roi_charge;MuonRoI_Evt_Charge',title='MuonRoI Charge;Charge;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=3,xmin=-1.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passCharge,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_Charge',title='MuonRoI_Eff_EtaVsPhi_Charge;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passCharge,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_Charge',title='MuonRoI_Eff_Pt_TGC_Charge;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_Evt_EtaVsPhi_Charge_Positive',type='TH2F',
                            title='MuonRoI Eta vs Phi Charge Positive;MuonRoI Eta;MuonRoI Phi',cutmask='roi_posCharge',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_Evt_EtaVsPhi_Charge_Negative',type='TH2F',
                            title='MuonRoI Eta vs Phi Charge Negative;MuonRoI Eta;MuonRoI Phi',cutmask='roi_negCharge',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_charge;MuonRoI_Evt_ChargeVsThr',type='TH2F',
                            title='MuonRoI Charge vs Thr;MuonRoI Thresholds;MuonRoI Charge',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=3,ymin=-1.5,ymax=1.5)

    myGroup.defineHistogram('roi_bw3coin;MuonRoI_Evt_BW3Coin',title='MuonRoI BW3Coin Flag;BW3Coin Flag;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passBW3Coin,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_BW3Coin',title='MuonRoI_Eff_EtaVsPhi_BW3Coin;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passBW3Coin,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_BW3Coin',title='MuonRoI_Eff_Pt_TGC_BW3Coin;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_Evt_EtaVsPhi_BW3Coin',type='TH2F',
                            title='MuonRoI Eta vs Phi BW3Coin;MuonRoI Eta;MuonRoI Phi',cutmask='roi_bw3coin',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_bw3coin;MuonRoI_Evt_ThrVsBW3Coin',type='TH2F',
                            title='MuonRoI Thr vs BW3Coin;MuonRoI Thresholds;MuonRoI BW3Coin',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_bw3coinveto;MuonRoI_Evt_BW3CoinVeto',title='MuonRoI BW3CoinVeto Flag;BW3CoinVeto Flag;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passBW3CoinVeto,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_BW3CoinVeto',title='MuonRoI_Eff_EtaVsPhi_BW3CoinVeto;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passBW3CoinVeto,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_BW3CoinVeto',title='MuonRoI_Eff_Pt_TGC_BW3CoinVeto;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_Evt_EtaVsPhi_BW3CoinVeto',type='TH2F',
                            title='MuonRoI Eta vs Phi BW3CoinVeto;MuonRoI Eta;MuonRoI Phi',cutmask='roi_bw3coinveto',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_bw3coinveto;MuonRoI_Evt_ThrVsBW3CoinVeto',type='TH2F',
                            title='MuonRoI Thr vs BW3CoinVeto;MuonRoI Thresholds;MuonRoI BW3CoinVeto',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_innercoin;MuonRoI_Evt_InnerCoin',title='MuonRoI InnerCoin Flag;InnerCoin Flag;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passInnerCoin,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_InnerCoin',title='MuonRoI_Eff_EtaVsPhi_InnerCoin;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passInnerCoin,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_InnerCoin',title='MuonRoI_Eff_Pt_TGC_InnerCoin;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_Evt_EtaVsPhi_InnerCoin',type='TH2F',
                            title='MuonRoI Eta vs Phi InnerCoin;MuonRoI Eta;MuonRoI Phi',cutmask='roi_innercoin',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_innercoin;MuonRoI_Evt_ThrVsInnerCoin',type='TH2F',
                            title='MuonRoI Thr vs InnerCoin;MuonRoI Thresholds;MuonRoI InnerCoin',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_innveto;MuonRoI_Evt_InnerCoinVeto',title='MuonRoI InnerCoinVeto Flag;InnerCoinVeto Flag;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passInnerCoinVeto,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_InnerCoinVeto',title='MuonRoI_Eff_EtaVsPhi_InnerCoinVeto;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passInnerCoinVeto,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_InnerCoinVeto',title='MuonRoI_Eff_Pt_TGC_InnerCoinVeto;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_Evt_EtaVsPhi_InnerCoinVeto',type='TH2F',
                            title='MuonRoI Eta vs Phi InnerCoinVeto;MuonRoI Eta;MuonRoI Phi',cutmask='roi_innveto',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_innveto;MuonRoI_Evt_ThrVsInnerCoinVeto',type='TH2F',
                            title='MuonRoI Thr vs InnerCoinVeto;MuonRoI Thresholds;MuonRoI InnerCoinVeto',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_goodmf;MuonRoI_Evt_GoodMF',title='MuonRoI GoodMF Flag;GoodMF Flag;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passGoodMF,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_GoodMF',title='MuonRoI_Eff_EtaVsPhi_GoodMF;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passGoodMF,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_GoodMF',title='MuonRoI_Eff_Pt_TGC_GoodMF;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_Evt_EtaVsPhi_GoodMF',type='TH2F',
                            title='MuonRoI Eta vs Phi GoodMF;MuonRoI Eta;MuonRoI Phi',cutmask='roi_goodmf',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_goodmf;MuonRoI_Evt_ThrVsGoodMF',type='TH2F',
                            title='MuonRoI Thr vs GoodMF;MuonRoI Thresholds;MuonRoI GoodMF',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_badmf;MuonRoI_Evt_BadMF',title='MuonRoI BadMF Flag;BadMF Flag;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('muon_l1passBadMF,muon_eta4gev,muon_phi4gev;MuonRoI_Eff_EtaVsPhi_BadMF',title='MuonRoI_Eff_EtaVsPhi_BadMF;Offline muon eta; Offline muon phi',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('muon_l1passBadMF,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_BadMF',title='MuonRoI_Eff_Pt_TGC_BadMF;Offline muon pT [GeV];Efficiency',
                            cutmask='muon_l1passThr1',type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_Evt_EtaVsPhi_BadMF',type='TH2F',
                            title='MuonRoI Eta vs Phi BadMF;MuonRoI Eta;MuonRoI Phi',cutmask='roi_badmf',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_badmf;MuonRoI_Evt_ThrVsBadMF',type='TH2F',
                            title='MuonRoI Thr vs BadMF;MuonRoI Thresholds;MuonRoI BadMF',cutmask='roi_tgc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_ismorecand;MuonRoI_Evt_RpcIsMoreCandInRoI',title='MuonRoI RpcIsMoreCandInRoI Flag;RpcIsMoreCandInRoI Flag;Number of events',
                            cutmask='roi_rpc',path=trigPath,xbins=2,xmin=-0.5,xmax=1.5)
    myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_Evt_EtaVsPhi_RpcIsMoreCandInRoI',type='TH2F',
                            title='MuonRoI Eta vs Phi RpcIsMoreCandInRoI;MuonRoI Eta;MuonRoI Phi',cutmask='roi_ismorecand',path=trigPath,
                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
    myGroup.defineHistogram('roi_thr,roi_ismorecand;MuonRoI_Evt_ThrVsRpcIsMoreCandInRoI',type='TH2F',
                            title='MuonRoI Thr vs RpcIsMoreCandInRoI;MuonRoI Thresholds;MuonRoI RpcIsMoreCandInRoI',cutmask='roi_rpc',path=trigPath,
                            xbins=20,xmin=-0.5,xmax=19.5,ybins=2,ymin=-0.5,ymax=1.5)

    myGroup.defineHistogram('roi_thr;MuonRoI_Evt_Thresholds_RPC',title='MuonRoI Thresholds RPC;MuonRoI Threshold number;Number of events',
                            cutmask='roi_rpc',path=trigPath,xbins=20,xmin=-0.5,xmax=19.5)
    myGroup.defineHistogram('roi_thr;MuonRoI_Evt_Thresholds_TGC',title='MuonRoI Thresholds TGC;MuonRoI Threshold number;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=20,xmin=-0.5,xmax=19.5)

    myGroup.defineHistogram('roi_lumiBlock,roi_phi_barrel;MuonRoI_Evt_PhiVsLB_Barrel_sideA',title='MuonRoI PhiVsLB Barrel sideA;Luminosity block;RoI Phi',type='TH2F',
                            cutmask='roi_sideA',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=8,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')
    myGroup.defineHistogram('roi_lumiBlock,roi_phi_barrel;MuonRoI_Evt_PhiVsLB_Barrel_sideC',title='MuonRoI PhiVsLB Barrel sideC;Luminosity block;RoI Phi',type='TH2F',
                            cutmask='roi_sideC',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=8,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')

    myGroup.defineHistogram('roi_lumiBlock,roi_phi_endcap;MuonRoI_Evt_PhiVsLB_Endcap_sideA',title='MuonRoI PhiVsLB Endcap sideA;Luminosity block;RoI Phi',type='TH2F',
                            cutmask='roi_sideA',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=48,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')
    myGroup.defineHistogram('roi_lumiBlock,roi_phi_endcap;MuonRoI_Evt_PhiVsLB_Endcap_sideC',title='MuonRoI PhiVsLB Endcap sideC;Luminosity block;RoI Phi',type='TH2F',
                            cutmask='roi_sideC',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=48,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')

    myGroup.defineHistogram('roi_lumiBlock,roi_phi_forward;MuonRoI_Evt_PhiVsLB_Forward_sideA',title='MuonRoI PhiVsLB Forward sideA;Luminosity block;RoI Phi',type='TH2F',
                            cutmask='roi_sideA',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=24,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')
    myGroup.defineHistogram('roi_lumiBlock,roi_phi_forward;MuonRoI_Evt_PhiVsLB_Forward_sideC',title='MuonRoI PhiVsLB Forward sideC;Luminosity block;RoI Phi',type='TH2F',
                            cutmask='roi_sideC',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=24,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')

    
    for n in range(1,16):

        myGroup.defineHistogram('roi_eta_wInnerCoin;MuonRoI_Evt_Eta_wInnerCoin_Thr%02d' % n,title='MuonRoI Eta wInnerCoin Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_eta_wBW3Coin;MuonRoI_Evt_Eta_wBW3Coin_Thr%02d' % n,title='MuonRoI Eta wBW3Coin Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)

        myGroup.defineHistogram('roi_eta_wInnerCoinVeto;MuonRoI_Evt_Eta_wInnerCoinVeto_Thr%02d' % n,title='MuonRoI Eta wInnerCoinVeto Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_eta_wBW3CoinVeto;MuonRoI_Evt_Eta_wBW3CoinVeto_Thr%02d' % n,title='MuonRoI Eta wBW3CoinVeto Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)

        myGroup.defineHistogram('roi_phi_wInnerCoin;MuonRoI_Evt_Phi_wInnerCoin_Thr%02d' % n,title='MuonRoI Phi wInnerCoin Thr%02d;MuonRoI Phi;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_phi_wBW3Coin;MuonRoI_Evt_Phi_wBW3Coin_Thr%02d' % n,title='MuonRoI Phi wBW3Coin Thr%02d;MuonRoI Phi;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)

        myGroup.defineHistogram('roi_phi_wInnerCoinVeto;MuonRoI_Evt_Phi_wInnerCoinVeto_Thr%02d' % n,title='MuonRoI Phi wInnerCoinVeto Thr%02d;MuonRoI Phi;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_phi_wBW3CoinVeto;MuonRoI_Evt_Phi_wBW3CoinVeto_Thr%02d' % n,title='MuonRoI Phi wBW3CoinVeto Thr%02d;MuonRoI Phi;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)

        myGroup.defineHistogram('roi_eta_wInnerCoin,roi_phi_wInnerCoin;MuonRoI_Evt_EtaVsPhi_wInnerCoin_Thr%02d' % n,title='MuonRoI Eta vs Phi wInnerCoin Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi,type='TH2F')
        myGroup.defineHistogram('roi_eta_wBW3Coin,roi_phi_wBW3Coin;MuonRoI_Evt_EtaVsPhi_wBW3Coin_Thr%02d' % n,title='MuonRoI Eta vs Phi wBW3Coin Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi,type='TH2F')

        myGroup.defineHistogram('roi_eta_wInnerCoinVeto,roi_phi_wInnerCoinVeto;MuonRoI_Evt_EtaVsPhi_wInnerCoinVeto_Thr%02d' % n,title='MuonRoI Eta vs Phi wInnerCoinVeto Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi,type='TH2F')
        myGroup.defineHistogram('roi_eta_wBW3CoinVeto,roi_phi_wBW3CoinVeto;MuonRoI_Evt_EtaVsPhi_wBW3CoinVeto_Thr%02d' % n,title='MuonRoI Eta vs Phi wBW3CoinVeto Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi,type='TH2F')

        myGroup.defineHistogram('roi_eta;MuonRoI_Evt_Eta_Thr%02d' % n,title='MuonRoI Eta Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_eta_rpc;MuonRoI_Evt_Eta_RPC_Thr%02d' % n,title='MuonRoI Eta RPC Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_eta_tgc;MuonRoI_Evt_Eta_TGC_Thr%02d' % n,title='MuonRoI Eta TGC Thr%02d;MuonRoI Eta;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_phi_rpc;MuonRoI_Evt_Phi_RPC_Thr%02d' % n,title='MuonRoI Phi RPC Thr%02d;MuonRoI Phi;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=32,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_phi_tgc;MuonRoI_Evt_Phi_TGC_Thr%02d' % n,title='MuonRoI Phi TGC Thr%02d;MuonRoI Phi;Number of events' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_Evt_EtaVsPhi_Thr%02d' % n,type='TH2F',title='MuonRoI Eta vs Phi Thr%02d;MuonRoI Eta;MuonRoI Phi' % n,
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)

        myGroup.defineHistogram('roi_innercoin,roi_eta_tgc;MuonRoI_Eff_Eta_wInnerCoin_Thr%02d' % n,title='MuonRoI Eff Eta wInnerCoin Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_innercoin,roi_phi_tgc;MuonRoI_Eff_Phi_wInnerCoin_Thr%02d' % n,title='MuonRoI Eff Phi wInnerCoin Thr%02d;MuonRoI Phi;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_innercoin,roi_eta_tgc,roi_phi_tgc;MuonRoI_Eff_EtaVsPhi_wInnerCoin_Thr%02d' % n,title='MuonRoI Eff Eta vs Phi wInnerCoin Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)

        myGroup.defineHistogram('roi_innveto,roi_eta_tgc;MuonRoI_Eff_Eta_wInnerCoinVeto_Thr%02d' % n,title='MuonRoI Eff Eta wInnerCoinVeto Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_innveto,roi_phi_tgc;MuonRoI_Eff_Phi_wInnerCoinVeto_Thr%02d' % n,title='MuonRoI Eff Phi wInnerCoinVeto Thr%02d;MuonRoI Phi;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_innveto,roi_eta_tgc,roi_phi_tgc;MuonRoI_Eff_EtaVsPhi_wInnerCoinVeto_Thr%02d' % n,title='MuonRoI Eff Eta vs Phi wInnerCoinVeto Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)

        myGroup.defineHistogram('roi_bw3coin,roi_eta_tgc;MuonRoI_Eff_Eta_wBW3Coin_Thr%02d' % n,title='MuonRoI Eff Eta wBW3Coin Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_bw3coin,roi_phi_tgc;MuonRoI_Eff_Phi_wBW3Coin_Thr%02d' % n,title='MuonRoI Eff Phi wBW3Coin Thr%02d;MuonRoI Phi;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_bw3coin,roi_eta_tgc,roi_phi_tgc;MuonRoI_Eff_EtaVsPhi_wBW3Coin_Thr%02d' % n,title='MuonRoI Eff Eta vs Phi wBW3Coin Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)

        myGroup.defineHistogram('roi_bw3coinveto,roi_eta_tgc;MuonRoI_Eff_Eta_wBW3CoinVeto_Thr%02d' % n,title='MuonRoI Eff Eta wBW3CoinVeto Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_bw3coinveto,roi_phi_tgc;MuonRoI_Eff_Phi_wBW3CoinVeto_Thr%02d' % n,title='MuonRoI Eff Phi wBW3CoinVeto Thr%02d;MuonRoI Phi;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_bw3coinveto,roi_eta_tgc,roi_phi_tgc;MuonRoI_Eff_EtaVsPhi_wBW3CoinVeto_Thr%02d' % n,title='MuonRoI Eff Eta vs Phi wBW3CoinVeto Thr%02d;MuonRoI Eta;Efficiency' % n,
                                cutmask='thrmask'+str(n),type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)


        myGroup.defineHistogram('muon_l1passThr%d,muon_pt_rpc;MuonRoI_Eff_Pt_RPC_Thr%02d' % (n,n),title='MuonRoI_Eff_Pt_RPC_Thr%02d;Offline muon pT [GeV];Efficiency' % n,
                                type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
        myGroup.defineHistogram('muon_l1passThr%d,muon_pt_tgc;MuonRoI_Eff_Pt_TGC_Thr%02d' % (n,n),title='MuonRoI_Eff_Pt_TGC_Thr%02d;Offline muon pT [GeV];Efficiency' % n,
                                type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
        myGroup.defineHistogram('muon_l1passThr%d,muon_phi_rpc;MuonRoI_Eff_Phi_RPC_Thr%02d' % (n,n),title='MuonRoI_Eff_Phi_RPC_Thr%02d;Offline muon phi [rad.];Efficiency' % n,
                                type='TEfficiency',path=trigPath,xbins=32,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('muon_l1passThr%d,muon_phi_tgc;MuonRoI_Eff_Phi_TGC_Thr%02d' % (n,n),title='MuonRoI_Eff_Phi_TGC_Thr%02d;Offline muon phi [rad.];Efficiency' % n,
                                type='TEfficiency',path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('muon_l1passThr%d,muon_eta;MuonRoI_Eff_Eta_Thr%02d' % (n,n),title='MuonRoI_Eff_Eta_Thr%02d;Offline muon eta;Efficiency' % n,
                                type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('muon_l1passThr%d,muon_eta,muon_phi;MuonRoI_Eff_EtaVsPhi_Thr%02d' % (n,n),title='MuonRoI_Eff_EtaVsPhi_Thr%02d;Offline muon eta; Offline muon phi' % n,
                                type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
        


        
    ################################################################################################################
    myGroupHit = helper.addGroup(tgcRawDataMonAlg, 'TgcRawDataMonitor_TgcHit', mainDir)
    hitPath = 'Hit/'
    hitEffPath = 'HitEff/'
    myGroupHit.defineHistogram('hit_n;TgcPrd_nHits',title='TgcPrd_nHits;Number of hits;Number of events',
                               path=hitPath,xbins=100,xmin=0,xmax=1000,opt='kAddBinsDynamically')
    myGroupHit.defineHistogram('hit_bunch;TgcPrd_Timing',title='TgcPrd_Timing;Timing;Number of events',
                               path=hitPath,xbins=4,xmin=-1.5,xmax=1.5,xlabels=['Previous','Current','Next'])
    
    for opt in ['', '_wTrack']:
        titlesuffix=opt
        if opt=='':titlesuffix='_All'
        myGroupHit.defineHistogram('mon_lb,hit_bw24sectors'+opt+';TgcPrd_BWSectorsVsLB'+titlesuffix,
                                   title='BWSectorsVsLB'+titlesuffix+';Luminosity block;TGC BW Sectors(+ for A-side, - for C-side)',type='TH2F',
                                   path=hitPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=25,ymin=-12.5,ymax=12.5,opt='kAddBinsDynamically')
        myGroupHit.defineHistogram('mon_lb,hit_bw24sectors_strip'+opt+';TgcPrd_BWSectorsVsLB_Strip'+titlesuffix,
                                   title='BWSectorsVsLB_Strip'+titlesuffix+';Luminosity block;TGC BW Sectors(+ for A-side, - for C-side)',type='TH2F',
                                   path=hitPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=25,ymin=-12.5,ymax=12.5,opt='kAddBinsDynamically')
        myGroupHit.defineHistogram('mon_lb,hit_bw24sectors_wire'+opt+';TgcPrd_BWSectorsVsLB_Wire'+titlesuffix,
                                   title='BWSectorsVsLB_Wire'+titlesuffix+';Luminosity block;TGC BW Sectors(+ for A-side, - for C-side)',type='TH2F',
                                   path=hitPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=25,ymin=-12.5,ymax=12.5,opt='kAddBinsDynamically')
        myGroupHit.defineHistogram('hit_bw24sectors'+opt+',hit_bwtiming'+opt+';TgcPrd_BWSectorsVsTiming'+titlesuffix,
                                   title='BWSectorsVsTiming'+titlesuffix+';TGC BW Sectors(+ for A-side, - for C-side);Timing',type='TH2F',
                                   path=hitPath,xbins=25,xmin=-12.5,xmax=12.5,ybins=3,ymin=-1.5,ymax=1.5,ylabels=['Previous','Current','Next'])
        myGroupHit.defineHistogram('hit_bw24sectors_strip'+opt+',hit_bwtiming_strip'+opt+';TgcPrd_BWSectorsVsTiming_Strip'+titlesuffix,
                                   title='BWSectorsVsTiming_Strip'+titlesuffix+';TGC BW Sectors(+ for A-side, - for C-side);Timing',type='TH2F',
                                   path=hitPath,xbins=25,xmin=-12.5,xmax=12.5,ybins=3,ymin=-1.5,ymax=1.5,ylabels=['Previous','Current','Next'])
        myGroupHit.defineHistogram('hit_bw24sectors_wire'+opt+',hit_bwtiming_wire'+opt+';TgcPrd_BWSectorsVsTiming_Wire'+titlesuffix,
                                   title='BWSectorsVsTiming_Wire'+titlesuffix+';TGC BW Sectors(+ for A-side, - for C-side);Timing',type='TH2F',
                                   path=hitPath,xbins=25,xmin=-12.5,xmax=12.5,ybins=3,ymin=-1.5,ymax=1.5,ylabels=['Previous','Current','Next'])
        myGroupHit.defineHistogram('hit_bwfulleta'+opt+',hit_bw24sectors'+opt+';TgcPrd_BWSectorsVsEta'+titlesuffix,
                                   title='BWSectorsVsEta'+titlesuffix+';iEta (0 for Forward, >0 for Endcap);TGC BW Sectors(+ for A-side, - for C-side)',type='TH2F',
                                   path=hitPath,xbins=6,xmin=-0.5,xmax=5.5,ybins=25,ymin=-12.5,ymax=12.5,opt='kAddBinsDynamically')
        myGroupHit.defineHistogram('hit_bwfulleta_strip'+opt+',hit_bw24sectors_strip'+opt+';TgcPrd_BWSectorsVsEta_Strip'+titlesuffix,
                                   title='BWSectorsVsEta_Strip'+titlesuffix+';iEta (0 for Forward, >0 for Endcap);TGC BW Sectors(+ for A-side, - for C-side)',type='TH2F',
                                   path=hitPath,xbins=6,xmin=-0.5,xmax=5.5,ybins=25,ymin=-12.5,ymax=12.5,opt='kAddBinsDynamically')
        myGroupHit.defineHistogram('hit_bwfulleta_wire'+opt+',hit_bw24sectors_wire'+opt+';TgcPrd_BWSectorsVsEta_Wire'+titlesuffix,
                                   title='BWSectorsVsEta_Wire'+titlesuffix+';iEta (0 for Forward, >0 for Endcap);TGC BW Sectors(+ for A-side, - for C-side)',type='TH2F',
                                   path=hitPath,xbins=6,xmin=-0.5,xmax=5.5,ybins=25,ymin=-12.5,ymax=12.5,opt='kAddBinsDynamically')



    myGroupHit.defineHistogram('hit_bcmask_bw24sectors_All,hit_bcmask_for_bw24sectors_All;TgcPrd_BWSectorsVsBCMask_All',
                               title='BWSectorsVsBCMask_All;TGC BW Sectors(+ for A-side, - for C-side);',type='TH2F',
                               path=hitPath,xbins=25,xmin=-12.5,xmax=12.5,ybins=8,ymin=-0.5,ymax=8.5,ylabels=['no hits','Next-only','Curr-only','Curr&Next','Prev-only','Prev&Next','Prev&Curr','Prev&Curr&Next'])
    myGroupHit.defineHistogram('hit_bcmask_bw24sectors_Wire,hit_bcmask_for_bw24sectors_Wire;TgcPrd_BWSectorsVsBCMask_Wire',
                               title='BWSectorsVsBCMask_Wire;TGC BW Sectors(+ for A-side, - for C-side);',type='TH2F',
                               path=hitPath,xbins=25,xmin=-12.5,xmax=12.5,ybins=8,ymin=-0.5,ymax=8.5,ylabels=['no hits','Next-only','Curr-only','Curr&Next','Prev-only','Prev&Next','Prev&Curr','Prev&Curr&Next'])
    myGroupHit.defineHistogram('hit_bcmask_bw24sectors_Strip,hit_bcmask_for_bw24sectors_Strip;TgcPrd_BWSectorsVsBCMask_Strip',
                               title='BWSectorsVsBCMask_Strip;TGC BW Sectors(+ for A-side, - for C-side);',type='TH2F',
                               path=hitPath,xbins=25,xmin=-12.5,xmax=12.5,ybins=8,ymin=-0.5,ymax=8.5,ylabels=['no hits','Next-only','Curr-only','Curr&Next','Prev-only','Prev&Next','Prev&Curr','Prev&Curr&Next'])


    
    for side in ['A', 'C']:# side-A or side-C
        for station in range(1,5):# M1,2,3,4
            for s_or_w in ['S','W']:# strip or wire
                name = "%sM%02i%s" % (side,station,s_or_w) # e.g. AM01W
                nbins = 10
                if station==1:
                    nbins = 720
                elif station==2 or station==3:
                    nbins = 576
                else: # station==4
                    nbins = 96

                x_name = "mon_lb"
                y_name = "hit_glblphi_%s" % (name)
                objname = "TgcPrd_GlobalChamberIndexVsLB_%s" % (name)
                title = "GlobalChamberIndexVsLB_%s" % (name)
                myGroupHit.defineHistogram(x_name+','+y_name+';'+objname,
                                        title=title+';Luminosity block;Global chamber index',type='TH2F',
                                        path=hitPath,xbins=100,xmin=-0.5,xmax=99.5,
                                        ybins=nbins,ymin=0.5,ymax=nbins+0.5,opt='kAddBinsDynamically')
                x_name = "hit_glblphi_%s" % (name)
                y_name = "hit_bunch_%s" % (name)
                objname = "TgcPrd_GlobalChamberIndexVsTiming_All_%s" % (name)
                title = "GlobalChamberIndexVsTiming_All_%s" % (name)
                myGroupHit.defineHistogram(x_name+','+y_name+';'+objname,
                                           title=title+';Global chamber index;Timing',type='TH2F',
                                           path=hitPath,xbins=nbins,xmin=0.5,xmax=nbins+0.5,
                                           ybins=3,ymin=-1.5,ymax=1.5,ylabels=['Previous','Current','Next'])
                x_name = "hit_glblphi_wTrack_%s" % (name)
                y_name = "hit_bunch_wTrack_%s" % (name)
                objname = "TgcPrd_GlobalChamberIndexVsTiming_wTrack_%s" % (name)
                title = "GlobalChamberIndexVsTiming_wTrack_%s" % (name)
                myGroupHit.defineHistogram(x_name+','+y_name+';'+objname,
                                           title=title+';Global chamber index;Timing',type='TH2F',
                                           path=hitPath,xbins=nbins,xmin=0.5,xmax=nbins+0.5,
                                           ybins=3,ymin=-1.5,ymax=1.5,ylabels=['Previous','Current','Next'])
                
                x_name = "hit_glblphi_effnum_%s" % (name)
                y_name = "hit_glblphi_effden_%s" % (name)
                objname = "TgcPrd_GlobalChamberIndex_Efficiency_%s" % (name)
                title = "GlobalChamberIndex_Efficiency_%s" % (name)
                myGroupHit.defineHistogram(x_name+','+y_name+';'+objname,
                                           title=title+';Global chamber index;Efficiency',type='TEfficiency',
                                           path=hitEffPath,xbins=nbins,xmin=0.5,xmax=nbins+0.5)

                x_name = "hit_bcmask_glblphi_%s" % (name)
                y_name = "hit_bcmask_%s" % (name)
                objname = "TgcPrd_GlobalChamberIndexVsBCMask_%s" % (name)
                title = "GlobalChamberIndexVsBCMask_%s" % (name)
                myGroupHit.defineHistogram(x_name+','+y_name+';'+objname,
                                           title=title+';Global chamber index;',type='TH2F',
                                           path=hitPath,xbins=nbins,xmin=0.5,xmax=nbins+0.5,
                                           ybins=8,ymin=-0.5,ymax=8.5,ylabels=['no hits',
                                                                               'Next-only',
                                                                               'Curr-only',
                                                                               'Curr&Next',
                                                                               'Prev-only',
                                                                               'Prev&Next',
                                                                               'Prev&Curr',
                                                                               'Prev&Curr&Next'])


                nbinsx = 10
                nbinsy = 10
                if station==1:
                    nbinsx = 15
                    nbinsy = 48
                elif station==2 or station==3:
                    nbinsx = 12
                    nbinsy = 48
                else: # station==4
                    nbinsx = 4
                    nbinsy = 24

                x_name = "hit_x_%s" % (name)
                y_name = "hit_y_%s" % (name)
                objname = "TgcPrd_iPhiVsiEta_%s" % (name)
                title = "iPhiVsiEta_%s" % (name)
                myGroupHit.defineHistogram(x_name+','+y_name+';'+objname,
                                           title=title+';Chamber eta index;Chamber phi index',type='TH2F',path=hitPath,
                                           xbins=nbinsx,xmin=0.5,xmax=nbinsx+0.5,
                                           ybins=nbinsy,ymin=0.5,ymax=nbinsy+0.5)

                eff_name = "hit_effnum_x_%s" % (name)
                x_name = "hit_effden_x_%s" % (name)
                y_name = "hit_effden_y_%s" % (name)
                objname = "TgcPrd_iPhiVsiEta_Efficiency_%s" % (name)
                title = "iPhiVsiEta_Efficiency_%s" % (name)
                myGroupHit.defineHistogram(eff_name+','+x_name+','+y_name+';'+objname,
                                           title=title+';Chamber eta index;Chamber phi index',type='TEfficiency',
                                           path=hitEffPath,
                                           xbins=nbinsx,xmin=0.5,xmax=nbinsx+0.5,
                                           ybins=nbinsy,ymin=0.5,ymax=nbinsy+0.5)



    for side in ['A', 'C']:# side-A or side-C
        for sector in range(0,16):# Sector 00...15 (00 for FI, 01..15 for EI, 01..12 for BW)
            for station in range(1,5):# M1,2,3,4
                for phi in range(25):# internal phi 0,1,2,3...24 (0..3 for BW and EI, 1..24 for FI)
                    for eta in range(6):# eta index 1,,,5 for Endcap, and 0 for Forward
                        for lay in range(1,4):# sub-layer 1,2,3 (triplet) or 1,2 (doublet)
                            if station<4 and (sector>12 or sector==0):continue # BW only 1..12 sectors
                            if station==4 and eta==0 and sector!=0:continue # FI only sector-0
                            if station==4 and eta==1 and sector%2==0:continue # EI only odd-sectors (1,3,5,7..15)
                            if station==4 and eta==0 and phi==0:continue # FI only 1..24 internal phi
                            if (station<4 or (station==4 and eta==1)) and phi>3: continue # BW and EI only 0..3 internal phi
                            if station==1 and eta==5:continue # BW M1 has only 1..4 eta
                            if station==4 and eta>1:continue # EI/FI eta 0 or 1
                            if station>1 and lay>2:continue # triplet is only M1
                            for s_or_w in ['S','W']:# strip or wire
                                if station==1 and lay==2 and s_or_w=="S":continue # there is no strip channel on M1 L2
                                nbins = 100
                                if s_or_w=="S":
                                    nbins = 32
                                else:
                                    if station==1:
                                        if eta==1:     nbins = 24
                                        elif eta==2:   nbins = 23
                                        elif eta==3:
                                            if lay==1: nbins = 61
                                            else:      nbins = 62
                                        elif eta==4:
                                            if lay==1: nbins = 92
                                            else:      nbins = 91
                                        else: # forward
                                            if lay==2: nbins = 104
                                            else:      nbins = 105
                                    elif station==2:
                                        if eta==1 or eta==2 or eta==3: nbins = 32
                                        elif eta==4:                   nbins = 103
                                        elif eta==5:                   nbins = 110
                                        else:                          nbins = 125 # forward
                                    elif station==3:
                                        if eta==1:   nbins = 31
                                        elif eta==2: nbins = 30
                                        elif eta==3: nbins = 32
                                        elif eta==4: nbins = 106
                                        elif eta==5: nbins = 96
                                        else:        nbins = 122
                                    else: # EI/FI
                                        if eta==1:
                                            if (sector==1 and phi==0) or (sector==1 and phi==2) or (sector==9 and phi==0) or (sector==9 and phi==2) or sector==11 or sector==15:
                                                nbins = 16 # EI short
                                            else:
                                                nbins = 24 # EI
                                        else:        nbins = 32 # FI
                                
                                if doGapByGapHitOcc:
                                    chamber_name = "%s%02dM%02df%02d%s%02dL%02d%s" % (side,sector,station,phi,'F' if eta==0 else 'E',eta,lay,s_or_w)
                                    objname = 'TgcPrd_ChannelOccupancy_'+ chamber_name
                                    title = 'Occ_'+chamber_name
                                    myGroupHit.defineHistogram('hit_on_'+chamber_name+';'+objname,
                                                               title=title+';Channel ID;Number of events',
                                                               path="GapByGapHits/",xbins=nbins,xmin=0.5,xmax=nbins+0.5)

                                if doGapByGapEffMap:
                                    objname = 'TgcPrd_GapByGapEffMap_' + chamber_name
                                    title = 'Eff_' + chamber_name
                                    objectx = 'hit_localX_effden_' + chamber_name
                                    objecty = 'hit_localY_effden_' + chamber_name
                                    objectz = 'hit_effnum_' + chamber_name
                                    myGroupHit.defineHistogram(objectz+','+objectx+','+objecty+';'+objname,
                                                               title=title+';X [mm];Y [mm]',type='TEfficiency',
                                                               path="GapByGapEffMap/",xbins=100,xmin=-1000,xmax=1000,ybins=100,ymin=-1500,ymax=1500)

                                if lay == 1 and doHitResiduals:
                                    chamber_name = "%s%02dM%02df%02d%s%02d%s" % (side,sector,station,phi,'F' if eta==0 else 'E',eta,s_or_w)
                                    objname = 'TgcPrd_Residual_' + chamber_name
                                    title = 'Res_'+chamber_name
                                    myGroupHit.defineHistogram('hit_residual_on_'+chamber_name+';'+objname,
                                                               title=title+';Residual [mm];Number of events',
                                                               path="Residuals/",xbins=400,xmin=-1000,xmax=1000)






    ################################################################################################################
    myGroupCoin = helper.addGroup(tgcRawDataMonAlg, 'TgcRawDataMonitor_TgcCoin', mainDir)
    coinPath = 'Coin/'
    myGroupCoin.defineHistogram('nTgcCoinDetElementIsNull;h_nTgcCoinDetElementIsNull',title='nTgcCoinDetElementIsNull',
                            path=coinPath,xbins=101,xmin=-0.5,xmax=100.5)
    myGroupCoin.defineHistogram('nTgcCoinPostOutPtrIsNull;h_nTgcCoinPostOutPtrIsNull',title='nTgcCoinPostOutPtrIsNull',
                            path=coinPath,xbins=101,xmin=-0.5,xmax=100.5)
    for coinType in ['SL','HPT','LPT']:
        for region in ['','Endcap','Forward']:
            suffix0 = coinType if region == '' else coinType+'_'+region
            nrois = 148 if region == 'Endcap' else 64
            nsectors = 48 if region == 'Endcap' else 24
            for chanType in ['','Wire','Strip']:
                if coinType == 'SL' and chanType != '': continue # no wire or strip for "SL"
                if coinType == 'HPT' and chanType == '': continue # always wire or strip for "HPT"
                if coinType == 'LPT' and chanType == '': continue # always wire or strip for "LPT"
                suffix = suffix0+'_' if chanType == '' else suffix0+'_'+chanType+'_'

                if coinType == 'SL':
                    for thr in range(1,16): #1.2...15
                        cut = suffix+"coin_cutmask_pt"+str(thr)
                        PT = "_Thr%02d" % thr
                        if region == '':
                            myGroupCoin.defineHistogram(suffix+'coin_eta,'+suffix+'coin_phi;'+suffix+'TgcCoin_Evt_EtaVsPhi'+PT,
                                                    title=suffix+'TgcCoin_Evt_EtaVsPhi'+PT+';Eta;Phi',
                                                    type='TH2F',path=coinPath,cutmask=cut,
                                                    xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
                        else:
                            myGroupCoin.defineHistogram(suffix+'coin_roi,'+suffix+'coin_sector;'+suffix+'TgcCoin_Evt_SectorVsRoI'+PT,
                                                    title=suffix+'TgcCoin_Evt_SectorVsRoI'+PT+';RoI;Trigger Sector',
                                                    type='TH2F',path=coinPath,cutmask=cut,
                                                    xbins=nrois,xmin=-0.5,xmax=nrois-0.5,
                                                    ybins=nsectors*2+1,ymin=-nsectors-0.5,ymax=nsectors+0.5)

                if region == '':
                    myGroupCoin.defineHistogram(suffix+'coin_eta,'+suffix+'coin_phi;'+suffix+'TgcCoin_Evt_EtaVsPhi',
                                            title=suffix+'TgcCoin_Evt_EtaVsPhi;Eta;Phi',
                                            type='TH2F',path=coinPath,
                                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
                    myGroupCoin.defineHistogram(suffix+'coin_ext_matched,'+suffix+'coin_ext_eta,'+suffix+'coin_ext_phi;'+suffix+'TgcCoin_Eff_EtaVsPhi',
                                            title=suffix+'TgcCoin_Eff_EtaVsPhi;Eta;Phi',
                                            type='TEfficiency',path=coinPath,
                                            xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
                else:
                    myGroupCoin.defineHistogram(suffix+'coin_sector,'+suffix+'coin_bunch;'+suffix+'TgcCoin_Evt_SectorVsTiming',
                                            title=suffix+'TgcCoin_Evt_SectorVsTiming;Trigger Sector;Timing',
                                            type='TH2F',path=coinPath,
                                            xbins=nsectors*2+1,xmin=-nsectors-0.5,xmax=nsectors+0.5,
                                            ybins=3,ymin=-1.5,ymax=1.5,ylabels=['Previous','Current','Next'])
                    myGroupCoin.defineHistogram(suffix+'coin_lb,'+suffix+'coin_sector;'+suffix+'TgcCoin_Evt_SectorVsLB',
                                            title=suffix+'TgcCoin_Evt_SectorVsLB;LumiBlock;Trigger Sector',
                                            type='TH2F',path=coinPath,
                                            xbins=100,xmin=0.5,xmax=100.5,opt='kAddBinsDynamically',
                                            ybins=nsectors*2+1,ymin=-nsectors-0.5,ymax=nsectors+0.5)
                    if coinType == 'SL':
                        myGroupCoin.defineHistogram(suffix+'coin_roi,'+suffix+'coin_sector;'+suffix+'TgcCoin_Evt_SectorVsRoI',
                                                title=suffix+'TgcCoin_Evt_SectorVsRoI;RoI;Trigger Sector',
                                                type='TH2F',path=coinPath,
                                                xbins=nrois,xmin=-0.5,xmax=nrois-0.5,
                                                ybins=nsectors*2+1,ymin=-nsectors-0.5,ymax=nsectors+0.5)
                        myGroupCoin.defineHistogram(suffix+'coin_veto_roi,'+suffix+'coin_veto_sector;'+suffix+'TgcCoin_Evt_SectorVsRoI_wVeto',
                                                title=suffix+'TgcCoin_Evt_SectorVsRoI_wVeto;RoI;Trigger Sector',
                                                type='TH2F',path=coinPath,
                                                xbins=nrois,xmin=-0.5,xmax=nrois-0.5,
                                                ybins=nsectors*2+1,ymin=-nsectors-0.5,ymax=nsectors+0.5)
                        myGroupCoin.defineHistogram(suffix+'coin_sector,'+suffix+'coin_pt;'+suffix+'TgcCoin_Evt_SectorVsThr',
                                                title=suffix+'TgcCoin_Evt_SectorVsThr;Trigger Sector;Threshold',
                                                type='TH2F',path=coinPath,
                                                xbins=nsectors*2+1,xmin=-nsectors-0.5,xmax=nsectors+0.5,
                                                ybins=15,ymin=0.5,ymax=15.5)
                        myGroupCoin.defineHistogram(suffix+'coin_sector,'+suffix+'coin_isPositiveDeltaR;'+suffix+'TgcCoin_Evt_SectorVsisPositiveDeltaR',
                                                title=suffix+'TgcCoin_Evt_SectorVsisPositiveDeltaR;Trigger Sector;isPositiveDeltaR',
                                                type='TH2F',path=coinPath,
                                                xbins=nsectors*2+1,xmin=-nsectors-0.5,xmax=nsectors+0.5,
                                                ybins=2,ymin=-0.5,ymax=1.5,ylabels=['Negative','Positive'])



    ################################################################################################################
    acc = helper.result()
    result.merge(acc)
    return result
    
if __name__=='__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Input.isMC = True

    import glob
    import sys
    if len(sys.argv) == 3:
        inputs = sys.argv[1].split(',')
        ConfigFlags.Input.Files = inputs
        ConfigFlags.Output.HISTFileName = sys.argv[2]
    else:
        inputs = glob.glob('/data01/data/valid1.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.ESD.e5112_s3765_r12697/ESD.*.pool.root.1')
        ConfigFlags.Input.Files = inputs
        ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput.root'

    ConfigFlags.Trigger.triggerConfig = "FILE"
    ConfigFlags.Trigger.triggerMenuSetup = "Dev_pp_run3_v1"

    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    tgcRawDataMonitorAcc = TgcRawDataMonitoringConfig(ConfigFlags)
    cfg.merge(tgcRawDataMonitorAcc)
    cfg.getEventAlgo('TgcRawDataMonAlg').OutputLevel = INFO
    cfg.getEventAlgo('TgcRawDataMonAlg').MonitorTriggerMultiplicity = True
    cfg.getEventAlgo('TgcRawDataMonAlg').MonitorThresholdPatterns = True
    cfg.getEventAlgo('TgcRawDataMonAlg').TagAndProbe = True
    cfg.getEventAlgo('TgcRawDataMonAlg').TgcRawDataMonitorTool.OutputLevel = INFO
    cfg.getEventAlgo('TgcRawDataMonAlg').FillGapByGapHistograms = True
    

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    cfg.merge(MagneticFieldSvcCfg(ConfigFlags))
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    cfg.merge(AtlasGeometryCfg(ConfigFlags))
    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg,generateL1Menu
    cfg.merge(L1ConfigSvcCfg(ConfigFlags))
    generateL1Menu(ConfigFlags)

    cfg.printConfig(withDetails=True, summariseProps = True)

    cfg.run()

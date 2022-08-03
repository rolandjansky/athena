#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import ROOT
from TrigMuonMonitoring.TrigMuonEfficiencyMonConfig import regex

def L2MuonSAIOMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    GroupName = 'L2MuonSAIO'

    from MuonSelectorTools.MuonSelectorToolsConfig import MuonSelectionToolCfg
    monAlg = helper.addAlgorithm(CompFactory.L2MuonSAIOMon,'L2MuonSAIOMon',
                                 MuonSelectionTool = helper.result().popToolsAndMerge(MuonSelectionToolCfg(helper.inputFlags, MuQuality=1)))

    ### monitorig groups
    from TrigConfigSvc.TriggerConfigAccess import getHLTMonitoringAccess
    moniAccess = getHLTMonitoringAccess(helper.inputFlags)
    Chains = moniAccess.monitoredChains(signatures="muonMon",monLevels=["shifter","t0","val"])
    monAlg.MonitoredChains = [c for c in Chains if ('l2io' in c)] 

    # if mon groups not found fall back to hard-coded trigger monitoring list
    if len(monAlg.MonitoredChains) == 0:
      monAlg.MonitoredChains = ['HLT_mu4_l2io_L1MU3V', 'HLT_mu24_ivarmedium_mu6_l2io_probe_L1MU14FCH']

    monAlg.Group = GroupName

    for chain in monAlg.MonitoredChains:

        histGroup = helper.addGroup(monAlg, GroupName+'_'+chain, 'HLT/MuonMon/L2MuonSAIO/'+chain)
        if chain == 'HLT_mu24_ivarmedium_mu6_l2io_probe_L1MU14FCH':
            threshold, level1 = regex('HLT_.*_mu([0-9]+)_l2io.*_(L1MU[0-9]+)').match(chain).groups()
        else:
            threshold, level1 = regex('HLT_mu([0-9]+).*_(L1MU[0-9]+)').match(chain).groups()
        monAlg.Plateaus.append( float(threshold) )


        # L2Inside-Out efficiency
        if chain == 'HLT_mu24_ivarmedium_mu6_l2io_probe_L1MU14FCH':
            histGroup.defineHistogram(chain+'_passL2InsideOut,'+chain+'_offdR;EffL2InsideOut_offdR',
                                      title='L2Inside-Out Efficiency dR of offline muon pair '+chain+';dR;Efficiency',
                                      cutmask=chain+'_passL2SA',
                                      type='TEfficiency', path='',xbins=100,xmin=0.,xmax=3.)

            histGroup.defineHistogram(chain+'_passL2InsideOut,'+chain+'_offPt;EffL2InsideOut_offPt',
                                      title='L2Inside-Out Efficiency pT of offline muon '+chain+';p_{T} [GeV/c];Efficiency',
                                      cutmask=chain+'_passL2SA',
                                      type='TEfficiency', path='',xbins=104,xmin=-52.,xmax=52.)

            histGroup.defineHistogram(chain+'_passL2InsideOut,'+chain+'_offEta;EffL2InsideOut_offEta',
                                      title='L2Inside-Out Efficiency Eta of offline muon '+chain+';#eta;Efficiency',
                                      cutmask=chain+'_passL2SA',
                                      type='TEfficiency', path='',xbins=27,xmin=-2.7,xmax=2.7)

            histGroup.defineHistogram(chain+'_passL2InsideOut,'+chain+'_offPhi;EffL2InsideOut_offPhi',
                                      title='L2Inside-Out Efficiency Phi of offline muon '+chain+';#phi;Efficiency',
                                      cutmask=chain+'_passL2SA',
                                      type='TEfficiency', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

        else:
            # basic EDM variables
            histGroup.defineHistogram(chain+'_Pt;L2CBIO_Pt_Barrel',
                                      title='L2MuonCBIO Pt Barrel '+chain+';p_{T} [GeV];Events',
                                      cutmask=chain+'_isBarrel',
                                      type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

            histGroup.defineHistogram(chain+'_Pt;L2CBIO_Pt_Endcap',
                                      title='L2MuonCBIO Pt Endcap '+chain+';p_{T} [GeV];Events',
                                      cutmask=chain+'_isEndcap',
                                      type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

            histGroup.defineHistogram(chain+'_Eta;L2CBIO_Eta',
                                      title='L2MuonCBIO Eta '+chain+';#eta;Events',
                                      type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

            histGroup.defineHistogram(chain+'_Phi;L2CBIO_Phi_Barrel',
                                      title='L2MuonCBIO Phi Barrel '+chain+';#phi;Events',
                                      cutmask=chain+'_isBarrel',
                                      type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

            histGroup.defineHistogram(chain+'_Phi;L2CBIO_Phi_Endcap',
                                      title='L2MuonCBIO Phi Endcap '+chain+';#phi;Events',
                                      cutmask=chain+'_isEndcap',
                                      type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

            # Comparison to Offline
            # basic variables
            histGroup.defineHistogram(chain+'_Pt_wrt_offline;L2CBIO_Pt_matched_to_offline_Barrel',
                                      title='L2MuonCBIO Pt matched to Offline muons Barrel '+chain+';p_{T} [GeV];Events',
                                      cutmask=chain+'_isBarrel',
                                      type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

            histGroup.defineHistogram(chain+'_Pt_wrt_offline;L2CBIO_Pt_matched_to_offline_Endcap',
                                      title='L2MuonCBIO Pt matched to Offline muons Endcap '+chain+';p_{T} [GeV];Events',
                                      cutmask=chain+'_isEndcap',
                                      type='TH1F', path='',xbins=210,xmin=-105.,xmax=105.)

            histGroup.defineHistogram(chain+'_Eta_wrt_offline;L2CBIO_wrt_Eta_matched_to_offline',
                                      title='L2MuonCBIO matched to Offline muons Eta '+chain+';#eta;Events',
                                      type='TH1F', path='',xbins=108,xmin=-2.7,xmax=2.7)

            histGroup.defineHistogram(chain+'_Phi_wrt_offline;L2CBIO_Phi_matched_to_offline_Barrel',
                                      title='L2MuonCBIO Phi matched to Offline muons Barrel '+chain+';#phi;Events',
                                      cutmask=chain+'_isBarrel',
                                      type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

            histGroup.defineHistogram(chain+'_Phi_wrt_offline;L2CBIO_Phi_matched_to_offline_Endcap',
                                      title='L2MuonCBIO Phi matched to Offline muons Endcap '+chain+';#phi;Events',
                                      cutmask=chain+'_isEndcap',
                                      type='TH1F', path='',xbins=96,xmin=-ROOT.TMath.Pi(),xmax=ROOT.TMath.Pi())

            # dRmin b/w cbio track & offline track at IP
            histGroup.defineHistogram(chain+'_dRmin;L2CBIO_dR_toOffl_Barrel',
                                      title='dR between L2CBIO and Offline Barrel '+chain+';#DeltaR;Events',
                                      cutmask=chain+'_isBarrel',
                                      type='TH1F', path='',xbins=100,xmin=0.,xmax=0.005)

            histGroup.defineHistogram(chain+'_dRmin;L2CBIO_dR_toOffl_Endcap',
                                      title='dR between L2CBIO and Offline Endcap '+chain+';#DeltaR;Events',
                                      cutmask=chain+'_isEndcap',
                                      type='TH1F', path='',xbins=100,xmin=0.,xmax=0.005)

            # distance b/w FTFroad & offline segment
            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Inn_Barrel;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Inn_barrel_pT4_6',
                                      title='distance b/w FTFroad and Offline segment Inner Barrel (pT4GeV-6GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_pt4to6',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Mid_Barrel;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Mid_barrel_pT4_6',
                                      title='distance b/w FTFroad and Offline segment Middle Barrel (pT4GeV-6GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_pt4to6',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Out_Barrel;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Out_barrel_pT4_6',
                                      title='distance b/w FTFroad and Offline segment Outer Barrel (pT4GeV-6GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_pt4to6',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Inn_Barrel;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Inn_barrel_pT6_8',
                                      title='distance b/w FTFroad and Offline segment Inner Barrel (pT6GeV-8GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_pt6to8',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Mid_Barrel;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Mid_barrel_pT6_8',
                                      title='distance b/w FTFroad and Offline segment Middle Barrel (pT6GeV-8GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_pt6to8',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Out_Barrel;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Out_barrel_pT6_8',
                                      title='distance b/w FTFroad and Offline segment Outer Barrel (pT6GeV-8GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_pt6to8',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Inn_Barrel;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Inn_barrel_pT8_x',
                                      title='distance b/w FTFroad and Offline segment Inner Barrel (pT over 8GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_ptover8',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Mid_Barrel;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Mid_barrel_pT8_x',
                                      title='distance b/w FTFroad and Offline segment Middle Barrel (pT over 8GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_ptover8',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Out_Barrel;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Out_barrel_pT8_x',
                                      title='distance b/w FTFroad and Offline segment Outer Barrel (pT over 8GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_ptover8',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Inn_Endcap;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Inn_endcap_pT4_6',
                                      title='distance b/w FTFroad and Offline segment Inner Endcap (pT4GeV-6GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_pt4to6',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Mid_Endcap;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Mid_endcap_pT4_6',
                                      title='distance b/w FTFroad and Offline segment Middle Endcap (pT4GeV-6GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_pt4to6',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Out_Endcap;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Out_endcap_pT4_6',
                                      title='distance b/w FTFroad and Offline segment Outer Endcap (pT4GeV-6GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_pt4to6',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Inn_Endcap;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Inn_endcap_pT6_8',
                                      title='distance b/w FTFroad and Offline segment Inner Endcap (pT6GeV-8GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_pt6to8',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Mid_Endcap;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Mid_endcap_pT6_8',
                                      title='distance b/w FTFroad and Offline segment Middle Endcap (pT6GeV-8GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_pt6to8',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Out_Endcap;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Out_endcap_pT6_8',
                                      title='distance b/w FTFroad and Offline segment Outer Endcap (pT6GeV-8GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_pt6to8',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Inn_Endcap;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Inn_endcap_pT8_x',
                                      title='distance b/w FTFroad and Offline segment Inner Endcap (pT over 8GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_ptover8',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Mid_Endcap;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Mid_endcap_pT8_x',
                                      title='distance b/w FTFroad and Offline segment Middle Endcap (pT over 8GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_ptover8',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            histGroup.defineHistogram(chain+'_distance_bw_FTFroad_and_offlinesegment_Out_Endcap;L2MuonSAIO_distance_bw_FTFroad_and_offlinesegment_Out_endcap_pT8_x',
                                      title='distance b/w FTFroad and Offline segment Outer Endcap (pT over 8GeV) '+chain+';distance (mm);Events',
                                      cutmask=chain+'_ptover8',
                                      type='TH1F', path='',xbins=100,xmin=-1000.,xmax=1000.)

            # number of MDT hits per L2SAIO track
            histGroup.defineHistogram(chain+'_MDT_N_Inn_Barrel;L2MuonSAIO_MDT_N_Inn_Barrel',
                                      title='L2MuonSAIO MDT number of hits Inner Barrel '+chain+';MDT nhits;Events',
                                      type='TH1I', path='',xbins=9,xmin=0,xmax=9)

            histGroup.defineHistogram(chain+'_MDT_N_Mid_Barrel;L2MuonSAIO_MDT_N_Mid_Barrel',
                                      title='L2MuonSAIO MDT number of hits Middle Barrel '+chain+';MDT nhits;Events',
                                      type='TH1I', path='',xbins=9,xmin=0,xmax=9)

            histGroup.defineHistogram(chain+'_MDT_N_Out_Barrel;L2MuonSAIO_MDT_N_Out_Barrel',
                                      title='L2MuonSAIO MDT number of hits Outer Barrel '+chain+';MDT nhits;Events',
                                      type='TH1I', path='',xbins=9,xmin=0,xmax=9)

            histGroup.defineHistogram(chain+'_MDT_N_Inn_Endcap;L2MuonSAIO_MDT_N_Inn_Endcap',
                                      title='L2MuonSAIO MDT number of hits Inner Endcap '+chain+';MDT nhits;Events',
                                      type='TH1I', path='',xbins=9,xmin=0,xmax=9)

            histGroup.defineHistogram(chain+'_MDT_N_Mid_Endcap;L2MuonSAIO_MDT_N_Mid_Endcap',
                                      title='L2MuonSAIO MDT number of hits Middle Endcap '+chain+';MDT nhits;Events',
                                      type='TH1I', path='',xbins=9,xmin=0,xmax=9)

            histGroup.defineHistogram(chain+'_MDT_N_Out_Endcap;L2MuonSAIO_MDT_N_Out_Endcap',
                                      title='L2MuonSAIO MDT number of hits Outer Endcap '+chain+';MDT nhits;Events',
                                      type='TH1I', path='',xbins=9,xmin=0,xmax=9)

            # MDT hit residual
            histGroup.defineHistogram(chain+'_MDT_residual_Inn_Barrel;L2MuonSAIO_MDT_residual_Inn_Barrel',
                                      title='L2MuonSAIO MDT residual Inner Barrel '+chain+';MDT residual Inner Barrel [mm];Events',
                                      type='TH1F', path='',xbins=150,xmin=-750.,xmax=750.)

            histGroup.defineHistogram(chain+'_MDT_residual_Mid_Barrel;L2MuonSAIO_MDT_residual_Mid_Barrel',
                                      title='L2MuonSAIO MDT residual Middle Barrel '+chain+';MDT residual Middle Barrel [mm];Events',
                                      type='TH1F', path='',xbins=150,xmin=-750.,xmax=750.)

            histGroup.defineHistogram(chain+'_MDT_residual_Out_Barrel;L2MuonSAIO_MDT_residual_Out_Barrel',
                                      title='L2MuonSAIO MDT residual Outer Barrel '+chain+';MDT residual Outer Barrel [mm];Events',
                                      type='TH1F', path='',xbins=150,xmin=-750.,xmax=750.)

            histGroup.defineHistogram(chain+'_MDT_residual_Inn_Endcap;L2MuonSAIO_MDT_residual_Inn_Endcap',
                                      title='L2MuonSAIO MDT residual Inner Endcap '+chain+';MDT residual Inner Endcap [mm];Events',
                                      type='TH1F', path='',xbins=150,xmin=-750.,xmax=750.)

            histGroup.defineHistogram(chain+'_MDT_residual_Mid_Endcap;L2MuonSAIO_MDT_residual_Mid_Endcap',
                                      title='L2MuonSAIO MDT residual Middle Endcap '+chain+';MDT residual Middle Endcap [mm];Events',
                                      type='TH1F', path='',xbins=150,xmin=-750.,xmax=750.)

            histGroup.defineHistogram(chain+'_MDT_residual_Out_Endcap;L2MuonSAIO_MDT_residual_Out_Endcap',
                                      title='L2MuonSAIO MDT residual Outer Endcap '+chain+';MDT residual Outer Endcap [mm];Events',
                                      type='TH1F', path='',xbins=150,xmin=-750.,xmax=750.)

            # distance b/w MDT hits & offline segment
            histGroup.defineHistogram(chain+'_distance_bw_MDT_and_offlinesegment_Inn_Barrel;L2MuonSAIO_distance_bw_MDT_and_offlinesegment_Inn_Barrel',
                                      title='L2MuonSAIO distance b/w MDT and Offline segment Inner Barrel '+chain+';distance [mm];Events',
                                      type='TH1F', path='',xbins=100,xmin=-750.,xmax=750.)

            histGroup.defineHistogram(chain+'_distance_bw_MDT_and_offlinesegment_Mid_Barrel;L2MuonSAIO_distance_bw_MDT_and_offlinesegment_Mid_Barrel',
                                      title='L2MuonSAIO distance b/w MDT and Offline segment Middle Barrel '+chain+';distance [mm];Events',
                                      type='TH1F', path='',xbins=100,xmin=-750.,xmax=750.)

            histGroup.defineHistogram(chain+'_distance_bw_MDT_and_offlinesegment_Out_Barrel;L2MuonSAIO_distance_bw_MDT_and_offlinesegment_Out_Barrel',
                                      title='L2MuonSAIO distance b/w MDT and Offline segment Outer Barrel '+chain+';distance [mm];Events',
                                      type='TH1F', path='',xbins=100,xmin=-750.,xmax=750.)

            histGroup.defineHistogram(chain+'_distance_bw_MDT_and_offlinesegment_Inn_Endcap;L2MuonSAIO_distance_bw_MDT_and_offlinesegment_Inn_Endcap',
                                      title='L2MuonSAIO distance b/w MDT and Offline segment Inner Endcap '+chain+';distance [mm];Events',
                                      type='TH1F', path='',xbins=100,xmin=-750.,xmax=750.)

            histGroup.defineHistogram(chain+'_distance_bw_MDT_and_offlinesegment_Mid_Endcap;L2MuonSAIO_distance_bw_MDT_and_offlinesegment_Mid_Endcap',
                                      title='L2MuonSAIO distance b/w MDT and Offline segment Middle Endcap '+chain+';distance [mm];Events',
                                      type='TH1F', path='',xbins=100,xmin=-750.,xmax=750.)

            histGroup.defineHistogram(chain+'_distance_bw_MDT_and_offlinesegment_Out_Endcap;L2MuonSAIO_distance_bw_MDT_and_offlinesegment_Out_Endcap',
                                      title='L2MuonSAIO distance b/w MDT and Offline segment Outer Endcap '+chain+';distance [mm];Events',
                                      type='TH1F', path='',xbins=100,xmin=-750.,xmax=750.)

            # superpoint reconstruction efficiency wrt offline segment
            histGroup.defineHistogram(chain+'_superpoint_pt4to6,'+chain+'_L2Muon_chamberID_index;L2MuonSAIO_superpoint_reconstruction_efficiency_wrt_offlinesegment_pT4_6',
                                      title='superpoint reconstruction efficiency wrt Offline segment (pT4GeV-6GeV) '+chain+';sector;efficiency',
                                      cutmask=chain+'_offlinesegment_pt4to6',
                                      type='TEfficiency', path='',xbins=6,xmin=0.,xmax=6.)

            histGroup.defineHistogram(chain+'_superpoint_pt6to8,'+chain+'_L2Muon_chamberID_index;L2MuonSAIO_superpoint_reconstruction_efficiency_wrt_offlinesegment_pT6_8',
                                      title='superpoint reconstruction efficiency wrt Offline segment (pT6GeV-8GeV) '+chain+';sector;efficiency',
                                      cutmask=chain+'_offlinesegment_pt6to8',
                                      type='TEfficiency', path='',xbins=6,xmin=0.,xmax=6.)

            histGroup.defineHistogram(chain+'_superpoint_ptover8,'+chain+'_L2Muon_chamberID_index;L2MuonSAIO_superpoint_reconstruction_efficiency_wrt_offlinesegment_pT8_x',
                                      title='superpoint reconstruction efficiency wrt Offline segment (pT over 8GeV) '+chain+';sector;efficiency',
                                      cutmask=chain+'_offlinesegment_ptover8',
                                      type='TEfficiency', path='',xbins=6,xmin=0.,xmax=6.)

            # superpoint multiplicity per L2SAIO track matched offline track
            histGroup.defineHistogram(chain+'_regionBE,'+chain+'_superpoint_multiplicity;L2MuonSAIO_superpoint_multiplicity_pT4_6',
                                      title='superpoint multiplicity (pT4GeV-6GeV) '+chain+';barrel=0/endcap=1;number of superpoints',
                                      cutmask=chain+'_pt4to6',
                                      type='TH2I', path='',xbins=2,xmin=0,xmax=2,ybins=6,ymin=0,ymax=6)

            histGroup.defineHistogram(chain+'_regionBE,'+chain+'_superpoint_multiplicity;L2MuonSAIO_superpoint_multiplicity_pT6_8',
                                      title='superpoint multiplicity (pT6GeV-8GeV) '+chain+';barrel=0/endcap=1;number of superpoints',
                                      cutmask=chain+'_pt6to8',
                                      type='TH2I', path='',xbins=2,xmin=0,xmax=2,ybins=6,ymin=0,ymax=6)

            histGroup.defineHistogram(chain+'_regionBE,'+chain+'_superpoint_multiplicity;L2MuonSAIO_superpoint_multiplicity_pT8_x',
                                      title='superpoint multiplicity (pT over 8GeV) '+chain+';barrel=0/endcap=1;number of superpoints',
                                      cutmask=chain+'_ptover8',
                                      type='TH2I', path='',xbins=2,xmin=0,xmax=2,ybins=6,ymin=0,ymax=6)

            # L2Inside-Out track multiplicity
            histGroup.defineHistogram(chain+'_offPt,'+chain+'_L2InsideOut_track_multiplicity;L2InsideOut_track_multiplicity_toOffl_pt_Barrel',
                                      title='L2Inside-Out track multiplicity per L2SA track wrt Offline in pt Barrel '+chain+';p_{T} [GeV/c];multiplicity',
                                      cutmask=chain+'_isBarrel',
                                      type='TH2F', path='',xbins=104,xmin=-52.,xmax=52.,ybins=10,ymin=0.,ymax=10.)

            histGroup.defineHistogram(chain+'_offPt,'+chain+'_L2InsideOut_track_multiplicity;L2InsideOut_track_multiplicity_toOffl_pt_Endcap',
                                      title='L2Inside-Out track multiplicity per L2SA track wrt Offline in pt Endcap '+chain+';p_{T} [GeV/c];multiplicity',
                                      cutmask=chain+'_isEndcap',
                                      type='TH2F', path='',xbins=104,xmin=-52.,xmax=52.,ybins=10,ymin=0.,ymax=10.)

            # pt resolution of cbio track wrt offline track
            histGroup.defineHistogram(chain+'_ptresol_pos;L2CBIO_ptresol_toOffl_pos',
                                      title='L2CBIO pT resolution wrt Offline positive muons '+chain+';p_{T} resol;Events',
                                      type='TH1F', path='',xbins=100,xmin=-0.2,xmax=0.2)

            histGroup.defineHistogram(chain+'_ptresol_neg;L2CBIO_ptresol_toOffl_neg',
                                      title='L2CBIO pT resolution wrt Offline negative muons '+chain+';p_{T} resol;Events',
                                      type='TH1F', path='',xbins=100,xmin=-0.2,xmax=0.2)

            histGroup.defineHistogram(chain+'_ptresol_pos;L2CBIO_ptresol_toOffl_pos_BarrelA',
                                      title='L2CBIO pT resolution wrt Offline positive muons Barrel A-side '+chain+';p_{T} resol;Events',
                                      cutmask=chain+'_isBarrelA',
                                      type='TH1F', path='',xbins=100,xmin=-0.2,xmax=0.2)

            histGroup.defineHistogram(chain+'_ptresol_pos;L2CBIO_ptresol_toOffl_pos_BarrelC',
                                      title='L2CBIO pT resolution wrt Offline positive muons Barrel C-side '+chain+';p_{T} resol;Events',
                                      cutmask=chain+'_isBarrelC',
                                      type='TH1F', path='',xbins=100,xmin=-0.2,xmax=0.2)

            histGroup.defineHistogram(chain+'_ptresol_pos;L2CBIO_ptresol_toOffl_pos_EndcapA',
                                      title='L2CBIO pT resolution wrt Offline positive muons Endcap A-side '+chain+';p_{T} resol;Events',
                                      cutmask=chain+'_isEndcapA',
                                      type='TH1F', path='',xbins=100,xmin=-0.2,xmax=0.2)

            histGroup.defineHistogram(chain+'_ptresol_pos;L2CBIO_ptresol_toOffl_pos_EndcapC',
                                      title='L2CBIO pT resolution wrt Offline positive muons Endcap C-side '+chain+';p_{T} resol;Events',
                                      cutmask=chain+'_isEndcapC',
                                      type='TH1F', path='',xbins=100,xmin=-0.2,xmax=0.2)

            histGroup.defineHistogram(chain+'_ptresol_neg;L2CBIO_ptresol_toOffl_neg_BarrelA',
                                      title='L2CBIO pT resolution wrt Offline negative muons Barrel A-side '+chain+';p_{T} resol;Events',
                                      cutmask=chain+'_isBarrelA',
                                      type='TH1F', path='',xbins=100,xmin=-0.2,xmax=0.2)

            histGroup.defineHistogram(chain+'_ptresol_neg;L2CBIO_ptresol_toOffl_neg_BarrelC',
                                      title='L2CBIO pT resolution wrt Offline negative muons Barrel C-side '+chain+';p_{T} resol;Events',
                                      cutmask=chain+'_isBarrelC',
                                      type='TH1F', path='',xbins=100,xmin=-0.2,xmax=0.2)

            histGroup.defineHistogram(chain+'_ptresol_neg;L2CBIO_ptresol_toOffl_neg_EndcapA',
                                      title='L2CBIO pT resolution wrt Offline negative muons Endcap A-side '+chain+';p_{T} resol;Events',
                                      cutmask=chain+'_isEndcapA',
                                      type='TH1F', path='',xbins=100,xmin=-0.2,xmax=0.2)

            histGroup.defineHistogram(chain+'_ptresol_neg;L2CBIO_ptresol_toOffl_neg_EndcapC',
                                      title='L2CBIO pT resolution wrt Offline negative muons Endcap C-side '+chain+';p_{T} resol;Events',
                                      cutmask=chain+'_isEndcapC',
                                      type='TH1F', path='',xbins=100,xmin=-0.2,xmax=0.2)

            histGroup.defineHistogram(chain+'_offEta,'+chain+'_ptresol;L2CBIO_ptresol_toOffl_eta',
                                      title='L2CBIO pT resolution wrt Offline in eta '+chain+';#eta;p_{T} resol',
                                      type='TH2F', path='',xbins=27,xmin=-2.7,xmax=2.7,ybins=100,ymin=-0.2,ymax=0.2)

            histGroup.defineHistogram(chain+'_offEta,'+chain+'_ptresol;L2CBIO_ptresol_toOffl_eta_pT4_6',
                                      title='L2CBIO pT resolution wrt Offline in eta (pT4GeV-6GeV) '+chain+';#eta;p_{T} resol',
                                      cutmask=chain+'_pt4to6',
                                      type='TH2F', path='',xbins=27,xmin=-2.7,xmax=2.7,ybins=100,ymin=-0.2,ymax=0.2)

            histGroup.defineHistogram(chain+'_offEta,'+chain+'_ptresol;L2CBIO_ptresol_toOffl_eta_pT6_8',
                                      title='L2CBIO pT resolution wrt Offline in eta (pT6GeV-8GeV) '+chain+';#eta;p_{T} resol',
                                      cutmask=chain+'_pt6to8',
                                      type='TH2F', path='',xbins=27,xmin=-2.7,xmax=2.7,ybins=100,ymin=-0.2,ymax=0.2)

            histGroup.defineHistogram(chain+'_offEta,'+chain+'_ptresol;L2CBIO_ptresol_toOffl_eta_pT8_x',
                                      title='L2CBIO pT resolution wrt Offline in eta (pT over 8GeV) '+chain+';#eta;p_{T} resol',
                                      cutmask=chain+'_ptover8',
                                      type='TH2F', path='',xbins=27,xmin=-2.7,xmax=2.7,ybins=100,ymin=-0.2,ymax=0.2)

            histGroup.defineHistogram(chain+'_offPt,'+chain+'_ptresol;L2CBIO_ptresol_toOffl_pt_BarrelA',
                                      title='L2CBIO pT resolution wrt Offline in pt Barrel A-side '+chain+';p_{T} [GeV/c];p_{T} resol',
                                      cutmask=chain+'_isBarrelA',
                                      type='TH2F', path='',xbins=104,xmin=-52.,xmax=52.,ybins=100,ymin=-0.2,ymax=0.2)

            histGroup.defineHistogram(chain+'_offPt,'+chain+'_ptresol;L2CBIO_ptresol_toOffl_pt_BarrelC',
                                      title='L2CBIO pT resolution wrt Offline in pt Barrel C-side '+chain+';p_{T} [GeV/c];p_{T} resol',
                                      cutmask=chain+'_isBarrelC',
                                      type='TH2F', path='',xbins=104,xmin=-52.,xmax=52.,ybins=100,ymin=-0.2,ymax=0.2)

            histGroup.defineHistogram(chain+'_offPt,'+chain+'_ptresol;L2CBIO_ptresol_toOffl_pt_EndcapA',
                                      title='L2CBIO pT resolution wrt Offline in pt Endcap A-side '+chain+';p_{T} [GeV/c];p_{T} resol',
                                      cutmask=chain+'_isEndcapA',
                                      type='TH2F', path='',xbins=104,xmin=-52.,xmax=52.,ybins=100,ymin=-0.2,ymax=0.2)

            histGroup.defineHistogram(chain+'_offPt,'+chain+'_ptresol;L2CBIO_ptresol_toOffl_pt_EndcapC',
                                      title='L2CBIO pT resolution wrt Offline in pt Endcap C-side '+chain+';p_{T} [GeV/c];p_{T} resol',
                                      cutmask=chain+'_isEndcapC',
                                      type='TH2F', path='',xbins=104,xmin=-52.,xmax=52.,ybins=100,ymin=-0.2,ymax=0.2)

    return

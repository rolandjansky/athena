# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

def TrigIDtrkMonitoringTool( legacy_monitoring=False ):

        # do we want the lagacy monitoring ?
 
        from TriggerJobOpts.HLTTriggerResultGetter import EDMDecodingVersion
        from TriggerJobOpts.TriggerFlags import TriggerFlags

        EDMDecodingVersion()

        mt_chains = True
        if ( TriggerFlags.EDMDecodingVersion < 3 or legacy_monitoring ) :
                mt_chains = False

        list = []

        from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestBase

        if True:

                ##############################################################
                # Cosmic instances
                ##############################################################

                # Cosmic Expert instance
                
                tidacos = TrigTestBase(name = "IDCosmicTool",
                                       histoPathBase = "/Trigger/HLT")
                tidacos.AnalysisConfig = "Tier0"
                tidacos.SliceTag = "HLT/TRIDT/Cosmic/Expert"
                # tidacos.OutputLevel = DEBUG
                tidacos.pixHitsOffline=-1
                tidacos.sctHitsOffline=-1
                tidacos.siHitsOffline=-1

                if mt_chains: 
                        tidacos.ntupleChainNames += [
                                "Offline",
                                "HLT_.*_cosmic.*:HLT_IDTrack_FTF",
                                "HLT_.*_cosmic.*:HLT_IDTrack_EFID"
                        ]
                else:
                        tidacos.ntupleChainNames += [
                                "Offline",
                                "HLT_id_cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
                                "HLT_id_cosmic.*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID"
                        ]

                list += [ tidacos ]
                        


                if not mt_chains:
                                        
                        # Cosmic Shifter instance
                        
                        tidacosshift = TrigTestBase(name = "IDCosmicShifterTool",
                                                    histoPathBase = "/Trigger/HLT")
                        tidacosshift.AnalysisConfig = "Tier0"
                        tidacosshift.SliceTag = "HLT/TRIDT/Cosmic/Shifter"
                        # tidacos.OutputLevel = DEBUG
                        tidacosshift.pixHitsOffline=-1
                        tidacosshift.sctHitsOffline=-1
                        tidacosshift.siHitsOffline=-1
                        tidacosshift.ntupleChainNames += [
                                "Offline",
                                "HLT_id_cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
                                "HLT_id_cosmic.*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID"
                        ]

                        list += [ tidacosshift ]
                        


                ##############################################################
                # Egamma instances
                ##############################################################

                # Expert instances 
                tidaegamma = TrigTestBase(name = "IDEgammaTool",
                                          histoPathBase = "/Trigger/HLT")
                tidaegamma.AnalysisConfig = "Tier0"
                tidaegamma.SliceTag = "HLT/TRIDT/Egamma/Expert"
                # tidabase.OutputLevel = DEBUG
                tidaegamma.UseHighestPT = True

                if mt_chains: 
                        tidaegamma.ntupleChainNames += [
                                "Offline",
                                "HLT_e.*etcut.*:key=HLT_IDTrack_Electron_FTF",
                                "HLT_e.*etcut.*:key=HLT_IDTrack_Electron_IDTrig",
                                "HLT_e.*_gsf_idperf.*:key=GSFTrigTrackParticles"
                        ]
                else:
                        tidaegamma.ntupleChainNames += [
                                "Offline",
                                "HLT_e.*idperf.*:InDetTrigTrackingxAODCnv_Electron_IDTrig",
                                "HLT_e.*idperf.*:InDetTrigTrackingxAODCnv_Electron_FTF",
                                "HLT_e.*_gsf_idperf:GSFTrigTrackParticles"
                        ]

                list += [ tidaegamma ]


                if not mt_chains:
                        # Shifter instances 
                        tidaegammashift = TrigTestBase(name = "IDEgammaShifterTool",
                                                       histoPathBase = "/Trigger/HLT")
                        tidaegammashift.AnalysisConfig = "Tier0"
                        tidaegammashift.SliceTag = "HLT/TRIDT/Egamma/Shifter"
                        tidaegammashift.UseHighestPT = True
                        # tidabase.OutputLevel = DEBUG
                        tidaegammashift.ntupleChainNames += [
                                "Offline",
                                "HLT_e28_lhtight_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
                                "HLT_e28_lhtight_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig",
                                "HLT_e24_lhmedium_idperf_L1EM20VH:InDetTrigTrackingxAODCnv_Electron_FTF",
                                "HLT_e24_lhmedium_idperf_L1EM20VH:InDetTrigTrackingxAODCnv_Electron_IDTrig",
                                "HLT_e26_lhtight_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
                                "HLT_e26_lhtight_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig",
                                # HI chains - pPb running
                                "HLT_e15_lhloose_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
                                "HLT_e15_lhloose_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig",
                                # PbPb running
                                "HLT_e15_medium_ion_idperf_L1EM12:InDetTrigTrackingxAODCnv_Electron_FTF",
                                "HLT_e15_medium_ion_idperf_L1EM12:InDetTrigTrackingxAODCnv_Electron_IDTrig",
                                "HLT_e.*_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
                                "HLT_e.*_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig"
                        ]

                        list += [ tidaegammashift ]



                        # Shifter purity instances 
                        tidaegammapurity = TrigTestBase(name = "IDEgammaPurityTool",
                                                        histoPathBase = "/Trigger/HLT")
                        tidaegammapurity.AnalysisConfig = "Tier0"
                        tidaegammapurity.SliceTag = "HLT/TRIDT/EgammaPurity/Expert"
                        tidaegammapurity.UseHighestPT = True
                        tidaegammapurity.RunPurity = True
                        # tidabase.OutputLevel = DEBUG
                        tidaegammapurity.Shifter = True
                        tidaegammapurity.ntupleChainNames += [
                                "Offline",
                                "HLT_e28_lhtight_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
                                "HLT_e28_lhtight_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig",
                                "HLT_e26_lhtight_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
                                "HLT_e26_lhtight_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig",
                                # HI chains 
                                "HLT_e15_medium_ion_idperf_L1EM12:InDetTrigTrackingxAODCnv_Electron_FTF",
                                "HLT_e15_medium_ion_idperf_L1EM12:InDetTrigTrackingxAODCnv_Electron_IDTrig",
                                "HLT_e.*_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
                                "HLT_e.*_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig"
                        ]

                        list += [ tidaegammapurity ]


                



                ##############################################################
                # Muon instances
                ##############################################################
                
                # Expert instances 
                tidamuon = TrigTestBase(name = "IDMuonTool",
                                        histoPathBase = "/Trigger/HLT")
                tidamuon.AnalysisConfig = "Tier0"
                tidamuon.SliceTag = "HLT/TRIDT/Muon/Expert"
                tidamuon.UseHighestPT = True
                # tidabase.OutputLevel = DEBUG

                if mt_chains:
                        tidamuon.ntupleChainNames += [
                                "Offline",
                                "HLT_mu.*idperf.*:key=HLT_IDTrack_Muon_FTF:roi=HLT_Roi_L2SAMuon",
                                "HLT_mu.*idperf.*:key=HLT_IDTrack_Muon_IDTrig:roi=HLT_Roi_L2SAMuonForEF",
                                "HLT_mu.*i.*:key=HLT_IDTrack_MuonIso_FTF:roi=HLT_Roi_MuonIso",
                                "HLT_mu.*i.*:key=HLT_IDTrack_MuonIso_IDTrig:roi=HLT_Roi_MuonIso"
                        ]
                else:
                        tidamuon.ntupleChainNames += [
                                "Offline",
                                "HLT_mu.*idperf.*:InDetTrigTrackingxAODCnv_Muon_FTF",
                                "HLT_mu.*idperf.*:InDetTrigTrackingxAODCnv_Muon_IDTrig"
                        ]

                list += [ tidamuon ]



                if not mt_chains:

                        # Shifter instances 
                        tidamuonshift = TrigTestBase(name = "IDMuonShifterTool",
                                                     histoPathBase = "/Trigger/HLT")
                        tidamuonshift.AnalysisConfig = "Tier0"
                        tidamuonshift.SliceTag = "HLT/TRIDT/Muon/Shifter"
                        tidamuonshift.UseHighestPT = True
                        tidamuonshift.ntupleChainNames += [
                                "Offline",
                                "HLT_mu20_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
                                "HLT_mu20_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",
                                # HI chains
                                "HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
                                "HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",
                                "HLT_mu8_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
                                "HLT_mu8_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",
                                "HLT_mu.*_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
                                "HLT_mu.*_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig"
                        ]

                        list += [ tidamuonshift ]
                        

                        # Shifter purity instances 
                        tidamuonpurity = TrigTestBase(name = "IDMuonPurityTool",
                                                      histoPathBase = "/Trigger/HLT")
                        tidamuonpurity.AnalysisConfig = "Tier0"
                        tidamuonpurity.SliceTag = "HLT/TRIDT/MuonPurity/Expert"
                        tidamuonpurity.UseHighestPT = True
                        tidamuonpurity.RunPurity = True
                        tidamuonpurity.Shifter = True
                        tidamuonpurity.ntupleChainNames += [
                                "Offline",
                                "HLT_mu20_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",
                                "HLT_mu20_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
                                # HI chains 
                                # "HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
                                # "HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",
                                # "HLT_mu8_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
                                # "HLT_mu8_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",
                                "HLT_mu.*_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
                                "HLT_mu.*_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig"
                        ]

                        list += [ tidamuonpurity ]
                        

                ##############################################################
                # Tau instances
                ##############################################################

                # Expert instances 
                tidatau = TrigTestBase(name = "IDTauTool",
                                       histoPathBase = "/Trigger/HLT")
                tidatau.AnalysisConfig = "Tier0"
                tidatau.SliceTag = "HLT/TRIDT/Tau/Expert"
                tidatau.UseHighestPT = True

                if mt_chains:
                        tidatau.ntupleChainNames += [
                                "Offline",
                                "HLT_tau.*idperf.*:key=HLT_IDTrack_TauCore_FTF:roi=HLT_Roi_TauCore",
                                "HLT_tau.*idperf.*:key=HLT_IDTrack_TauIso_FTF:roi=HLT_Roi_TauIso_TauID",
                                "HLT_tau.*idperf.*:key=HLT_IDTrack_Tau_IDTrig:roi=HLT_TauIso_TauID"
                         ]
                else:
                        tidatau.ntupleChainNames += [
                                "Offline",
                                "HLT_tau.*idperf.*track:key=InDetTrigTrackingxAODCnv_Tau_IDTrig",
                                "HLT_tau.*idperf.*track:key=InDetTrigTrackingxAODCnv_Tau_FTF",
                                "HLT_tau.*idperf.*tracktwo.*:key=InDetTrigTrackingxAODCnv_TauCore_FTF:roi=forID1",
                                "HLT_tau.*idperf.*tracktwo.*:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3",
                                "HLT_tau.*idperf.*tracktwo.*:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3"
                        ]

                list += [ tidatau ]




                if not mt_chains:
                        # Shifter instances 
                        tidataushift = TrigTestBase(name = "IDTauShifterTool",
                                                    histoPathBase = "/Trigger/HLT")
                        tidataushift.AnalysisConfig = "Tier0"
                        tidataushift.SliceTag = "HLT/TRIDT/Tau/Shifter"
                        tidataushift.UseHighestPT = True
                        tidataushift.ntupleChainNames += [
                                "Offline",
                                #                       "HLT_tau25_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_TauCore_FTF:roi=forID1",
                                "HLT_tau25_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3",
                                "HLT_tau25_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3",
                                "HLT_tau.*_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3",
                                "HLT_tau.*_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3"
                        ]

                        list += [ tidataushift ]
                        
                        
                        
                        # Shifter Purity instances 
                        tidataupurity = TrigTestBase(name = "IDTauPurityTool",
                                                     histoPathBase = "/Trigger/HLT")
                        tidataupurity.AnalysisConfig = "Tier0"
                        tidataupurity.SliceTag = "HLT/TRIDT/TauPurity/Expert"
                        tidataupurity.UseHighestPT = True
                        tidataupurity.RunPurity = True
                        tidataupurity.Shifter = True
                        tidataupurity.ntupleChainNames += [
                                "Offline",
                                #                       "HLT_tau25_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_TauCore_FTF:roi=forID1",
                                #                       "HLT_tau25_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3",
                                #                       "HLT_tau25_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3",
                                "HLT_tau.*_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3",
                                "HLT_tau.*_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3"
                        ]

                        list += [ tidataupurity ]
                        
                        



                
                ##############################################################
                # Bjet instances - check track collection names
                ##############################################################
                        
                # Expert instances
                tidabjet = TrigTestBase(name = "IDBjetTool",
                                        histoPathBase = "/Trigger/HLT")
                tidabjet.AnalysisConfig = "Tier0"
                tidabjet.SliceTag = "HLT/TRIDT/Bjet/Expert"

                if mt_chains:
                        tidabjet.ntupleChainNames += [
                                "Offline",
                                ":HLT_IDTrack_FS_FTF:vtx=HLT_IDVertex_FS",
                                "HLT_j.*.*boffperf.*:key=HLT_IDTrack_Bjet_FTF",
                                "HLT_j.*.*boffperf.*:key=HLT_IDTrack_Bjet_IDTrig"
                        ]
                else:
                        tidabjet.ntupleChainNames += [
                                "Offline",
                                # jet based chains
                                "HLT_j55_bperf:InDetTrigTrackingxAODCnv_Bjet_FTF",
                                "HLT_j55_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
                                "HLT_j55_bperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:post:pt=5000",
                                "HLT_j55_bperf_split:key=InDetTrigTrackingxAODCnv_Bjet_FTF",
                                "HLT_j55_bperf_split:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig",
                                "HLT_j.*boffperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:post:pt=5000",
                                "HLT_j.*boffperf_split:key=InDetTrigTrackingxAODCnv_Bjet_FTF",
                                "HLT_j.*boffperf_split:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig"
                        ]
                
                list += [ tidabjet ]

                        
                if not mt_chains:

                        # Shifter instances
                        tidabjetshift = TrigTestBase(name = "IDBjetShifterTool",
                                                     histoPathBase = "/Trigger/HLT")
                        tidabjetshift.AnalysisConfig = "Tier0"
                        tidabjetshift.SliceTag = "HLT/TRIDT/Bjet/Shifter"
                        tidabjetshift.ntupleChainNames += [
                                "Offline",
                                "HLT_j45_gsc55_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
                                "HLT_j45_gsc55_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
                                "HLT_j35_gsc55_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
                                "HLT_j35_gsc55_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
                                "HLT_j55.*_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
                                "HLT_j55.*_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
                                "HLT_j5.*_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
                                "HLT_j5.*_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
                                "HLT_j.*_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
                                "HLT_j.*_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig"
                        ]

                        list += [ tidabjetshift ]
                        
                        # Shifter Purity instances
                        tidabjetpurity = TrigTestBase(name = "IDBjetPurityerTool",
                                                      histoPathBase = "/Trigger/HLT")
                        tidabjetpurity.AnalysisConfig = "Tier0"
                        tidabjetpurity.SliceTag = "HLT/TRIDT/BjetPurity/Expert"
                        tidabjetpurity.RunPurity = True
                        tidabjetpurity.Shifter = True
                        tidabjetpurity.ntupleChainNames += [
                                "Offline",
                                "HLT_j45_gsc55_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
                                "HLT_j45_gsc55_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
                                "HLT_j225_gsc400_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
                                "HLT_j225_gsc400_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
                                "HLT_j85_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
                                "HLT_j85_boffperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig"
                        ]

                        list += [ tidabjetpurity ]
                        
                        
                        # Shifter instances
                        tidabjetvtxshift = TrigTestBase(name = "IDBjetVtxShifterTool",
                                                        histoPathBase = "/Trigger/HLT")
                        tidabjetvtxshift.AnalysisConfig = "Tier0"
                        tidabjetvtxshift.SliceTag = "HLT/TRIDT/BjetVtx/Shifter"
                        tidabjetvtxshift.ShifterChains = 2
                        tidabjetvtxshift.ntupleChainNames += [
                                "Offline",
                                #                       "HLT_j225_gsc400_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx",
                                #                       "HLT_j225_gsc400_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx",
                                "HLT_j45_gsc55_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx",
                                "HLT_j45_gsc55_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx",
                                "HLT_j35_gsc55_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx",
                                "HLT_j35_gsc55_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx",
                                "HLT_j55.*_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx",
                                "HLT_j55.*_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx",
                                "HLT_j5.*_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx",
                                "HLT_j5.*_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx",
                                "HLT_j.*_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx",
                                "HLT_j.*_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx"
                        ]

                        list += [ tidabjetvtxshift ]
                        
                        
                        # Shifter instances
                        tidajetvtxshift = TrigTestBase(name = "IDJetVtxShifterTool",
                                                       histoPathBase = "/Trigger/HLT")
                        tidajetvtxshift.AnalysisConfig = "Tier0"
                        tidajetvtxshift.SliceTag = "HLT/TRIDT/JetVtx/Shifter"
                        tidajetvtxshift.ShifterChains = 2
                        tidajetvtxshift.ntupleChainNames += [
                                "Offline",
                                "HLT_j85_gsc110_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx",
                                "HLT_j85_gsc110_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx",
                                "HLT_j225_gsc400_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx",
                                "HLT_j225_gsc400_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx",
                                "HLT_j225_gsc.*_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx",
                                "HLT_j225_gsc.*_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx",
                                "HLT_j50_gsc.*_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx",
                                "HLT_j50_gsc.*_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx",
                                "HLT_j.*_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx",
                                "HLT_j.*_boffperf_split:InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx"
                        ]

                        list += [ tidajetvtxshift ]
                        
                        
                        
                        

                        ##############################################################
                        # Bphys instances
                        ##############################################################
                        
                        # Expert instances 
                        tidabphys = TrigTestBase(name = "IDBphysTool",
                                                 histoPathBase = "/Trigger/HLT")
                        tidabphys.AnalysisConfig = "Tier0"
                        tidabphys.SliceTag = "HLT/TRIDT/Bphys/Expert"
                        # tidabase.OutputLevel = DEBUG
                        tidabphys.ntupleChainNames += [
                                "Offline",
                                "HLT_mu.*_mu.*idperf.*noid:InDetTrigTrackingxAODCnv_Bphysics_FTF",
                                "HLT_mu.*_mu.*idperf.*noid:InDetTrigTrackingxAODCnv_Bphysics_IDTrig"
                                #                       "HLT_mu.*_mu.*idperf.*noid:InDetTrigTrackingxAODCnv_Bphysics_EFID"
                                #                       "HLT_.*Bmumux.*:InDetTrigTrackingxAODCnv_Bphysics_EFID",
                                #                       "HLT_.*Bmumux.*:InDetTrigTrackingxAODCnv_Bphysics_IDTrig",
                                #                       "HLT_.*Bmumux.*:InDetTrigTrackingxAODCnv_Bphysics_FTF"
                        ]

                        list += [ tidabphys ]
                        
                        
                        # Shifter instances 
                        tidabphysshift = TrigTestBase(name = "IDBphysShifterTool",
                                                      histoPathBase = "/Trigger/HLT")
                        tidabphysshift.AnalysisConfig = "Tier0"
                        tidabphysshift.SliceTag = "HLT/TRIDT/Bphys/Shifter"
                        tidabphysshift.ntupleChainNames += [
                                "Offline",
                                #                       "HLT_.*Bmumux.*:InDetTrigTrackingxAODCnv_Bphysics_IDTrig",
                                #                       "HLT_.*Bmumux.*:InDetTrigTrackingxAODCnv_Bphysics_FTF"
                                "HLT_mu.*_mu.*idperf.*noid:InDetTrigTrackingxAODCnv_Bphysics_FTF",
                                "HLT_mu.*_mu.*idperf.*noid:InDetTrigTrackingxAODCnv_Bphysics_IDTrig"
                        ]

                        list += [ tidabphysshift ]
                        




#                 ##############################################################
#                 # minBias instances
#                 ##############################################################
                
#                 # Expert instances 
#                 tidaminbias = TrigTestBase(name = "IDminBiasTool",
#                                         histoPathBase = "/Trigger/HLT")
#                 tidaminbias.AnalysisConfig = "Tier0"
#                 tidaminbias.SliceTag = "HLT/TRIDT/minBias/Expert"
#                 # tidabase.OutputLevel = DEBUG
#                 tidaminbias.ntupleChainNames += [
#                         "Offline",
#                         "HLT_mb_.*idperf.*:InDetTrigTrackingxAODCnv_minBias_EFID"
#                         ]
#                 list += [ "TrigTestBase/IDminBiasTool" ]


#                 # Shifter instances 
#                 tidaminbiasshift = TrigTestBase(name = "IDminBiasShifterTool",
#                                         histoPathBase = "/Trigger/HLT")
#                 tidaminbiasshift.AnalysisConfig = "Tier0"
#                 tidaminbiasshift.SliceTag = "HLT/TRIDT/minBias/Shifter"
#                 tidaminbiasshift.ntupleChainNames += [
#                         "Offline",
#                         "HLT_mb_.*idperf.*:InDetTrigTrackingxAODCnv_minBias_EFID"
#                         ]

#                 list += [ tidaminbiasshift ]



        return list

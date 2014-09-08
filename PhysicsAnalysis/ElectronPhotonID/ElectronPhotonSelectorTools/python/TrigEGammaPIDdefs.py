# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class trigEgammaIDQuality:
    ElectronIDLoose = 0
    ElectronIDMedium = 1
    ElectronIDMedium1 = 2
    ElectronIDMedium2 = 3
    ElectronIDMedium3 = 4
    ElectronIDMedium_TRT = 5
    ElectronIDTight = 6
    ElectronIDTightEF = 7
    ElectronIDTightEF_TRT = 8
    ElectronIDTightEF_NoEoP_WithTightDeltaEta = 9
    ElectronIDTight1 = 10
    ElectronIDTight2 = 11
    ElectronIDHltTighter = 12
    PhotonIDLoose = 13
    PhotonIDLooseEF = 14
    PhotonIDMediumEF = 15
    PhotonIDTight = 16

class BitDefElectron:
   """  @brief cluster eta range """
   ClusterEtaRange_Electron        =  0
   
   """ @brief matching to photon (not necessarily conversion--the name is historical) """
   ConversionMatch_Electron        =  1
   
   """ @brief cluster leakage into the hadronic calorimeter """
   ClusterHadronicLeakage_Electron =  2
   """ @brief energy in 2nd sampling (e.g E277>0) """
   ClusterMiddleEnergy_Electron    =  3 
   """ @brief energy ratio in 2nd sampling (e.g E237/E277) """
   ClusterMiddleEratio37_Electron  =  4
   """ @brief energy ratio in 2nd sampling (e.g E233/E237) """
   ClusterMiddleEratio33_Electron  =  5
   """ @brief width in the second sampling (e.g Weta2) """
   ClusterMiddleWidth_Electron     =  6
   """ @brief energy fraction in the third layer """ 
   ClusterBackEnergyFraction_Electron = 7
   """ @brief fraction of energy found in 1st sampling (NB: not used in fact for electrons)"""
   ClusterStripsEratio_Electron    =  8
   """ @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et) """
   ClusterStripsDeltaEmax2_Electron = 9
   """ @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1) """
   ClusterStripsDeltaE_Electron    = 10
   """ @brief shower width in 1st sampling """
   ClusterStripsWtot_Electron      = 11
   """ @brief shower shape in shower core 1st sampling """
   ClusterStripsFracm_Electron     = 12
   """ @brief shower width weighted by distance from the maximum one """
   ClusterStripsWeta1c_Electron    = 13

   """ @brief difference between max and 2nd max in strips """
   ClusterStripsDEmaxs1_Electron   = 15
   """ @brief B layer hit """
   TrackBlayer_Electron            = 16
   """ @brief number of Pixel hits """
   TrackPixel_Electron             = 17
   """ @brief number of Pixel and SCT hits """
   TrackSi_Electron                = 18
   """ @brief distance of closet approach """
   TrackA0_Electron                = 19
   """ @brief eta difference between cluster and extrapolated track in the 1st sampling """
   TrackMatchEta_Electron          = 20
   """ @brief phi difference between cluster and extrapolated track in the 2nd sampling """
   TrackMatchPhi_Electron          = 21
   """ @brief energy-momentum match """
   TrackMatchEoverP_Electron       = 22
   """ @brief number of TRT hits """
   TrackTRThits_Electron           = 24
   """ @brief ratio of high to all TRT hits for isolated electrons """
   TrackTRTratio_Electron          = 25
   """ @brief ratio of high to all TRT hits for non-isolated electrons """    
   TrackTRTratio90_Electron        = 26

   """ @brief distance of closet approach for tight selection """
   TrackA0Tight_Electron           = 27
   """ @brief eta difference between cluster and extrapolated track in the 1st sampling for tight selection """
   TrackMatchEtaTight_Electron     = 28

   """ @brief isolation """
   Isolation_Electron              = 29
   """ @brief calorimetric isolation """
   ClusterIsolation_Electron       = 30
   """ @brief tracker isolation """
   TrackIsolation_Electron         = 31


#
# cut definition for electrons
#
class CutDefElectron:
	""" @brief cuts of hadronic leakage"""
	HADLEAKETA_ELECTRON = \
	   0x1 << BitDefElectron.ClusterEtaRange_Electron        | \
	   0x1 << BitDefElectron.ClusterHadronicLeakage_Electron
	""" @brief old cuts in strips (without ClusterStripsDEmaxs1)"""
	CALOSTRIPSOLD_ELECTRON = \
	   0x1 << BitDefElectron.ClusterStripsEratio_Electron     | \
	   0x1 << BitDefElectron.ClusterStripsDeltaEmax2_Electron | \
	   0x1 << BitDefElectron.ClusterStripsDeltaE_Electron     | \
	   0x1 << BitDefElectron.ClusterStripsWtot_Electron       | \
	   0x1 << BitDefElectron.ClusterStripsFracm_Electron      | \
	   0x1 << BitDefElectron.ClusterStripsWeta1c_Electron     
	""" @brief cuts in strips (with ClusterStripsDEmaxs1)"""
	CALOSTRIPS_ELECTRON = \
	   0x1 << BitDefElectron.ClusterStripsEratio_Electron     | \
	   0x1 << BitDefElectron.ClusterStripsDeltaEmax2_Electron | \
	   0x1 << BitDefElectron.ClusterStripsDeltaE_Electron     | \
	   0x1 << BitDefElectron.ClusterStripsWtot_Electron       | \
	   0x1 << BitDefElectron.ClusterStripsFracm_Electron      | \
	   0x1 << BitDefElectron.ClusterStripsWeta1c_Electron     | \
	   0x1 << BitDefElectron.ClusterStripsDEmaxs1_Electron    
	""" @brief cuts in strips for loose electrons  sampling"""
	CALOSTRIPS_LOOSE_ELECTRON = \
	   0x1 << BitDefElectron.ClusterStripsWtot_Electron       | \
	   0x1 << BitDefElectron.ClusterStripsDEmaxs1_Electron    

	""" @brief cuts in middle sampling"""
	CALOMIDDLE_ELECTRON = \
	   0x1 << BitDefElectron.ClusterMiddleEnergy_Electron     | \
	   0x1 << BitDefElectron.ClusterMiddleEratio37_Electron   | \
	   0x1 << BitDefElectron.ClusterMiddleWidth_Electron     
	""" @brief calorimeter isolation"""
	CALORIMETRICISOLATION_ELECTRON =  \
	   0x1 << BitDefElectron.ClusterIsolation_Electron 

	""" @brief "old" all cuts in calorimeter (except isolation) without ClusterStripsDEmaxs1 """
	CALONOISOOLD_ELECTRON = HADLEAKETA_ELECTRON | CALOSTRIPSOLD_ELECTRON | CALOMIDDLE_ELECTRON 
	""" @brief "old" all cuts in calorimeter (including isolation) without ClusterStripsDEmaxs1 """
	CALOOLD_ELECTRON = CALONOISOOLD_ELECTRON | CALORIMETRICISOLATION_ELECTRON
	""" @brief all cuts in calorimeter (except isolation)"""
	CALO_ELECTRON = HADLEAKETA_ELECTRON | CALOSTRIPS_ELECTRON | CALOMIDDLE_ELECTRON 

	""" @brief Track quality cuts except b-layer for electrons"""
	TRACKINGNOBLAYER_ELECTRON = \
	   0x1 << BitDefElectron.TrackPixel_Electron   | \
	   0x1 << BitDefElectron.TrackSi_Electron      | \
	   0x1 << BitDefElectron.TrackA0_Electron
	""" @brief Track quality cuts except b-layer and A0 for electrons"""
	TRACKINGLOOSE_ELECTRON = \
	   0x1 << BitDefElectron.TrackPixel_Electron   | \
	   0x1 << BitDefElectron.TrackSi_Electron
	""" @brief Track quality cuts for electrons"""
	TRACKING_ELECTRON = \
	   TRACKINGNOBLAYER_ELECTRON | \
	   0x1 << BitDefElectron.TrackBlayer_Electron
	""" @brief Track cluster matching in eta for electrons"""
	TRACKMATCHDETA_ELECTRON = \
	   0x1 << BitDefElectron.TrackMatchEta_Electron
	""" @brief Track cluster matching in eta, phi for electrons"""
	TRACKMATCHNOEOVERP_ELECTRON = \
	   0x1 << BitDefElectron.TrackMatchEta_Electron      | \
	   0x1 << BitDefElectron.TrackMatchPhi_Electron
	""" @brief Track cluster matching in eta, phi, E/p for electrons"""
	TRACKMATCH_ELECTRON = \
	   0x1 << BitDefElectron.TrackMatchEta_Electron      | \
	   0x1 << BitDefElectron.TrackMatchPhi_Electron      | \
	   0x1 << BitDefElectron.TrackMatchEoverP_Electron  
	""" @brief Tight Track cluster matching """
	TRACKMATCHTIGHT_ELECTRON = \
	   0x1 << BitDefElectron.TrackMatchEtaTight_Electron      | \
	   0x1 << BitDefElectron.TrackA0Tight_Electron
	""" @brief Tight conversion matching """
	CONVMATCH_ELECTRON = \
	   0x1 << BitDefElectron.ConversionMatch_Electron

	""" @brief TRT hits and TR ratio for electrons"""
	TRT_ELECTRON =  \
	   0x1 << BitDefElectron.TrackTRThits_Electron   | \
	   0x1 << BitDefElectron.TrackTRTratio_Electron
	""" @brief TRT hits and TR ratio (@90% eff) for electrons"""
	TRT90_ELECTRON =  \
	   0x1 << BitDefElectron.TrackTRThits_Electron   | \
	   0x1 << BitDefElectron.TrackTRTratio90_Electron

	""" @brief isolation by tracker """
	TRACKINGISOLATION_ELECTRON =  \
	   0x1 << BitDefElectron.TrackIsolation_Electron
	""" @brief isolation for electrons is combination of calo and tracker """
	ISOLATION_ELECTRON =  \
	   0x1 << BitDefElectron.Isolation_Electron 
	""" @brief isolation for electrons is combination of calo and tracker """
	CALOTRACKISOLATION_ELECTRON =  \
	   CALORIMETRICISOLATION_ELECTRON | TRACKINGISOLATION_ELECTRON

	""" @brief all cuts except TRT for electrons"""
	ALLNOTRT_ELECTRON =  \
	   TRACKING_ELECTRON | TRACKMATCH_ELECTRON | CALO_ELECTRON
	""" @brief all cuts except TRT for electrons(old way)"""
	ALLNOTRTOLD_ELECTRON =  \
	   TRACKING_ELECTRON | TRACKMATCH_ELECTRON | CALOOLD_ELECTRON
	""" @brief all cuts for electrons"""
	ALL_ELECTRON =  \
	   ALLNOTRT_ELECTRON | TRT_ELECTRON
	""" @brief all cuts fo electrons (old way)"""
	ALLOLD_ELECTRON =  \
	   ALLNOTRTOLD_ELECTRON | TRT_ELECTRON
	
	#############
	### Added TrigEGamma Definitions
	""" @brief Tight Track cluster matching redefined for EF """
	TRACKMATCHTIGHT_ELECTRON_EF = 0x1 << BitDefElectron.TrackA0Tight_Electron
	""" @brief Track-cluster matching with tight deta for electrons"""
	TRACKMATCHDETATIGHT_ELECTRON = 0x1 << BitDefElectron.TrackMatchEtaTight_Electron
	""" @brief Track-cluster matching in eta, E/p for electrons redefined for EF"""
	TRACKMATCH_ELECTRON_EF = 0x1 << BitDefElectron.TrackMatchEta_Electron | \
		0x1 << BitDefElectron.TrackMatchEoverP_Electron  
	""" @brief Track-cluster matching in eta for electrons redefined for EF"""
	TRACKMATCH_ELECTRON_NoEoP_EF = 0x1 << BitDefElectron.TrackMatchEta_Electron
	""" @brief Track quality cuts except b-layer and A0 for electrons"""
	TRACKINGNOBLAYERNOA0_ELECTRON = \
	   0x1 << BitDefElectron.TrackPixel_Electron   | \
	   0x1 << BitDefElectron.TrackSi_Electron

class SelectionDefElectron:
	
	# old definitions of cuts as in rel<15.2.0
#	ElectronLooseOLD =  CutDefElectron.CALOMIDDLE_ELECTRON | CutDefElectron.HADLEAKETA_ELECTRON
#	ElectronMediumOLD =  CutDefElectron.CALOOLD_ELECTRON | \
#		CutDefElectron.TRACKINGNOBLAYER_ELECTRON | CutDefElectron.TRACKMATCHDETA_ELECTRON
#	ElectronMediumNoIsoOLD =  CutDefElectron.CALONOISOOLD_ELECTRON | \
#		CutDefElectron.TRACKINGNOBLAYER_ELECTRON | CutDefElectron.TRACKMATCHDETA_ELECTRON
#	ElectronTightOLD =  CutDefElectron.ALLOLD_ELECTRON
#	ElectronTightTRTNoIsoOLD =  CutDefElectron.TRACKING_ELECTRON | \
#		CutDefElectron.TRACKMATCH_ELECTRON | CutDefElectron.CALONOISOOLD_ELECTRON | CutDefElectron.TRT90_ELECTRON 
#	ElectronTightNoIsolationOLD = ElectronTightTRTNoIsoOLD
	
	
	# new definitions of cuts as in rel>=15.2.0
	# since 15.7.0 also Tight selection contains 
	#     TRACKMATCHTIGHT_ELECTRON 
	#     CONVMATCH_ELECTRON
	# since rel 16.0.0 definition of ElectronTight has changed
	
	""" @brief Loose electron selection """
	ElectronLoose =  CutDefElectron.CALOMIDDLE_ELECTRON | CutDefElectron.HADLEAKETA_ELECTRON
	""" @brief Medium electron selection with the delta eta cut has been removed """
	ElectronMedium_WithoutTrackMatch =  CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKINGNOBLAYER_ELECTRON
	""" @brief Medium electron selection without deta match with isolation requirement """
	ElectronMediumIso_WithoutTrackMatch = ElectronMedium_WithoutTrackMatch | CutDefElectron.ISOLATION_ELECTRON
	""" @brief Medium electron selection with deta match (was ElectronMedium in 15.X)"""
	ElectronMedium_WithTrackMatch =  CutDefElectron.CALO_ELECTRON | \
		CutDefElectron.TRACKINGNOBLAYER_ELECTRON | CutDefElectron.TRACKMATCHDETA_ELECTRON
	""" @brief MediumIso electron selection with deta match (was ElectronMediumIso in 15.X)"""
	ElectronMediumIso_WithTrackMatch =  ElectronMedium_WithTrackMatch | CutDefElectron.ISOLATION_ELECTRON
	""" @brief Medium electron selecton """
	ElectronMedium = ElectronMedium_WithTrackMatch
	""" @brief MediumIso electron selecton """
	ElectronMediumIso = ElectronMediumIso_WithTrackMatch
	""" @brief obsolete - kept not crash clients """
	ElectronMediumNoIso = ElectronMedium
	
	""" @brief Tight electron without track match (corresponds to RobusterTight in 15.6.X but 
	      without the deltaEta bits TrackMatchEta_Electron and TrackMatchEtaTight_Electron) """
	ElectronTight_WithoutTrackMatch =  CutDefElectron.CALO_ELECTRON | \
	   CutDefElectron.TRACKING_ELECTRON | \
	   0x1 << BitDefElectron.TrackMatchEoverP_Electron | \
	   0x1 << BitDefElectron.TrackA0Tight_Electron | \
	   CutDefElectron.CONVMATCH_ELECTRON | CutDefElectron.TRT_ELECTRON 
	
	""" @brief Tight without conversion requirement or track match """
	ElectronTight_WithoutTrackMatch_NoConvCut =  CutDefElectron.CALO_ELECTRON | \
	   CutDefElectron.TRACKING_ELECTRON | \
	   0x1 << BitDefElectron.TrackMatchEoverP_Electron | \
	   0x1 << BitDefElectron.TrackA0Tight_Electron | \
	   CutDefElectron.TRT_ELECTRON
	
	""" @brief Tight_WithTrackMatch electron selection with isolation requirement """
	ElectronTightIso_WithoutTrackMatch = ElectronTight_WithoutTrackMatch | CutDefElectron.ISOLATION_ELECTRON
	
	""" @brief Tight electron selection with track match"""
	ElectronTight_WithTrackMatch = CutDefElectron.CALO_ELECTRON | \
		CutDefElectron.TRACKING_ELECTRON | CutDefElectron.TRACKMATCH_ELECTRON | \
	   CutDefElectron.TRACKMATCHTIGHT_ELECTRON | CutDefElectron.CONVMATCH_ELECTRON | \
		CutDefElectron.TRT_ELECTRON 
	
	""" @brief Tight with track match without conversion requirement """
	ElectronTight_WithTrackMatch_NoConvCut =  \
	   CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | CutDefElectron.TRACKMATCH_ELECTRON | \
	   CutDefElectron.TRACKMATCHTIGHT_ELECTRON | CutDefElectron.TRT_ELECTRON 
	
	""" @brief Tight_WithTrackMatch electron selection with isolation requirement """
	ElectronTightIso_WithTrackMatch = ElectronTight_WithTrackMatch | CutDefElectron.ISOLATION_ELECTRON
	
	""" @brief Tight electron selection """
	ElectronTight = ElectronTight_WithTrackMatch
	""" @brief Tight without conversion requirement """
	ElectronTight_NoConvCut = ElectronTight_WithTrackMatch_NoConvCut
	""" @brief Tight electron selection with isolation requirement """
	ElectronTightIso = ElectronTightIso_WithTrackMatch
	
	""" @brief obsolete - kept not to crash clients """
	ElectronTightTRTNoIso = ElectronTight
	""" @brief obsolete - kept not to crash clients """
	ElectronTightNoIsolation = ElectronTight
	
	""" @brief Tight electron (corresponds to RobusterTight in 15.6.X with the loose deltaEta bit
	      TrackMatchEta_Electron but with out the tight one TrackMatchEtaTight_Electron) """
	ElectronTight_WithLooseEtaTrackMatch = CutDefElectron.CALO_ELECTRON | \
	   CutDefElectron.TRACKING_ELECTRON | CutDefElectron.TRACKMATCHDETA_ELECTRON | \
	   0x1 << BitDefElectron.TrackMatchEoverP_Electron | \
	   0x1 << BitDefElectron.TrackA0Tight_Electron | \
	   CutDefElectron.CONVMATCH_ELECTRON | CutDefElectron.TRT_ELECTRON 

	""" @brief Tight electron (corresponds to RobusterTight in 15.6.X with the deltaEta bits
	      TrackMatchEta_Electron and TrackMatchEtaTight_Electron) """
	ElectronTight_WithTightEtaTrackMatch = CutDefElectron.CALO_ELECTRON | \
	   CutDefElectron.TRACKING_ELECTRON | CutDefElectron.TRACKMATCHDETA_ELECTRON | \
	   CutDefElectron.TRACKMATCHTIGHT_ELECTRON | \
	   0x1 << BitDefElectron.TrackMatchEoverP_Electron |  \
	   CutDefElectron.CONVMATCH_ELECTRON | CutDefElectron.TRT_ELECTRON 
	
	""" @brief Tight_WithLooseEtaTrackMatch electron selection with isolation requirement """
	ElectronTightIso_WithLooseEtaTrackMatch =  ElectronTight_WithLooseEtaTrackMatch | \
	   CutDefElectron.ISOLATION_ELECTRON
	
	""" @brief Tight_WithTightEtaTrackMatch electron selection with isolation requirement """
	ElectronTightIso_WithTightEtaTrackMatch = ElectronTight_WithTightEtaTrackMatch | \
	   CutDefElectron.ISOLATION_ELECTRON
	
	""" @brief Tight electron (NB: kept for backward compatibility) """
	ElectronTightRobust = ElectronTight_WithLooseEtaTrackMatch
	
	#############
	### Added TrigEGamma Definitions
	""" @brief Loose1 Electron """
	ElectronLoose1 =  CutDefElectron.CALOMIDDLE_ELECTRON | \
                          CutDefElectron.HADLEAKETA_ELECTRON | \
                          CutDefElectron.CALOSTRIPS_LOOSE_ELECTRON | \
		          CutDefElectron.TRACKINGLOOSE_ELECTRON |  \
                          CutDefElectron.TRACKMATCHDETA_ELECTRON
	""" @brief Loose2 Electron """
	ElectronLoose2    = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
                            CutDefElectron.TRACKMATCHDETATIGHT_ELECTRON

	""" @brief Medium Electron for TRT chains """
	ElectronMedium_TRT = CutDefElectron.CALO_ELECTRON
	""" @brief Medium1 Electron definition for trigger """
	#AT: 7Sept2011, remove TRT Hits from medium1, following offline medium++ prescription
	ElectronMedium1    = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
		CutDefElectron.TRACKMATCHDETA_ELECTRON | \
		0x1 << BitDefElectron.TrackTRTratio_Electron | \
		0x1 << BitDefElectron.ClusterBackEnergyFraction_Electron
	""" @brief Medium2 Electron definition for trigger """
	ElectronMedium2    = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
		CutDefElectron.TRACKMATCHDETATIGHT_ELECTRON | \
		0x1 << BitDefElectron.TrackTRTratio_Electron
	## ElectronMedium2    = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
## 		CutDefElectron.TRACKMATCHDETATIGHT_ELECTRON | CutDefElectron.TRT_ELECTRON
	""" @brief Medium3 Electron definition for trigger """
	ElectronMedium3    = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
		CutDefElectron.TRACKMATCHDETATIGHT_ELECTRON | CutDefElectron.TRT_ELECTRON
	""" @brief Medium4 Electron definition for trigger """
	ElectronMedium4    = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
		CutDefElectron.TRACKMATCHDETATIGHT_ELECTRON | CutDefElectron.TRT_ELECTRON
   
	""" @brief Tight Electron definition for EF, removes ConversionMatch,TrackMatchPhi,TrackMatchEtaTight bits"""
	ElectronTightEF    = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
		CutDefElectron.TRACKMATCH_ELECTRON_EF | CutDefElectron.TRACKMATCHTIGHT_ELECTRON_EF | \
		CutDefElectron.TRT_ELECTRON
	""" @brief Tight Electron definition for EF, removes ConversionMatch,TrackMatchPhi, TrackMatchEoverP and add TrackMatchEtaTight bits"""
	ElectronTightEF_NoEoP_WithTightDeltaEta    = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
		CutDefElectron.TRACKMATCH_ELECTRON_NoEoP_EF | CutDefElectron.TRACKMATCHTIGHT_ELECTRON | \
		CutDefElectron.TRT_ELECTRON
	""" @brief Tight Electron definition for TRT chains in EF """
	ElectronTightEF_TRT = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRT_ELECTRON 
	""" @brief Tight Electron1 definition for e15_tight in EF """
	ElectronTight1 = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
		CutDefElectron.TRACKMATCHDETA_ELECTRON | CutDefElectron.TRACKMATCH_ELECTRON_EF | \
		CutDefElectron.TRACKMATCHTIGHT_ELECTRON | CutDefElectron.TRT_ELECTRON
	""" @brief Tight Electron definition for e5_tight """
	ElectronTight2 = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
		CutDefElectron.TRACKMATCHDETA_ELECTRON | CutDefElectron.TRT_ELECTRON | \
		0x1 << BitDefElectron.ClusterBackEnergyFraction_Electron
	""" @brief Tight Electron definition for e5_tight """
	ElectronHltTighter = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
		CutDefElectron.TRACKMATCHDETA_ELECTRON | CutDefElectron.TRT_ELECTRON | \
		CutDefElectron.TRACKMATCHTIGHT_ELECTRON_EF | CutDefElectron.TRACKMATCH_ELECTRON_EF

	""" @brief Electron definition for Et cut with track quality"""
	Electron_trk    = BitDefElectron.TrackSi_Electron | BitDefElectron.TrackPixel_Electron
	Electron_trkTRT = Electron_trk | CutDefElectron.TRT_ELECTRON
        
	""" @brief Loose electron for background trigger """
	ElectronLooseTrk = ElectronLoose | CutDefElectron.TRACKINGNOBLAYERNOA0_ELECTRON | \
                           CutDefElectron.TRACKMATCHDETA_ELECTRON

""" \enum Bitdefinitons for the egamma class for photon identification
    see egammaParameters for info on the variable definitions """
class BitDefPhoton:
   """ @brief cluster eta range """
   ClusterEtaRange_Photon             =  0
   """ @brief cluster eta range """
   ClusterEtaRange_PhotonLoose        =  1
   
   # selection for Loose photons
   """ @brief cluster leakage into the hadronic calorimeter """
   ClusterHadronicLeakage_PhotonLoose =  2
   """ @brief energy in 2nd sampling (e277) """
   ClusterMiddleEnergy_PhotonLoose    =  3 
   """ @brief energy ratio in 2nd sampling """
   ClusterMiddleEratio37_PhotonLoose  =  4
   """ @brief energy ratio in 2nd sampling """
   ClusterMiddleEratio33_PhotonLoose  =  5
   """ @brief width in the second sampling """
   ClusterMiddleWidth_PhotonLoose     =  6

   """ @brief energy fraction in the third layer """
   ClusterBackEnergyFraction_Photon = 7
   
   # selection for tight photons
   """ @brief cluster leakage into the hadronic calorimeter """
   ClusterHadronicLeakage_Photon      = 10
   """ @brief energy in 2nd sampling (e277) """
   ClusterMiddleEnergy_Photon         = 11 
   """ @brief energy ratio in 2nd sampling """
   ClusterMiddleEratio37_Photon       = 12
   """ @brief energy ratio in 2nd sampling for photons """
   ClusterMiddleEratio33_Photon       = 13
   """ @brief width in the second sampling """
   ClusterMiddleWidth_Photon          = 14
   
   """ @brief fraction of energy found in 1st sampling """
   ClusterStripsEratio_Photon         = 15
   """ @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et) """
   ClusterStripsDeltaEmax2_Photon     = 16
   """ @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1) """
   ClusterStripsDeltaE_Photon         = 17
   """ @brief shower width in 1st sampling """
   ClusterStripsWtot_Photon           = 18
   """ @brief shower shape in shower core 1st sampling """
   ClusterStripsFracm_Photon          = 19
   """ @brief shower width weighted by distance from the maximum one """
   ClusterStripsWeta1c_Photon         = 20
   """ @brief difference between max and 2nd max in strips """
   ClusterStripsDEmaxs1_Photon        = 21
   
   """ @brief energy-momentum match for photon selection"""
   TrackMatchEoverP_Photon            = 22
   
   """ @brief ambiguity resolution for photon (vs electron) """
   AmbiguityResolution_Photon         = 23
   
   """ @brief isolation """
   Isolation_Photon                   = 29
   """ @brief calorimetric isolation for photon selection """
   ClusterIsolation_Photon            = 30
   """ @brief tracker isolation for photon selection """
   TrackIsolation_Photon              = 31



#
# Photon cut definitions
#
class CutDefPhoton:
	""" @brief cuts of hadronic leakage (for Loose selection)"""
	HADLEAKETA_PHOTONLOOSE = \
	   0x1 << BitDefPhoton.ClusterEtaRange_PhotonLoose  | \
	   0x1 << BitDefPhoton.ClusterHadronicLeakage_PhotonLoose
	""" @brief cuts of hadronic leakage"""
	HADLEAKETA_PHOTON =  \
	   0x1 << BitDefPhoton.ClusterEtaRange_Photon        | \
	   0x1 << BitDefPhoton.ClusterHadronicLeakage_Photon
	""" @brief cuts in middle sampling (for Loose selection)"""
	CALOMIDDLE_PHOTONLOOSE= \
	   0x1 << BitDefPhoton.ClusterMiddleEnergy_PhotonLoose     | \
	   0x1 << BitDefPhoton.ClusterMiddleEratio37_PhotonLoose   | \
	   0x1 << BitDefPhoton.ClusterMiddleEratio33_PhotonLoose   | \
	   0x1 << BitDefPhoton.ClusterMiddleWidth_PhotonLoose
	""" @brief cuts in middle sampling"""
	CALOMIDDLE_PHOTON = \
	   0x1 << BitDefPhoton.ClusterMiddleEnergy_Photon     | \
	   0x1 << BitDefPhoton.ClusterMiddleEratio37_Photon   | \
	   0x1 << BitDefPhoton.ClusterMiddleEratio33_Photon   | \
	   0x1 << BitDefPhoton.ClusterMiddleWidth_Photon     
	""" @brief old cuts in strips (without ClusterStripsDEmaxs1)"""
	CALOSTRIPSOLD_PHOTON = \
	   0x1 << BitDefPhoton.ClusterStripsEratio_Photon     | \
	   0x1 << BitDefPhoton.ClusterStripsDeltaEmax2_Photon | \
	   0x1 << BitDefPhoton.ClusterStripsDeltaE_Photon     | \
	   0x1 << BitDefPhoton.ClusterStripsWtot_Photon       | \
	   0x1 << BitDefPhoton.ClusterStripsFracm_Photon      | \
	   0x1 << BitDefPhoton.ClusterStripsWeta1c_Photon     
	""" @brief cuts in strips (with ClusterStripsDEmaxs1)"""
	CALOSTRIPS_PHOTON = \
	   0x1 << BitDefPhoton.ClusterStripsEratio_Photon     | \
	   0x1 << BitDefPhoton.ClusterStripsDeltaEmax2_Photon | \
	   0x1 << BitDefPhoton.ClusterStripsDeltaE_Photon     | \
	   0x1 << BitDefPhoton.ClusterStripsWtot_Photon       | \
	   0x1 << BitDefPhoton.ClusterStripsFracm_Photon      | \
	   0x1 << BitDefPhoton.ClusterStripsWeta1c_Photon     | \
	   0x1 << BitDefPhoton.ClusterStripsDEmaxs1_Photon

	""" @brief calorimeter isolation"""
	CALORIMETRICISOLATION_PHOTON =  0x1 << BitDefPhoton.ClusterIsolation_Photon
	""" @brief "old" all cuts in calorimeter (except isolation) without ClusterStripsDEmaxs1 """
	CALONOISOOLD_PHOTON =  HADLEAKETA_PHOTON | CALOSTRIPSOLD_PHOTON | CALOMIDDLE_PHOTON 
	""" @brief all cuts in calorimeter (except isolation)"""
	CALO_PHOTON = HADLEAKETA_PHOTON | CALOSTRIPS_PHOTON | CALOMIDDLE_PHOTON
	
	""" @brief isolation by tracker """
	TRACKINGISOLATION_PHOTON =  0x1 << BitDefPhoton.TrackIsolation_Photon
	""" @brief isolation for photons is combination of calo and tracker """
	ISOLATION_PHOTON =  0x1 << BitDefPhoton.Isolation_Photon 
	""" @brief isolation for photons is combination of calo and tracker """
	CALOTRACKISOLATION_PHOTON =  CALORIMETRICISOLATION_PHOTON | TRACKINGISOLATION_PHOTON
	
	""" @brief Track cluster matching E/p for photons"""
	TRACKMATCH_PHOTON = 0x1 << BitDefPhoton.TrackMatchEoverP_Photon  
	
	""" @brief Ambigiuty resolve for photons """
	AMBIGUITYRESOLVE_PHOTON = 0x1 << BitDefPhoton.AmbiguityResolution_Photon
	
	#######
	## Added for Trigger
	""" @brief cuts of hadronic leakage (for Loose selection)"""
	HADLEAKETA_PHOTON_EF = \
		0x1 << BitDefPhoton.ClusterEtaRange_Photon | \
		0x1 << BitDefPhoton.ClusterHadronicLeakage_PhotonLoose

	""" @brief cuts of Eratio (for Medium selection)"""
	CALO_PHOTON_REAT_WETA2_ERATIO = \
		0x1 << BitDefPhoton.ClusterEtaRange_Photon | \
		0x1 << BitDefPhoton.ClusterMiddleEnergy_PhotonLoose | \
		0x1 << BitDefPhoton.ClusterMiddleEratio37_PhotonLoose | \
		0x1 << BitDefPhoton.ClusterMiddleWidth_PhotonLoose   | \
		0x1 << BitDefPhoton.ClusterStripsEratio_Photon




# Photon Selection Definitions
#
class SelectionDefPhoton:
	""" @brief Loose photon selection with Ambiguity resolver"""
	PhotonLooseAR =  CutDefPhoton.CALOMIDDLE_PHOTONLOOSE | CutDefPhoton.HADLEAKETA_PHOTONLOOSE | \
		CutDefPhoton.AMBIGUITYRESOLVE_PHOTON
	""" @brief Tight photon selection with Ambiguity resolver"""
	PhotonTightAR = CutDefPhoton.CALO_PHOTON | CutDefPhoton.TRACKMATCH_PHOTON | \
		CutDefPhoton.AMBIGUITYRESOLVE_PHOTON
	""" @brief Tight photon selection with isolation and Ambiguity resolver"""
	PhotonTightARIso = PhotonTightAR | CutDefPhoton.ISOLATION_PHOTON
	""" @brief Loose photon selection """
	PhotonLoose =  CutDefPhoton.CALOMIDDLE_PHOTONLOOSE | CutDefPhoton.HADLEAKETA_PHOTONLOOSE
	""" @brief Tight photon selection """
	PhotonTight = CutDefPhoton.CALO_PHOTON 
	""" @brief Tight photon selection with isolation"""
	PhotonTightIso = PhotonTight | CutDefPhoton.ISOLATION_PHOTON
	""" @brief Tight photon (old definition) """
	PhotonTightOLD = CutDefPhoton.CALONOISOOLD_PHOTON | CutDefPhoton.CALORIMETRICISOLATION_PHOTON
	
	#############
	### Added TrigEGamma Definitions
	""" @brief Loose photon selection for online EF"""
	PhotonLooseEF =  CutDefPhoton.CALOMIDDLE_PHOTONLOOSE | CutDefPhoton.HADLEAKETA_PHOTON_EF
   
        #Added for 2g20_medium
	""" @brief Medium photon selection for online EF""" 
	PhotonMediumEF = CutDefPhoton.HADLEAKETA_PHOTON_EF | CutDefPhoton.CALO_PHOTON_REAT_WETA2_ERATIO


print ""
print "==============================================================================="
print "====                       TrigEgamma isEM bit masks                      ====="
print "====  ElectronLoose:                              0x%08x              =====" % SelectionDefElectron.ElectronLoose
print "====  ElectronMedium:                             0x%08x              =====" % SelectionDefElectron.ElectronMedium
print "====  ElectronMedium1:                            0x%08x              =====" % SelectionDefElectron.ElectronMedium1
print "====  ElectronMedium2:                            0x%08x              =====" % SelectionDefElectron.ElectronMedium2
print "====  ElectronMedium3:                            0x%08x              =====" % SelectionDefElectron.ElectronMedium3
print "====  ElectronMedium4:                            0x%08x              =====" % SelectionDefElectron.ElectronMedium3
print "====  ElectronMedium_TRT:                         0x%08x              =====" % SelectionDefElectron.ElectronMedium_TRT
print "====  ElectronTight:                              0x%08x              =====" % SelectionDefElectron.ElectronTight
print "====  ElectronTightEF:                            0x%08x              =====" % SelectionDefElectron.ElectronTightEF
print "====  ElectronTightEF_TRT:                        0x%08x              =====" % SelectionDefElectron.ElectronTightEF_TRT
print "====  ElectronTightEF_NoEoP_WithTightDeltaEta:    0x%08x              =====" % SelectionDefElectron.ElectronTightEF_NoEoP_WithTightDeltaEta
print "====  ElectronTight1:                             0x%08x              =====" % SelectionDefElectron.ElectronTight1
print "====  ElectronTight2:                             0x%08x              =====" % SelectionDefElectron.ElectronTight2
print "====  ElectronHltTighter:                         0x%08x              =====" % SelectionDefElectron.ElectronHltTighter
print "====  PhotonLoose:                                0x%08x              =====" % SelectionDefPhoton.PhotonLoose
print "====  PhotonLooseEF:                              0x%08x              =====" % SelectionDefPhoton.PhotonLooseEF
print "====  PhotonMediumEF:                             0x%08x              =====" % SelectionDefPhoton.PhotonMediumEF
print "====  PhotonTight:                                0x%08x              =====" % SelectionDefPhoton.PhotonTight
print "==============================================================================="
print ""

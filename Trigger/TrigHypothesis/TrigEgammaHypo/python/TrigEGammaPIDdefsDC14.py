# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class TrigEgammaIDQualityDC14:
    ElectronIDLoose = 0
    ElectronIDMedium = 1
    ElectronIDTight = 2

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
           0x1 << BitDefElectron.ClusterMiddleEratio33_Electron   | \
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

class SelectionDefElectronDC14:
	
# New PID def class for DC14 tunes
# PID working points Loose, Medium, Tight
# Use PID defs from Loose1, Medium1, Tight1

	
	#############
	### Added TrigEGamma Definitions
	""" @brief Loose1 Electron """
	ElectronLoose =  CutDefElectron.CALOMIDDLE_ELECTRON | \
                          CutDefElectron.HADLEAKETA_ELECTRON | \
                          CutDefElectron.CALOSTRIPS_LOOSE_ELECTRON | \
		          CutDefElectron.TRACKINGLOOSE_ELECTRON |  \
                          CutDefElectron.TRACKMATCHDETA_ELECTRON
	""" @brief Medium1 Electron definition for trigger """
	#AT: 7Sept2011, remove TRT Hits from medium1, following offline medium++ prescription
	ElectronMedium    = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
		CutDefElectron.TRACKMATCHDETA_ELECTRON | \
		0x1 << BitDefElectron.TrackTRTratio_Electron | \
		0x1 << BitDefElectron.ClusterBackEnergyFraction_Electron
	""" @brief Tight Electron1 definition for e15_tight in EF """
	ElectronTight = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
		CutDefElectron.TRACKMATCHDETA_ELECTRON | CutDefElectron.TRACKMATCH_ELECTRON_EF | \
		CutDefElectron.TRACKMATCHTIGHT_ELECTRON | CutDefElectron.TRT_ELECTRON

print ""
print "==============================================================================="
print "====                       TrigEgamma isEM bit masks for DC14             ====="
print "====  ElectronLoose:                              0x%08x              =====" % SelectionDefElectronDC14.ElectronLoose
print "====  ElectronMedium:                             0x%08x              =====" % SelectionDefElectronDC14.ElectronMedium
print "====  ElectronTight:                              0x%08x              =====" % SelectionDefElectronDC14.ElectronTight
print "==============================================================================="
print ""

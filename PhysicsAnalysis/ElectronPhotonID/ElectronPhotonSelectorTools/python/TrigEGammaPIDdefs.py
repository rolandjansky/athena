# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

class TrigEgammaIDQuality:
    ElectronIDLoose = 0
    ElectronIDLoose0 = 1
    ElectronIDLoose1 = 2
    ElectronIDLoose2 = 3
    ElectronIDMedium = 4
    ElectronIDMedium1 = 5
    ElectronIDMedium2 = 6
    ElectronIDMedium3 = 7
    ElectronIDMedium_TRT = 8
    ElectronIDTight = 9
    ElectronIDTightEF = 10
    ElectronIDTightEF_TRT = 11
    ElectronIDTightEF_NoEoP_WithTightDeltaEta = 12
    ElectronIDTight1 = 13
    ElectronIDTight2 = 14
    ElectronIDHltTighter = 15
    PhotonIDLoose = 16
    PhotonIDLooseEF = 17
    PhotonIDMediumEF = 18
    PhotonIDTight = 19
    PhotonIDLooseEFTauMass = 20
    ElectronIDLooseHLT = 21
    ElectronIDMediumHLT = 22
    ElectronIDTightHLT = 23

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
   """ @brief Cut on the TRT eProbabilityHT_Electron """
   TrackTRTeProbabilityHT_Electron   = 23
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
           0x1 << BitDefElectron.TrackTRTratio_Electron  | \
           0x1 << BitDefElectron.TrackTRTeProbabilityHT_Electron
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
        
        #############
        ### Added TrigEGamma Definitions
        ElectronLoose1 =  CutDefElectron.CALOMIDDLE_ELECTRON | \
                          CutDefElectron.HADLEAKETA_ELECTRON | \
                          CutDefElectron.CALOSTRIPS_LOOSE_ELECTRON | \
                          CutDefElectron.TRACKINGLOOSE_ELECTRON |  \
                          CutDefElectron.TRACKMATCHDETA_ELECTRON
        #AT: 7Sept2011, remove TRT Hits from medium1, following offline medium++ prescription
        ElectronMedium1    = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
                CutDefElectron.TRACKMATCHDETA_ELECTRON | \
                0x1 << BitDefElectron.TrackTRTratio_Electron | \
                0x1 << BitDefElectron.TrackTRTeProbabilityHT_Electron | \
                0x1 << BitDefElectron.ClusterBackEnergyFraction_Electron
        """ @brief Tight Electron1 definition for e15_tight in EF """
        ElectronTight1 = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
                CutDefElectron.TRACKMATCHDETA_ELECTRON | CutDefElectron.TRACKMATCH_ELECTRON_EF | \
                CutDefElectron.TRACKMATCHTIGHT_ELECTRON | CutDefElectron.TRT_ELECTRON
        
        #############
        ### Added TrigEGammaDC14 Definitions for Run2
        """ @brief Loose1 with RPhi Electron """
        ElectronLooseHLT =  CutDefElectron.CALOMIDDLE_ELECTRON | \
                          CutDefElectron.HADLEAKETA_ELECTRON | \
                          CutDefElectron.CALOSTRIPS_LOOSE_ELECTRON | \
                          CutDefElectron.TRACKINGLOOSE_ELECTRON |  \
                          CutDefElectron.TRACKMATCHDETA_ELECTRON 
        """ @brief Medium1 with Rphi Electron definition for trigger """
        #AT: 7Sept2011, remove TRT Hits from medium1, following offline medium++ prescription
        ElectronMediumHLT    = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
                CutDefElectron.TRACKMATCHDETA_ELECTRON | \
                CutDefElectron.TRACKMATCHDETATIGHT_ELECTRON | \
                0x1 << BitDefElectron.TrackTRTratio_Electron | \
                0x1 << BitDefElectron.TrackTRTeProbabilityHT_Electron | \
                0x1 << BitDefElectron.ClusterBackEnergyFraction_Electron | \
                0x1 << BitDefElectron.ClusterMiddleEratio33_Electron

        """ @brief Tight with Rphi&f3 Electron1 definition """
        ElectronTightHLT = CutDefElectron.CALO_ELECTRON | CutDefElectron.TRACKING_ELECTRON | \
                CutDefElectron.TRACKMATCHDETA_ELECTRON | \
                CutDefElectron.TRACKMATCHTIGHT_ELECTRON | \
                CutDefElectron.TRT_ELECTRON | \
                0x1 << BitDefElectron.ClusterBackEnergyFraction_Electron | \
                0x1 << BitDefElectron.ClusterMiddleEratio33_Electron

        Electron_trk = CutDefElectron.TRACKINGLOOSE_ELECTRON
        

r""" \enum Bitdefinitons for the egamma class for photon identification
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

        """ @brief cuts in strips (with ClusterStripsDEmaxs1)"""
        CALOSTRIPS_PHOTONTIGHT = \
            0x1 << BitDefPhoton.ClusterStripsEratio_Photon     | \
            0x1 << BitDefPhoton.ClusterStripsDeltaEmax2_Photon | \
            0x1 << BitDefPhoton.ClusterStripsDeltaE_Photon     | \
            0x1 << BitDefPhoton.ClusterStripsWtot_Photon       | \
            0x1 << BitDefPhoton.ClusterStripsFracm_Photon      | \
            0x1 << BitDefPhoton.ClusterStripsWeta1c_Photon     | \
            0x1 << BitDefPhoton.ClusterStripsDEmaxs1_Photon    

        """ @brief cuts in strips (with ClusterStripsDEmaxs1)"""
        CALOSTRIPS_PHOTONMEDIUM = \
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
        PhotonLoose =  CutDefPhoton.CALOMIDDLE_PHOTON | CutDefPhoton.HADLEAKETA_PHOTON
        """ @brief Medium photon selection """
        PhotonMedium = CutDefPhoton.HADLEAKETA_PHOTON | CutDefPhoton.CALOMIDDLE_PHOTON | CutDefPhoton.CALOSTRIPS_PHOTONMEDIUM
        """ @brief Tight photon selection """
        PhotonTight = CutDefPhoton.HADLEAKETA_PHOTON | CutDefPhoton.CALOMIDDLE_PHOTON | CutDefPhoton.CALOSTRIPS_PHOTONTIGHT
        """ @brief Tight photon selection with isolation"""
        PhotonTightIso = PhotonTight | CutDefPhoton.ISOLATION_PHOTON
        
        #############
        ### Added TrigEGamma Definitions
        """ @brief Loose photon selection for online EF"""
        PhotonLooseEF =  CutDefPhoton.CALOMIDDLE_PHOTONLOOSE | CutDefPhoton.HADLEAKETA_PHOTON_EF
   
        #Added for 2g20_medium
        """ @brief Medium photon selection for online EF""" 
        PhotonMediumEF = CutDefPhoton.HADLEAKETA_PHOTON_EF | CutDefPhoton.CALO_PHOTON_REAT_WETA2_ERATIO


print ("")
print ("===============================================================================")
print ("====                       TrigEgamma isEM bit masks                      =====")
print ("====  ElectronLoose1:                              0x%08x              =====" % SelectionDefElectron.ElectronLoose1)
print ("====  ElectronMedium1:                             0x%08x              =====" % SelectionDefElectron.ElectronMedium1)
print ("====  ElectronTight1:                              0x%08x              =====" % SelectionDefElectron.ElectronTight1)
print ("====  ElectronLooseHLT:                            0x%08x              =====" % SelectionDefElectron.ElectronLooseHLT)
print ("====  ElectronMediumHLT:                           0x%08x              =====" % SelectionDefElectron.ElectronMediumHLT)
print ("====  ElectronTightHLT:                            0x%08x              =====" % SelectionDefElectron.ElectronTightHLT)
print ("====  PhotonLoose:                                 0x%08x              =====" % SelectionDefPhoton.PhotonLoose)
print ("====  PhotonLooseEF:                               0x%08x              =====" % SelectionDefPhoton.PhotonLooseEF)
print ("====  PhotonMedium:                                0x%08x              =====" % SelectionDefPhoton.PhotonMedium)
print ("====  PhotonMediumEF:                              0x%08x              =====" % SelectionDefPhoton.PhotonMediumEF)
print ("====  PhotonTight:                                 0x%08x              =====" % SelectionDefPhoton.PhotonTight)
print ("===============================================================================")
print ("")

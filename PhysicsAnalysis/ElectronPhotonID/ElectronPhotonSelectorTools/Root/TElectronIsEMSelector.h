/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __TELECTRONISEMSELECTOR__
#define __TELECTRONISEMSELECTOR__

/**
   @class TElectronIsEMSelector
   @brief Example tool to select objects in pure ROOT

   @author Karsten Koeneke (CERN), Jovan Mitrevski (UCSC)
   @date   April 2011 - Feb 2012

   based on egammaElectronCutIDTool from F. Derue

*/
// Include the return object and the base class
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"
#include "AsgMessaging/AsgMessaging.h"
#include "AsgMessaging/StatusCode.h"
#include <vector>
#include <string>

class AsgElectronIsEMSelector;

namespace Root {
  class TElectronIsEMSelector : public asg::AsgMessaging
  {

    friend class ::AsgElectronIsEMSelector;

  public: 
    /** Standard constructor */
    TElectronIsEMSelector(const char* name="TElectronIsEMSelector");

    /** Standard destructor */
    ~TElectronIsEMSelector();
    

    // Main methods
    /** Initialize this class */
    StatusCode initialize();

    /** The main accept method: the actual cuts are applied here */
    asg::AcceptData accept(
				// eta position in second sampling
				float eta2,
				// transverse energy in calorimeter (using eta position in second sampling)
				double et,
				//////////////// - calo
				// E(3*3) in 2nd sampling
				float Reta, //e233,
				// E(3*7) in 2nd sampling
				float Rphi, //e237,
				// transverse energy in 1st scintillator of hadronic calorimeter
				float Rhad1, //ethad1,
				// transverse energy in hadronic calorimeter
				float Rhad, //ethad,
				// E(7*7) in 2nd sampling
				float e277,
				// shower width in 3 strips in 1st sampling
				float weta1c,
				// shower width in 2nd sampling
				float weta2c,
				// fraction of energy reconstructed in the 1st sampling
				float f1,
				// E of 2nd max between max and min in strips
				float emax2,
				// E of 1st max in strips
				float Eratio, //emax,
				// E(min) in strips
				float DeltaE, //emin,
				// total shower width in 1st sampling
				float wtot,
				// E(+/-3)-E(+/-1)/E(+/-1)
				float fracm,
				// fraction of energy reconstructed in the 3rd sampling
				float f3,
				//////////////// - tracking
                                // is effective number of BL hits+outliers at least 1?
                                bool passBLayerRequirement,
                                // number of pixel hits + dead sensors
                                int nPixHitsPlusDeadSensors,
                                // number of silicon hits + dead sensors
                                int nSiHitsPlusDeadSensors,
				// TRT hits
				int nTRThigh,
				int nTRThighOutliers,
				int nTRT,
				int nTRTOutliers,
				int nTRTXenonHits,
				float TRT_PID,
				// transverse impact parameter
				float trackd0,
				// Delta eta,phi matching
				float deltaeta,
				float deltaphi,
				// E/p
				double ep) const;

      /** Return dummy accept with only info */
      asg::AcceptData accept() const { return asg::AcceptData(&m_acceptInfo); }

      // calculate the isEM. (Used internally by accept)
    unsigned int calcIsEm(
			  // eta position in second sampling
			  float eta2,
			  // transverse energy in calorimeter (using eta position in second sampling)
			  double et,
			  //////////////// - calo
			  // E(3*3) in 2nd sampling
			  float Reta, //e233,
			  // E(3*7) in 2nd sampling
			  float Rphi, //e237,
			  // transverse energy in 1st scintillator of hadronic calorimeter
			  float Rhad1, //ethad1,
			  // transverse energy in hadronic calorimeter
			  float Rhad, //ethad,
			  // E(7*7) in 2nd sampling
			  float e277,
			  // shower width in 3 strips in 1st sampling
			  float weta1c,
			  // shower width in 2nd sampling
			  float weta2c,
			  // fraction of energy reconstructed in the 1st sampling
			  float f1,
			  // E of 2nd max between max and min in strips
			  float emax2,
			  // E of 1st max in strips
			  float Eratio, //emax,
			  // E(min) in strips
			  float DeltaE, //emin,
			  // total shower width in 1st sampling
			  float wtot,
			  // E(+/-3)-E(+/-1)/E(+/-1)
			  float fracm,
			  // fraction of energy reconstructed in the 3rd sampling
			  float f3,
			  //////////////// - tracking
                          // is effective number of BL hits+outliers at least 1?
                          bool passBLayerRequirement,
                          // number of pixel hits + dead sensors
                          int nPixHitsPlusDeadSensors,
                          // number of silicon hits + dead sensors
                          int nSiHitsPlusDeadSensors,
			  // TRT hits
			  int nTRThigh,
			  int nTRThighOutliers,
			  int nTRT,
			  int nTRTOutliers,
			  int nTRTXenonHits,
			  float TRT_PID,
			  // transverse impact parameter
			  float trackd0,
			  // Delta eta,phi matching
			  float deltaeta,
			  float deltaphi,
			  // E/p
			  double ep) const;

    // used internally by calcIsEm, but left public because it can be useful for users. Note that
    // it doesn't necessarily zero all the values that pass, so make sure that the input iflag
    // bits that you are concerned with start out with 0.
    unsigned int calocuts_electrons(
				    // eta position in second sampling
				    float eta2,
				    // transverse energy in calorimeter (using eta position in second sampling)
				    double et,
				    // E(3*3) in 2nd sampling
				    float Reta, //e233,
				    // E(3*7) in 2nd sampling
				    float Rphi, //e237,
				    // transverse energy in 1st scintillator of hadronic calorimeter
				    float Rhad1, //ethad1,
				    // transverse energy in hadronic calorimeter
				    float Rhad, //ethad,
				    // E(7*7) in 2nd sampling
				    float e277,
				    // shower width in 3 strips in 1st sampling
				    float weta1c,
				    // shower width in 2nd sampling
				    float weta2c,
				    // fraction of energy reconstructed in the 1st sampling
				    float f1,
				    // E of 2nd max between max and min in strips
				    float emax2,
				    // E of 1st max in strips
				    float Eratio, //emax,
				    // E(min) in strips
				    float DeltaE, //emin,
				    // total shower width in 1st sampling
				    float wtot,
				    // E(+/-3)-E(+/-1)/E(+/-1)
				    float fracm,
				    // fraction of energy reconstructed in the 3rd sampling
				    float f3,
				    // the starting isEM (should be 0 if new)
				    unsigned int iflag,
                                    // trigETthr : threshold in ET to apply the cuts at trigger level
                                    double trigEtTh=0
				    ) const;

    // used internally by calcIsEm, but left public because it can be useful for users. Note that
    // it doesn't necessarily zero all the values that pass, so make sure that the input iflag
    // bits that you are concerned with start out with 0.
    unsigned int TrackCut(  
			  // eta of the cluster in the 2nd sampling
			  float eta2,
			  // transverse energy in calorimeter (using eta position in second sampling)
			  double et,
                          // is effective number of BL hits+outliers at least 1?
                          bool passBLayerRequirement,
                          // number of pixel hits + dead sensors
                          int nPixHitsPlusDeadSensors,
                          // number of silicon hits + dead sensors
                          int nSiHitsPlusDeadSensors,
			  // TRT hits
			  int nTRThigh,
			  int nTRThighOutliers,
			  int nTRT,
			  int nTRTOutliers,
			  int nTRTXenonHits,
			  float TRT_PID,
			  // transverse impact parameter
			  float trackd0,
			  // Delta eta,phi matching
			  float deltaeta,
			  float deltaphi,
			  // E/p
			  double ep,
			  unsigned int iflag) const;


    ///////////////////////////////////
    // Public members (the cut values)
    ///////////////////////////////

    /** @brief which subset of cuts to apply */
    unsigned int m_isEMMask;

    /** @brief use of TRT outliers*/
    bool m_useTRTOutliers;
    /** @brief use of TRT Xenon Hits*/ 
    bool m_useTRTXenonHits; 

    /** @brief range of eta bins for e-ID*/
    std::vector<float> m_cutBinEta;
    /** @brief range of ET bins for e-ID*/
    std::vector<float> m_cutBinET;
    /** @brief cut on fraction of energy deposited in 1st sampling for e-ID*/
    std::vector<float> m_cutF1;
    /** @brief cut on hadronic energy for e-ID */
    std::vector<float> m_cutHadLeakage;
    /** @brief cut on ratio e237/e277 for e-ID*/
    std::vector<float> m_cutReta37;
    /** @brief cut on ratio e233/e277 for e-ID*/
    std::vector<float> m_cutRphi33;
    /** @brief cut on shower width in 2nd sampling for e-ID*/
    std::vector<float> m_cutWeta2c;
    /** @brief cut on Delta Emax2 in 1st sampling for e-ID*/
    std::vector<float> m_cutDeltaEmax2;
    /** @brief cut on Emax2 - Emin in 1st sampling for e-ID*/
    std::vector<float> m_cutDeltaE;
    /** @brief cut on (Emax1-Emax2)/(Emax1-Emax2) for e-ID*/
    std::vector<float> m_cutDEmaxs1;
    /** @brief cut on total width in 1st sampling for e-ID*/
    std::vector<float> m_cutWtot;
    /** @brief cut on width in 1st sampling for e-ID*/
    std::vector<float> m_cutWeta1c;
    /** @brief cut on Fside in 1st sampling for e-ID*/
    std::vector<float> m_cutFracm;
    
    /** @brief cut values for cut on f3 */
    std::vector<float> m_cutF3;

    /** @brief cut min on b-layer hits for e-ID*/
    std::vector<int> m_cutBL;
    /** @brief cut min on pixel hits for e-ID*/
    std::vector<int> m_cutPi;
    /** @brief cut min on precision hits for e-ID*/
    std::vector<int> m_cutSi;
    /** @brief cut min on transverse impact parameter for e-ID*/
    std::vector<float> m_cutA0;
    /** @brief cut min on transverse impact parameter for Tight e-ID*/
    std::vector<float> m_cutA0Tight;
    /** @brief cut max on delta eta for e-ID*/
    std::vector<float> m_cutDeltaEta;
    /** @brief cut max on delta eta for Tight e-ID*/
    std::vector<float> m_cutDeltaEtaTight;
    /** @brief cut min on delta phi for e-ID (this should be negative) */
    std::vector<float> m_cutminDeltaPhi;
    /** @brief cut max on delta phi for e-ID*/
    std::vector<float> m_cutmaxDeltaPhi;
    /** @brief cut min on E/p for e-ID*/
    std::vector<float> m_cutminEp;
    /** @brief cut max on E/p for e-ID*/
    std::vector<float> m_cutmaxEp;
    
    /** @brief Eta binning for cuts on TRT for e-ID*/
    std::vector<float> m_cutBinEta_TRT;
    /** @brief Et binning for cuts on TRT for e-ID*/
    std::vector<float>  m_cutBinET_TRT;
    /** @brief cut on Number of TRT hits for e-ID*/
    std::vector<float> m_cutNumTRT;
    /** @brief cut on Ratio of TR hits to Number of TRT hits for e-ID*/
    std::vector<float> m_cutTRTRatio;
    /** @brief cut on Ratio of TR hits to Number of TRT hits for 10% loss due to TRT */
    std::vector<float> m_cutTRTRatio90;
    /** @brief cut on on eProbabilityHT new TRT PID tool */
    std::vector<float> m_cutEProbabilityHT;

    const asg::AcceptInfo& getAcceptInfo() const { return m_acceptInfo; }
      
    // Private members
  private:

    // would ideally be protected: only to be used by ARASelector
    asg::AcceptData fillAccept(unsigned int isEM) const;

    std::vector<int> FindEtEtaBin(double et, double eta2) const;

    template<typename T>
    bool CheckVar(const std::vector<T>& vec, int choice) const;

    /// Accept info
    asg::AcceptInfo     m_acceptInfo;
      
    // the cut positions

    /** @brief cluster eta range */
    int m_cutPositionClusterEtaRange_Electron;
    
    /** @brief matching to photon (not necessarily conversion--the name is historical) */
    int m_cutPositionConversionMatch_Electron;
    
    /** @brief cluster leakage into the hadronic calorimeter */
    int m_cutPositionClusterHadronicLeakage_Electron;
    /** @brief Et<0 cut */
    int m_cutPositionClusterMiddleEnergy_Electron;
    /** @brief energy ratio in 2nd sampling (e.g E237/E277) */
    int m_cutPositionClusterMiddleEratio37_Electron;
    /** @brief energy ratio in 2nd sampling (e.g E233/E237) */
    int m_cutPositionClusterMiddleEratio33_Electron;
    /** @brief width in the second sampling (e.g Weta2) */
    int m_cutPositionClusterMiddleWidth_Electron;

    /** @brief energy fraction in the third layer */
    int m_cutPositionClusterBackEnergyFraction_Electron;

    /** @brief fraction of energy found in 1st sampling (NB: not used in fact for electrons)*/
    int m_cutPositionClusterStripsEratio_Electron;
    /** @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et) */
    int m_cutPositionClusterStripsDeltaEmax2_Electron;
    /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1) */
    int m_cutPositionClusterStripsDeltaE_Electron;
    /** @brief shower width in 1st sampling */
    int m_cutPositionClusterStripsWtot_Electron;
    /** @brief shower shape in shower core 1st sampling */
    int m_cutPositionClusterStripsFracm_Electron;
    /** @brief shower width weighted by distance from the maximum one */
    int m_cutPositionClusterStripsWeta1c_Electron;

    /** @brief difference between max and 2nd max in strips */
    int m_cutPositionClusterStripsDEmaxs1_Electron;
    /** @brief B layer hit */
    int m_cutPositionTrackBlayer_Electron;
    /** @brief number of Pixel hits */
    int m_cutPositionTrackPixel_Electron;
    /** @brief number of Pixel and SCT hits */
    int m_cutPositionTrackSi_Electron;
    /** @brief distance of closet approach */
    int m_cutPositionTrackA0_Electron;
    /** @brief eta difference between cluster and extrapolated track in the 1st sampling */
    int m_cutPositionTrackMatchEta_Electron;
    /** @brief phi difference between cluster and extrapolated track in the 2nd sampling */
    int m_cutPositionTrackMatchPhi_Electron;
    /** @brief energy-momentum match */
    int m_cutPositionTrackMatchEoverP_Electron;
    /** @brief Cut on the TRT eProbabilityHT_Electron */
    int m_cutPositionTrackTRTeProbabilityHT_Electron;
    /** @brief number of TRT hits */
    int m_cutPositionTrackTRThits_Electron;
    /** @brief ratio of high to all TRT hits for isolated electrons */
    int m_cutPositionTrackTRTratio_Electron;
    /** @brief ratio of high to all TRT hits for non-isolated electrons (not for new ++ menus) */    
    int m_cutPositionTrackTRTratio90_Electron;

    /** @brief distance of closet approach for tight selection (not to be used in new ++ menus) */
    int m_cutPositionTrackA0Tight_Electron;
    /** @brief eta difference between cluster and extrapolated track in the 1st sampling for 
	tight selection (not to be used in new ++ menus)*/
    int m_cutPositionTrackMatchEtaTight_Electron;

    /** @brief isolation */
    int m_cutPositionIsolation_Electron;
    /** @brief calorimetric isolation */
    int m_cutPositionClusterIsolation_Electron;
    /** @brief tracker isolation */
    int m_cutPositionTrackIsolation_Electron;

    // the cut names

    /** @brief cluster eta range */
    const std::string m_cutNameClusterEtaRange_Electron;
    
    /** @brief matching to photon (not necessarily conversion--the name is historical) */
    const std::string m_cutNameConversionMatch_Electron;
    
    /** @brief cluster leakage into the hadronic calorimeter */
    const std::string m_cutNameClusterHadronicLeakage_Electron;
    /** @brief energy in 2nd sampling (e.g E277>0) */
    const std::string m_cutNameClusterMiddleEnergy_Electron;
    /** @brief energy ratio in 2nd sampling (e.g E237/E277) */
    const std::string m_cutNameClusterMiddleEratio37_Electron;
    /** @brief energy ratio in 2nd sampling (e.g E233/E237) */
    const std::string m_cutNameClusterMiddleEratio33_Electron;
    /** @brief width in the second sampling (e.g Weta2) */
    const std::string m_cutNameClusterMiddleWidth_Electron;

    /** @brief energy fraction in the third layer */
    const std::string m_cutNameClusterBackEnergyFraction_Electron;

    /** @brief fraction of energy found in 1st sampling (NB: not used in fact for electrons)*/
    const std::string m_cutNameClusterStripsEratio_Electron;
    /** @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et) */
    const std::string m_cutNameClusterStripsDeltaEmax2_Electron;
    /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1) */
    const std::string m_cutNameClusterStripsDeltaE_Electron;
    /** @brief shower width in 1st sampling */
    const std::string m_cutNameClusterStripsWtot_Electron;
    /** @brief shower shape in shower core 1st sampling */
    const std::string m_cutNameClusterStripsFracm_Electron;
    /** @brief shower width weighted by distance from the maximum one */
    const std::string m_cutNameClusterStripsWeta1c_Electron;

    /** @brief difference between max and 2nd max in strips */
    const std::string m_cutNameClusterStripsDEmaxs1_Electron;
    /** @brief B layer hit */
    const std::string m_cutNameTrackBlayer_Electron;
    /** @brief number of Pixel hits */
    const std::string m_cutNameTrackPixel_Electron;
    /** @brief number of Pixel and SCT hits */
    const std::string m_cutNameTrackSi_Electron;
    /** @brief distance of closet approach */
    const std::string m_cutNameTrackA0_Electron;
    /** @brief eta difference between cluster and extrapolated track in the 1st sampling */
    const std::string m_cutNameTrackMatchEta_Electron;
    /** @brief phi difference between cluster and extrapolated track in the 2nd sampling */
    const std::string m_cutNameTrackMatchPhi_Electron;
    /** @brief energy-momentum match */
    const std::string m_cutNameTrackMatchEoverP_Electron;
    /** @brief Cut on the TRT eProbabilityHT_Electron */
    const std::string m_cutNameTrackTRTeProbabilityHT_Electron;
    /** @brief number of TRT hits */
    const std::string m_cutNameTrackTRThits_Electron;
    /** @brief ratio of high to all TRT hits for isolated electrons */
    const std::string m_cutNameTrackTRTratio_Electron;
    /** @brief ratio of high to all TRT hits for non-isolated electrons (not for new ++ menus) */    
    const std::string m_cutNameTrackTRTratio90_Electron;

    /** @brief distance of closet approach for tight selection (not to be used in new ++ menus) */
    const std::string m_cutNameTrackA0Tight_Electron;
    /** @brief eta difference between cluster and extrapolated track in the 1st sampling for 
	tight selection (not to be used in new ++ menus)*/
    const std::string m_cutNameTrackMatchEtaTight_Electron;

    /** @brief isolation */
    const std::string m_cutNameIsolation_Electron;
    /** @brief calorimetric isolation */
    const std::string m_cutNameClusterIsolation_Electron;
    /** @brief tracker isolation */
    const std::string m_cutNameTrackIsolation_Electron;



  }; // End: class definition

} // End: namespace Root

#endif


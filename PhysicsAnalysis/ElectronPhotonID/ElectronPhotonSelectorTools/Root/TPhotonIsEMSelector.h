/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Dear emacs, this is -*-c++-*-

#ifndef __TPHOTONISEMSELECTOR__
#define __TPHOTONISEMSELECTOR__

/**
   @class TPhotonIsEMSelector
   @brief Example tool to select objects in pure ROOT

   @author Karsten Koeneke (CERN), Jovan Mitrevski (UCSC)
   @date   April 2011 - Feb 2012

   based on egammaPhotonCutIDTool from F. Derue

*/


// ROOT includes
#include <TString.h>

// Include the return object and the base class
#include "PATCore/TAccept.h"
#include "PATCore/TSelectorToolBase.h"
#include "EgammaAnalysisInterfaces/egammaPIDdefs.h"
#include "AsgTools/AsgMessaging.h"

#include <vector>


class AsgPhotonIsEMSelector;

namespace Root {
  class TPhotonIsEMSelector : public TSelectorToolBase,public asg::AsgMessaging
  {
    
    friend class ::AsgPhotonIsEMSelector;
    

  public: 
    /** Standard constructor */
    TPhotonIsEMSelector(const char* name="TPhotonIsEMSelector");
    
    /** Standard destructor */
    ~TPhotonIsEMSelector();

    // Main methods
    /** Initialize this class */
    int initialize();

    /** Finalize this class; everything that should be done after the event loop should go here */
    inline int finalize() { return 1; };


    /** The main accept method: the actual cuts are applied here */
    const Root::TAccept& accept(
				// eta position in second sampling
				float eta2,
				// transverse energy in calorimeter (using eta position in second sampling)
				double et,
				// transverse energy in 1st scintillator of hadronic calorimeter/ET
				float Rhad1,
				// transverse energy in hadronic calorimeter/ET
				float Rhad,
				// E(7*7) in 2nd sampling
				float e277,
				// E(3*7)/E(7*7) in 2nd sampling
				float Reta,
				// E(3*3)/E(3*7) in 2nd sampling
				float Rphi,
				// shower width in 2nd sampling
				float weta2c,
				// fraction of energy reconstructed in strips
				float f1,
				// (E of 1st max in strips-E of 2nd max)/(E of 1st max+E of 2nd max)
				float Eratio,
				// E(2nd max)-E(min) in strips
				float DeltaE,
				//  E of 2nd max between max and min in strips
				// float emax2,
				// shower width in 3 strips in 1st sampling
				float weta1c,
				// total shower width in strips
				float wtot,
				// E(+/-3)-E(+/-1)/E(+/-1)
				float fracm,
				// fraction of energy reconstructed in the 3rd sampling
				float f3,
				// E/p
				double ep,
				// is it a conversion
				bool isConversion) ;

    // calculate the isEM. (Used internally by accept)
    unsigned int calcIsEm(
			  // eta position in second sampling
			  float eta2,
			  // transverse energy in calorimeter (using eta position in second sampling)
			  double et,
			  // transverse energy in 1st scintillator of hadronic calorimeter/ET
			  float Rhad1,
			  // transverse energy in hadronic calorimeter/ET
			  float Rhad,
			  // E(7*7) in 2nd sampling
			  float e277,
			  // E(3*7)/E(7*7) in 2nd sampling
			  float Reta,
			  // E(3*3)E(3*7) in 2nd sampling
			  float Rphi,
			  // shower width in 2nd sampling
			  float weta2c,
			  // fraction of energy reconstructed in strips
			  float f1,
			  // (E of 1st max in strips-E of 2nd max)/(E of 1st max+E of 2nd max)
			  float Eratio,
			  // E(2nd max)-E(min) in strips
			  float DeltaE,
			  // shower width in 3 strips in 1st sampling
			  //  E of 2nd max between max and min in strips
			  // shower width in 3 strips in 1st sampling
			  float weta1c,
			  // total shower width in strips
			  float wtot,
			  // E(+/-3)-E(+/-1)/E(+/-1)
			  float fracm,
			  // fraction of energy reconstructed in the 3rd sampling
			  float f3,
			  // E/p
			  double ep,
			  // is it a conversion
			  bool isConversion ) const ;

    /** @brief Apply calorimeter cuts for selection of converted photons*/
    unsigned int calocuts_photonsConverted(
					   // eta position in second sampling
					   float eta2,
					   // transverse energy in calorimeter 
					   double et,
					   // hadronic leakage ratios
					   float Rhad1,
					   float Rhad,
					   // E(7*7) in 2nd sampling
					   float e277,
					   // ratios
					   float Reta,
					   float Rphi,
					   // shower width in 2nd sampling
					   float weta2c,
					   // fraction of energy reconstructed in strips
					   float f1,
					   // (Emax1-Emax2)/(Emax1+Emax2)
					   float Eratio,
					   // difference of energy between max and min
					   float DeltaE,
					   // parametrization of E(2nd max)
					   //float deltaemax2,
					   // shower width in 3 strips in 1st sampling
					   float weta1c,
					   // total shower width in strips
					   float wtot,
					   // E(+/-3)-E(+/-1)/E(+/-1)
					   float fracm,
					   // fraction of energy reconstructed in the 3rd sampling
					   float f3,
					   // E/p
					   double ep,
					   unsigned int iflag) const;

    /** @brief Apply calorimeter cuts for selection of non converted photons*/
    unsigned int calocuts_photonsNonConverted(
					      // eta position in second sampling
					      float eta2,
					      // transverse energy in calorimeter 
					      double et,
					      // hadronic leakage ratios
					      float Rhad1,
					      float Rhad,
					      // E(7*7) in 2nd sampling
					      float e277,
					      // ratios
					      float Reta,
					      float Rphi,
					      // shower width in 2nd sampling
					      float weta2c,
					      // fraction of energy reconstructed in strips
					      float f1,
					      // (Emax1-Emax2)/(Emax1+Emax2)
					      float Eratio,
					      // difference of energy between max and min
					      float DeltaE,
					      // parametrization of E(2nd max)
					      //float deltaemax2,
					      // shower width in 3 strips in 1st sampling
					      float weta1c,
					      // total shower width in strips
					      float wtot,
					      // E(+/-3)-E(+/-1)/E(+/-1)
					      float fracm,
					      // fraction of energy reconstructed in the 3rd sampling
					      float f3,
					      unsigned int iflag) const;
    

    unsigned int isEM() const {return m_isEM; };
    //unsigned int isEMMask() const {return m_isEMMask; } // user should not need this


    ///////////////////////////////////
    // Public members (the cut values)
    ///////////////////////////////

    /** @brief which subset of cuts to apply */
    unsigned int isEMMask;


    /** @brief boolean to force to test converted photon hypothesis */
    bool forceConvertedPhotonPID;
    /** @brief boolean to force to test non converted photon hypothesis */
    bool forceNonConvertedPhotonPID;

    
    //
    // selection for non-converted photons
    //
    /** @brief range of eta bins for photon-ID*/
    std::vector<float> CutBinEta_photonsNonConverted;
    /** @brief range of ET bins for photon-ID*/
    std::vector<float> CutBinEnergy_photonsNonConverted;
    /** @brief Cut in E277 for photons*/
    std::vector<float> e277_photonsNonConverted;
    /** @brief Cut on hadronic leakage for photons*/
    std::vector<float> CutHadLeakage_photonsNonConverted;
    /** @brief ratio E237/E277*/
    std::vector<float> Reta37_photonsNonConverted;
    /** @brief ratio E233/E237*/
    std::vector<float> Rphi33_photonsNonConverted;
    /** @brief Cut on width in 2nd sampling for photons*/
    std::vector<float> weta2_photonsNonConverted;
    
    /** @brief binning in eta in strips for photons*/
    std::vector<float> CutBinEtaStrips_photonsNonConverted;
    /** @brief */
    std::vector<float> CutBinEnergyStrips_photonsNonConverted;
    /** @brief Cut on fraction of energy rec. in 1st sampling for photons*/
    std::vector<float> f1_photonsNonConverted;
    /** @brief Cut on Demax2 for photons*/
    // std::vector<float> emax2r_photonsNonConverted;
    /** @brief Cut on Emax2-Emin for photons*/
    std::vector<float> deltae_photonsNonConverted;
    /** @brief cut on (Emax1-Emax2)/(Emax1-Emax2) for photons*/
    std::vector<float> DEmaxs1_photonsNonConverted;
    /** @brief Cut on total width in strips for photons*/
    std::vector<float> wtot_photonsNonConverted;
    /** @brief Cut on fraction of energy outside core for photons*/
    std::vector<float> fracm_photonsNonConverted;
    /** @brief Cut on width in 3 strips for photons*/
    std::vector<float> w1_photonsNonConverted;
    /** @brief cut values for cut on f3 or f3core*/
    std::vector<float> CutF3_photonsNonConverted;

    //
    // selection for converted photons
    //
    /** @brief range of eta bins for photon-ID*/
    std::vector<float> CutBinEta_photonsConverted;
    /** @brief range of ET bins for photon-ID*/
    std::vector<float> CutBinEnergy_photonsConverted;
    /** @brief Cut in E277 for photons*/
    std::vector<float> e277_photonsConverted;
    /** @brief Cut on hadronic leakage for photons*/
    std::vector<float> CutHadLeakage_photonsConverted;
    /** @brief ratio E237/E277*/
    std::vector<float> Reta37_photonsConverted;
    /** @brief ratio E233/E237*/
    std::vector<float> Rphi33_photonsConverted;
    /** @brief Cut on width in 2nd sampling for photons*/
    std::vector<float> weta2_photonsConverted;
    
    /** @brief binning in eta in strips for photons*/
    std::vector<float> CutBinEtaStrips_photonsConverted;
    /** @brief */
    std::vector<float> CutBinEnergyStrips_photonsConverted;
    /** @brief Cut on fraction of energy rec. in 1st sampling for photons*/
    std::vector<float> f1_photonsConverted;
    /** @brief Cut on Demax2 for photons*/
    // std::vector<float> emax2r_photonsConverted;
    /** @brief Cut on Emax2-Emin for photons*/
    std::vector<float> deltae_photonsConverted;
    /** @brief cut on (Emax1-Emax2)/(Emax1-Emax2) for photons*/
    std::vector<float> DEmaxs1_photonsConverted;
    /** @brief Cut on total width in strips for photons*/
    std::vector<float> wtot_photonsConverted;
    /** @brief Cut on fraction of energy outside core for photons*/
    std::vector<float> fracm_photonsConverted;
    /** @brief Cut on width in 3 strips for photons*/
    std::vector<float> w1_photonsConverted;
    /** @brief cut min on E/p for e-ID*/
    std::vector<float> CutminEp_photonsConverted;
    /** @brief cut max on E/p for e-ID*/
    std::vector<float> CutmaxEp_photonsConverted;
    /** @brief cut values for cut on f3 or f3core*/
    std::vector<float> CutF3_photonsConverted;



    // Private members
  private:

    // would ideally be protected: only to be used by ARASelector
    void setIsEM(unsigned int isEM) { m_isEM = isEM; };
    const Root::TAccept& fillAccept();

    bool CheckVar(std::vector<float> vec, int choice) const;
    bool CheckVar(std::vector<int> vec, int choice) const;

    unsigned int m_isEM;

    // the cut positions

    /** @brief cluster eta range */
    int m_cutPositionClusterEtaRange_Photon;
    /** @brief energy fraction in the third layer */
    int m_cutPositionClusterBackEnergyFraction_Photon;
    /** @brief cluster leakage into the hadronic calorimeter */
    int m_cutPositionClusterHadronicLeakage_Photon;
    /** @brief energy in 2nd sampling (e277) */
    int m_cutPositionClusterMiddleEnergy_Photon;
    /** @brief energy ratio in 2nd sampling */
    int m_cutPositionClusterMiddleEratio37_Photon;
    /** @brief energy ratio in 2nd sampling for photons */
    int m_cutPositionClusterMiddleEratio33_Photon;
    /** @brief width in the second sampling */
    int m_cutPositionClusterMiddleWidth_Photon;
    /** @brief fraction of energy found in 1st sampling */
    int m_cutPositionClusterStripsEratio_Photon;
    /** @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et) */
    // int m_cutPositionClusterStripsDeltaEmax2_Photon;
    /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1) */
    int m_cutPositionClusterStripsDeltaE_Photon;
    /** @brief shower width in 1st sampling */
    int m_cutPositionClusterStripsWtot_Photon;
    /** @brief shower shape in shower core 1st sampling */
    int m_cutPositionClusterStripsFracm_Photon;
    /** @brief shower width weighted by distance from the maximum one */
    int m_cutPositionClusterStripsWeta1c_Photon;
    /** @brief difference between max and 2nd max in strips */
    int m_cutPositionClusterStripsDEmaxs1_Photon;
    /** @brief energy-momentum match for photon selection*/
    int m_cutPositionTrackMatchEoverP_Photon;
    /** @brief ambiguity resolution for photon (vs electron) */
    int m_cutPositionAmbiguityResolution_Photon;
    /** @brief isolation */
    int m_cutPositionIsolation_Photon;
    /** @brief calorimetric isolation for photon selection */
    int m_cutPositionClusterIsolation_Photon;
    /** @brief tracker isolation for photon selection */
    int m_cutPositionTrackIsolation_Photon;

    // the cut names

    /** @brief cluster eta range */
    const TString m_cutNameClusterEtaRange_Photon;
    /** @brief energy fraction in the third layer */
    const TString m_cutNameClusterBackEnergyFraction_Photon;
    /** @brief cluster leakage into the hadronic calorimeter */
    const TString m_cutNameClusterHadronicLeakage_Photon;
    /** @brief energy in 2nd sampling (e277) */
    const TString m_cutNameClusterMiddleEnergy_Photon;
    /** @brief energy ratio in 2nd sampling */
    const TString m_cutNameClusterMiddleEratio37_Photon;
    /** @brief energy ratio in 2nd sampling for photons */
    const TString m_cutNameClusterMiddleEratio33_Photon;
    /** @brief width in the second sampling */
    const TString m_cutNameClusterMiddleWidth_Photon;
    /** @brief fraction of energy found in 1st sampling */
    const TString m_cutNameClusterStripsEratio_Photon;
    /** @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et) */
    // const TString m_cutNameClusterStripsDeltaEmax2_Photon;
    /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1) */
    const TString m_cutNameClusterStripsDeltaE_Photon;
    /** @brief shower width in 1st sampling */
    const TString m_cutNameClusterStripsWtot_Photon;
    /** @brief shower shape in shower core 1st sampling */
    const TString m_cutNameClusterStripsFracm_Photon;
    /** @brief shower width weighted by distance from the maximum one */
    const TString m_cutNameClusterStripsWeta1c_Photon;
    /** @brief difference between max and 2nd max in strips */
    const TString m_cutNameClusterStripsDEmaxs1_Photon;
    /** @brief energy-momentum match for photon selection*/
    const TString m_cutNameTrackMatchEoverP_Photon;
    /** @brief ambiguity resolution for photon (vs electron) */
    const TString m_cutNameAmbiguityResolution_Photon;
    /** @brief isolation */
    const TString m_cutNameIsolation_Photon;
    /** @brief calorimetric isolation for photon selection */
    const TString m_cutNameClusterIsolation_Photon;
    /** @brief tracker isolation for photon selection */
    const TString m_cutNameTrackIsolation_Photon;

  }; // End: class definition

} // End: namespace Root

#endif 

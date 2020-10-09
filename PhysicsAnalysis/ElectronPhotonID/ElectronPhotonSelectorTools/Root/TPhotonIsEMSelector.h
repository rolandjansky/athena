/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Dear emacs, this is -*-c++-*-

#ifndef __TPHOTONISEMSELECTOR__
#define __TPHOTONISEMSELECTOR__


/**  @class TPhotonIsEMSelector.
 *
 *   @date  Feb 2012 - Aug 2018
 *   Original by  Jovan Mitrevski (Feb. 2012)
 *   Modified by Roger Naranjo
 * 
 *   Package: PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
 * 
 *   @brief This class in intented to apply a cut based identification criteria to photons
 * 
*/


// Include the return object and the base class
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"
#include "AsgMessaging/AsgMessaging.h"
#include "AsgMessaging/StatusCode.h"

#include <vector>
#include <string>


class AsgPhotonIsEMSelector;

namespace Root {
  class TPhotonIsEMSelector : public asg::AsgMessaging {

    friend class ::AsgPhotonIsEMSelector;


  public:
    /** Standard constructor */
    TPhotonIsEMSelector(const char *name = "TPhotonIsEMSelector");

    /** Standard destructor */
    ~TPhotonIsEMSelector();

    // Main methods
    /** Initialize this class */
    StatusCode initialize();

    /** The main accept method: the actual cuts are applied here */
    asg::AcceptData accept(
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
      bool isConversion);

    /** Return dummy accept with only info */
    asg::AcceptData accept() const { return asg::AcceptData(&m_acceptInfo); }

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
      bool isConversion) const;

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

    ///////////////////////////////////
    // Public members (the cut values)
    ///////////////////////////////

    /** @brief which subset of cuts to apply */
    unsigned int m_isEMMask;


    /** @brief boolean to force to test converted photon hypothesis */
    bool m_forceConvertedPhotonPID;
    /** @brief boolean to force to test non converted photon hypothesis */
    bool m_forceNonConvertedPhotonPID;


    //
    // selection for non-converted photons
    //
    /** @brief range of eta bins for photon-ID*/
    std::vector<float> m_cutBinEta_photonsNonConverted;
    /** @brief range of ET bins for photon-ID*/
    std::vector<float> m_cutBinEnergy_photonsNonConverted;
    /** @brief Cut in E277 for photons*/
    std::vector<float> m_e277_photonsNonConverted;
    /** @brief Cut on hadronic leakage for photons*/
    std::vector<float> m_cutHadLeakage_photonsNonConverted;
    /** @brief ratio E237/E277*/
    std::vector<float> m_Reta37_photonsNonConverted;
    /** @brief ratio E233/E237*/
    std::vector<float> m_Rphi33_photonsNonConverted;
    /** @brief Cut on width in 2nd sampling for photons*/
    std::vector<float> m_weta2_photonsNonConverted;

    /** @brief binning in eta in strips for photons*/
    std::vector<float> m_cutBinEtaStrips_photonsNonConverted;
    /** @brief */
    std::vector<float> m_cutBinEnergyStrips_photonsNonConverted;
    /** @brief Cut on fraction of energy rec. in 1st sampling for photons*/
    std::vector<float> m_f1_photonsNonConverted;
    /** @brief Cut on Demax2 for photons*/
    // std::vector<float> emax2r_photonsNonConverted;
    /** @brief Cut on Emax2-Emin for photons*/
    std::vector<float> m_deltae_photonsNonConverted;
    /** @brief cut on (Emax1-Emax2)/(Emax1-Emax2) for photons*/
    std::vector<float> m_DEmaxs1_photonsNonConverted;
    /** @brief Cut on total width in strips for photons*/
    std::vector<float> m_wtot_photonsNonConverted;
    /** @brief Cut on fraction of energy outside core for photons*/
    std::vector<float> m_fracm_photonsNonConverted;
    /** @brief Cut on width in 3 strips for photons*/
    std::vector<float> m_w1_photonsNonConverted;
    /** @brief cut values for cut on f3 or f3core*/
    std::vector<float> m_cutF3_photonsNonConverted;

    //
    // selection for converted photons
    //
    /** @brief range of eta bins for photon-ID*/
    std::vector<float> m_cutBinEta_photonsConverted;
    /** @brief range of ET bins for photon-ID*/
    std::vector<float> m_cutBinEnergy_photonsConverted;
    /** @brief Cut in E277 for photons*/
    std::vector<float> m_e277_photonsConverted;
    /** @brief Cut on hadronic leakage for photons*/
    std::vector<float> m_cutHadLeakage_photonsConverted;
    /** @brief ratio E237/E277*/
    std::vector<float> m_Reta37_photonsConverted;
    /** @brief ratio E233/E237*/
    std::vector<float> m_Rphi33_photonsConverted;
    /** @brief Cut on width in 2nd sampling for photons*/
    std::vector<float> m_weta2_photonsConverted;

    /** @brief binning in eta in strips for photons*/
    std::vector<float> m_cutBinEtaStrips_photonsConverted;
    /** @brief */
    std::vector<float> m_cutBinEnergyStrips_photonsConverted;
    /** @brief Cut on fraction of energy rec. in 1st sampling for photons*/
    std::vector<float> m_f1_photonsConverted;
    /** @brief Cut on Demax2 for photons*/
    // std::vector<float> emax2r_photonsConverted;
    /** @brief Cut on Emax2-Emin for photons*/
    std::vector<float> m_deltae_photonsConverted;
    /** @brief cut on (Emax1-Emax2)/(Emax1-Emax2) for photons*/
    std::vector<float> m_DEmaxs1_photonsConverted;
    /** @brief Cut on total width in strips for photons*/
    std::vector<float> m_wtot_photonsConverted;
    /** @brief Cut on fraction of energy outside core for photons*/
    std::vector<float> m_fracm_photonsConverted;
    /** @brief Cut on width in 3 strips for photons*/
    std::vector<float> m_w1_photonsConverted;
    /** @brief cut min on E/p for e-ID*/
    std::vector<float> m_cutminEp_photonsConverted;
    /** @brief cut max on E/p for e-ID*/
    std::vector<float> m_cutmaxEp_photonsConverted;
    /** @brief cut values for cut on f3 or f3core*/
    std::vector<float> m_cutF3_photonsConverted;

    /// accesss to the accept info object
    const asg::AcceptInfo &getAcceptInfo() const { return m_acceptInfo; }

    // Private members
  private:

    // would ideally be protected: only to be used by ARASelector
    asg::AcceptData fillAccept(unsigned int isEM);

    template<typename T>
    bool checkVar(const std::vector <T> &vec, int choice) const;

    /// Accept info
    asg::AcceptInfo m_acceptInfo;

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
    const std::string m_cutNameClusterEtaRange_Photon;
    /** @brief energy fraction in the third layer */
    const std::string m_cutNameClusterBackEnergyFraction_Photon;
    /** @brief cluster leakage into the hadronic calorimeter */
    const std::string m_cutNameClusterHadronicLeakage_Photon;
    /** @brief energy in 2nd sampling (e277) */
    const std::string m_cutNameClusterMiddleEnergy_Photon;
    /** @brief energy ratio in 2nd sampling */
    const std::string m_cutNameClusterMiddleEratio37_Photon;
    /** @brief energy ratio in 2nd sampling for photons */
    const std::string m_cutNameClusterMiddleEratio33_Photon;
    /** @brief width in the second sampling */
    const std::string m_cutNameClusterMiddleWidth_Photon;
    /** @brief fraction of energy found in 1st sampling */
    const std::string m_cutNameClusterStripsEratio_Photon;
    /** @brief energy of 2nd maximum in 1st sampling ~e2tsts1/(1000+const_lumi*et) */
    // const std::string m_cutNameClusterStripsDeltaEmax2_Photon;
    /** @brief difference between 2nd maximum and 1st minimum in strips (e2tsts1-emins1) */
    const std::string m_cutNameClusterStripsDeltaE_Photon;
    /** @brief shower width in 1st sampling */
    const std::string m_cutNameClusterStripsWtot_Photon;
    /** @brief shower shape in shower core 1st sampling */
    const std::string m_cutNameClusterStripsFracm_Photon;
    /** @brief shower width weighted by distance from the maximum one */
    const std::string m_cutNameClusterStripsWeta1c_Photon;
    /** @brief difference between max and 2nd max in strips */
    const std::string m_cutNameClusterStripsDEmaxs1_Photon;
    /** @brief energy-momentum match for photon selection*/
    const std::string m_cutNameTrackMatchEoverP_Photon;
    /** @brief ambiguity resolution for photon (vs electron) */
    const std::string m_cutNameAmbiguityResolution_Photon;
    /** @brief isolation */
    const std::string m_cutNameIsolation_Photon;
    /** @brief calorimetric isolation for photon selection */
    const std::string m_cutNameClusterIsolation_Photon;
    /** @brief tracker isolation for photon selection */
    const std::string m_cutNameTrackIsolation_Photon;

  }; // End: class definition

} // End: namespace Root

#endif 

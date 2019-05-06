/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMCANDIDATEOFFLINEDATA_H
#define MMCANDIDATEOFFLINEDATA_H

// local includes
#include "TrigT1NSWSimTools/MMCandidateData.h" 

// forward declarations
class TVector3;


// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *   @short class modeling the strip hit fragment for the NSW L1 offline simulation
   *
   * This holds the trigger candidate DeltaTheta measurement and the associated position
   * within the NSW detector. To be delivered to the Trigger Sector Logic for the candidate
   * confirmation.
   *
   *  @author Alessandro Di Mattia <dimattia@cern.ch>, Ourania Sidiropoulou <osidirop@cern.ch>
   *
   *
   */

  class MMCandidateOfflineData: public MMCandidateData {

  private:

    int   m_trig_bcid;            //!< trig BCID
    float m_delta_theta;          //!< Delta Theta measurement: defines a prompt track if found within the trigger threshold
    float m_eta_position;         //!< Eta position of the prompt track candidate
    float m_phi_position;         //!< Sigma of the Eta position of the prompt track candidate
    float m_eta_sigma_position;   //!< Phi position of the prompt track candidate
    float m_phi_sigma_position;   //!< Sigma of the Phi position of the prompt track candidate


  public:
    MMCandidateOfflineData(int trig_bcid, float delta_theta, float eta_pos, float phi_pos, float eta_sigma_pos = 0.0, float phi_sigma_pos = 0.0);
    ~MMCandidateOfflineData();

    //! methods for retrieving the bare data
    int trig_BCID()    const;  //!< get the BCID

    //! helper  methods decoding the trigger candiate fragment delivered by the FPGA trigger logic for the NSW detector
    float deltaTheta()  const; //!< Delta Theta measurement: defines a prompt track if found within the trigger threshold
    float eta()         const; //!< Eta position of the prompt track candidate
    float etaSigma()    const; //!< Sigma of the Eta position of the prompt track candidate
    float phi()         const; //!< Phi position of the prompt track candidate
    float phiSigma()    const; //!< Sigma of the Phi position of the prompt track candidate

  };  // end of MMCandidateOfflineData class

} // namespace NSWL1

#endif

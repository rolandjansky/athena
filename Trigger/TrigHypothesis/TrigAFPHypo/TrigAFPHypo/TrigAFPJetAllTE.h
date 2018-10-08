/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGAFPHYPO_TRIGAFPJETALLTE_H
#define TRIGAFPHYPO_TRIGAFPJETALLTE_H

#include <string>

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigAFPHypo/AFPProtonTransportTool.h"

/**
 * @brief Class for the AFP hypothesis algorithm that targets exclusive jet events
 *
 * This algorithm targets exclusive jet events, where two protons emit jets and 
 * remain intact. The position of the protons at AFP is extrapolated using the 
 * jets kinematics, which is expected to be highly correlated to proton kinematics,
 * due to energy conservation. The predicted proton position is then compared to 
 * measurements made in the AFP detector using AFP tracks reconstructed by the 
 * FEX algorithm (Trig_AFPSiTrkReco in the TrigAFPReco package), comparing the x, y 
 * position as well as the radius of the distance.
 */

class TrigAFPJetAllTE: public HLT::AllTEAlgo {
public:
  /**
   * @brief Sets all variables and objects set by the Python class to their default.
   *
   * Sets the variable corresponding to the decision to record the event to false.
   * Sets the cache variable to avoid repeating already existing calculations to false.
   * Creats null pointers corresponding to the parameterizations of the A and C side beams.
   * Sets all variables that can be defined in the Python class to their default. These are:
   * - Thresholds for x, y and radius cuts
   * - Nominal beam energy, used to calculate the proton position prediction
   * - Files with the proton transport parameterization
   * - Position shifts between parameterization and measurements
   * - Corrections in x position due to alignment for each ATLAS side
   *
   * Sets all monitored variables, used to check if the algorithm is doing things as expected.
   * The monitored variables are:
   * - Jet multiplicity
   * - Dijet system eta, rapidity and mass
   * - Position distance between measurement and prediction for A and C side
   * - Track multiplicity in each ATLAS side
   * - Difference in x and y values between measurement and prediction for A and C side
   * - Tracks x and y values for A and C side for prediction and measurement
   */

  TrigAFPJetAllTE(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigAFPJetAllTE();

  /**
   * @brief This method calculates proton position at AFP based on jets kinematics and compares to AFP detector measurements
   *
   * In this method a container with jets is retrieved from the trigger element. 
   * Then the necessary variables for dijet kinematics calculations, as well as for cuts are set. 
   * Afterwards the dijet system TLorentzVector is defined using the two most energetic jets. Using a proton transport parameterization this 
   * information is used to predict the protons position at AFP in each ATLAS side.
   * The next step is to retrieve a container of AFP tracks reconstructed in the FEX algorithm (Trig_AFPSiTrkReco in TrigAFPReco package).
   * Using the tracks container the differences in position, x and y values are calculated, taking into account beam alignment correction (set up
   * from the Python class). For each side the differences are compared with the thresholds (set from the Python class) and a boolean is defined 
   * corresponding to the cuts being passed or not, and the node corresponding to this decision is set to this value.
   * The steps corresponding to the calculations and comparisions are only made if a cache variable stats that these were not already made.
   */

  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE,
                            unsigned int outputTE);

  ///Method to run at the end of each event that sets the cache to calculate to false and the default decision to record the next event to false
  HLT::ErrorCode hltEndEvent();

  /**
   * @brief Initialize method to fetch the parameterization txt files, set the parameterization objects and the energy variable.
   *
   * In the initialize method files with the parameterization for each ATLAS side are retrieved, using PathResolver::find_file.
   * With these files objects of the proton transport parameterization are created, for each A and C side.
   * A Info print statement is made with the radius, x and y thresholds and the names of the parameterization files.
   * The last step is to set a total energy variable, which is double of each beam energy.
   */

  HLT::ErrorCode hltInitialize();

  ///Finalize method where the pointers of the transport beams are deleted
  HLT::ErrorCode hltFinalize();

private:
  ///Boolean corresponding to the decision to record the event based on the selection being met.
  bool m_decision;
  ///Bolean that is set to true after the calculations and comparisions are made, to avoid redoing these.
  bool m_useCachedResult;

  ///Proton transport parameterizaton object used in the proton position prediction for ATLAS A side
  AFPProtonTransportTool* m_transportBeam1;
  ///Proton transport parameterizaton object used in the proton position prediction for ATLAS C side
  AFPProtonTransportTool* m_transportBeam2;

  ///String with the name of the txt file containing the parameterization corresponding to the ATLAS A side
  std::string m_protonTransportParamFileName1;
  ///String with the name of the txt file containing the parameterization corresponding to the ATLAS C side
  std::string m_protonTransportParamFileName2;

  ///Threshold for the radius distance between parameterization and measurements 
  float m_maxProtonDist;
  ///Threshold for the x distance between parameterization and measurements 
  float m_maxProtonDiff_x;
  ///Threshold for the y distance between parameterization and measurements 
  float m_maxProtonDiff_y;

  float m_beamEnergy;       ///< energy of one beam i.e. half of centre-of-mass energy
  float m_totalEnergy;      ///< beams centre-of-mass energy 2*#m_beamEnergy

  ///Shift in x position between parameterization and measurements
  float m_protonPosShift_x;
  ///Shif in y position between parameterization and measurements
  float m_protonPosShift_y;

  ///Beam alignment corrections in the x position of ATLAS A side
  float m_alignmentCorrection_nearA;
  ///Beam alignment corrections in the x position of ATLAS C side
  float m_alignmentCorrection_nearC;

  ///Variable to convert from MeV to GeV
  const float m_GeV = 0.001;

  //Monitored variables
  ///Jet multiplicity monitoring variable
  int m_jetsN;
  ///Dijet system mass monitoring variable
  double m_dijetMass;
  ///Dijet system pseudo-rapidity monitoring variable
  double m_dijetEta;
  ///Dijet system rapidity monitoring variable
  double m_dijetRapidity;

  ///A side track multiplicity monitoring variable
  int m_sideA_tracksN;
  ///Minimum distance between tracks measuremtnts and predictions in ATLAS A side
  double m_sideA_minDist;
  ///Closest x position of tracks measurements in ATLAS A side compared to prediction
  double m_sideA_trkX;
  ///Closest y position of tracks measurements in ATLAS A side compared to prediction
  double m_sideA_trkY;
  ///Closest x position of prediction in ATLAS A side compared to measurements
  double m_sideA_predictX;
  ///Closest y position of prediction in ATLAS A side compared to measurements
  double m_sideA_predictY;
  ///Minimum distnace in x between measurements and prediction in ATLAS A side
  double m_sideA_diffX;
  ///Minimum distnace in y between measurements and prediction in ATLAS A side
  double m_sideA_diffY;

  ///C side track multiplicity monitoring variable
  int m_sideC_tracksN;
  ///Minimum distance between tracks measuremtnts and predictions in ATLAS C side
  double m_sideC_minDist;
  ///Closest x position of tracks measurements in ATLAS C side
  double m_sideC_trkX;
  ///Closest y position of tracks measurements in ATLAS C side
  double m_sideC_trkY;
  ///Closest x position of prediction in ATLAS C side compared to measurements
  double m_sideC_predictX;
  ///Closest y position of prediction in ATLAS C side compared to measurements
  double m_sideC_predictY;
  ///Minimum distnace in x between measurements and prediction in ATLAS C side
  double m_sideC_diffX;
  ///Minimum distnace in y between measurements and prediction in ATLAS C side
  double m_sideC_diffY;

  ///Bolean to change the decision to accept single or double AFP proton tags
   bool m_requireDoubleTag;
};

#endif

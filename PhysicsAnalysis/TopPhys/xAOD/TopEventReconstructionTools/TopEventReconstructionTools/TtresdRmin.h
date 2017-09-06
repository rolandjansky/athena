/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TtresdRmin_H
#define TtresdRmin_H

#include <iostream>
#include <vector>

#include "TLorentzVector.h"

using namespace std;

class TtresdRmin {


/////////////////////////////////////////////////////////////////////////////////////////////
//  public
/////////////////////////////////////////////////////////////////////////////////////////////

 public:

  /**
   * Class constructor. Set unit used in four momentum vector calculations
   * @param units : Unit used for reconstruction, MeV or GeV
   */
  TtresdRmin(TString units = "GeV", bool verbose = false);


  /**
   * Class destructor
   */
  virtual ~TtresdRmin() {};


  /**
   * Initialize the tool, giving specific parameters needed for dRmin cut
   * Paramters are given in GeV scale, will be scaled internally to MeV if set.
   *
   * @param highJetMass : jet mass for highMass jet criteria
   * @param minJetPt : minimum pT of jets used for dRmin method
   * @param dRCutOffset : Offset for dRmin specific cut
   * @param dRCutMassFactor : Factor applied to jet mass in dRmin specific cut
   * @return void
   */
  void Init(double highJetMass = 60.0, double minJetPt = 25.0, double dRCutOffset = 2.5, double dRCutMassFactor = 0.015);


  /**
   * Set the objects needed to calculate the invariant ttbar mass
   *
   * @param jets     : Jet container
   * @param lepton   : Selected lepton
   * @param neutrino : Reconstructed neutrino
   * @return bool    : Success of setup
   */
  bool SetupEvent(vector<TLorentzVector*> &jets, TLorentzVector &lepton, TLorentzVector &neutrino);


  /**
   * Get jet indicices identified by dRmin method, used for mtt reconstruction
   * @return vector<int> : Indices of identified jets in passed jet vector
   */
  inline vector<int> GetJetIndices() { return m_jet_indices; };


  /**
   * Get index of jet with highest mass
   * @return int : jet index
   */
  inline int GetHighMassJetIndex() { return m_highMassJetIndex; };


  /**
   * Get highest jet mass found in jet vector
   * @return double : jet mass
   */
  inline double GetHighestJetMass() { return m_highestJetMass; };


  /**
   * Set threshold for highMass case
   * @param highJetMass threshold for the high jet mass case
   */
  inline void SetHighMThreshold(double highJetMass) { m_highM = highJetMass; };


  /**
   * Get calculated invariant ttbar mass for dRmin method
   * @return double : invariant ttbar mass
   */
  double GetMttdRmin();

  /**
   * Get the TLorentzVector for the ttbar system
   */
  TLorentzVector GetTlvdRmin();


/////////////////////////////////////////////////////////////////////////////////////////////
//  private
/////////////////////////////////////////////////////////////////////////////////////////////

 private:
  /**
   * Find the jet indices used for mtt reconstruction for events containing a high mass jet
   * @return bool : success of function
   */
  bool FindJetIndices_HM();


  /**
   * Find the jet indices used for mtt reconstruction for events NOT containing a high mass jet
   * @return bool : success of function
   */
  bool FindJetIndices_LM();

  bool    m_init;                      ///< Flag of initialization status of tool
  bool    m_verbose;                   ///< Flag for verbosity level
  double  m_highM;                     ///< Jet mass threshold to identify it as a "high mass jet"
  double  m_Units;                     ///< Unit factor used, GeV = 1, MeV = 1000
  double  m_minJetPt;                  ///< Minimum jet pT used for highM jet search
  double  m_dRCutOffset;               ///< Offset of dRmin specific cut on jet_dR
  double  m_dRCutMassFactor;           ///< Jet mass factor of dRmin specific cut on jet_dR

  int     m_highMassJetIndex;          ///< Index of highest mass jet for current event
  double  m_highestJetMass;            ///< Highest jet mass found for current event

  vector<TLorentzVector*> m_jets;      ///< Input vector of jet objects
  TLorentzVector m_lepton;             ///< Input lepton object
  TLorentzVector m_neutrino;           ///< Input neutrino object
  TLorentzVector m_Wleptonic;          ///< leptonic W object reconstructed from lepton and neutrino

  vector<int> m_jet_indices;           ///< Indices of jets used for reconstruction of invariant ttbar mass

};

#endif

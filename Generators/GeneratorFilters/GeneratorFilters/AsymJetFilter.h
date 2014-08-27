/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_ASYMJETFILTER_H
#define GENERATORFILTERS_ASYMJETFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "CLHEP/Vector/LorentzVector.h"

/// @class AsymJetFilter
/// @brief Asymetric jet filter: selects two jets with two different pT thresholds
/// @author S. Laplace Nov. 2006
class AsymJetFilter : public GenFilter {
public:

  AsymJetFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  /// @name Fixed properties
  //@{
  static const int m_grphi = 105 ; // -CLHEP::pi to CLHEP::pi in units of 0.06 (approx)
  static const int m_greta = 200 ; // -6.0 to 6.0 in units 0.06 (approx)
  //@}

  /// Settable properties
  //@{
  double m_UserEta;  //range over which triggers are searched for
  int m_UserNumber; // how many are we looking for
  double m_UserThresh1; // lowest et jet
  double m_UserThresh2; // lowest et jet
  double m_dRThresh; // isolation cut
  double m_Stop; //seed tower threshold
  double m_Cone; //cone sixe
  int m_Gride; //how many cells in eta
  int m_Gridp; //how many cells in phi
  bool m_Type; // cone or grid to define jet
  //@}

  /// @name Internal parameters
  //@{
  double m_EtaRange; //range over which search runs
  double m_emaxeta; // largest eta of bins
  double m_edeta; // size of eta bins
  double m_edphi; //size of phi bins
  double m_twopi; //twopi
  double m_phi_min, m_phi_max;
  int m_nphicell; // number of phi cells inside half cone
  int m_netacell; // number of eta cells inside half cone
  int m_nphicell2; // number of phi cells inside full cone
  int m_netacell2; // number of eta cells inside full cone
  std::vector<CLHEP::HepLorentzVector> m_Jets; //store jets
  //@}

};

#endif

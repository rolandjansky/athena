/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERSJETWITHTRUTHPHOTONFILTER_H
#define GENERATORFILTERSJETWITHTRUTHPHOTONFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "JetFilter.h"
#include <math.h>

/**
@class JetFilterWithTruthPhoton 
@brief Applies the generator level filter for jet candidates with truth photons
@author J Tanaka: Aug 2012
*/

// based on JetFilter.h

class JetFilterWithTruthPhoton : public GenFilter {
public:
  JetFilterWithTruthPhoton(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~JetFilterWithTruthPhoton();
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

private:
  // Fixed Properties:-
  static const int m_grphi=105 ; // -pi to pi in units of 0.06 (approx)
  static const int m_greta=200 ; // -6.0 to 6.0 in units 0.06 {approx}
  // Setable Properties:-
  double m_UserEta;  //range over which triggers are searched for
  int m_UserNumber; // how many are we looking for
  double  m_UserThresh; // lowest et jet
  double m_Stop; //seed tower threshold
  double m_Cone; //cone sixe
  int m_Gride; //how many cells in eta
  int m_Gridp; //how many cells in phi
  bool m_Type; // cone or grid to define jet
  // internal parameters
  double m_EtaRange; //range over which search runs
  double m_emaxeta; // largest eta of bins
  double m_edeta; // size of eta bins
  double m_edphi; //size of phi bins
  double m_twopi; //twopi
  int m_nphicell; // number of phi cells inside half cone
  int m_netacell; // number of eta cells inside half cone
  int m_nphicell2; // number of phi cells inside full cone
  int m_netacell2; // number of eta cells inside full cone
  std::vector<JetFilter::McObj> m_Jets; //store jets

  double  m_UserLeadingThresh; // highest et jet

  double m_photonEta;
  double m_photonPt;
  double m_photonSumPt;
  double m_dRphotonjet;

  double m_massDijet;

  // Private Methods:=
};
#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//===========================
// Author: F Spano'
// Time of writing: Dec 2006 
// Modifications:
//===========================

#ifndef ROIMON_H
#define ROIMON_H

#include "CommissionEvent/ComTime.h" 

#include <string>
#include "TH1D.h"
#include "TH2D.h"


class ROIMon{
public:
  ROIMon(ComTime* comtime, float distance);
  ~ROIMon();
  StatusCode initialize();
  int Accept(CLHEP::Hep3Vector Point);
  int AcceptTime();
  float GetDistance(CLHEP::Hep3Vector Point);
  float GetTime();
  
 private:
  std::string m_cellContainerName;
  Double_t m_MinDist;
  Double_t m_tileMuDeltaEta ;
  Double_t m_tileMuDeltaPhi ;
  float m_maxDistance;
  float m_theDistance;
  float m_theTime;
  ComTime* m_myComTime;  
};
#endif // ROIMon_H: F Spano'


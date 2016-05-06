/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWDATABASE_H
#define EFLOWDATABASE_H

/********************************************************************

NAME:     eflowDatabase.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  23rd August, 2005

********************************************************************/

//C++ Headers
#include <vector>

class eflowDatabase  {

 public:

  eflowDatabase();
  ~eflowDatabase() {};

  double getEtaUnit() {return m_etaUnit;}
  double getPhiUnit() {return m_phiUnit;}

  double getEmX0PerUnitLength() {return m_emX0PerUnitLength;}
  double getHadX0PerUnitLength() {return m_hadX0PerUnitLength;}

  std::vector<double> getCellEtaWidth() {return m_cellEtaWidth;}
  std::vector<double> getCellPhiWidth() {return m_cellPhiWidth;}

 private:
  
  double m_etaUnit;
  double m_phiUnit;

  double m_emX0PerUnitLength;
  double m_hadX0PerUnitLength; 

  std::vector<double>  m_cellEtaWidth;
  std::vector<double>  m_cellPhiWidth;
};

#endif

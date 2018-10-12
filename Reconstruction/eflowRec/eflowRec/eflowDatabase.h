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

/**
Stores calorimeter cell eta and phi widths, a X0 per unit length in the EM and HAD calorimeters.
*/
class eflowDatabase  {

 public:

  eflowDatabase();
  ~eflowDatabase() {};

  double getEtaUnit() {return m_etaUnit;}
  double getPhiUnit() {return m_phiUnit;}

  double getEmX0PerUnitLength() {return m_emX0PerUnitLength;}
  double getHadX0PerUnitLength() {return m_hadX0PerUnitLength;}
  double getFCalX0PerUnitLength(int layer) {
    if(layer==0) return m_fcal0;
    else if(layer==1) return m_fcal1;
    else if(layer==2) return m_fcal2;
    else return 0.0;
  }

  std::vector<double> getCellEtaWidth() {return m_cellEtaWidth;}
  std::vector<double> getCellPhiWidth() {return m_cellPhiWidth;}

 private:
  
  double m_etaUnit;
  double m_phiUnit;

  double m_emX0PerUnitLength;
  double m_hadX0PerUnitLength; 

  double m_fcal0;
  double m_fcal1;
  double m_fcal2;

  std::vector<double>  m_cellEtaWidth;
  std::vector<double>  m_cellPhiWidth;
};

#endif

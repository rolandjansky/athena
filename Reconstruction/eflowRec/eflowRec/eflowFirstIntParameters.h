/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWFIRSTINTPARAMETRS_H
#define EFLOWFIRSTINTPARAMETRS_H

/********************************************************************

NAME:     eflowFirstIntParameters.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowFirstInt.h"
#include "eflowRec/eflowCaloRegions.h"
//C++ Headers
#include <vector>
#include <istream>

class eflowFirstIntParameters :  public eflowFirstInt {

 public:

  eflowFirstIntParameters();
  ~eflowFirstIntParameters()  {}

  friend std::istream& operator>> (std::istream& in, eflowFirstIntParameters& rhs);

  const std::vector<double>& getShapeParameters(eflowCaloENUM layer) const  {return m_p[layer];}
  double getShapeParameter(eflowCaloENUM layer, int paramIndex) const  {return m_p[layer][paramIndex];}

  void setShapeParameters(eflowCaloENUM layer, const std::vector<double>& p)  {m_p[layer] = p;}
  void setShapeParameter(eflowCaloENUM layer, int paramNumber, double shapeParam)   {m_p[layer][paramNumber] = shapeParam;}

  //TEMPORARY accessor to m_p
  void printM_P();


 protected:

  std::vector< std::vector<double> > m_p;

};
#endif

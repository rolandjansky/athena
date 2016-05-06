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

  bool getWeightedParameters(const eflowFirstIntParameters* bin1, const eflowFirstIntParameters* bin2, const double w1);

  const std::vector<double>& getShapeParameters(eflowCaloENUM layer) const  {return m_parameters[layer];}
  double getShapeParameter(eflowCaloENUM layer, int paramIndex) const  {return m_parameters[layer][paramIndex];}

  void setShapeParameter(eflowCaloENUM layer, int paramNumber, double shapeParam)   {m_parameters[layer][paramNumber] = shapeParam;}

  //TEMPORARY accessor to m_parameters
  void printM_Parameters();
  static int nShapeParams() {
    return m_nShapeParams;
  }

 private:

  void setShapeParameters(eflowCaloENUM layer, const std::vector<double>& p)  {m_parameters[layer] = p;}

 private:
  static const int m_nShapeParams;
  std::vector< std::vector<double> > m_parameters;

};
#endif

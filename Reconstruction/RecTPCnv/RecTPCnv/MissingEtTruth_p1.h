/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETTRUTH_P1_H
#define RECTPCNV_MISSINGETTRUTH_P1_H

/********************************************************************

NAME:     MissingET_p1
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Persistent rapresentation of MissingEtTruth class
********************************************************************/
#include "RecTPCnv/MissingET_p1.h"
#include <vector>

// forward declarations
class MissingEtTruthCnv_p1;

class MissingEtTruth_p1
{
 friend class MissingEtTruthCnv_p1;
 
 public:

  /**  default constructor */
  MissingEtTruth_p1() {};

  /** destructor */
  ~MissingEtTruth_p1() = default;


 protected:
  
  MissingET_p1 m_met;
  
  /// basic components of MissingEtTruth_p1
  std::vector<double> m_exTruth;
  std::vector<double> m_eyTruth;
  std::vector<double> m_etSumTruth;
  
};

#endif

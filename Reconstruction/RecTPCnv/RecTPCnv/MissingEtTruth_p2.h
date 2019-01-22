/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETTRUTH_P2_H
#define RECTPCNV_MISSINGETTRUTH_P2_H

/********************************************************************

NAME:     MissingET_p2
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  I. Vukotic
CREATED:  May 2009

PURPOSE:  Persistent rapresentation of MissingEtTruth class
********************************************************************/
#include "RecTPCnv/MissingET_p2.h"
#include <vector>

// forward declarations
class MissingEtTruthCnv_p2;

class MissingEtTruth_p2
{
 friend class MissingEtTruthCnv_p2;
 
 public:

  /**  default constructor */
  MissingEtTruth_p2() {};

  /** destructor */
  ~MissingEtTruth_p2() = default;


 protected:
  
	std::vector<float> m_allTheData;
  
};

#endif

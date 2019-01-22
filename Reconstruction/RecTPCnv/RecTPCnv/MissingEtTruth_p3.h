/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETTRUTH_P3_H
#define RECTPCNV_MISSINGETTRUTH_P3_H

/********************************************************************
NAME:     MissingEtTruth_p3
PACKAGE:  offline/Reconstruction/RecTPCnv
PURPOSE:  Persistent rapresentation of MissingEtTruth class
********************************************************************/
#include <vector>

// forward declarations
class MissingEtTruthCnv_p3;

class MissingEtTruth_p3
{
 friend class MissingEtTruthCnv_p3;
 
 public:

  /**  default constructor */
  MissingEtTruth_p3() {};

  /** destructor */
  ~MissingEtTruth_p3() = default;


 protected:
  
	std::vector<float> m_allTheData;
  
};

#endif

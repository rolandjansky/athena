/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETREGIONS_P1_H
#define RECTPCNV_MISSINGETREGIONS_P1_H

/********************************************************************

NAME:     MissingET_p1
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Persistent rapresentation of MissingEtRegions class
********************************************************************/

#include <vector>

// forward declarations
class MissingEtRegionsCnv_p1;

class MissingEtRegions_p1
{
 friend class MissingEtRegionsCnv_p1;
 
 public:

  /**  default constructor */
  MissingEtRegions_p1() {};

  /** destructor */
  ~MissingEtRegions_p1() = default;


 protected:
  
  //unsigned int m_reg;
  //MissingEtRegions::RegionIndex m_reg;
    
  /// basic components of MissingEtRegions_p1
  std::vector<double> m_exReg;
  std::vector<double> m_eyReg;
  std::vector<double> m_etReg;
  

};

#endif

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETREGIONS_P2_H
#define RECTPCNV_MISSINGETREGIONS_P2_H

/********************************************************************

NAME:     MissingET_p2
PACKAGE:  offline/Reconstruction/RecTPCnv

PURPOSE:  Persistent rapresentation of MissingEtRegions class
********************************************************************/

#include <vector>

// forward declarations
class MissingEtRegionsCnv_p2;

class MissingEtRegions_p2
{
 friend class MissingEtRegionsCnv_p2;
 
 public:

  /**  default constructor */
  MissingEtRegions_p2() {};

  /** destructor */
  ~MissingEtRegions_p2() = default;


 protected:
	// not needed for now since MissingEtRegions is not saved standalone.
    // std::vector<float> m_AllTheData;
  

};

#endif

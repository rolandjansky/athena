/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETREGIONS_P3_H
#define RECTPCNV_MISSINGETREGIONS_P3_H

/********************************************************************

NAME:     MissingEtRegions_p3
PACKAGE:  offline/Reconstruction/RecTPCnv
PURPOSE:  Persistent rapresentation of MissingEtRegions class
********************************************************************/

#include <vector>

// forward declarations
class MissingEtRegionsCnv_p3;

class MissingEtRegions_p3
{
 friend class MissingEtRegionsCnv_p3;
 
 public:

  /**  default constructor */
  MissingEtRegions_p3() {};

  /** destructor */
  ~MissingEtRegions_p3() = default;


 protected:
	// not needed for now since MissingEtRegions is not saved standalone.
    // std::vector<float> m_AllTheData;
  

};

#endif

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCALO_P3_H
#define RECTPCNV_MISSINGETCALO_P3_H

/********************************************************************
NAME:     MissingETCalo_p3
PURPOSE:  Persistent rapresentation of MissingEtCalo class
********************************************************************/
#include <vector>

// forward declarations
class MissingEtCaloCnv_p3;


class MissingEtCalo_p3
{
 friend class MissingEtCaloCnv_p3;
 
 public:

  /**  default constructor */
  MissingEtCalo_p3() {};

  /** destructor */
  ~MissingEtCalo_p3() = default;


 protected:
    
	std::vector<float> m_allTheData;
  
};

#endif

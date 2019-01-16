// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGET_P1_H
#define RECTPCNV_MISSINGET_P1_H

/********************************************************************

NAME:     MissingET_p1
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Persistent rapresentation of MissingET class
********************************************************************/

#include "RecTPCnv/MissingEtRegions_p1.h"

// forward declarations
class MissingETCnv_p1;

class MissingET_p1
{
 friend class MissingETCnv_p1;
 
 public:

  /**  default constructor */
  MissingET_p1()
    : m_source(0),
      m_ex(0),
      m_ey(0),
      m_etSum(0)
  {};

  /** destructor */
  ~MissingET_p1() = default;


 protected:
  
  MissingEtRegions_p1 m_regions;
  
  unsigned int m_source;
  
  /// basic components of MissingET_p1
  float m_ex;
  float m_ey;
  float m_etSum;
  
};


#endif

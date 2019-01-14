/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCALO_P1_H
#define RECTPCNV_MISSINGETCALO_P1_H

/********************************************************************

NAME:     MissingET_p1
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Persistent rapresentation of MissingEtCalo class
********************************************************************/
#include "RecTPCnv/MissingET_p1.h"
#include <vector>

// forward declarations
class MissingEtCaloCnv_p1;


class MissingEtCalo_p1
{
 friend class MissingEtCaloCnv_p1;
 
 public:

  /**  default constructor */
  MissingEtCalo_p1() {};

  /** destructor */
  ~MissingEtCalo_p1() = default;


 protected:
    
  MissingET_p1 m_met;
  
  /// basic components of MissingEtCalo_p1
  std::vector<double> m_exCalo;
  std::vector<double> m_eyCalo;
  std::vector<double> m_etSumCalo;
  std::vector<unsigned int> m_nCellsCalo;
  
};

#endif

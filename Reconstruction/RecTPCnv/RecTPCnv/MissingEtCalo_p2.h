/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCALO_P2_H
#define RECTPCNV_MISSINGETCALO_P2_H

/********************************************************************

NAME:     MissingETCalo_p2

PURPOSE:  Persistent rapresentation of MissingEtCalo class
********************************************************************/
#include <vector>

// forward declarations
class MissingEtCaloCnv_p2;


class MissingEtCalo_p2
{
 friend class MissingEtCaloCnv_p2;
 
 public:

  /**  default constructor */
  MissingEtCalo_p2() {};

  /** destructor */
  ~MissingEtCalo_p2() = default;


 protected:
    
	std::vector<float> m_AllTheData;
  // MissingET_p1 m_met;
  // 
  // /// basic components of MissingEtCalo_p1
  // std::vector<double> m_exCalo;
  // std::vector<double> m_eyCalo;
  // std::vector<double> m_etSumCalo;
  // std::vector<unsigned int> m_nCellsCalo;
  
};

#endif

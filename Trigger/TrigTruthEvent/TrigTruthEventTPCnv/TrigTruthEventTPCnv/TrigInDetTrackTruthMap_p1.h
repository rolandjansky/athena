/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTruthEventTPCnv
 * @class  : TrigInDetTrackTruthMap_p1
 *
 * @brief  p1 persistent version of TrigInDetTrackTruthMap
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackTruthMap_p1.h,v 1.1 2008-04-04 14:46:41 ahamil Exp $
 **********************************************************************************/
#ifndef TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_P1_H
#define TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_P1_H

#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigInDetTrackTruthMap_p1 {
  friend class TrigInDetTrackTruthMapCnv_p1;
    
 public:
    
  // default constructor
  TrigInDetTrackTruthMap_p1() { }
  
  // default destructor
  virtual ~TrigInDetTrackTruthMap_p1() = default;

  //data members
  ElementLinkIntVector_p1  m_elink_vec;  //actual type ElementLinkVector< TrigInDetTrackCollection >
  std::vector<TPObjRef>  m_truth_vec;  //actual type std::vector< TrigInDetTrackTruth >

};

#endif

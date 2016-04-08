/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTruthEventTPCnv
 * @Class  : TrigInDetTrackTruth_p1
 *
 * @brief persistent partner for TrigInDetTrackTruth
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackTruth_p1.h,v 1.1 2008-04-04 14:46:42 ahamil Exp $
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGINDETTRACKTRUTH_P1_H
#define TRIGEVENTTPCNV_TRIGINDETTRACKTRUTH_P1_H

#include "TrigInDetTruthEvent/TrigInDetTrackTruth.h"

class TrigInDetTrackTruth_p1 
{
 public:
  int best_match_hits;
  int best_Si_match_hits;
  int best_TRT_match_hits;

  std::vector<TPObjRef> m_true_part_vec;  //actual type std::vector<HepMcParticleLink> 
  std::vector<TPObjRef> m_nr_common_hits; //actual type std::vector<TrigIDHitStats>
  std::vector< std::pair<unsigned int, unsigned int> >  m_family_tree; 

};

#endif

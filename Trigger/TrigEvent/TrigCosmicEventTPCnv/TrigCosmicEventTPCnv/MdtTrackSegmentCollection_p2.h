/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : MdtTrackSegmentCollection_p2
 *
 * @brief persistent partner for MdtTrackSegmentCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * V2 06.01.2011 giagu - Stefano.Giagu@cern.ch
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCOLLECTION_P2_H
#define TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCOLLECTION_P2_H

#include "TrigCosmicEventTPCnv/MdtTrackSegment_p2.h"
  
class MdtTrackSegmentCollection_p2 : public std::vector<MdtTrackSegment_p2> {};
// end of class definitions
 
#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : MdtTrackSegment_p1
 *
 * @brief persistent partner for MdtTrackSegment
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: MdtTrackSegment_p1.h,v 1.2 2009-04-01 22:02:52 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENT_P1_H
#define TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENT_P1_H

#include "TrigCosmicEvent/MdtTrackSegment.h"

class MdtTrackSegment_p1 
{
  friend class MdtTrackSegmentCnv_p1;

 public:
  
  MdtTrackSegment_p1() {}
  virtual ~MdtTrackSegment_p1(){}
  
 protected:

  int mStationId;
  int mTrackId;
  double mAlpha; // Need double precision
  double mB; // Need double precision
  int mSwap;
  float mT0;
  float mChi2;
  int mNHits;
  float mR;
  float mZ;

};

#endif

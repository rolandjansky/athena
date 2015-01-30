/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : MdtTrackSegment_p2
 *
 * @brief persistent partner for MdtTrackSegment
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * V2 06.01.2011 giagu - Stefano.Giagu@cern.ch
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENT_P2_H
#define TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENT_P2_H

class MdtTrackSegment_p2 {
  friend class MdtTrackSegmentCnv_p2;

 public:
  
  MdtTrackSegment_p2() {}
  virtual ~MdtTrackSegment_p2(){}
  
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

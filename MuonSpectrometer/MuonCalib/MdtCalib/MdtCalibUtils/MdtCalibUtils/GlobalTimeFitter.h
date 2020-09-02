/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_GLOBALTIMEFITTER_H
#define MUONCALIB_GLOBALTIMEFITTER_H

#include "MdtCalibInterfaces/IMdtSegmentFitter.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"

#include "MuonCalibEvent/MdtCalibHit.h"
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/MdtCalibrationFactory.h"

#include <iostream>

namespace MuonCalib {

/**
@class GlobalTimeFitter
Provides the operator to fit  ....
*/

  class GlobalTimeFitter {
  public:
    GlobalTimeFitter( const IMdtSegmentFitter * fitter ) : m_fitter(fitter) {m_rtRel=NULL;};
    GlobalTimeFitter( const IMdtSegmentFitter * fitter, const IRtRelation* rtRel ) : m_fitter(fitter), m_rtRel(rtRel) {};
    ~GlobalTimeFitter(){};  

  double GTFit( MuonCalibSegment * seg ) ;
  double GTFit2( MuonCalibSegment * seg ) ;

  IRtRelation * getDefaultRtRelation() ;

  inline void setRtRelation(const IRtRelation * rtRel) {m_rtRel=rtRel ;}  ;

  inline double getDefaultResolution(double r) { 
         double resolH8 = 0.164*std::exp(-r/4.43)+0.043 ; //  resolution from H8 TestBeam
         return 2.*resolH8 ; //  resolution from Cosmics in the pit
         // return 2.0 ; //  FLAT 2 mm resolution
  } ;
  bool fit (MuonCalibSegment &seg) const                         {  return m_fitter->fit(seg); }
  bool fit (MuonCalibSegment &seg, MuonCalib::IMdtSegmentFitter::HitSelection selection) const {  return m_fitter->fit(seg, selection); }

  private:
    const IMdtSegmentFitter * m_fitter;
    const IRtRelation * m_rtRel ;
  };

}

#endif

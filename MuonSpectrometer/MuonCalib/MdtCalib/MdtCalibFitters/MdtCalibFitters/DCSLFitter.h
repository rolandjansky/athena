/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DCSLFitter.h
//   Header file for class DCSLFitter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_DCSLFITTER_H
#define MUONCALIB_DCSLFITTER_H

#include "MdtCalibInterfaces/IMdtSegmentFitter.h"

#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib {
/**@class DCSLFitter
   Straight line fitter for drift circles
   
   @author Niels.Van.Eldik@cern.ch
*/
    

  class DCSLFitter : public IMdtSegmentFitter {
  public:
    DCSLFitter() : m_debug(false), m_error_dy0(0.0), m_error_dtheta(0.0) {}
  
    /** fit using all hits */
    bool fit( MuonCalibSegment& seg ) const;

    /** fit subset of the hits.
	If the HitSelection vector contains a 0 for a given hit the hit is used
	else the hit is not included in the fit.
	The size of the HitSelection vector should be equal to the number of hits on track else
	no fit is performed.
    */
    bool fit( MuonCalibSegment& seg, HitSelection selection ) const;
//    double error_dy0() { return m_error_dy0;} 
//    double error_dtheta() { return m_error_dtheta;} 

    /** set print level */
    void printLevel(int level);
  private:
  
    bool m_debug;
    
    mutable double m_error_dy0;
    mutable double m_error_dtheta;

    double getY( const Amg::Vector3D& p ) const { return p.z(); }
    double getZ( const Amg::Vector3D& p ) const { return p.y(); }
    Amg::Vector3D getVec( double x, double y, double z) const { return Amg::Vector3D( x, z, y ); }
  };

}

#endif

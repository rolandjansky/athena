/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuCCaFitter.h
//   Header file for class MuCCaFitter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////

#ifndef MuCCaFITTER_H
#define MuCCaFITTER_H

#include "MdtCalibInterfaces/IMdtSegmentFitter.h"

#include "MuonCalibEventBase/MuonCalibSegment.h"

#include <vector>

    
namespace MuonCalib {

/**@class MuCCaFitter
   Interface to the straight line fitter for drift circles used by Calib 
   
   @author Fabrizio.Petrucci@cern.ch
*/
class MuCCaFitter : public IMdtSegmentFitter {
 public:
  /** fit using all hits */
  bool fit( MuonCalibSegment& seg ) const;

  /** fit subset of the hits.
      If the HitSelection vector contains a 0 for a given hit the hit is used
      else the hit is not included in the fit.
      The size of the HitSelection vector should be equal to the number of hits on track else
      no fit is performed.
  */
  bool fit( MuonCalibSegment& seg, HitSelection selection ) const;

  /** set print level */
  void printLevel(int level);

 private:

 bool m_debug;

 /** these methods are needed to change the reference frame between the
     local one of the hit and one used by the fitter: apparently the 2 frames 
     are the same ... TO BE CHECKED!!!
 */   
 //  double getY( const Amg::Vector3D& p ) const { return p.y(); }
 //  double getX( const Amg::Vector3D& p ) const { return p.x(); }
 //  double getZ( const Amg::Vector3D& p ) const { return p.z(); }
 //  Amg::Vector3D getVec( double x, double y, double z) const { return Amg::Vector3D( x, y, z ); }
 double getY( const Amg::Vector3D& p ) const { return p.z(); }
 double getX( const Amg::Vector3D& p ) const { return p.x(); }
 double getZ( const Amg::Vector3D& p ) const { return p.y(); }
  Amg::Vector3D getVec( double x, double y, double z) const { return Amg::Vector3D( x, z, y ); }
};

/**@class MuCCaFitterImplementation
   The straight line fitter for drift circles used by Calib 
   
   @author Fabrizio.Petrucci@cern.ch, Paolo.Branchini@cern.ch 
*/
class MuCCaFitterImplementation{
 public:
  void Computelin(double x1, double y1, double r1,  
                  double x2, double y2, double r2);
  void Computelinparnew(double x1, double y1, double r1, 
                        double x2, double y2, double r2);
  void computehitsfromcircles(double x0,double y0,double r0,
                              double x1,double y1,double r1,double a,double b);
  //  void Computehitpsemes(int nhit,double *xcirc, double *ycirc,
  //                        double *rcirc, double a, double b);
  void Computehitpsemes(int nhit,std::vector<double> xcirc, std::vector<double> ycirc, std::vector<double>rcirc, double a, double b);
  //  void Computeparam3(int number_of_hits,double x[100],double y[100],
  //                     double r[100],double sr[100]);
  void Computeparam3(int number_of_hits,std::vector<double> x,std::vector<double> y,std::vector<double> r,std::vector<double> sr);
  double get_a();
  double get_b();
  double get_da();
  double get_db();
  double get_corrab();
  double get_chi2f();
 private:
  /** parameters of the 4 tangent lines from first and last hit */
  double m_angularcoefficient[4];  
  double m_bfpar[4]; 
  /** track points from 2 circles */
  double m_xout0; 
  double m_yout0;
  double m_xout1;
  double m_yout1;
  /** track points */
  double m_xpoint[100]; 
  double m_ypoint[100]; 
  /** results */
  double m_aoutn; //!< track slope 
  double m_sig2a; //!< track slope's variance 
  double m_bout;  //!< track constant term 
  double m_sig2b; //!< error on track constant term 
  double m_corrab;//!< correlation term
  double m_chi2f; //!< chisquared
};

}
#endif

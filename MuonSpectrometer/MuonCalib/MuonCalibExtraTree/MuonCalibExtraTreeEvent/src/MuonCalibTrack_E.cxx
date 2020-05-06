/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraUtils/MuonCalibSLPropagator.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHole_E.h"
#include "MuonCalibStl/DeleteObject.h"
#include <algorithm>
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

namespace MuonCalib {



  MuonCalibTrack_E::MuonCalibTrack_E() : m_x0(0.), m_y0(0.), m_z0(0.), m_phi(0.), m_theta(0.), m_qOverP(0.),m_author(0), 
                                         m_cov00(-999.),m_cov01(-999.),m_cov02(-999.),m_cov03(-999.),m_cov04(-999.), 
                                         m_cov11(-999.),m_cov12(-999.),m_cov13(-999.),m_cov14(-999.), 
                                         m_cov22(-999.),m_cov23(-999.),m_cov24(-999.), 
                                         m_cov33(-999.),m_cov34(-999.), 
                                         m_cov44(-999.), 
					 m_chi2(-999), m_ndof(-999)
  {
  }

  MuonCalibTrack_E::MuonCalibTrack_E(double x0, double y0, double z0, double phi, double theta, double qOverP, int author,
                                    double cov00, double cov01, double cov02, double cov03, double cov04,  
                                    double cov11, double cov12, double cov13, double cov14,  
                                    double cov22, double cov23, double cov24,  
                                    double cov33, double cov34,  
                                    double cov44,   
                                    double chi2, int ndof) :
    m_x0(x0), m_y0(y0), m_z0(z0), m_phi(phi), m_theta(theta), m_qOverP(qOverP),m_author(author),
    m_cov00(cov00), m_cov01(cov01), m_cov02(cov02), m_cov03(cov03), m_cov04(cov04), 
    m_cov11(cov11), m_cov12(cov12), m_cov13(cov13), m_cov14(cov14), 
    m_cov22(cov22), m_cov23(cov23), m_cov24(cov24), 
    m_cov33(cov33), m_cov34(cov34), 
    m_cov44(cov44), 
    m_chi2(chi2), m_ndof(ndof) {}
  
  MuonCalibTrack_E::MuonCalibTrack_E( const MuonCalibTrack_E& pat ) {
    copy(pat);    
  } 

  MuonCalibTrack_E::~MuonCalibTrack_E() {
    clear();
  }
  

  MuonCalibTrack_E& MuonCalibTrack_E::operator=( const MuonCalibTrack_E& rhs ){
    if(this!=&rhs){    
      clear();
      copy(rhs);
    }
    return(*this);
  }

  void MuonCalibTrack_E::copy( const MuonCalibTrack_E& track ){
      m_x0 = track.x0();
      m_y0 = track.y0();
      m_z0 = track.z0();
      m_phi = track.phi();
      m_theta = track.theta() ;
      m_qOverP = track.qOverP();
      m_author = track.author();
      m_cov00 = track.cov00();
      m_cov01 = track.cov01();
      m_cov02 = track.cov02();
      m_cov03 = track.cov03();
      m_cov04 = track.cov04();
      m_cov11 = track.cov11();
      m_cov12 = track.cov12();
      m_cov13 = track.cov13();
      m_cov14 = track.cov14();
      m_cov22 = track.cov22();
      m_cov23 = track.cov23();
      m_cov24 = track.cov24();
      m_cov33 = track.cov33();
      m_cov34 = track.cov34();
      m_cov44 = track.cov44();
      m_chi2 = track.chi2();
      m_ndof = track.m_ndof;
      m_hits.reserve( track.m_hits.size() );
      HitVector::const_iterator hit = track.beginHit();
      HitVector::const_iterator hit_end = track.endHit();
      for(; hit!=hit_end; ++hit) m_hits.push_back(  new MuonCalibHit_E( **hit ) ) ;
      m_holes.reserve( track.m_holes.size() );
      HoleVector::const_iterator hole = track.beginHole();
      HoleVector::const_iterator hole_end = track.endHole();
      for(; hole!=hole_end; ++hole) m_holes.push_back(  new MuonCalibHole_E( **hole ) ) ;
  }

  void MuonCalibTrack_E::clear() {
    std::for_each( m_hits.begin(), m_hits.end(), DeleteObject() ) ;
    m_hits.clear();
    std::for_each( m_holes.begin(), m_holes.end(), DeleteObject() ) ;
    m_holes.clear();
    //we do not have ownership of the segments on track 
    m_segments_on_track.clear();
  }

  
  void MuonCalibTrack_E::addHit( MuonCalibHit_E* hit ) {
    if(hit) 
      m_hits.push_back( hit ) ;
  }  
      
  void MuonCalibTrack_E::addHole( MuonCalibHole_E* hole ) {
    if(hole) 
      m_holes.push_back( hole ) ;
  }  

  float MuonCalib::MuonCalibTrack_E::z0ip() const 
  {
    //extrapolate z to IP                 
    Amg::Vector3D  trackPos(m_x0, m_y0, m_z0);
    Amg::Vector3D trackDir(1.,0.,0.);
    Amg::setTheta(trackDir, m_theta);
    Amg::setPhi(trackDir, m_phi);
    MuonCalib::MuonCalibSLPropagator slProp;
    Amg::Vector3D posAtIp = slProp.propagateToPerigee( trackPos, trackDir );
    float z0_atip = posAtIp.z();
    return z0_atip;
  }
  
}//namespace MuonCalib

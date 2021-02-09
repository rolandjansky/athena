/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// PatternTrackParameters.cxx , (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkPatternParameters/PatternTrackParameters.h"

///////////////////////////////////////////////////////////////////
// Conversion Trk::PatternTrackParameters to  Trk::TrackParameters
///////////////////////////////////////////////////////////////////

const Trk::ParametersBase<5,Trk::Charged>*  Trk::PatternTrackParameters::convert(bool covariance) const
{
  AmgSymMatrix(5)* e = nullptr;
  if(covariance && m_covariance != nullptr) {
    e = new AmgSymMatrix(5)(*m_covariance);
  }
  const AmgVector(5)& p = m_parameters;
  return m_surface ? (m_surface->createTrackParameters(p[0],p[1],p[2],p[3],p[4],e)): nullptr;
}

///////////////////////////////////////////////////////////////////
// Conversion Trk::TrackParameters to Trk::PatternTrackParameters
///////////////////////////////////////////////////////////////////

bool Trk::PatternTrackParameters::production(const Trk::ParametersBase<5,Trk::Charged>* T) {

  if(!T) { return false;
}
  if (!T->hasSurface()) {
    return false;
  }

  m_surface.reset(T->associatedSurface().isFree() ? T->associatedSurface().clone() : &T->associatedSurface());

  m_parameters = T->parameters();

  const AmgSymMatrix(5)* C = T->covariance();   

  if(C) {
    if (m_covariance == nullptr) {
      m_covariance = std::make_unique<AmgSymMatrix(5)>();
    }

    for (std::size_t i = 0; i < 5; i++) {
      for (std::size_t j = 0; j <= i; j++) {
        m_covariance->fillSymmetric(i, j, (*C)(i, j));
      }
    }
  }
  else {
    m_covariance.reset(nullptr);
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Covariance matrix production using jacobian cov(this) = J*( Tp cov)*Jt
///////////////////////////////////////////////////////////////////
      

AmgSymMatrix(5) Trk::PatternTrackParameters::newCovarianceMatrix
(const AmgSymMatrix(5) & V, const double * Jac)
{
  AmgSymMatrix(5) rv;

  double a11 = (Jac[ 0]*V(0, 0)+Jac[ 1]*V(0, 1)+Jac[ 2]*V(0, 2))+(Jac[ 3]*V(0, 3)+Jac[ 4]*V(0, 4));
  double a12 = (Jac[ 0]*V(0, 1)+Jac[ 1]*V(1, 1)+Jac[ 2]*V(1, 2))+(Jac[ 3]*V(1, 3)+Jac[ 4]*V(1, 4));
  double a13 = (Jac[ 0]*V(0, 2)+Jac[ 1]*V(1, 2)+Jac[ 2]*V(2, 2))+(Jac[ 3]*V(2, 3)+Jac[ 4]*V(2, 4));
  double a14 = (Jac[ 0]*V(0, 3)+Jac[ 1]*V(1, 3)+Jac[ 2]*V(2, 3))+(Jac[ 3]*V(3, 3)+Jac[ 4]*V(3, 4));
  double a15 = (Jac[ 0]*V(0, 4)+Jac[ 1]*V(1, 4)+Jac[ 2]*V(2, 4))+(Jac[ 3]*V(3, 4)+Jac[ 4]*V(4, 4));

  rv.fillSymmetric(0, 0, (a11*Jac[ 0]+a12*Jac[ 1]+a13*Jac[ 2])+(a14*Jac[ 3]+a15*Jac[ 4]));

  double a21 = (Jac[ 5]*V(0, 0)+Jac[ 6]*V(0, 1)+Jac[ 7]*V(0, 2))+(Jac[ 8]*V(0, 3)+Jac[ 9]*V(0, 4));
  double a22 = (Jac[ 5]*V(0, 1)+Jac[ 6]*V(1, 1)+Jac[ 7]*V(1, 2))+(Jac[ 8]*V(1, 3)+Jac[ 9]*V(1, 4));
  double a23 = (Jac[ 5]*V(0, 2)+Jac[ 6]*V(1, 2)+Jac[ 7]*V(2, 2))+(Jac[ 8]*V(2, 3)+Jac[ 9]*V(2, 4));
  double a24 = (Jac[ 5]*V(0, 3)+Jac[ 6]*V(1, 3)+Jac[ 7]*V(2, 3))+(Jac[ 8]*V(3, 3)+Jac[ 9]*V(3, 4));
  double a25 = (Jac[ 5]*V(0, 4)+Jac[ 6]*V(1, 4)+Jac[ 7]*V(2, 4))+(Jac[ 8]*V(3, 4)+Jac[ 9]*V(4, 4));

  rv.fillSymmetric(1, 0, (a21*Jac[ 0]+a22*Jac[ 1]+a23*Jac[ 2])+(a24*Jac[ 3]+a25*Jac[ 4]));
  rv.fillSymmetric(1, 1, (a21*Jac[ 5]+a22*Jac[ 6]+a23*Jac[ 7])+(a24*Jac[ 8]+a25*Jac[ 9]));

  double a31 = (Jac[10]*V(0, 0)+Jac[11]*V(0, 1)+Jac[12]*V(0, 2))+(Jac[13]*V(0, 3)+Jac[14]*V(0, 4));
  double a32 = (Jac[10]*V(0, 1)+Jac[11]*V(1, 1)+Jac[12]*V(1, 2))+(Jac[13]*V(1, 3)+Jac[14]*V(1, 4));
  double a33 = (Jac[10]*V(0, 2)+Jac[11]*V(1, 2)+Jac[12]*V(2, 2))+(Jac[13]*V(2, 3)+Jac[14]*V(2, 4));
  double a34 = (Jac[10]*V(0, 3)+Jac[11]*V(1, 3)+Jac[12]*V(2, 3))+(Jac[13]*V(3, 3)+Jac[14]*V(3, 4));
  double a35 = (Jac[10]*V(0, 4)+Jac[11]*V(1, 4)+Jac[12]*V(2, 4))+(Jac[13]*V(3, 4)+Jac[14]*V(4, 4));

  rv.fillSymmetric(2, 0, (a31*Jac[ 0]+a32*Jac[ 1]+a33*Jac[ 2])+(a34*Jac[ 3]+a35*Jac[ 4]));
  rv.fillSymmetric(2, 1, (a31*Jac[ 5]+a32*Jac[ 6]+a33*Jac[ 7])+(a34*Jac[ 8]+a35*Jac[ 9]));
  rv.fillSymmetric(2, 2, (a31*Jac[10]+a32*Jac[11]+a33*Jac[12])+(a34*Jac[13]+a35*Jac[14]));

  double a41 = (Jac[15]*V(0, 0)+Jac[16]*V(0, 1)+Jac[17]*V(0, 2))+(Jac[18]*V(0, 3)+Jac[19]*V(0, 4));
  double a42 = (Jac[15]*V(0, 1)+Jac[16]*V(1, 1)+Jac[17]*V(1, 2))+(Jac[18]*V(1, 3)+Jac[19]*V(1, 4));
  double a43 = (Jac[15]*V(0, 2)+Jac[16]*V(1, 2)+Jac[17]*V(2, 2))+(Jac[18]*V(2, 3)+Jac[19]*V(2, 4));
  double a44 = (Jac[15]*V(0, 3)+Jac[16]*V(1, 3)+Jac[17]*V(2, 3))+(Jac[18]*V(3, 3)+Jac[19]*V(3, 4));
  double a45 = (Jac[15]*V(0, 4)+Jac[16]*V(1, 4)+Jac[17]*V(2, 4))+(Jac[18]*V(3, 4)+Jac[19]*V(4, 4));

  rv.fillSymmetric(3, 0, (a41*Jac[ 0]+a42*Jac[ 1]+a43*Jac[ 2])+(a44*Jac[ 3]+a45*Jac[ 4]));
  rv.fillSymmetric(3, 1, (a41*Jac[ 5]+a42*Jac[ 6]+a43*Jac[ 7])+(a44*Jac[ 8]+a45*Jac[ 9]));
  rv.fillSymmetric(3, 2, (a41*Jac[10]+a42*Jac[11]+a43*Jac[12])+(a44*Jac[13]+a45*Jac[14]));
  rv.fillSymmetric(3, 3, (a41*Jac[15]+a42*Jac[16]+a43*Jac[17])+(a44*Jac[18]+a45*Jac[19]));

  double a51 = Jac[20]*V(0, 4);
  double a52 = Jac[20]*V(1, 4);
  double a53 = Jac[20]*V(2, 4);
  double a54 = Jac[20]*V(3, 4);
  double a55 = Jac[20]*V(4, 4);

  rv.fillSymmetric(4, 0, (a51*Jac[ 0]+a52*Jac[ 1]+a53*Jac[ 2])+(a54*Jac[ 3]+a55*Jac[ 4]));
  rv.fillSymmetric(4, 1, (a51*Jac[ 5]+a52*Jac[ 6]+a53*Jac[ 7])+(a54*Jac[ 8]+a55*Jac[ 9]));
  rv.fillSymmetric(4, 2, (a51*Jac[10]+a52*Jac[11]+a53*Jac[12])+(a54*Jac[13]+a55*Jac[14]));
  rv.fillSymmetric(4, 3, (a51*Jac[15]+a52*Jac[16]+a53*Jac[17])+(a54*Jac[18]+a55*Jac[19]));
  rv.fillSymmetric(4, 4,                                                    a55*Jac[20]);

  return rv;
}

///////////////////////////////////////////////////////////////////
// Global position of simple track parameters
///////////////////////////////////////////////////////////////////

Amg::Vector3D Trk::PatternTrackParameters::position() const
{
  return calculatePosition();
} 

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& Trk::operator << 
  (std::ostream& sl,const Trk::PatternTrackParameters& se)
{ 
  return se.dump(sl); 
}   

MsgStream& Trk::operator    << 
(MsgStream& sl, const Trk::PatternTrackParameters& se)
{
  return se.dump(sl);
}
 
///////////////////////////////////////////////////////////////////
// Print track parameters information
///////////////////////////////////////////////////////////////////

std::ostream& Trk::PatternTrackParameters::dump( std::ostream& out ) const
{
  const Trk::Surface*  s = m_surface.get();
  const AmgVector(5)&  P = m_parameters;
  std::string name;
  std::string iscov;

  boost::io::ios_all_saver ias(out);

  const Trk::DiscSurface* di;
  const Trk::ConeSurface* cn;
  const Trk::PlaneSurface* pl;
  const Trk::PerigeeSurface* pe;
  const Trk::CylinderSurface* cy;
  const Trk::StraightLineSurface* li;
  
  if     ((pl=dynamic_cast<const Trk::PlaneSurface*>       (s))) { name = "Plane"   ;
  } else if((li=dynamic_cast<const Trk::StraightLineSurface*>(s))) { name = "Line"    ;
  } else if((di=dynamic_cast<const Trk::DiscSurface*>        (s))) { name = "Disc"    ;
  } else if((cy=dynamic_cast<const Trk::CylinderSurface*>    (s))) { name = "Cylinder";
  } else if((pe=dynamic_cast<const Trk::PerigeeSurface*>     (s))) { name = "Perigee" ;
  } else if((cn=dynamic_cast<const Trk::ConeSurface*>        (s))) { name = "Cone"    ;
  } else  {
    out << "Track parameters are not valid " << std::endl;
    ias.restore();
    return out;
  }

  out << "Track parameters for " << name << " surface " << std::endl;
   
  out.unsetf(std::ios::fixed);
  out.setf  (std::ios::showpos);
  out.setf  (std::ios::scientific);

  if (m_covariance != nullptr) {
    const AmgSymMatrix(5) & V = *m_covariance;
    out << std::setprecision(4) <<
      P[ 0]<<" |"<<V(0, 0) << std::endl;
    out << std::setprecision(4) <<
      P[ 1]<<" |"<<V(0, 1)<<" "<<V(1, 1) << std::endl;
    out << std::setprecision(4) <<
      P[ 2]<<" |"<<V(0, 2)<<" "<<V(1, 2)<<" "<<V(2, 2) << std::endl;
    out << std::setprecision(4) <<
      P[ 3]<<" |"<<V(0, 3)<<" "<<V(1, 3)<<" "<<V(2, 3)<<" "<<V(3, 3) << std::endl;
    out << std::setprecision(4) <<
      P[ 4]<<" |"<<V(0, 4)<<" "<<V(1, 4)<<" "<<V(2, 4)<<" "<<V(3, 4)<<" "<<V(4, 4) << std::endl;
  }
  else {
    out << std::setprecision(4) << P[ 0] << " |" << std::endl;
    out << std::setprecision(4) << P[ 1] << " |" << std::endl;
    out << std::setprecision(4) << P[ 2] << " |" << std::endl;
    out << std::setprecision(4) << P[ 3] << " |" << std::endl;
    out << std::setprecision(4) << P[ 4] << " |" << std::endl;
  }
  out << std::setprecision(-1);

  out.setf  (std::ios::fixed);
  out.unsetf(std::ios::showpos);
  out.unsetf(std::ios::scientific);
  ias.restore();
  return out;
}

///////////////////////////////////////////////////////////////////
// Print track parameters information
///////////////////////////////////////////////////////////////////

MsgStream& Trk::PatternTrackParameters::dump(MsgStream& out) const
{
  const Trk::Surface*  s = m_surface.get();
  const AmgVector(5)&  P = m_parameters;
  std::string name;
  std::string iscov;

  const Trk::DiscSurface* di;
  const Trk::ConeSurface* cn;
  const Trk::PlaneSurface* pl;
  const Trk::PerigeeSurface* pe;
  const Trk::CylinderSurface* cy;
  const Trk::StraightLineSurface* li;

  if     ((pl=dynamic_cast<const Trk::PlaneSurface*>       (s))) { name = "Plane"   ;
  } else if((li=dynamic_cast<const Trk::StraightLineSurface*>(s))) { name = "Line"    ;
  } else if((di=dynamic_cast<const Trk::DiscSurface*>        (s))) { name = "Disc"    ;
  } else if((cy=dynamic_cast<const Trk::CylinderSurface*>    (s))) { name = "Cylinder";
  } else if((pe=dynamic_cast<const Trk::PerigeeSurface*>     (s))) { name = "Perigee" ;
  } else if((cn=dynamic_cast<const Trk::ConeSurface*>        (s))) { name = "Cone"    ;
  } else  {
    out << "Track parameters are not valid " << std::endl;
    return out;
  }

  out << "Track parameters for " << name << " surface " << std::endl;
  
  out.unsetf(std::ios::fixed);
  out.setf  (std::ios::showpos);
  out.setf  (std::ios::scientific);

  if (m_covariance != nullptr) {
    const AmgSymMatrix(5) & V = *m_covariance;
    out << std::setprecision(4) <<
      P[ 0]<<" |"<<V(0, 0) << std::endl;
    out << std::setprecision(4) <<
      P[ 1]<<" |"<<V(0, 1)<<" "<<V(1, 1) << std::endl;
    out << std::setprecision(4) <<
      P[ 2]<<" |"<<V(0, 2)<<" "<<V(1, 2)<<" "<<V(2, 2) << std::endl;
    out << std::setprecision(4) <<
      P[ 3]<<" |"<<V(0, 3)<<" "<<V(1, 3)<<" "<<V(2, 3)<<" "<<V(3, 3) << std::endl;
    out << std::setprecision(4) <<
      P[ 4]<<" |"<<V(0, 4)<<" "<<V(1, 4)<<" "<<V(2, 4)<<" "<<V(3, 4)<<" "<<V(4, 4) << std::endl;
  }
  else {
    out << std::setprecision(4) << P[ 0] << " |" << std::endl;
    out << std::setprecision(4) << P[ 1] << " |" << std::endl;
    out << std::setprecision(4) << P[ 2] << " |" << std::endl;
    out << std::setprecision(4) << P[ 3] << " |" << std::endl;
    out << std::setprecision(4) << P[ 4] << " |" << std::endl;
  }
  out << std::setprecision(-1);

  out.setf  (std::ios::fixed);
  out.unsetf(std::ios::showpos);
  out.unsetf(std::ios::scientific);
  return out;
}	

///////////////////////////////////////////////////////////////////
// Protected methods
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Global position for track parameters on plane
///////////////////////////////////////////////////////////////////

Amg::Vector3D Trk::PatternTrackParameters::localToGlobal
(const Trk::PlaneSurface* su) const
{
  const Amg::Transform3D&  T = su->transform();
  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};

  Amg::Vector3D gp (m_parameters[0]*Ax[0]+m_parameters[1]*Ay[0]+T(0,3),
		    m_parameters[0]*Ax[1]+m_parameters[1]*Ay[1]+T(1,3),
		    m_parameters[0]*Ax[2]+m_parameters[1]*Ay[2]+T(2,3));
  return gp;
}

///////////////////////////////////////////////////////////////////
// Global position for track parameters on straight line
///////////////////////////////////////////////////////////////////

Amg::Vector3D Trk::PatternTrackParameters::localToGlobal
(const Trk::StraightLineSurface* su) const
{
  const Amg::Transform3D&  T = su->transform();
  double A[3] = {T(0,2),T(1,2),T(2,2)};

  double Sf;
  double Cf; sincos(m_parameters[2],&Sf,&Cf);
  double Se;
  double Ce; sincos(m_parameters[3],&Se,&Ce);

  double P3 = Cf*Se; 
  double P4 = Sf*Se; 
  double P5 = Ce;    
  double Bx = A[1]*P5-A[2]*P4;
  double By = A[2]*P3-A[0]*P5;
  double Bz = A[0]*P4-A[1]*P3;
  double Bn = 1./sqrt(Bx*Bx+By*By+Bz*Bz); Bx*=Bn; By*=Bn; Bz*=Bn;

  Amg::Vector3D gp
    (m_parameters[1]*A[0]+Bx*m_parameters[0]+T(0,3),
     m_parameters[1]*A[1]+By*m_parameters[0]+T(1,3),
     m_parameters[1]*A[2]+Bz*m_parameters[0]+T(2,3));
  return gp;
}

///////////////////////////////////////////////////////////////////
// Global position for track parameters on disck
///////////////////////////////////////////////////////////////////

Amg::Vector3D Trk::PatternTrackParameters::localToGlobal
(const Trk::DiscSurface* su) const
{
  const Amg::Transform3D&  T = su->transform();
  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};

  double Sf;
  double Cf; sincos(m_parameters[1],&Sf,&Cf);

  double d0 = Cf*Ax[0]+Sf*Ay[0]; 
  double d1 = Cf*Ax[1]+Sf*Ay[1]; 
  double d2 = Cf*Ax[2]+Sf*Ay[2];

  Amg::Vector3D gp
    (m_parameters[0]*d0+T(0,3),
     m_parameters[0]*d1+T(1,3),
     m_parameters[0]*d2+T(2,3));
  return gp;
}

///////////////////////////////////////////////////////////////////
// Global position for track parameters on cylinder
///////////////////////////////////////////////////////////////////

Amg::Vector3D Trk::PatternTrackParameters::localToGlobal
(const Trk::CylinderSurface* su) const
{
  const Amg::Transform3D&  T = su->transform();
  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};
  double Az[3] = {T(0,2),T(1,2),T(2,2)};

  double  R = su->bounds().r();
  double fr = m_parameters[0]/R;

  double Sf;
  double Cf; sincos(fr,&Sf,&Cf);

  Amg::Vector3D gp
    (R*(Cf*Ax[0]+Sf*Ay[0])+m_parameters[1]*Az[0]+T(0,3),
     R*(Cf*Ax[1]+Sf*Ay[1])+m_parameters[1]*Az[1]+T(1,3),
     R*(Cf*Ax[2]+Sf*Ay[2])+m_parameters[1]*Az[2]+T(2,3));
  return gp;
}

///////////////////////////////////////////////////////////////////
// Global position for track parameters on perigee
///////////////////////////////////////////////////////////////////

Amg::Vector3D Trk::PatternTrackParameters::localToGlobal
(const Trk::PerigeeSurface* su) const
{
  const Amg::Transform3D&  T = su->transform();
  double A[3] = {T(0,2),T(1,2),T(2,2)};

  double Sf;
  double Cf; sincos(m_parameters[2],&Sf,&Cf);
  double Se;
  double Ce; sincos(m_parameters[3],&Se,&Ce);

  double P3 = Cf*Se; 
  double P4 = Sf*Se; 
  double P5 = Ce;    
  double Bx = A[1]*P5-A[2]*P4;
  double By = A[2]*P3-A[0]*P5;
  double Bz = A[0]*P4-A[1]*P3;
  double Bn = 1./sqrt(Bx*Bx+By*By+Bz*Bz); Bx*=Bn; By*=Bn; Bz*=Bn;

  Amg::Vector3D gp
    (m_parameters[1]*A[0]+Bx*m_parameters[0]+T(0,3),
     m_parameters[1]*A[1]+By*m_parameters[0]+T(1,3),
     m_parameters[1]*A[2]+Bz*m_parameters[0]+T(2,3));
  return gp;
}

///////////////////////////////////////////////////////////////////
// Global position for track parameters on cone
///////////////////////////////////////////////////////////////////

Amg::Vector3D Trk::PatternTrackParameters::localToGlobal
(const Trk::ConeSurface* su) const
{
  const Amg::Transform3D&  T = su->transform();
  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};
  double Az[3] = {T(0,2),T(1,2),T(2,2)};

  double r  = m_parameters[1]*su->bounds().tanAlpha();
  double Sf;
  double Cf; sincos((m_parameters[0]/r),&Sf,&Cf);
  double xl = r*Cf;
  double yl = r*Sf;


  Amg::Vector3D gp
    (Ax[0]*xl+Ay[0]*yl+Az[0]*m_parameters[1]+T(0,3),
     Ax[1]*xl+Ay[1]*yl+Az[1]*m_parameters[1]+T(1,3),
     Ax[2]*xl+Ay[2]*yl+Az[2]*m_parameters[1]+T(2,3));
  return gp;
}

///////////////////////////////////////////////////////////////////
// Initiate track parameters
///////////////////////////////////////////////////////////////////

bool Trk::PatternTrackParameters::initiate
(PatternTrackParameters& Tp, const Amg::Vector2D& P,const Amg::MatrixX& E)
{
  
  int n = E.rows(); if(n<=0 || n>2) { return false;
}

  if (Tp.m_covariance != nullptr) {
    if (m_covariance == nullptr) {
      m_covariance = std::make_unique<AmgSymMatrix(5)>(*Tp.m_covariance);
    } else {
      *m_covariance = *Tp.m_covariance;
    }
  } else {
    if (m_covariance == nullptr) {
      m_covariance = std::make_unique<AmgSymMatrix(5)>();
    }
  }

  m_parameters[0] = P  (0);

  m_covariance->fillSymmetric(0, 0, E(0,0));
  
  if(n==2) {
    m_parameters[ 1] = P(1);
    m_covariance->fillSymmetric(0, 1, E(1,0));
    m_covariance->fillSymmetric(1, 1, E(1,1));
  }
  else    {
    m_parameters[ 1] = Tp.m_parameters[ 1];
  }
  m_parameters[ 2] = Tp.m_parameters[ 2];
  m_parameters[ 3] = Tp.m_parameters[ 3];
  m_parameters[ 4] = Tp.m_parameters[ 4];

  if (Tp.m_surface != nullptr) {
    m_surface.reset(Tp.m_surface->isFree() ? Tp.m_surface->clone() : Tp.m_surface.get());
  } else {
    m_surface.reset(nullptr);
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Change direction of the parameters
///////////////////////////////////////////////////////////////////
      
void Trk::PatternTrackParameters::changeDirection()
{
  const double pi = M_PI;
  const double pi2 = 2.*M_PI; //NB CLHEP also defines pi and pi2 constants.

  m_parameters[ 2] =  m_parameters[2]-pi; 
  m_parameters[ 3] =  pi-m_parameters[3];
  m_parameters[ 4] = -m_parameters[4]   ;

  if( m_parameters[2] < -pi) { m_parameters[2]+=pi2;
}


  if(!dynamic_cast<const Trk::StraightLineSurface*>(m_surface.get()) &&
     !dynamic_cast<const Trk::PerigeeSurface*>     (m_surface.get())) {
    
    if(m_covariance == nullptr) { return;
}

    m_covariance->fillSymmetric(0, 3, -(*m_covariance)(0, 3));
    m_covariance->fillSymmetric(1, 3, -(*m_covariance)(1, 3));
    m_covariance->fillSymmetric(2, 3, -(*m_covariance)(2, 3));
    m_covariance->fillSymmetric(0, 4, -(*m_covariance)(0, 4));
    m_covariance->fillSymmetric(1, 4, -(*m_covariance)(1, 4));
    m_covariance->fillSymmetric(2, 4, -(*m_covariance)(2, 4));

    return;
  }

  m_parameters[ 0] = -m_parameters[ 0];


  if(m_covariance == nullptr) { return;
}

  m_covariance->fillSymmetric(0, 1, -(*m_covariance)(0, 1));
  m_covariance->fillSymmetric(0, 2, -(*m_covariance)(0, 2));
  m_covariance->fillSymmetric(1, 3, -(*m_covariance)(1, 3));
  m_covariance->fillSymmetric(2, 3, -(*m_covariance)(2, 3));
  m_covariance->fillSymmetric(1, 4, -(*m_covariance)(1, 4));
  m_covariance->fillSymmetric(2, 4, -(*m_covariance)(2, 4));
}

Amg::Vector3D Trk::PatternTrackParameters::calculatePosition(void) const {
  if (!m_surface) {
    return Amg::Vector3D(0, 0, 0);
  }

  if (const Trk::PlaneSurface * plane = dynamic_cast<const Trk::PlaneSurface*>(m_surface.get()); plane != nullptr) {
    return localToGlobal(plane);
  } else if (const Trk::StraightLineSurface * line = dynamic_cast<const Trk::StraightLineSurface*>(m_surface.get()); line != nullptr) {
    return localToGlobal(line);
  } else if (const Trk::DiscSurface * disc = dynamic_cast<const Trk::DiscSurface*>(m_surface.get()); disc != nullptr) {
    return localToGlobal(disc);
  } else if (const Trk::CylinderSurface * cylinder = dynamic_cast<const Trk::CylinderSurface*>(m_surface.get()); cylinder != nullptr) {
    return localToGlobal(cylinder);
  } else if (const Trk::PerigeeSurface * pline = dynamic_cast<const Trk::PerigeeSurface*>(m_surface.get()); pline != nullptr) {
    return localToGlobal(pline);
  } else if (const Trk::ConeSurface * cone = dynamic_cast<const Trk::ConeSurface*>(m_surface.get()); cone != nullptr) {
    return localToGlobal(cone);
  } else {
    return Amg::Vector3D(0, 0, 0);
  }
}

Amg::Vector3D Trk::PatternTrackParameters::calculateMomentum(void) const {
  double p = m_parameters[4] != 0. ? 1. / std::abs(m_parameters[4]) : 10e9;

  double Sf = std::sin(m_parameters[2]), Cf = std::cos(m_parameters[2]);
  double Se = std::sin(m_parameters[3]), Ce = std::cos(m_parameters[3]);

  return Amg::Vector3D(p * Se * Cf, p * Se * Sf, p * Ce);
}

bool Trk::PatternTrackParameters::hasSurface() const {
  return m_surface != nullptr;
}

Amg::RotationMatrix3D Trk::PatternTrackParameters::measurementFrame() const {
  return associatedSurface().measurementFrame(this->position(), this->momentum());
}

Trk::PatternTrackParameters * Trk::PatternTrackParameters::clone() const {
  return new PatternTrackParameters(*this);
}

Trk::ParametersType Trk::PatternTrackParameters::type() const {
  return Trk::Pattern;
}

int Trk::PatternTrackParameters::surfaceType() const {
  return m_surface->type();
}

void Trk::PatternTrackParameters::updateParametersHelper(const AmgVector(5) &) {
}

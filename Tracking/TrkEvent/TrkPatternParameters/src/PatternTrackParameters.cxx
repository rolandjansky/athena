/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "TrkParametersBase/ChargeDefinition.h"

///////////////////////////////////////////////////////////////////
// Conversion Trk::PatternTrackParameters to  Trk::TrackParameters
///////////////////////////////////////////////////////////////////

const Trk::ParametersBase<5,Trk::Charged>*  Trk::PatternTrackParameters::convert(bool covariance) const
{
  AmgSymMatrix(5)* e = 0;
  if(covariance && m_iscovariance) {

    const double* c = &m_covariance[0];
    e = new AmgSymMatrix(5);
    (*e)<<
      c[ 0],c[ 1],c[ 3],c[ 6],c[10],
      c[ 1],c[ 2],c[ 4],c[ 7],c[11],
      c[ 3],c[ 4],c[ 5],c[ 8],c[12],
      c[ 6],c[ 7],c[ 8],c[ 9],c[13],
      c[10],c[11],c[12],c[13],c[14];
  }
  const double* p = &m_parameters[0];
  return m_surface->createTrackParameters(p[0],p[1],p[2],p[3],p[4],e);
}

///////////////////////////////////////////////////////////////////
// Conversion Trk::TrackParameters to Trk::PatternTrackParameters
///////////////////////////////////////////////////////////////////

bool Trk::PatternTrackParameters::production(const Trk::ParametersBase<5,Trk::Charged>* T) {

  if(!T) return false;

  m_surface = &T->associatedSurface(); if(!m_surface) return false;

  const AmgVector(5) Vp = T->parameters() ;
  m_parameters[0] = Vp[0];
  m_parameters[1] = Vp[1];
  m_parameters[2] = Vp[2];
  m_parameters[3] = Vp[3];
  m_parameters[4] = Vp[4];

  const AmgSymMatrix(5)* C = T->covariance();   

  if(C) {

    const AmgSymMatrix(5)& V = *C              ;
    double*                c = &m_covariance[0];
    c[ 0]=V(0,0);
    c[ 1]=V(1,0); c[ 2]=V(1,1);
    c[ 3]=V(2,0); c[ 4]=V(2,1); c[ 5]=V(2,2);
    c[ 6]=V(3,0); c[ 7]=V(3,1); c[ 8]=V(3,2); c[ 9]=V(3,3);
    c[10]=V(4,0); c[11]=V(4,1); c[12]=V(4,2); c[13]=V(4,3); c[14]=V(4,4);
    m_iscovariance = true;
  }
  else {
    m_iscovariance = false;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Covariance matrix production using jacobian cov(this) = J*( Tp cov)*Jt
///////////////////////////////////////////////////////////////////
      
void Trk::PatternTrackParameters::newCovarianceMatrix
(Trk::PatternTrackParameters& Tp,double* Jac)
{
  double* V  = &Tp.m_covariance[0];
  double a11 = (Jac[ 0]*V[ 0]+Jac[ 1]*V[ 1]+Jac[ 2]*V[ 3])+(Jac[ 3]*V[ 6]+Jac[ 4]*V[10]);
  double a12 = (Jac[ 0]*V[ 1]+Jac[ 1]*V[ 2]+Jac[ 2]*V[ 4])+(Jac[ 3]*V[ 7]+Jac[ 4]*V[11]);  
  double a13 = (Jac[ 0]*V[ 3]+Jac[ 1]*V[ 4]+Jac[ 2]*V[ 5])+(Jac[ 3]*V[ 8]+Jac[ 4]*V[12]);   
  double a14 = (Jac[ 0]*V[ 6]+Jac[ 1]*V[ 7]+Jac[ 2]*V[ 8])+(Jac[ 3]*V[ 9]+Jac[ 4]*V[13]);   
  double a15 = (Jac[ 0]*V[10]+Jac[ 1]*V[11]+Jac[ 2]*V[12])+(Jac[ 3]*V[13]+Jac[ 4]*V[14]);   

  m_covariance[ 0] = (a11*Jac[ 0]+a12*Jac[ 1]+a13*Jac[ 2])+(a14*Jac[ 3]+a15*Jac[ 4]);
  
  double a21 = (Jac[ 5]*V[ 0]+Jac[ 6]*V[ 1]+Jac[ 7]*V[ 3])+(Jac[ 8]*V[ 6]+Jac[ 9]*V[10]);   
  double a22 = (Jac[ 5]*V[ 1]+Jac[ 6]*V[ 2]+Jac[ 7]*V[ 4])+(Jac[ 8]*V[ 7]+Jac[ 9]*V[11]);   
  double a23 = (Jac[ 5]*V[ 3]+Jac[ 6]*V[ 4]+Jac[ 7]*V[ 5])+(Jac[ 8]*V[ 8]+Jac[ 9]*V[12]);   
  double a24 = (Jac[ 5]*V[ 6]+Jac[ 6]*V[ 7]+Jac[ 7]*V[ 8])+(Jac[ 8]*V[ 9]+Jac[ 9]*V[13]);   
  double a25 = (Jac[ 5]*V[10]+Jac[ 6]*V[11]+Jac[ 7]*V[12])+(Jac[ 8]*V[13]+Jac[ 9]*V[14]);   

  m_covariance[ 1] = (a21*Jac[ 0]+a22*Jac[ 1]+a23*Jac[ 2])+(a24*Jac[ 3]+a25*Jac[ 4]);
  m_covariance[ 2] = (a21*Jac[ 5]+a22*Jac[ 6]+a23*Jac[ 7])+(a24*Jac[ 8]+a25*Jac[ 9]);
  
  double a31 = (Jac[10]*V[ 0]+Jac[11]*V[ 1]+Jac[12]*V[ 3])+(Jac[13]*V[ 6]+Jac[14]*V[10]);   
  double a32 = (Jac[10]*V[ 1]+Jac[11]*V[ 2]+Jac[12]*V[ 4])+(Jac[13]*V[ 7]+Jac[14]*V[11]);   
  double a33 = (Jac[10]*V[ 3]+Jac[11]*V[ 4]+Jac[12]*V[ 5])+(Jac[13]*V[ 8]+Jac[14]*V[12]);   
  double a34 = (Jac[10]*V[ 6]+Jac[11]*V[ 7]+Jac[12]*V[ 8])+(Jac[13]*V[ 9]+Jac[14]*V[13]);   
  double a35 = (Jac[10]*V[10]+Jac[11]*V[11]+Jac[12]*V[12])+(Jac[13]*V[13]+Jac[14]*V[14]);   
  
  m_covariance[ 3] = (a31*Jac[ 0]+a32*Jac[ 1]+a33*Jac[ 2])+(a34*Jac[ 3]+a35*Jac[ 4]);
  m_covariance[ 4] = (a31*Jac[ 5]+a32*Jac[ 6]+a33*Jac[ 7])+(a34*Jac[ 8]+a35*Jac[ 9]);
  m_covariance[ 5] = (a31*Jac[10]+a32*Jac[11]+a33*Jac[12])+(a34*Jac[13]+a35*Jac[14]);

  double a41 = (Jac[15]*V[ 0]+Jac[16]*V[ 1]+Jac[17]*V[ 3])+(Jac[18]*V[ 6]+Jac[19]*V[10]);   
  double a42 = (Jac[15]*V[ 1]+Jac[16]*V[ 2]+Jac[17]*V[ 4])+(Jac[18]*V[ 7]+Jac[19]*V[11]);   
  double a43 = (Jac[15]*V[ 3]+Jac[16]*V[ 4]+Jac[17]*V[ 5])+(Jac[18]*V[ 8]+Jac[19]*V[12]);   
  double a44 = (Jac[15]*V[ 6]+Jac[16]*V[ 7]+Jac[17]*V[ 8])+(Jac[18]*V[ 9]+Jac[19]*V[13]);   
  double a45 = (Jac[15]*V[10]+Jac[16]*V[11]+Jac[17]*V[12])+(Jac[18]*V[13]+Jac[19]*V[14]);   

  m_covariance[ 6] = (a41*Jac[ 0]+a42*Jac[ 1]+a43*Jac[ 2])+(a44*Jac[ 3]+a45*Jac[ 4]);
  m_covariance[ 7] = (a41*Jac[ 5]+a42*Jac[ 6]+a43*Jac[ 7])+(a44*Jac[ 8]+a45*Jac[ 9]);
  m_covariance[ 8] = (a41*Jac[10]+a42*Jac[11]+a43*Jac[12])+(a44*Jac[13]+a45*Jac[14]);
  m_covariance[ 9] = (a41*Jac[15]+a42*Jac[16]+a43*Jac[17])+(a44*Jac[18]+a45*Jac[19]);
  
  double a51 = Jac[20]*V[10];   
  double a52 = Jac[20]*V[11];   
  double a53 = Jac[20]*V[12];   
  double a54 = Jac[20]*V[13];   
  double a55 = Jac[20]*V[14];   

  m_covariance[10] = (a51*Jac[ 0]+a52*Jac[ 1]+a53*Jac[ 2])+(a54*Jac[ 3]+a55*Jac[ 4]);
  m_covariance[11] = (a51*Jac[ 5]+a52*Jac[ 6]+a53*Jac[ 7])+(a54*Jac[ 8]+a55*Jac[ 9]);
  m_covariance[12] = (a51*Jac[10]+a52*Jac[11]+a53*Jac[12])+(a54*Jac[13]+a55*Jac[14]);
  m_covariance[13] = (a51*Jac[15]+a52*Jac[16]+a53*Jac[17])+(a54*Jac[18]+a55*Jac[19]);
  m_covariance[14] =                                                    a55*Jac[20];
  m_iscovariance   = true;
}

///////////////////////////////////////////////////////////////////
// Global position of simple track parameters
///////////////////////////////////////////////////////////////////

Amg::Vector3D Trk::PatternTrackParameters::position() const
{
  
  Amg::Vector3D gp(0.,0.,0.);
  if(!m_surface) return gp;

  const Trk::PlaneSurface       * plane  ;
  if((plane   = dynamic_cast<const Trk::PlaneSurface*>       (m_surface))) {
    return localToGlobal(plane   );
  }
  
  const Trk::StraightLineSurface* line   ; 
  if((line    = dynamic_cast<const Trk::StraightLineSurface*>(m_surface))) {
    return localToGlobal(line    );
  }

  const Trk::DiscSurface        * disc   ;
  if((disc    = dynamic_cast<const Trk::DiscSurface*>        (m_surface))) {
    return localToGlobal(disc    );
  }

  const Trk::CylinderSurface    * cylinder;
  if((cylinder= dynamic_cast<const Trk::CylinderSurface*>    (m_surface))) {
    return localToGlobal(cylinder);
  }

  const Trk::PerigeeSurface     * pline   ;
  if((pline   = dynamic_cast<const Trk::PerigeeSurface*>     (m_surface))) {
    return localToGlobal(pline   );
  }

  const Trk::ConeSurface        *  cone   ;
  if((cone    = dynamic_cast<const Trk::ConeSurface*>        (m_surface))) {
    return localToGlobal(cone   );
  }

  return gp;
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
  const Trk::Surface*  s = m_surface; 
  const double* P        = &m_parameters[0];
  const double* V        = &m_covariance[0];
  std::string name;
  std::string iscov;

  boost::io::ios_all_saver ias(out);

  const Trk::DiscSurface* di;
  const Trk::ConeSurface* cn;
  const Trk::PlaneSurface* pl;
  const Trk::PerigeeSurface* pe;
  const Trk::CylinderSurface* cy;
  const Trk::StraightLineSurface* li;
  
  if     ((pl=dynamic_cast<const Trk::PlaneSurface*>       (s))) name = "Plane"   ;
  else if((li=dynamic_cast<const Trk::StraightLineSurface*>(s))) name = "Line"    ;
  else if((di=dynamic_cast<const Trk::DiscSurface*>        (s))) name = "Disc"    ;
  else if((cy=dynamic_cast<const Trk::CylinderSurface*>    (s))) name = "Cylinder";
  else if((pe=dynamic_cast<const Trk::PerigeeSurface*>     (s))) name = "Perigee" ;
  else if((cn=dynamic_cast<const Trk::ConeSurface*>        (s))) name = "Cone"    ;
  else  {
    out << "Track parameters are not valid " << std::endl;
    ias.restore();
    return out;
  }

  out << "Track parameters for " << name << " surface " << std::endl;
   
  out.unsetf(std::ios::fixed);
  out.setf  (std::ios::showpos);
  out.setf  (std::ios::scientific);

  if (m_iscovariance) {
    out << std::setprecision(4) <<
      P[ 0]<<" |"<<V[ 0] << std::endl;
    out << std::setprecision(4) <<
      P[ 1]<<" |"<<V[ 1]<<" "<<V[ 2] << std::endl;
    out << std::setprecision(4) <<
      P[ 2]<<" |"<<V[ 3]<<" "<<V[ 4]<<" "<<V[ 5] << std::endl;
    out << std::setprecision(4) <<
      P[ 3]<<" |"<<V[ 6]<<" "<<V[ 7]<<" "<<V[ 8]<<" "<<V[ 9] << std::endl;
    out << std::setprecision(4) <<
      P[ 4]<<" |"<<V[10]<<" "<<V[11]<<" "<<V[12]<<" "<<V[13]<<" "<<V[14] << std::endl;
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
  const Trk::Surface*  s = m_surface; 
  const double* P        = &m_parameters[0];
  const double* V        = &m_covariance[0];
  std::string name;
  std::string iscov;

  const Trk::DiscSurface* di;
  const Trk::ConeSurface* cn;
  const Trk::PlaneSurface* pl;
  const Trk::PerigeeSurface* pe;
  const Trk::CylinderSurface* cy;
  const Trk::StraightLineSurface* li;

  if     ((pl=dynamic_cast<const Trk::PlaneSurface*>       (s))) name = "Plane"   ;
  else if((li=dynamic_cast<const Trk::StraightLineSurface*>(s))) name = "Line"    ;
  else if((di=dynamic_cast<const Trk::DiscSurface*>        (s))) name = "Disc"    ;
  else if((cy=dynamic_cast<const Trk::CylinderSurface*>    (s))) name = "Cylinder";
  else if((pe=dynamic_cast<const Trk::PerigeeSurface*>     (s))) name = "Perigee" ;
  else if((cn=dynamic_cast<const Trk::ConeSurface*>        (s))) name = "Cone"    ;
  else  {
    out << "Track parameters are not valid " << std::endl;
    return out;
  }

  out << "Track parameters for " << name << " surface " << std::endl;
  
  out.unsetf(std::ios::fixed);
  out.setf  (std::ios::showpos);
  out.setf  (std::ios::scientific);

  if (m_iscovariance) {
    out << std::setprecision(4) <<
      P[ 0]<<" |"<<V[ 0] << std::endl;
    out << std::setprecision(4) <<
      P[ 1]<<" |"<<V[ 1]<<" "<<V[ 2] << std::endl;
    out << std::setprecision(4) <<
      P[ 2]<<" |"<<V[ 3]<<" "<<V[ 4]<<" "<<V[ 5] << std::endl;
    out << std::setprecision(4) <<
      P[ 3]<<" |"<<V[ 6]<<" "<<V[ 7]<<" "<<V[ 8]<<" "<<V[ 9] << std::endl;
    out << std::setprecision(4) <<
      P[ 4]<<" |"<<V[10]<<" "<<V[11]<<" "<<V[12]<<" "<<V[13]<<" "<<V[14] << std::endl;
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

  double Sf,Cf; sincos(m_parameters[2],&Sf,&Cf);
  double Se,Ce; sincos(m_parameters[3],&Se,&Ce);

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

  double Sf,Cf; sincos(m_parameters[1],&Sf,&Cf);

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

  double Sf,Cf; sincos(fr,&Sf,&Cf);

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

  double Sf,Cf; sincos(m_parameters[2],&Sf,&Cf);
  double Se,Ce; sincos(m_parameters[3],&Se,&Ce);

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
  double Sf,Cf; sincos((m_parameters[0]/r),&Sf,&Cf);
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
  
  int n = E.rows(); if(n<=0 || n>2) return false;

  m_parameters[0] = P  (0); 
  m_covariance[0] = E(0,0); 
  
  if(n==2) {
    m_parameters[ 1] = P(1);
    m_covariance[ 1] = E(1,0);
    m_covariance[ 2] = E(1,1);
  }
  else    {
    m_parameters[ 1] = Tp.m_parameters[ 1];
    m_covariance[ 1] = Tp.m_covariance[ 1];
    m_covariance[ 2] = Tp.m_covariance[ 2];
  }
  m_parameters[ 2] = Tp.m_parameters[ 2];
  m_parameters[ 3] = Tp.m_parameters[ 3];
  m_parameters[ 4] = Tp.m_parameters[ 4];
  
  m_covariance[ 3] = Tp.m_covariance[ 3];
  m_covariance[ 4] = Tp.m_covariance[ 4];
  m_covariance[ 5] = Tp.m_covariance[ 5];
  m_covariance[ 6] = Tp.m_covariance[ 6];
  m_covariance[ 7] = Tp.m_covariance[ 7];
  m_covariance[ 8] = Tp.m_covariance[ 8];
  m_covariance[ 9] = Tp.m_covariance[ 9];
  m_covariance[10] = Tp.m_covariance[10];
  m_covariance[11] = Tp.m_covariance[11];
  m_covariance[12] = Tp.m_covariance[12];
  m_covariance[13] = Tp.m_covariance[13];
  m_covariance[14] = Tp.m_covariance[14];
  
  m_surface        = Tp.m_surface;  
  m_iscovariance   = true        ;
  
  return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Change direction of the parameters
///////////////////////////////////////////////////////////////////
      
void Trk::PatternTrackParameters::changeDirection()
{
  const double pi = M_PI, pi2 = 2.*M_PI; //NB CLHEP also defines pi and pi2 constants.

  m_parameters[ 2] =  m_parameters[2]-pi; 
  m_parameters[ 3] =  pi-m_parameters[3];
  m_parameters[ 4] = -m_parameters[4]   ;

  if( m_parameters[2] < -pi) m_parameters[2]+=pi2;


  if(!dynamic_cast<const Trk::StraightLineSurface*>(m_surface) &&
     !dynamic_cast<const Trk::PerigeeSurface*>     (m_surface)) {
    
    if(!m_iscovariance) return;

    m_covariance[ 6] = -m_covariance[ 6];
    m_covariance[ 7] = -m_covariance[ 7];
    m_covariance[ 8] = -m_covariance[ 8];
    m_covariance[10] = -m_covariance[10];
    m_covariance[11] = -m_covariance[11];
    m_covariance[12] = -m_covariance[12];
    return;
  }

  m_parameters[ 0] = -m_parameters[ 0];

  if(!m_iscovariance) return;

  m_covariance[ 1] = -m_covariance[ 1];
  m_covariance[ 3] = -m_covariance[ 3];
  m_covariance[ 7] = -m_covariance[ 7];
  m_covariance[ 8] = -m_covariance[ 8];
  m_covariance[11] = -m_covariance[11];
  m_covariance[12] = -m_covariance[12];
}

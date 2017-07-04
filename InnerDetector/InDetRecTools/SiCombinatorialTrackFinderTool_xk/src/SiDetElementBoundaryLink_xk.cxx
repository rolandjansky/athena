/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::SiDetElementBoundaryLink_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "TrkSurfaces/PlaneSurface.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SiCombinatorialTrackFinderTool_xk/SiDetElementBoundaryLink_xk.h"
#include "TrkSurfaces/AnnulusBounds.h" 

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiDetElementBoundaryLink_xk::SiDetElementBoundaryLink_xk
( const InDetDD::SiDetectorElement*& Si)
{
  m_detelement = 0 ;
  m_dR         = 0.;
  const Trk::PlaneSurface* pla = dynamic_cast<const Trk::PlaneSurface*>(& Si->surface());
  if(!pla) return;
  m_detelement = Si;

  double Sl    = .5*Si->design().length  ();
  double Swmax = .5*Si->design().maxWidth();
  double Swmin = .5*Si->design().minWidth();

  Amg::Vector3D  AF = Si->phiAxis();
  Amg::Vector3D  AE = Si->etaAxis();

  const Amg::Transform3D&  T = pla->transform();
  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};

  double x[4],y[4],z[4];

  x[0]     = AF.x()*Swmax+AE.x()*Sl;
  y[0]     = AF.y()*Swmax+AE.y()*Sl;
  z[0]     = AF.z()*Swmax+AE.z()*Sl;

  x[1]     = AF.x()*Swmin-AE.x()*Sl;
  y[1]     = AF.y()*Swmin-AE.y()*Sl;
  z[1]     = AF.z()*Swmin-AE.z()*Sl;

  x[2]     =-AF.x()*Swmin-AE.x()*Sl;
  y[2]     =-AF.y()*Swmin-AE.y()*Sl;
  z[2]     =-AF.z()*Swmin-AE.z()*Sl;

  x[3]     =-AF.x()*Swmax+AE.x()*Sl;
  y[3]     =-AF.y()*Swmax+AE.y()*Sl;
  z[3]     =-AF.z()*Swmax+AE.z()*Sl;

  const Trk::AnnulusBounds* B = dynamic_cast<const Trk::AnnulusBounds*>(&Si->design().bounds()); 
  if(!B) {
    for(int i=0; i!=4; ++i) {

      int    k1     = i  ;
      int    k2     = i+1; if(k2>3) k2=0;
      double x1     =  x[k1]*Ax[0]+y[k1]*Ax[1]+z[k1]*Ax[2]; 
      double y1     =  x[k1]*Ay[0]+y[k1]*Ay[1]+z[k1]*Ay[2]; 
      double x2     =  x[k2]*Ax[0]+y[k2]*Ax[1]+z[k2]*Ax[2]; 
      double y2     =  x[k2]*Ay[0]+y[k2]*Ay[1]+z[k2]*Ay[2]; 
      double d      = (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
      double ax     =-(y2-y1)*(y1*x2-x1*y2)/d;
      double ay     = (x2-x1)*(y1*x2-x1*y2)/d;
      m_bound[i][2] = sqrt(ax*ax+ay*ay);
      m_bound[i][1] = ay/m_bound[i][2];
      m_bound[i][0] = ax/m_bound[i][2];
    }
    return;
  }

  m_dR = B->R();

  std::pair<Amg::Vector3D,Amg::Vector3D> strip0(Si->endsOfStrip(InDetDD::SiLocalPosition( Sl+m_dR, Swmax+10.,0))); 
  Amg::Vector3D a0(strip0.first ); 
  Amg::Vector3D b0(strip0.second);  
  double x0 = a0[0]*Ax[0]+a0[1]*Ax[1];
  double y0 = a0[0]*Ay[0]+a0[1]*Ay[1];
  double x1 = b0[0]*Ax[0]+b0[1]*Ax[1];
  double y1 = b0[0]*Ay[0]+b0[1]*Ay[1];
  if(y1 > y0) {x[0] = x1; y[0] = y1;} else  {x[0] = x0; y[0] = y0;}

  std::pair<Amg::Vector3D,Amg::Vector3D> strip1(Si->endsOfStrip(InDetDD::SiLocalPosition(-Sl+m_dR, Swmin+10.,0))); 
  Amg::Vector3D a1(strip1.first ); 
  Amg::Vector3D b1(strip1.second);  
  x0        = a1[0]*Ax[0]+a1[1]*Ax[1];
  y0        = a1[0]*Ay[0]+a1[1]*Ay[1];
  x1        = b1[0]*Ax[0]+b1[1]*Ax[1];
  y1        = b1[0]*Ay[0]+b1[1]*Ay[1];
  if(y1 < y0) {x[1] = x1; y[1] = y1;} else  {x[1] = x0; y[1] = y0;}

  std::pair<Amg::Vector3D,Amg::Vector3D> strip2(Si->endsOfStrip(InDetDD::SiLocalPosition(-Sl+m_dR,-Swmin-10.,0))); 
  Amg::Vector3D a2(strip2.first ); 
  Amg::Vector3D b2(strip2.second);  
  x0        = a2[0]*Ax[0]+a2[1]*Ax[1];
  y0        = a2[0]*Ay[0]+a2[1]*Ay[1];
  x1        = b2[0]*Ax[0]+b2[1]*Ax[1];
  y1        = b2[0]*Ay[0]+b2[1]*Ay[1];
  if(y1 < y0) {x[2] = x1; y[2] = y1;} else  {x[2] = x0; y[2] = y0;}

  std::pair<Amg::Vector3D,Amg::Vector3D> strip3(Si->endsOfStrip(InDetDD::SiLocalPosition( Sl+m_dR,-Swmax-10.,0))); 
  Amg::Vector3D a3(strip3.first ); 
  Amg::Vector3D b3(strip3.second);  
  x0        = a3[0]*Ax[0]+a3[1]*Ax[1];
  y0        = a3[0]*Ay[0]+a3[1]*Ay[1];
  x1        = b3[0]*Ax[0]+b3[1]*Ax[1];
  y1        = b3[0]*Ay[0]+b3[1]*Ay[1];
  if(y1 > y0) {x[3] = x1; y[3] = y1;} else  {x[3] = x0; y[3] = y0;}

  y[0]-=m_dR;
  y[1]-=m_dR;
  y[2]-=m_dR;
  y[3]-=m_dR;

  for(int i=0; i!=4; ++i) {
    
    int    k1     = i  ;
    int    k2     = i+1; if(k2>3) k2=0;
    double x1     =  x[k1];
    double y1     =  y[k1];
    double x2     =  x[k2];
    double y2     =  y[k2];
    double d      = (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
    double ax     =-(y2-y1)*(y1*x2-x1*y2)/d;
    double ay     = (x2-x1)*(y1*x2-x1*y2)/d;
    m_bound[i][2] = sqrt(ax*ax+ay*ay);
    m_bound[i][1] = ay/m_bound[i][2];
    m_bound[i][0] = ax/m_bound[i][2];
  }
}

///////////////////////////////////////////////////////////////////
// Detector element intersection test
///////////////////////////////////////////////////////////////////

int InDet::SiDetElementBoundaryLink_xk::intersect(const Trk::PatternTrackParameters& Tp,double& a) const
{
  double x  = Tp.par()[0];
  double y  = Tp.par()[1]-m_dR;
  int    n  = 0;
  a         = m_bound[0][0]*x+m_bound[0][1]*y-m_bound[0][2]; 
  double a2 = m_bound[1][0]*x+m_bound[1][1]*y-m_bound[1][2]; if(a2>a) {a=a2; n=1;}
  double a3 = m_bound[2][0]*x+m_bound[2][1]*y-m_bound[2][2]; if(a3>a) {a=a3; n=2;}
  double a4 = m_bound[3][0]*x+m_bound[3][1]*y-m_bound[3][2]; if(a4>a) {a=a4; n=3;}

  if(a > 20. ) return 1;

  double D  = (m_bound[n][0]*m_bound[n][0]* Tp.cov()[0]+
	       m_bound[n][1]*m_bound[n][1]* Tp.cov()[2]+
	       m_bound[n][0]*m_bound[n][1]*(Tp.cov()[1]*2.))*100.;

  if((a*a) <= D) return 0;

  if(a >  3.) return 1;
  if(a < -3.) return -1; return 0;
}


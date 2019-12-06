/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
( const InDetDD::SiDetectorElement*& Si, bool isITk)
{
  m_ITkGeometry = isITk;
  m_detelement = 0;
  m_dR = 0.;
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

if (not B) {
  
  for(int i=0; i!=4; ++i) {
    int    k1    = i;
    int    k2    = i+1; if(k2>3) k2=0;
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
    m_bound00[i]  = m_bound[i][0]*m_bound[i][0]*100.;
    m_bound01[i]  = m_bound[i][0]*m_bound[i][1]*200.;
    m_bound11[i]  = m_bound[i][1]*m_bound[i][1]*100.;
  }
  
  return;    

} else {
  
  m_dR = pla->center().perp();
  
  std::vector < std::pair < double, double > > corners = B->corners();
  
  x[0] =  corners.at(0).first;
  x[1] =  corners.at(1).first;
  x[2] =  corners.at(2).first;
  x[3] =  corners.at(3).first;
  
  y[0]=corners.at(0).second - m_dR;
  y[1]=corners.at(1).second - m_dR;
  y[2]=corners.at(2).second - m_dR;
  y[3]=corners.at(3).second - m_dR;
  
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
    m_bound00[i]  = m_bound[i][0]*m_bound[i][0]*100.;
    m_bound01[i]  = m_bound[i][0]*m_bound[i][1]*200.;
    m_bound11[i]  = m_bound[i][1]*m_bound[i][1]*100.;
  }
  return;
}
return;

}

///////////////////////////////////////////////////////////////////
// Detector element intersection test
///////////////////////////////////////////////////////////////////

int InDet::SiDetElementBoundaryLink_xk::intersect(const Trk::PatternTrackParameters& Tp,double& a) const
{
  double x = Tp.par()[0];
  double y = Tp.par()[1]-m_dR;

  double b[4] = {m_bound[0][0]*x+m_bound[0][1]*y-m_bound[0][2], 
		 m_bound[1][0]*x+m_bound[1][1]*y-m_bound[1][2], 
		 m_bound[2][0]*x+m_bound[2][1]*y-m_bound[2][2], 
		 m_bound[3][0]*x+m_bound[3][1]*y-m_bound[3][2]};
 
  int n; b[1] > b[0] ? n = 1 : n = 0; 
  int m; b[3] > b[2] ? m = 3 : m = 2;
  if(b[m] > b[n]) n = m;

  a  = b[n];
      
  if (not m_ITkGeometry) {
    if(a > 20. ) return 1;
    double D  = m_bound00[n]*Tp.cov()[0]+m_bound11[n]*Tp.cov()[2]+m_bound01[n]*Tp.cov()[1];

    if((a*a) <= D) return 0;

    if(a >  2.) return 1;
    if(a < -2.) {
      if(!m_detelement->nearBondGap(Tp.localPosition(), 3.*sqrt(Tp.cov()[2]))) return -1;
    }
    return 0;

  } else {
    if(     a > 20. ) return 1;
    if(fabs(a) <=3. ) return 0;

    double D  = m_bound00[n]*Tp.cov()[0]+m_bound11[n]*Tp.cov()[2]+m_bound01[n]*Tp.cov()[1];

    if     ((a*a) <= D) return  0;
    else if(   a >  0.) return  1;
    else                return -1; 
  }
  return 0;
}

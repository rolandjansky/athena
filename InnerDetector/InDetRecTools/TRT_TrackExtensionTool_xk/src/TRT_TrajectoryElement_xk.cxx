/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TRT_TrackExtensionTool_xk/TRT_TrajectoryElement_xk.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"

///////////////////////////////////////////////////////////////////
// Set TRT helper and road width
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrajectoryElement_xk::set
(const TRT_ID                           *    m,
 const Trk::IPatternParametersPropagator*   pr, 
 const Trk::IPatternParametersUpdator   *   up, 
 Trk::IRIO_OnTrackCreator               * riod,
 Trk::IRIO_OnTrackCreator               * rion,
 double scale)
{
  m_trtid       = m               ;
  m_riomakerD   = riod            ;
  m_riomakerN   = rion            ;
  m_proptool    = pr              ;
  m_updatortool = up              ;
  m_scale_error = scale           ;
}

void InDet::TRT_TrajectoryElement_xk::set
(const Trk::MagneticFieldProperties &  mf,MagField::IMagFieldSvc*& ms)
{
  m_fieldprop    = mf;
  m_fieldService = ms;
}

///////////////////////////////////////////////////////////////////
// Set trajectory element for precision seed
///////////////////////////////////////////////////////////////////

bool InDet::TRT_TrajectoryElement_xk::initiateForPrecisionSeed
(bool st,const InDetDD::TRT_BaseElement*&          de,
 InDet::TRT_DriftCircleCollection::const_iterator& sb,
 InDet::TRT_DriftCircleCollection::const_iterator& se,
 std::pair<Amg::Vector3D,double>&                  gp,
 const double* dir,double width2)
{
  m_isCluster  = st;
  m_detelement = de;

  // Test boundary active region of detector elements
  //
  if(!boundaryTest(3.,gp)) return false;

  // Trajectory element links production
  //
  initiateLinksForPrecisionSeed(sb,se,gp,dir,width2);
  return true;
}

///////////////////////////////////////////////////////////////////
// Set trajectory element for TRT seed
///////////////////////////////////////////////////////////////////

bool InDet::TRT_TrajectoryElement_xk::initiateForTRTSeed
(bool st,const InDetDD::TRT_BaseElement*&          de,
 InDet::TRT_DriftCircleCollection::const_iterator& sb,
 InDet::TRT_DriftCircleCollection::const_iterator& se,
 std::pair<Amg::Vector3D,double>&            gp,
 const double* dir,double width2)
{

  m_isCluster  = st  ;
  m_detelement = de  ;

  // Test boundary active region of detector elements
  //
  bool boundary = boundaryTest(7.,gp);

  // Trajectory element links production
  //
  initiateLinksForTRTSeed(sb,se,gp,dir,width2);
  return boundary;
}

///////////////////////////////////////////////////////////////////
// Initiate trajectory element links to straws
///////////////////////////////////////////////////////////////////

bool InDet::TRT_TrajectoryElement_xk::boundaryTest
(double dw, std::pair<Amg::Vector3D,double>& gp)
{
  if(!m_detelement) return false;

  double x                = gp.first.x  ();
  double y                = gp.first.y  ();
  double z                = gp.first.z  ();

  const Amg::Vector3D&     C = m_detelement->center  ();

  // Test track position
  //
  const Trk::RectangleBounds* rb = 0;
  const Trk::DiscBounds*      db = 0;

  if     ((rb=dynamic_cast<const Trk::RectangleBounds*>(&m_detelement->bounds()))) {

    // Barrel
    //    
    m_barrel = true                   ;
    m_z      = z                      ;
    m_zMin   = C.z()-rb->halflengthY();
    m_zMax   = C.z()+rb->halflengthY();
    double d = fabs(z - C.z()); if( d > rb->halflengthY()+dw) return false;
  }
  else if((db=dynamic_cast<const Trk::DiscBounds*>     (&m_detelement->bounds()))) {
   
    // Endcap
    //    
    m_barrel    = false        ;
    m_radius    = sqrt(x*x+y*y);
    m_z         = z            ;
    m_radiusMin = db->rMin()   ;
    m_radiusMax = db->rMax()   ;

    double d = m_radius-m_radiusMin;  if(d < -dw) return false; 
    d        = m_radiusMax-m_radius;  if(d < -dw) return false;
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Initiate trajectory element links to straws for precision  seed
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrajectoryElement_xk::initiateLinksForPrecisionSeed 
(InDet::TRT_DriftCircleCollection::const_iterator& sb,
 InDet::TRT_DriftCircleCollection::const_iterator& se,
 std::pair<Amg::Vector3D,double>&                  gp,
 const double* dir,double width2)
{
  m_status     =-1           ;
  m_bestlink   =-1           ;
  m_nlinks     = 0           ;
  double x     = gp.first.x();
  double y     = gp.first.y();
  double z     = gp.first.z();

  const Amg::Transform3D& T = m_detelement->surface().transform();

  double step = fabs(dir[0]*T(0,2)+dir[1]*T(1,2)+dir[2]*T(2,2));
  step  > .05 ? step = 1./step : step = 20.;

  int Nstraws = 0;
  const InDetDD::TRT_BarrelElement* be = dynamic_cast<const InDetDD::TRT_BarrelElement*>(m_detelement);
  if(be) Nstraws = be->nStraws();
  else   {
    const InDetDD::TRT_EndcapElement* ne = dynamic_cast<const InDetDD::TRT_EndcapElement*>(m_detelement);
    if(ne) Nstraws = ne->nStraws();
  }

  if(be) {

    for(int ns=0; ns!=Nstraws; ++ns) {

      const Amg::Transform3D& t = m_detelement->strawTransform(ns);

      double xs    = t(0,3);
      double ys    = t(1,3);
      double dx    = x-xs  ;
      double dy    = y-ys  ;
      double d     = dx*dx+dy*dy;
      if(d > width2) continue;

      double Az[3] = {t(0,2),t(1,2),t(2,2)};
      double D     = dir[0]*Az[0]+dir[1]*Az[1]+dir[2]*Az[2];
      double A     = (1.-D)*(1.+D);
      double dz    = z-t(2,3);
      double S     = (dx*(D*Az[0]-dir[0])+dy*(D*Az[1]-dir[1])+dz*(D*Az[2]-dir[2]))/A;
      dx          +=(dir[0]*S);
      dy          +=(dir[1]*S);
      dz          +=(dir[2]*S);
      double Bx    = Az[1]*dir[2]-Az[2]*dir[1];
      double By    = Az[2]*dir[0]-Az[0]*dir[2];
      double Bz    = Az[0]*dir[1]-Az[1]*dir[0];
      double im    =(dx*Bx+dy*By+dz*Bz)/sqrt(Bx*Bx+By*By+Bz*Bz);
      double zl    = dx*Az[0]+dy*Az[1]+dz*Az[2];
      S           +=gp.second; 
      d            = fabs(im); if(y*xs-x*ys > 0.) d=-d;
      m_link[m_nlinks].set(ns,d,im,zl,S); 
      if(++m_nlinks==24) break;
    }
  }
  else {

    for(int ns=0; ns!=Nstraws; ++ns) {

      const Amg::Transform3D& t = m_detelement->strawTransform(ns);

      double xs    = t(0,3);
      double ys    = t(1,3);
      double as    = m_radius/sqrt(xs*xs+ys*ys);
      double dx    = x-xs*as;
      double dy    = y-ys*as;
      double d     = dx*dx+dy*dy;
      if(d > width2) continue;

      double zs    = t(2,3);
      double Az[3] = {t(0,2),t(1,2),t(2,2)};
      double D     = dir[0]*Az[0]+dir[1]*Az[1]+dir[2]*Az[2];
      double A     = (1.-D)*(1.+D);
      dx           = x-xs;
      dy           = y-ys;
      double dz    = z-zs;
      double S     =(dx*(D*Az[0]-dir[0])+dy*(D*Az[1]-dir[1])+dz*(D*Az[2]-dir[2]))/A;
      dx          +=(dir[0]*S);
      dy          +=(dir[1]*S);
      dz          +=(dir[2]*S);
      double Bx    = Az[1]*dir[2]-Az[2]*dir[1];
      double By    = Az[2]*dir[0]-Az[0]*dir[2];
      double Bz    = Az[0]*dir[1]-Az[1]*dir[0];
      double im    =(dx*Bx+dy*By+dz*Bz)/sqrt(Bx*Bx+By*By+Bz*Bz);
      double zl    = dx*Az[0]+dy*Az[1]+dz*Az[2];
      S           +=gp.second; 
      d            = fabs(im); if(y*xs-x*ys > 0.) d=-d;
      m_link[m_nlinks].set(ns,d,im,zl,S); 
      if(++m_nlinks==24) break;
    }
  }
  m_dpositive = 1000.;
  m_dnegative =-1000.;
  if(m_isCluster && m_nlinks) {

    bool nl = false; 
    for(; sb!=se; ++sb) {

      int  ns = m_trtid->straw((*sb)->identify());

      for(int l=0; l!=m_nlinks; ++l) {
	if(ns!=m_link[l].number()) continue;
	nl=true; m_link[l].set((*sb));
 
	double d = m_link[l].distance();
	if(d >= 0.) {if(d < m_dpositive) m_dpositive = d;}
	else        {if(d > m_dnegative) m_dnegative = d;}
	break;
      }
    }
    if(!nl) m_isCluster = false;
  }
}

///////////////////////////////////////////////////////////////////
// Initiate trajectory element links to straws for TRT seed
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrajectoryElement_xk::initiateLinksForTRTSeed 
(InDet::TRT_DriftCircleCollection::const_iterator& sb,
 InDet::TRT_DriftCircleCollection::const_iterator& se,
 std::pair<Amg::Vector3D,double>&                  gp,
 const double* dir,double width2)
{
  m_status     =-1           ;
  m_bestlink   =-1           ;
  m_nlinks     = 0           ;
  double x     = gp.first.x();
  double y     = gp.first.y();
  double z     = gp.first.z();

  const Amg::Transform3D& T = m_detelement->surface().transform();

  double step = fabs(dir[0]*T(0,2)+dir[1]*T(1,2)+dir[2]*T(2,2));
  step  > .05 ? step = 1./step : step = 20.;

  int Nstraws = 0;
  const InDetDD::TRT_BarrelElement* be = dynamic_cast<const InDetDD::TRT_BarrelElement*>(m_detelement);
  if(be) Nstraws = be->nStraws();
  else   {
    const InDetDD::TRT_EndcapElement* en = dynamic_cast<const InDetDD::TRT_EndcapElement*>(m_detelement);
    if(en) Nstraws = en->nStraws();
  }
  if(be) {

    for(int ns=0; ns!=Nstraws; ++ns) {

      const Amg::Transform3D& t = m_detelement->strawTransform(ns);

      double xs    = t(0,3);
      double ys    = t(1,3);
      double dx    = x-xs  ;
      double dy    = y-ys  ;
      double d     = dx*dx+dy*dy;
      if(d > width2) continue;

      double Az[3] = {t(0,2),t(1,2),t(2,2)};
      double D     = dir[0]*Az[0]+dir[1]*Az[1]+dir[2]*Az[2];
      double A     = (1.-D)*(1.+D);
      double dz    = z-t(2,3);
      double S     = (dx*(D*Az[0]-dir[0])+dy*(D*Az[1]-dir[1])+dz*(D*Az[2]-dir[2]))/A;
      dx          +=(dir[0]*S);
      dy          +=(dir[1]*S);
      dz          +=(dir[2]*S);
      double Bx    = Az[1]*dir[2]-Az[2]*dir[1];
      double By    = Az[2]*dir[0]-Az[0]*dir[2];
      double Bz    = Az[0]*dir[1]-Az[1]*dir[0];
      double im    =(dx*Bx+dy*By+dz*Bz)/sqrt(Bx*Bx+By*By+Bz*Bz);
      double zl    = dx*Az[0]+dy*Az[1]+dz*Az[2];
      S           +=gp.second; 
      d            = fabs(im); if(y*xs-x*ys > 0.) d=-d;
      m_link[m_nlinks].set(ns,d,im,zl,S); 
      if(++m_nlinks==24) break;
    }
  }
  else {

    double ri = 1./m_radius;
    double ax = x*ri;
    double ay = y*ri;

    for(int ns=0; ns!=Nstraws; ++ns) {

      const Amg::Transform3D& t = m_detelement->strawTransform(ns);

      double xs    = t(0,3);
      double ys    = t(1,3);
      double as    = m_radius/sqrt(xs*xs+ys*ys);
      double dx    = x-xs*as;
      double dy    = y-ys*as;
      double d     = dx*dx+dy*dy;
      if(d > width2) continue;

      double zs    = t(2,3);
      double Az[3] = {t(0,2),t(1,2),t(2,2)};
      double D     = dir[0]*Az[0]+dir[1]*Az[1]+dir[2]*Az[2];
      double A     = 1./((1.-D)*(1.+D));
      dx           = x-xs;
      dy           = y-ys;
      double dz    = z-zs;
      double Dx    = (D*Az[0]-dir[0]);
      double Dy    = (D*Az[1]-dir[1]);
      double S     =(dx*Dx+dy*Dy+dz*(D*Az[2]-dir[2]))*A;
      dx          +=(dir[0]*S);
      dy          +=(dir[1]*S);
      dz          +=(dir[2]*S);
      double Bx    = Az[1]*dir[2]-Az[2]*dir[1];
      double By    = Az[2]*dir[0]-Az[0]*dir[2];
      double Bz    = Az[0]*dir[1]-Az[1]*dir[0];
      double B     = 1./sqrt(Bx*Bx+By*By+Bz*Bz);
      double im    = (dx*Bx+dy*By+dz*Bz)*B;
      double zl    = dx*Az[0]+dy*Az[1]+dz*Az[2];

      // d(im)/dr calculation for endcap
      //
      double dS    =(ax*Dx+ay*Dy)*A   ;
      double sx    = ax+dir[0]*dS     ;
      double sy    = ay+dir[1]*dS     ;
      double sd    =(sx*Bx+sy*By)*B   ;
      double sz    = sx*Az[0]+sy*Az[1];
      S           +=gp.second; 
      if( y*xs-x*ys > 0.) {d =-fabs(im); sd =-fabs(im+sd)-d;}
      else                {d = fabs(im); sd = fabs(im+sd)-d;}

      m_link[m_nlinks].set(ns,d,im,zl,S,sd,sz);
      if(++m_nlinks==24) break;
    }
  }

  m_dpositive = 1000.;
  m_dnegative =-1000.;
  if(m_isCluster && m_nlinks) {

    bool nl = false; 
    for(; sb!=se; ++sb) {

      int  ns = m_trtid->straw((*sb)->identify());

      for(int l=0; l!=m_nlinks; ++l) {
	if(ns!=m_link[l].number()) continue;
	nl=true; m_link[l].set((*sb)); 

	double d = m_link[l].distance();
	if(d >= 0.) {if(d < m_dpositive) m_dpositive = d;}
	else        {if(d > m_dnegative) m_dnegative = d;}
	break;
      }
    }
    if(!nl) m_isCluster = false;
  }
}

///////////////////////////////////////////////////////////////////
// Build trajectory element without propagation for precision seed
///////////////////////////////////////////////////////////////////

bool InDet::TRT_TrajectoryElement_xk::buildForPrecisionSeed
(double a,double b,bool& useDriftTime, bool& hole)
{

  m_bestlink = -1;
  m_status   = -1;
  hole = false; if(!m_nlinks) return false;

  for(int l=0; l!=m_nlinks; ++l) {

    double v  = (a*m_link[l].way()+b)*m_link[l].way();
    double d  = m_link[l].distance()-v;
    double ad = fabs(d);

    if(ad > 2.05) continue;

    if(!m_link[l].cluster()) {

      if(ad < 1.85) {
	m_status = 0; hole=true; m_bestlink=l; m_link[l].newImpactParameter(d);
      }
      return false;
    }

    m_status  = 1; m_bestlink=l; m_link[l].newImpactParameter(d);
    if(!useDriftTime) return true;

    double r = m_link[l].cluster()->localPosition()[Trk::driftRadius];
    double e = m_scale_error*sqrt(m_link[l].cluster()->localCovariance()(0,0));

    if(r > 2.05) r = 2.05; 
    double r2 = r+e; if(r2 > 2.05) r2 = 2.05;
    double r1 = r-e; if(r1 >= r2) r1 = r2-2.*e;

    if(ad < r1 || ad > r2) useDriftTime = false; else m_status = 2; 

    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Build trajectory element without propagation for TRT seed
// without radius correction
///////////////////////////////////////////////////////////////////

bool InDet::TRT_TrajectoryElement_xk::buildForTRTSeed
(double a,double b,bool& useDriftTime, bool& hole)
{

  m_status   = -1;
  hole = false; if( m_bestlink < 0) return false;
  int l = m_bestlink;

  double v  = (a*m_link[l].way()+b)*m_link[l].way();
  double d  = m_link[l].distance()-v;
  double ad = fabs(d);
  
  if(ad > 2.05) return false;

  if(!m_link[l].cluster()) {

    if(ad < 1.85) {
      m_status = 0; hole=true; m_bestlink=l; m_link[l].newImpactParameter(d);
    }
    return false;
  }
  m_status  = 1; m_link[l].newImpactParameter(d);
  if(!useDriftTime) return true;
  
  double r = m_link[l].cluster()->localPosition()[Trk::driftRadius];
  double e = m_scale_error*sqrt(m_link[l].cluster()->localCovariance()(0,0));
  
  if(r > 2.05) r = 2.05; 
  double r2 = r+e; if(r2 > 2.05) r2 = 2.05;
  double r1 = r-e; if(r1 >= r2) r1 = r2-2.*e;
  if(ad < r1 || ad > r2) useDriftTime = false; else m_status = 2; 
  return true;
}

///////////////////////////////////////////////////////////////////
// Find close linek
///////////////////////////////////////////////////////////////////

double InDet::TRT_TrajectoryElement_xk::findCloseLink
(double a,double b)
{
  m_bestlink =    -1; 
  double dm = 10000.;

  for(int l=0; l < m_nlinks; ++l) {

    double v  = (a*m_link[l].way()+b)*m_link[l].way();
    double d  = fabs(m_link[l].distance()-v);

    if(d < dm) {dm=d;  m_bestlink=l;} 
  }
  return dm;
}

///////////////////////////////////////////////////////////////////
// Build RIOnTrack
///////////////////////////////////////////////////////////////////

const Trk::RIO_OnTrack* InDet::TRT_TrajectoryElement_xk::rioOnTrack()
{
  if(m_bestlink < 0 || m_status<=0) return 0; 

  int l = m_bestlink;

  const Trk::StraightLineSurface* line = (const Trk::StraightLineSurface*)
    (&(m_link[l].cluster()->detectorElement())->surface(m_link[l].cluster()->identify()));
  Trk::AtaStraightLine Tp(m_link[l].impact(),m_link[l].zlocal(),1.,1.,1.,*line);
  if(m_status==2)  return m_riomakerD->correct(*m_link[l].cluster(),Tp);
  else             return m_riomakerN->correct(*m_link[l].cluster(),Tp);
}

///////////////////////////////////////////////////////////////////
// Build simple RIOnTrack
///////////////////////////////////////////////////////////////////

const Trk::RIO_OnTrack* InDet::TRT_TrajectoryElement_xk::rioOnTrackSimple()
{
  if(m_bestlink < 0 || m_status<=0) return 0; 
  
  int l = m_bestlink;
  Amg::Vector3D dir(1.,0.,0.);
  Trk::DefinedParameter radius(0.,Trk::locX);
  Amg::MatrixX cov(1,1);
  cov(0,0) = 1.;
  if(m_status==2) {
    return new InDet::TRT_DriftCircleOnTrack(m_link[l].cluster(),Trk::LocalParameters(radius),cov,0,0.,dir,Trk::DECIDED);
  }
  return new InDet::TRT_DriftCircleOnTrack(m_link[l].cluster(),Trk::LocalParameters(radius),cov,0,0.,dir,Trk::NODRIFTTIME);
}

///////////////////////////////////////////////////////////////////
// Propagate track parameters to closest drift circles
///////////////////////////////////////////////////////////////////

bool InDet::TRT_TrajectoryElement_xk::propagate
(Trk::PatternTrackParameters& Ta,Trk::PatternTrackParameters& Tb)
{
  if(m_bestlink<0) return false;

  return m_proptool->propagate
    (Ta,m_link[m_bestlink].surface(),Tb,Trk::anyDirection,m_fieldprop,Trk::pion);
}

///////////////////////////////////////////////////////////////////
// Add drift circles information to track parameters             
///////////////////////////////////////////////////////////////////

bool InDet::TRT_TrajectoryElement_xk::addCluster
(Trk::PatternTrackParameters& Ta,Trk::PatternTrackParameters& Tb,double& Xi2)
{

  if(m_status <=0) return false;

  int n;
  if(m_status==2) {

    const TRT_DriftCircle* DS = m_link[m_bestlink].cluster();
    double dr = DS->localPosition().x(); if(m_link[m_bestlink].impact() < 0.) dr=-dr;
    
    Trk::DefinedParameter radius(dr,Trk::locX);
    Trk::LocalParameters  lp(radius);

    Trk::PatternTrackParameters To = Ta;
    bool Q = m_updatortool->addToState(Ta,lp,DS->localCovariance(),Tb,Xi2,n);
    if(Q && Xi2 < 15.) return Q;
    m_status = 1; Ta = To;
  }

  Trk::DefinedParameter  radius(0.,Trk::locX);
  Trk::LocalParameters   lp(radius);

  AmgSymMatrix(1) cov; cov<<1.33333;
  return m_updatortool->addToState(Ta,lp,cov,Tb,Xi2,n);
}

///////////////////////////////////////////////////////////////////
// Global trajectory coordinates calculation            
///////////////////////////////////////////////////////////////////

bool InDet::TRT_TrajectoryElement_xk::trajectoryGlobalPosition
(Amg::Vector3D& G,double& WAY)
{
  int l = m_bestlink;
  if( l<0 ) return false;

  const Amg::Transform3D& t = m_detelement->strawTransform(m_link[l].number());

  double Az[3] = {t(0,2),t(1,2),t(2,2)};
  double Rc[3] = {t(0,3),t(1,3),t(2,3)};

  double Bx,By;

  if(fabs(Az[2]) > .7) {              // Barrel

    double Ri = 1./sqrt(Rc[0]*Rc[0]+Rc[1]*Rc[1]);
    Bx =-Az[2]*Rc[1]*Ri; By = Az[2]*Rc[0]*Ri;
  }
  else if(Rc[2] > 0. ) {             // Positive endcap

    Bx = Az[1];          By =-Az[0];
  } 
  else                 {             // Negative endcap

    Bx =-Az[1];          By = Az[0];
  }
  double zl = m_link[l].zlocal();
  double im = m_link[l].impact();
  G[0]      = zl*Az[0]+Bx*im+Rc[0];
  G[1]      = zl*Az[1]+By*im+Rc[1]; 
  G[2]      = zl*Az[2]      +Rc[2]; 
  WAY       = m_link[l].way()     ; 
  return true;
}

///////////////////////////////////////////////////////////////////
// Track parameters estimation on detector element
///////////////////////////////////////////////////////////////////

bool  InDet::TRT_TrajectoryElement_xk::trackParametersEstimation
(TRT_TrajectoryElement_xk* E1,TRT_TrajectoryElement_xk* E2,Trk::PatternTrackParameters& Tp,
 double ZvHW)
{
  
  if(!E1) return false;

  Amg::Vector3D Gp[3];  
  double     Wa[3];  

  if   (!    trajectoryGlobalPosition(Gp[0],Wa[0])) return false;
  if   (!E1->trajectoryGlobalPosition(Gp[1],Wa[1])) return false;
  
  int mode; Wa[0] > Wa[1] ? mode=0 : mode=1;

  if(E2) {
    if(!E2->trajectoryGlobalPosition(Gp[2],Wa[2])) return false;
  }
  else    {
    double dx = Gp[1][0]-Gp[0][0];
    double dy = Gp[1][1]-Gp[0][1];
    Gp[2][0]  = 0.;
    Gp[2][1]  = 0.;
    Gp[2][2]  = Gp[0][2]-(Gp[1][2]-Gp[0][2])*sqrt((Gp[0][0]*Gp[0][0]+Gp[0][1]*Gp[0][1])/(dx*dx+dy*dy));
    Wa[2]     = 0.;
    mode      = 2 ;
  }

  double x0 = Gp[0][0]               ;
  double y0 = Gp[0][1]               ;
  double x1 = Gp[1][0]-x0            ;
  double y1 = Gp[1][1]-y0            ; 
  double x2 = Gp[2][0]-x0            ;
  double y2 = Gp[2][1]-y0            ;
  double r1 = sqrt(x1*x1+y1*y1)      ;
  double u1 = 1./r1                  ;
  double r2 = 1./(x2*x2+y2*y2)       ;
  double a  = x1*u1                  ;
  double b  = y1*u1                  ;
  double u2 = (a*x2+b*y2)*r2         ;
  double v2 = (a*y2-b*x2)*r2         ;
  double A  = v2/(u2-u1)             ;
  double B  = 2.*(v2-A*u2)           ;
  double C  = B/sqrt(1.+A*A)         ;

  double f,TP[3];

  if(mode==0) {
    f = atan2(-b-a*A,-a+b*A); C=-C; polarAngleEstimation(E2,Gp[0],Gp[2],C,ZvHW,TP);
  }
  else if(mode==1) { 
    f = atan2( b+a*A, a-b*A);       polarAngleEstimation(E2,Gp[0],Gp[2],C,ZvHW,TP); 
  }  
  else             {
    f = atan2( b+a*A, a-b*A);       polarAngleEstimation(E1,Gp[0],Gp[1],C,ZvHW,TP);
  }

  // Track parameters estimation
  //
  double P[5] = {m_link[m_bestlink].impact(),m_link[m_bestlink].zlocal(),f,atan2(1.,TP[0]),.0001};
  
  double pos0[3]; pos0[0]=Gp[0][0]; pos0[1]=Gp[0][1]; pos0[2]=Gp[0][2];
  double pos1[3]; pos1[0]=Gp[1][0]; pos1[1]=Gp[1][1]; pos1[2]=Gp[1][2];
  double pos2[3]; pos2[0]=Gp[2][0]; pos2[1]=Gp[2][1]; pos2[2]=Gp[2][2];
  double H0  [3]; m_fieldService->getFieldZR(pos0,H0);
  double H1  [3]; m_fieldService->getFieldZR(pos1,H1);
  double H2  [3]; m_fieldService->getFieldZR(pos2,H2);
  double Hz = .333333*(H0[2]+H1[2]+H2[2]);

  // If magnetic field exist
  //
  double Cm = 1.         ;
  double T2 = TP[0]*TP[0];
  if(fabs(Hz)>1.e-9) {Cm = 1./(300.*Hz*sqrt(1.+T2)); P[4] = -C*Cm;}

  // Covariance of track parameters estimation
  //
  double wa; mode != 2 ? wa = 1./(Wa[0]-Wa[2]) : wa = 1./Wa[1];
  const double dS = 4.         ;

  double df       = dS*wa        ;
  double dp       = 8.*df*wa*Cm  ;
  double c0       = 4.           ;
  double c1       = TP[1]        ;
  double c2       = df*df*(1.+T2); 
  double c3       = TP[2]        ;
  double c4       = dp*dp        ;
  double V[15] ={c0,
		 0.,c1,
		 0.,0.,c2,
		 0.,0.,0.,c3,
		 0.,0.,0.,0.,c4};
  
  Tp.setParametersWithCovariance(&m_link[m_bestlink].surface(),P,V);
  return true;
}

///////////////////////////////////////////////////////////////////
// Track parameters estimation for perigee
///////////////////////////////////////////////////////////////////

bool  InDet::TRT_TrajectoryElement_xk::trackParametersEstimation
(TRT_TrajectoryElement_xk* E1,Trk::PatternTrackParameters& Tp,double ZvHW)
{
  
  if(!E1) return false;

  Amg::Vector3D Gp[3];  
  double     Wa[3];  
  if   (!    trajectoryGlobalPosition(Gp[1],Wa[1])) return false;
  if   (!E1->trajectoryGlobalPosition(Gp[2],Wa[2])) return false;

  double dx = Gp[2][0]-Gp[1][0];
  double dy = Gp[2][1]-Gp[1][1];
  Gp[0][0]  = 0.;
  Gp[0][1]  = 0.;
  Gp[0][2]  = Gp[1][2]-(Gp[2][2]-Gp[1][2])*sqrt((Gp[1][0]*Gp[1][0]+Gp[1][1]*Gp[1][1])/(dx*dx+dy*dy));
  Wa[0]     = 0.;

  double x0 = Gp[0][0]               ;
  double y0 = Gp[0][1]               ;
  double x1 = Gp[1][0]-x0            ;
  double y1 = Gp[1][1]-y0            ; 
  double x2 = Gp[2][0]-x0            ;
  double y2 = Gp[2][1]-y0            ;
  double r1 = sqrt(x1*x1+y1*y1)      ;
  double u1 = 1./r1                  ;
  double r2 = 1./(x2*x2+y2*y2)       ;
  double a  = x1*u1                  ;
  double b  = y1*u1                  ;
  double u2 = (a*x2+b*y2)*r2         ;
  double v2 = (a*y2-b*x2)*r2         ;
  double A  = v2/(u2-u1)             ;
  double B  = 2.*(v2-A*u2)           ;
  double C  = B/sqrt(1.+A*A)         ;

  double f,TP[3];

  f = atan2( b+a*A, a-b*A); polarAngleEstimation(E1,Gp[1],Gp[2],C,ZvHW,TP);

  // Track parameters estimation
  //
  double P[5] = {0.,Gp[0][2],f,atan2(1.,TP[0]),.0001};

  double pos0[3]; pos0[0]=Gp[0][0]; pos0[1]=Gp[0][1]; pos0[2]=Gp[0][2];
  double pos1[3]; pos1[0]=Gp[1][0]; pos1[1]=Gp[1][1]; pos1[2]=Gp[1][2];
  double pos2[3]; pos2[0]=Gp[2][0]; pos2[1]=Gp[2][1]; pos2[2]=Gp[2][2];
  double H0  [3]; m_fieldService->getFieldZR(pos0,H0);
  double H1  [3]; m_fieldService->getFieldZR(pos1,H1);
  double H2  [3]; m_fieldService->getFieldZR(pos2,H2);
  double Hz = .333333*(H0[2]+H1[2]+H2[2]);

  // If magnetic field exist
  //
  double Cm =          1.;
  double T2 = TP[0]*TP[0];
  if(fabs(Hz)>1.e-9) {Cm = 1./(300.*Hz*sqrt(1.+T2)); P[4] = -C*Cm;}

  // Covariance of track parameters estimation
  //
  double wa = 1./Wa[2];

  const double dS = 4.           ;
  double df       = dS*wa        ;
  double dp       = 8.*df*wa*Cm  ;
  double c0       = .1           ;
  double c1       = ZvHW*ZvHW*.1 ;
  double c2       = df*df*(1.+T2); 
  double c3       = TP[2]        ;
  double c4       = dp*dp        ;
  double V[15] ={c0,
		 0.,c1,
		 0.,0.,c2,
		 0.,0.,0.,c3,
		 0.,0.,0.,0.,c4};
  
  Tp.setParametersWithCovariance(0,P,V);
  return true;
}

///////////////////////////////////////////////////////////////////
// Polar angle estimation
//
// Tp[0] = dz/dr=1./tan(Theta)
// Tp[1] = cov(Z loc)
// Tp[2] = cov(Theta)
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrajectoryElement_xk::polarAngleEstimation
(TRT_TrajectoryElement_xk* E,Amg::Vector3D& G0,Amg::Vector3D& G1,double C,double VZ,double* Tp)
{
  double dx = G1[0]-G0[0]           ;
  double dy = G1[1]-G0[1]           ;
  double dz = G1[2]-G0[2]           ;
  double dr = sqrt(dx*dx+dy*dy)     ;
  double rc = dr*C                  ;
  Tp[0]     = dz/(dr*(1.+.04*rc*rc));

  double r0 = G0[0]*G0[0]+G0[1]*G0[1];
  double r1 = G1[0]*G1[0]+G1[1]*G1[1];

  if( r0 > r1) {

    Tp[0]  =-Tp[0];
    if     (  m_barrel )   { //---------------------------->BB

      double dZ = m_zMax-m_zMin              ;
      double dT = (dZ+2.*VZ)                 ;  
      Tp[1]     = dZ*dZ*.1                   ;
      Tp[2]     = dT*dT*.1/r0                ;
    }
    else if(E->m_barrel)   { //---------------------------->EB

      double T2 = Tp[0]*Tp[0]                ; 
      Tp[1]     = 400.                       ;
      Tp[2]     = (VZ*VZ)/((1.+T2)*r0)       ;
    }
    else                   { //---------------------------->EE

      double T2 = Tp[0]*Tp[0]                ; 
      Tp[1]     = 400.                       ;
      Tp[2]     = (VZ*VZ)/((1.+T2)*r0)       ;
    }
  }
  else {
    if     (E->m_barrel)   { //---------------------------->BB

      double dZ = m_zMax-m_zMin              ; 
      double dT = (E->m_zMax-E->m_zMin+2.*VZ);  
      Tp[1]     = dZ*dZ*.1                   ;
      Tp[2]     = dT*dT*.1/r1                ;
    }
    else if(  m_barrel )   { //---------------------------->BE

      double T2 = Tp[0]*Tp[0]                ;
      double dr = sqrt(r1)-sqrt(r0)          ;
      Tp[1]     = dr*dr*VZ*VZ/r1             ;
      Tp[2]     = (VZ*VZ)/((1.+T2)*r1)       ;      

    }             
    else                   { //---------------------------->EE 

      double T2 = Tp[0]*Tp[0]                ; 
      Tp[1]     = 400.                       ;
      Tp[2]     = (VZ*VZ)/((1.+T2)*r1)       ;
    } 
  }
}

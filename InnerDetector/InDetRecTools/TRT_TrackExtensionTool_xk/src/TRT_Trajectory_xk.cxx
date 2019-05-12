/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TRT_TrackExtensionTool_xk/TRT_Trajectory_xk.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/Track.h"

///////////////////////////////////////////////////////////////////
// Set TRT detector manager
///////////////////////////////////////////////////////////////////

void  InDet::TRT_Trajectory_xk::set
(const TRT_ID                     *   m,
 Trk::IPatternParametersPropagator*   pr,
 Trk::IPatternParametersUpdator   *   up,
 Trk::IRIO_OnTrackCreator         * riod,
 Trk::IRIO_OnTrackCreator         * rion,
 double roadwidth                       ,
 double zvertexwidth                    ,
 double impact                          ,
 double scale                           )
{
  m_proptool     = pr                 ;
  m_updatortool  = up                 ;
  m_roadwidth2   = roadwidth*roadwidth;
  m_zVertexWidth = fabs(zvertexwidth) ;
  m_impact       = fabs(impact      ) ;
  m_scale_error  = fabs(scale       ) ;
  for(int i=0; i!=400; ++i) m_elements[i].set(m,pr,up,riod,rion,m_scale_error);  
}

void  InDet::TRT_Trajectory_xk::set
(Trk::MagneticFieldProperties& mp,MagField::IMagFieldSvc*& ms )

{
  m_fieldprop    = mp;
  for(int i=0; i!=400; ++i) m_elements[i].set(mp,ms);  
}

///////////////////////////////////////////////////////////////////
// Initiate trajectory for precision seed 
///////////////////////////////////////////////////////////////////

void InDet::TRT_Trajectory_xk::initiateForPrecisionSeed
(std::list< std::pair<Amg::Vector3D,double> >      & Gp  ,
 std::list<const InDetDD::TRT_BaseElement*>        & De  ,
 const TRT_DriftCircleContainer*                   & TRTc,
 const Trk::PatternTrackParameters                 & Tp   )
{
  m_ndf             =  0;
  m_nholes          =  0;
  m_nholesb         =  0;
  m_nholese         =  0;
  m_dholes          =  0;
  m_nclusters       =  0;
  m_ntclusters      =  0;
  m_nElements       =  0;
  m_naElements      =  0;
  m_xi2             = 0.;
  m_parameters      = Tp;

  InDet::TRT_DriftCircleContainer::const_iterator w;
  InDet::TRT_DriftCircleCollection::const_iterator ti,te;

  std::list<const InDetDD::TRT_BaseElement*>::iterator d=De.begin(),de=De.end();

  std::list< std::pair<Amg::Vector3D,double> >::iterator i=Gp.begin(),i0=Gp.begin(),ie=Gp.end();
  if(i0==ie) return;
 
  // Primary trajectory direction calculation
  //
  double A[4]; A[3]=Tp.par()[4];

  for(++i; i!=ie; ++i) {
    if( (*i).second-(*i0).second > 1.) {
      
      A[0] = (*i).first.x()-(*i0).first.x();
      A[1] = (*i).first.y()-(*i0).first.y();
      A[2] = (*i).first.z()-(*i0).first.z();
      double As = 1./sqrt(A[0]*A[0]+A[1]*A[1]+A[2]*A[2]);
      A[0]*=As; A[1]*=As; A[2]*=As; i0=i; break;
    } 
  }

  for(i=Gp.begin(); i!=ie; ++i) {
    
    IdentifierHash id = (*d)->identifyHash(); w=(*TRTc).indexFind(id);
    bool q;
    if(w!=(*TRTc).end() && (*w)->begin()!=(*w)->end()) {
      ti = (*w)->begin(); te = (*w)->end  ();

      q = m_elements[m_nElements].initiateForPrecisionSeed(true,(*d),ti,te,(*i),A,m_roadwidth2);
      if(q && m_elements[m_nElements].isCluster()) ++m_naElements;

    }
    else                                               {

      q = m_elements[m_nElements].initiateForPrecisionSeed(false,(*d),ti,te,(*i),A,m_roadwidth2);
    }
    if(q && m_elements[m_nElements].nlinks()) ++m_nElements;
    if(++d==de) break;

    // New trajectory direction calculation
    //
    if( (*i).second-(*i0).second > 50.) {
      
      A[0] = (*i).first.x()-(*i0).first.x();
      A[1] = (*i).first.y()-(*i0).first.y();
      A[2] = (*i).first.z()-(*i0).first.z();
      double As = 1./sqrt(A[0]*A[0]+A[1]*A[1]+A[2]*A[2]);
      A[0]*=As; A[1]*=As; A[2]*=As; i0=i;
    }
  }
  m_firstRoad       = 0                        ;
  m_lastRoad        = m_firstRoad+m_nElements-1;
  m_firstTrajectory = m_firstRoad              ;
  m_lastTrajectory  = m_lastRoad               ;
}

///////////////////////////////////////////////////////////////////
// Initiate trajectory for TRT seed 
///////////////////////////////////////////////////////////////////

void InDet::TRT_Trajectory_xk::initiateForTRTSeed
(std::list< std::pair<Amg::Vector3D,double> >      & Gp  ,
 std::list<const InDetDD::TRT_BaseElement*>        & De  ,
 const TRT_DriftCircleContainer*                   & TRTc,
 const Trk::PatternTrackParameters                 & Tp   )
{
  m_ndf             =  0;
  m_nholes          =  0;
  m_nholesb         =  0;
  m_nholese         =  0;
  m_dholes          =  0;
  m_nclusters       =  0;
  m_ntclusters      =  0;
  m_nElements       =  0;
  m_naElements      =  0;
  m_firstRoad       =  0;
  m_lastRoad        = m_firstRoad; 
  m_firstTrajectory =  -1;
  m_xi2             = 0.;
  m_parameters      = Tp;

  InDet::TRT_DriftCircleContainer::const_iterator w;
  InDet::TRT_DriftCircleCollection::const_iterator ti,te;

  std::list<const InDetDD::TRT_BaseElement*>::iterator d=De.begin(),de=De.end();

  std::list< std::pair<Amg::Vector3D,double> >::iterator i=Gp.begin(),i0=Gp.begin(),ie=Gp.end();
  if(i0==ie) return;

  // Primary trajectory direction calculation
  //
  double A[4]; A[3]=Tp.par()[4];

  for(++i; i!=ie; ++i) {
    if( (*i).second-(*i0).second > 1.) {

      A[0] = (*i).first.x()-(*i0).first.x();
      A[1] = (*i).first.y()-(*i0).first.y();
      A[2] = (*i).first.z()-(*i0).first.z();
      double As = 1./sqrt(A[0]*A[0]+A[1]*A[1]+A[2]*A[2]);
      A[0]*=As; A[1]*=As; A[2]*=As; i0=i; break;
    } 
  }

  for(i=Gp.begin(); i!=ie; ++i) {

    IdentifierHash id = (*d)->identifyHash(); w=(*TRTc).indexFind(id);
    bool q;
    if(w!=(*TRTc).end() && (*w)->begin()!=(*w)->end()) {

      ti = (*w)->begin(); te = (*w)->end  ();
      q = m_elements[m_lastRoad].initiateForTRTSeed(1,(*d),ti,te,(*i),A,m_roadwidth2);
      if(m_elements [m_lastRoad].isCluster()) ++m_naElements;

    }
    else                                               {
      q = m_elements[m_lastRoad].initiateForTRTSeed(0,(*d),ti,te,(*i),A,m_roadwidth2);
    }

    if(m_elements[m_lastRoad].nlinks()) {

      if(q) {
	if(m_firstTrajectory < 0) {m_firstTrajectory = m_lastTrajectory = m_lastRoad;}
	else                      {                    m_lastTrajectory = m_lastRoad;}
      }
      ++m_lastRoad;
    }
    if(++d==de) break;

    // New trajectory direction calculation
    //
    if( (*i).second-(*i0).second > 50.) {
      
      A[0] = (*i).first.x()-(*i0).first.x();
      A[1] = (*i).first.y()-(*i0).first.y();
      A[2] = (*i).first.z()-(*i0).first.z();
      double As = 1./sqrt(A[0]*A[0]+A[1]*A[1]+A[2]*A[2]);
      A[0]*=As; A[1]*=As; A[2]*=As; i0=i;
    }
  }
  m_nElements = m_lastRoad - m_firstRoad; m_lastRoad-=1;
  if(m_firstTrajectory < 0) m_naElements = 0;
}

///////////////////////////////////////////////////////////////////
// Track finding with drift time information
///////////////////////////////////////////////////////////////////

void InDet::TRT_Trajectory_xk::trackFindingWithDriftTime(double DA)
{
  const double rcut = 2.2;
  double       rs   = 2.0;
  int          n    = 0  ;

  for(int i=m_firstTrajectory; i<=m_lastTrajectory; ++i) {
    
    if(!m_elements[i].isCluster()) continue;
    int nl = m_elements[i].nlinks();

    double dmin = m_elements[i].dnegative()-.01;
    double dmax = m_elements[i].dpositive()+.01;
    if(fabs(dmin) < rcut || fabs(dmax) < rcut) {dmin = -rcut; dmax = rcut;}

    for(int j=0; j!=nl; ++j) {

      if(!m_elements[i].link(j).cluster()) continue;
      double u = m_elements[i].link(j).way();      if(       u==0.        ) continue; 
      double d = m_elements[i].link(j).distance(); if(d < dmin || d > dmax) continue;
      double ui= 1./u;
      double s = rs*ui;
      double v = d *ui;
      m_U[n] = u; m_V[n++] = v-s; m_U[n] = u; m_V[n++] = v+s;
      
      double r = m_elements[i].link(j).cluster()->localPosition()(Trk::driftRadius);
      double e = .35;

      if(r>rs) r=rs; 
      
      double r2 = r+e; if(r2 >  rs) r2 = rs;
      double r1 = r-e; if(r1 >= r2) r1 = r2-2.*e;
      r1       *= ui;
      r2       *= ui;
      if( r<e ) {
	m_U[n] = u; m_V[n++] = v-r2;   m_U[n] = u; m_V[n++] = v+r2; 
      }
      else   {
	m_U[n] = u; m_V[n++] = v-r2;   m_U[n] = u; m_V[n++] = v-r1; 
	m_U[n] = u; m_V[n++] = v+r1;   m_U[n] = u; m_V[n++] = v+r2; 
      }  
      if(n>4990) goto stab;
    }
  }
 stab:
  stabline(n,DA);
}

///////////////////////////////////////////////////////////////////
// Track finding without drift time information
///////////////////////////////////////////////////////////////////

void InDet::TRT_Trajectory_xk::trackFindingWithoutDriftTime(double DA)
{
  const double rcut = 2.2 ;
  const double rs   = 2.00;
  const double sr   = 50  ;
  int          n    = 0   ;

  for(int i=m_firstTrajectory; i<=m_lastTrajectory; ++i) {
    
    if(!m_elements[i].isCluster()) continue;
    int nl = m_elements[i].nlinks();

    double dmin = m_elements[i].dnegative()-.01;
    double dmax = m_elements[i].dpositive()+.01;
    if(fabs(dmin) < rcut || fabs(dmax) < rcut) {dmin = -rcut; dmax = rcut;}

    for(int j=0; j!=nl; ++j) {

      if(!m_elements[i].link(j).cluster()) continue;

      double u = m_elements[i].link(j).way(); if(u==0.) continue;  double ui = 1./u;
 
      double di= m_elements[i].link(j).distance(); if(di < dmin || di > dmax) continue;
 
      double d = rs+fabs(m_elements[i].link(j).sdistance()*sr);
      double s = d *ui;
      double v = di*ui;
      m_U[n] = u; m_V[n++] = v-s; m_U[n] = u; m_V[n++] = v+s;
      if(n>4990) goto stab;
    }
  }
  stab:
  stabline(n,DA);
}

///////////////////////////////////////////////////////////////////
// Track finding with drift time information for best link
///////////////////////////////////////////////////////////////////

void InDet::TRT_Trajectory_xk::trackFindingWithDriftTimeBL(double DA)
{
  double rs = 2.0;
  int    n  = 0  ;

  for(int i=m_firstTrajectory; i<=m_lastTrajectory; ++i) {
    
    int l = m_elements[i].bestlink();
    if( l<0 || !m_elements[i].link(l).cluster()) continue;

    double u = m_elements[i].link(l).way(); if(u==0.) continue; double ui = 1./u;
    double s = rs*ui;
    double v = m_elements[i].link(l).distance()*ui;
    m_U[n] = u; m_V[n++] = v-s; m_U[n] = u; m_V[n++] = v+s;
    
    double r = m_elements[i].link(l).cluster()->localPosition()(Trk::driftRadius);
    double e = .35;
    
    if(r>rs) r=rs; 
    
    double r2 = r+e; if(r2 >  rs) r2 = rs;
    double r1 = r-e; if(r1 >= r2) r1 = r2-2.*e;
    r1       *= ui;
    r2       *= ui;
    if( r<e ) {
      m_U[n] = u; m_V[n++] = v-r2;   m_U[n] = u; m_V[n++] = v+r2; 
    }
    else   {
      m_U[n] = u; m_V[n++] = v-r2;   m_U[n] = u; m_V[n++] = v-r1; 
      m_U[n] = u; m_V[n++] = v+r1;   m_U[n] = u; m_V[n++] = v+r2; 
    }  
    if(n>4990) break;
  }
  stabline(n,DA);
}

///////////////////////////////////////////////////////////////////
// Track finding without drift time information for best links
///////////////////////////////////////////////////////////////////

void InDet::TRT_Trajectory_xk::trackFindingWithoutDriftTimeBL(double DA)
{
  const double rs = 2.00;
  const double sr = 50  ;
  int          n  = 0   ;

  for(int i=m_firstTrajectory; i<=m_lastTrajectory; ++i) {
    
    int l = m_elements[i].bestlink();
    if( l<0 || !m_elements[i].link(l).cluster()) continue;
    
    double u = m_elements[i].link(l).way(); if(u==0.) continue; double ui = 1./u;
    double d = rs+fabs(m_elements[i].link(l).sdistance()*sr);
    double s = d*ui;
    double v = m_elements[i].link(l).distance()*ui;
    m_U[n] = u; m_V[n++] = v-s; m_U[n] = u; m_V[n++] = v+s;
    if(n>4990) break;
  }
  stabline(n,DA);
}

///////////////////////////////////////////////////////////////////
//  Search start and stop trajectory for endcap TRT
///////////////////////////////////////////////////////////////////

bool InDet::TRT_Trajectory_xk::searchStartStop()
{
  const double rs = 2.00, rse = 1.85, sr = 40;

  int w[400], W = 0;

  for(int e = m_firstRoad; e<=m_lastRoad; ++e) {

    double D = m_elements[e].findCloseLink(m_A,m_B);
    int    b = m_elements[e].bestlink();
    w[e]     = 0;
    if( D < rs+fabs(m_elements[e].link(b).sdistance()*sr)) {

      if     (m_elements[e].link(b).cluster())  w[e] = 1;
      else if(       D      <      rse       )  w[e] =-1;
      
    }
    W+=w[e];
  }

  // Search first element trajectory
  //
  int Wm = W;
  m_firstTrajectory  = m_firstRoad;
  for(int e =  m_firstRoad; e < m_lastRoad; ++e ) {
    if(w[e]>0) {if(W>Wm) {Wm=W; m_firstTrajectory=e;} --W;} else if(w[e]<0) ++W;
  } 

  // Search last element trajectory
  //
  W = Wm;
  m_lastTrajectory = m_lastRoad;
  for(int e = m_lastRoad; e > m_firstTrajectory; --e) {
    if(w[e]>0) {if(W>Wm) {Wm=W; m_lastTrajectory=e;} --W;} else if(w[e]<0) ++W;
  }
  if((m_lastTrajectory-m_firstTrajectory) < 5 ) return false;
  return true;
}

///////////////////////////////////////////////////////////////////
// Trajectory  building without propagation for precision seeds
///////////////////////////////////////////////////////////////////

void  InDet::TRT_Trajectory_xk::buildTrajectoryForPrecisionSeed
(bool useDritRadius)
{
  m_nclusters = m_ntclusters = 0;

  int e  = m_firstTrajectory;
  int el = m_lastTrajectory ;

  for(; e<=el; ++e) {

    bool q =  useDritRadius;
    bool h = false         ;
    if     (m_elements[e].buildForPrecisionSeed(m_A,m_B,q,h)) { // Cluster
      
      if(++m_nclusters==1) m_firstTrajectory=e;
      m_lastTrajectory=e; 
      if(q) ++m_ntclusters;
      m_nholes = m_nholese; 
    }
    else if(h)                                                  { // Hole
      m_nclusters ? ++m_nholese : ++m_nholesb;
    }
  }  
  m_nholese-=m_nholes;
}

///////////////////////////////////////////////////////////////////
// Trajectory  building without propagation for TRT seeds
///////////////////////////////////////////////////////////////////

void  InDet::TRT_Trajectory_xk::buildTrajectoryForTRTSeed
(bool useDritRadius)
{
  
  m_nclusters = m_ntclusters = 0;
  int e       = m_firstTrajectory;
  int el      = m_lastTrajectory ;

  for(; e<=el; ++e) {

    bool q =  useDritRadius;
    bool h = false         ;
    if     (m_elements[e].buildForTRTSeed(m_A,m_B,q,h)) { // Cluster
      
      if(++m_nclusters==1) m_firstTrajectory=e;
      m_lastTrajectory=e; 
      if(q) ++m_ntclusters;
      m_nholes = m_nholese; 
    }
    else if(h)                                                  { // Hole
      m_nclusters ? ++m_nholese : ++m_nholesb;
    }
  }  
  m_nholese-=m_nholes;
}

///////////////////////////////////////////////////////////////////
// Trajectory correction for new polar angle
///////////////////////////////////////////////////////////////////

void  InDet::TRT_Trajectory_xk::radiusCorrection()
{
  if(isFirstElementBarrel()) return;
  
  int e     = m_firstTrajectory;
  int el    = m_lastTrajectory ;
  double r0 = m_elements[e ].radiusMin();
  double z0 = m_elements[e ].z        ();
  double r1 = m_elements[el].radiusMax();
  double z1 = m_elements[el].z        ();
  double dz = z1-z0;  if(fabs(dz) < 1.) return;                     
  double rz = (r1-r0)/dz; 
  double zv = z0 - r0/rz;
  
  if     (zv >  m_zVertexWidth) zv = m_zVertexWidth;
  else if(zv < -m_zVertexWidth) zv =-m_zVertexWidth;
  rz  = r0/(z0-zv);

  for (; e<=el; ++e) {
    double dr = r0+(m_elements[e].z()-z0)*rz-m_elements[e].radius();
    m_elements[e].radiusCorrection(dr);  
  }
}

///////////////////////////////////////////////////////////////////
// Trajectory  conversion to MeasurementBase
///////////////////////////////////////////////////////////////////

void InDet::TRT_Trajectory_xk::convert
(std::vector<const Trk::MeasurementBase*>& MB)
{
  for(int e = m_firstTrajectory; e<=m_lastTrajectory; ++e) {

    const Trk::MeasurementBase* mb = m_elements[e].rioOnTrack();
    if(mb) MB.push_back(mb);
  }  
}

///////////////////////////////////////////////////////////////////
// Trajectory  conversion to TrackSegment
///////////////////////////////////////////////////////////////////

Trk::TrackSegment* InDet::TRT_Trajectory_xk::convert()
{

  // Test quality of propagation to perigee
  //
  if(fabs(m_parameters.pT()) < 300.) return 0;

  const Trk::Surface* sur = m_parameters.associatedSurface();
  
  if(!sur) {
    Amg::Vector3D GP(0.,0.,0.); sur = new Trk::PerigeeSurface(GP);
  }

  DataVector<const Trk::MeasurementBase>* rio 
      = new DataVector<const Trk::MeasurementBase>;

  // Pseudo-measurement production
  //
  std::pair<const Trk::PseudoMeasurementOnTrack*,const Trk::PseudoMeasurementOnTrack* > pms;

  // Vector of TRT_DriftCircleOnTrack production
  //
  int nbarrel=0,nendcap=0;
  //const Trk::Surface *lastbarrelsurf=0;
  
  for(int e = m_firstTrajectory; e<=m_lastTrajectory; ++e) {

    const Trk::MeasurementBase* r = m_elements[e].rioOnTrack();
    if(r) {
      if (fabs(r->associatedSurface().transform()(2,2)) <.5) nendcap++;
      else {
        nbarrel++;
        //lastbarrelsurf=&r->associatedSurface();
      }
      
      rio->push_back(r);
    }
  }  
  if(rio->empty()) {delete rio; return 0;}
  int bec=0;
  if (nbarrel>0 && nendcap>0) bec=1;
  else if (nbarrel==0 && nendcap>0) bec=2;
  pms=pseudoMeasurements(&(**rio->begin()).associatedSurface(),&(**rio->rbegin()).associatedSurface(),bec);
  if(pms.first) rio->insert(rio->begin(),pms.first);
  if(pms.second) {
    if (fabs((**rio->rbegin()).associatedSurface().center().z())<2650.) rio->push_back(pms.second);
    else rio->insert(rio->begin()+1,pms.second);
  }
  // Track segment production
  //
  double P[5]={m_parameters.par()[0],
	       m_parameters.par()[1],
	       m_parameters.par()[2],
	       m_parameters.par()[3],
	       m_parameters.par()[4]};
  
  if(!m_ndf) {m_ndf = rio->size()-5; m_xi2 = 0.;}

  if(!sur) {
    Amg::Vector3D GP(0.,0.,0.); sur = new Trk::PerigeeSurface(GP);
  }
  Trk::FitQuality      * fqu = new Trk::FitQualityOnSurface(m_xi2,m_ndf)         ;

  return new Trk::TrackSegment(Trk::LocalParameters(P[0],P[1],P[2],P[3],P[4]),m_parameters.covariance(),sur,rio,fqu,Trk::Segment::TRT_SegmentMaker);
}

///////////////////////////////////////////////////////////////////
// Pseudo-measurement production
///////////////////////////////////////////////////////////////////

std::pair<const Trk::PseudoMeasurementOnTrack* , const Trk::PseudoMeasurementOnTrack*>
InDet::TRT_Trajectory_xk::pseudoMeasurements(const Trk::Surface *firstsurf, const Trk::Surface* lastsurf,int bec)
{
  const Trk::PseudoMeasurementOnTrack *pmon=0,*pmon2=0;
  if(!firstsurf || !lastsurf) return std::make_pair(pmon,pmon2);
  double pseudotheta=0;
  const Trk::CylinderBounds *cylb=dynamic_cast<const Trk::CylinderBounds *>(&lastsurf->bounds());
  if(!cylb) return std::make_pair(pmon,pmon2);

  double halfz=cylb->halflengthZ();
  cylb=dynamic_cast<const Trk::CylinderBounds *>(&firstsurf->bounds());  
  if(!cylb) return std::make_pair(pmon,pmon2);

  double halfzfirst=cylb->halflengthZ();
  double tempz=lastsurf->center().z();
  double tempr=0;
  //double pseudoz=0;
  Amg::Vector3D          pseudopoint;
  Amg::RotationMatrix3D  pseudorot;
  if (bec==0) {
    //pseudotheta=M_PI/2.;
    tempr=lastsurf->center().perp();
    if (tempr<1000.) tempz+= (lastsurf->center().z()>0) ? halfz : -halfz;
    pseudopoint = Amg::Vector3D(lastsurf->center().x(),lastsurf->center().y(),tempz);
    pseudorot   = lastsurf->transform().rotation();
  }
  else if (fabs(lastsurf->center().z())<2650.) {
    //std::cout << "sur center: " << sur->center() << " halfz: " << halfz << " firstsur center: " << firstsur->center() << " halfzfirst: " << halfzfirst << std::endl;
    tempr= lastsurf->center().perp()+halfz;
    tempz=lastsurf->center().z();
    pseudopoint = Amg::Vector3D(lastsurf->center().x(),lastsurf->center().y(),tempz)-halfz*lastsurf->transform().rotation().col(2);
    pseudorot   = lastsurf->transform().rotation();

  }
  else {
    tempr=firstsurf->center().perp()-halfzfirst;
    tempz=firstsurf->center().z();
    pseudopoint = Amg::Vector3D(firstsurf->center().x(),firstsurf->center().y(),tempz)+halfz*firstsurf->transform().rotation().col(2);
    pseudorot = firstsurf->transform().rotation();

  }
  pseudotheta=atan2(tempr,fabs(tempz));
  //std::cout << "bec: " << bec << " pseudotheta: " << pseudotheta << std::endl;
  if (lastsurf->center().z()<0) pseudotheta=M_PI-pseudotheta;

  Amg::Transform3D*  T = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D::Zero())*Amg::RotationMatrix3D::Identity());
  Trk::StraightLineSurface surn(T,100.,1000.);

  Trk::PatternTrackParameters P;  
  bool Q = m_proptool->propagateParameters
    (m_parameters,surn,P,Trk::anyDirection,m_fieldprop,Trk::pion);
  if(!Q) return std::make_pair(pmon,pmon2);


  Amg::MatrixX cov = (bec==0) ? Amg::MatrixX(2,2): Amg::MatrixX(1,1);
  cov(0,0)= 100.;
  if (bec==0) {
    cov(1,0) = 0.;
    cov(0,1) = 0.;
    cov(1,1) = .00001;
  }
  Trk::LocalParameters par = (bec==0) ? Trk::LocalParameters(std::make_pair(0,Trk::locZ),std::make_pair(pseudotheta,Trk::theta)) : Trk::LocalParameters(std::make_pair(0,Trk::locZ));

  pmon =  new Trk::PseudoMeasurementOnTrack(par,cov,surn);  

  if (bec==0) return std::make_pair(pmon,pmon2);
  cov = Amg::MatrixX(1,1);
  cov(0,0) = 100.;
  par = Trk::LocalParameters(std::make_pair(0,Trk::locZ));
  T    = new Amg::Transform3D(Amg::Translation3D(pseudopoint)*pseudorot);
  Trk::StraightLineSurface surn2(T,100.,1000.);

  pmon2 =  new Trk::PseudoMeasurementOnTrack(par,cov,surn2);

  //delete surn;
  return std::make_pair(pmon,pmon2);
}

///////////////////////////////////////////////////////////////////
// Track parameters estimation for last point of the trajectory
///////////////////////////////////////////////////////////////////

bool InDet::TRT_Trajectory_xk::trackParametersEstimationForLastPoint()
{
  int n1 = 0; 

  for(int e = (m_lastTrajectory+m_firstTrajectory)/2; e < m_lastTrajectory; ++e) {
    if(m_elements[e].status() >= 0) {n1=e; break;}
  }
  if(!n1) return false;
  return m_elements[m_lastTrajectory].trackParametersEstimation
    (&m_elements[n1],&m_elements[m_firstTrajectory],m_parameters,m_zVertexWidth);

}

///////////////////////////////////////////////////////////////////
// Track parameters estimation for first point of the trajectory
///////////////////////////////////////////////////////////////////

bool InDet::TRT_Trajectory_xk::trackParametersEstimationForFirstPoint()
{
  int n1 = 0;

  for(int e = (m_lastTrajectory+m_firstTrajectory)/2; e < m_lastTrajectory; ++e) {
    if(m_elements[e].status() >= 0) {n1=e; break;}
  }
  if(!n1) return false;
  return m_elements[m_firstTrajectory].trackParametersEstimation
    (&m_elements[n1],&m_elements[m_lastTrajectory],m_parameters,m_zVertexWidth);
}

///////////////////////////////////////////////////////////////////
// Track parameters estimation for first point of the trajectory
// with vertex constraint
///////////////////////////////////////////////////////////////////

bool InDet::TRT_Trajectory_xk::trackParametersEstimationForFirstPointWithVertexConstraint()
{
  return m_elements[m_firstTrajectory].trackParametersEstimation
    (&m_elements[m_lastTrajectory],0,m_parameters,m_zVertexWidth);

}

///////////////////////////////////////////////////////////////////
// Track parameters estimation for perigee with vertex constraint
///////////////////////////////////////////////////////////////////

bool InDet::TRT_Trajectory_xk::trackParametersEstimationForPerigeeWithVertexConstraint()
{
  return m_elements[m_firstTrajectory].trackParametersEstimation
    (&m_elements[m_lastTrajectory],m_parameters,m_zVertexWidth);
}

///////////////////////////////////////////////////////////////////
// Trajectory  fitter
///////////////////////////////////////////////////////////////////

bool InDet::TRT_Trajectory_xk::fitter()
{
  const double trad = .003;
  double        rad =  0. ;

  if(!trackParametersEstimationForLastPoint() || fabs(m_parameters.pT()) < 300.) return false;
  double sin2 = 1./sin(m_parameters.par()[3]); sin2*= sin2        ;
  double P42  =        m_parameters.par()[4] ; P42  = P42*P42*134.;
  
  if(!m_elements[m_lastTrajectory].addCluster(m_parameters,m_parameters,m_xi2)) return false;  


  Trk::NoiseOnSurface  noise;

  m_ndf =-4;
  bool barrel = false;
  for(int e = m_lastTrajectory-1; e>=m_firstTrajectory; --e) {

    rad+=trad;
    bool br = m_elements[e].isBarrel(); 
    if( (br && !barrel) || (!br && barrel) ) rad+=.1; barrel = br;

    if(m_elements[e].status() > 0) {

      // Add noise contribution
      //
      double covP = rad*P42; noise.set(covP*sin2,covP,0.,1.); 

      m_parameters.addNoise(noise,Trk::oppositeMomentum); 
    
      // Propagate to straw surface
      //
      if(!m_elements[e].propagate(m_parameters,m_parameters)) return false;

      // Update with straw information
      //
      double xi;
      if(!m_elements[e].addCluster(m_parameters,m_parameters,xi)) return false;
      m_xi2+=xi; ++m_ndf;
      rad = 0.;
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Search a stright line Y = A*X+B crossed max number segments                 
///////////////////////////////////////////////////////////////////


void InDet::TRT_Trajectory_xk::stabline(int Np,double DA) {

  m_A=m_B=0; if(Np<2) return; 
  double Amax =fabs(DA);

  int    i=0; 

  while(i!=Np) {m_SS[i].m_NA=i; m_SS[i].m_F=m_V[i]; ++i; m_SS[i].m_NA=i; m_SS[i].m_F=m_V[i]; ++i;}
  sort(m_SS,Np);

  // Search B with A = 0
  //
  i=0; int sm=0, s=0, l=0;
  while(i!=Np) {
    if((m_SS[i  ].m_NA&1)==(m_MA[i  ]=0)) {if(++s>sm) {sm=s; l=  i;}} else --s; 
    if((m_SS[i+1].m_NA&1)==(m_MA[i+1]=0)) {if(++s>sm) {sm=s; l=1+i;}} else --s; i+=2;
  } 

  m_B = .5*(m_V[m_SS[l].m_NA]+m_V[m_SS[l+1].m_NA]);

  // Search A and B
  //
  while(1) {

    i=(l=m_SS[l].m_NA); double u0=m_U[l], v0=m_V[l]; 

    // Vector slopes preparation
    //
    while(++i!=Np)  {if(m_U[i]!=u0) {break;}} 
    double U1=-1000., d=0.; int m=0;
    while(i<Np-1) {
      if (m_U[i]==U1) {
	if(fabs(m_SS[m].m_F=(m_V[i  ]-v0)*d)<Amax) m_SS[m++].m_NA=i;
        if(fabs(m_SS[m].m_F=(m_V[i+1]-v0)*d)<Amax) m_SS[m++].m_NA=i+1;
      }
      else          {
	d=1./((U1=m_U[i])-u0); 
	if(fabs(m_SS[m].m_F=(m_V[i  ]-v0)*d)<Amax) m_SS[m++].m_NA=i;
        if(fabs(m_SS[m].m_F=(m_V[i+1]-v0)*d)<Amax) m_SS[m++].m_NA=i+1;
      }
      i+=2;
    } 
    (i=l);  while(--i>0)  {if(m_U[i]!=u0) {break;}} U1=-1000.;
    while(i>0) {
      if (m_U[i]==U1) {
	if(fabs(m_SS[m].m_F=(m_V[i  ]-v0)*d)<Amax) m_SS[m++].m_NA=i;
        if(fabs(m_SS[m].m_F=(m_V[i-1]-v0)*d)<Amax) m_SS[m++].m_NA=i-1;
      }
      else          {
	d=1./((U1=m_U[i])-u0); 
	if(fabs(m_SS[m].m_F=(m_V[i  ]-v0)*d)<Amax) m_SS[m++].m_NA=i;
        if(fabs(m_SS[m].m_F=(m_V[i-1]-v0)*d)<Amax) m_SS[m++].m_NA=i-1;
      }
      i-=2;
    } 

    if(m<=4) break;
    sort(m_SS,m);

    int   nm = 0; s = 0; sm=-1000;
    for(int i=0; i!=m-1; ++i) {

       int na = m_SS[i].m_NA;

      if((na&1)==0) {if(na>l) {if(++s>sm) {sm=s; nm=i;}} else --s;}
      else          {if(na<l) {if(++s>sm) {sm=s; nm=i;}} else --s;}
    }
    if(nm==0) break;
    m_B = v0-(m_A=.5*(m_SS[nm].m_F+m_SS[nm+1].m_F))*u0;  m_MA[l]=1;

    if     (m_MA[m_SS[nm  ].m_NA]==0) l = nm;
    else if(m_MA[m_SS[nm+1].m_NA]==0) l = nm+1; 
    else    break;
  }
}

///////////////////////////////////////////////////////////////////
// The procedure sorts the elements into ascending order.
///////////////////////////////////////////////////////////////////

void InDet::TRT_Trajectory_xk::sort(samiStruct* s,int n) {
  std::sort(s,s+n,[](const samiStruct &a,const samiStruct&b)->bool {return a.m_F<b.m_F;});
}

///////////////////////////////////////////////////////////////////
// Trajectory  conversion to track 
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::TRT_Trajectory_xk::convert(const Trk::Track& Tr)
{
  const double trad = .003;
  double        rad =  0. ;

  const DataVector<const Trk::TrackStateOnSurface>* 
    tsos = Tr.trackStateOnSurfaces();
  
  if(!m_parameters.production((*(tsos->rbegin()))->trackParameters())) return 0;

  double sin2 = 1./sin(m_parameters.par()[3]); sin2*= sin2        ;
  double P42  =        m_parameters.par()[4] ; P42  = P42*P42*134.;

  Trk::NoiseOnSurface  noise;
  
  m_xi2              = 0.               ;
  m_ndf              = 0                ;
  bool barrel        = false            ;
  int  nworse        = 0                ;
  int lastTrajectory = m_firstTrajectory;
  Trk::PatternTrackParameters Tp        ; 
  Trk::PatternTrackParameters Tpl = m_parameters;

  for(int e = m_firstTrajectory; e<=m_lastTrajectory; ++e) {
    
    rad+=trad;
    bool br = m_elements[e].isBarrel();
    if( (br && !barrel) || (!br && barrel) ) rad+=.1; barrel = br;
 
    if(m_elements[e].status() > 0) {

      // Add noise contribution
      //
      double covP = rad*P42; noise.set(covP*sin2,covP,0.,1.); 
      m_parameters.addNoise(noise,Trk::alongMomentum); 
    
      // Propagate to straw surface
      //
      if(!m_elements[e].propagate(m_parameters,m_parameters)) break;

      // Update with straw information
      //
      double xi;
      if(!m_elements[e].addCluster(m_parameters,Tp,xi)) break;

     if(xi < 5.) {
	m_xi2+=xi; ++m_ndf ; 
	lastTrajectory = e ;
 	m_parameters   = Tp;
	Tpl            = Tp;
	nworse         = 0 ;
     }
      else {
	if(++nworse > 2) break;
      }
     rad = 0.;
    }
  }

  if(lastTrajectory == m_firstTrajectory || m_ndf < 5 || m_xi2 > 2.*double(m_ndf)) return 0;
  m_lastTrajectory = lastTrajectory;

  DataVector<const Trk::TrackStateOnSurface>::const_iterator 
    s = tsos->begin(), se = tsos->end();

  // Change first track parameters of the track
  //
  if(!Tp.production((*s)->trackParameters())) return 0;
  m_parameters = Tpl; updateTrackParameters(Tp);

  // Fill new track information
  //
  DataVector<const Trk::TrackStateOnSurface>* 
    tsosn = new DataVector<const Trk::TrackStateOnSurface>;

  tsosn->push_back(new Trk::TrackStateOnSurface(0,Tp.convert(true),0,0,(*s)->types()));

  // Copy old information to new track
  //
  for(++s; s!=se; ++s) {
    tsosn->push_back(new Trk::TrackStateOnSurface(*(*s)));
  }
  
  // Add new information from TRT without parameters 
  //
  for(int e = m_firstTrajectory; e < m_lastTrajectory; ++e) {
    const Trk::MeasurementBase* mb = m_elements[e].rioOnTrackSimple();
    if(mb) {
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>  typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Measurement);
      tsosn->push_back(new Trk::TrackStateOnSurface(mb,0,0,0,typePattern));
    }
  }
 
  // For last points add new information from TRT with parameters
  //
  const Trk::MeasurementBase* mb = m_elements[m_lastTrajectory].rioOnTrackSimple();
  if(mb) {
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>  typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Measurement);
    tsosn->push_back
      (new Trk::TrackStateOnSurface(mb,m_parameters.convert(true),0,0,typePattern));
  }
  
  // New fit quality production
  //
  const Trk::FitQuality* fqo = Tr.fitQuality();
  if(fqo) {
    m_xi2+= fqo->chiSquared();
    m_ndf+= fqo->numberDoF ();
  }
  Trk::FitQuality* fq = new Trk::FitQuality(m_xi2,m_ndf);
  return new Trk::Track (Tr.info(),tsosn,fq);
}

///////////////////////////////////////////////////////////////////
// Helix updating according new invert momentum information           
// Pi      New               invert momentum      
// CovPi   New covariance of invert momentum      
///////////////////////////////////////////////////////////////////

void InDet::TRT_Trajectory_xk::updateTrackParameters(Trk::PatternTrackParameters& T)
{
  if(m_parameters.cov()[14] >= T.cov()[14]) return;

  double Pi    = m_parameters.par()[ 4];
  double CovPi = m_parameters.cov()[14];
 
  const double* p = T.par();
  const double* v = T.cov();

  double V[15] = {v[ 0],
		  v[ 1],v[ 2],
		  v[ 3],v[ 4],v[ 5],
		  v[ 6],v[ 7],v[ 8],v[ 9],
		  v[10],v[11],v[12],v[13],v[14]};
  double P[ 5] = {p[ 0],p[ 1],p[ 2],p[ 3],p[ 4]};
  

  double W  = (1./CovPi-1./V[14]), det = V[14]/CovPi, Wd = W/det;
  double a0 = -V[10]*Wd;
  double a1 = -V[11]*Wd;
  double a2 = -V[12]*Wd;
  double a3 = -V[13]*Wd;
  double a4 = 1./det;

  // New covariance matrix 
  //
  V[ 0]  +=V[10]*a0;     
  V[ 1]  +=V[10]*a1;
  V[ 2]  +=V[11]*a1;
  V[ 3]  +=V[10]*a2;
  V[ 4]  +=V[11]*a2;
  V[ 5]  +=V[12]*a2; 
  V[ 6]  +=V[10]*a3;
  V[ 7]  +=V[11]*a3;
  V[ 8]  +=V[12]*a3;
  V[ 9]  +=V[13]*a3;
  V[10]  *=a4;
  V[11]  *=a4;
  V[12]  *=a4;
  V[13]  *=a4;
  V[14]   =CovPi;

  // New parameters
  //
  Wd = (Pi-P[4])*W;
  P[ 0] +=V[10]*Wd;
  P[ 1] +=V[11]*Wd;
  P[ 2] +=V[12]*Wd;
  P[ 3] +=V[13]*Wd;
  P[ 4]  = Pi;
  T.setParametersWithCovariance(T.associatedSurface(),P,V);
}


///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::TRT_Trajectory_xk& se)
{ 
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_Trajectory_xk::dump( std::ostream& out ) const
{
  
  if(m_nElements <=0 ) {
    out<<"Trajectory does not exist"<<std::endl;
  }
  out<<"|-------------------------------------------------------------------------|"
     <<std::endl;
  out<<"|                         TRAJECTORY "
     <<"                                     |"
     <<std::endl;

  out<<"| Has"<<std::setw(3)<<m_nElements
     <<" ("
     <<std::setw(3)<<m_naElements
     <<")"
     <<" elements and "
     <<std::setw(3)<<m_nclusters
     <<" ("<<std::setw(3)<<m_ntclusters<<")"<<" clusters "
     <<" and "
     <<std::setw(3)<<m_nholesb
     <<std::setw(3)<<m_nholes
     <<std::setw(3)<<m_nholese
     <<" holes       |"
     <<std::endl;
  out<<"| First and last elements of the       road "
     <<std::setw(3)<<m_firstRoad<<" "
     <<std::setw(3)<<m_lastRoad
     <<"                       |"
     <<std::endl;

  out<<"| First and last elements of the trajectory "
     <<std::setw(3)<<m_firstTrajectory<<" " 
     <<std::setw(3)<<m_lastTrajectory
     <<"                       |"
     <<std::endl;


  out<<"|-------------------------------------------------------------------------|"
     <<std::endl;
  out<<"|  n | b| c| nlinks | best | status |    impact  |    zlocal  |     way   |"
     <<std::endl;
  out<<"|-------------------------------------------------------------------------|"
     <<std::endl;
  
  for(int e = m_firstRoad; e<=m_lastRoad; ++e) {

    if(m_elements[e].status()<0) continue;
    
    int    l  = m_elements[e].bestlink();
    double im = m_elements[e].link(l).impact();
    double zl = m_elements[e].link(l).zlocal();
    double w  = m_elements[e].link(l).way   ();
    std::string ss;
    if     (m_elements[e].status()< 0) ss ="         ";
    else if(m_elements[e].status()==1) ss ="      +- ";
    else if(m_elements[e].status()==2) ss ="      ++ ";
    else                               ss ="      -- "; 

    out<<"| "
       <<std::setw(3)<<e
       <<std::setw(3)<<m_elements[e].isBarrel ()
       <<std::setw(3)<<m_elements[e].isCluster()
       <<std::setw(7)<<m_elements[e].nlinks   ()
       <<std::setw(7)<<m_elements[e].bestlink ()
       <<std::setw(4)<<ss
       <<std::setw(13)<<std::setprecision(6)<<im
       <<std::setw(13)<<std::setprecision(6)<<zl
       <<std::setw(13)<<std::setprecision(6)<<w
       <<" |"
       <<std::endl;
  }
  out<<"|-------------------------------------------------------------------------|"
     <<std::endl;
  return out;
}


/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// RungeKuttaPropagator.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////

#include "TrkExUtils/RungeKuttaUtils.h"
#include "TrkExUtils/JacobianHelper.h"
#include "TrkExRungeKuttaPropagator/RungeKuttaPropagator.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkExUtils/IntersectionSolution.h"
#include "TrkExUtils/TransportJacobian.h"
#include "TrkPatternParameters/PatternTrackParameters.h"


/// enables -ftree-vectorize in gcc 
#include "CxxUtils/vectorize.h"
ATH_ENABLE_VECTORIZATION;

/////////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////////

Trk::RungeKuttaPropagator::RungeKuttaPropagator
(const std::string& p,const std::string& n,const IInterface* t) :  
  AthAlgTool(p,n,t)
{
  m_dlt               = .000200;
  m_helixStep         = 1.     ; 
  m_straightStep      = .01    ;
  m_usegradient       = false  ;
 
  declareInterface<Trk::IPropagator>(this);   
  declareInterface<Trk::IPatternParametersPropagator>(this);
  declareProperty("AccuracyParameter"  ,m_dlt          );
  declareProperty("MaxHelixStep"       ,m_helixStep    );
  declareProperty("MaxStraightLineStep", m_straightStep);
  declareProperty("IncludeBgradients"  , m_usegradient );
}

/////////////////////////////////////////////////////////////////////////////////
// initialize
/////////////////////////////////////////////////////////////////////////////////

StatusCode Trk::RungeKuttaPropagator::initialize()
{
  ATH_MSG_VERBOSE(" RungeKutta_Propagator initialize() successful" );

  // temporarily protect the use of the field cond object/field cache for clients with IOV callbacks
      
  // Read handle for AtlasFieldCacheCondObj
  ATH_CHECK( m_fieldCondObjInputKey.initialize() );
  ATH_MSG_DEBUG("initialize() init key: " << m_fieldCondObjInputKey.key());

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
// finalize
/////////////////////////////////////////////////////////////////////////////////

StatusCode  Trk::RungeKuttaPropagator::finalize()
{
  ATH_MSG_VERBOSE(name() <<" finalize() successful");
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////////

Trk::RungeKuttaPropagator::~RungeKuttaPropagator()= default;

/////////////////////////////////////////////////////////////////////////////////
// Main function for NeutralParameters propagation 
/////////////////////////////////////////////////////////////////////////////////
      
Trk::NeutralParameters* Trk::RungeKuttaPropagator::propagate
(const Trk::NeutralParameters        & Tp,
 const Trk::Surface                  & Su,
 Trk::PropDirection                    D ,
 const Trk::BoundaryCheck            & B ,
 bool                          returnCurv) const
{
  double J[25];
  Cache cache{};
  return propagateStraightLine(cache,true,Tp,Su,D,B,J,returnCurv);
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for track parameters and covariance matrix propagation
// without transport Jacobian production
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters* Trk::RungeKuttaPropagator::propagate
(const ::EventContext&               ctx,
 const Trk::TrackParameters  & Tp,
 const Trk::Surface          & Su,
 Trk::PropDirection             D,
 const Trk::BoundaryCheck    &  B,
 const MagneticFieldProperties& M, 
 ParticleHypothesis              ,
 bool                  returnCurv,
 const TrackingVolume*           ) const 
{
  double J[25];
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_maxPath = 10000.;
  return propagateRungeKutta(cache,true,Tp,Su,D,B,M,J,returnCurv);
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for track parameters and covariance matrix propagation
// with transport Jacobian production
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters* Trk::RungeKuttaPropagator::propagate
(const ::EventContext&               ctx,
 const Trk::TrackParameters   & Tp ,
 const Trk::Surface&            Su ,
 Trk::PropDirection             D  ,
 const Trk::BoundaryCheck     & B  ,
 const MagneticFieldProperties& M  , 
 TransportJacobian           *& Jac,
 double&                 pathLength,
 ParticleHypothesis                ,
 bool                    returnCurv,
 const TrackingVolume*             ) const 
{
  double J[25];
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  pathLength < 0. ?  cache.m_maxPath = 10000. : cache.m_maxPath = pathLength; 
  Trk::TrackParameters* Tpn = propagateRungeKutta(cache,true,Tp,Su,D,B,M,J,returnCurv);
  pathLength = cache.m_step;  
  
  if(Tpn) { 
    J[24]=J[20]; J[23]=0.; J[22]=0.; J[21]=0.; J[20]=0.;
    Jac = new Trk::TransportJacobian(J);
  }
  else Jac = nullptr;
  return Tpn;
}

/////////////////////////////////////////////////////////////////////////////////
// Main function to finds the closest surface
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters* Trk::RungeKuttaPropagator::propagate
(const ::EventContext&               ctx,
 const TrackParameters        & Tp  ,
 std::vector<DestSurf>        & DS  ,
 PropDirection                  D   ,
 const MagneticFieldProperties& M   ,
 ParticleHypothesis                 ,
 std::vector<unsigned int>    & Sol ,
 double                       & Path,
 bool                         usePathLim,
 bool                               ,
 const TrackingVolume*              ) const
{
 
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  Sol.erase(Sol.begin(),Sol.end()); Path = 0.; if(DS.empty()) return nullptr;
  cache.m_direction               = D; 

  // Test is it measured track parameters
  //
  bool useJac; Tp.covariance() ?  useJac = true : useJac = false;

  // Magnetic field information preparation
  //
  M.magneticFieldMode() == Trk::FastField  ? cache.m_solenoid     = true : cache.m_solenoid     = false;  
  (useJac && m_usegradient) ? cache.m_needgradient = true : cache.m_needgradient = false; 
  M.magneticFieldMode() != Trk::NoField    ? cache.m_mcondition   = true : cache.m_mcondition   = false;

  // Transform to global presentation
  //

  double Po[45];
  double Pn[45]; 
  if(!Trk::RungeKuttaUtils::transformLocalToGlobal(useJac,Tp,Po)) return nullptr;
  Po[42]=Po[43]=Po[44]=0.;

  // Straight line track propagation for small step
  //
  if(D!=0) {
    double S= m_straightStep;
    if(D < 0) S = -S;
    S = straightLineStep(useJac,S,Po);
  }

  double Wmax  = 50000.    ; // Max pass
  double W     = 0.        ; // Current pass
  double Smax  = 100.      ; // Max step 
  if(D < 0) Smax = -Smax;
  if(usePathLim) Wmax = fabs(Path);

  std::multimap<double,int> DN; double Scut[3];
  int Nveto = Trk::RungeKuttaUtils::fillDistancesMap(DS,DN,Po,W,&Tp.associatedSurface(),Scut);

  // Test conditions tor start propagation and chocse direction if D == 0
  //
  if(DN.empty()) return nullptr;

  if(D == 0 && fabs(Scut[0]) < fabs(Scut[1])) Smax = -Smax; 

  if(Smax < 0. && Scut[0] > Smax) Smax =    Scut[0];
  if(Smax > 0. && Scut[1] < Smax) Smax =    Scut[1];
  if(Wmax >    3.*Scut[2]       ) Wmax = 3.*Scut[2]; 

  double                 Sl   = Smax ;
  double                 St   = Smax ;  
  bool                   InS  = false;
  TrackParameters* To         = nullptr    ;

  for(int i=0; i!=45; ++i) Pn[i]=Po[i];
  
  //----------------------------------Niels van Eldik patch
  double last_St    =   0. ;
  bool   last_InS   = !InS ;
  bool   reverted_P = false;
  //----------------------------------

  cache.m_newfield = true;
 
  while (fabs(W) < Wmax) {

    std::pair<double,int> SN;
    double                 S;

    if(cache.m_mcondition) {
      
      //----------------------------------Niels van Eldik patch
      if (reverted_P && St == last_St && InS == last_InS /*&& condition_fulfiled*/) {
          // inputs are not changed will get same result.
          break;
      }
      last_St  =  St;
      last_InS = InS;
      //----------------------------------

      if(!cache.m_needgradient) S = rungeKuttaStep            (cache,useJac,St,Pn,InS);
      else                S = rungeKuttaStepWithGradient(cache,St,Pn,InS);
    }
    else  {

      //----------------------------------Niels van Eldik patch
      if (reverted_P && St == last_St /*&& !condition_fulfiled*/) {
          // inputs are not changed will get same result.
          break;
      }
      last_St  =  St;
      last_InS = InS;
      //----------------------------------

      S = straightLineStep(useJac,St,Pn);
    }
    //----------------------------------Niels van Eldik patch
    reverted_P=false;
    //----------------------------------

    bool next; SN=Trk::RungeKuttaUtils::stepEstimator(DS,DN,Po,Pn,W,m_straightStep,Nveto,next); 

    if(next) {for(int i=0; i!=45; ++i) Po[i]=Pn[i]; W+=S; Nveto=-1; }
    else     {for(int i=0; i!=45; ++i) Pn[i]=Po[i]; reverted_P=true; cache.m_newfield= true;}

    if (fabs(S)+1. < fabs(St)) Sl=S; 
    InS ? St = 2.*S : St = S; 

    if(SN.second >= 0) {

      double Sa = fabs(SN.first);

      if(Sa > m_straightStep) {
	if(fabs(St) > Sa) St = SN.first;
      } 
      else                                {
	Path = W+SN.first;
	if((To = crossPoint(Tp,DS,Sol,Pn,SN))) return To;
	Nveto = SN.second; St = Sl;
      }
    }
  }
  return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for track parameters propagation without covariance matrix
// without transport Jacobian production
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters* Trk::RungeKuttaPropagator::propagateParameters
(const ::EventContext&               ctx,
 const Trk::TrackParameters  & Tp,
 const Trk::Surface          & Su, 
 Trk::PropDirection             D,
 const Trk::BoundaryCheck    &  B,
 const MagneticFieldProperties& M, 
 ParticleHypothesis              ,
 bool                  returnCurv,
 const TrackingVolume*           ) const 
{
  double J[25];
  Cache cache;

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_maxPath = 10000.;
  return propagateRungeKutta(cache,false,Tp,Su,D,B,M,J,returnCurv);
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for track parameters propagation without covariance matrix
// with transport Jacobian production
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters* Trk::RungeKuttaPropagator::propagateParameters
(const ::EventContext&               ctx,
 const Trk::TrackParameters    & Tp ,
 const Trk::Surface            & Su , 
 Trk::PropDirection              D  ,
 const Trk::BoundaryCheck      & B  ,
 const MagneticFieldProperties&  M  , 
 TransportJacobian            *& Jac,
 ParticleHypothesis                 ,
 bool                     returnCurv,
 const TrackingVolume*              ) const 
{
  double J[25];
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);
  cache.m_maxPath = 10000.;
  Trk::TrackParameters* Tpn = propagateRungeKutta   (cache,true,Tp,Su,D,B,M,J,returnCurv);
  
  if(Tpn) {
    J[24]=J[20]; J[23]=0.; J[22]=0.; J[21]=0.; J[20]=0.;
    Jac = new Trk::TransportJacobian(J);
  }
  else Jac = nullptr;
  return Tpn;
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for neutral track parameters propagation with or without jacobian
/////////////////////////////////////////////////////////////////////////////////
Trk::NeutralParameters* Trk::RungeKuttaPropagator::propagateStraightLine
(Cache&                         cache ,
 bool                           useJac,
 const Trk::NeutralParameters&  Tp    ,
 const Trk::Surface&            Su    ,
 Trk::PropDirection             D     ,
 const Trk::BoundaryCheck&      B     ,
 double                       * Jac   ,
 bool                       returnCurv) const 
{
  const Trk::Surface* su = &Su;
  if(su == &Tp.associatedSurface()) return buildTrackParametersWithoutPropagation(Tp,Jac);

  cache.m_direction               = D    ;
  cache.m_mcondition              = false;


  double P[64];
  double Step = 0; if(!Trk::RungeKuttaUtils::transformLocalToGlobal(useJac,Tp,P)) return nullptr;

  const Amg::Transform3D&  T = Su.transform();  
  int ty = Su.type(); 

  if      (ty == Trk::Surface::Plane    ) {

    double s[4];
    double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2);

    if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;}
    else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;}
    if(!propagateWithJacobian(cache,useJac,1,s,P,Step)) return nullptr;
  }
  else if (ty == Trk::Surface::Line     ) {

    double s[6] ={T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2)};
    if(!propagateWithJacobian(cache,useJac,0,s,P,Step)) return nullptr;
  }
  else if (ty == Trk::Surface::Disc     ) {

    double s[4];
    double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2);

    if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;}
    else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;}
    if(!propagateWithJacobian(cache,useJac,1,s,P,Step)) return nullptr;
  }
  else if (ty == Trk::Surface::Cylinder ) {

    const Trk::CylinderSurface* cyl = static_cast<const Trk::CylinderSurface*>(su);

    double r0[3] = {P[0],P[1],P[2]};
    double s [9] = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),cyl->bounds().r(),cache.m_direction,0.};

    if(!propagateWithJacobian(cache,useJac,2,s,P,Step)) return nullptr;

    // For cylinder we do test for next cross point
    //
    if(cyl->bounds().halfPhiSector() < 3.1 && newCrossPoint(*cyl,r0,P)) {
      s[8] = 0.; if(!propagateWithJacobian(cache,useJac,2,s,P,Step)) return nullptr;
    }
  }
  else if (ty == Trk::Surface::Perigee  ) {

    double s[6] ={T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2)};
    if(!propagateWithJacobian(cache,useJac,0,s,P,Step)) return nullptr;
  }
  else if (ty == Trk::Surface::Cone     ) {
    
    double k     = static_cast<const Trk::ConeSurface*>(su)->bounds().tanAlpha(); k = k*k+1.;
    double s[9]  = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),k,cache.m_direction,0.};
    if(!propagateWithJacobian(cache,useJac,3,s,P,Step)) return nullptr;
  }
  else return nullptr;

  if(cache.m_direction!=0. && (cache.m_direction*Step)<0.) return nullptr;

  // Common transformation for all surfaces (angles and momentum)
  //
  if(useJac) {
    double p=1./P[6]; P[35]*=p; P[36]*=p; P[37]*=p; P[38]*=p; P[39]*=p; P[40]*=p;
  }

  if(cache.m_maxPathLimit)  returnCurv = true;

  bool uJ = useJac; if(returnCurv) uJ = false;

  double p[5]; Trk::RungeKuttaUtils::transformGlobalToLocal(su,uJ,P,p,Jac);

  if(B) {Amg::Vector2D L(p[0],p[1]); if(!Su.insideBounds(L,0.)) return nullptr;}


  // Transformation to curvilinear presentation
  //
  if(returnCurv)  Trk::RungeKuttaUtils::transformGlobalToCurvilinear(useJac,P,p,Jac);


  if(!useJac || !Tp.covariance()) {

    if(!returnCurv) {
      return Su.createNeutralParameters(p[0],p[1],p[2],p[3],p[4],nullptr); 
    }
    else            {
      Amg::Vector3D gp(P[0],P[1],P[2]);
      return new Trk::NeutralCurvilinearParameters(gp,p[2],p[3],p[4]);
    }
  }

  AmgSymMatrix(5)* e  = Trk::RungeKuttaUtils::newCovarianceMatrix(Jac,*Tp.covariance());
  AmgSymMatrix(5)& cv = *e;
  
  if(cv(0,0)<=0. || cv(1,1)<=0. || cv(2,2)<=0. || cv(3,3)<=0. || cv(4,4)<=0.) {
    delete e; return nullptr;
  }

  if(!returnCurv) {
    return Su.createNeutralParameters(p[0],p[1],p[2],p[3],p[4],e); 
  }
  else            {
    Amg::Vector3D gp(P[0],P[1],P[2]);
    return new Trk::NeutralCurvilinearParameters(gp,p[2],p[3],p[4],e);
  }
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for charged track parameters propagation with or without jacobian
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters* Trk::RungeKuttaPropagator::propagateRungeKutta
(Cache&                         cache ,
 bool                           useJac,
 const Trk::TrackParameters&    Tp    ,
 const Trk::Surface&            Su    ,
 Trk::PropDirection             D     ,
 const Trk::BoundaryCheck&      B     ,
 const MagneticFieldProperties& M     ,
 double                       * Jac   ,
 bool                       returnCurv) const 
{ 
  const Trk::Surface* su = &Su;

  cache.m_direction               = D ; 

  M.magneticFieldMode() == Trk::FastField ? cache.m_solenoid     = true : cache.m_solenoid     = false;  
  (useJac && m_usegradient)? cache.m_needgradient = true : cache.m_needgradient = false; 
  M.magneticFieldMode() != Trk::NoField   ? cache.m_mcondition   = true : cache.m_mcondition   = false;

  if(su == &Tp.associatedSurface()) return buildTrackParametersWithoutPropagation(Tp,Jac);


  double P[64];
  double Step = 0.; if(!Trk::RungeKuttaUtils::transformLocalToGlobal(useJac,Tp,P)) return nullptr;

  const Amg::Transform3D&  T = Su.transform();  
  int ty = Su.type(); 
  
  if      (ty == Trk::Surface::Plane    ) {
    
    double s[4];
    double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2);

    if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;}
    else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;}
    if(!propagateWithJacobian(cache,useJac,1,s,P,Step)) return nullptr;

  }
  else if (ty == Trk::Surface::Line     ) {

    double s[6] = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2)};
    if(!propagateWithJacobian(cache,useJac,0,s,P,Step)) return nullptr;
  }
  else if (ty == Trk::Surface::Disc     ) {

    double s[4];
    double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2);
    if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;}
    else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;}
    if(!propagateWithJacobian(cache,useJac,1,s,P,Step)) return nullptr;
  }
  else if (ty == Trk::Surface::Cylinder ) {

    const Trk::CylinderSurface* cyl = static_cast<const Trk::CylinderSurface*>(su);

    double r0[3] = {P[0],P[1],P[2]};
    double s [9] = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),cyl->bounds().r(),cache.m_direction,0.};
    if(!propagateWithJacobian(cache,useJac,2,s,P,Step)) return nullptr;

    // For cylinder we do test for next cross point
    //
    if(cyl->bounds().halfPhiSector() < 3.1 && newCrossPoint(*cyl,r0,P)) {
      s[8] = 0.; if(!propagateWithJacobian(cache,useJac,2,s,P,Step)) return nullptr;
    }
  }
  else if (ty == Trk::Surface::Perigee  ) {

    double s[6] = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2)};
    if(!propagateWithJacobian(cache,useJac,0,s,P,Step)) return nullptr;
  }
  else if (ty == Trk::Surface::Cone     ) {
    
    double k    =  static_cast<const Trk::ConeSurface*>(su)->bounds().tanAlpha(); k = k*k+1.;
    double s[9] = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),k,cache.m_direction,0.};
    if(!propagateWithJacobian(cache,useJac,3,s,P,Step)) return nullptr;
  }
  else return nullptr;

  if(cache.m_direction && (cache.m_direction*Step)<0.) {return nullptr;} cache.m_step = Step;

  // Common transformation for all surfaces (angles and momentum)
  //
  if(useJac) {
    double p=1./P[6]; P[35]*=p; P[36]*=p; P[37]*=p; P[38]*=p; P[39]*=p; P[40]*=p;
  }

  if(cache.m_maxPathLimit)  returnCurv = true;

  bool uJ = useJac; if(returnCurv) uJ = false;
  double p[5]; Trk::RungeKuttaUtils::transformGlobalToLocal(su,uJ,P,p,Jac);

  if(B) {Amg::Vector2D L(p[0],p[1]); if(!Su.insideBounds(L,0.)) return nullptr;}

  // Transformation to curvilinear presentation
  //
  if(returnCurv)  Trk::RungeKuttaUtils::transformGlobalToCurvilinear(useJac,P,p,Jac);

  if(!useJac || !Tp.covariance()) {

    if(!returnCurv) {
      return Su.createTrackParameters(p[0],p[1],p[2],p[3],p[4],nullptr); 
    }
    else            {
      Amg::Vector3D gp(P[0],P[1],P[2]);
      return new Trk::CurvilinearParameters(gp,p[2],p[3],p[4]);
    }
  }

  AmgSymMatrix(5)* e  = Trk::RungeKuttaUtils::newCovarianceMatrix(Jac,*Tp.covariance());
  AmgSymMatrix(5)& cv = *e;
  
  if(cv(0,0)<=0. || cv(1,1)<=0. || cv(2,2)<=0. || cv(3,3)<=0. || cv(4,4)<=0.) {
    delete e; return nullptr;
  }

  if(!returnCurv) {
    return Su.createTrackParameters(p[0],p[1],p[2],p[3],p[4],e); 
  }
  else            {
    Amg::Vector3D gp(P[0],P[1],P[2]);
    return new Trk::CurvilinearParameters(gp,p[2],p[3],p[4],e);
  }
}

/////////////////////////////////////////////////////////////////////////////////
// Global positions calculation inside CylinderBounds
// where mS - max step allowed if mS > 0 propagate along    momentum
//                                mS < 0 propogate opposite momentum
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaPropagator::globalPositions 
(const ::EventContext&               ctx,
 std::list<Amg::Vector3D>      & GP,
 const TrackParameters         & Tp,
 const MagneticFieldProperties & M ,
 const CylinderBounds          & CB,
 double                          mS,
 ParticleHypothesis                ,
 const TrackingVolume*             ) const
{
  double P[45]; if(!Trk::RungeKuttaUtils::transformLocalToGlobal(false,Tp,P)) return;
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_direction = fabs(mS);
  if(mS > 0.) globalOneSidePositions(cache,GP,P,M,CB, mS);
  else        globalTwoSidePositions(cache,GP,P,M,CB,-mS);
}

/////////////////////////////////////////////////////////////////////////////////
//  Global position together with direction of the trajectory on the surface
/////////////////////////////////////////////////////////////////////////////////

const Trk::IntersectionSolution* Trk::RungeKuttaPropagator::intersect
( const ::EventContext&               ctx,
  const Trk::TrackParameters   & Tp,
  const Trk::Surface           & Su,
  const MagneticFieldProperties& M ,
  ParticleHypothesis               ,
  const TrackingVolume*            ) const 
{
  bool nJ = false;
  const Trk::Surface* su = &Su;
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);
 
  cache.m_direction            = 0. ;

  cache.m_needgradient = false; 
  M.magneticFieldMode() == Trk::FastField ? cache.m_solenoid     = true : cache.m_solenoid     = false;  
  M.magneticFieldMode() != Trk::NoField   ? cache.m_mcondition   = true : cache.m_mcondition   = false;

  double P[64]; if(!Trk::RungeKuttaUtils::transformLocalToGlobal(false,Tp,P)) return nullptr;
  double Step = 0.;

  const Amg::Transform3D&  T = Su.transform();  
  int ty = Su.type(); 

  if      (ty == Trk::Surface::Plane    ) {

    double s[4];
    const double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2);

    if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;}
    else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;}
    if(!propagateWithJacobian(cache,nJ,1,s,P,Step)) return nullptr;
  }
  else if (ty == Trk::Surface::Line     ) {

    double s[6] ={T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2)};
    if(!propagateWithJacobian(cache,nJ,0,s,P,Step)) return nullptr;
  }
  else if (ty == Trk::Surface::Disc     ) {

    double s[4];
    const double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2);

    if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;}
    else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;}
    if(!propagateWithJacobian(cache,nJ,1,s,P,Step)) return nullptr;
  }
  else if (ty == Trk::Surface::Cylinder ) {

    const Trk::CylinderSurface* cyl = static_cast<const Trk::CylinderSurface*>(su);

    double r0[3] = {P[0],P[1],P[2]};
    double s [9] = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),cyl->bounds().r(),cache.m_direction,0.};

    if(!propagateWithJacobian(cache,nJ,2,s,P,Step)) return nullptr;

    // For cylinder we do test for next cross point
    //
    if(cyl->bounds().halfPhiSector() < 3.1 && newCrossPoint(*cyl,r0,P)) {
      s[8] = 0.; if(!propagateWithJacobian(cache,nJ,2,s,P,Step)) return nullptr;
    }
  }
  else if (ty == Trk::Surface::Perigee  ) {

    double s[6] ={T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2)};
    if(!propagateWithJacobian(cache,nJ,0,s,P,Step)) return nullptr;
  }
  else if (ty == Trk::Surface::Cone     ) {
    
    double k     = static_cast<const Trk::ConeSurface*>(su)->bounds().tanAlpha(); k = k*k+1.;
    double s[9]  = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),k,cache.m_direction,0.};
    if(!propagateWithJacobian(cache,nJ,3,s,P,Step)) return nullptr;
  }
  else return nullptr;

  if(cache.m_maxPathLimit) return nullptr;

  Amg::Vector3D Glo(P[0],P[1],P[2]);
  Amg::Vector3D Dir(P[3],P[4],P[5]);
  Trk::IntersectionSolution* Int = new Trk::IntersectionSolution();
  Int->push_back(new Trk::TrackSurfaceIntersection(Glo,Dir,Step));    
  return Int;
}                                                   

/////////////////////////////////////////////////////////////////////////////////
// Runge Kutta main program for propagation with or without Jacobian
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaPropagator::propagateWithJacobian   
(Cache& cache                     ,                    
 bool Jac                         ,
 int kind                         ,
 double                       * Su,
 double                       * P ,
 double                       & W ) const
{
  const double Smax   = 1000.     ;  // max. step allowed
  double       Wmax   = cache.m_maxPath ;  // Max way allowed
  double       Wwrong = 500.      ;  // Max way with wrong direction
  double*      R      = &P[ 0]    ;  // Start coordinates
  double*      A      = &P[ 3]    ;  // Start directions
  double*      SA     = &P[42]    ; SA[0]=SA[1]=SA[2]=0.;
  cache.m_maxPathLimit      = false     ;  

  if(cache.m_mcondition && fabs(P[6]) > .1) return false; 

  // Step estimation until surface
  //
  bool Q; double S;
  double Step=Trk::RungeKuttaUtils::stepEstimator(kind,Su,P,Q); if(!Q) return false;

  bool dir = true;
  if(cache.m_mcondition && cache.m_direction && cache.m_direction*Step < 0.)  {
    Step = -Step; dir = false;
  }

  Step>Smax ? S=Smax : Step<-Smax ? S=-Smax : S=Step;
  double So = fabs(S); int iS = 0;

  bool InS = false;

  // Rkuta extrapolation
  //
  int niter = 0;
  cache.m_newfield = true;
  while(fabs(Step) > m_straightStep) {

    if(++niter > 10000) return false;

    if(cache.m_mcondition) {

      if(!cache.m_needgradient) W+=(S=rungeKuttaStep            (cache,Jac,S,P,InS));
      else                W+=(S=rungeKuttaStepWithGradient(cache,S,P,InS));
    }
    else  {
      W+=(S=straightLineStep(Jac,S,P));
    }

    Step = stepEstimatorWithCurvature(cache,kind,Su,P,Q); if(!Q) return false;

    if(!dir) {
      if(cache.m_direction && cache.m_direction*Step < 0.)  Step = -Step;
      else                                      dir  =  true;
    }
    
    if(S*Step<0.) {S = -S; ++iS;}

    double aS    = fabs(S   );
    double aStep = fabs(Step);
    if     (    aS > aStep             )  S = Step;
    else if(!iS && InS && aS*2. < aStep)  S*=2.   ;
    if(!dir && fabs(W) > Wwrong              ) return false;

    if(iS > 10 || (iS>3 && fabs(S)>=So)) {if(!kind) break; return false;}
    double dW =  Wmax-fabs(W);
    if(fabs(S) > dW) {S > 0. ? S = dW : S = -dW; Step = S; cache.m_maxPathLimit = true;}
    So=fabs(S);
  }
  
  // Output track parameteres
  //
  W+=Step;

  if(fabs(Step) < .001) return true;

  A [0]+=(SA[0]*Step); 
  A [1]+=(SA[1]*Step);
  A [2]+=(SA[2]*Step);
  double CBA = 1./sqrt(A[0]*A[0]+A[1]*A[1]+A[2]*A[2]);

  R[0]+=Step*(A[0]-.5*Step*SA[0]); A[0]*=CBA;
  R[1]+=Step*(A[1]-.5*Step*SA[1]); A[1]*=CBA;
  R[2]+=Step*(A[2]-.5*Step*SA[2]); A[2]*=CBA;
  return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Runge Kutta trajectory model (units->mm,MeV,kGauss)
// Uses Nystroem algorithm (See Handbook Net. Bur. ofStandards, procedure 25.5.20)
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaPropagator::rungeKuttaStep
  (Cache&                         cache,
   bool                           Jac,
   double                         S  , 
   double                       * P  ,
   bool                         & InS) const
{
  double* R    =          &P[ 0];            // Coordinates 
  double* A    =          &P[ 3];            // Directions
  double* sA   =          &P[42];
  const double  Pi   =  149.89626*P[6];      // Invert mometum/2. 
  double  dltm = m_dlt*.03      ;

  double f0[3];
  double f[3]; 
  if(cache.m_newfield) getField(cache,R,f0); else {f0[0]=cache.m_field[0]; f0[1]=cache.m_field[1]; f0[2]=cache.m_field[2];}

  bool Helix = false; if(fabs(S) < m_helixStep) Helix = true; 
  while(S != 0.) {
     
    double S3=(1./3.)*S;
    double S4=.25*S;
    double PS2=Pi*S;

    // First point
    //   
    double H0[3] = {f0[0]*PS2, f0[1]*PS2, f0[2]*PS2};
    double A0    = A[1]*H0[2]-A[2]*H0[1]            ;
    double B0    = A[2]*H0[0]-A[0]*H0[2]            ;
    double C0    = A[0]*H0[1]-A[1]*H0[0]            ;
    double A2    = A0+A[0]                          ;
    double B2    = B0+A[1]                          ;
    double C2    = C0+A[2]                          ;
    double A1    = A2+A[0]                          ;
    double B1    = B2+A[1]                          ;
    double C1    = C2+A[2]                          ;
    
    // Second point
    //
    if(!Helix) {
      double gP[3]={R[0]+A1*S4, R[1]+B1*S4, R[2]+C1*S4};
      getField(cache,gP,f);
    }
    else       {f[0]=f0[0]; f[1]=f0[1]; f[2]=f0[2];}

    double H1[3] = {f[0]*PS2,f[1]*PS2,f[2]*PS2}; 
    double A3    = (A[0]+B2*H1[2])-C2*H1[1]    ; 
    double B3    = (A[1]+C2*H1[0])-A2*H1[2]    ; 
    double C3    = (A[2]+A2*H1[1])-B2*H1[0]    ;
    double A4    = (A[0]+B3*H1[2])-C3*H1[1]    ; 
    double B4    = (A[1]+C3*H1[0])-A3*H1[2]    ; 
    double C4    = (A[2]+A3*H1[1])-B3*H1[0]    ;
    double A5    = 2.*A4-A[0]                  ; 
    double B5    = 2.*B4-A[1]                  ; 
    double C5    = 2.*C4-A[2]                  ;    

    // Last point
    //
    if(!Helix) {
      double gP[3]={R[0]+S*A4, R[1]+S*B4, R[2]+S*C4};    
      getField(cache,gP,f);
    }
    else       {f[0]=f0[0]; f[1]=f0[1]; f[2]=f0[2];} 

    double H2[3] = {f[0]*PS2,f[1]*PS2,f[2]*PS2}; 
    double A6    = B5*H2[2]-C5*H2[1]           ;
    double B6    = C5*H2[0]-A5*H2[2]           ;
    double C6    = A5*H2[1]-B5*H2[0]           ;
    
    
    // Test approximation quality on give step and possible step reduction
    //
    //double dE[4] = {(A1+A6)-(A3+A4),(B1+B6)-(B3+B4),(C1+C6)-(C3+C4),S};
    //double cS    = stepReduction(dE);
    //if     (cSn <  1.) {S*=cS; continue;}
    //cSn == 1. InS = false : InS = true;
    //

    // Test approximation quality on give step and possible step reduction
    //
    double EST = fabs((A1+A6)-(A3+A4))+fabs((B1+B6)-(B3+B4))+fabs((C1+C6)-(C3+C4)); 
    if(EST>m_dlt) {S*=.5; dltm = 0.; continue;} EST<dltm ? InS = true : InS = false;

    // Parameters calculation
    //   
    double A00=A[0];
    double A11=A[1];
    double A22=A[2];

    double Aarr[3]{A00,A11,A22}; 
    double A0arr[3]{A0,B0,C0}; 
    double A3arr[3]{A3,B3,C3}; 
    double A4arr[3]{A4,B4,C4}; 
    double A6arr[3]{A6,B6,C6}; 

    A[0] = 2.*A3+(A0+A5+A6); 
    A[1] = 2.*B3+(B0+B5+B6); 
    A[2] = 2.*C3+(C0+C5+C6);
    
    double D  = (A[0]*A[0]+A[1]*A[1])+(A[2]*A[2]-9.);
    double Sl = 2./S                                ;
    D         = (1./3.)-((1./648.)*D)*(12.-D)       ;

    R[0] +=(A2+A3+A4)*S3;
    R[1] +=(B2+B3+B4)*S3;
    R[2] +=(C2+C3+C4)*S3;
    A[0] *=D            ;
    A[1] *=D            ;
    A[2] *=D            ;
    sA[0] = A6*Sl       ; 
    sA[1] = B6*Sl       ;
    sA[2] = C6*Sl       ; 

    cache.m_field[0]=f[0]; cache.m_field[1]=f[1]; cache.m_field[2]=f[2]; cache.m_newfield = false;

    if(!Jac) return S;

    // Jacobian calculation
    //
    Trk::theJacProp(P,H0,H1,H2,Aarr,A0arr,A3arr,A4arr,A6arr,S3); 
    // double* d2A = &P[24];
    // double* d3A = &P[31]; 
    // double* d4A = &P[38]; 
    // double d2A0 = H0[2]*d2A[1]-H0[1]*d2A[2];
    // double d2B0 = H0[0]*d2A[2]-H0[2]*d2A[0];
    // double d2C0 = H0[1]*d2A[0]-H0[0]*d2A[1];
    // double d3A0 = H0[2]*d3A[1]-H0[1]*d3A[2];
    // double d3B0 = H0[0]*d3A[2]-H0[2]*d3A[0];
    // double d3C0 = H0[1]*d3A[0]-H0[0]*d3A[1];
    // double d4A0 =(A0+H0[2]*d4A[1])-H0[1]*d4A[2];
    // double d4B0 =(B0+H0[0]*d4A[2])-H0[2]*d4A[0];
    // double d4C0 =(C0+H0[1]*d4A[0])-H0[0]*d4A[1];
    // double d2A2 = d2A0+d2A[0];                
    // double d2B2 = d2B0+d2A[1];                
    // double d2C2 = d2C0+d2A[2];
    // double d3A2 = d3A0+d3A[0];                
    // double d3B2 = d3B0+d3A[1];                
    // double d3C2 = d3C0+d3A[2];
    // double d4A2 = d4A0+d4A[0];                
    // double d4B2 = d4B0+d4A[1];                
    // double d4C2 = d4C0+d4A[2];
    // double d0   = d4A[0]-A00;
    // double d1   = d4A[1]-A11;
    // double d2   = d4A[2]-A22;
    // double d2A3 = ( d2A[0]+d2B2*H1[2])-d2C2*H1[1];
    // double d2B3 = ( d2A[1]+d2C2*H1[0])-d2A2*H1[2];
    // double d2C3 = ( d2A[2]+d2A2*H1[1])-d2B2*H1[0];
    // double d3A3 = ( d3A[0]+d3B2*H1[2])-d3C2*H1[1];
    // double d3B3 = ( d3A[1]+d3C2*H1[0])-d3A2*H1[2];
    // double d3C3 = ( d3A[2]+d3A2*H1[1])-d3B2*H1[0];
    // double d4A3 = ((A3+d0)+d4B2*H1[2])-d4C2*H1[1];
    // double d4B3 = ((B3+d1)+d4C2*H1[0])-d4A2*H1[2];
    // double d4C3 = ((C3+d2)+d4A2*H1[1])-d4B2*H1[0];
    // double d2A4 = ( d2A[0]+d2B3*H1[2])-d2C3*H1[1];
    // double d2B4 = ( d2A[1]+d2C3*H1[0])-d2A3*H1[2];
    // double d2C4 = ( d2A[2]+d2A3*H1[1])-d2B3*H1[0];
    // double d3A4 = ( d3A[0]+d3B3*H1[2])-d3C3*H1[1];
    // double d3B4 = ( d3A[1]+d3C3*H1[0])-d3A3*H1[2];
    // double d3C4 = ( d3A[2]+d3A3*H1[1])-d3B3*H1[0];
    // double d4A4 = ((A4+d0)+d4B3*H1[2])-d4C3*H1[1];
    // double d4B4 = ((B4+d1)+d4C3*H1[0])-d4A3*H1[2];
    // double d4C4 = ((C4+d2)+d4A3*H1[1])-d4B3*H1[0];
    // double d2A5 = 2.*d2A4-d2A[0];            
    // double d2B5 = 2.*d2B4-d2A[1];            
    // double d2C5 = 2.*d2C4-d2A[2];
    // double d3A5 = 2.*d3A4-d3A[0];            
    // double d3B5 = 2.*d3B4-d3A[1];            
    // double d3C5 = 2.*d3C4-d3A[2];            
    // double d4A5 = 2.*d4A4-d4A[0];            
    // double d4B5 = 2.*d4B4-d4A[1];            
    // double d4C5 = 2.*d4C4-d4A[2];            
    // double d2A6 = d2B5*H2[2]-d2C5*H2[1];      
    // double d2B6 = d2C5*H2[0]-d2A5*H2[2];      
    // double d2C6 = d2A5*H2[1]-d2B5*H2[0];      
    // double d3A6 = d3B5*H2[2]-d3C5*H2[1];      
    // double d3B6 = d3C5*H2[0]-d3A5*H2[2];      
    // double d3C6 = d3A5*H2[1]-d3B5*H2[0];
    // double d4A6 = d4B5*H2[2]-d4C5*H2[1];      
    // double d4B6 = d4C5*H2[0]-d4A5*H2[2];      
    // double d4C6 = d4A5*H2[1]-d4B5*H2[0];      
      
    // double* dR  = &P[21];
    // dR [0]+=(d2A2+d2A3+d2A4)*S3;
    // dR [1]+=(d2B2+d2B3+d2B4)*S3;
    // dR [2]+=(d2C2+d2C3+d2C4)*S3;
    // d2A[0] =((d2A0+2.*d2A3)+(d2A5+d2A6))*(1./3.);      
    // d2A[1] =((d2B0+2.*d2B3)+(d2B5+d2B6))*(1./3.); 
    // d2A[2] =((d2C0+2.*d2C3)+(d2C5+d2C6))*(1./3.);

    // dR          = &P[28];
    // dR [0]+=(d3A2+d3A3+d3A4)*S3;
    // dR [1]+=(d3B2+d3B3+d3B4)*S3;
    // dR [2]+=(d3C2+d3C3+d3C4)*S3;
    // d3A[0] =((d3A0+2.*d3A3)+(d3A5+d3A6))*(1./3.);      
    // d3A[1] =((d3B0+2.*d3B3)+(d3B5+d3B6))*(1./3.); 
    // d3A[2] =((d3C0+2.*d3C3)+(d3C5+d3C6))*(1./3.);

    // dR          = &P[35];
    // dR [0]+=(d4A2+d4A3+d4A4)*S3;
    // dR [1]+=(d4B2+d4B3+d4B4)*S3;
    // dR [2]+=(d4C2+d4C3+d4C4)*S3;
    // d4A[0] =((d4A0+2.*d4A3)+(d4A5+d4A6+A6))*(1./3.);      
    // d4A[1] =((d4B0+2.*d4B3)+(d4B5+d4B6+B6))*(1./3.); 
    // d4A[2] =((d4C0+2.*d4C3)+(d4C5+d4C6+C6))*(1./3.);
    return S;
  }
  return S;
}

/////////////////////////////////////////////////////////////////////////////////
// Runge Kutta trajectory model (units->mm,MeV,kGauss)
// Uses Nystroem algorithm (See Handbook Net. Bur. ofStandards, procedure 25.5.20)
//    Where magnetic field information iS              
//    f[ 0],f[ 1],f[ 2] - Hx    , Hy     and Hz of the magnetic field         
//    f[ 3],f[ 4],f[ 5] - dHx/dx, dHx/dy and dHx/dz                           
//    f[ 6],f[ 7],f[ 8] - dHy/dx, dHy/dy and dHy/dz                           
//    f[ 9],f[10],f[11] - dHz/dx, dHz/dy and dHz/dz                           
//                                                                                   
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaPropagator::rungeKuttaStepWithGradient
(Cache&                       cache,                 
 double                         S  , 
 double                       * P  ,
 bool                         & InS) const
{
  const double C33 = 1./3.      ;  
  double* R    =          &P[ 0];           // Coordinates 
  double* A    =          &P[ 3];           // Directions
  double* sA   =          &P[42];
  const double  Pi   =  149.89626*P[6];           // Invert mometum/2. 
  double  dltm = m_dlt*.03      ;

  double f0[3];
  double f1[3];
  double f2[3];
  double g0[9];
  double g1[9];
  double g2[9];
  double H0[12];
  double H1[12];
  double H2[12];
  getFieldGradient(cache,R,f0,g0);

  while(S != 0.) {
 
    
    double S3=C33*S;
    double S4=.25*S;
    double PS2=Pi*S;

    // First point
    //   
    H0[0] = f0[0]*PS2; H0[1] = f0[1]*PS2; H0[2] = f0[2]*PS2;
    double A0    = A[1]*H0[2]-A[2]*H0[1]             ;
    double B0    = A[2]*H0[0]-A[0]*H0[2]             ;
    double C0    = A[0]*H0[1]-A[1]*H0[0]             ;
    double A2    = A[0]+A0                           ;
    double B2    = A[1]+B0                           ;
    double C2    = A[2]+C0                           ;
    double A1    = A2+A[0]                           ;
    double B1    = B2+A[1]                           ;
    double C1    = C2+A[2]                           ;
    
    // Second point
    //
    double gP1[3]={R[0]+A1*S4, R[1]+B1*S4, R[2]+C1*S4};
    getFieldGradient(cache,gP1,f1,g1);

    H1[0] = f1[0]*PS2; H1[1] = f1[1]*PS2; H1[2] = f1[2]*PS2; 
    double A3    = B2*H1[2]-C2*H1[1]+A[0]         ; 
    double B3    = C2*H1[0]-A2*H1[2]+A[1]         ; 
    double C3    = A2*H1[1]-B2*H1[0]+A[2]         ;
    double A4    = B3*H1[2]-C3*H1[1]+A[0]         ; 
    double B4    = C3*H1[0]-A3*H1[2]+A[1]         ; 
    double C4    = A3*H1[1]-B3*H1[0]+A[2]         ;
    double A5    = A4-A[0]+A4                     ; 
    double B5    = B4-A[1]+B4                     ; 
    double C5    = C4-A[2]+C4                     ;
    
    // Last point
    //
    double gP2[3]={R[0]+S*A4, R[1]+S*B4, R[2]+S*C4};    
    getFieldGradient(cache,gP2,f2,g2);

    H2[0] = f2[0]*PS2; H2[1] = f2[1]*PS2; H2[2] = f2[2]*PS2; 
    double A6    = B5*H2[2]-C5*H2[1]              ;
    double B6    = C5*H2[0]-A5*H2[2]              ;
    double C6    = A5*H2[1]-B5*H2[0]              ;

    
    // Test approximation quality on give step and possible step reduction
    /*
    double dE[4] = {(A1+A6)-(A3+A4),(B1+B6)-(B3+B4),(C1+C6)-(C3+C4),S};
    double cS    = stepReduction(dE);
    if     (cSn <  1.) {S*=cS; continue;}
    cSn == 1. InS = false : InS = true;
    */
    
    // Test approximation quality on give step and possible step reduction
    //
    double EST = fabs((A1+A6)-(A3+A4))+fabs((B1+B6)-(B3+B4))+fabs((C1+C6)-(C3+C4)); 
    if(EST>m_dlt) {S*=.5; dltm = 0.; continue;} EST<dltm ? InS = true : InS = false;

    // Parameters calculation
    //   
    const double A00 = A[0];
    const double A11=A[1];
    const double A22=A[2];
    R[0]+=(A2+A3+A4)*S3; A[0] = ((A0+2.*A3)+(A5+A6))*C33;
    R[1]+=(B2+B3+B4)*S3; A[1] = ((B0+2.*B3)+(B5+B6))*C33;
    R[2]+=(C2+C3+C4)*S3; A[2] = ((C0+2.*C3)+(C5+C6))*C33;
    double CBA = 1./sqrt(A[0]*A[0]+A[1]*A[1]+A[2]*A[2]);
    A[0]*=CBA; A[1]*=CBA; A[2]*=CBA;
 
    const double Sl = 2./S;  
    sA[0] = A6*Sl; 
    sA[1] = B6*Sl;
    sA[2] = C6*Sl; 

    // Jacobian calculation
    //
    for(int i=3; i!=12; ++i) {H0[i]=g0[i-3]*PS2; H1[i]=g1[i-3]*PS2; H2[i]=g2[i-3]*PS2;}

    for(int i=7; i<35; i+=7) {

      double* dR   = &P[i]                                          ;  
      double* dA   = &P[i+3]                                        ;
      double dH0   = H0[ 3]*dR[0]+H0[ 4]*dR[1]+H0[ 5]*dR[2]         ; // dHx/dp
      double dH1   = H0[ 6]*dR[0]+H0[ 7]*dR[1]+H0[ 8]*dR[2]         ; // dHy/dp
      double dH2   = H0[ 9]*dR[0]+H0[10]*dR[1]+H0[11]*dR[2]         ; // dHz/dp
     
      const double dA0   =(H0[ 2]*dA[1]-H0[ 1]*dA[2])+(A[1]*dH2-A[2]*dH1) ; // dA0/dp
      const double dB0   =(H0[ 0]*dA[2]-H0[ 2]*dA[0])+(A[2]*dH0-A[0]*dH2) ; // dB0/dp
      const double dC0   =(H0[ 1]*dA[0]-H0[ 0]*dA[1])+(A[0]*dH1-A[1]*dH0) ; // dC0/dp
      const double dA2   = dA0+dA[0];
      double dX = dR[0]+(dA2+dA[0])*S4           ; // dX /dp
      const double dB2   = dB0+dA[1];
      double dY = dR[1]+(dB2+dA[1])*S4           ; // dY /dp
      const double dC2   = dC0+dA[2];
      double dZ = dR[2]+(dC2+dA[2])*S4           ; // dZ /dp
      dH0          = H1[ 3]*dX   +H1[ 4]*dY   +H1[ 5]*dZ            ; // dHx/dp
      dH1          = H1[ 6]*dX   +H1[ 7]*dY   +H1[ 8]*dZ            ; // dHy/dp
      dH2          = H1[ 9]*dX   +H1[10]*dY   +H1[11]*dZ            ; // dHz/dp
     
      const double dA3   =(dA[0]+dB2*H1[2]-dC2*H1[1])+(B2*dH2-C2*dH1)     ; // dA3/dp
      const double dB3   =(dA[1]+dC2*H1[0]-dA2*H1[2])+(C2*dH0-A2*dH2)     ; // dB3/dp
      const double dC3   =(dA[2]+dA2*H1[1]-dB2*H1[0])+(A2*dH1-B2*dH0)     ; // dC3/dp
      const double dA4   =(dA[0]+dB3*H1[2]-dC3*H1[1])+(B3*dH2-C3*dH1)     ; // dA4/dp
      const double dB4   =(dA[1]+dC3*H1[0]-dA3*H1[2])+(C3*dH0-A3*dH2)     ; // dB4/dp
      const double dC4   =(dA[2]+dA3*H1[1]-dB3*H1[0])+(A3*dH1-B3*dH0)     ; // dC4/dp
      const double dA5   = dA4+dA4-dA[0];  dX = dR[0]+dA4*S               ; // dX /dp 
      const double dB5   = dB4+dB4-dA[1];  dY = dR[1]+dB4*S               ; // dY /dp
      const double dC5   = dC4+dC4-dA[2];  dZ = dR[2]+dC4*S               ; // dZ /dp
      dH0          = H2[ 3]*dX   +H2[ 4]*dY   +H2[ 5]*dZ                  ; // dHx/dp
      dH1          = H2[ 6]*dX   +H2[ 7]*dY   +H2[ 8]*dZ                  ; // dHy/dp
      dH2          = H2[ 9]*dX   +H2[10]*dY   +H2[11]*dZ                  ; // dHz/dp
      
      const double dA6   =(dB5*H2[2]-dC5*H2[1])+(B5*dH2-C5*dH1)           ; // dA6/dp
      const double dB6   =(dC5*H2[0]-dA5*H2[2])+(C5*dH0-A5*dH2)           ; // dB6/dp
      const double dC6   =(dA5*H2[1]-dB5*H2[0])+(A5*dH1-B5*dH0)           ; // dC6/dp
      dR[0]+=(dA2+dA3+dA4)*S3; dA[0]=((dA0+2.*dA3)+(dA5+dA6))*C33   ;      
      dR[1]+=(dB2+dB3+dB4)*S3; dA[1]=((dB0+2.*dB3)+(dB5+dB6))*C33   ; 
      dR[2]+=(dC2+dC3+dC4)*S3; dA[2]=((dC0+2.*dC3)+(dC5+dC6))*C33   ;
    }

    double* dR   = &P[35]                                                ;  
    double* dA   = &P[38]                                                ;
      
    double dH0   = H0[ 3]*dR[0]+H0[ 4]*dR[1]+H0[ 5]*dR[2]                ; // dHx/dp
    double dH1   = H0[ 6]*dR[0]+H0[ 7]*dR[1]+H0[ 8]*dR[2]                ; // dHy/dp
    double dH2   = H0[ 9]*dR[0]+H0[10]*dR[1]+H0[11]*dR[2]                ; // dHz/dp
    const double dA0   =(H0[ 2]*dA[1]-H0[ 1]*dA[2])+(A[1]*dH2-A[2]*dH1+A0)     ; // dA0/dp
    const double dB0   =(H0[ 0]*dA[2]-H0[ 2]*dA[0])+(A[2]*dH0-A[0]*dH2+B0)     ; // dB0/dp
    const double dC0   =(H0[ 1]*dA[0]-H0[ 0]*dA[1])+(A[0]*dH1-A[1]*dH0+C0)     ; // dC0/dp
    const double dA2   = dA0+dA[0];
    double dX = dR[0]+(dA2+dA[0])*S4                  ; // dX /dp
    const double dB2   = dB0+dA[1];
    double dY = dR[1]+(dB2+dA[1])*S4                  ; // dY /dp
    const double dC2   = dC0+dA[2];
    double dZ = dR[2]+(dC2+dA[2])*S4                  ; // dZ /dp
    dH0          = H1[ 3]*dX   +H1[ 4]*dY   +H1[ 5]*dZ                   ; // dHx/dp
    dH1          = H1[ 6]*dX   +H1[ 7]*dY   +H1[ 8]*dZ                   ; // dHy/dp
    dH2          = H1[ 9]*dX   +H1[10]*dY   +H1[11]*dZ                   ; // dHz/dp
    
    const double dA3   =(dA[0]+dB2*H1[2]-dC2*H1[1])+((B2*dH2-C2*dH1)+(A3-A00)) ; // dA3/dp
    const double dB3   =(dA[1]+dC2*H1[0]-dA2*H1[2])+((C2*dH0-A2*dH2)+(B3-A11)) ; // dB3/dp
    const double dC3   =(dA[2]+dA2*H1[1]-dB2*H1[0])+((A2*dH1-B2*dH0)+(C3-A22)) ; // dC3/dp
    const double dA4   =(dA[0]+dB3*H1[2]-dC3*H1[1])+((B3*dH2-C3*dH1)+(A4-A00)) ; // dA4/dp
    const double dB4   =(dA[1]+dC3*H1[0]-dA3*H1[2])+((C3*dH0-A3*dH2)+(B4-A11)) ; // dB4/dp
    const double dC4   =(dA[2]+dA3*H1[1]-dB3*H1[0])+((A3*dH1-B3*dH0)+(C4-A22)) ; // dC4/dp
    const double dA5   = dA4+dA4-dA[0];  dX = dR[0]+dA4*S                      ; // dX /dp 
    const double dB5   = dB4+dB4-dA[1];  dY = dR[1]+dB4*S                      ; // dY /dp
    const double dC5   = dC4+dC4-dA[2];  dZ = dR[2]+dC4*S                      ; // dZ /dp
    dH0          = H2[ 3]*dX   +H2[ 4]*dY   +H2[ 5]*dZ                   ; // dHx/dp
    dH1          = H2[ 6]*dX   +H2[ 7]*dY   +H2[ 8]*dZ                   ; // dHy/dp
    dH2          = H2[ 9]*dX   +H2[10]*dY   +H2[11]*dZ                   ; // dHz/dp
    
    const double dA6   =(dB5*H2[2]-dC5*H2[1])+(B5*dH2-C5*dH1+A6)               ; // dA6/dp
    const double dB6   =(dC5*H2[0]-dA5*H2[2])+(C5*dH0-A5*dH2+B6)               ; // dB6/dp
    const double dC6   =(dA5*H2[1]-dB5*H2[0])+(A5*dH1-B5*dH0+C6)               ; // dC6/dp
   
    dR[0]+=(dA2+dA3+dA4)*S3; dA[0]=((dA0+2.*dA3)+(dA5+dA6))*C33          ;      
    dR[1]+=(dB2+dB3+dB4)*S3; dA[1]=((dB0+2.*dB3)+(dB5+dB6))*C33          ; 
    dR[2]+=(dC2+dC3+dC4)*S3; dA[2]=((dC0+2.*dC3)+(dC5+dC6))*C33          ;
    return S;
  }
  return S;
}

/////////////////////////////////////////////////////////////////////////////////
// Straight line trajectory model 
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaPropagator::straightLineStep
(bool    Jac,
 double  S  , 
 double* P  ) const
{
  double*  R   = &P[ 0];             // Start coordinates
  double*  A   = &P[ 3];             // Start directions
  double* sA   = &P[42];           

  // Track parameters in last point
  //
  R[0]+=(A[0]*S); R[1]+=(A[1]*S); R[2]+=(A[2]*S); if(!Jac) return S;
  
  // Derivatives of track parameters in last point
  //
  for(int i=7; i<42; i+=7) {

    double* dR = &P[i]; 
    double* dA = &P[i+3];
    dR[0]+=(dA[0]*S); dR[1]+=(dA[1]*S); dR[2]+=(dA[2]*S);
  }
  sA[0]=sA[1]=sA[2]=0.; return S;
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for simple track parameters and covariance matrix propagation
// Ta->Su = Tb
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaPropagator::propagate
(const ::EventContext&               ctx,
 Trk::PatternTrackParameters  & Ta,
 const Trk::Surface           & Su,
 Trk::PatternTrackParameters  & Tb,
 Trk::PropDirection             D ,
 const MagneticFieldProperties& M , 
 ParticleHypothesis               ) const 
{
  double S;
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);
 
  cache.m_maxPath = 10000.;
  return propagateRungeKutta(cache,true,Ta,Su,Tb,D,M,S);
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for simple track parameters and covariance matrix propagation
// Ta->Su = Tb with step to surface calculation
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaPropagator::propagate
(const ::EventContext&               ctx,
 Trk::PatternTrackParameters  & Ta,
 const Trk::Surface           & Su,
 Trk::PatternTrackParameters  & Tb,
 Trk::PropDirection             D ,
 const MagneticFieldProperties& M , 
 double                       & S , 
 ParticleHypothesis               ) const 
{
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_maxPath = 10000.; 
  return propagateRungeKutta(cache,true,Ta,Su,Tb,D,M,S);
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for simple track parameters propagation without covariance matrix
// Ta->Su = Tb
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaPropagator::propagateParameters
(const ::EventContext&               ctx,
 Trk::PatternTrackParameters  & Ta,
 const Trk::Surface           & Su,
 Trk::PatternTrackParameters  & Tb, 
 Trk::PropDirection             D ,
 const MagneticFieldProperties& M ,
 ParticleHypothesis               ) const 
{
  double S;
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  cache.m_maxPath = 10000.; 
  return propagateRungeKutta(cache,false,Ta,Su,Tb,D,M,S);
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for simple track parameters propagation without covariance matrix
// Ta->Su = Tb with step calculation
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaPropagator::propagateParameters
(const ::EventContext&               ctx,
Trk::PatternTrackParameters  & Ta,
 const Trk::Surface           & Su,
 Trk::PatternTrackParameters  & Tb, 
 Trk::PropDirection             D ,
 const MagneticFieldProperties& M ,
 double                       & S , 
 ParticleHypothesis               ) const 
{
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);
 
  cache.m_maxPath = 10000.;
  return propagateRungeKutta(cache,false,Ta,Su,Tb,D,M,S);
}

/////////////////////////////////////////////////////////////////////////////////
// Global positions calculation inside CylinderBounds
// where mS - max step allowed if mS > 0 propagate along    momentum
//                                mS < 0 propogate opposite momentum
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaPropagator::globalPositions 
(const ::EventContext&               ctx,
 std::list<Amg::Vector3D>         & GP,
 const Trk::PatternTrackParameters& Tp,
 const MagneticFieldProperties    & M,
 const CylinderBounds             & CB,
 double                             mS,
 ParticleHypothesis                   ) const
{
  double P[45]; if(!Trk::RungeKuttaUtils::transformLocalToGlobal(false,Tp,P)) return;

  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);
 
  cache.m_direction = fabs(mS);
  if(mS > 0.) globalOneSidePositions(cache,GP,P,M,CB, mS);
  else        globalTwoSidePositions(cache,GP,P,M,CB,-mS);
}

/////////////////////////////////////////////////////////////////////////////////
// GlobalPostions and steps for set surfaces
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaPropagator::globalPositions
(
 const ::EventContext&               ctx,
 const PatternTrackParameters                 & Tp,
 std::list<const Trk::Surface*>               & SU,
 std::list< std::pair<Amg::Vector3D,double> > & GP,
 const Trk::MagneticFieldProperties           & M ,
 ParticleHypothesis                               ) const
{
  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx); 

  cache.m_direction               = 0.    ;
  cache.m_mcondition              = false ;
  cache.m_maxPath                 = 10000.;
  cache.m_needgradient = false;
  M.magneticFieldMode() == Trk::FastField ? cache.m_solenoid     = true : cache.m_solenoid     = false;  
  M.magneticFieldMode() != Trk::NoField   ? cache.m_mcondition   = true : cache.m_mcondition   = false;


  double Step = 0.;
  double P[64]; if(!Trk::RungeKuttaUtils::transformLocalToGlobal(false,Tp,P)) return;


  std::list<const Trk::Surface*>::iterator su = SU.begin();
  std::list<const Trk::Surface*>::iterator sue = SU.end();

  // Loop trough all input surfaces
  //
  for(; su!=sue; ++su) {

    const Amg::Transform3D&  T = (*su)->transform();  
    int ty = (*su)->type(); 
   
    if( ty == Trk::Surface::Plane ) {

      double s[4];
      double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2);

      if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;}
      else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;}
      if(!propagateWithJacobian(cache,false,1,s,P,Step)) return;
    }
    else if(ty == Trk::Surface::Line ) {

      double s[6]={T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2)};
      if(!propagateWithJacobian(cache,false,0,s,P,Step)) return;
    }
    else if (ty == Trk::Surface::Disc     ) {

      double s[4];
      double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2);
      if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;}
      else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;}
      if(!propagateWithJacobian(cache,false,1,s,P,Step)) return;
    }
    else if (ty == Trk::Surface::Cylinder ) {

      const Trk::CylinderSurface* cyl = static_cast<const Trk::CylinderSurface*>(*su);
      double r0[3] = {P[0],P[1],P[2]};
      double s [9] = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),cyl->bounds().r(),cache.m_direction,0.};

      if(!propagateWithJacobian(cache,false,2,s,P,Step)) return;

      // For cylinder we do test for next cross point
      //
      if(cyl->bounds().halfPhiSector() < 3.1 && newCrossPoint(*cyl,r0,P)) {
        s[8] = 0.; if(!propagateWithJacobian(cache,false,2,s,P,Step)) return;
      }
    }
    else if (ty == Trk::Surface::Perigee  ) {

      double s[6]={T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2)};
      if(!propagateWithJacobian(cache,false,0,s,P,Step)) return ;
    }
    else if (ty == Trk::Surface::Cone     ) {
    
      double k     = static_cast<const Trk::ConeSurface*>(*su)->bounds().tanAlpha(); k = k*k+1.;
      double s[9]  = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),k,cache.m_direction,0.};
      if(!propagateWithJacobian(cache,false,3,s,P,Step)) return;
    }
    else return;

    if(cache.m_maxPathLimit) return;

    Amg::Vector3D gp(P[0],P[1],P[2]); GP.emplace_back(gp,Step);
  }
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for simple track propagation with or without jacobian
// Ta->Su = Tb for pattern track parameters
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaPropagator::propagateRungeKutta
(Cache&                         cache,
 bool                           useJac,
 Trk::PatternTrackParameters  & Ta    ,
 const Trk::Surface           & Su    ,
 Trk::PatternTrackParameters  & Tb    ,
 Trk::PropDirection             D     ,
 const MagneticFieldProperties& M     ,
 double                       & Step  ) const 
{  
  const Trk::Surface* su = &Su; if(!su) return false;
  if(su == Ta.associatedSurface()) {Tb = Ta; return true;}

  cache.m_direction               = D ; 

  if(useJac && !Ta.iscovariance()) useJac = false;

  M.magneticFieldMode() == Trk::FastField  ? cache.m_solenoid     = true : cache.m_solenoid     = false;  
  (useJac && m_usegradient) ? cache.m_needgradient = true : cache.m_needgradient = false; 
  M.magneticFieldMode() != Trk::NoField    ? cache.m_mcondition   = true : cache.m_mcondition   = false;


  double P[45]; if(!Trk::RungeKuttaUtils::transformLocalToGlobal(useJac,Ta,P)) return false; Step = 0.;

  const Amg::Transform3D&  T = Su.transform();  
  int ty = Su.type(); 

  if      (ty == Trk::Surface::Plane    ) {

    double s[4];
    double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2);

    if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;}
    else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;}
    if(!propagateWithJacobian(cache,useJac,1,s,P,Step)) return false;
  }
    else if (ty == Trk::Surface::Line     ) {

    double s[6] ={T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2)};
    if(!propagateWithJacobian(cache,useJac,0,s,P,Step)) return false;
  }
  else if (ty == Trk::Surface::Disc     ) {

    double s[4];
    double d  = T(0,3)*T(0,2)+T(1,3)*T(1,2)+T(2,3)*T(2,2);

    if(d>=0.) {s[0]= T(0,2); s[1]= T(1,2); s[2]= T(2,2); s[3]= d;}
    else      {s[0]=-T(0,2); s[1]=-T(1,2); s[2]=-T(2,2); s[3]=-d;}
    if(!propagateWithJacobian(cache,useJac,1,s,P,Step)) return false;
  }
  else if (ty == Trk::Surface::Cylinder ) {

    const Trk::CylinderSurface* cyl = static_cast<const Trk::CylinderSurface*>(su);

    double r0[3] = {P[0],P[1],P[2]};
    double s [9] = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),cyl->bounds().r(),cache.m_direction,0.};

    if(!propagateWithJacobian(cache,useJac,2,s,P,Step)) return false;

    // For cylinder we do test for next cross point
    //
    if(cyl->bounds().halfPhiSector() < 3.1 && newCrossPoint(*cyl,r0,P)) {
      s[8] = 0.; if(!propagateWithJacobian(cache,useJac,2,s,P,Step)) return false;
    }
  }
  else if (ty == Trk::Surface::Perigee  ) {

    double s[6] ={T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2)};
    if(!propagateWithJacobian(cache,useJac,0,s,P,Step)) return false;
  }
  else if (ty == Trk::Surface::Cone     ) {
    
    double k     = static_cast<const Trk::ConeSurface*>(su)->bounds().tanAlpha(); k = k*k+1.;
    double s[9]  = {T(0,3),T(1,3),T(2,3),T(0,2),T(1,2),T(2,2),k,cache.m_direction,0.};
    if(!propagateWithJacobian(cache,useJac,3,s,P,Step)) return false;
  }
  else return false;

  if(cache.m_maxPathLimit || (cache.m_direction && (cache.m_direction*Step)<0.)) return false;

  // Common transformation for all surfaces (angles and momentum)
  //
  if(useJac) {
    double p=1./P[6]; P[35]*=p; P[36]*=p; P[37]*=p; P[38]*=p; P[39]*=p; P[40]*=p;
  }

  double p[5];
  double Jac[21]; Trk::RungeKuttaUtils::transformGlobalToLocal(su,useJac,P,p,Jac);

  // New simple track parameters production
  //
  Tb.setParameters(&Su,p); 
  if(useJac) {
    Tb.newCovarianceMatrix(Ta,Jac);
    const double* cv = Tb.cov();
    if( cv[0]<=0. || cv[2]<=0. || cv[5]<=0. || cv[9]<=0. || cv[14]<=0.) return false;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Test new cross point
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaPropagator::newCrossPoint
(const Trk::CylinderSurface& Su,
 const double              * Ro,
 const double              * P ) const
{
  const double pi = 3.1415927;
  const double pi2=2.*pi; 
  const Amg::Transform3D& T = Su.transform();
  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};

  double R     = Su.bounds().r();
  double x     = Ro[0]-T(0,3);
  double y     = Ro[1]-T(1,3);
  double z     = Ro[2]-T(2,3);

  double RC    = x*Ax[0]+y*Ax[1]+z*Ax[2];
  double RS    = x*Ay[0]+y*Ay[1]+z*Ay[2];

  if( (RC*RC+RS*RS) <= (R*R) ) return false;
  
  x           = P[0]-T(0,3);
  y           = P[1]-T(1,3);
  z           = P[2]-T(2,3);
  RC          = x*Ax[0]+y*Ax[1]+z*Ax[2];
  RS          = x*Ay[0]+y*Ay[1]+z*Ay[2];
  double dF   = fabs(atan2(RS,RC)-Su.bounds().averagePhi());
  if(dF > pi) dF = pi2-pi;
  return dF > Su.bounds().halfPhiSector();
}

/////////////////////////////////////////////////////////////////////////////////
// Build new track parameters without propagation
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters* Trk::RungeKuttaPropagator::buildTrackParametersWithoutPropagation
(const Trk::TrackParameters& Tp,double* Jac) const
{
  Jac[0]=Jac[6]=Jac[12]=Jac[18]=Jac[20]=1.;
  Jac[1]=Jac[2]=Jac[3]=Jac[4]=Jac[5]=Jac[7]=Jac[8]=Jac[9]=Jac[10]=Jac[11]=Jac[13]=Jac[14]=Jac[15]=Jac[16]=Jac[17]=Jac[19]=0.;
  return Tp.clone();
}

/////////////////////////////////////////////////////////////////////////////////
// Build new neutral track parameters without propagation
/////////////////////////////////////////////////////////////////////////////////

Trk::NeutralParameters* Trk::RungeKuttaPropagator::buildTrackParametersWithoutPropagation
(const Trk::NeutralParameters& Tp,double* Jac) const
{
  Jac[0]=Jac[6]=Jac[12]=Jac[18]=Jac[20]=1.;
  Jac[1]=Jac[2]=Jac[3]=Jac[4]=Jac[5]=Jac[7]=Jac[8]=Jac[9]=Jac[10]=Jac[11]=Jac[13]=Jac[14]=Jac[15]=Jac[16]=Jac[17]=Jac[19]=0.;
  return Tp.clone();
}

/////////////////////////////////////////////////////////////////////////////////
// Step estimator take into accout curvature
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaPropagator::stepEstimatorWithCurvature
(Cache& cache, int kind,double* Su,const double* P,bool& Q) const
{
  // Straight step estimation
  //
  double  Step = Trk::RungeKuttaUtils::stepEstimator(kind,Su,P,Q); if(!Q) return 0.; 
  double AStep = fabs(Step);
  if( kind || AStep < m_straightStep || !cache.m_mcondition ) return Step;

  const double* SA = &P[42]; // Start direction
  double S = .5*Step;
  
  double Ax    = P[3]+S*SA[0];
  double Ay    = P[4]+S*SA[1];
  double Az    = P[5]+S*SA[2];
  double As    = 1./sqrt(Ax*Ax+Ay*Ay+Az*Az);

  double PN[6] = {P[0],P[1],P[2],Ax*As,Ay*As,Az*As};
  double StepN = Trk::RungeKuttaUtils::stepEstimator(kind,Su,PN,Q); if(!Q) {Q = true; return Step;}
  if(fabs(StepN) < AStep) return StepN;
  return Step;
} 

/////////////////////////////////////////////////////////////////////////////////
// Global positions calculation inside CylinderBounds (one side)
// where mS - max step allowed if mS > 0 propagate along    momentum
//                                mS < 0 propogate opposite momentum
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaPropagator::globalOneSidePositions 
(Cache &                         cache,  
 std::list<Amg::Vector3D>      & GP,
 const double                  * P,
 const MagneticFieldProperties & M ,
 const CylinderBounds          & CB,
 double                          mS,
 ParticleHypothesis                ) const
{
  M.magneticFieldMode() == Trk::FastField ? cache.m_solenoid   = true : cache.m_solenoid  = false;  
  M.magneticFieldMode() != Trk::NoField   ? cache.m_mcondition = true : cache.m_mcondition = false;

  double Pm[45]; for(int i=0; i!=7; ++i) Pm[i]=P[i];

  double       W     = 0.                 ; // way
  double       R2max = CB.r()*CB.r()      ; // max. radius**2 of region
  double       Zmax  = CB.halflengthZ()   ; // max. Z         of region
  double       R2    = P[0]*P[0]+P[1]*P[1]; // Start radius**2
  double       Dir   = P[0]*P[3]+P[1]*P[4]; // Direction
  double       S     = mS                 ; // max step allowed
  double       R2m   = R2                 ;

  if(cache.m_mcondition && fabs(P[6]) > .1) return; 

  // Test position of the track  
  //
  if(fabs(P[2]) > Zmax || R2 > R2max) return;

  Amg::Vector3D g0(P[0],P[1],P[2]); GP.push_back(g0);

  bool   per = false; if(fabs(Dir)<.00001) per = true;
  bool   InS = false;

  int niter = 0;
  int sm    = 1;
  for(int s = 0; s!=2; ++s) {

    cache.m_newfield = true;

    if(s) {if(per) break; S = -S;}
    double p[45]; for(int i=0; i!=7; ++i) p[i]=P[i]; p[42]=p[43]=p[44]=0.;

    while(W<100000. && ++niter < 1000) {
      
      if(cache.m_mcondition) {
	W+=(S=rungeKuttaStep  (cache,0,S,p,InS)); 
      }
      else {
	W+=(S=straightLineStep(0,    S,p)); 
      }       
      if(InS && fabs(2.*S)<mS) S*=2.;

      Amg::Vector3D g(p[0],p[1],p[2]); 
      if(!s) GP.push_back(g); else GP.push_front(g);
   
      // Test position of the track  
      //
      R2 = p[0]*p[0]+p[1]*p[1];

      if(R2 < R2m) {
	Pm[0]=p[0]; Pm[1]=p[1]; Pm[2]=p[2]; 
	Pm[3]=p[3]; Pm[4]=p[4]; Pm[5]=p[5]; R2m = R2; sm = s;
      }
      if(fabs(p[2]) > Zmax || R2 > R2max) break;
      if(!s && P[3]*p[3]+P[4]*p[4] < 0. ) break;

      // Test perigee 
      //
      if((p[0]*p[3]+p[1]*p[4])*Dir < 0.) {
	if(s) break;
        per = true;
      }
    }
  }

  if(R2m < 400.) return;

  // Propagate to perigee
  //
  Pm[42]=Pm[43]=Pm[44]=0.;
  per = false;

  for(int s = 0; s!=3; ++s) {

    cache.m_newfield = true;

    double A = (1.-Pm[5])*(1.+Pm[5]); if(A==0.) break;
    S        = -(Pm[0]*Pm[3]+Pm[1]*Pm[4])/A;
    if(fabs(S) < 1. || ++niter > 1000) break;

    if(cache.m_mcondition) {
      W+=rungeKuttaStep(cache,0,S,Pm,InS); 
    }
    else {
      W+=straightLineStep(0,S,Pm); 
    }       
    per = true;
  }

  if(per) {
    if(sm) {Amg::Vector3D gf(Pm[0],Pm[1],Pm[2]); GP.front() = gf;}
    else   {Amg::Vector3D gf(Pm[0],Pm[1],Pm[2]); GP.back () = gf;} 
  }
  else   {
    double x = GP.front().x() ;
    double y = GP.front().y();
    if( (x*x+y*y) > (Pm[0]*Pm[0]+Pm[1]*Pm[1]) ) {
     if(sm) GP.pop_front();
     else   GP.pop_back ();
    }
  }
  }

/////////////////////////////////////////////////////////////////////////////////
// Global positions calculation inside CylinderBounds (one side)
// where mS - max step allowed if mS > 0 propagate along    momentum
//                                mS < 0 propogate opposite momentum
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaPropagator::globalTwoSidePositions 
(Cache&                        cache,
 std::list<Amg::Vector3D>      & GP,
 const double                  * P,
 const MagneticFieldProperties & M ,
 const CylinderBounds          & CB,
 double                          mS,
 ParticleHypothesis                ) const
{
  M.magneticFieldMode() == Trk::FastField ? cache.m_solenoid   = true : cache.m_solenoid   = false;  
  M.magneticFieldMode() != Trk::NoField   ? cache.m_mcondition = true : cache.m_mcondition = false;

  double       W     = 0.                 ; // way
  double       R2max = CB.r()*CB.r()      ; // max. radius**2 of region
  double       Zmax  = CB.halflengthZ()   ; // max. Z         of region
  double       R2    = P[0]*P[0]+P[1]*P[1]; // Start radius**2
  double       S     = mS                 ; // max step allowed

  if(cache.m_mcondition && fabs(P[6]) > .1) return; 

  // Test position of the track  
  //
  if(fabs(P[2]) > Zmax || R2 > R2max) return;

  Amg::Vector3D g0(P[0],P[1],P[2]); GP.push_back(g0);

  bool   InS = false;

  int niter = 0;
  for(int s = 0; s!=2; ++s) {

    cache.m_newfield = true;
    
    if(s) {S = -S;}
    double p[45]; for(int i=0; i!=7; ++i) p[i]=P[i]; p[42]=p[43]=p[44]=0.;

    while(W<100000. && ++niter < 1000) {
      
      if(cache.m_mcondition) {
	W+=(S=rungeKuttaStep  (cache,0,S,p,InS)); 
      }
      else {
	W+=(S=straightLineStep(0,    S,p)); 
      }       
      if(InS && fabs(2.*S)<mS) S*=2.;

      Amg::Vector3D g(p[0],p[1],p[2]); 
      if(!s) GP.push_back(g); else GP.push_front(g);
   
      // Test position of the track  
      //
      R2 = p[0]*p[0]+p[1]*p[1];
      if(fabs(p[2]) > Zmax || R2 > R2max) break;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////
// Track parameters in cross point preparation
/////////////////////////////////////////////////////////////////////////////////

Trk::TrackParameters* Trk::RungeKuttaPropagator::crossPoint
(const TrackParameters    & Tp,
 std::vector<DestSurf>    & SU,
 std::vector<unsigned int>& So,
 double                   * P ,
 std::pair<double,int>    & SN) const
{
  double*   R = &P[ 0]   ;  // Start coordinates
  double*   A = &P[ 3]   ;  // Start directions
  double*  SA = &P[42]   ;  // d(directions)/dStep
  double Step = SN.first ; 
  int      N  = SN.second; 

  double As[3];
  double Rs[3];
  
  As[0] = A[0]+SA[0]*Step; 
  As[1] = A[1]+SA[1]*Step;
  As[2] = A[2]+SA[2]*Step;
  double CBA = 1./sqrt(As[0]*As[0]+As[1]*As[1]+As[2]*As[2]);
  
  Rs[0] = R[0]+Step*(As[0]-.5*Step*SA[0]); As[0]*=CBA;
  Rs[1] = R[1]+Step*(As[1]-.5*Step*SA[1]); As[1]*=CBA;
  Rs[2] = R[2]+Step*(As[2]-.5*Step*SA[2]); As[2]*=CBA;

  Amg::Vector3D pos(Rs[0],Rs[1],Rs[2]);
  Amg::Vector3D dir(As[0],As[1],As[2]);

  Trk::DistanceSolution ds = SU[N].first->straightLineDistanceEstimate(pos,dir,SU[N].second);  
  if(ds.currentDistance(false) > .010) return nullptr;

  P[0] = Rs[0]; A[0] = As[0];
  P[1] = Rs[1]; A[1] = As[1];
  P[2] = Rs[2]; A[2] = As[2];

  So.push_back(N);

  // Transformation track parameters
  //
  bool useJac; Tp.covariance() ? useJac = true : useJac = false;

  if(useJac) {
    double d=1./P[6]; P[35]*=d; P[36]*=d; P[37]*=d; P[38]*=d; P[39]*=d; P[40]*=d;
  }
  double p[5];
  double Jac[25]; 
  Trk::RungeKuttaUtils::transformGlobalToLocal(SU[N].first,useJac,P,p,Jac);

  if(!useJac) return SU[N].first->createTrackParameters(p[0],p[1],p[2],p[3],p[4],nullptr); 

  AmgSymMatrix(5)* e  = Trk::RungeKuttaUtils::newCovarianceMatrix(Jac,*Tp.covariance());
  AmgSymMatrix(5)& cv = *e;

  if(cv(0,0)<=0. || cv(1,1)<=0. || cv(2,2)<=0. || cv(3,3)<=0. || cv(4,4)<=0.) {
    delete e; return nullptr;
  }
  return  SU[N].first->createTrackParameters(p[0],p[1],p[2],p[3],p[4],e);  
}

/////////////////////////////////////////////////////////////////////////////////
// Step reduction from STEP propagator
// Input information ERRx,ERRy,ERRz,current step   
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaPropagator::stepReduction(const double* E) const
{
  double dlt2  = m_dlt*m_dlt;
  double dR2   = (E[0]*E[0]+E[1]*E[1]+E[2]*E[2])*(E[3]*E[3]*4.); if(dR2 < 1.e-40) dR2 = 1.e-40;
  double cS    = std::pow(dlt2/dR2,0.125);
  if(cS < .25) cS = .25; 
  if((dR2 > 16.*dlt2 && cS < 1.) || cS >=3.) return cS;
  return 1.;
}

/////////////////////////////////////////////////////////////////////////////////
// Simple propagation on Step 
// Ri and Pi - initial coordinate and momentum
// Ro and Po - output  coordinate and momentum after propagation
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaPropagator::propagateStep(const EventContext& ctx,
                                              const Amg::Vector3D& Ri,const  Amg::Vector3D& Pi,
                                              double Charge,double Step,
                                              Amg::Vector3D& Ro, Amg::Vector3D& Po, 
                                              const MagneticFieldProperties& Mag) const
{

  Cache cache{};

  // Get field cache object
  getFieldCacheObject(cache, ctx);

  // Magnetic field information preparation
  //
  Mag.magneticFieldMode() == Trk::FastField ? cache.m_solenoid   = true : cache.m_solenoid   = false;  
  Mag.magneticFieldMode() != Trk::NoField   ? cache.m_mcondition = true : cache.m_mcondition = false;

  double M  = sqrt(Pi[0]*Pi[0]+Pi[1]*Pi[1]+Pi[2]*Pi[2]); if(M < .00001) {Ro = Ri; Po = Pi; return;}
  double Mi = 1./M;
  
  double P[45];
  P[0] =    Ri[0]; P[1]=    Ri[1]; P[2] =    Ri[2];
  P[3] = Mi*Pi[0]; P[4]= Mi*Pi[1]; P[5] = Mi*Pi[2]; P[6] = Charge*Mi;

  bool  In = false;
  double S = Step ;
  double W = 0.   ;

  cache.m_newfield = true;

  while (true) {

    if(cache.m_mcondition) {W+=(S=rungeKuttaStep  (cache,false,S,P,In));}
    else             {W+=(S=straightLineStep(false,S,P   ));}

    double ws  = Step-W  ;
    double wsa = fabs(ws);
    if(wsa < .0001 || ws*Step < 0.) break;

    double Sa = fabs(S);
    if     (Sa  > wsa          ) S = ws;
    else if(In && wsa >= 2.*Sa ) S*= 2.; 
  }
  Ro[0] =   P[0];  Ro[1] =   P[1];  Ro[2] =   P[2];
  Po[0] = M*P[3];  Po[1] = M*P[4];  Po[2] = M*P[5];
}


void Trk::RungeKuttaPropagator::getFieldCacheObject( Cache& cache,const EventContext& ctx) const
{

    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
    if (fieldCondObj == nullptr) {

        // temporarily protect for when cache a cannot be retrieved in an IOV callback
        ATH_MSG_ERROR("extrapolate: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key()
                      << ". Skipping use of field cache!");

        // ATH_MSG_ERROR("extrapolate: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
        // return;
    }
    fieldCondObj->getInitializedCache (cache.m_fieldCache);

}


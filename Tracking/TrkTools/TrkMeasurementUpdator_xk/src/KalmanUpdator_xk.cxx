/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class Trk::KalmanUpdator_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 07/06/2005 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "TrkMeasurementUpdator_xk/KalmanUpdator_xk.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkPatternParameters/PatternTrackParameters.h"
#include "TrkParameters/TrackParameters.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

Trk::KalmanUpdator_xk::KalmanUpdator_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p)
{
  mapKeyProduction();
  m_covBoundary = 1.;

  m_cov0.push_back(  10.);
  m_cov0.push_back(  10.);
  m_cov0.push_back( .025);
  m_cov0.push_back( .025);
  m_cov0.push_back(.0001);

  declareInterface<IUpdator>( this );
  declareInterface<IPatternParametersUpdator>( this );
  declareProperty("InitialCovariances",m_cov0);
  declareProperty("BoundaryCovariances",m_covBoundary);
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

Trk::KalmanUpdator_xk::~KalmanUpdator_xk()
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode Trk::KalmanUpdator_xk::initialize()
{
  // init message stream
  //
  msg(MSG::INFO) << "initialize() successful in " << name() << endreq;

  if( m_cov0.size()!=5) {
    
      m_cov0.erase(m_cov0.begin(),m_cov0.end());
      m_cov0.push_back(  10.);
      m_cov0.push_back(  10.);
      m_cov0.push_back( .025);
      m_cov0.push_back( .025);
      m_cov0.push_back(.0001);
  }
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode Trk::KalmanUpdator_xk::finalize()
{
   return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Add local position together with erro matrix  without Xi2 calculation
///////////////////////////////////////////////////////////////////
    
const Trk::TrackParameters* Trk::KalmanUpdator_xk::addToState 
(const Trk::TrackParameters& T,
 const Amg::Vector2D       & P,
 const Amg::MatrixX        & E) const
{
  Trk::FitQualityOnSurface* Q=0; 
  return update(T,P,E,Q, 1,false);
}

///////////////////////////////////////////////////////////////////

bool                        Trk::KalmanUpdator_xk::addToState 
(Trk::PatternTrackParameters& T,
 const Amg::Vector2D        & P,
 const Amg::MatrixX         & E,
 Trk::PatternTrackParameters& Ta) const
{
  int    n ; 
  double x2;
  return update(T,P,E, 1,false,Ta,x2,n);
}

///////////////////////////////////////////////////////////////////

bool                        Trk::KalmanUpdator_xk::addToStateOneDimension  
(Trk::PatternTrackParameters& T,
 const Amg::Vector2D        & P,
 const Amg::MatrixX         & E,
 Trk::PatternTrackParameters& Ta) const
{
  double x2;
  return updateOneDimension(T,P,E, 1,false,Ta,x2);
}

///////////////////////////////////////////////////////////////////
// Remove local position together with erro matrix  without Xi2 calculation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* Trk::KalmanUpdator_xk::removeFromState 
(const Trk::TrackParameters& T,
 const Amg::Vector2D       & P,
 const Amg::MatrixX        & E) const
{
  Trk::FitQualityOnSurface* Q=0; 
  return update(T,P,E,Q,-1,false);
}

///////////////////////////////////////////////////////////////////

bool                        Trk::KalmanUpdator_xk::removeFromState 
(Trk::PatternTrackParameters& T,
 const Amg::Vector2D        & P,
 const Amg::MatrixX         & E,
 Trk::PatternTrackParameters& Ta) const
{
  int    n ; 
  double x2;
  return update(T,P,E,-1,false,Ta,x2,n);
}

///////////////////////////////////////////////////////////////////
// Add local parameters together with error matrix  without Xi2 calculation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* Trk::KalmanUpdator_xk::addToState 
(const Trk::TrackParameters& T,
 const Trk::LocalParameters& P,
 const Amg::MatrixX        & E) const
{
  Trk::FitQualityOnSurface* Q=0; 
  return update(T,P,E,Q, 1,false);
}

///////////////////////////////////////////////////////////////////

bool                        Trk::KalmanUpdator_xk::addToState 
(Trk::PatternTrackParameters& T,
 const LocalParameters      & P,
 const Amg::MatrixX         & E,
 Trk::PatternTrackParameters& Ta) const
{
  int    n ; 
  double x2;
  return update(T,P,E, 1,false,Ta,x2,n);
}

///////////////////////////////////////////////////////////////////
// Remove local parameters together with error matrix  without Xi2 calculation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* Trk::KalmanUpdator_xk::removeFromState 
(const Trk::TrackParameters& T,
 const Trk::LocalParameters& P,
 const Amg::MatrixX        & E) const
{
  Trk::FitQualityOnSurface* Q=0; 
  return update(T,P,E,Q,-1,false);
}

///////////////////////////////////////////////////////////////////

bool                       Trk::KalmanUpdator_xk::removeFromState 
(Trk::PatternTrackParameters& T,
 const LocalParameters      & P,
 const Amg::MatrixX         & E,
 Trk::PatternTrackParameters& Ta) const
{
  int    n ; 
  double x2;
  return update(T,P,E,-1,false,Ta,x2,n);
}

///////////////////////////////////////////////////////////////////
// Add local position together with error matrix  with Xi2 calculation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* Trk::KalmanUpdator_xk::addToState 
(const Trk::TrackParameters& T,
 const Amg::Vector2D       & P,
 const Amg::MatrixX        & E,
 Trk::FitQualityOnSurface* & Q) const
{
  return update(T,P,E,Q, 1,true);
}

///////////////////////////////////////////////////////////////////

bool                        Trk::KalmanUpdator_xk::addToState 
(Trk::PatternTrackParameters& T ,
 const Amg::Vector2D        & P ,
 const Amg::MatrixX         & E ,
 Trk::PatternTrackParameters& Ta,
 double                     & Q ,
 int                        & N ) const
{
  return update(T,P,E, 1,true,Ta,Q,N);
}

///////////////////////////////////////////////////////////////////

bool                        Trk::KalmanUpdator_xk::addToStateOneDimension  
(Trk::PatternTrackParameters& T ,
 const Amg::Vector2D        & P ,
 const Amg::MatrixX         & E ,
 Trk::PatternTrackParameters& Ta,
 double                     & Q ,
 int                        & N ) const
{
  N = 1;
  return updateOneDimension(T,P,E, 1,true,Ta,Q);
}


///////////////////////////////////////////////////////////////////
// Remove local position together with error matrix  with Xi2 calculation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* Trk::KalmanUpdator_xk::removeFromState 
(const Trk::TrackParameters& T,
 const Amg::Vector2D       & P,
 const Amg::MatrixX        & E,
 Trk::FitQualityOnSurface* & Q) const
{
  return update(T,P,E,Q,-1,true);
}

///////////////////////////////////////////////////////////////////

bool                        Trk::KalmanUpdator_xk::removeFromState 
(Trk::PatternTrackParameters& T ,
 const Amg::Vector2D        & P ,
 const Amg::MatrixX         & E ,
 Trk::PatternTrackParameters& Ta,
 double                     & Q ,
 int                        & N ) const
{
  return update(T,P,E,-1,true,Ta,Q,N);
}

///////////////////////////////////////////////////////////////////
// Add local parameters together with error matrix  with Xi2 calculation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* Trk::KalmanUpdator_xk::addToState 
(const Trk::TrackParameters& T,
 const Trk::LocalParameters& P,
 const Amg::MatrixX        & E,
 Trk::FitQualityOnSurface* & Q) const
{
  return update(T,P,E,Q, 1,true);
}

///////////////////////////////////////////////////////////////////
// Add local parameters together with error matrix  with Xi2 calculation
///////////////////////////////////////////////////////////////////

std::pair<AmgVector(5),AmgSymMatrix(5)>* Trk::KalmanUpdator_xk::updateParameterDifference 
(const AmgVector(5)       & T ,
 const AmgSymMatrix(5)    & Et, 
 const Amg::VectorX       & P , 
 const Amg::MatrixX       & Ep, 
 const int                & K ,
 Trk::FitQualityOnSurface*& Q ,
 bool                       X) const
{
  Q = 0;

  // Conversion local parameters
  //
  int n = Ep.cols(); if(n==0 || n>5) return 0;
  double m[5],mv[15];

  m [ 0]=P (0  ); 
  mv[ 0]=Ep(0,0);

  if(n>1) {
    m [ 1]=P (1  );
    mv[ 1]=Ep(1,0); mv[ 2]=Ep(1,1);
  }
  if(n>2) {
    m [ 2]=P (2  );
    mv[ 3]=Ep(2,0); mv[ 4]=Ep(2,1); mv[ 5]=Ep(2,2);
  }
  if(n>3) {
    m [ 3]=P (3  );
    mv[ 6]=Ep(3,0); mv[ 7]=Ep(3,1); mv[ 8]=Ep(3,2); mv[ 9]=Ep(3,3);
  }
  if(n>4) {
    m [ 4]=P(4  );
    mv[10]=Ep(4,0); mv[11]=Ep(4,1); mv[12]=Ep(4,2); mv[13]=Ep(4,3); mv[14]=Ep(4,4);
  }

  // Conversion track parameters to updator presentation
  //
  double p [ 5] = {T(0),T(1),T(2),T(3),T(4)};
  double pv[15] = {Et(0,0),
		   Et(1,0),Et(1,1),
		   Et(2,0),Et(2,1),Et(2,2),
		   Et(3,0),Et(3,1),Et(3,2),Et(3,3),
		   Et(4,0),Et(4,1),Et(4,2),Et(4,3),Et(4,4)};
  
  bool update = false;
  double x2;
  if     (n==2 && K==3) {
    update = updateWithTwoDim(1,X,m,mv,p,pv,x2);
    if(update && X) Q = new Trk::FitQualityOnSurface(x2,2); 
  }
  else if(n==1 && K==1) {
    update = updateWithOneDim(1,X,m,mv,p,pv,x2);
    if(update && X) Q = new Trk::FitQualityOnSurface(x2,1); 
  }
  else                  {
    update = updateWithAnyDim(1,X,m,mv,p,pv,x2,n,K);
    if(update && X) Q = new Trk::FitQualityOnSurface(x2,n); 
  }
  if(!update) return 0;

  testAngles(p,pv); 

  AmgVector(5)    nT ; nT <<p[0],p[1],p[2],p[3],p[4];
  AmgSymMatrix(5) nEt; nEt<<
			 pv[ 0],pv[ 1],pv[ 3],pv[ 6],pv[10],
			 pv[ 1],pv[ 2],pv[ 4],pv[ 7],pv[11],
			 pv[ 3],pv[ 4],pv[ 5],pv[ 8],pv[12],
			 pv[ 6],pv[ 7],pv[ 8],pv[ 9],pv[13],
			 pv[10],pv[11],pv[12],pv[13],pv[14];

  return new std::pair<AmgVector(5),AmgSymMatrix(5)>(std::make_pair(nT,nEt));
} 

///////////////////////////////////////////////////////////////////

bool                        Trk::KalmanUpdator_xk::addToState 
(Trk::PatternTrackParameters& T ,
 const Trk::LocalParameters & P , 
 const Amg::MatrixX         & E ,
 Trk::PatternTrackParameters& Ta,
 double                     & Q ,
 int                        & N ) const
{
  return update(T,P,E, 1,true,Ta,Q,N);
}

///////////////////////////////////////////////////////////////////
// Remove local parameters together with error matrix  with Xi2 calculation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* Trk::KalmanUpdator_xk::removeFromState 
(const Trk::TrackParameters& T,
 const Trk::LocalParameters& P,
 const Amg::MatrixX        & E,
 Trk::FitQualityOnSurface* & Q) const
{
  return update(T,P,E,Q,-1,true);
}

///////////////////////////////////////////////////////////////////

bool                        Trk::KalmanUpdator_xk::removeFromState 
(Trk::PatternTrackParameters& T ,
 const Trk::LocalParameters & P ,
 const Amg::MatrixX         & E ,
 Trk::PatternTrackParameters& Ta,
 double                     & Q ,
 int                        & N ) const
{
  return update(T,P,E,-1,true,Ta,Q,N);
}

///////////////////////////////////////////////////////////////////
// Combine two state without Xi2 calculation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* Trk::KalmanUpdator_xk::combineStates   
(const Trk::TrackParameters& T1, const Trk::TrackParameters& T2) const
{
  double M[5],MV[15]; if(!trackParametersToUpdator(T1,M,MV)) return 0;
  double P[5],PV[15]; if(!trackParametersToUpdator(T2,P,PV)) return 0;

  double x2; double* m; double* mv; double* p; double* pv;
  if(MV[14] > PV[14]) {m = &M[0]; mv = &MV[0]; p = &P[0]; pv = &PV[0];}
  else                {m = &P[0]; mv = &PV[0]; p = &M[0]; pv = &MV[0];}

  if(updateWithFiveDim(false,m,mv,p,pv,x2)) {
    testAngles(p,pv); return updatorToTrackParameters(T1,p,pv);
  }
  return 0;
}

///////////////////////////////////////////////////////////////////

bool                        Trk::KalmanUpdator_xk::combineStates 
(Trk::PatternTrackParameters& T1,
 Trk::PatternTrackParameters& T2,
 Trk::PatternTrackParameters& T3) const
{
  double M[5],MV[15]; bool q1 = trackParametersToUpdator(T1,M,MV);
  double P[5],PV[15]; bool q2 = trackParametersToUpdator(T2,P,PV);

  if(q1 && q2) {

    double x2; double* m; double* mv; double* p; double* pv;
    if(MV[14] > PV[14]) {m = &M[0]; mv = &MV[0]; p = &P[0]; pv = &PV[0];}
    else                {m = &P[0]; mv = &PV[0]; p = &M[0]; pv = &MV[0];}

    if(updateWithFiveDim(false,m,mv,p,pv,x2)) {
      testAngles(p,pv);
      T3.setParametersWithCovariance(T1.associatedSurface(),p,pv);
      return true;
    }
    return false;
  }
  else if(q1) {T3=T1; return true;}
  else if(q2) {T3=T2; return true;}
  else return false;
}


///////////////////////////////////////////////////////////////////
// Combine two state with Xi2 calculation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* Trk::KalmanUpdator_xk::combineStates   
(const TrackParameters& T1, const TrackParameters& T2,
 FitQualityOnSurface*& Q) const
{
  double M[5],MV[15]; if(!trackParametersToUpdator(T1,M,MV)) return 0;
  double P[5],PV[15]; if(!trackParametersToUpdator(T2,P,PV)) return 0;

  double x2; double* m; double* mv; double* p; double* pv;
  if(MV[14] > PV[14]) {m = &M[0]; mv = &MV[0]; p = &P[0]; pv = &PV[0];}
  else                {m = &P[0]; mv = &PV[0]; p = &M[0]; pv = &MV[0];}

  if(updateWithFiveDim(true,m,mv,p,pv,x2)) {
    Q = new Trk::FitQualityOnSurface(x2,5);
    testAngles(p,pv); return updatorToTrackParameters(T1,p,pv);
  }
  return 0;
}

///////////////////////////////////////////////////////////////////

bool                        Trk::KalmanUpdator_xk::combineStates 
(Trk::PatternTrackParameters& T1,
 Trk::PatternTrackParameters& T2,
 Trk::PatternTrackParameters& T3,
 double                     & Q ) const
{

  double M[5],MV[15]; bool q1 = trackParametersToUpdator(T1,M,MV);
  double P[5],PV[15]; bool q2 = trackParametersToUpdator(T2,P,PV);

  if(q1 && q2) {

    double* m; double* mv; double* p; double* pv;
    if(MV[14] > PV[14]) {m = &M[0]; mv = &MV[0]; p = &P[0]; pv = &PV[0];}
    else                {m = &P[0]; mv = &PV[0]; p = &M[0]; pv = &MV[0];}

    if(updateWithFiveDim(true,m,mv,p,pv,Q)) {
      testAngles(p,pv);
      T3.setParametersWithCovariance(T1.associatedSurface(),p,pv);
      return true;
    }
    return false;
  }
  else if(q1) {T3=T1; Q=0; return true;}
  else if(q2) {T3=T2; Q=0; return true;}
  else return false;
}

///////////////////////////////////////////////////////////////////
// Xi2 of Local position
///////////////////////////////////////////////////////////////////
      
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdator_xk::predictedStateFitQuality 
(const Trk::TrackParameters& T,
 const Amg::Vector2D       & P,
 const Amg::MatrixX        & E) const
{
  const AmgSymMatrix(5)* v = T.covariance();
  if(!v) return 0;

  double t[5] = {T.parameters()[0],T.parameters()[1],
		 (*v)(0,0),(*v)(1,0),(*v)(1,1)};

  int N; double x2; 
  if(predictedStateFitQuality(t,P,E,N,x2)) return new Trk::FitQualityOnSurface(x2,N);
  return 0;
}

///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::predictedStateFitQuality 
(const Trk::PatternTrackParameters& T,
 const Amg::Vector2D              & P,
 const Amg::MatrixX               & E,
 int                              & N,
 double                           & X2) const
{
  if(!T.iscovariance()) {N = 0; return false;}

  double t[5] = {T.par()[0],T.par()[1],
		 T.cov()[0],T.cov()[1],T.cov()[2]};

  return predictedStateFitQuality(t,P,E,N,X2); 
}

///////////////////////////////////////////////////////////////////
// Xi2 of Local parameters
///////////////////////////////////////////////////////////////////

const Trk::FitQualityOnSurface* 
Trk::KalmanUpdator_xk::predictedStateFitQuality
(const Trk::TrackParameters& T,
 const Trk::LocalParameters& P,
 const Amg::MatrixX        & E) const
{
  // Conversion track parameters
  //
  double p[5],pv[15]; if(!trackParametersToUpdator(T,p,pv)) return 0;

  // Conversion local parameters
  //
  int n,k;
  double m[5],mv[15]; if(!localParametersToUpdator(P,E,n,k,m,mv)) return 0;

  // Xi2 calculation
  //
  double r[5],w[15]={1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
  int ib = m_key[k], ie=m_key[k+1];
  for(int i=ib; i!=ie; ++i) {w[i-ib] = mv[i-ib]+pv[m_map[i]];}

  bool q=true; if(n!=1) q=invert(n,w,w); else w[0]=1./w[0];

  if(q) {
    differenceParLoc(k,m,p,r);
    return new Trk::FitQualityOnSurface(Xi2(n,r,w),n);
  }
  return 0;
}

///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::predictedStateFitQuality
(const Trk::PatternTrackParameters& T,
 const Trk::LocalParameters       & P,
 const Amg::MatrixX               & E,
 int                              & N,
 double                           & X2) const
{
  // Conversion track parameters
  //
  double p[5],pv[15]; if(!trackParametersToUpdator(T,p,pv)) return false;

  // Conversion local parameters
  //
  int n,k;
  double m[5],mv[15]; if(!localParametersToUpdator(P,E,n,k,m,mv)) return false;

  // Xi2 calculation
  //
  double r[5],w[15]={1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
  int ib = m_key[k], ie=m_key[k+1];
  for(int i=ib; i!=ie; ++i) {w[i-ib] = mv[i-ib]+pv[m_map[i]];}

  bool q=true; if(n!=1) q=invert(n,w,w); else w[0]=1./w[0];
  N = n;
  if(q) {differenceParLoc(k,m,p,r); X2 = Xi2(n,r,w); return true ;}
  return false;
}


///////////////////////////////////////////////////////////////////
// Xi2 of Local position
///////////////////////////////////////////////////////////////////
      
const Trk::FitQualityOnSurface* 
Trk::KalmanUpdator_xk::fullStateFitQuality
(const Trk::TrackParameters& T,
 const Amg::Vector2D       & P,
 const Amg::MatrixX        & E) const
{
  const AmgSymMatrix(5)* v = T.covariance();
  if(!v) return 0;
  double t[5] = {T.parameters()[0],T.parameters()[1],
		 (*v)(0,0),(*v)(1,0),(*v)(1,1)};

  int N; double x2;
  if(fullStateFitQuality(t,P,E,N,x2)) return new Trk::FitQualityOnSurface(x2,N);
  return 0;
}

///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::fullStateFitQuality 
(const Trk::PatternTrackParameters& T,
 const Amg::Vector2D              & P,
 const Amg::MatrixX               & E,
 int                              & N,
 double                           & X2) const
{
  if(!T.iscovariance()) {N = 0; return false;}

  double t[5] = {T.par()[0],T.par()[1],
		 T.cov()[0],T.cov()[1],T.cov()[2]};

  return fullStateFitQuality(t,P,E,N,X2); 
}

///////////////////////////////////////////////////////////////////
// Xi2 of Local parameters
///////////////////////////////////////////////////////////////////

const Trk::FitQualityOnSurface* Trk::KalmanUpdator_xk::fullStateFitQuality
(const Trk::TrackParameters& T,
 const Trk::LocalParameters& P,
 const Amg::MatrixX        & E) const
{
  // Conversion track parameters
  //
  double p[5],pv[15]; if(!trackParametersToUpdator(T,p,pv)) return 0;
  // Conversion local parameters
  //
  int n,k;
  double m[5],mv[15]; if(!localParametersToUpdator(P,E,n,k,m,mv)) return 0;
  
  // Xi2 calculation
  //
  double r[5],w[15]={1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
  int ib = m_key[k], ie=m_key[k+1];
  for(int i=ib; i!=ie; ++i) {w[i-ib] = mv[i-ib]-pv[m_map[i]];}
  
  bool q=true; if(n!=1) q=invert(n,w,w); else w[0]=1./w[0];

  if(q) {
    differenceParLoc(k,m,p,r);
    return new Trk::FitQualityOnSurface(Xi2(n,r,w),n);
  }
  return new Trk::FitQualityOnSurface(0.,n);
}

///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::fullStateFitQuality
(const Trk::PatternTrackParameters& T,
 const Trk::LocalParameters       & P,
 const Amg::MatrixX               & E,
 int                              & N,
 double                           & X2) const
{
  // Conversion track parameters
  //
  N = 0;
  double p[5],pv[15]; if(!trackParametersToUpdator(T,p,pv)) return false;

  // Conversion local parameters
  //
  int k;
  double m[5],mv[15]; if(!localParametersToUpdator(P,E,N,k,m,mv)) return false;

  // Xi2 calculation
  //
  double r[5],w[15];
  int ib = m_key[k], ie=m_key[k+1];
  for(int i=ib; i!=ie; ++i) {w[i-ib] = mv[i-ib]-pv[m_map[i]];}

  bool q=true; if(N!=1) q=invert(N,w,w); else w[0]=1./w[0];

  if(q) {differenceParLoc(k,m,p,r); X2 = Xi2(N,r,w); return true ;}
  return false;
}

///////////////////////////////////////////////////////////////////
// Xi2 of two track parameters
///////////////////////////////////////////////////////////////////

const Trk::FitQualityOnSurface* Trk::KalmanUpdator_xk::predictedStateFitQuality
(const Trk::TrackParameters& T1,
 const Trk::TrackParameters& T2) const
{
  const double pi2 = 2.*M_PI , pi = M_PI;
  double m[5],mv[15]; if(!trackParametersToUpdator(T1,m,mv)) return 0;
  double p[5],pv[15]; if(!trackParametersToUpdator(T2,p,pv)) return 0;
  double r[5] = {m [ 0]-p [ 0],
		 m [ 1]-p [ 1],
		 m [ 2]-p [ 2],
		 m [ 3]-p [ 3],
		 m [ 4]-p [ 4]};

  if     (r[2] > pi) r[2] = fmod(r[2]+pi,pi2)-pi;
  else if(r[2] <-pi) r[2] = fmod(r[2]-pi,pi2)+pi;
  if     (r[3] > pi) r[3] = fmod(r[3]+pi,pi2)-pi;
  else if(r[3] <-pi) r[3] = fmod(r[3]-pi,pi2)+pi;

  double w[15]= {mv[ 0]+pv[ 0],
		 mv[ 1]+pv[ 1],
		 mv[ 2]+pv[ 2],
		 mv[ 3]+pv[ 3],
		 mv[ 4]+pv[ 4],
		 mv[ 5]+pv[ 5],
		 mv[ 6]+pv[ 6],
		 mv[ 7]+pv[ 7],
		 mv[ 8]+pv[ 8],
		 mv[ 9]+pv[ 9],
		 mv[10]+pv[10],
		 mv[11]+pv[11],
		 mv[12]+pv[12],
		 mv[13]+pv[13],
		 mv[14]+pv[14]};

  if(invert(5,w,w)) return new Trk::FitQualityOnSurface(Xi2(5,r,w),5);
  return 0;
}

///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::predictedStateFitQuality
(const Trk::PatternTrackParameters& T1,
 const Trk::PatternTrackParameters& T2,
 double                           & X2) const
{
  const double pi2 = 2.*M_PI,pi = M_PI;
  double m[5],mv[15]; if(!trackParametersToUpdator(T1,m,mv)) return 0;
  double p[5],pv[15]; if(!trackParametersToUpdator(T2,p,pv)) return 0;
  double r[5] = {m [ 0]-p [ 0],
		 m [ 1]-p [ 1],
		 m [ 2]-p [ 2],
		 m [ 3]-p [ 3],
		 m [ 4]-p [ 4]};

  if     (r[2] > pi) r[2] = fmod(r[2]+pi,pi2)-pi;
  else if(r[2] <-pi) r[2] = fmod(r[2]-pi,pi2)+pi;
  if     (r[3] > pi) r[3] = fmod(r[3]+pi,pi2)-pi;
  else if(r[3] <-pi) r[3] = fmod(r[3]-pi,pi2)+pi;

  double w[15]= {mv[ 0]+pv[ 0],
		 mv[ 1]+pv[ 1],
		 mv[ 2]+pv[ 2],
		 mv[ 3]+pv[ 3],
		 mv[ 4]+pv[ 4],
		 mv[ 5]+pv[ 5],
		 mv[ 6]+pv[ 6],
		 mv[ 7]+pv[ 7],
		 mv[ 8]+pv[ 8],
		 mv[ 9]+pv[ 9],
		 mv[10]+pv[10],
		 mv[11]+pv[11],
		 mv[12]+pv[12],
		 mv[13]+pv[13],
		 mv[14]+pv[14]};

  if(invert(5,w,w)) {X2 = Xi2(5,r,w); return true;}
  return false;
}

///////////////////////////////////////////////////////////////////
// let the client tools know how the assumptions on the initial
// precision for non-measured track parameters are configured 
///////////////////////////////////////////////////////////////////

const std::vector<double> Trk::KalmanUpdator_xk::initialErrors() const
{
  std::vector<double> errors(5);

  errors[0] = sqrt(m_cov0[0]);
  errors[1] = sqrt(m_cov0[1]);
  errors[2] = sqrt(m_cov0[2]);
  errors[3] = sqrt(m_cov0[3]);
  errors[4] = sqrt(m_cov0[4]);
  return errors;
}

///////////////////////////////////////////////////////////////////
// Protected methods
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Add or remove local position together with error matrix  
// with or witout Xi2 calculation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* Trk::KalmanUpdator_xk::update 
(const Trk::TrackParameters& T,
 const Amg::Vector2D       & P,
 const Amg::MatrixX        & E,
 Trk::FitQualityOnSurface* & Q,
 int                         O,
 bool                        X) const
{
  // Measurement information preparation
  //
  double m[2],mv[3];  
  int  n = E.rows(); if(n<=0) return 0;
  m [0]  = P[0];
  mv[0]  = E(0,0);
  if(n==2) {
    m [1]  = P[1];
    mv[1]  = E(1,0);
    mv[2]  = E(1,1);
  }

  // Conversion track parameters to updator presentation
  //
  double p[5],pv[15];  bool measured = trackParametersToUpdator(T,p,pv);
  bool update = false;

  if(measured) {
    double x2;
    if     (n==2) {
      update = updateWithTwoDim(O,X,m,mv,p,pv,x2);
      if(update && X && !Q) Q = new Trk::FitQualityOnSurface(x2,2); 
    }
    else if(n==1) {
      update = updateWithOneDim(O,X,m,mv,p,pv,x2);
      if(update && X && !Q) Q = new Trk::FitQualityOnSurface(x2,1); 
    }
    if(update) {
      testAngles(p,pv); return updatorToTrackParameters(T,p,pv);
    } 
    return 0;
  }

  // For no measured track parameters
  //
  if(O<0) return 0;
  
  if     (n==1) {
    update = updateNoMeasuredWithOneDim(m,mv,p,pv);
    if(update && X && !Q)  Q = new Trk::FitQualityOnSurface(0.,1); 
  }
  else if(n==2) {
    update = updateNoMeasuredWithTwoDim(m,mv,p,pv);
    if(update && X && !Q)  Q = new Trk::FitQualityOnSurface(0.,2); 
  }
 
  if(update) return updatorToTrackParameters(T,p,pv); 
  return 0;
}

///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::update 
(Trk::PatternTrackParameters& T ,
 const Amg::Vector2D        & P ,
 const Amg::MatrixX         & E ,
 int                          O ,
 bool                         X ,
 Trk::PatternTrackParameters& Ta,
 double                     & Q ,
 int                        & N ) const
{
  // Measurement information preparation
  //
  double m[2],mv[3];  
  N      = E.rows(); if(N<=0) return false;
  m [0]  = P[0];
  mv[0]  = E(0,0);
  if(N==2) {
    m [1]  = P[1];
    mv[1]  = E(1,0);
    mv[2]  = E(1,1);
  }

  // Conversion track parameters to updator presentation
  //
  double p[5],pv[15];  bool measured = trackParametersToUpdator(T,p,pv);
  bool update = false;

  if(measured) {

    if     (N==2) update = updateWithTwoDim(O,X,m,mv,p,pv,Q);
    else if(N==1) update = updateWithOneDim(O,X,m,mv,p,pv,Q);
    if(update) {
      testAngles(p,pv);
      Ta.setParametersWithCovariance(T.associatedSurface(),p,pv);
    }
    return update;
  }

  // For no measured track parameters
  //
  if(O<0) return false;
  Q = 0.;
  if     (N==1) update = updateNoMeasuredWithOneDim(m,mv,p,pv);
  else if(N==2) update = updateNoMeasuredWithTwoDim(m,mv,p,pv);


  if(update) Ta.setParametersWithCovariance(T.associatedSurface(),p,pv);
  return update;
}

///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::updateOneDimension 
(Trk::PatternTrackParameters& T ,
 const Amg::Vector2D        & P ,
 const Amg::MatrixX         & E ,
 int                          O ,
 bool                         X ,
 Trk::PatternTrackParameters& Ta,
 double                     & Q ) const
{
  // Measurement information preparation
  //
  double m[2],mv[3];  
  int N  = E.rows(); if(N!=2 ) return false;
  m [0]  = P[0];
  m [1]  = P[1];
  mv[0]  = E(0,0);
  mv[1]  = E(1,0);
  mv[2]  = E(1,1);

  // Conversion track parameters to updator presentation
  //
  double p[5],pv[15];  bool measured = trackParametersToUpdator(T,p,pv);
  bool update = false;

  if(measured) {

    if(fabs(mv[1]) < 1.e-30) {
      update = updateWithOneDimWithBoundary(O,X,m,mv,p,pv,Q);
    }
    else                     {
      update = updateWithTwoDimWithBoundary(O,X,m,mv,p,pv,Q);
    }
    if(update) {
      testAngles(p,pv);
      Ta.setParametersWithCovariance(T.associatedSurface(),p,pv);
    }
    return update;
  }

  // For no measured track parameters
  //
  if(O<0) return false;
  Q = 0.;
  update = updateNoMeasuredWithTwoDim(m,mv,p,pv);
  if(update) Ta.setParametersWithCovariance(T.associatedSurface(),p,pv);
  return update;
}


///////////////////////////////////////////////////////////////////
// Add or remove local parameters together with error matrix  
// with or witout Xi2 calculation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* Trk::KalmanUpdator_xk::update 
(const Trk::TrackParameters& T,
 const Trk::LocalParameters& P,
 const Amg::MatrixX        & E,
 Trk::FitQualityOnSurface* & Q,
 int                         O,
 bool                        X) const
{
  // Conversion local parameters
  //
  int n,k;
  double m[5],mv[15]; if(!localParametersToUpdator(P,E,n,k,m,mv)) return 0;

  // Conversion track parameters to updator presentation
  //
  double p[5],pv[15];  bool measured = trackParametersToUpdator(T,p,pv);
  bool update = false;

  if(measured) {

    double x2;
    if     (n==2 && k==3) {
      update = updateWithTwoDim(O,X,m,mv,p,pv,x2);
      if(update && X && !Q) Q = new Trk::FitQualityOnSurface(x2,2); 
    }
    else if(n==1 && k==1) {
      update = updateWithOneDim(O,X,m,mv,p,pv,x2);
      if(update && X && !Q) Q = new Trk::FitQualityOnSurface(x2,1); 
    }
    else                  {
      update = updateWithAnyDim(O,X,m,mv,p,pv,x2,n,k);
      if(update && X && !Q)  Q = new Trk::FitQualityOnSurface(x2,n); 
    }
    if(update) {testAngles(p,pv); return updatorToTrackParameters(T,p,pv);}
    return 0;
  }

  // For no measured track parameters
  //
  if(O<0) return 0;
  if     (n==1 && k==1) {
    update = updateNoMeasuredWithOneDim(m,mv,p,pv);
    if(update && X && !Q)  Q = new Trk::FitQualityOnSurface(0.,1); 
  }
  else if(n==2 && k==3) {
    update = updateNoMeasuredWithTwoDim(m,mv,p,pv);
    if(update && X && !Q)  Q = new Trk::FitQualityOnSurface(0.,2); 
  }
  else                 {
    update = updateNoMeasuredWithAnyDim(m,mv,p,pv,k);
    if(update && X && !Q)  Q = new Trk::FitQualityOnSurface(0.,n); 
  }
  return (update) ? updatorToTrackParameters(T,p,pv) : 0;
}

///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::update 
(Trk::PatternTrackParameters& T ,
 const Trk::LocalParameters & P ,
 const Amg::MatrixX         & E ,
 int                          O ,
 bool                         X ,
 Trk::PatternTrackParameters& Ta,
 double                     & Q ,
 int                        & N ) const
{
  // Conversion local parameters
  //
  int k;
  double m[5],mv[15]; if(!localParametersToUpdator(P,E,N,k,m,mv)) return false;
 
  // Conversion track parameters to updator presentation
  //
  double p[5],pv[15];  bool measured = trackParametersToUpdator(T,p,pv);
  bool update = false;

  if(measured) {

    if     (N==2 && k==3) update = updateWithTwoDim(O,X,m,mv,p,pv,Q);
    else if(N==1 && k==1) update = updateWithOneDim(O,X,m,mv,p,pv,Q);
    else                  update = updateWithAnyDim(O,X,m,mv,p,pv,Q,N,k);
   if(update) {
     testAngles(p,pv);
     Ta.setParametersWithCovariance(T.associatedSurface(),p,pv);
   }
   return update;
  }

  // For no measured track parameters
  //
  if(O<0) return false;
  Q = 0.;
  if     (N==1 && k==1)  update = updateNoMeasuredWithOneDim(m,mv,p,pv);
  else if(N==2 && k==3)  update = updateNoMeasuredWithTwoDim(m,mv,p,pv);
  else                   update = updateNoMeasuredWithAnyDim(m,mv,p,pv,k);
  if(update) Ta.setParametersWithCovariance(T.associatedSurface(),p,pv);
  return update;
}

///////////////////////////////////////////////////////////////////
// Xi2 of Local position
///////////////////////////////////////////////////////////////////
      
bool Trk::KalmanUpdator_xk::predictedStateFitQuality 
(const double*             T,
 const Amg::Vector2D     & P,
 const Amg::MatrixX      & E,
 int&                      N,
 double&                  X2) const
{
  // Measurement information preparation
  //
  double m[2],mv[3]; X2 = 0.;  
  N      = E.rows(); if(N<=0) return false;
  m [0]  = P[0]-T[0];
  mv[0]  = E(0,0)+T[2];  
  if(N==1) {
    
    if(mv[0]>0.) X2 = m[0]*m[0]/mv[0];
    return true;
  }
  if(N==2) {
    m [1]     = P[1]-T[1];
    mv[1]     = E(1,0)+T[3];
    mv[2]     = E(1,1)+T[4];
    double d  = mv[0]*mv[2]-mv[1]*mv[1]; 
    if(d>0.) X2 = (m[0]*m[0]*mv[2]+m[1]*m[1]*mv[0]-2.*m[0]*m[1]*mv[1])/d;
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Xi2 of Local position
///////////////////////////////////////////////////////////////////
      
bool Trk::KalmanUpdator_xk::fullStateFitQuality
(const double*               T,
 const Amg::Vector2D       & P,
 const Amg::MatrixX        & E,
 int&                        N,
 double&                    X2) const
{
  // Measurement information preparation
  //
  double m[2],mv[3]; X2 = 0.; 
  N      = E.rows(); if(N<=0) return false;
  m [0]  = P[0]-T[0];
  mv[0]  = E(0,0)-T[2];  
  if(N==1) {
    
    if(mv[0]>0.) X2 = m[0]*m[0]/mv[0];
    return true;
  }
  if(N==2) {
    m [1]     = P[1]-T[1];
    mv[1]     = E(1,0)-T[3];
    mv[2]     = E(1,1)-T[4];
    double d  = mv[0]*mv[2]-mv[1]*mv[1]; 
    if(d>0.) X2 = (m[0]*m[0]*mv[2]+m[1]*m[1]*mv[0]-2.*m[0]*m[1]*mv[1])/d;
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Add one dimension information to no measured track parameters 
// M and MV is measuremet           together with covariance
// P and PV is new track parameters together with covarianace
///////////////////////////////////////////////////////////////////

bool  Trk::KalmanUpdator_xk::updateNoMeasuredWithOneDim
(double* M,double* MV,double* P, double* PV) const
{
  P [ 0] =    M [ 0];
  PV[ 0] =    MV[ 0];
  PV[ 1] =        0.;
  PV[ 2] = m_cov0[1];
  PV[ 3] =        0.;
  PV[ 4] =        0.;
  PV[ 5] = m_cov0[2];
  PV[ 6] =        0.;
  PV[ 7] =        0.;
  PV[ 8] =        0.;
  PV[ 9] = m_cov0[3];
  PV[10] =        0.;
  PV[11] =        0.;
  PV[12] =        0.;
  PV[13] =        0.;
  PV[14] = m_cov0[4];
  return true;
}

///////////////////////////////////////////////////////////////////
// Add two dimension information to no measured track parameters 
// M and MV is measuremet           together with covariance
// P and PV is new track parameters together with covarianace
///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::updateNoMeasuredWithTwoDim
(double* M,double* MV,double* P, double* PV) const
{
  P [ 0] =    M [ 0];
  P [ 1] =    M [ 1];
  PV[ 0] =    MV[ 0];
  PV[ 1] =    MV[ 1];
  PV[ 2] =    MV[ 2];
  PV[ 3] =        0.;
  PV[ 4] =        0.;
  PV[ 5] = m_cov0[2];
  PV[ 6] =        0.;
  PV[ 7] =        0.;
  PV[ 8] =        0.;
  PV[ 9] = m_cov0[3];
  PV[10] =        0.;
  PV[11] =        0.;
  PV[12] =        0.;
  PV[13] =        0.;
  PV[14] = m_cov0[4];
  return true;
}

///////////////////////////////////////////////////////////////////
// Add any dimension information (>=1 and <=5)  to no measured 
// track parameters 
// M and MV is measuremet           together with covariance
// P and PV is new track parameters together with covarianace
// K is key word
///////////////////////////////////////////////////////////////////

bool  Trk::KalmanUpdator_xk::updateNoMeasuredWithAnyDim
(double* M,double* MV,double* P,double* PV,int K) const
{

  int i=0,j=0; 
  while(K) {if(K&1) P[i]=M[j++]; K>>=1; ++i;} if(i==0) return false;

  PV[ 0] = m_cov0[0];
  PV[ 1] =        0.;
  PV[ 2] = m_cov0[1];
  PV[ 3] =        0.;
  PV[ 4] =        0.;
  PV[ 5] = m_cov0[2];
  PV[ 6] =        0.;
  PV[ 7] =        0.;
  PV[ 8] =        0.;
  PV[ 9] = m_cov0[3];
  PV[10] =        0.;
  PV[11] =        0.;
  PV[12] =        0.;
  PV[13] =        0.;
  PV[14] = m_cov0[4];

  int ne = m_key[K+1]; i=0;
  for(int n = m_key[K]; n!=ne; ++n) {PV[m_map[n]] = MV[i++];}
  return true;
}

///////////////////////////////////////////////////////////////////
// Add or remove one dimension information to measured track parameters 
// M and MV is measuremet           together with covariance
// P and PV is new track parameters together with covarianace
///////////////////////////////////////////////////////////////////

bool  Trk::KalmanUpdator_xk::updateWithOneDim
(int O,bool X,double* M,double* MV,double* P, double* PV,double& xi2) const
{
  double v0;
  if(O>0) {v0 = MV[0]+PV[0];}
  else    {v0 = MV[0]-PV[0];}

  if(v0<=0.) return false;
  double w0 = 1./v0;
  double r0 = M[0]-P[0];

  // K matrix with (5x1) size
  //
  double k0 = PV[ 0]*w0;
  double k1 = PV[ 1]*w0;
  double k2 = PV[ 3]*w0;
  double k3 = PV[ 6]*w0;
  double k4 = PV[10]*w0;

  if(O<0) {k0=-k0; k1=-k1; k2=-k2; k3=-k3; k4=-k4;}
  
  // New parameters
  //
  P[0]+=(k0*r0);
  P[1]+=(k1*r0);
  P[2]+=(k2*r0);
  P[3]+=(k3*r0);
  P[4]+=(k4*r0);

  // New covariance matrix
  //
  if((PV[14]-= (k4*PV[10]))<=0.) return false; 
      PV[13]-= (k4*PV[ 6]);
      PV[12]-= (k4*PV[ 3]);
      PV[11]-= (k4*PV[ 1]);
      PV[10]-= (k4*PV[ 0]);
  if((PV[ 9]-= (k3*PV[ 6]))<=0.) return false; 
      PV[ 8]-= (k3*PV[ 3]); 
      PV[ 7]-= (k3*PV[ 1]); 
      PV[ 6]-= (k3*PV[ 0]); 
  if((PV[ 5]-= (k2*PV[ 3]))<=0.) return false; 
      PV[ 4]-= (k2*PV[ 1]);
      PV[ 3]-= (k2*PV[ 0]);
  if((PV[ 2]-= (k1*PV[ 1]))<=0.) return false; 
      PV[ 1]-= (k1*PV[ 0]);
  if((PV[ 0]-= (k0*PV[ 0]))<=0.) return false; 
  
  if(X) xi2 = r0*r0*w0;
  return true;
}

///////////////////////////////////////////////////////////////////
// Add or remove one dimension information to measured track parameters 
// and check boundary for second parameters
// M and MV is measuremet           together with covariance
// P and PV is new track parameters together with covarianace
///////////////////////////////////////////////////////////////////

bool  Trk::KalmanUpdator_xk::updateWithOneDimWithBoundary
(int O,bool X,double* M,double* MV,double* P, double* PV,double& xi2) const
{
  double v0;
  if(O>0) {v0 = MV[0]+PV[0];}
  else    {v0 = MV[0]-PV[0];}

  if(v0<=0.) return false;
  double w0 = 1./v0;
  double r0 = M[0]-P[0];

  // K matrix with (5x1) size
  //
  double k0 = PV[ 0]*w0;
  double k1 = PV[ 1]*w0;
  double k2 = PV[ 3]*w0;
  double k3 = PV[ 6]*w0;
  double k4 = PV[10]*w0;

  if(O<0) {k0=-k0; k1=-k1; k2=-k2; k3=-k3; k4=-k4;}
  
  // Boundary check
  //
  double P1 = P[1]+k1*r0          ;
  double dP = P1-M[1]             ;
  double W  = sqrt(MV[2])*1.732051; 
  
  if(fabs(dP) <= W) {

    P[0]+=(k0*r0);
    P[1] = P1    ;
    P[2]+=(k2*r0);
    P[3]+=(k3*r0);
    P[4]+=(k4*r0);
    if(X) xi2 = r0*r0*w0;
  }
  else              {

    dP > W ? M[1]+=W : M[1]-=W; MV[2] = m_covBoundary;
    if(!updateWithTwoDimParameters(O,true,M,MV,P,PV,xi2)) return false;
  }

  // New covariance matrix
  //
  if((PV[14]-= (k4*PV[10]))<=0.) return false; 
      PV[13]-= (k4*PV[ 6]);
      PV[12]-= (k4*PV[ 3]);
      PV[11]-= (k4*PV[ 1]);
      PV[10]-= (k4*PV[ 0]);
  if((PV[ 9]-= (k3*PV[ 6]))<=0.) return false; 
      PV[ 8]-= (k3*PV[ 3]); 
      PV[ 7]-= (k3*PV[ 1]); 
      PV[ 6]-= (k3*PV[ 0]); 
  if((PV[ 5]-= (k2*PV[ 3]))<=0.) return false; 
      PV[ 4]-= (k2*PV[ 1]);
      PV[ 3]-= (k2*PV[ 0]);
  if((PV[ 2]-= (k1*PV[ 1]))<=0.) return false; 
      PV[ 1]-= (k1*PV[ 0]);
  if((PV[ 0]-= (k0*PV[ 0]))<=0.) return false; 

  return true;
}

///////////////////////////////////////////////////////////////////
// Add or remove two dimension information to measured track parameters 
// M and MV is measuremet           together with covariance
// P and PV is new track parameters together with covarianace
///////////////////////////////////////////////////////////////////

bool  Trk::KalmanUpdator_xk::updateWithTwoDim
(int O,bool X,double* M,double* MV,double* P, double* PV,double& xi2) const
{
  double v0,v1,v2;
  if(O>0) {v0 = MV[0]+PV[0]; v1 = MV[1]+PV[1]; v2 = MV[2]+PV[2];}
  else    {v0 = MV[0]-PV[0]; v1 = MV[1]-PV[1]; v2 = MV[2]-PV[2];}

  double d  = v0*v2-v1*v1; if(d<=0.) return false; d=1./d;
  double w0 = v2*d;
  double w1 =-v1*d;
  double w2 = v0*d;
  double r0 = M[0]-P[0];
  double r1 = M[1]-P[1];

  // K matrix with (5x2) size
  //
  double k0 = PV[ 0]*w0+PV[ 1]*w1, k1 = PV[ 0]*w1+PV[ 1]*w2; 
  double k2 = PV[ 1]*w0+PV[ 2]*w1, k3 = PV[ 1]*w1+PV[ 2]*w2;
  double k4 = PV[ 3]*w0+PV[ 4]*w1, k5 = PV[ 3]*w1+PV[ 4]*w2;
  double k6 = PV[ 6]*w0+PV[ 7]*w1, k7 = PV[ 6]*w1+PV[ 7]*w2;
  double k8 = PV[10]*w0+PV[11]*w1, k9 = PV[10]*w1+PV[11]*w2;

  if(O<0) {
    k0=-k0; k1=-k1; k2=-k2; k3=-k3; k4=-k4; 
    k5=-k5; k6=-k6; k7=-k7; k8=-k8; k9=-k9;
  }
  
  // New parameters
  //
  P[0]+=(k0*r0+k1*r1);
  P[1]+=(k2*r0+k3*r1);
  P[2]+=(k4*r0+k5*r1);
  P[3]+=(k6*r0+k7*r1);
  P[4]+=(k8*r0+k9*r1);

  // New covariance matrix
  //
  if((PV[14]-= (k8*PV[10]+k9*PV[11]))<=0.) return false;
      PV[13]-= (k8*PV[ 6]+k9*PV[ 7]);
      PV[12]-= (k8*PV[ 3]+k9*PV[ 4]);
      PV[11]-= (k8*PV[ 1]+k9*PV[ 2]);
      PV[10]-= (k8*PV[ 0]+k9*PV[ 1]);
  if((PV[ 9]-= (k6*PV[ 6]+k7*PV[ 7]))<=0.) return false;
      PV[ 8]-= (k6*PV[ 3]+k7*PV[ 4]); 
      PV[ 7]-= (k6*PV[ 1]+k7*PV[ 2]); 
      PV[ 6]-= (k6*PV[ 0]+k7*PV[ 1]); 
  if((PV[ 5]-= (k4*PV[ 3]+k5*PV[ 4]))<=0.) return false;
      PV[ 4]-= (k4*PV[ 1]+k5*PV[ 2]);
      PV[ 3]-= (k4*PV[ 0]+k5*PV[ 1]);
  if((PV[ 2]-= (k3*PV[ 2]+k2*PV[ 1]))<=0.) return false;
  double c1 = (1.-k3)*PV[ 1]-k2*PV[ 0];
  if((PV[ 0]-= (k0*PV[ 0]+k1*PV[ 1]))<=0.) return false;
      PV[ 1] = c1;
  
  if(X) xi2 = (r0*r0*w0+r1*r1*w2+2.*r0*r1*w1);
  return true;
}

///////////////////////////////////////////////////////////////////
// Add or remove two dimension information to measured track parameters 
// without calculation new covariance matrix
// M and MV is measuremet           together with covariance
// P and PV is new track parameters together with covarianace
///////////////////////////////////////////////////////////////////

bool  Trk::KalmanUpdator_xk::updateWithTwoDimParameters
(int O,bool X,double* M,double* MV,double* P, double* PV,double& xi2) const
{
  double v0,v1,v2;
  if(O>0) {v0 = MV[0]+PV[0]; v1 = MV[1]+PV[1]; v2 = MV[2]+PV[2];}
  else    {v0 = MV[0]-PV[0]; v1 = MV[1]-PV[1]; v2 = MV[2]-PV[2];}

  double d  = v0*v2-v1*v1; if(d<=0.) return false; d=1./d;
  double w0 = v2*d;
  double w1 =-v1*d;
  double w2 = v0*d;
  double r0 = M[0]-P[0];
  double r1 = M[1]-P[1];

  // K matrix with (5x2) size
  //
  double k0 = PV[ 0]*w0+PV[ 1]*w1, k1 = PV[ 0]*w1+PV[ 1]*w2; 
  double k2 = PV[ 1]*w0+PV[ 2]*w1, k3 = PV[ 1]*w1+PV[ 2]*w2;
  double k4 = PV[ 3]*w0+PV[ 4]*w1, k5 = PV[ 3]*w1+PV[ 4]*w2;
  double k6 = PV[ 6]*w0+PV[ 7]*w1, k7 = PV[ 6]*w1+PV[ 7]*w2;
  double k8 = PV[10]*w0+PV[11]*w1, k9 = PV[10]*w1+PV[11]*w2;

  if(O<0) {
    k0=-k0; k1=-k1; k2=-k2; k3=-k3; k4=-k4; 
    k5=-k5; k6=-k6; k7=-k7; k8=-k8; k9=-k9;
  }
  
  // New parameters
  //
  P[0]+=(k0*r0+k1*r1);
  P[1]+=(k2*r0+k3*r1);
  P[2]+=(k4*r0+k5*r1);
  P[3]+=(k6*r0+k7*r1);
  P[4]+=(k8*r0+k9*r1);
  if(X) xi2 = (r0*r0*w0+r1*r1*w2+2.*r0*r1*w1);
  return true;
}

///////////////////////////////////////////////////////////////////
// Add or remove two dimension information to measured track parameters 
// as one dimension and check boundary for second parameter
// M and MV is measuremet           together with covariance
// P and PV is new track parameters together with covarianace
///////////////////////////////////////////////////////////////////

bool  Trk::KalmanUpdator_xk::updateWithTwoDimWithBoundary
(int O,bool X,double* M,double* MV,double* P, double* PV,double& xi2) const
{
  // Increase second covariance
  //
  double sa  = MV[0]+MV[2]            ;
  double d   = MV[0]*MV[2]-MV[1]*MV[1];
  double ds  = d/sa                   ;
  double V0  = ds*(1.+ds/sa)          ;
  double V1  = sa-V0                  ;
  double dV  = 1./(V0-V1)             ;
  double sc  = MV[1]*dV               ;
  double al  = (MV[0]-MV[2])*dV       ;
  double s2  = .5*(1.-al)             ;
  double c2  = s2+al                  ;
  double c   = sqrt(c2)               ;
  double s   = sc/c                   ;

  // New measurement 
  //
  double M0 = c*M[0]+s*M[1];
  M [ 1]    = c*M[1]-s*M[0];
  M [ 0]    = M0           ;
  MV[ 0]    = V0           ;
  MV[ 1]    = 0.           ;
  MV[ 2]    = V1           ; 
  
  // Rotate track parameters and covariance matrix
  //
  double P0  = P[0]                     ;
  P [ 0]     = c*P0  +s*P[1]            ;
  P [ 1]     = c*P[1]-s*P0              ;
  double B   = 2.*sc*PV[ 1]             ;    
  double PV0 = PV[ 0]                   ;
  double PV3 = PV[ 3]                   ;
  double PV6 = PV[ 6]                   ;
  double PV10= PV[10]                   ;
  PV[ 0]     = c2*PV0+s2*PV[ 2]+B       ;
  PV[ 1]     = sc*(PV[ 2]-PV0)+PV[ 1]*al; 
  PV[ 2]     = s2*PV0+c2*PV[ 2]-B       ;
  PV[ 3]     = c*PV3   +s*PV[ 4]        ;
  PV[ 4]     = c*PV[ 4]-s*PV3           ;
  PV[ 6]     = c*PV6   +s*PV[ 7]        ;
  PV[ 7]     = c*PV[ 7]-s*PV6           ;
  PV[10]     = c*PV10  +s*PV[11]        ;
  PV[11]     = c*PV[11]-s*PV10          ;  
  
  if(!updateWithOneDimWithBoundary(O,X,M,MV,P,PV,xi2)) return false;

  // Back rotation new track parameters and covariance matrix
  //
  s = -s; sc = -sc;

  P0         = P[0]                     ;
  P [ 0]     = c*P0  +s*P[1]            ;
  P [ 1]     = c*P[1]-s*P0              ;
  B          = 2.*sc*PV[ 1]             ;    
  PV0        = PV[ 0]                   ;
  PV3        = PV[ 3]                   ;
  PV6        = PV[ 6]                   ;
  PV10       = PV[10]                   ;
  PV[ 0]     = c2*PV0+s2*PV[ 2]+B       ;
  PV[ 1]     = sc*(PV[ 2]-PV0)+PV[ 1]*al; 
  PV[ 2]     = s2*PV0+c2*PV[ 2]-B       ;
  PV[ 3]     = c*PV3   +s*PV[ 4]        ;
  PV[ 4]     = c*PV[ 4]-s*PV3           ;
  PV[ 6]     = c*PV6   +s*PV[ 7]        ;
  PV[ 7]     = c*PV[ 7]-s*PV6           ;
  PV[10]     = c*PV10  +s*PV[11]        ;
  PV[11]     = c*PV[11]-s*PV10          ;  
  return true;
}

///////////////////////////////////////////////////////////////////
// Add  five dimension information to measured track parameters 
// M and MV is measuremet           together with covariance
// P and PV is new track parameters together with covarianace
///////////////////////////////////////////////////////////////////

bool  Trk::KalmanUpdator_xk::updateWithFiveDim
(bool X,double* M,double* MV,double* P, double* PV,double& xi2) const
{
  const double pi2 = 2.*M_PI, pi = M_PI;

  double w[15]={MV[ 0]+PV[ 0],MV[ 1]+PV[ 1],MV[ 2]+PV[ 2], 
		MV[ 3]+PV[ 3],MV[ 4]+PV[ 4],MV[ 5]+PV[ 5], 
 		MV[ 6]+PV[ 6],MV[ 7]+PV[ 7],MV[ 8]+PV[ 8], 
 		MV[ 9]+PV[ 9],MV[10]+PV[10],MV[11]+PV[11], 
  		MV[12]+PV[12],MV[13]+PV[13],MV[14]+PV[14]};
  
  if(!invert5(w,w)) return false;

  double k00 =(PV[ 0]*w[ 0]+PV[ 1]*w[ 1]+PV[ 3]*w[ 3])+(PV[ 6]*w[ 6]+PV[10]*w[10]);
  double k01 =(PV[ 0]*w[ 1]+PV[ 1]*w[ 2]+PV[ 3]*w[ 4])+(PV[ 6]*w[ 7]+PV[10]*w[11]);
  double k02 =(PV[ 0]*w[ 3]+PV[ 1]*w[ 4]+PV[ 3]*w[ 5])+(PV[ 6]*w[ 8]+PV[10]*w[12]);
  double k03 =(PV[ 0]*w[ 6]+PV[ 1]*w[ 7]+PV[ 3]*w[ 8])+(PV[ 6]*w[ 9]+PV[10]*w[13]);
  double k04 =(PV[ 0]*w[10]+PV[ 1]*w[11]+PV[ 3]*w[12])+(PV[ 6]*w[13]+PV[10]*w[14]);
  double k10 =(PV[ 1]*w[ 0]+PV[ 2]*w[ 1]+PV[ 4]*w[ 3])+(PV[ 7]*w[ 6]+PV[11]*w[10]);
  double k11 =(PV[ 1]*w[ 1]+PV[ 2]*w[ 2]+PV[ 4]*w[ 4])+(PV[ 7]*w[ 7]+PV[11]*w[11]);
  double k12 =(PV[ 1]*w[ 3]+PV[ 2]*w[ 4]+PV[ 4]*w[ 5])+(PV[ 7]*w[ 8]+PV[11]*w[12]);
  double k13 =(PV[ 1]*w[ 6]+PV[ 2]*w[ 7]+PV[ 4]*w[ 8])+(PV[ 7]*w[ 9]+PV[11]*w[13]);
  double k14 =(PV[ 1]*w[10]+PV[ 2]*w[11]+PV[ 4]*w[12])+(PV[ 7]*w[13]+PV[11]*w[14]);
  double k20 =(PV[ 3]*w[ 0]+PV[ 4]*w[ 1]+PV[ 5]*w[ 3])+(PV[ 8]*w[ 6]+PV[12]*w[10]);
  double k21 =(PV[ 3]*w[ 1]+PV[ 4]*w[ 2]+PV[ 5]*w[ 4])+(PV[ 8]*w[ 7]+PV[12]*w[11]);
  double k22 =(PV[ 3]*w[ 3]+PV[ 4]*w[ 4]+PV[ 5]*w[ 5])+(PV[ 8]*w[ 8]+PV[12]*w[12]);
  double k23 =(PV[ 3]*w[ 6]+PV[ 4]*w[ 7]+PV[ 5]*w[ 8])+(PV[ 8]*w[ 9]+PV[12]*w[13]);
  double k24 =(PV[ 3]*w[10]+PV[ 4]*w[11]+PV[ 5]*w[12])+(PV[ 8]*w[13]+PV[12]*w[14]);
  double k30 =(PV[ 6]*w[ 0]+PV[ 7]*w[ 1]+PV[ 8]*w[ 3])+(PV[ 9]*w[ 6]+PV[13]*w[10]);
  double k31 =(PV[ 6]*w[ 1]+PV[ 7]*w[ 2]+PV[ 8]*w[ 4])+(PV[ 9]*w[ 7]+PV[13]*w[11]);
  double k32 =(PV[ 6]*w[ 3]+PV[ 7]*w[ 4]+PV[ 8]*w[ 5])+(PV[ 9]*w[ 8]+PV[13]*w[12]);
  double k33 =(PV[ 6]*w[ 6]+PV[ 7]*w[ 7]+PV[ 8]*w[ 8])+(PV[ 9]*w[ 9]+PV[13]*w[13]);
  double k34 =(PV[ 6]*w[10]+PV[ 7]*w[11]+PV[ 8]*w[12])+(PV[ 9]*w[13]+PV[13]*w[14]);
  double k40 =(PV[10]*w[ 0]+PV[11]*w[ 1]+PV[12]*w[ 3])+(PV[13]*w[ 6]+PV[14]*w[10]);
  double k41 =(PV[10]*w[ 1]+PV[11]*w[ 2]+PV[12]*w[ 4])+(PV[13]*w[ 7]+PV[14]*w[11]);
  double k42 =(PV[10]*w[ 3]+PV[11]*w[ 4]+PV[12]*w[ 5])+(PV[13]*w[ 8]+PV[14]*w[12]);
  double k43 =(PV[10]*w[ 6]+PV[11]*w[ 7]+PV[12]*w[ 8])+(PV[13]*w[ 9]+PV[14]*w[13]);
  double k44 =(PV[10]*w[10]+PV[11]*w[11]+PV[12]*w[12])+(PV[13]*w[13]+PV[14]*w[14]);

  // Residial production
  //
  double r[5]={M[0]-P[0],M[1]-P[1],M[2]-P[2],M[3]-P[3],M[4]-P[4]};

  // Test for angles differences
  //
  if     (r[2] > pi) r[2] = fmod(r[2]+pi,pi2)-pi;
  else if(r[2] <-pi) r[2] = fmod(r[2]-pi,pi2)+pi;
  if     (r[3] > pi) r[3] = fmod(r[3]+pi,pi2)-pi;
  else if(r[3] <-pi) r[3] = fmod(r[3]-pi,pi2)+pi;

  // New parameters
  //
  double p0 =(k00*r[0]+k01*r[1]+k02*r[2])+(k03*r[3]+k04*r[4]); P[0]+=p0;
  double p1 =(k10*r[0]+k11*r[1]+k12*r[2])+(k13*r[3]+k14*r[4]); P[1]+=p1;
  double p2 =(k20*r[0]+k21*r[1]+k22*r[2])+(k23*r[3]+k24*r[4]); P[2]+=p2;
  double p3 =(k30*r[0]+k31*r[1]+k32*r[2])+(k33*r[3]+k34*r[4]); P[3]+=p3;
  double p4 =(k40*r[0]+k41*r[1]+k42*r[2])+(k43*r[3]+k44*r[4]); P[4]+=p4;

  // New covariance matrix
  //
  double v0  =(k00*PV[ 0]+k01*PV[ 1]+k02*PV[ 3])+(k03*PV[ 6]+k04*PV[10]);
  double v1  =(k10*PV[ 0]+k11*PV[ 1]+k12*PV[ 3])+(k13*PV[ 6]+k14*PV[10]);
  double v2  =(k10*PV[ 1]+k11*PV[ 2]+k12*PV[ 4])+(k13*PV[ 7]+k14*PV[11]);
  double v3  =(k20*PV[ 0]+k21*PV[ 1]+k22*PV[ 3])+(k23*PV[ 6]+k24*PV[10]);
  double v4  =(k20*PV[ 1]+k21*PV[ 2]+k22*PV[ 4])+(k23*PV[ 7]+k24*PV[11]);
  double v5  =(k20*PV[ 3]+k21*PV[ 4]+k22*PV[ 5])+(k23*PV[ 8]+k24*PV[12]);
  double v6  =(k30*PV[ 0]+k31*PV[ 1]+k32*PV[ 3])+(k33*PV[ 6]+k34*PV[10]);
  double v7  =(k30*PV[ 1]+k31*PV[ 2]+k32*PV[ 4])+(k33*PV[ 7]+k34*PV[11]);
  double v8  =(k30*PV[ 3]+k31*PV[ 4]+k32*PV[ 5])+(k33*PV[ 8]+k34*PV[12]);
  double v9  =(k30*PV[ 6]+k31*PV[ 7]+k32*PV[ 8])+(k33*PV[ 9]+k34*PV[13]);
  double v10 =(k40*PV[ 0]+k41*PV[ 1]+k42*PV[ 3])+(k43*PV[ 6]+k44*PV[10]);
  double v11 =(k40*PV[ 1]+k41*PV[ 2]+k42*PV[ 4])+(k43*PV[ 7]+k44*PV[11]);
  double v12 =(k40*PV[ 3]+k41*PV[ 4]+k42*PV[ 5])+(k43*PV[ 8]+k44*PV[12]);
  double v13 =(k40*PV[ 6]+k41*PV[ 7]+k42*PV[ 8])+(k43*PV[ 9]+k44*PV[13]);
  double v14 =(k40*PV[10]+k41*PV[11]+k42*PV[12])+(k43*PV[13]+k44*PV[14]);

  if((PV[ 0]-=v0 )<=0.) return false;
      PV[ 1]-=v1 ;
  if((PV[ 2]-=v2 )<=0.) return false;
      PV[ 3]-=v3 ; 
      PV[ 4]-=v4 ; 
  if((PV[ 5]-=v5 )<=0.) return false;
      PV[ 6]-=v6 ; 
      PV[ 7]-=v7 ; 
      PV[ 8]-=v8 ; 
  if((PV[ 9]-=v9 )<=0.) return false;
      PV[10]-=v10; 
      PV[11]-=v11; 
      PV[12]-=v12; 
      PV[13]-=v13; 
  if((PV[14]-=v14)<=0.) return false;

  if(X) xi2 = Xi2(5,r,w);
  return true;
}

///////////////////////////////////////////////////////////////////
// Add or remove any dimension information to measured track parameters 
// M and MV is measuremet           together with covariance
// P and PV is new track parameters together with covarianace
///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::updateWithAnyDim
(int O,bool X,double* M,double* MV,double* P, double* PV,double& xi2,
 int N,int K) const
{
  double s,w[15]={1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
  int ib = m_key[K], ie=m_key[K+1];

  if(O>0) {s= 1.; for(int i=ib; i!=ie; ++i) {w[i-ib] = MV[i-ib]+PV[m_map[i]];}}
  else    {s=-1.; for(int i=ib; i!=ie; ++i) {w[i-ib] = MV[i-ib]-PV[m_map[i]];}}
  
  if(N==1) w[0] = 1./w[0]; else if(!invert(N,w,w)) return false;
  double v[15]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.}; 
  for(int i=ib; i!=ie; ++i) {v[m_map[i]] =w[i-ib];}

  double k00 =((PV[ 0]*v[ 0]+PV[ 1]*v[ 1]+PV[ 3]*v[ 3])+(PV[ 6]*v[ 6]+PV[10]*v[10]))*s;
  double k01 =((PV[ 0]*v[ 1]+PV[ 1]*v[ 2]+PV[ 3]*v[ 4])+(PV[ 6]*v[ 7]+PV[10]*v[11]))*s;
  double k02 =((PV[ 0]*v[ 3]+PV[ 1]*v[ 4]+PV[ 3]*v[ 5])+(PV[ 6]*v[ 8]+PV[10]*v[12]))*s;
  double k03 =((PV[ 0]*v[ 6]+PV[ 1]*v[ 7]+PV[ 3]*v[ 8])+(PV[ 6]*v[ 9]+PV[10]*v[13]))*s;
  double k04 =((PV[ 0]*v[10]+PV[ 1]*v[11]+PV[ 3]*v[12])+(PV[ 6]*v[13]+PV[10]*v[14]))*s;
  double k10 =((PV[ 1]*v[ 0]+PV[ 2]*v[ 1]+PV[ 4]*v[ 3])+(PV[ 7]*v[ 6]+PV[11]*v[10]))*s;
  double k11 =((PV[ 1]*v[ 1]+PV[ 2]*v[ 2]+PV[ 4]*v[ 4])+(PV[ 7]*v[ 7]+PV[11]*v[11]))*s;
  double k12 =((PV[ 1]*v[ 3]+PV[ 2]*v[ 4]+PV[ 4]*v[ 5])+(PV[ 7]*v[ 8]+PV[11]*v[12]))*s;
  double k13 =((PV[ 1]*v[ 6]+PV[ 2]*v[ 7]+PV[ 4]*v[ 8])+(PV[ 7]*v[ 9]+PV[11]*v[13]))*s;
  double k14 =((PV[ 1]*v[10]+PV[ 2]*v[11]+PV[ 4]*v[12])+(PV[ 7]*v[13]+PV[11]*v[14]))*s;
  double k20 =((PV[ 3]*v[ 0]+PV[ 4]*v[ 1]+PV[ 5]*v[ 3])+(PV[ 8]*v[ 6]+PV[12]*v[10]))*s;
  double k21 =((PV[ 3]*v[ 1]+PV[ 4]*v[ 2]+PV[ 5]*v[ 4])+(PV[ 8]*v[ 7]+PV[12]*v[11]))*s;
  double k22 =((PV[ 3]*v[ 3]+PV[ 4]*v[ 4]+PV[ 5]*v[ 5])+(PV[ 8]*v[ 8]+PV[12]*v[12]))*s;
  double k23 =((PV[ 3]*v[ 6]+PV[ 4]*v[ 7]+PV[ 5]*v[ 8])+(PV[ 8]*v[ 9]+PV[12]*v[13]))*s;
  double k24 =((PV[ 3]*v[10]+PV[ 4]*v[11]+PV[ 5]*v[12])+(PV[ 8]*v[13]+PV[12]*v[14]))*s;
  double k30 =((PV[ 6]*v[ 0]+PV[ 7]*v[ 1]+PV[ 8]*v[ 3])+(PV[ 9]*v[ 6]+PV[13]*v[10]))*s;
  double k31 =((PV[ 6]*v[ 1]+PV[ 7]*v[ 2]+PV[ 8]*v[ 4])+(PV[ 9]*v[ 7]+PV[13]*v[11]))*s;
  double k32 =((PV[ 6]*v[ 3]+PV[ 7]*v[ 4]+PV[ 8]*v[ 5])+(PV[ 9]*v[ 8]+PV[13]*v[12]))*s;
  double k33 =((PV[ 6]*v[ 6]+PV[ 7]*v[ 7]+PV[ 8]*v[ 8])+(PV[ 9]*v[ 9]+PV[13]*v[13]))*s;
  double k34 =((PV[ 6]*v[10]+PV[ 7]*v[11]+PV[ 8]*v[12])+(PV[ 9]*v[13]+PV[13]*v[14]))*s;
  double k40 =((PV[10]*v[ 0]+PV[11]*v[ 1]+PV[12]*v[ 3])+(PV[13]*v[ 6]+PV[14]*v[10]))*s;
  double k41 =((PV[10]*v[ 1]+PV[11]*v[ 2]+PV[12]*v[ 4])+(PV[13]*v[ 7]+PV[14]*v[11]))*s;
  double k42 =((PV[10]*v[ 3]+PV[11]*v[ 4]+PV[12]*v[ 5])+(PV[13]*v[ 8]+PV[14]*v[12]))*s;
  double k43 =((PV[10]*v[ 6]+PV[11]*v[ 7]+PV[12]*v[ 8])+(PV[13]*v[ 9]+PV[14]*v[13]))*s;
  double k44 =((PV[10]*v[10]+PV[11]*v[11]+PV[12]*v[12])+(PV[13]*v[13]+PV[14]*v[14]))*s;

  // Residial production
  //
  double r[5]; differenceLocPar(K,M,P,r);

  // New parameters
  //
  double p0 =(k00*r[0]+k01*r[1]+k02*r[2])+(k03*r[3]+k04*r[4]); P[0]+=p0;
  double p1 =(k10*r[0]+k11*r[1]+k12*r[2])+(k13*r[3]+k14*r[4]); P[1]+=p1;
  double p2 =(k20*r[0]+k21*r[1]+k22*r[2])+(k23*r[3]+k24*r[4]); P[2]+=p2;
  double p3 =(k30*r[0]+k31*r[1]+k32*r[2])+(k33*r[3]+k34*r[4]); P[3]+=p3;
  double p4 =(k40*r[0]+k41*r[1]+k42*r[2])+(k43*r[3]+k44*r[4]); P[4]+=p4;

  // New covariance matrix
  //
  double v0  =(k00*PV[ 0]+k01*PV[ 1]+k02*PV[ 3])+(k03*PV[ 6]+k04*PV[10]);
  double v1  =(k10*PV[ 0]+k11*PV[ 1]+k12*PV[ 3])+(k13*PV[ 6]+k14*PV[10]);
  double v2  =(k10*PV[ 1]+k11*PV[ 2]+k12*PV[ 4])+(k13*PV[ 7]+k14*PV[11]);
  double v3  =(k20*PV[ 0]+k21*PV[ 1]+k22*PV[ 3])+(k23*PV[ 6]+k24*PV[10]);
  double v4  =(k20*PV[ 1]+k21*PV[ 2]+k22*PV[ 4])+(k23*PV[ 7]+k24*PV[11]);
  double v5  =(k20*PV[ 3]+k21*PV[ 4]+k22*PV[ 5])+(k23*PV[ 8]+k24*PV[12]);
  double v6  =(k30*PV[ 0]+k31*PV[ 1]+k32*PV[ 3])+(k33*PV[ 6]+k34*PV[10]);
  double v7  =(k30*PV[ 1]+k31*PV[ 2]+k32*PV[ 4])+(k33*PV[ 7]+k34*PV[11]);
  double v8  =(k30*PV[ 3]+k31*PV[ 4]+k32*PV[ 5])+(k33*PV[ 8]+k34*PV[12]);
  double v9  =(k30*PV[ 6]+k31*PV[ 7]+k32*PV[ 8])+(k33*PV[ 9]+k34*PV[13]);
  double v10 =(k40*PV[ 0]+k41*PV[ 1]+k42*PV[ 3])+(k43*PV[ 6]+k44*PV[10]);
  double v11 =(k40*PV[ 1]+k41*PV[ 2]+k42*PV[ 4])+(k43*PV[ 7]+k44*PV[11]);
  double v12 =(k40*PV[ 3]+k41*PV[ 4]+k42*PV[ 5])+(k43*PV[ 8]+k44*PV[12]);
  double v13 =(k40*PV[ 6]+k41*PV[ 7]+k42*PV[ 8])+(k43*PV[ 9]+k44*PV[13]);
  double v14 =(k40*PV[10]+k41*PV[11]+k42*PV[12])+(k43*PV[13]+k44*PV[14]);
  PV[ 0]-=v0 ;
  PV[ 1]-=v1 ; PV[ 2]-=v2 ;
  PV[ 3]-=v3 ; PV[ 4]-=v4 ; PV[ 5]-=v5 ;
  PV[ 6]-=v6 ; PV[ 7]-=v7 ; PV[ 8]-=v8 ; PV[ 9]-=v9 ;
  PV[10]-=v10; PV[11]-=v11; PV[12]-=v12; PV[13]-=v13; PV[14]-=v14;

  if(PV[0]<=0.||PV[2]<=0.||PV[5]<=0.||PV[9]<=0.||PV[14]<=0.) return false;
 
  if(X) xi2 = Xi2(5,r,v);
  return true;
}

///////////////////////////////////////////////////////////////////
// Transformation track parameters to updator presentation 
// true if it is measured track parameters
///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::trackParametersToUpdator
(const Trk::TrackParameters& T,double* P,double* V) const
{
  P[0] = T.parameters()[0];
  P[1] = T.parameters()[1];
  P[2] = T.parameters()[2];
  P[3] = T.parameters()[3];
  P[4] = T.parameters()[4];


  const AmgSymMatrix(5)* v = T.covariance();
  if(!v) return false;

  const AmgSymMatrix(5)& c = *v;
  V[ 0] = c(0,0); 
  V[ 1] = c(1,0);
  V[ 2] = c(1,1);
  V[ 3] = c(2,0);
  V[ 4] = c(2,1); 
  V[ 5] = c(2,2);
  V[ 6] = c(3,0); 
  V[ 7] = c(3,1);
  V[ 8] = c(3,2); 
  V[ 9] = c(3,3);
  V[10] = c(4,0);
  V[11] = c(4,1);
  V[12] = c(4,2);
  V[13] = c(4,3);
  V[14] = c(4,4);
  return true;
}

///////////////////////////////////////////////////////////////////
// Transformation track parameters to updator presentation 
// true if it is measured track parameters
///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::trackParametersToUpdator
(const Trk::PatternTrackParameters& T,double* P,double* V) const
{
  const double* par = T.par();

  P[0] = par[0];
  P[1] = par[1];
  P[2] = par[2];
  P[3] = par[3];
  P[4] = par[4];

  if(!T.iscovariance()) return false; 

  const double* cov = T.cov();

  V[ 0] = cov[ 0];
  V[ 1] = cov[ 1];
  V[ 2] = cov[ 2];
  V[ 3] = cov[ 3];
  V[ 4] = cov[ 4];
  V[ 5] = cov[ 5];
  V[ 6] = cov[ 6];
  V[ 7] = cov[ 7];
  V[ 8] = cov[ 8];
  V[ 9] = cov[ 9];
  V[10] = cov[10];
  V[11] = cov[11];
  V[12] = cov[12];
  V[13] = cov[13];
  V[14] = cov[14];
  return true;
}

///////////////////////////////////////////////////////////////////
// Transformation local parameters to updator presentation
// N - dimension of local parameters,K - key  word 
// P - parameters, V -covariance 
///////////////////////////////////////////////////////////////////

bool  Trk::KalmanUpdator_xk::localParametersToUpdator
(const Trk::LocalParameters& L,const Amg::MatrixX& C,
 int& N,int& K,double* P,double* V) const
{

  N = C.rows(); if(N==0 || N>5 || L.dimension()!=N) return false;
  K = L.parameterKey();

  //const CLHEP::HepVector& H = L;

  P[ 0]=L(0); 
  V[ 0]=C(0,0);

  if(N>1) {
    P[ 1]=L(1);
    V[ 1]=C(1,0); V[ 2]=C(1,1);
  }
  if(N>2) {
    P[ 2]=L(2);
    V[ 3]=C(2,0); V[ 4]=C(2,1); V[ 5]=C(2,2);
  }
  if(N>3) {
    P[ 3]=L(3);
    V[ 6]=C(3,0); V[ 7]=C(3,1); V[ 8]=C(3,2); V[ 9]=C(3,3);
  }
  if(N>4) {
    P[ 4]=L(4);
    V[10]=C(4,0); V[11]=C(4,1); V[12]=C(4,2); V[13]=C(4,3); V[14]=C(4,4);
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Track parameters production from updator presentation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* Trk::KalmanUpdator_xk::updatorToTrackParameters
(const Trk::TrackParameters& T,double* P,double* V) const
{  
  AmgSymMatrix(5)* e = new AmgSymMatrix(5);

  (*e)<<
    V[ 0],V[ 1],V[ 3],V[ 6],V[10],
    V[ 1],V[ 2],V[ 4],V[ 7],V[11],
    V[ 3],V[ 4],V[ 5],V[ 8],V[12],
    V[ 6],V[ 7],V[ 8],V[ 9],V[13],
    V[10],V[11],V[12],V[13],V[14];
  return T.associatedSurface().createTrackParameters(P[0],P[1],P[2],P[3],P[4],e); 
}

///////////////////////////////////////////////////////////////////
// Inversion of a positive definite symmetric matrix 
// with size (2x2), (3x3), (4x4) and (5x5)                 
// Input parameters  : n - size of matrix
//                     a - the elements of the lower triangle of     
//                               the matrix to be inverted                 
// Output parameters : b -  inverted matrix                           
///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::invert(int n,double* a,double* b) const 
{
  if(n==2) return invert2(a,b);
  if(n==3) return invert3(a,b);
  if(n==4) return invert4(a,b);
  if(n==5) return invert5(a,b);
  return false;
}

///////////////////////////////////////////////////////////////////
// Inversion of a positive definite symmetric matrix (2x2)                 
// using Kramer's rule                                                                            
// Input parameters  : a(0/2) -  the elements of the lower triangle of     
//                               the matrix to be inverted                 
//                               0                                         
//                               1   2                                     
// Output parameters : b(0/2) -  inverted matrix                           
///////////////////////////////////////////////////////////////////

bool  Trk::KalmanUpdator_xk::invert2 (double* a,double* b) const 
{
  double d  = a[0]*a[2]-a[1]*a[1]; if(d<=0.) return false; d=1./d;
  double b0 = a[2]*d; 
  b[1]      =-a[1]*d;
  b[2]      = a[0]*d;
  b[0]      =     b0;
  return true;
}

///////////////////////////////////////////////////////////////////
// Inversion of a positive definite symmetric matrix (3x3)                 
// using Kramer's rule                                                                            
// Input parameters  : a(0/5) -  the elements of the lower triangle of     
//                               the matrix to be inverted                 
//                               0                                         
//                               1   2                                     
//                               3   4  5                                  
// Output parameters : b(0/5) -  inverted matrix                           
///////////////////////////////////////////////////////////////////

bool  Trk::KalmanUpdator_xk::invert3(double* a,double* b) const 
{
  double b0 = (a[2]*a[5]-a[4]*a[4]);
  double b1 =-(a[1]*a[5]-a[3]*a[4]);
  double b2 = (a[0]*a[5]-a[3]*a[3]);
  double b3 = (a[1]*a[4]-a[2]*a[3]);
  double b4 =-(a[0]*a[4]-a[1]*a[3]);
  double b5 = (a[0]*a[2]-a[1]*a[1]);
  double  d = a[0]*b0+a[1]*b1+a[3]*b3; if(d<=0.) return false; 
  b[0]    = b0*(d=1./d);
  b[1]    = b1*d;
  b[2]    = b2*d;
  b[3]    = b3*d;
  b[4]    = b4*d;
  b[5]    = b5*d;
  return true;
}

///////////////////////////////////////////////////////////////////
// Inversion of a positive definite symmetric matrix (4x4)       
// using Kramer's rule 
// Input parameters  : a(0/9) - the elements of the lower triangle of         
//                               the matrix to be inverted                     
//                               0                                             
//                               1   2                                         
//                               3   4  5                                      
//                               6   7  8  9                                   
// Output parameters : b(0/9) - inverted matrix                               
///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::invert4(double* a,double* b) const
{
  double d00 = a[1]*a[4]-a[2]*a[3];
  double d01 = a[1]*a[5]-a[4]*a[3];
  double d02 = a[2]*a[5]-a[4]*a[4];
  double d03 = a[1]*a[7]-a[2]*a[6];
  double d04 = a[1]*a[8]-a[4]*a[6];
  double d05 = a[1]*a[9]-a[7]*a[6];
  double d06 = a[2]*a[8]-a[4]*a[7];
  double d07 = a[2]*a[9]-a[7]*a[7];
  double d08 = a[3]*a[7]-a[4]*a[6];
  double d09 = a[3]*a[8]-a[5]*a[6];
  double d10 = a[3]*a[9]-a[8]*a[6];
  double d11 = a[4]*a[8]-a[5]*a[7];
  double d12 = a[4]*a[9]-a[8]*a[7];
  double d13 = a[5]*a[9]-a[8]*a[8];
  
  // Determinant calculation
  //
  double c0  = a[2]*d13-a[4]*d12+a[7]*d11; 
  double c1  = a[1]*d13-a[4]*d10+a[7]*d09;
  double c2  = a[1]*d12-a[2]*d10+a[7]*d08;
  double c3  = a[1]*d11-a[2]*d09+a[4]*d08;
  double det = a[0]*c0-a[1]*c1+a[3]*c2-a[6]*c3;
  
  if (det <= 0.) return false;
  det = 1./det;
   
  b[2] =  (a[0]*d13-a[3]*d10+a[6]*d09)*det;
  b[4] = -(a[0]*d12-a[1]*d10+a[6]*d08)*det;
  b[5] =  (a[0]*d07-a[1]*d05+a[6]*d03)*det;
  b[7] =  (a[0]*d11-a[1]*d09+a[3]*d08)*det;
  b[8] = -(a[0]*d06-a[1]*d04+a[3]*d03)*det;
  b[9] =  (a[0]*d02-a[1]*d01+a[3]*d00)*det;
  b[0] =  c0                          *det; 
  b[1] = -c1                          *det;
  b[3] =  c2                          *det;
  b[6] = -c3                          *det;
  return true;
}

///////////////////////////////////////////////////////////////////
// Inversion of a positive definite symmetric matrix (5x5)                     
// by a modification of the Gauss-Jordan method                                
//                                                                             
// Input parameters  : a(0/14) - the elements of the lower triangle of         
//                               the matrix to be inverted                     
//                               0                                             
//                               1   2                                         
//                               3   4  5                                      
//                               6   7  8  9                                   
//                               10 11 12 13 14                                
// Output parameters : b(0/14) - inverted matrix                               
///////////////////////////////////////////////////////////////////

bool Trk::KalmanUpdator_xk::invert5(double* a,double* b) const
{
  if(a[ 0] <=0.) return false;
  double x1    = 1./a[ 0];
  double x2    =-a[ 1]*x1;
  double x3    =-a[ 3]*x1;
  double x4    =-a[ 6]*x1;
  double x5    =-a[10]*x1;
  double b0    = a[ 2]+a[ 1]*x2; if(b0<=0.) return false;
  double b1    = a[ 4]+a[ 3]*x2;
  double b2    = a[ 5]+a[ 3]*x3;
  double b3    = a[ 7]+a[ 6]*x2;
  double b4    = a[ 8]+a[ 6]*x3;
  double b5    = a[ 9]+a[ 6]*x4;
  double b6    = a[11]+a[10]*x2;
  double b7    = a[12]+a[10]*x3;
  double b8    = a[13]+a[10]*x4;
  double b9    = a[14]+a[10]*x5;
  double y1    = 1./b0;
  double y2    =-b1*y1;
  double y3    =-b3*y1;
  double y4    =-b6*y1;
  double y5    = x2*y1;

  if((b0 = b2+b1*y2) <=0.) return false;
  b1           = b4+b3*y2;
  b2           = b5+b3*y3;
  b3           = b7+b6*y2;
  b4           = b8+b6*y3;
  b5           = b9+b6*y4;
  b6           = x3+x2*y2;
  b7           = x4+x2*y3;
  b8           = x5+x2*y4;
  b9           = x1+x2*y5;
  x1           = 1./b0;
  x2           =-b1*x1; 
  x3           =-b3*x1;
  x4           = b6*x1;
  x5           = y2*x1;
  if((b0 = b2+b1*x2) <=0.) return  false;
  b1           = b4+b3*x2;
  b2           = b5+b3*x3;
  b3           = b7+b6*x2;
  b4           = b8+b6*x3;
  b5           = b9+b6*x4;
  b6           = y3+y2*x2;
  b7           = y4+y2*x3;
  b8           = y5+y2*x4;
  b9           = y1+y2*x5;
  y1           = 1./b0;
  y2           =-b1*y1; 
  y3           = b3*y1;
  y4           = b6*y1;
  y5           = x2*y1;
  if((b0 = b2+b1*y2) <=0.) return false;
  b1           = b4+b3*y2;
  b2           = b5+b3*y3;
  b3           = b7+b6*y2;
  b4           = b8+b6*y3;
  b5           = b9+b6*y4;
  b6           = x3+x2*y2;
  b7           = x4+x2*y3;
  b8           = x5+x2*y4;
  b9           = x1+x2*y5;
  b[14]        = 1./b0;     
  b[10]        = b1*b[14];
  b[11]        = b3*b[14];
  b[12]        = b6*b[14];
  b[13]        = y2*b[14];
  b[ 0]        = b2+b1*b[10];
  b[ 1]        = b4+b3*b[10];
  b[ 2]        = b5+b3*b[11];
  b[ 3]        = b7+b6*b[10];
  b[ 4]        = b8+b6*b[11];
  b[ 5]        = b9+b6*b[12];
  b[ 6]        = y3+y2*b[10];
  b[ 7]        = y4+y2*b[11];
  b[ 8]        = y5+y2*b[12];
  b[ 9]        = y1+y2*b[13];
  return true;
}

///////////////////////////////////////////////////////////////////
// Xi2 for 2x2, 3x3, 4x4 and 5x5 matrix calculation 
// R - residial  and W -weight matrix
///////////////////////////////////////////////////////////////////

double Trk::KalmanUpdator_xk::Xi2(int N,double* R,double* W) const
{
  if(N==1) return Xi2for1(R,W);
  if(N==2) return Xi2for2(R,W);
  if(N==3) return Xi2for3(R,W);
  if(N==4) return Xi2for4(R,W);
  if(N==5) return Xi2for5(R,W);
  return 0.;
}

///////////////////////////////////////////////////////////////////
// Xi2 of 1x1 matrix and 1 parameters
// R - residial  and W -weight matrix
///////////////////////////////////////////////////////////////////

double Trk::KalmanUpdator_xk::Xi2for1(double* R,double* W) const
{
  double Xi2 = R[0]*W[0]*R[0];
  return Xi2;
}

///////////////////////////////////////////////////////////////////
// Xi2 of 2x2 matrix and 2 parameters
// R - residial  and W -weight matrix
///////////////////////////////////////////////////////////////////

double Trk::KalmanUpdator_xk::Xi2for2(double* R,double* W) const
{
  double Xi2 =
    (R[0]*W[ 0]+R[1]*W[ 1])*R[0]+
    (R[0]*W[ 1]+R[1]*W[ 2])*R[1];
  return Xi2;
}

///////////////////////////////////////////////////////////////////
// Xi2 of 3x3 matrix and 3 parameters
// R - residial  and W -weight matrix
///////////////////////////////////////////////////////////////////

double Trk::KalmanUpdator_xk::Xi2for3(double* R,double* W) const
{
  double Xi2 =
    (R[0]*W[ 0]+R[1]*W[ 1]+R[2]*W[ 3])*R[0]+
    (R[0]*W[ 1]+R[1]*W[ 2]+R[2]*W[ 4])*R[1]+
    (R[0]*W[ 3]+R[1]*W[ 4]+R[2]*W[ 5])*R[2];
  return Xi2;
}

///////////////////////////////////////////////////////////////////
// Xi2 of 4x4 matrix and 4 parameters
// R - residial  and W -weight matrix
///////////////////////////////////////////////////////////////////

double Trk::KalmanUpdator_xk::Xi2for4(double* R,double* W) const
{
  double Xi2 =
    ((R[0]*W[ 0]+R[1]*W[ 1])+(R[2]*W[ 3]+R[3]*W[ 6]))*R[0]+
    ((R[0]*W[ 1]+R[1]*W[ 2])+(R[2]*W[ 4]+R[3]*W[ 7]))*R[1]+
    ((R[0]*W[ 3]+R[1]*W[ 4])+(R[2]*W[ 5]+R[3]*W[ 8]))*R[2]+
    ((R[0]*W[ 6]+R[1]*W[ 7])+(R[2]*W[ 8]+R[3]*W[ 9]))*R[3];
  return Xi2;
}

///////////////////////////////////////////////////////////////////
// Xi2 of 5x5 matrix and 5 parameters
// R - residial  and W -weight matrix
///////////////////////////////////////////////////////////////////

double Trk::KalmanUpdator_xk::Xi2for5(double* R,double* W) const
{
  double Xi2 =
    ((R[0]*W[ 0]+R[1]*W[ 1]+R[2]*W[ 3])+(R[3]*W[ 6]+R[4]*W[10]))*R[0]+
    ((R[0]*W[ 1]+R[1]*W[ 2]+R[2]*W[ 4])+(R[3]*W[ 7]+R[4]*W[11]))*R[1]+
    ((R[0]*W[ 3]+R[1]*W[ 4]+R[2]*W[ 5])+(R[3]*W[ 8]+R[4]*W[12]))*R[2]+
    ((R[0]*W[ 6]+R[1]*W[ 7]+R[2]*W[ 8])+(R[3]*W[ 9]+R[4]*W[13]))*R[3]+
    ((R[0]*W[10]+R[1]*W[11]+R[2]*W[12])+(R[3]*W[13]+R[4]*W[14]))*R[4];
  return Xi2;
}

///////////////////////////////////////////////////////////////////
// Residial between local and track parameters calculation
// L - local parameters, T - track parameters, R-residial
///////////////////////////////////////////////////////////////////

int Trk::KalmanUpdator_xk::differenceParLoc
(int K,double* L,double* T, double* R) const 
{
  const double pi2 = 2.*M_PI, pi = M_PI;
  
  int i = 0;
  if(K &  1) {R[i]=L[i]-T[0]; ++i;} 
  if(K &  2) {R[i]=L[i]-T[1]; ++i;}
  if(K &  4) {
    R[i]=L[i]-T[2]; 
    if     (R[i] > pi) R[i] = fmod(R[i]+pi,pi2)-pi;
    else if(R[i] <-pi) R[i] = fmod(R[i]-pi,pi2)+pi;
    ++i;
  }
  if(K &  8) {
    R[i]=L[i]-T[3]; 
    if     (R[i] > pi) R[i] = fmod(R[i]+pi,pi2)-pi;
    else if(R[i] <-pi) R[i] = fmod(R[i]-pi,pi2)+pi;
    ++i;
  }
  if(K & 16) {R[i]=L[i]-T[4]; ++i;}  
  return i;
}

///////////////////////////////////////////////////////////////////
// Residial between track parameters and local parameters calculation
// L - local parameters, T - track parameters, R-residial
///////////////////////////////////////////////////////////////////

void Trk::KalmanUpdator_xk::differenceLocPar
(int K,double* L,double* T, double* R) const 
{
  const double pi2 = 2.*M_PI, pi = M_PI;
  int i = 0;
  R[0]=0.; if(K &  1) R[0]=L[i++]-T[0];
  R[1]=0.; if(K &  2) R[1]=L[i++]-T[1]; 
  R[2]=0.; if(K &  4) {
    R[2]=L[i++]-T[2]; 
    if     (R[2] > pi) R[2] = fmod(R[2]+pi,pi2)-pi;
    else if(R[2] <-pi) R[2] = fmod(R[2]-pi,pi2)+pi;
  }
  R[3]=0.; if(K &  8){
    R[3]=L[i++]-T[3]; 
    if     (R[3] > pi) R[3] = fmod(R[3]+pi,pi2)-pi;
    else if(R[3] <-pi) R[3] = fmod(R[3]-pi,pi2)+pi;
  }
  R[4]=0.; if(K & 16) R[4]=L[i++]-T[4];
}

///////////////////////////////////////////////////////////////////
// Map for different combination of Local parameters preparation
///////////////////////////////////////////////////////////////////

void Trk::KalmanUpdator_xk::mapKeyProduction()
{
  int n=0; m_key[0]=m_key[1]=0;

  for(int K=1; K!= 32; ++K) {

    unsigned int I[5]={0,0,0,0,0}, m=0; 
    for(int i=0; i!=5; ++i) {if((K>>i)&1) I[i]=1;}

    for(int i=0; i!=5; ++i) {
      for(int j=0; j<=i; ++j) {if(I[i] && I[j]) {m_map[n++] = m;} ++m;}
    }
    m_key[K+1] = n;
  }
}

/////////////////////////////////////////////////////////////////////////////////
// Test angles inside boundaries
// Azimuthal angle p[2] shoud be > -pi and < +pi  
// Polar     angle p[3] shoud be >  0  and < +pi
/////////////////////////////////////////////////////////////////////////////////

void Trk::KalmanUpdator_xk::testAngles(double* p,double* v) const
{
  const double pi2 = 2.*M_PI, pi = M_PI;

  // Polar     angle check
  //
  if     (p[3] > pi) p[3] = fmod(p[3]+pi,pi2)-pi;
  else if(p[3] <-pi) p[3] = fmod(p[3]-pi,pi2)+pi;
  
  if     (p[3] < 0.) {

    p[ 3] = -p[ 3];
    p[ 2]+=     pi;
    v[ 6] = -v[ 6]; 
    v[ 7] = -v[ 7]; 
    v[ 8] = -v[ 8]; 
    v[13] = -v[13];
  }

  // Azimuthal angle check
  //
  if     (p[2] > pi) p[2] = fmod(p[2]+pi,pi2)-pi;
  else if(p[2] <-pi) p[2] = fmod(p[2]-pi,pi2)+pi;
}


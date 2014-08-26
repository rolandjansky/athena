/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>

#include "TrkSurfaces/PerigeeSurface.h" 
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "SiCombinatorialTrackFinderTool_xk/SiTrajectoryElement_xk.h"

#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"

///////////////////////////////////////////////////////////////////
// Set trajectory element
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::set
(int                                               st,
 const InDet::SiDetElementBoundaryLink_xk*&        dl,
 const InDet::SiClusterCollection::const_iterator& sb, 
 const InDet::SiClusterCollection::const_iterator& se,
 const InDet::SiCluster*                           si
 )
{
  
  m_status       = 0                       ;
  m_detstatus    = st                      ;
  m_nlinksF      = 0                       ;
  m_nlinksB      = 0                       ;
  m_nholesF      = 0                       ;
  m_nholesB      = 0                       ;
  m_dholesF      = 0                       ;
  m_dholesB      = 0                       ;
  m_nclustersF   = 0                       ;
  m_nclustersB   = 0                       ;
  m_ndfF         = 0                       ;
  m_ndfB         = 0                       ;
  m_ntsos        = 0                       ;
  m_detelement   = dl->detElement()        ;
  m_detlink      = dl                      ;
  m_surface      = &m_detelement->surface();
  m_sibegin      = sb                      ;
  m_siend        = se                      ; 
  m_cluster      = si                      ;
  m_clusterOld   = si                      ;
  m_clusterNoAdd = 0                       ;
  m_stereo       = false                   ; 
  m_xi2F         = 10000.                  ;
  m_xi2B         = 10000.                  ;
  m_xi2totalF    = 0.                      ;
  m_xi2totalB    = 0.                      ;
  m_halflenght   = 0.                      ;
  m_detelement->isSCT() ? m_ndf=1 : m_ndf=2;
  noiseInitiate()                          ;
  (m_detelement->isSCT() && m_detelement->design().shape()==InDetDD::Trapezoid) ? 
    m_stereo = true : m_stereo = false;

  if(m_detstatus && m_ndf == 1) m_halflenght = (*sb)->width().z()*.5;

  if(!m_detstatus) {

    IdentifierHash idHash = m_detelement->identifyHash();
    if(m_ndf==2) {if(!m_tools->pixcond()->isGood(idHash)) m_detstatus = -1;}
    else         {if(!m_tools->sctcond()->isGood(idHash)) m_detstatus = -1;}
  }
  if(m_surface) return true; return false;
}

///////////////////////////////////////////////////////////////////
// Set work information to trajectory
///////////////////////////////////////////////////////////////////

void InDet::SiTrajectoryElement_xk::setTools(const InDet::SiTools_xk* t)
{
  m_tools = t;
} 

///////////////////////////////////////////////////////////////////
// Initiate first element of trajectory using external 
// track parameters
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::firstTrajectorElement
(const Trk::TrackParameters& Tpa)
{
  if(!m_cluster) return false;     

  Trk::PatternTrackParameters Tp; if(!Tp.production(&Tpa)) return false;

  const Trk::Surface* pl = Tp.associatedSurface();

  // Track propagation if need
  //
  if(m_surface==pl) m_parametersPF = Tp;
  else  if(!propagate(Tp,m_parametersPF,m_step)) return false;

  // Initiate track parameters without initial covariance
  //
  double cv[15]={ 1. ,
		  0. , 1.,
		  0. , 0.,.001,
		  0. , 0.,  0.,.001,
		  0. , 0.,  0.,  0.,.00001};
  
  m_parametersPF.setCovariance(cv);
  initiateState(m_parametersPF,m_parametersUF);

  noiseProduction(1,m_parametersUF);

  m_step         =  0.  ;
  m_xi2F         =  0.  ;
  m_xi2totalF    =  0.  ;
  m_status       =  1   ;
  m_inside       = -1   ;
  m_nlinksF      =  0   ;
  m_nholesF      =  0   ;
  m_dholesF      =  0   ;
  m_clusterNoAdd =  0   ;
  m_nclustersF   =  1   ;
  m_ndfF         = m_ndf;
  return true;
}

///////////////////////////////////////////////////////////////////
// Initiate first element of trajectory using smoother result
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::firstTrajectorElement()
{

  if(!m_cluster || !m_status) return false;
  if(m_status > 1 ) m_parametersPF = m_parametersUB;

  m_parametersPF.diagonalization(100.);
  if(!initiateState(m_parametersPF,m_parametersUF)) return false;
  noiseProduction(1,m_parametersUF);

  m_xi2F         =  0.  ;
  m_xi2totalF    =  0.  ;
  m_status       =  1   ;
  m_inside       = -1   ;
  m_nlinksF      =  0   ;
  m_nholesF      =  0   ;
  m_dholesF      =  0   ;
  m_clusterNoAdd =  0   ;
  m_nclustersF   =  1   ;
  m_ndfF         = m_ndf;
  return true;
}

///////////////////////////////////////////////////////////////////
// Initiate last element of trajectory 
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::lastTrajectorElement()
{
  if(m_status==0 || !m_cluster) return false; 
  noiseProduction(1,m_parametersUF);

  m_parametersSM = m_parametersPF;
  m_parametersPB = m_parametersUF;
  m_parametersPB.diagonalization(100.);
  if(!initiateState(m_parametersPB,m_parametersUB)) return false;

  m_status       =      3 ;
  m_inside       =     -1 ;
  m_nlinksB      =      0 ;
  m_nholesB      =      0 ;
  m_dholesB      =      0 ;
  m_clusterNoAdd =      0 ;
  m_nclustersB   =      1 ;
  m_ndfB         =  m_ndf ;
  m_xi2B         =  m_xi2F;
  m_xi2totalB    =  m_xi2F;
  return true;
}

///////////////////////////////////////////////////////////////////
// Propagate information in forward direction without closest 
// clusters search
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::ForwardPropagationWithoutSearch
(InDet::SiTrajectoryElement_xk& TE)
{
  // Track propagation
  // 
  if(TE.m_cluster) {

    TE.m_parametersUF.addNoise   (TE.m_noise,Trk::alongMomentum);
    if(!propagate(TE.m_parametersUF,m_parametersPF,m_step)) return false;
    TE.m_parametersUF.removeNoise(TE.m_noise,Trk::alongMomentum);
    m_dholesF = 0;
  }
  else             {

    TE.m_parametersPF.addNoise   (TE.m_noise,Trk::alongMomentum);
    if(!propagate(TE.m_parametersPF,m_parametersPF,m_step)) return false;
    TE.m_parametersPF.removeNoise(TE.m_noise,Trk::alongMomentum);
    m_dholesF = TE.m_dholesF;
  }

  m_nclustersF = TE.m_nclustersF;
  m_nholesF    = TE.m_nholesF   ; 
  m_ndfF       = TE.m_ndfF      ;
  m_xi2totalF  = TE.m_xi2totalF ;
  m_step      += TE.m_step      ;  

  // Track update
  //
  if( m_cluster) {

    if(!addCluster(m_parametersPF,m_parametersUF,m_xi2F)) return false;
    m_inside  = -1; ++m_nclustersF; m_xi2totalF+=m_xi2F; m_ndfF+=m_ndf;
  }
  else           {

    m_inside = m_detlink->intersect(m_parametersPF);
    if(m_inside < 0 && m_detstatus >=0) {++m_nholesF; ++m_dholesF;}
  }

  // Noise production
  //
  if(m_inside<=0) {

    if(m_cluster) noiseProduction(1,m_parametersUF);
    else          noiseProduction(1,m_parametersPF);
  }
  else            {
    noiseInitiate();
  }
  m_status       = 1;
  m_nlinksF      = 0;
  m_clusterNoAdd = 0;
  return true;
}

///////////////////////////////////////////////////////////////////
// Propagate information in forward direction with closest 
// clusters search
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::ForwardPropagationWithSearch
(InDet::SiTrajectoryElement_xk& TE)
{
  // Track propagation
  // 
  if(TE.m_cluster) {

    TE.m_parametersUF.addNoise   (TE.m_noise,Trk::alongMomentum);
    if(!propagate(TE.m_parametersUF,m_parametersPF,m_step)) return false; 
    TE.m_parametersUF.removeNoise(TE.m_noise,Trk::alongMomentum);
    m_dholesF = 0;
  }
  else             {

    TE.m_parametersPF.addNoise   (TE.m_noise,Trk::alongMomentum);
    if(!propagate(TE.m_parametersPF,m_parametersPF,m_step)) return false; 
    TE.m_parametersPF.removeNoise(TE.m_noise,Trk::alongMomentum);
    m_dholesF = TE.m_dholesF;
  }

  m_status       =         1                           ;
  m_nlinksF      =         0                           ;
  m_clusterOld   = m_cluster                           ;
  m_cluster      =         0                           ;
  m_clusterNoAdd =         0                           ;  
  m_xi2F         =    10000.                           ;
  m_inside       = m_detlink->intersect(m_parametersPF);
  m_nholesF      = TE.m_nholesF                        ;
  m_nclustersF   = TE.m_nclustersF                     ; 
  m_ndfF         = TE.m_ndfF                           ;
  m_xi2totalF    = TE.m_xi2totalF                      ;
  m_step        += TE.m_step                           ;
  
  if(m_inside > 0) {noiseInitiate(); return true;}
  
  if((m_nlinksF=searchClusters(m_parametersPF,m_linkF))) {

    m_xi2F =  m_linkF[0].xi2();

    if     (m_xi2F <= m_tools->xi2max()    ) {

      m_cluster = m_linkF[0].cluster();
      if(!addCluster(m_parametersPF,m_parametersUF)) return false;
      noiseProduction(1,m_parametersUF);
      ++m_nclustersF; m_xi2totalF+=m_xi2F; m_ndfF+=m_ndf;
    }
    else if(m_xi2F <= m_tools->xi2maxNoAdd()) { 
	
      m_clusterNoAdd =  m_linkF[0].cluster();
      noiseProduction(1,m_parametersPF);
    }
  }
  else {
    noiseProduction(1,m_parametersPF);
  }

  if(m_inside < 0 && m_detstatus >=0 && !m_cluster && !m_clusterNoAdd) {++m_nholesF; ++m_dholesF;}
  return true;
}

///////////////////////////////////////////////////////////////////
// Forward propagation for search closest cluster
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::ForwardPropagationForClusterSeach
 (int n,const Trk::TrackParameters& Tpa,
 const InDet::SiDetElementBoundaryLink_xk*&        dl,
 const InDet::SiClusterCollection::const_iterator& sb, 
 const InDet::SiClusterCollection::const_iterator& se)
{
  m_detstatus    = 1                       ;
  m_sibegin      = sb                      ;
  m_siend        = se                      ; 
  m_detelement   = dl->detElement()        ;
  m_detlink      = dl                      ;
  m_surface      = &m_detelement->surface();
  m_detelement->isSCT() ? m_ndf=1 : m_ndf=2;
  m_halflenght   = 0.                      ;
  m_stereo       = false                   ;

  (m_detelement->isSCT() && m_detelement->design().shape()==InDetDD::Trapezoid) ? 
    m_stereo = true : m_stereo = false;

  if(m_detstatus && m_ndf == 1) m_halflenght = (*sb)->width().z()*.5;

  if(!n) {
    Trk::PatternTrackParameters Tp; if(!Tp.production(&Tpa)) return false;
    if(!propagateParameters(Tp,m_parametersPF,m_step)) return false;
 
   if(!m_parametersPF.iscovariance()) {

      double cv[15]={ .02 ,
	 	       0., .02,
		       0., 0.,.000001,
		       0., 0.,   0.,.000001,
		       0., 0.,   0.,   0.,.000000001};
      
      m_parametersPF.setCovariance(cv);
    }
  }
  else {
    if(!propagate(m_parametersPF,m_parametersPF,m_step)) return false;
  }
  m_nlinksF=searchClusters(m_parametersPF,m_linkF);
  return true;
}

///////////////////////////////////////////////////////////////////
// Forward propagation for search closest cluster
///////////////////////////////////////////////////////////////////

void InDet::SiTrajectoryElement_xk::CloseClusterSeach
 (Trk::PatternTrackParameters& Tpa,
 const InDet::SiDetElementBoundaryLink_xk*&        dl,
 const InDet::SiClusterCollection::const_iterator& sb, 
 const InDet::SiClusterCollection::const_iterator& se)
{
  m_detstatus    = 1                       ;
  m_cluster      = 0                       ;
  m_sibegin      = sb                      ;
  m_siend        = se                      ; 
  m_detelement   = dl->detElement()        ;
  m_detlink      = dl                      ;
  m_surface      = &m_detelement->surface();
  m_detelement->isSCT() ? m_ndf=1 : m_ndf=2;
  m_halflenght   = 0.                      ;
  m_stereo       = false                   ;

  (m_detelement->isSCT() && m_detelement->design().shape()==InDetDD::Trapezoid) ? 
    m_stereo = true : m_stereo = false;
  if(m_detstatus && m_ndf == 1) m_halflenght = (*sb)->width().z()*.5;

  if(m_detlink->intersect(Tpa) > 0 || !searchClusters(Tpa,m_linkF)) return;
  m_cluster = m_linkF[0].cluster();
  m_xi2F    = m_linkF[0].xi2    ();
}

///////////////////////////////////////////////////////////////////
// Backward propagation for filter
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::BackwardPropagationFilter
(InDet::SiTrajectoryElement_xk& TE)
{
  // Track propagation
  // 
  if(TE.m_noise.correctionIMom() < 1.) {

    if(TE.m_cluster) {

      TE.m_parametersUB.addNoise   (TE.m_noise,Trk::oppositeMomentum);
      if(!propagate(TE.m_parametersUB,m_parametersPB,m_step)) return false; 
      TE.m_parametersUB.removeNoise(TE.m_noise,Trk::oppositeMomentum);
      m_dholesB = 0;
    }
    else             {

      TE.m_parametersPB.addNoise   (TE.m_noise,Trk::oppositeMomentum);
      if(!propagate(TE.m_parametersPB,m_parametersPB,m_step)) return false; 
      TE.m_parametersPB.removeNoise(TE.m_noise,Trk::oppositeMomentum);
      m_dholesB = TE.m_dholesB;
    }
  }
  else                                  {

    if(TE.m_cluster) {

      if(!propagate(TE.m_parametersUB,m_parametersPB,m_step)) return false; 
      m_dholesB = 0;
    }
    else             {

      if(!propagate(TE.m_parametersPB,m_parametersPB,m_step)) return false; 
      m_dholesB = TE.m_dholesB;
    }
  }
  m_status       =         2;
  m_nlinksB      =         0;
  m_clusterOld   = m_cluster;
  m_cluster      =         0;
  m_clusterNoAdd =         0;
  m_xi2B         =    10000.;
  m_inside       = m_detlink->intersect(m_parametersPB);
  m_nholesB      = TE.m_nholesB                        ;
  m_nclustersB   = TE.m_nclustersB                     ; 
  m_ndfB         = TE.m_ndfB                           ;
  m_xi2totalB    = TE.m_xi2totalB                      ;
  m_step        += TE.m_step                           ;

  if(m_inside >0 ) {noiseInitiate(); return true;}
  
  if((m_nlinksB = searchClusters(m_parametersPB,m_linkB))) {

    m_xi2B =  m_linkB[0].xi2();
    
    if     (m_xi2B <= m_tools->xi2max()     ) {
      
      m_cluster = m_linkB[0].cluster();
      if(!addCluster(m_parametersPB,m_parametersUB)) return false;
      noiseProduction(-1,m_parametersUB);
      ++m_nclustersB; m_xi2totalB+=m_xi2B; m_ndfB+=m_ndf;
    }
    else if(m_xi2B <= m_tools->xi2maxNoAdd()) { 
      
      m_clusterNoAdd =  m_linkB[0].cluster();
      noiseProduction(-1,m_parametersPB);
    }
  }
  else {
    noiseProduction(-1,m_parametersPB);
  }

  if(m_inside < 0 && m_detstatus >=0 && !m_cluster && !m_clusterNoAdd) {++m_nholesB; ++m_dholesB;}
  return true;
}

///////////////////////////////////////////////////////////////////
// Backward propagation for smoother  
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::BackwardPropagationSmoother
(InDet::SiTrajectoryElement_xk& TE,bool isTwoSpacePointsSeed)
{
  
  int                         dir = -1;
  if(m_step - TE.m_step > 0.) dir =  1;

  // Track propagation
  //
  double step;
  if(TE.m_cluster) {

    if(!propagate(TE.m_parametersUB,m_parametersPB,dir,step)) return false;
    m_dholesB = 0;
  }
  else             {

    if(!propagate(TE.m_parametersPB,m_parametersPB,dir,step)) return false;
    m_dholesB = TE.m_dholesB;
  }
  
  m_parametersPB.addNoise(m_noise,Trk::oppositeMomentum);

  // Forward-backward predict parameters
  //
  if(!combineStates(m_parametersPB,m_parametersPF,m_parametersSM)) return false;

  m_cluster ? m_status = 3 : m_status = 2;

  double Xi2max = m_tools->xi2max(); if( isTwoSpacePointsSeed) Xi2max*=2.;
  m_inside       = m_detlink->intersect(m_parametersSM);
  m_nlinksB      =         0               ;
  m_clusterOld   = m_cluster               ;
  m_cluster      =         0               ;
  m_clusterNoAdd =         0               ;
  m_xi2B         =    10000.               ;
  m_nholesB      = TE.m_nholesB            ;
  m_nclustersB   = TE.m_nclustersB         ; 
  m_ndfB         = TE.m_ndfB               ; 
  m_xi2totalB    = TE.m_xi2totalB          ;
  //m_step        += TE.m_step               ;
  if(m_inside> 0 ) return true;


  // For not first cluster on trajectory
  //
  if(!m_clusterOld || m_ndfF != m_ndf || m_ndfF+m_ndfB >6) {

    if((m_nlinksB = searchClusters(m_parametersSM,m_linkB))) {
    
      m_xi2B = m_linkB[0].xi2();
      
      
      if     (m_xi2B <= Xi2max) {

	m_cluster = m_linkB[0].cluster();

	if(!addCluster(m_parametersPB,m_parametersUB)) return false;
	++m_nclustersB; m_xi2totalB+=m_xi2B; m_ndfB+=m_ndf;
      }
      else if(m_xi2B <= m_tools->xi2maxNoAdd()) {
	
	m_clusterNoAdd = m_linkB[0].cluster();
      }
    }
    if(m_inside < 0 && m_detstatus >=0 && !m_cluster && !m_clusterNoAdd) {++m_nholesB; ++m_dholesB;}
    return true;
  }

  // For first cluster of short trajectory
  //
  m_cluster =  m_clusterOld;
  if(!addCluster(m_parametersPB,m_parametersUB,m_xi2B)) return false;

  if     (m_xi2B <=          Xi2max       ) {++m_nclustersB; m_xi2totalB+=m_xi2B; m_ndfB+=m_ndf;}
  else if(m_xi2B <= m_tools->xi2maxNoAdd()) {m_cluster      = 0; m_clusterNoAdd = m_clusterOld; }
  else                                      {m_cluster      = 0;                                }
  if(m_inside < 0 && !m_cluster && !m_clusterNoAdd) {++m_nholesB; ++m_dholesB;}
  return true;
}

///////////////////////////////////////////////////////////////////
// Add next cluster for backward propagation
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::addNextClusterB()
{
  if(m_nlinksB <= 0) return false; 

  if(m_cluster) m_xi2totalB-=m_xi2B;

  if(m_nlinksB > 1 && m_linkB[1].xi2() <= m_tools->xi2max()) {
  
    int n = 0; 
    for(; n!=m_nlinksB-1; ++n) m_linkB[n]=m_linkB[n+1]; m_nlinksB=n;

    m_cluster   = m_linkB[0].cluster();
    m_xi2B      = m_linkB[0].xi2()    ;
    m_xi2totalB+= m_xi2B              ;
    if(!addCluster(m_parametersPB,m_parametersUB)) return false;
  }
  else {
    m_nlinksB = 0;
    m_cluster = 0; --m_nclustersB; m_ndfB-=m_ndf;
    if(m_inside < 0 ) {++m_nholesB; ++m_dholesB;} m_xi2B = 0.;
  }
  return true;
} 

///////////////////////////////////////////////////////////////////
// Add next cluster for forward propagation
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::addNextClusterF()
{
  if(m_nlinksF <= 0) return false;
 
  if(m_cluster) m_xi2totalF-=m_xi2F;

  if(m_nlinksF > 1 && m_linkF[1].xi2() <= m_tools->xi2max()) {
  
    int n = 0; 
    for(; n!=m_nlinksF-1; ++n) m_linkF[n]=m_linkF[n+1]; m_nlinksF=n;
    
    m_cluster   = m_linkF[0].cluster();
    m_xi2F      = m_linkF[0].xi2()    ;
    m_xi2totalF+= m_xi2F              ;
    if(!addCluster(m_parametersPF,m_parametersUF)) return false;
  }
  else            {
    m_nlinksF = 0;
    m_cluster = 0; --m_nclustersF; m_ndfF-=m_ndf;
    if(m_inside < 0) {++m_nholesF; ++m_dholesF;} m_xi2F = 0.;
  }
  return true;
} 

///////////////////////////////////////////////////////////////////
// Add next cluster for backward propagation
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::addNextClusterB
(InDet::SiTrajectoryElement_xk& TE,const InDet::SiCluster* Cl)
{

  m_clusterOld = m_cluster      ;
  m_cluster    = Cl             ;
  m_nclustersB = TE.m_nclustersB;
  m_ndfB       = TE.m_ndfB      ;
  m_nholesB    = TE.m_nholesB   ;
  m_xi2totalB  = TE.m_xi2totalB ; 

  TE.m_cluster ? m_dholesB = 0 : m_dholesB = TE.m_dholesB;

  if(Cl) {

    if(!addCluster(m_parametersPB,m_parametersUB,m_xi2B)) return false;
    m_inside    = -1; noiseProduction(-1,m_parametersUB);
    ++m_nclustersB; m_xi2totalB+=m_xi2B; m_ndfB+=m_ndf; 
  }
  else  {

    if(m_inside < 0) {++m_nholesB; ++m_dholesB;} m_xi2B = 0.;
  }
  return true;
} 

///////////////////////////////////////////////////////////////////
// Add next cluster for forward propagation
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::addNextClusterF
(InDet::SiTrajectoryElement_xk& TE,const InDet::SiCluster* Cl)
{

  m_clusterOld = m_cluster      ;
  m_cluster    = Cl             ;
  m_nclustersF = TE.m_nclustersF;
  m_ndfF       = TE.m_ndfF      ;
  m_nholesF    = TE.m_nholesF   ;
  m_xi2totalF  = TE.m_xi2totalF ; 

  TE.m_cluster ? m_dholesF = 0 : m_dholesF = TE.m_dholesF;
  
  if(Cl) {

    if(!addCluster(m_parametersPF,m_parametersUF,m_xi2F)) return false;
    m_inside    = -1; noiseProduction(1,m_parametersUF);
    ++m_nclustersF; m_xi2totalF+=m_xi2F; m_ndfF+=m_ndf; 
  }
  else  {

    if(m_inside < 0) {++m_nholesF; ++m_dholesF;} m_xi2F = 0.;
  }
  return true;
} 

///////////////////////////////////////////////////////////////////
// Search closest cluster with stereo angle
///////////////////////////////////////////////////////////////////

int  InDet::SiTrajectoryElement_xk::searchClustersWithStereo
(Trk::PatternTrackParameters& Tp,InDet::SiClusterLink_xk* L) 
{
  int nl     = 0;  if(m_detstatus<=0) return 0;

  double P0  = Tp.par()[0];
  double P1  = Tp.par()[1];
  double PV0 = Tp.cov()[0];
  double PV1 = Tp.cov()[1];
  double PV2 = Tp.cov()[2];
  double Xc  = m_tools->xi2maxlink ();

  InDet::SiClusterCollection::const_iterator p =  m_sibegin;

  for(; p!=m_siend; ++p) {

    const InDet::SiCluster*      c = (*p);
    const Amg::Vector2D&         M = c->localPosition();
    const Amg::MatrixX&          V = c->localCovariance();

    double MV0 = V(0,0);
    double MV1 = V(1,0);
    double MV2 = V(1,1);
    double v0  = MV0+PV0;
    double v1  = MV1+PV1;
    double v2  = MV2+PV2;
    double r0  = M[0]-P0;
    double r1  = M[1]-P1;
    double d   = v0*v2-v1*v1; if(d<=0.) continue; d=1./d;
    double x   = (r0*(r0*v2-r1*v1)+r1*(r1*v0-r0*v1))*d;
    if(x > Xc) continue;
    
    r1  = fabs(r1+d*((PV1*v2-PV2*v1)*r0+(PV2*v0-PV1*v1)*r1));  
    x  -= (r1*r1)/MV2                                       ;
    r1 -= m_halflenght                                      ;
    
    if(r1 > 0. &&  (x+=((r1*r1)/PV2)) > Xc) continue;
   
    InDet::SiClusterLink_xk l(c,x);
    for(int i=0; i!=nl; ++i) L[i].Comparison(l); if(nl<10) L[nl++]=l;
  }
  return nl;
}

///////////////////////////////////////////////////////////////////
// Search closest cluster without stereo angle for pixels
///////////////////////////////////////////////////////////////////

int  InDet::SiTrajectoryElement_xk::searchClustersWithoutStereoPIX
(Trk::PatternTrackParameters& Tp,InDet::SiClusterLink_xk* L) 
{
  int nl     = 0; if(m_detstatus<=0) return 0;

  double P0  = Tp.par()[0];
  double P1  = Tp.par()[1];
  double PV0 = Tp.cov()[0];
  double PV1 = Tp.cov()[1];
  double PV2 = Tp.cov()[2];
  double Xc  = m_tools->xi2maxlink ();

  InDet::SiClusterCollection::const_iterator p =  m_sibegin;

  for(; p!=m_siend; ++p) {
    
    const InDet::SiCluster*  c  = (*p); 
    const Amg::Vector2D&     M = c->localPosition();

    double MV0,MV1,MV2; patternCovariances(c,MV0,MV1,MV2);

    double v0  = MV0+PV0;
    double v1  = MV1+PV1;
    double v2  = MV2+PV2;
    double r0  = M[0]-P0;
    double r1  = M[1]-P1;
    double d   = v0*v2-v1*v1; if(d<=0.) continue;
    double x   = (r0*(r0*v2-r1*v1)+r1*(r1*v0-r0*v1))/d;

    if(x>Xc) continue;

    InDet::SiClusterLink_xk l(c,x);
    for(int i=0; i!=nl; ++i) L[i].Comparison(l); if(nl<10) L[nl++]=l;
  }
  return nl;
} 

///////////////////////////////////////////////////////////////////
// Search closest cluster without stereo angle for SCT 
///////////////////////////////////////////////////////////////////

int  InDet::SiTrajectoryElement_xk::searchClustersWithoutStereoSCT
(Trk::PatternTrackParameters& Tp,InDet::SiClusterLink_xk* L) 
{
  const double covb = .25;

  int nl     = 0; if(m_detstatus<=0) return 0;

  double P0  = Tp.par()[0];
  double P1  = Tp.par()[1];
  double PV0 = Tp.cov()[0];
  double PV1 = Tp.cov()[1];
  double PV2 = Tp.cov()[2];
  double Xc  = m_tools->xi2maxlink ();
  InDet::SiClusterCollection::const_iterator p =  m_sibegin;

  for(; p!=m_siend; ++p) {
    
    const InDet::SiCluster*      c = (*p); 
    const Amg::Vector2D&         M = c->localPosition();
    const Amg::MatrixX&          V = c->localCovariance();

    double MV0 = c->width().phiR(); MV0*=(MV0*.08333);

    if(MV0 < V(0,0)) MV0=V(0,0);

    double r0  = M[0]-P0;
    double v0  = MV0+PV0; if(v0<=0.) continue; 
    double d   = 1./v0  ;
    double x   = r0*r0*d;
    
    if(x>Xc) continue   ;
    
    double dP1 = P1+PV1*d*r0-M[1];

    if(fabs(dP1) > m_halflenght) {

      double r1; 
      dP1 > m_halflenght ? r1 = m_halflenght-P1 : r1 = -m_halflenght-P1;
      
      double v1 =      PV1     ;
      double v2 = covb+PV2     ;  
      d           = v0*v2-v1*v1; 
      if(d > 0.) x = (r0*r0*v2+r1*r1*v0-2.*r0*r1*v1)/d;
      else       x = 1000.;
      if(x>Xc) continue;
    }

    InDet::SiClusterLink_xk l(c,x);
    for(int i=0; i!=nl; ++i) L[i].Comparison(l); if(nl<10) L[nl++]=l;
  }
  return nl;
}

///////////////////////////////////////////////////////////////////
// Search closest cluster with stereo angle
///////////////////////////////////////////////////////////////////

int  InDet::SiTrajectoryElement_xk::searchClustersWithStereoAss
(Trk::PatternTrackParameters& Tp,InDet::SiClusterLink_xk* L) 
{
  int nl     = 0;  if(m_detstatus<=0) return 0;

  double P0  = Tp.par()[0];
  double P1  = Tp.par()[1];
  double PV0 = Tp.cov()[0];
  double PV1 = Tp.cov()[1];
  double PV2 = Tp.cov()[2];
  double Xc  = m_tools->xi2maxlink ();

  InDet::SiClusterCollection::const_iterator p =  m_sibegin;

  for(; p!=m_siend; ++p) {

    const InDet::SiCluster*      c = (*p);
    if(m_tools->assoTool()->isUsed(*c)) continue;

    const Amg::Vector2D&         M = c->localPosition();
    const Amg::MatrixX&          V = c->localCovariance();

    double MV0 = V(0,0);
    double MV1 = V(1,0);
    double MV2 = V(1,1);
    double v0  = MV0+PV0;
    double v1  = MV1+PV1;
    double v2  = MV2+PV2;
    double r0  = M[0]-P0;
    double r1  = M[1]-P1;
    double d   = v0*v2-v1*v1; if(d<=0.) continue; d=1./d; 
    double x   = (r0*(r0*v2-r1*v1)+r1*(r1*v0-r0*v1))*d;
    if(x > Xc) continue;
    
    r1  = fabs(r1+d*((PV1*v2-PV2*v1)*r0+(PV2*v0-PV1*v1)*r1));  
    x  -= (r1*r1)/MV2                                       ;
    r1 -= m_halflenght                                      ;
    
    if(r1 > 0. &&  (x+=((r1*r1)/PV2)) > Xc) continue;
   
    InDet::SiClusterLink_xk l(c,x);
    for(int i=0; i!=nl; ++i) L[i].Comparison(l); if(nl<10) L[nl++]=l;
  }
  return nl;
}

///////////////////////////////////////////////////////////////////
// Search closest cluster without stereo angle for pixels
///////////////////////////////////////////////////////////////////

int  InDet::SiTrajectoryElement_xk::searchClustersWithoutStereoAssPIX
(Trk::PatternTrackParameters& Tp,InDet::SiClusterLink_xk* L) 
{
  int nl     = 0; if(m_detstatus<=0) return 0;

  double P0  = Tp.par()[0];
  double P1  = Tp.par()[1];
  double PV0 = Tp.cov()[0];
  double PV1 = Tp.cov()[1];
  double PV2 = Tp.cov()[2];
  double Xc  = m_tools->xi2maxlink ();

  InDet::SiClusterCollection::const_iterator p =  m_sibegin;

  for(; p!=m_siend; ++p) {
    
    const InDet::SiCluster*  c  = (*p); 
    if(m_tools->assoTool()->isUsed(*c)) continue;

    const Amg::Vector2D& M = c->localPosition();

    double MV0,MV1,MV2; patternCovariances(c,MV0,MV1,MV2);

    double v0  = MV0+PV0;
    double v1  = MV1+PV1;
    double v2  = MV2+PV2;
    double r0  = M[0]-P0;
    double r1  = M[1]-P1;
    double d   = v0*v2-v1*v1; if(d<=0.) continue; 
    double x   = (r0*(r0*v2-r1*v1)+r1*(r1*v0-r0*v1))/d; 

    if(x>Xc) continue;

    InDet::SiClusterLink_xk l(c,x);
    for(int i=0; i!=nl; ++i) L[i].Comparison(l); if(nl<10) L[nl++]=l;
  }
  return nl;
} 

///////////////////////////////////////////////////////////////////
// Search closest cluster without stereo angle for SCT 
///////////////////////////////////////////////////////////////////

int  InDet::SiTrajectoryElement_xk::searchClustersWithoutStereoAssSCT
(Trk::PatternTrackParameters& Tp,InDet::SiClusterLink_xk* L) 
{
  const double covb = 1.;

  int nl     = 0; if(m_detstatus<=0) return 0;

  double P0  = Tp.par()[0];
  double P1  = Tp.par()[1];
  double PV0 = Tp.cov()[0];
  double PV1 = Tp.cov()[1];
  double PV2 = Tp.cov()[2];
  double Xc  = m_tools->xi2maxlink ();
  InDet::SiClusterCollection::const_iterator p =  m_sibegin;

  for(; p!=m_siend; ++p) {
    
    const InDet::SiCluster*      c = (*p); 
    if(m_tools->assoTool()->isUsed(*c)) continue;

    const Amg::Vector2D&    M = c->localPosition();
    const Amg::MatrixX&     V = c->localCovariance();

    double MV0 = c->width().phiR(); MV0*=(MV0*.08333);

    if(MV0 < V(0,0)) MV0=V(0,0);

    double r0  = M[0]-P0;
    double v0  = MV0+PV0; if(v0<=0.) continue;
    double d   = 1./v0  ;
    double x   = r0*r0*d;
    
    if(x>Xc) continue   ;
    
    double dP1 = P1+PV1*d*r0-M[1];

    if(fabs(dP1) > m_halflenght) {

      double r1; 
      dP1 > m_halflenght ? r1 = m_halflenght-P1 : r1 = -m_halflenght-P1;
      
      double v1 =      PV1     ;
      double v2 = covb+PV2     ;  
      d           = v0*v2-v1*v1; 
      if(d > 0.) x = (r0*r0*v2+r1*r1*v0-2.*r0*r1*v1)/d;
      else       x = 1000.;
      if(x>Xc) continue;
    }

    InDet::SiClusterLink_xk l(c,x);
    for(int i=0; i!=nl; ++i) L[i].Comparison(l); if(nl<10) L[nl++]=l;
  }
  return nl;
}

///////////////////////////////////////////////////////////////////
// TrackStateOnSurface production 
///////////////////////////////////////////////////////////////////

Trk::TrackStateOnSurface*  
InDet::SiTrajectoryElement_xk::trackStateOnSurface (bool change,bool cov,bool multi,int Q)
{
  const Trk::TrackParameters    * tp = 0;
  if(!change)                     tp = trackParameters                (cov,Q);
  else                            tp = trackParametersWithNewDirection(cov,Q); 
  if(!tp) return 0;
  if(&tp->associatedSurface()!=m_surface) {delete tp; return 0;}
  
  const Trk::FitQualityOnSurface* fq = 0;
  const Trk::MeasurementBase    * ro = 0;

  if     (m_status == 1) fq = new Trk::FitQualityOnSurface(m_xi2F,m_ndf);
  else                   fq = new Trk::FitQualityOnSurface(m_xi2B,m_ndf);

  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pat(0);
  
  if(m_cluster) {
    ro = m_tools->rioTool()->correct(*m_cluster     ,*tp);
    pat.set(Trk::TrackStateOnSurface::Measurement);
  }
  else          {
    ro = m_tools->rioTool()->correct(*m_clusterNoAdd,*tp);
    pat.set(Trk::TrackStateOnSurface::Outlier    );
  } 
  const Trk::ScatteringAngles* sa = new Trk::ScatteringAngles
    (0.,0.,sqrt(m_noise.covarianceAzim()),sqrt(m_noise.covariancePola()));

  const Trk::MaterialEffectsOnTrack* me = new Trk::MaterialEffectsOnTrack
    (m_radlengthN,sa,tp->associatedSurface());

  pat.set(Trk::TrackStateOnSurface::Scatterer);
  Trk::TrackStateOnSurface* sos = new Trk::TrackStateOnSurface(ro,tp,fq,me,pat);  
  
  m_tsos[0] = sos;  m_utsos[0] = true; m_ntsos = 1;

  if(multi && m_cluster && m_ndf==2 && m_nlinksB > 1) {

    for(int i=1; i!= m_nlinksB; ++i) {

      if(m_linkB[i].xi2() > m_tools->xi2multi()) break; 

      const Trk::TrackParameters    * tpn = 0;
      if(!change)                     tpn = trackParameters                (cov,Q);
      else                            tpn = trackParametersWithNewDirection(cov,Q); 
      if(!tpn) break;;

      const Trk::FitQualityOnSurface   * fqn = new Trk::FitQualityOnSurface(m_linkB[i].xi2(),m_ndf);
      const Trk::MeasurementBase       * ron = m_tools->rioTool()->correct(*m_linkB[i].cluster(),*tp);
      const Trk::MaterialEffectsOnTrack* men = new Trk::MaterialEffectsOnTrack(*me); 
      m_tsos [m_ntsos] = new Trk::TrackStateOnSurface(ron,tpn,fqn,men,pat);  
      m_utsos[m_ntsos] = false;
      if(++m_ntsos == 3) break;
    }
  }
  return sos;
}

///////////////////////////////////////////////////////////////////
// TrackStateOnSurface production for simple track
///////////////////////////////////////////////////////////////////

Trk::TrackStateOnSurface*  
InDet::SiTrajectoryElement_xk::trackSimpleStateOnSurface 
(bool change,bool cov,int Q)
{
  const Trk::TrackParameters* tp = 0;

  if(Q) {
    if(!change) tp = trackParameters                (cov,Q);
    else        tp = trackParametersWithNewDirection(cov,Q); 
    if(&tp->associatedSurface()!=m_surface) {delete tp; return 0;}
  }

  IdentifierHash  iH = m_detelement->identifyHash();
  const Trk::MeasurementBase* ro  = 0;
  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pat(0);

  const InDet::SiCluster* cl = 0;
  if(m_cluster) {
    cl = m_cluster;      pat.set(Trk::TrackStateOnSurface::Measurement);
  } 
  else          { 
    cl = m_clusterNoAdd; pat.set(Trk::TrackStateOnSurface::Outlier    );
  }
  pat.set(Trk::TrackStateOnSurface::Scatterer);

  Trk::LocalParameters   locp   = Trk::LocalParameters(cl->localPosition());
  Amg::MatrixX           cv     = cl->localCovariance();

  const Trk::FitQualityOnSurface* fq = 0;
  if     (m_status == 1) fq = new Trk::FitQualityOnSurface(m_xi2F,m_ndf);
  else                   fq = new Trk::FitQualityOnSurface(m_xi2B,m_ndf);

  if(m_ndf == 1) {

    const InDet::SCT_Cluster*  sc  = dynamic_cast<const InDet::SCT_Cluster*> (cl); 
    if(sc)  ro =  new InDet::SCT_ClusterOnTrack (sc,locp,cv,iH,sc->globalPosition());
  }
  else           {

    const InDet::PixelCluster*  pc  = dynamic_cast<const InDet::PixelCluster*> (cl);
    if(pc) ro = new InDet::PixelClusterOnTrack(pc,locp,cv,iH,pc->globalPosition(),pc->gangedPixel());
  }
  return new Trk::TrackStateOnSurface(ro,tp,fq,0,pat);
}

///////////////////////////////////////////////////////////////////
// TrackStateOnSurface production for perigee
///////////////////////////////////////////////////////////////////

Trk::TrackStateOnSurface*  
InDet::SiTrajectoryElement_xk::trackPerigeeStateOnSurface ()
{
  if(m_parametersUB.associatedSurface()!=m_surface) return 0;
  
  double step                   ;
  Trk::PatternTrackParameters Tp; 

  Trk::PerigeeSurface per;

  bool Q = m_tools->propTool()->propagate
	(m_parametersUB,per,Tp,Trk::anyDirection,m_tools->fieldTool(),step,Trk::pion);

  if(Q) {
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>  typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Perigee);
    return new Trk::TrackStateOnSurface(0,Tp.convert(true),0,0,typePattern);
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
// TrackParameters production
// Q = 0 no first or last  element of the trajectory
// Q = 1             first element of the trajectory
// Q = 2             last  element of the tracjectory
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters*  
InDet::SiTrajectoryElement_xk::trackParameters(bool cov,int Q)
{
  if     (m_status == 1) {
    if(m_cluster) return m_parametersUF.convert(cov);
    else          return m_parametersPF.convert(cov);
    
  }
  else if(m_status == 2) {
    if(m_cluster) return m_parametersUB.convert(cov);
    else          return m_parametersPB.convert(cov);
  }
  else if(m_status == 3) {

    if(Q==0) {
      if(m_cluster) {
	if(addCluster(m_parametersSM,m_parametersSM))                return m_parametersSM.convert(cov);
	else if(m_parametersUB.cov()[14] < m_parametersPF.cov()[14]) return m_parametersUB.convert(cov);
	else                                                         return m_parametersPF.convert(cov);
      }
      else                                                           return m_parametersSM.convert(cov);
    }
    if(Q==1) {if(m_cluster) return m_parametersUB.convert(cov);}
    if(Q==2) {if(m_cluster) return m_parametersUF.convert(cov);}
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
// Noise production
// Dir  = +1 along momentum , -1 opposite momentum 
// Model = 1 - muon, 2 - electron 
///////////////////////////////////////////////////////////////////

void  InDet::SiTrajectoryElement_xk::noiseProduction
(int Dir,const Trk::PatternTrackParameters& Tp)
{

  int Model = m_noisemodel; 
  if(Model < 1 || Model > 2) return; 
  const Amg::Transform3D& T = m_surface->transform();
  
  const double* par  = Tp.par()           ;
  double        q    = fabs(par[4])       ;
  double        cosp = cos( par[3])       ;
  double       sinp2 = (1.-cosp)*(1.+cosp); if(sinp2 < .0000001) sinp2 = .0000001;
  double s     = 
    fabs(sqrt(sinp2)*(cos(par[2])*T(0,2)+sin(par[2]*T(1,2)))+cosp*T(2,2));
  s  < .05 ? s = 20. : s = 1./s; 

  m_radlengthN = s*m_radlength; 
  double covariancePola = 134.*m_radlengthN*q*q;
  double covarianceAzim = covariancePola/sinp2;
  double covarianceIMom,correctionIMom;

  if(Model==1) {
    double        dp = m_energylose*q*s;
    covarianceIMom = .2*dp*dp*q*q;
    correctionIMom = 1.-dp;
  }
  else {
    correctionIMom = .70;
    covarianceIMom = (correctionIMom-1.)*(correctionIMom-1.)*q*q;
  }
  if(Dir>0) correctionIMom = 1./correctionIMom;
  m_noise.set(covarianceAzim,covariancePola,covarianceIMom,correctionIMom);
}

///////////////////////////////////////////////////////////////////
// TrackParameters production with new direction
// Q = 0 no first or last  element of the trajectory
// Q = 1             first element of the trajectory
// Q = 2             last  element of the tracjectory
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters*  
InDet::SiTrajectoryElement_xk::trackParametersWithNewDirection(bool cov,int Q)
{
  if     (m_status == 1) {
    if(m_cluster) return trackParameters(m_parametersUF,cov);
    else          return trackParameters(m_parametersPF,cov);
    
  }
  else if(m_status == 2) {
    if(m_cluster) return trackParameters(m_parametersUB,cov);
    else          return trackParameters(m_parametersPB,cov);
  }
  else if(m_status == 3) {

    if(Q==0) {
      if(m_cluster) {
	if(addCluster(m_parametersSM,m_parametersSM))                return trackParameters(m_parametersSM,cov);
	else if(m_parametersUB.cov()[14] < m_parametersPF.cov()[14]) return trackParameters(m_parametersUB,cov);
	else                                                         return trackParameters(m_parametersPF,cov);
      }
      else                                                           return trackParameters(m_parametersSM,cov);
    }
    if(Q==1) {if(m_cluster) return trackParameters(m_parametersUB,cov);}
    if(Q==2) {if(m_cluster) return trackParameters(m_parametersUF,cov);}
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
// TrackParameters production with new direction
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters*  
InDet::SiTrajectoryElement_xk::trackParameters
(Trk::PatternTrackParameters& Tp,bool cov)
{
  Tp.changeDirection();
  return Tp.convert(cov);
}


///////////////////////////////////////////////////////////////////
// Step calculation 
///////////////////////////////////////////////////////////////////

double InDet::SiTrajectoryElement_xk::step
(InDet::SiTrajectoryElement_xk& TE)
{
  Trk::PatternTrackParameters Ta,Tb; 

  if    (TE.m_status == 1) {
    if(TE.m_cluster) Ta = TE.m_parametersUF;
    else             Ta = TE.m_parametersPF;
  }
  else if(TE.m_status == 2) {
    if(TE.m_cluster) Ta = TE.m_parametersUB;
    else             Ta = TE.m_parametersPB;
  }
  else if(TE.m_status == 3) {
    Ta = TE.m_parametersSM;
  }

  double step = 0.;
  bool   Q    = m_tools->propTool()->propagateParameters
    (Ta,*m_surface,Tb,Trk::anyDirection,m_tools->fieldTool(),step,Trk::pion);
  if(Q) return step;
  return 0.;
}

///////////////////////////////////////////////////////////////////
// Global position of track parameters
///////////////////////////////////////////////////////////////////

Amg::Vector3D InDet::SiTrajectoryElement_xk::globalPosition()
{
  if    (m_status == 1) {
    if(m_cluster)  return m_parametersUF.position();
    else           return m_parametersPF.position();
  }
  else if(m_status == 2) {
    if(m_cluster)  return m_parametersUB.position();
    else           return m_parametersPB.position();
  }
  else if(m_status == 3) {

    Amg::Vector3D gp(0.,0.,0.);
    bool QA; Trk::PatternTrackParameters S1,SM,S2(parametersPF()); 
    
    if(m_cluster) S1 = parametersUB();
    else          S1 = parametersPB();
    
    QA = m_tools->updatorTool()->combineStates(S1,S2,SM);
    
    if(QA) {
      gp = SM.position();
    }
    return gp;
  }
  Amg::Vector3D gp(0.,0.,0.);
  return gp;
}

///////////////////////////////////////////////////////////////////
// Step estimation to 0,0,0
///////////////////////////////////////////////////////////////////

double InDet::SiTrajectoryElement_xk::stepToPerigee()
{
  Amg::Vector3D M;
  Amg::Vector3D P; 
   
  if(m_cluster) {
    M = m_parametersUB.momentum();
    P = m_parametersUB.position();
  }  
  else          {
    M = m_parametersPB.momentum(); 
    P = m_parametersPB.position();
  }
  
  return -(P[0]*M[0]+P[1]*M[1]);
}

///////////////////////////////////////////////////////////////////
// Errase cluster fo forward propagation
///////////////////////////////////////////////////////////////////

void InDet::SiTrajectoryElement_xk::eraseClusterForwardPropagation()
{
  m_cluster=0; --m_nclustersF; m_xi2totalF-=m_xi2F; m_ndfF-=m_ndf;
}

///////////////////////////////////////////////////////////////////
// Quality of the trajectory element
///////////////////////////////////////////////////////////////////

double InDet::SiTrajectoryElement_xk::quality(int& holes) const
{

  if(!m_cluster && !m_clusterNoAdd) {
    
    if(m_detstatus < 0) return 0.;
    
    if     (m_inside <  0) {
      double w = 2.-m_tools->xi2max(); if(++holes > 1) w*=2.; return w;
    }
    else if(m_inside == 0) return -1.;
    else                   return  0.;
  }

  double w,X,Xc = m_tools->xi2max()+2.;
  m_status == 1 ? X = m_xi2F        : X = m_xi2B;
  m_ndf    == 2 ? w = 1.2*(Xc-X*.5) : w = Xc-X  ; if(w < -1.) w = -1.;
  holes    = 0;

  return w;
}

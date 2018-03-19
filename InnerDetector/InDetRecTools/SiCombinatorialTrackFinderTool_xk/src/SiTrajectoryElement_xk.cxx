/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>

#include "TrkSurfaces/PerigeeSurface.h" 
#include "TrkSurfaces/AnnulusBounds.h" 
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

void InDet::SiTrajectoryElement_xk::set
(int                                               st,
 const InDet::SiDetElementBoundaryLink_xk*&        dl,
 const InDet::SiClusterCollection::const_iterator& sb, 
 const InDet::SiClusterCollection::const_iterator& se,
 const InDet::SiCluster*                           si
 )
{
  m_fieldMode    = false; if(m_tools->fieldTool().magneticFieldMode()!=0) m_fieldMode = true;
  m_status       = 0                       ;
  m_detstatus    = st                      ;
  m_ndist        = 0                       ;
  m_nlinksF      = 0                       ;
  m_nlinksB      = 0                       ;
  m_nholesF      = 0                       ;
  m_nholesB      = 0                       ;
  m_dholesF      = 0                       ;
  m_dholesB      = 0                       ;
  m_nclustersF   = 0                       ;
  m_nclustersB   = 0                       ;
  m_npixelsB     = 0                       ;
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
  m_tools->electron() ? m_xi2max = m_tools->xi2maxBrem() : m_xi2max = m_tools->xi2max();
  m_halflenght   = 0.                      ;
  m_detelement->isSCT() ? m_ndf=1 : m_ndf=2;

  if(m_tools->heavyion()) {
    if(m_ndf==2) {m_xi2max = 13.; m_xi2maxNoAdd = 13.;}
    else         {m_xi2max =  4.; m_xi2maxNoAdd =  8.;}
  }

  noiseInitiate()                          ;
  (m_detelement->isSCT() && (m_detelement->design().shape()==InDetDD::Trapezoid || m_detelement->design().shape()==InDetDD::Annulus)   ) ? 
    m_stereo = true : m_stereo = false;

  if(m_detstatus && m_ndf == 1) m_halflenght = (*sb)->width().z()*.5;

  if(!m_detstatus) {

    IdentifierHash idHash = m_detelement->identifyHash();
    if(m_ndf==2) {if(!m_tools->pixcond()->isGood(idHash)) m_detstatus = -1;}
    else         {if(!m_tools->sctcond()->isGood(idHash)) m_detstatus = -1;}
  }
  
  const Amg::Transform3D& T  = m_surface->transform();

  m_Tr[ 0] = T(0,0); m_Tr[ 1]=T(1,0); m_Tr[ 2]=T(2,0);
  m_Tr[ 3] = T(0,1); m_Tr[ 4]=T(1,1); m_Tr[ 5]=T(2,1);
  m_Tr[ 6] = T(0,2); m_Tr[ 7]=T(1,2); m_Tr[ 8]=T(2,2);
  m_Tr[ 9] = T(0,3); m_Tr[10]=T(1,3); m_Tr[11]=T(2,3);
  m_Tr[12] = m_Tr[ 9]*m_Tr[ 6]+m_Tr[10]*m_Tr[ 7]+m_Tr[11]*m_Tr[ 8];
  m_A[0] = 1.; m_A[1] = 0.; m_A[2] = 0.;
  return;
}

///////////////////////////////////////////////////////////////////
// Set work information to trajectory
///////////////////////////////////////////////////////////////////

void InDet::SiTrajectoryElement_xk::setTools(const InDet::SiTools_xk* t)
{
  m_tools        = t                      ;
  m_useassoTool  = m_tools->useassoTool ();
  m_fieldService = m_tools->magfield    ();
  m_updatorTool  = m_tools->updatorTool ();
  m_proptool     = m_tools->propTool    ();
  m_riotool      = m_tools->rioTool     ();
  m_assoTool     = m_tools->assoTool    ();
} 

void InDet::SiTrajectoryElement_xk::setParameters()
{
  m_maxholes     = m_tools->maxholes   ()  ; 
  m_maxdholes    = m_tools->maxdholes  ()  ; 
  m_xi2max       = m_tools->xi2max     ()  ;
  m_xi2maxNoAdd  = m_tools->xi2maxNoAdd()  ;  
  m_xi2maxlink   = m_tools->xi2maxlink ()  ;
  m_xi2multi     = m_tools->xi2multi   ()  ;
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

  if(m_detelement->isDBM()) {

    double tn = tan(Tp.par()[3]);
    cv[ 5]    = .0001           ;
    cv[14]    = (tn*tn*1.e-6)   ;
  } 

  m_parametersPF.setCovariance(cv);
  initiateState(m_parametersPF,m_parametersUF);

  noiseProduction(1,m_parametersUF);

  m_dist         = -10. ;
  m_step         =  0.  ;
  m_xi2F         =  0.  ;
  m_xi2totalF    =  0.  ;
  m_status       =  1   ;
  m_inside       = -1   ;
  m_ndist        =  0   ;
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

  m_dist         = -10. ;
  m_xi2F         =  0.  ;
  m_xi2totalF    =  0.  ;
  m_status       =  1   ;
  m_inside       = -1   ;
  m_ndist        =  0   ;
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
  m_ndist        =      0 ;
  m_nlinksB      =      0 ;
  m_nholesB      =      0 ;
  m_dholesB      =      0 ;
  m_clusterNoAdd =      0 ;
  m_nclustersB   =      1 ; m_ndf == 2 ? m_npixelsB = 1 : m_npixelsB = 0;
  m_ndfB         =  m_ndf ;
  m_xi2B         =  m_xi2F;
  m_xi2totalB    =  m_xi2F;
  m_dist         =    -10.;
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
  m_ndist      = TE.m_ndist     ;
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

    m_inside = m_detlink->intersect(m_parametersPF,m_dist);

    if( m_detstatus >=0) {
      if(m_inside <  0 ) {++m_nholesF; ++m_dholesF;}
      if(m_dist   < -2.) ++m_ndist;
    }
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
  m_inside       = m_detlink->intersect(m_parametersPF,m_dist);
  m_nholesF      = TE.m_nholesF                        ;
  m_ndist        = TE.m_ndist                          ;
  m_nclustersF   = TE.m_nclustersF                     ; 
  m_ndfF         = TE.m_ndfF                           ;
  m_xi2totalF    = TE.m_xi2totalF                      ;
  m_step        += TE.m_step                           ;
  
  if(m_inside > 0) {noiseInitiate(); return true;}
  
  if((m_nlinksF=searchClusters(m_parametersPF,m_linkF))) {

    m_xi2F =  m_linkF[0].xi2();

    if     (m_xi2F <= m_xi2max    ) {

      m_cluster = m_linkF[0].cluster();
      if(!addCluster(m_parametersPF,m_parametersUF)) return false;
      noiseProduction(1,m_parametersUF);
      ++m_nclustersF; m_xi2totalF+=m_xi2F; m_ndfF+=m_ndf;
    }
    else if(m_xi2F <= m_xi2maxNoAdd) { 
	
      m_clusterNoAdd =  m_linkF[0].cluster();
      noiseProduction(1,m_parametersPF);
    }
  }
  else {
    noiseProduction(1,m_parametersPF);
  }
  
  if(m_detstatus >=0 && !m_cluster) {
    if(m_inside < 0 && !m_clusterNoAdd) {++m_nholesF; ++m_dholesF;} 
    if(m_dist < -2. ) ++m_ndist;
  }
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

  if(m_detlink->intersect(Tpa,m_dist) > 0 || !searchClusters(Tpa,m_linkF)) return;
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
  m_inside       = m_detlink->intersect(m_parametersPB,m_dist);
  m_nholesB      = TE.m_nholesB                        ;
  m_ndist        = TE.m_ndist                          ;
  m_nclustersB   = TE.m_nclustersB                     ; 
  m_npixelsB     = TE.m_npixelsB                       ;
  m_ndfB         = TE.m_ndfB                           ;
  m_xi2totalB    = TE.m_xi2totalB                      ;
  m_step        += TE.m_step                           ;

  if(m_inside >0 ) {noiseInitiate(); return true;}
  
  if((m_nlinksB = searchClusters(m_parametersPB,m_linkB))) {

    m_xi2B =  m_linkB[0].xi2();
    
    if     (m_xi2B <= m_xi2max     ) {
      
      m_cluster = m_linkB[0].cluster();
      if(!addCluster(m_parametersPB,m_parametersUB)) return false;
      noiseProduction(-1,m_parametersUB);
      ++m_nclustersB; m_xi2totalB+=m_xi2B; m_ndfB+=m_ndf; if(m_ndf==2) ++m_npixelsB;
    }
    else if(m_xi2B <= m_xi2maxNoAdd) { 
      
      m_clusterNoAdd =  m_linkB[0].cluster();
      noiseProduction(-1,m_parametersPB);
    }
  }
  else {
    noiseProduction(-1,m_parametersPB);
  }

  if(m_detstatus >=0 && !m_cluster){
    if(m_inside < 0 && !m_clusterNoAdd) {++m_nholesB; ++m_dholesB;}
    if(m_dist < -2.) ++m_ndist;
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Backward propagation for smoother  
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::BackwardPropagationSmoother
(InDet::SiTrajectoryElement_xk& TE,bool isTwoSpacePointsSeed)
{

  // Track propagation
  //
  double step;
  if(TE.m_cluster) {

    if(!propagate(TE.m_parametersUB,m_parametersPB,step)) return false;
    m_dholesB = 0;
  }
  else             {

    if(!propagate(TE.m_parametersPB,m_parametersPB,step)) return false;
    m_dholesB = TE.m_dholesB;
  }
  
  m_parametersPB.addNoise(m_noise,Trk::oppositeMomentum);

  // Forward-backward predict parameters
  //
  if(!combineStates(m_parametersPB,m_parametersPF,m_parametersSM)) return false;

  m_cluster ? m_status = 3 : m_status = 2;

  double Xi2max = m_xi2max; if( isTwoSpacePointsSeed) Xi2max*=2.;
  m_inside       = m_detlink->intersect(m_parametersSM,m_dist);
  m_nlinksB      =         0               ;
  m_clusterOld   = m_cluster               ;
  m_cluster      =         0               ;
  m_clusterNoAdd =         0               ;
  m_xi2B         =    10000.               ;
  m_nholesB      = TE.m_nholesB            ;
  m_ndist        = TE.m_ndist              ;
  m_nclustersB   = TE.m_nclustersB         ; 
  m_npixelsB     = TE.m_npixelsB           ;
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
	++m_nclustersB; m_xi2totalB+=m_xi2B; m_ndfB+=m_ndf; if(m_ndf==2) ++m_npixelsB;
      }
      else if(m_xi2B <= m_xi2maxNoAdd) {
	
	m_clusterNoAdd = m_linkB[0].cluster();
      }
    }
    if(m_detstatus >=0 && !m_cluster) {
      if(m_inside < 0 && !m_clusterNoAdd) {++m_nholesB; ++m_dholesB;}
      if(m_dist < -2.) ++m_ndist;
    } 
    return true;
  }

  // For first cluster of short trajectory
  //
  m_cluster =  m_clusterOld;
  if(!addCluster(m_parametersPB,m_parametersUB,m_xi2B)) return false;

  if     (m_xi2B <= Xi2max       ) {++m_nclustersB; m_xi2totalB+=m_xi2B; m_ndfB+=m_ndf; if(m_ndf==2) ++m_npixelsB;}
  else if(m_xi2B <= m_xi2maxNoAdd) {m_cluster      = 0; m_clusterNoAdd = m_clusterOld; }
  else                                      {m_cluster      = 0;                       }

  if(!m_cluster) {
    if(m_inside < 0 && !m_clusterNoAdd) {++m_nholesB; ++m_dholesB;}
    if(m_dist < -2.) ++m_ndist;

  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Add next cluster for backward propagation
///////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::addNextClusterB()
{
  if(m_nlinksB <= 0) return false; 

  if(m_cluster) m_xi2totalB-=m_xi2B;

  if(m_nlinksB > 1 && m_linkB[1].xi2() <= m_xi2max) {
  
    int n = 0; 
    for(; n!=m_nlinksB-1; ++n) m_linkB[n]=m_linkB[n+1];
    m_nlinksB=n;

    m_cluster   = m_linkB[0].cluster();
    m_xi2B      = m_linkB[0].xi2()    ;
    m_xi2totalB+= m_xi2B              ;
    if(!addCluster(m_parametersPB,m_parametersUB)) return false;
  }
  else {
    m_nlinksB = 0;
    m_cluster = 0; --m_nclustersB; m_ndfB-=m_ndf; if(m_ndf==2) --m_npixelsB;
    if(m_inside < 0 ) {++m_nholesB; ++m_dholesB;} m_xi2B = 0.;
    if(m_dist   < -2.) ++m_ndist;
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

  if(m_nlinksF > 1 && m_linkF[1].xi2() <= m_xi2max) {
  
    int n = 0; 
    for(; n!=m_nlinksF-1; ++n) m_linkF[n]=m_linkF[n+1];
    m_nlinksF=n;
    
    m_cluster   = m_linkF[0].cluster();
    m_xi2F      = m_linkF[0].xi2()    ;
    m_xi2totalF+= m_xi2F              ;
    if(!addCluster(m_parametersPF,m_parametersUF)) return false;
  }
  else            {
    m_nlinksF = 0;
    m_cluster = 0; --m_nclustersF; m_ndfF-=m_ndf;
    if(m_inside < 0) {++m_nholesF; ++m_dholesF;} m_xi2F = 0.;
    if(m_dist   < -2.) ++m_ndist;
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
  m_npixelsB   = TE.m_npixelsB  ;
  m_ndfB       = TE.m_ndfB      ;
  m_nholesB    = TE.m_nholesB   ;
  m_ndist      = TE.m_ndist     ;
  m_xi2totalB  = TE.m_xi2totalB ; 

  TE.m_cluster ? m_dholesB = 0 : m_dholesB = TE.m_dholesB;

  if(Cl) {

    if(!addCluster(m_parametersPB,m_parametersUB,m_xi2B)) return false;
    m_inside    = -1; noiseProduction(-1,m_parametersUB);
    ++m_nclustersB; m_xi2totalB+=m_xi2B; m_ndfB+=m_ndf; if(m_ndf==2) ++m_npixelsB;
  }
  else  {

    if(m_inside < 0) {++m_nholesB; ++m_dholesB;} m_xi2B = 0.;
    if(m_dist < -2.) ++m_ndist;
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
  m_ndist      = TE.m_ndist     ;
  m_xi2totalF  = TE.m_xi2totalF ; 

  TE.m_cluster ? m_dholesF = 0 : m_dholesF = TE.m_dholesF;
  
  if(Cl) {

    if(!addCluster(m_parametersPF,m_parametersUF,m_xi2F)) return false;
    m_inside    = -1; noiseProduction(1,m_parametersUF);
    ++m_nclustersF; m_xi2totalF+=m_xi2F; m_ndfF+=m_ndf; 
  }
  else  {

    if(m_inside < 0) {++m_nholesF; ++m_dholesF;} m_xi2F = 0.;
    if(m_dist < -2.) ++m_ndist;
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
  double Xc  = m_xi2maxlink;
  double Xl  = m_xi2maxlink;
  double Xm  = m_xi2max    ;

  const InDet::SiCluster* cl = 0;

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

    if(x < Xm) {
      InDet::SiClusterLink_xk l(c,x);
      for(int i=0; i!=nl; ++i) L[i].Comparison(l);
      if(nl<10) L[nl++]=l; else Xm=L[9].xi2();
      Xc = Xm+6.;
    }
    else if(!nl && x < Xl) {Xl = x; Xc = x+6.; cl = c;}
  }
  if(cl && !nl) {L[nl++].Set(cl,Xl);}
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
  double Xc  = m_xi2maxlink;
  double Xm  = m_xi2max    ;

  const InDet::SiCluster* cl = 0;

  InDet::SiClusterCollection::const_iterator p =  m_sibegin;

  for(; p!=m_siend; ++p) {
    
    const InDet::SiCluster*  c  = (*p); 
    const Amg::Vector2D&     M = c->localPosition();

    double MV0 = c->width().phiR();
    double MV2 = c->width().z   ();

    double r0  = M[0]-P0, r02 = r0*r0; 
    double r1  = M[1]-P1, r12 = r1*r1;

    double v0  = .08333*(MV0*MV0)+PV0; if(r02 >(Xc*v0)) continue;
    double v2  = .08333*(MV2*MV2)+PV2; if(r12 >(Xc*v2)) continue;
    double v1  =                  PV1;
    double d   = v0*v2-v1*v1;          if(   d<=0.    ) continue;
    double x   = (r02*v2+r12*v0-(r0*r1)*(2.*v1))/d;

    if(x>Xc) continue;

    if(x < Xm) {
      InDet::SiClusterLink_xk l(c,x);
      for(int i=0; i!=nl; ++i) L[i].Comparison(l);
      if(nl<10) L[nl++]=l; else Xm=L[9].xi2();
      Xc = Xm;
    }
    else if(!nl) {Xc = x; cl = c;}
  }
  if(cl && !nl) {L[nl++].Set(cl,Xc);}
  return nl;
} 

///////////////////////////////////////////////////////////////////
// Search closest cluster without stereo angle for SCT 
///////////////////////////////////////////////////////////////////

int  InDet::SiTrajectoryElement_xk::searchClustersWithoutStereoSCT
(Trk::PatternTrackParameters& Tp,InDet::SiClusterLink_xk* L) 
{
  int nl     = 0; if(m_detstatus<=0) return 0;

  double P0  = Tp.par()[0];
  double P1  = Tp.par()[1];
  double PV0 = Tp.cov()[0];
  double PV1 = Tp.cov()[1];
  double PV2 = Tp.cov()[2];
  double Xc  = m_xi2maxlink;
  double Xm  = m_xi2max    ;

  const InDet::SiCluster* cl = 0;

  InDet::SiClusterCollection::const_iterator p =  m_sibegin;

  for(; p!=m_siend; ++p) {
    
    const InDet::SiCluster*      c = (*p); 
    const Amg::Vector2D&         M = c->localPosition();

    double MV0 = c->width().phiR()   ;
    double v0  = .08333*(MV0*MV0)+PV0;   
    double r0  = M[0]-P0;
    double d   = 1./v0;
    double x   = (r0*r0)*d;
    
    if(x>Xc) continue;
    
    double dP1 = (P1-M[1])+PV1*d*r0;

    if(fabs(dP1) > m_halflenght) {

      double r1; 
      dP1 > m_halflenght ? r1 = m_halflenght-P1 : r1 = -(m_halflenght+P1);
      
      double v1 = PV1;
      double v2 = PV2;  
      d = v0*v2-v1*v1  ; if(d<=0.) continue; 
      x = (r0*(r0*v2-r1*v1)+r1*(r1*v0-r0*v1))/d;
      if(x>Xc) continue;
    }

    if(x < Xm) {
      InDet::SiClusterLink_xk l(c,x);
      for(int i=0; i!=nl; ++i) L[i].Comparison(l);
      if(nl<10) L[nl++]=l; else Xm=L[9].xi2();
      Xc = Xm;
    }
    else if(!nl) {Xc = x; cl = c;}
  }
  if(cl && !nl) {L[nl++].Set(cl,Xc);}
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
  double Xc  = m_xi2maxlink;
  double Xl  = m_xi2maxlink;
  double Xm  = m_xi2max    ;

  const InDet::SiCluster* cl = 0;

  InDet::SiClusterCollection::const_iterator p =  m_sibegin;

  for(; p!=m_siend; ++p) {

    const InDet::SiCluster*      c = (*p); 
    if(m_assoTool->isUsed(*c)) continue;
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

    if(x < Xm) {
      InDet::SiClusterLink_xk l(c,x);
      for(int i=0; i!=nl; ++i) L[i].Comparison(l);
      if(nl<10) L[nl++]=l; else Xm=L[9].xi2();
      Xc = Xm+6.;
    }
    else if(!nl && x < Xl) {Xl = x; Xc = x+6.; cl = c;}
  }
  if(cl && !nl) {L[nl++].Set(cl,Xl);}
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
  double Xc  = m_xi2maxlink;
  double Xm  = m_xi2max    ;

  const InDet::SiCluster* cl = 0;

  InDet::SiClusterCollection::const_iterator p =  m_sibegin;

  for(; p!=m_siend; ++p) {
    
    const InDet::SiCluster*  c  = (*p);  
    if(m_assoTool->isUsed(*c)) continue;
    const Amg::Vector2D&     M = c->localPosition();

    double MV0 = c->width().phiR();
    double MV2 = c->width().z   ();

    double r0  = M[0]-P0, r02 = r0*r0; 
    double r1  = M[1]-P1, r12 = r1*r1;

    double v0  = .08333*(MV0*MV0)+PV0; if(r02 >(Xc*v0)) continue;
    double v2  = .08333*(MV2*MV2)+PV2; if(r12 >(Xc*v2)) continue;
    double v1  =                  PV1;
    double d   = v0*v2-v1*v1;          if(   d<=0.    ) continue;
    double x   = (r02*v2+r12*v0-(r0*r1)*(2.*v1))/d;

    if(x>Xc) continue;

    if(x < Xm) {
      InDet::SiClusterLink_xk l(c,x);
      for(int i=0; i!=nl; ++i) L[i].Comparison(l);
      if(nl<10) L[nl++]=l; else Xm=L[9].xi2();
      Xc = Xm;
    }
    else if(!nl) {Xc = x; cl = c;}
  }
  if(cl && !nl) {L[nl++].Set(cl,Xc);}
  return nl;
} 

///////////////////////////////////////////////////////////////////
// Search closest cluster without stereo angle for SCT 
///////////////////////////////////////////////////////////////////

int  InDet::SiTrajectoryElement_xk::searchClustersWithoutStereoAssSCT
(Trk::PatternTrackParameters& Tp,InDet::SiClusterLink_xk* L) 
{
  int nl     = 0; if(m_detstatus<=0) return 0;

  double P0  = Tp.par()[0];
  double P1  = Tp.par()[1];
  double PV0 = Tp.cov()[0];
  double PV1 = Tp.cov()[1];
  double PV2 = Tp.cov()[2];
  double Xc  = m_xi2maxlink;
  double Xm  = m_xi2max    ;

  const InDet::SiCluster* cl = 0;

  InDet::SiClusterCollection::const_iterator p =  m_sibegin;

  for(; p!=m_siend; ++p) {
    
    const InDet::SiCluster*      c = (*p); 
    if(m_assoTool->isUsed(*c)) continue;
    const Amg::Vector2D&         M = c->localPosition();

    double MV0 = c->width().phiR()   ;
    double v0  = .08333*(MV0*MV0)+PV0;   
    double r0  = M[0]-P0;
    double d   = 1./v0;
    double x   = (r0*r0)*d;
    
    if(x>Xc) continue;
    
    double dP1 = (P1-M[1])+PV1*d*r0;

    if(fabs(dP1) > m_halflenght) {

      double r1; 
      dP1 > m_halflenght ? r1 = m_halflenght-P1 : r1 = -(m_halflenght+P1);
      
      double v1 = PV1;
      double v2 = PV2;  
      d = v0*v2-v1*v1  ; if(d<=0.) continue; 
      x = (r0*(r0*v2-r1*v1)+r1*(r1*v0-r0*v1))/d;
      if(x>Xc) continue;
    }

    if(x < Xm) {
      InDet::SiClusterLink_xk l(c,x);
      for(int i=0; i!=nl; ++i) L[i].Comparison(l);
      if(nl<10) L[nl++]=l; else Xm=L[9].xi2();
      Xc = Xm;
    }
    else if(!nl) {Xc = x; cl = c;}
  }
  if(cl && !nl) {L[nl++].Set(cl,Xc);}
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
    ro = m_riotool->correct(*m_cluster     ,*tp);
    pat.set(Trk::TrackStateOnSurface::Measurement);
  }
  else          {
    ro = m_riotool->correct(*m_clusterNoAdd,*tp);
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

      if(m_linkB[i].xi2() > m_xi2multi) break; 

      const Trk::TrackParameters    * tpn = 0;
      if(!change)                     tpn = trackParameters                (cov,Q);
      else                            tpn = trackParametersWithNewDirection(cov,Q); 
      if(!tpn) break;;

      const Trk::FitQualityOnSurface   * fqn = new Trk::FitQualityOnSurface(m_linkB[i].xi2(),m_ndf);
      const Trk::MeasurementBase       * ron = m_riotool->correct(*m_linkB[i].cluster(),*tp);
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

    const InDet::SCT_Cluster*  sc  = static_cast<const InDet::SCT_Cluster*> (cl); 
    if(sc)  ro =  new InDet::SCT_ClusterOnTrack (sc,locp,cv,iH,sc->globalPosition());
  }
  else           {

    const InDet::PixelCluster*  pc  = static_cast<const InDet::PixelCluster*> (cl);
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

  bool Q = m_proptool->propagate
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

  double q = fabs(Tp.par()[4]);
  double s = fabs(m_A[0]*m_Tr[6]+m_A[1]*m_Tr[7]+m_A[2]*m_Tr[8]); s  < .05 ? s = 20. : s = 1./s; 
  double d = (1.-m_A[2])*(1.+m_A[2]);   if(d < 1.e-5) d = 1.e-5;

  m_radlengthN = s*m_radlength; 
  double covariancePola = (134.*m_radlengthN)*(q*q);
  double covarianceAzim = covariancePola/d;
  double covarianceIMom,correctionIMom;

  if(Model==1) {
    double        dp = m_energylose*q*s;
    covarianceIMom = (.2*dp*dp)*(q*q);
    correctionIMom = 1.-dp;
  }
  else {
    correctionIMom = .70;
    covarianceIMom = (correctionIMom-1.)*(correctionIMom-1.)*(q*q);
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
  bool   Q    = propagateParameters(Ta,Tb,step);
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
    
    QA = m_updatorTool->combineStates(S1,S2,SM);
    
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
      double w = 2.-m_xi2max; if(++holes > 1) w*=2.; return w;
    }
    else if(m_inside == 0) return -1.;
    else                   return  0.;
  }

  double w,X,Xc = m_xi2max+2.;
  m_status == 1 ? X = m_xi2F        : X = m_xi2B;
  m_ndf    == 2 ? w = 1.2*(Xc-X*.5) : w = Xc-X  ; if(w < -1.) w = -1.;
  holes    = 0;

  return w;
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for pattern track parameters and covariance matrix propagation
// to PlaneSurface Ta->pSu = Tb with step to surface calculation
/////////////////////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::propagate
(Trk::PatternTrackParameters  & Ta,
 Trk::PatternTrackParameters  & Tb,
 double                       & S ) 
{
  bool useJac = true; if(!Ta.iscovariance()) useJac = false;

  double P[64]; transformPlaneToGlobal(useJac,Ta,P);

  if( m_fieldMode) {if(!rungeKuttaToPlane      (useJac,P)) return false;}
  else             {if(!straightLineStepToPlane(useJac,P)) return false;} 

  S = P[45]; return transformGlobalToPlane(useJac,P,Ta,Tb);
}

/////////////////////////////////////////////////////////////////////////////////
// Main function for pattern track parameters and covariance matrix propagation
// to PlaneSurface Ta->pSu = Tb with step to surface calculation
/////////////////////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::propagateParameters
(Trk::PatternTrackParameters  & Ta,
 Trk::PatternTrackParameters  & Tb,
 double                       & S ) 
{
  bool useJac = false;

  double P[64]; transformPlaneToGlobal(useJac,Ta,P);

  if( m_fieldMode) {if(!rungeKuttaToPlane      (useJac,P)) return false;}
  else             {if(!straightLineStepToPlane(useJac,P)) return false;} 

  S = P[45]; return transformGlobalToPlane(useJac,P,Ta,Tb);
}

/////////////////////////////////////////////////////////////////////////////////
// Tramsform from plane to global
/////////////////////////////////////////////////////////////////////////////////

void InDet::SiTrajectoryElement_xk::transformPlaneToGlobal
(bool useJac,Trk::PatternTrackParameters& Tp,double* P) 
{
  double Sf,Cf,Ce,Se;   sincos(Tp.par()[2],&Sf,&Cf);  sincos(Tp.par()[3],&Se,&Ce);

  const Amg::Transform3D& T  = Tp.associatedSurface()->transform();

  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};

  P[ 0] = Tp.par()[0]*Ax[0]+Tp.par()[1]*Ay[0]+T(0,3);                    // X
  P[ 1] = Tp.par()[0]*Ax[1]+Tp.par()[1]*Ay[1]+T(1,3);                    // Y
  P[ 2] = Tp.par()[0]*Ax[2]+Tp.par()[1]*Ay[2]+T(2,3);                    // Z
  P[ 3] = Cf*Se;                                                         // Ax
  P[ 4] = Sf*Se;                                                         // Ay
  P[ 5] = Ce;                                                            // Az
  P[ 6] = Tp.par()[4];                                                   // CM
  if(fabs(P[6])<1.e-20) {P[6] < 0. ? P[6]=-1.e-20 : P[6]= 1.e-20;}    

  if(useJac) {

    //     /dL1   |      /dL2    |    /dPhi    |    /dThe     |    /dCM     |
    P[ 7]  = Ax[0]; P[14] = Ay[0]; P[21] =   0.; P[28] =    0.; P[35] =   0.; // dX /
    P[ 8]  = Ax[1]; P[15] = Ay[1]; P[22] =   0.; P[29] =    0.; P[36] =   0.; // dY /
    P[ 9]  = Ax[2]; P[16] = Ay[2]; P[23] =   0.; P[30] =    0.; P[37] =   0.; // dZ /
    P[10]  =    0.; P[17] =    0.; P[24] =-P[4]; P[31] = Cf*Ce; P[38] =   0.; // dAx/
    P[11]  =    0.; P[18] =    0.; P[25] = P[3]; P[32] = Sf*Ce; P[39] =   0.; // dAy/
    P[12]  =    0.; P[19] =    0.; P[26] =   0.; P[33] =   -Se; P[40] =   0.; // dAz/
    P[42]  =    0.; P[43] =    0.; P[44] =   0.;                              // d(Ax,Ay,Az)/ds
  }
  P[45] =   0.;
}

/////////////////////////////////////////////////////////////////////////////////
// Tramsform from global to plane
/////////////////////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::transformGlobalToPlane
(bool useJac,double* P,Trk::PatternTrackParameters& Ta,Trk::PatternTrackParameters& Tb) 
{

  double Ax[3] = {m_Tr[0],m_Tr[1],m_Tr[2]};
  double Ay[3] = {m_Tr[3],m_Tr[4],m_Tr[5]};
  double Az[3] = {m_Tr[6],m_Tr[7],m_Tr[8]};
  double d [3] = {P[0]-m_Tr[ 9],P[1]-m_Tr[10],P[2]-m_Tr[11]};

  double p[5] = {d[0]*Ax[0]+d[1]*Ax[1]+d[2]*Ax[2],
		 d[0]*Ay[0]+d[1]*Ay[1]+d[2]*Ay[2],
		 atan2(P[4],P[3])                ,
		 acos(P[5])                      ,
		 P[6]                            };

  Tb.setParameters(m_surface,p); m_A[0] = P[3]; m_A[1] = P[4]; m_A[2] = P[5];
  if(!useJac) return true;

  // Condition trajectory on surface
  //
  double A  = Az[0]*P[3]+Az[1]*P[4]+Az[2]*P[5]; if(A!=0.) A=1./A;
  double s0 = Az[0]*P[ 7]+Az[1]*P[ 8]+Az[2]*P[ 9];
  double s1 = Az[0]*P[14]+Az[1]*P[15]+Az[2]*P[16]; 
  double s2 = Az[0]*P[21]+Az[1]*P[22]+Az[2]*P[23];
  double s3 = Az[0]*P[28]+Az[1]*P[29]+Az[2]*P[30];
  double s4 = Az[0]*P[35]+Az[1]*P[36]+Az[2]*P[37]; 
  double T0 =(Ax[0]*P[ 3]+Ax[1]*P[ 4]+Ax[2]*P[ 5])*A; 
  double T1 =(Ay[0]*P[ 3]+Ay[1]*P[ 4]+Ay[2]*P[ 5])*A;
  double n  = 1./P[6]; 

  double Jac[21];

  // Jacobian production
  //
  Jac[ 0] = (Ax[0]*P[ 7]+Ax[1]*P[ 8])+(Ax[2]*P[ 9]-s0*T0);    // dL0/dL0
  Jac[ 1] = (Ax[0]*P[14]+Ax[1]*P[15])+(Ax[2]*P[16]-s1*T0);    // dL0/dL1
  Jac[ 2] = (Ax[0]*P[21]+Ax[1]*P[22])+(Ax[2]*P[23]-s2*T0);    // dL0/dPhi
  Jac[ 3] = (Ax[0]*P[28]+Ax[1]*P[29])+(Ax[2]*P[30]-s3*T0);    // dL0/dThe
  Jac[ 4] =((Ax[0]*P[35]+Ax[1]*P[36])+(Ax[2]*P[37]-s4*T0))*n; // dL0/dCM

  Jac[ 5] = (Ay[0]*P[ 7]+Ay[1]*P[ 8])+(Ay[2]*P[ 9]-s0*T1);    // dL1/dL0
  Jac[ 6] = (Ay[0]*P[14]+Ay[1]*P[15])+(Ay[2]*P[16]-s1*T1);    // dL1/dL1
  Jac[ 7] = (Ay[0]*P[21]+Ay[1]*P[22])+(Ay[2]*P[23]-s2*T1);    // dL1/dPhi
  Jac[ 8] = (Ay[0]*P[28]+Ay[1]*P[29])+(Ay[2]*P[30]-s3*T1);    // dL1/dThe
  Jac[ 9] =((Ay[0]*P[35]+Ay[1]*P[36])+(Ay[2]*P[37]-s4*T1))*n; // dL1/dCM

  double P3,P4, C = P[3]*P[3]+P[4]*P[4]; 
  if(C > 1.e-20) {C= 1./C ; P3 = P[3]*C; P4 =P[4]*C; C =-sqrt(C);}
  else           {C=-1.e10; P3 = 1.    ; P4 =0.    ;             }

  double T2  =(P3*P[43]-P4*P[42])*A;
  double C44 = C*P[44]           *A;

  Jac[10] = P3*P[11]-P4*P[10]-s0*T2;    // dPhi/dL0
  Jac[11] = P3*P[18]-P4*P[17]-s1*T2;    // dPhi/dL1
  Jac[12] = P3*P[25]-P4*P[24]-s2*T2;    // dPhi/dPhi
  Jac[13] = P3*P[32]-P4*P[31]-s3*T2;    // dPhi/dThe
  Jac[14] =(P3*P[39]-P4*P[38]-s4*T2)*n; // dPhi/dCM

  Jac[15] = C*P[12]-s0*C44;             // dThe/dL0
  Jac[16] = C*P[19]-s1*C44;             // dThe/dL1
  Jac[17] = C*P[26]-s2*C44;             // dThe/dPhi
  Jac[18] = C*P[33]-s3*C44;             // dThe/dThe
  Jac[19] =(C*P[40]-s4*C44)*n;          // dThe/dCM
  Jac[20] = 1.;                         // dCM /dCM

  Tb.newCovarianceMatrix(Ta,Jac); 
  const double* t = &Tb.cov()[0];
  if(t[0]<=0. || t[2]<=0. || t[5]<=0. || t[9]<=0. || t[14]<=0.) return false;
  return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Runge Kutta step to plane
/////////////////////////////////////////////////////////////////////////////////

bool  InDet::SiTrajectoryElement_xk::rungeKuttaToPlane
(bool Jac,double* P)
{
  const double Smin = .1        ;
  const double Shel = 5.        ;
  const double dlt  = .001      ;

  if(fabs(P[6]) > .05) return false;

  int    it    =               0;
  double* R    =          &P[ 0];            // Coordinates 
  double* A    =          &P[ 3];            // Directions
  double* sA   =          &P[42];
  double  Pi   =  149.89626*P[6];            // Invert mometum/2. 
  double  Pa   = fabs      (P[6]);

  double  a    = A[0]*m_Tr[6]+A[1]*m_Tr[7]+A[2]*m_Tr[8]                 ; if(a==0.) return false; 
  double  S    = ((m_Tr[12]-R[0]*m_Tr[6])-(R[1]*m_Tr[7]+R[2]*m_Tr[8]))/a; 
  double  S0   = fabs(S)                                                ;

  if(S0 <= Smin) {
    R[0]+=(A[0]*S); R[1]+=(A[1]*S); R[2]+=(A[2]*S); P[45]+=S; return true;
  }
  else  if( (Pa*S0) > .3) {
    S > 0. ? S = .3/Pa : S=-.3/Pa;
  }
  
  bool   ste   = false; 

  double f0[3],f[3]; m_fieldService->getFieldZR(R,f0); 

  while(true) {

    bool Helix = false; if(fabs(S) < Shel) Helix = true;
    double S3=(1./3.)*S, S4=.25*S, PS2=Pi*S;
 
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
      m_fieldService->getFieldZR(gP,f);
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
      m_fieldService->getFieldZR(gP,f);
    }
    else       {f[0]=f0[0]; f[1]=f0[1]; f[2]=f0[2];} 
    
    double H2[3] = {f[0]*PS2,f[1]*PS2,f[2]*PS2}; 
    double A6    = B5*H2[2]-C5*H2[1]           ;
    double B6    = C5*H2[0]-A5*H2[2]           ;
    double C6    = A5*H2[1]-B5*H2[0]           ;

    // Test approximation quality on give step and possible step reduction
    //
    if(!ste) {
      double EST = fabs((A1+A6)-(A3+A4))+fabs((B1+B6)-(B3+B4))+fabs((C1+C6)-(C3+C4)); 
      if(EST>dlt) {S*=.6; continue;} 
    }

    // Parameters calculation
    //   
    if((!ste && S0 > fabs(S)*100.) || fabs(P[45]+=S) > 2000.) return false;
    ste = true;

    double A00 = A[0], A11=A[1], A22=A[2];

    R[0]+=(A2+A3+A4)*S3; A[0] = ((A0+2.*A3)+(A5+A6))*(1./3.);
    R[1]+=(B2+B3+B4)*S3; A[1] = ((B0+2.*B3)+(B5+B6))*(1./3.);
    R[2]+=(C2+C3+C4)*S3; A[2] = ((C0+2.*C3)+(C5+C6))*(1./3.);
	
    double D   = 1./sqrt(A[0]*A[0]+A[1]*A[1]+A[2]*A[2]);
    A[0]*=D; A[1]*=D; A[2]*=D;

    if(Jac) {

      double* d2A = &P[24];
      double* d3A = &P[31]; 
      double* d4A = &P[38]; 
      double d2A0 = H0[2]*d2A[1]-H0[1]*d2A[2];
      double d2B0 = H0[0]*d2A[2]-H0[2]*d2A[0];
      double d2C0 = H0[1]*d2A[0]-H0[0]*d2A[1];
      double d3A0 = H0[2]*d3A[1]-H0[1]*d3A[2];
      double d3B0 = H0[0]*d3A[2]-H0[2]*d3A[0];
      double d3C0 = H0[1]*d3A[0]-H0[0]*d3A[1];
      double d4A0 =(A0+H0[2]*d4A[1])-H0[1]*d4A[2];
      double d4B0 =(B0+H0[0]*d4A[2])-H0[2]*d4A[0];
      double d4C0 =(C0+H0[1]*d4A[0])-H0[0]*d4A[1];
      double d2A2 = d2A0+d2A[0];                
      double d2B2 = d2B0+d2A[1];                
      double d2C2 = d2C0+d2A[2];
      double d3A2 = d3A0+d3A[0];                
      double d3B2 = d3B0+d3A[1];                
      double d3C2 = d3C0+d3A[2];
      double d4A2 = d4A0+d4A[0];                
      double d4B2 = d4B0+d4A[1];                
      double d4C2 = d4C0+d4A[2];
      double d0   = d4A[0]-A00;
      double d1   = d4A[1]-A11;
      double d2   = d4A[2]-A22;
      double d2A3 = ( d2A[0]+d2B2*H1[2])-d2C2*H1[1];
      double d2B3 = ( d2A[1]+d2C2*H1[0])-d2A2*H1[2];
      double d2C3 = ( d2A[2]+d2A2*H1[1])-d2B2*H1[0];
      double d3A3 = ( d3A[0]+d3B2*H1[2])-d3C2*H1[1];
      double d3B3 = ( d3A[1]+d3C2*H1[0])-d3A2*H1[2];
      double d3C3 = ( d3A[2]+d3A2*H1[1])-d3B2*H1[0];
      double d4A3 = ((A3+d0)+d4B2*H1[2])-d4C2*H1[1];
      double d4B3 = ((B3+d1)+d4C2*H1[0])-d4A2*H1[2];
      double d4C3 = ((C3+d2)+d4A2*H1[1])-d4B2*H1[0];
      double d2A4 = ( d2A[0]+d2B3*H1[2])-d2C3*H1[1];
      double d2B4 = ( d2A[1]+d2C3*H1[0])-d2A3*H1[2];
      double d2C4 = ( d2A[2]+d2A3*H1[1])-d2B3*H1[0];
      double d3A4 = ( d3A[0]+d3B3*H1[2])-d3C3*H1[1];
      double d3B4 = ( d3A[1]+d3C3*H1[0])-d3A3*H1[2];
      double d3C4 = ( d3A[2]+d3A3*H1[1])-d3B3*H1[0];
      double d4A4 = ((A4+d0)+d4B3*H1[2])-d4C3*H1[1];
      double d4B4 = ((B4+d1)+d4C3*H1[0])-d4A3*H1[2];
      double d4C4 = ((C4+d2)+d4A3*H1[1])-d4B3*H1[0];
      double d2A5 = 2.*d2A4-d2A[0];            
      double d2B5 = 2.*d2B4-d2A[1];            
      double d2C5 = 2.*d2C4-d2A[2];
      double d3A5 = 2.*d3A4-d3A[0];            
      double d3B5 = 2.*d3B4-d3A[1];            
      double d3C5 = 2.*d3C4-d3A[2];            
      double d4A5 = 2.*d4A4-d4A[0];            
      double d4B5 = 2.*d4B4-d4A[1];            
      double d4C5 = 2.*d4C4-d4A[2];            
      double d2A6 = d2B5*H2[2]-d2C5*H2[1];      
      double d2B6 = d2C5*H2[0]-d2A5*H2[2];      
      double d2C6 = d2A5*H2[1]-d2B5*H2[0];      
      double d3A6 = d3B5*H2[2]-d3C5*H2[1];      
      double d3B6 = d3C5*H2[0]-d3A5*H2[2];      
      double d3C6 = d3A5*H2[1]-d3B5*H2[0];
      double d4A6 = d4B5*H2[2]-d4C5*H2[1];      
      double d4B6 = d4C5*H2[0]-d4A5*H2[2];      
      double d4C6 = d4A5*H2[1]-d4B5*H2[0];      
      
      double* dR  = &P[21];
      dR [0]+=(d2A2+d2A3+d2A4)*S3;
      dR [1]+=(d2B2+d2B3+d2B4)*S3;
      dR [2]+=(d2C2+d2C3+d2C4)*S3;
      d2A[0] =((d2A0+2.*d2A3)+(d2A5+d2A6))*(1./3.);      
      d2A[1] =((d2B0+2.*d2B3)+(d2B5+d2B6))*(1./3.); 
      d2A[2] =((d2C0+2.*d2C3)+(d2C5+d2C6))*(1./3.);

      dR          = &P[28];
      dR [0]+=(d3A2+d3A3+d3A4)*S3;
      dR [1]+=(d3B2+d3B3+d3B4)*S3;
      dR [2]+=(d3C2+d3C3+d3C4)*S3;
      d3A[0] =((d3A0+2.*d3A3)+(d3A5+d3A6))*(1./3.);      
      d3A[1] =((d3B0+2.*d3B3)+(d3B5+d3B6))*(1./3.); 
      d3A[2] =((d3C0+2.*d3C3)+(d3C5+d3C6))*(1./3.);

      dR          = &P[35];
      dR [0]+=(d4A2+d4A3+d4A4)*S3;
      dR [1]+=(d4B2+d4B3+d4B4)*S3;
      dR [2]+=(d4C2+d4C3+d4C4)*S3;
      d4A[0] =((d4A0+2.*d4A3)+(d4A5+d4A6+A6))*(1./3.);      
      d4A[1] =((d4B0+2.*d4B3)+(d4B5+d4B6+B6))*(1./3.); 
      d4A[2] =((d4C0+2.*d4C3)+(d4C5+d4C6+C6))*(1./3.);
    }

    // New step estimation
    //
    double  a    = A[0]*m_Tr[6]+A[1]*m_Tr[7]+A[2]*m_Tr[8]; if(a==0.) return false;
    double  Sn   = ((m_Tr[12]-R[0]*m_Tr[6])-(R[1]*m_Tr[7]+R[2]*m_Tr[8]))/a;
    double aSn = fabs(Sn);

    if(aSn <= Smin) {
      double Sl = 2./S; sA[0] = A6*Sl; sA[1] = B6*Sl; sA[2] = C6*Sl;
      R[0]+=(A[0]*Sn); R[1]+=(A[1]*Sn); R[2]+=(A[2]*Sn); P[45]+=Sn; return true;  
    }

    double aS = fabs(S);

    if     (  S*Sn < 0. ) {
      if(++it > 2) return false;
      if (aSn < aS) S = Sn; else S =-S;
    }
    else if( aSn  < aS  ) S = Sn;
   
    f0[0]=f[0]; f0[1]=f[1]; f0[2]=f[2];
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////
// Straight line step to plane
/////////////////////////////////////////////////////////////////////////////////

bool InDet::SiTrajectoryElement_xk::straightLineStepToPlane
(bool Jac,double* P) 
{
  double*  R   = &P[ 0];             // Start coordinates
  double*  A   = &P[ 3];             // Start directions

  double  a    = A[0]*m_Tr[6]+A[1]*m_Tr[7]+A[2]*m_Tr[8]; if(a==0.) return false; 
  double  S    = ((m_Tr[12]-R[0]*m_Tr[6])-(R[1]*m_Tr[7]+R[2]*m_Tr[8]))/a;
  P[45]        = S;

  // Track parameters in last point
  //
  R[0]+=(A[0]*S); R[1]+=(A[1]*S); R[2]+=(A[2]*S); if(!Jac) return true;
  
  // Derivatives of track parameters in last point
  //
  for(int i=7; i<42; i+=7) {

    double* dR = &P[i  ]; 
    double* dA = &P[i+3];
    dR[0]+=(dA[0]*S); dR[1]+=(dA[1]*S); dR[2]+=(dA[2]*S);
  }
  return true;
}

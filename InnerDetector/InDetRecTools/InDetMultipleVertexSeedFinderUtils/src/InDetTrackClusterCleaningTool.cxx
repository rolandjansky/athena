/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetMultipleVertexSeedFinderUtils/InDetTrackClusterCleaningTool.h"
#include "TrkTrack/Track.h"
#include "VxVertex/Vertex.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBase.h" 
#include "TrkExInterfaces/IExtrapolator.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace InDet
{

 StatusCode InDetTrackClusterCleaningTool::initialize()
 {
  if ( m_extrapolator.retrieve().isFailure() ) 
  {                              
   msg(MSG::ERROR) << "Failed to retrieve tool " << m_extrapolator << endmsg;
   return StatusCode::FAILURE;                                                  
  } else {                                                                       
   msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endmsg;
  }    
 
  return StatusCode::SUCCESS;
 }//end of initialize method
    

 InDetTrackClusterCleaningTool::InDetTrackClusterCleaningTool(const std::string& t, const std::string& n, const IInterface*  p):
 AthAlgTool(t,n,p), m_extrapolator("Trk::Extrapolator") ,m_zOffset(3.)
 {
  declareInterface<InDetTrackClusterCleaningTool>(this);  
  declareProperty("nStandDeviations",        m_zOffset = 3.0);

//extrapolator
  declareProperty("Extrapolator",m_extrapolator);  
 }//end of constructor 

 InDetTrackClusterCleaningTool::~InDetTrackClusterCleaningTool(){}

 std::pair<std::vector<const Trk::Track*>,std::vector<const Trk::Track*> > InDetTrackClusterCleaningTool::
                                                                           clusterAndOutliers(const std::vector<const Trk::Track*>& cluster,
                                                                                                          const Trk::Vertex * reference )const
 {
  std::vector<const Trk::Track*> clusterSeed(0);
  std::vector<const Trk::Track*> outliers(0);
  double z_center = 0;
  std::vector<const Trk::Track*>::const_iterator inb = cluster.begin();
  std::vector<const Trk::Track*>::const_iterator ine = cluster.end();
  
  unsigned int cluster_size = 0;
  Trk::PerigeeSurface perigeeSurface(reference->position());
  
//first getting the cluster center  
  for(std::vector<const Trk::Track*>::const_iterator i = inb; i != ine; ++i)
  {
   const Trk::TrackParameters * perigee(0);
   if(!reference) perigee = (*i)->perigeeParameters();
   else perigee = m_extrapolator->extrapolate(**i,perigeeSurface,
					      Trk::anyDirection,true, 
					      Trk::pion); 
   
   if(perigee)
   { 
    z_center += perigee->parameters()[Trk::z0];
    msg(MSG::DEBUG)<<"Adding parameters: "<<perigee->parameters()[Trk::z0] <<endmsg;
    ++cluster_size;
   }else{
    msg(MSG::WARNING)<<" The TrackParticleBase provided does not contain perigee parameters"<<endmsg;
   }//end of perigee security check
  }//end of loop definig the center of a cluster
  msg(MSG::DEBUG)<<"Z center is: "<<z_center<<" for  tracks: "<<cluster_size<<endmsg;
  
  if(cluster_size != 0) {
    z_center = z_center/cluster_size;
  }
   
//discarding outlying tracks
  for(std::vector<const Trk::Track*>::const_iterator i = inb; i != ine; ++i)
  {
    const Trk::TrackParameters * measPerigee(0);
   if(!reference) measPerigee=(*i)->perigeeParameters();
   else{
     
     //here we want to make an extrapolation    
     measPerigee = m_extrapolator->extrapolate(**i,
					       perigeeSurface,Trk::anyDirection,
					       true, Trk::pion); 
   }
   
   if(measPerigee)
     {
       double z0 = measPerigee->parameters()[Trk::z0];
       const AmgSymMatrix(5) * cov = measPerigee->covariance();
       
       double sigma_z0 = Amg::error(*cov,Trk::z0);
    
//if the track is closer than several standard deviations, keep it    
    if(fabs(z_center-z0)< sigma_z0*m_zOffset) clusterSeed.push_back(*i); 

//declare it an outlier otherwise
    else outliers.push_back(*i);
   }else{
    outliers.push_back(*i);
    msg(MSG::WARNING)  << "This track has no meas perigee. Regarded as outlyer" << endmsg;
   }//end of meas perigee protection check
  }//end of selection loop over all the tracks  
  
  std::pair<std::vector<const Trk::Track*>,std::vector<const Trk::Track*> > result(clusterSeed, outliers);
  return result;
 }//end of cleaning method (Track)  
 
//method working with TrackParticleBases 
  std::pair<std::vector<const Trk::TrackParticleBase*>,std::vector<const Trk::TrackParticleBase*> > 
  InDetTrackClusterCleaningTool::clusterAndOutliers(const std::vector<const Trk::TrackParticleBase*>& cluster,
						    const Trk::Vertex * reference)const
 {
  std::vector<const Trk::TrackParticleBase*> clusterSeed(0);
  std::vector<const Trk::TrackParticleBase*> outliers(0);
 
  double z_center = 0;
  
  std::vector<const Trk::TrackParticleBase*>::const_iterator inb = cluster.begin();
  std::vector<const Trk::TrackParticleBase*>::const_iterator ine = cluster.end();
  
  unsigned int cluster_size = 0;
  
  msg(MSG::DEBUG)<<"Receiving a cluster of size: "<< cluster.size()<<endmsg;
  
  Trk::PerigeeSurface perigeeSurface(reference->position());
   
//first getting the cluster center  
  for(std::vector<const Trk::TrackParticleBase*>::const_iterator i = inb; i != ine; ++i)
  {
   const Trk::TrackParameters * perigee(0);
   if(!reference) perigee = &((*i)->definingParameters());
   else perigee = m_extrapolator->extrapolate((*i)->definingParameters(),perigeeSurface,Trk::anyDirection,true, Trk::pion);
   
   if(perigee)
   { 
    z_center += perigee->parameters()[Trk::z0];
    msg(MSG::DEBUG)<<"Adding parameters: "<<perigee->parameters()[Trk::z0] <<endmsg;
    ++cluster_size;
   }else{
    msg(MSG::WARNING)<<" The TrackParticleBase provided does not contain perigee parameters"<<endmsg;
   }//end of perigee security check
  }//end of loop definig the center of a cluster
  msg(MSG::DEBUG)<<"Z center is: "<<z_center<<" for  tracks: "<<cluster_size<<endmsg;
  
  if(cluster_size != 0) {
    z_center = z_center/cluster_size;
  }

  msg(MSG::DEBUG)<<"Looping over the cluster" <<endmsg;

  for(std::vector<const Trk::TrackParticleBase*>::const_iterator i = inb; i != ine; ++i)
  {
   const Trk::TrackParameters * measPerigee(0);
   if(!reference) measPerigee = &((*i)->definingParameters());
   else  measPerigee = m_extrapolator->extrapolate((*i)->definingParameters(),perigeeSurface,Trk::anyDirection,true, Trk::pion);
  
   if(0!=measPerigee)
   {
    double z0 = measPerigee->parameters()[Trk::z0];
    const AmgSymMatrix(5) * cov = measPerigee->covariance();    
    double sigma_z0 = Amg::error(*cov,Trk::z0);

    msg(MSG::DEBUG)<<"Perigee Z0 and corresponding sigma "<<z0<<" "<<sigma_z0<<endmsg;
    msg(MSG::DEBUG)<<"Center of the cluster "<<z_center<<endmsg;
    msg(MSG::DEBUG)<<"Offset "<<m_zOffset<<endmsg;
    msg(MSG::DEBUG)<<"discriminant "<<fabs(z_center-z0)<<" "<< sigma_z0*m_zOffset <<endmsg;

//if the track is closer than several standard deviations, keep it    
    if(fabs(z_center-z0)< sigma_z0*m_zOffset) clusterSeed.push_back(*i); 

//declare it an outlier otherwise
    else outliers.push_back(*i);
   }else{
    outliers.push_back(*i);
    msg(MSG::WARNING)  << "This track has no meas perigee. Regarded as outlyer" << endmsg;
   }//end of measured perigee check
  }//end of separation loop
  
  std::pair<std::vector<const Trk::TrackParticleBase*>,std::vector<const Trk::TrackParticleBase*> > result(clusterSeed, outliers);
  return result;
 }

 std::pair<std::vector<const Trk::TrackParameters *>, 
	   std::vector<const xAOD::TrackParticle *> >  InDetTrackClusterCleaningTool::clusterAndOutliers(std::vector<const xAOD::TrackParticle *> cluster, const xAOD::Vertex * reference) const
	   {
	     
	     std::vector<const Trk::TrackParameters*> clusterSeed(0);
	     std::vector<const xAOD::TrackParticle*> outliers(0);
	     
	     double z_center = 0;
	     
	     std::vector<const xAOD::TrackParticle*>::const_iterator inb = cluster.begin();
	     std::vector<const xAOD::TrackParticle*>::const_iterator ine = cluster.end();
	     
	     unsigned int cluster_size = 0;
	     
	     msg(MSG::DEBUG)<<"Receiving a cluster of size: "<< cluster.size()<<endmsg;
	     
	     Trk::PerigeeSurface perigeeSurface(reference->position());
	     
	     //first getting the cluster center  
	     for(std::vector<const xAOD::TrackParticle*>::const_iterator i = inb; i != ine; ++i)
	       {
		 const Trk::TrackParameters * perigee(0);
		 
		 perigee = m_extrapolator->extrapolate(**i,perigeeSurface,Trk::anyDirection,true, Trk::pion);
		 
		 if(perigee)
		   { 
		     z_center += perigee->parameters()[Trk::z0];
		     msg(MSG::DEBUG)<<"Adding parameters: "<<perigee->parameters()[Trk::z0] <<endmsg;
		     ++cluster_size;
		   }else{
		   msg(MSG::WARNING)<<" The TrackParticleBase provided does not contain perigee parameters"<<endmsg;
		 }//end of perigee security check
	       }//end of loop definig the center of a cluster
	     msg(MSG::DEBUG)<<"Z center is: "<<z_center<<" for  tracks: "<<cluster_size<<endmsg;
	     
	     if(cluster_size != 0) {
	       z_center = z_center/cluster_size;
	     }
	     
	     msg(MSG::DEBUG)<<"Looping over the cluster" <<endmsg;
	     
	     for(std::vector<const xAOD::TrackParticle*>::const_iterator i = inb; i != ine; ++i)
	       {
		 const Trk::TrackParameters * measPerigee(0);
		 measPerigee = m_extrapolator->extrapolate(**i,perigeeSurface,Trk::anyDirection,true, Trk::pion);
		 
		 if(0!=measPerigee)
		   {
		     double z0 = measPerigee->parameters()[Trk::z0];
		     const AmgSymMatrix(5) * cov = measPerigee->covariance();    
		     double sigma_z0 = Amg::error(*cov,Trk::z0);
		     
		     msg(MSG::DEBUG)<<"Perigee Z0 and corresponding sigma "<<z0<<" "<<sigma_z0<<endmsg;
		     msg(MSG::DEBUG)<<"Center of the cluster "<<z_center<<endmsg;
		     msg(MSG::DEBUG)<<"Offset "<<3.0<<endmsg;
		     msg(MSG::DEBUG)<<"discriminant "<<fabs(z_center-z0)<<" "<< sigma_z0*3.0 <<endmsg;
		     
		     //if the track is closer than several standard deviations, keep it    
		     if(fabs(z_center-z0)< sigma_z0*3.0) clusterSeed.push_back(&((*i)->perigeeParameters())); 
		     
		     //declare it an outlier otherwise
		     else outliers.push_back(*i);
		   }else{
		   outliers.push_back(*i);
		   msg(MSG::WARNING)  << "This track has no meas perigee. Regarded as outlyer" << endmsg;
		 }//end of measured perigee check
	       }//end of separation loop
	     
	     std::pair<std::vector<const Trk::TrackParameters *>, 
	       std::vector<const xAOD::TrackParticle *> > result(clusterSeed, outliers);
	     return result;
  
}
  
  
}//end of namespace definitions


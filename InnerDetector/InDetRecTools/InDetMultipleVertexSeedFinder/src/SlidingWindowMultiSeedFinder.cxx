/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetMultipleVertexSeedFinder/SlidingWindowMultiSeedFinder.h"
//#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "VxVertex/Vertex.h"

#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "InDetMultipleVertexSeedFinderUtils/InDetTrackZ0SortingTool.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "xAODTracking/Vertex.h"

namespace InDet
{

 StatusCode SlidingWindowMultiSeedFinder::initialize()
 {
  if(m_trkFilter.retrieve().isFailure())
  {
   msg(MSG::ERROR)<<" Unable to retrieve "<<m_trkFilter<<endmsg;
   return StatusCode::FAILURE;
  }else msg(MSG::INFO)<<"Track selector tool retrieved"<<endmsg; 
   
  if(m_sortingTool.retrieve().isFailure())
  {
   msg(MSG::ERROR)<<" Unable to retrieve "<<m_sortingTool<<endmsg;
   return StatusCode::FAILURE;
  }else msg(MSG::INFO)<<"Track sorting tool retrieved"<<endmsg;  
   
  if(m_beamService.retrieve().isFailure())
  {
   msg(MSG::ERROR)<<"Unable to retieve "<<m_beamService<<endmsg;
   return StatusCode::FAILURE;
  }else msg(MSG::INFO)<< "BeamSpot service retrieved"<<endmsg; 


  if(m_vtxSeedFinder.retrieve().isFailure())
  {
    msg(MSG::ERROR) << "Unable to retrieve " << m_vtxSeedFinder <<endmsg;
    return StatusCode::FAILURE;
  }else msg(MSG::INFO)<< "Vertex seed finder retriever" << endmsg; 
    
  if ( m_extrapolator.retrieve().isFailure() ) 
  {                              
   msg(MSG::ERROR) << "Failed to retrieve tool " << m_extrapolator << endmsg;
   return StatusCode::FAILURE;                                                  
  } else {                                                                       
   msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endmsg;
  }                                
  return StatusCode::SUCCESS;
 }//end of initialize mtehod
 
 StatusCode SlidingWindowMultiSeedFinder::finalize()
 {
  msg(MSG::INFO)  << "Finalize successful" << endmsg;
  return StatusCode::SUCCESS;
 }
 
 SlidingWindowMultiSeedFinder::SlidingWindowMultiSeedFinder(const std::string& t, const std::string& n, const
                                                            IInterface*p):AthAlgTool(t,n,p), 
                                                                          m_clusterLength(5.),
                                                                          m_breakingDistance(3.), 
                                                                          m_addingDistance(0.1), 
                                                                          m_useMaxInCluster(false),
                                                                          m_ignoreLevel(0),
                                                                          m_ignoreBeamSpot(false),
                                                                          m_vtxSeedFinder("Trk::CrossDistancesSeedFinder"),
                                                                          m_beamService("BeamCondSvc" ,n),
                                                                          m_extrapolator("Trk::Extrapolator") 
 {
  declareInterface<IMultiPVSeedFinder>(this); 
  declareProperty("clusterLength",      m_clusterLength);
  declareProperty("BreakingDistance",   m_breakingDistance);
  declareProperty("AddingDistance",     m_addingDistance);
  declareProperty("UseMaxInCluster",    m_useMaxInCluster);
  declareProperty("IgnoreLevel",	m_ignoreLevel);
  declareProperty("IgnoreBeamSpot",	m_ignoreBeamSpot);
  
//track filter  
   declareProperty("TrackSelector", m_trkFilter);

//sorting tool
  declareProperty("SortingTool", m_sortingTool);

//vertex finder tool (needed when no beam spot is available)
  declareProperty("VertexSeedFinder",m_vtxSeedFinder);
  
//beam spot finding tool
  declareProperty("BeamSpotSvc", m_beamService);

//extrapolator
  declareProperty("Extrapolator", m_extrapolator);  
  
 }
 
 SlidingWindowMultiSeedFinder::~SlidingWindowMultiSeedFinder()
 {}

 std::vector< std::vector<const Trk::Track *> > SlidingWindowMultiSeedFinder::seeds(const std::vector<const Trk::Track*>& tracks )const
 {
  std::vector<const Trk::Track*> preselectedTracks(0);
  std::vector<const Trk::Track*>::const_iterator tr = tracks.begin();
  std::vector<const Trk::Track*>::const_iterator tre = tracks.end(); 
  
  
  Trk::RecVertex  beamRecVertex(m_beamService->beamVtx());
  for(;tr!=tre;++tr) if(m_trkFilter->decision(**tr,&beamRecVertex)) preselectedTracks.push_back(*tr);
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Beam spot position is: "<< beamRecVertex.position()<<endmsg;

  Trk::Vertex* beamVertex=&beamRecVertex;
  
  if (m_ignoreBeamSpot)
  {
    Trk::Vertex* myVertex=new Trk::Vertex(m_vtxSeedFinder->findSeed(tracks));
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<
                              " vtx seed x: " << myVertex->position().x() << 
                              " vtx seed y: " << myVertex->position().y() << 
                              " vtx seed z: " << myVertex->position().z() << endmsg;
    beamVertex=myVertex;
  }


  
//output container  
  std::vector< std::vector<const Trk::Track *> > result(0);

//sorting the tracks on their z_0 basis.
  if(preselectedTracks.size() !=0)
  {
   std::vector<int> indexOfSorted = m_sortingTool->sortedIndex(preselectedTracks,beamVertex);  
   
   std::vector<const Trk::Track *> tmp_cluster(0); 
  
   Trk::PerigeeSurface perigeeSurface(beamVertex->position());

   const Trk::TrackParameters * exPerigee = 0;
   if (!indexOfSorted.empty()) exPerigee = 
				 m_extrapolator->extrapolate(*preselectedTracks[indexOfSorted[0]],perigeeSurface,Trk::anyDirection,true, Trk::pion);
         
   float lastTrackZ0  = -999.;
   if(exPerigee) { lastTrackZ0 = exPerigee->parameters()[Trk::z0]; delete exPerigee; }
   else
   {
     msg(MSG::WARNING)<<"Impossible to extrapolate the first track; returning 0 container for this event"<<endmsg;
     if (m_ignoreBeamSpot) delete beamVertex;
     return result;
   }          	 	 
	 	   	   
//looping over sorted container:
//storing the Z0 of last iteration...
   float prevTrackZ0 = 0.;
   
   float addingDistance = m_addingDistance;
   if(m_useMaxInCluster) addingDistance = 0.;
   for(unsigned int i=0;i<indexOfSorted.size();++i)
   {
    const  Trk::TrackParameters * lexPerigee = m_extrapolator->extrapolate(*preselectedTracks[indexOfSorted[i]],
									   perigeeSurface,Trk::anyDirection,true, Trk::pion); 
    float currentTrackZ0 = lexPerigee->parameters()[Trk::z0];
    delete lexPerigee;
    
    if(!i) prevTrackZ0 = currentTrackZ0; 
     
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Z0 of current  track"<< currentTrackZ0<<endmsg;    
    
    if((fabs(currentTrackZ0 - lastTrackZ0)>m_clusterLength && fabs(currentTrackZ0 - prevTrackZ0)>addingDistance )|| 
                                                              fabs(currentTrackZ0 - prevTrackZ0)>m_breakingDistance)
    {
  
//this track is outside the current cluster
     if(int(tmp_cluster.size())>m_ignoreLevel) result.push_back(tmp_cluster);
     tmp_cluster.clear();
     tmp_cluster.push_back(preselectedTracks[indexOfSorted[i]]);
     lastTrackZ0 = currentTrackZ0;
    }else{
 
//this track is within the current cluster    
     tmp_cluster.push_back(preselectedTracks[indexOfSorted[i]]);
     
//checking whethet this is the last track in list
     if(i==indexOfSorted.size()-1 && int(tmp_cluster.size())>m_ignoreLevel) result.push_back(tmp_cluster);
     
    }//end of check for the cluster size
        
//making adding distance max in cluster    
    if(m_useMaxInCluster)
    { 
      float diff = fabs(currentTrackZ0-prevTrackZ0);
      if(addingDistance < diff) addingDistance = diff; 
    }//end of  using max in cluster
    prevTrackZ0 = currentTrackZ0;
   }//end of loop over the sorted container, using sorted index
  
  }//end of check for successfull preselection
   
  if (m_ignoreBeamSpot) delete beamVertex;
  return result;
 }//end of seeding method
 
 
 std::vector< std::vector<const Trk::TrackParticleBase *> > SlidingWindowMultiSeedFinder::seeds(const std::vector<const Trk::TrackParticleBase*>& tracks )const
 {
  std::vector<const Trk::TrackParticleBase*> preselectedTracks(0);
  std::vector<const Trk::TrackParticleBase*>::const_iterator tr = tracks.begin();
  std::vector<const Trk::TrackParticleBase*>::const_iterator tre = tracks.end(); 
  
//using the beam position for pre-selection  
  
  Trk::RecVertex  beamRecVertex(m_beamService->beamVtx()); 
  for(;tr!=tre;++tr) if(m_trkFilter->decision(**tr,&beamRecVertex)) preselectedTracks.push_back(*tr);
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Beam spot position is: "<< beamRecVertex.position()<<endmsg;
  Trk::Vertex* beamVertex=&beamRecVertex;


  if (m_ignoreBeamSpot)
  {
    std::vector<const Trk::TrackParameters*> perigeeList;
    std::vector<const Trk::TrackParticleBase*>::const_iterator trackBegin=tracks.begin();
    std::vector<const Trk::TrackParticleBase*>::const_iterator trackEnd=tracks.end();
    for (std::vector<const Trk::TrackParticleBase*>::const_iterator trackIter=trackBegin;trackIter!=trackEnd;trackIter++)
    {
      perigeeList.push_back(&((*trackIter)->definingParameters()));
    }
    
    Trk::Vertex* myVertex=new Trk::Vertex(m_vtxSeedFinder->findSeed(perigeeList));

    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<
                              " vtx seed x: " << myVertex->position().x() << 
                              " vtx seed y: " << myVertex->position().y() << 
                              " vtx seed z: " << myVertex->position().z() << endmsg;
    beamVertex=myVertex;
  }

//output container  
  std::vector< std::vector<const Trk::TrackParticleBase*> > result(0);

//sorting the tracks on their z_0 basis.
  if(preselectedTracks.size() !=0)
  {
   std::vector<int> indexOfSorted = m_sortingTool->sortedIndex(preselectedTracks, beamVertex);
   
   std::vector<const Trk::TrackParticleBase *> tmp_cluster(0); 
   
//extrapolating the tracks to the actual beam spot   
   const Trk::TrackParameters * exPerigee(0);
   Trk::PerigeeSurface perigeeSurface(beamVertex->position());

   exPerigee = m_extrapolator->extrapolate(preselectedTracks[indexOfSorted[0]]->definingParameters(),
					   perigeeSurface,Trk::anyDirection,true, Trk::pion);
  
   float lastTrackZ0  = -999.;
   if(exPerigee) { lastTrackZ0 = exPerigee->parameters()[Trk::z0]; delete exPerigee; }
   else
   {
     msg(MSG::WARNING)<<"Impossible to extrapolate the first track; returning 0 container for this event"<<endmsg;
     if (m_ignoreBeamSpot) delete beamVertex;
     return result;
   }          	   

   if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Z0 of last track"<< lastTrackZ0<<endmsg;

   
//looping over sorted container:  storing the Z0 of last iteration...
   float prevTrackZ0 =0.;

   float addingDistance = m_addingDistance;
   if(m_useMaxInCluster) addingDistance = 0.;
   
   for(unsigned int i=0;i<indexOfSorted.size();++i)
   {
   
     const  Trk::TrackParameters * lexPerigee = m_extrapolator->extrapolate(preselectedTracks[indexOfSorted[i]]->definingParameters(),
									    perigeeSurface,Trk::anyDirection,true, Trk::pion); 
    float currentTrackZ0 = lexPerigee->parameters()[Trk::z0];
    delete lexPerigee;
    
    if(!i)prevTrackZ0 = currentTrackZ0;    
    
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Z0 of current  track"<< currentTrackZ0<<endmsg;   
    if((fabs(currentTrackZ0 - lastTrackZ0)>m_clusterLength && fabs(currentTrackZ0 - prevTrackZ0)>addingDistance )|| 
                                                              fabs(currentTrackZ0 - prevTrackZ0)>m_breakingDistance)
    {
  
//this track is outside the current cluster or the distance is too big. breaking the cluster here
     if(int(tmp_cluster.size())>m_ignoreLevel) result.push_back(tmp_cluster);
     tmp_cluster.clear();
     tmp_cluster.push_back(preselectedTracks[indexOfSorted[i]]);
     lastTrackZ0 = currentTrackZ0;
     if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Finishing a cluster, starting the new one"<<endmsg;
    
    }else{
 
//this track is within the current cluster  or the distance to next is too short. Keeping populating it.
  
     tmp_cluster.push_back(preselectedTracks[indexOfSorted[i]]);
     if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Pushing a track to a cluster"<<endmsg;
     
     if(i==indexOfSorted.size()-1 && int(tmp_cluster.size())>m_ignoreLevel) result.push_back(tmp_cluster);
     
    }//end of check for the cluster size
    
//making adding distance max in cluster    
    if(m_useMaxInCluster)
    { 
      float diff = fabs(currentTrackZ0-prevTrackZ0);
      if(addingDistance < diff) addingDistance = diff; 
    }//end of  using max in cluster
    prevTrackZ0 = currentTrackZ0;
   }//end of loop
  }//end of preselection size check
  
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Returning number of clusters: "<< result.size()<<endmsg;
  if (m_ignoreBeamSpot) delete beamVertex;
  return result; 
   
 }//end of seeding method for TrackParticles


std::vector< std::vector<const Trk::TrackParameters *> > SlidingWindowMultiSeedFinder::seeds(const std::vector<const xAOD::TrackParticle*>& tracks )const
  {
 
    //step 1: preselection 
    std::vector<const xAOD::TrackParticle*> preselectedTracks(0);
    
    //selecting with respect to the beam spot
    
    
    xAOD::Vertex * beamposition = new xAOD::Vertex(); 
    beamposition->setPosition(m_beamService->beamVtx().position());
    beamposition->setCovariancePosition(m_beamService->beamVtx().covariancePosition());
    // for(;tr!=tre;++tr) if(m_trkFilter->decision(**tr, &beamrecposition)) preselectedTracks.push_back(*tr);
    // if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Beam spot position is: "<< beamrecposition.position()<<endmsg;
    //Trk::Vertex* beamposition=&beamrecposition;
    
    for (std::vector<const xAOD::TrackParticle*>::const_iterator itr  = tracks.begin(); itr != tracks.end(); ++itr) {
      if (m_trkFilter->decision(**itr,beamposition)) preselectedTracks.push_back(*itr);
    }
    
    
    
    std::vector<const Trk::TrackParameters*> perigeeList;
    std::vector<const xAOD::TrackParticle*>::const_iterator trackBegin=tracks.begin();
    std::vector<const xAOD::TrackParticle*>::const_iterator trackEnd=tracks.end();
    for (std::vector<const xAOD::TrackParticle*>::const_iterator trackIter=trackBegin;trackIter!=trackEnd;trackIter++)
      {
	perigeeList.push_back(&((*trackIter)->perigeeParameters()));
      }
 
   Trk::RecVertex* myVertex=new Trk::RecVertex(m_vtxSeedFinder->findSeed(perigeeList)); 
   
    if (m_ignoreBeamSpot)
      {
	
	if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<
				" vtx seed x: " << myVertex->position().x() << 
				" vtx seed y: " << myVertex->position().y() << 
				" vtx seed z: " << myVertex->position().z() << endmsg;
	beamposition->setPosition(myVertex->position());
	beamposition->setCovariancePosition(myVertex->covariancePosition());
	//hmm, will this leak? I think so.
      }
    
    // std::cout<<"Number after pre-selection : "<<preselectedTracks.size()<<std::endl;
    
    //step 2: sorting in z0
    //output container  
    std::vector< std::vector<const Trk::TrackParameters *> > result(0);
    if(preselectedTracks.size() !=0)
      {
	std::vector<int> indexOfSorted = m_sortingTool->sortedIndex(preselectedTracks, beamposition);
	
	std::vector<const Trk::TrackParameters *> tmp_cluster(0); 
	
	//extrapolating the tracks to the actual beam spot   
	const Trk::TrackParameters * exPerigee(0);
	Trk::PerigeeSurface perigeeSurface(beamposition->position());
	
	exPerigee = m_extrapolator->extrapolate(*preselectedTracks[indexOfSorted[0]],
						perigeeSurface,Trk::anyDirection,true, Trk::pion);
	
	float lastTrackZ0  = -999.;
	if(exPerigee) { lastTrackZ0 = exPerigee->parameters()[Trk::z0]; delete exPerigee; }
	else
	  {
	    msg(MSG::WARNING)<<"Impossible to extrapolate the first track; returning 0 container for this event"<<endmsg;
	    delete beamposition; 
	    delete myVertex;
	    return result;
	  }          	   
	
	if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Z0 of last track"<< lastTrackZ0<<endmsg;
	
	
	//looping over sorted container:  storing the Z0 of last iteration...
	float prevTrackZ0 =0.;

        float addingDistance = m_addingDistance;
	if(m_useMaxInCluster) addingDistance = 0.;
	
	for(unsigned int i=0;i<indexOfSorted.size();++i)
	  {
	    
	    const  Trk::TrackParameters * lexPerigee = m_extrapolator->extrapolate(*preselectedTracks[indexOfSorted[i]],
										   perigeeSurface,Trk::anyDirection,true, Trk::pion); 
	    float currentTrackZ0 = lexPerigee->parameters()[Trk::z0];
	    delete lexPerigee;
	    
	    if(!i)prevTrackZ0 = currentTrackZ0;    
	    
	    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Z0 of current  track"<< currentTrackZ0<<endmsg;   
	    if((fabs(currentTrackZ0 - lastTrackZ0)>m_clusterLength && fabs(currentTrackZ0 - prevTrackZ0)>addingDistance )|| 
	       fabs(currentTrackZ0 - prevTrackZ0)>m_breakingDistance)
	      {
		
		//this track is outside the current cluster or the distance is too big. breaking the cluster here
		if(int(tmp_cluster.size())>m_ignoreLevel) result.push_back(tmp_cluster);
		tmp_cluster.clear();
		tmp_cluster.push_back(&(preselectedTracks[indexOfSorted[i]]->perigeeParameters()));
		lastTrackZ0 = currentTrackZ0;
		if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Finishing a cluster, starting the new one"<<endmsg;
		
	      }else{
	      
	      //this track is within the current cluster  or the distance to next is too short. Keeping populating it.
	      
	      tmp_cluster.push_back(&(preselectedTracks[indexOfSorted[i]]->perigeeParameters()));
	      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Pushing a track to a cluster"<<endmsg;
	      
	      if(i==indexOfSorted.size()-1 && int(tmp_cluster.size())>m_ignoreLevel) result.push_back(tmp_cluster);
	      
	    }//end of check for the cluster size
	    
	    //making adding distance max in cluster    
	    if(m_useMaxInCluster)
	      { 
		float diff = fabs(currentTrackZ0-prevTrackZ0);
		if(addingDistance < diff) addingDistance = diff; 
	      }//end of  using max in cluster
	    prevTrackZ0 = currentTrackZ0;
	  }//end of loop
      }//end of preselection size check
    
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Returning number of clusters: "<< result.size()<<endmsg;
    delete beamposition; 
    delete myVertex;
    return result; 	
    
  }
  /*  
 std::vector<int> SlidingWindowMultiSeedFinder::m_z0sort(std::vector<const xAOD::TrackParticle*>& tracks,xAOD::Vertex * reference) const
 {
   
    // std::vector<int> no_perigee(0);
    std::map<double, int> mapOfZ0; 
    std::vector<const xAOD::TrackParticle*>::const_iterator tb = tracks.begin();
    std::vector<const xAOD::TrackParticle*>::const_iterator te = tracks.end();
    unsigned int j=0;
    
    for(;tb != te ;++tb)
      {
	const Trk::TrackParameters * perigee = 0;
	
	
	//here we want to make an extrapolation    
	Trk::PerigeeSurface perigeeSurface(reference->position());
	perigee = m_extrapolator->extrapolate(**tb,
					      perigeeSurface,
					      Trk::anyDirection,true, Trk::pion);  
	
	if(perigee)
	  {
	    double trkZ0 = perigee->parameters()[Trk::z0];
	    mapOfZ0.insert(std::map<double, int>::value_type(trkZ0,j)); 
	    delete perigee;
	    perigee =0;
	    
	  }else{
	  msg(MSG::WARNING)  << "This track particle has no perigee state. Not egligible for sorting. Will NOT be written to the sorted vector" << endmsg;
	  //    no_perigee.push_back(j);
	}//end of perigee existance check
	++j;
      }//end of loop over all track particle base's
    
    //creating an output vector, filling it and returning
    std::vector<int> result(0);
    
    //sorted part  
    std::map<double, int>::const_iterator mb = mapOfZ0.begin();
    std::map<double, int>::const_iterator me = mapOfZ0.end(); 
    for(;mb!=me;++mb) result.push_back((*mb).second);
    
    //part failed sorting
    //  std::vector<int>::const_iterator ib = no_perigee.begin();
    //  std::vector<int>::const_iterator ie = no_perigee.end();
    //  for(;ib!=ie;++ib) result.push_back(*ib);  
    return result;
  }
  */    
    
}//end of namespace definitions

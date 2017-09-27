/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetMultipleVertexSeedFinder/DivisiveMultiSeedFinder.h"
#include "TrkTrack/Track.h"
//#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "InDetMultipleVertexSeedFinderUtils/InDetTrackZ0SortingTool.h"
#include "InDetMultipleVertexSeedFinderUtils/InDetTrackClusterCleaningTool.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "InDetRecToolInterfaces/IMultiPVSeedFinder.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "xAODTracking/Vertex.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace InDet
{

 StatusCode DivisiveMultiSeedFinder::initialize()
 {
 
  if(m_trkFilter.retrieve().isFailure())
  {
   msg(MSG::ERROR)<<" Unable to retrieve "<<m_trkFilter<<endmsg;
   return StatusCode::FAILURE;
  }else msg(MSG::INFO)<<"Track filter retrieved"<<endmsg; 
   
  if(m_sortingTool.retrieve().isFailure())
  {
   msg(MSG::ERROR)<<" Unable to retrieve "<<m_sortingTool<<endmsg;
   return StatusCode::FAILURE;
  }else msg(MSG::INFO)<<"Track sorting tool retrieved"<<endmsg;  
  
  if(m_vtxSeedFinder.retrieve().isFailure())
  {
    msg(MSG::ERROR) << "Unable to retrieve " << m_vtxSeedFinder <<endmsg;
    return StatusCode::FAILURE;
  }else msg(MSG::INFO)<< "Vertex seed finder retriever" << endmsg; 

  if(m_cleaningTool.retrieve().isFailure())
  {
   msg(MSG::ERROR)<<" Unable to retrieve "<<m_cleaningTool<<endmsg;
   return StatusCode::FAILURE;
  }else msg(MSG::INFO)<<"Track sorting tool retrieved"<<endmsg;  
   
  if(m_beamService.retrieve().isFailure())
  {
   msg(MSG::ERROR)<<"Unable to retrieve "<<m_beamService<<endmsg;
   return StatusCode::FAILURE;
  }else msg(MSG::INFO)<<"BeamSpot service retrieved"<<endmsg; 
   
  if ( m_extrapolator.retrieve().isFailure() ) 
  {                              
   msg(MSG::ERROR) << "Failed to retrieve tool " << m_extrapolator << endmsg;
   return StatusCode::FAILURE;                                                  
  } else msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endmsg; 
   
  return StatusCode::SUCCESS;
 }//end of initialize mtehod
 
 StatusCode DivisiveMultiSeedFinder::finalize()
 {
  msg(MSG::INFO)  << "Finalize successful" << endmsg;
  return StatusCode::SUCCESS;
 }
 
 DivisiveMultiSeedFinder::DivisiveMultiSeedFinder(const std::string& t, const std::string& n, const
 IInterface*p):AthAlgTool(t,n,p), 
               m_sepDistance(0.5), 
               m_nRemaining(1),
               m_ignoreBeamSpot(false),
               m_beamService("BeamCondSvc",n),
               m_extrapolator("Trk::Extrapolator"), 
               m_vtxSeedFinder("Trk::CrossDistancesSeedFinder")
 {
  declareInterface<IMultiPVSeedFinder>(this); 
  declareProperty("separationDistance",       m_sepDistance);
  declareProperty("nRemainTracks",            m_nRemaining);
  declareProperty("IgnoreBeamSpot",	m_ignoreBeamSpot);
  
//track filter  
  declareProperty("TrackSelector",m_trkFilter);
    
//sorting tool
  declareProperty("SortingTool", m_sortingTool);

//cleaning tool
  declareProperty("CleaningTool", m_cleaningTool);
  
//vertex finder tool (needed when no beam spot is available)
  declareProperty("VertexSeedFinder",m_vtxSeedFinder);

//beam spot service  
  declareProperty("BeamSpotSvc", m_beamService);
  
//extrapolator
  declareProperty("Extrapolator",m_extrapolator);   
  
 }//end of constructor
 
 DivisiveMultiSeedFinder::~DivisiveMultiSeedFinder()
 {}

 std::vector< std::vector<const Trk::Track *> > DivisiveMultiSeedFinder::seeds(const std::vector<const Trk::Track*>& tracks )const
 {
//step 1: preselection 
  std::vector<const Trk::Track*> preselectedTracks(0);
  std::vector<const Trk::Track*>::const_iterator tr = tracks.begin();
  std::vector<const Trk::Track*>::const_iterator tre = tracks.end(); 
  
//selecting with respect to the beam spot
  Trk::RecVertex beamrecposition(m_beamService->beamVtx());  
  for(;tr!=tre;++tr) if(m_trkFilter->decision(**tr,&beamrecposition)) preselectedTracks.push_back(*tr);
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Beam spot position is: "<< beamrecposition.position()<<endmsg;
  Trk::Vertex* beamposition=&beamrecposition;
  
  if (m_ignoreBeamSpot)
  {
    Trk::Vertex* myVertex=new Trk::Vertex(m_vtxSeedFinder->findSeed(tracks));
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<
                              " vtx seed x: " << myVertex->position().x() << 
                              " vtx seed y: " << myVertex->position().y() << 
                              " vtx seed z: " << myVertex->position().z() << endmsg;
    beamposition=myVertex;
  }

//step 2: sorting in z0
//output container  
  std::vector< std::vector<const Trk::Track *> > result(0);
  if(preselectedTracks.size() !=0)
  {
    //std::vector<int> indexOfSorted = m_z0sort(preselectedTracks, beamposition);
    std::vector<int> indexOfSorted = m_sortingTool->sortedIndex(preselectedTracks, beamposition);
//step 3 preclustering
   std::vector< std::vector<const Trk::Track *> > preClusters(0);

//left-handed track position
   std::vector<const Trk::Track *> tmp_cluster(0); 
   
   Trk::PerigeeSurface perigeeSurface(beamposition->position());

   const Trk::TrackParameters * exPerigee = m_extrapolator->extrapolate(*preselectedTracks[indexOfSorted[0]],
									perigeeSurface,Trk::anyDirection,true, Trk::pion);
         
   double lastTrackZ0  = -999.;
   if(exPerigee) { lastTrackZ0 = exPerigee->parameters()[Trk::z0]; delete exPerigee; }
   else
   {
     msg(MSG::WARNING)<<"Impossible to extrapolate the first track; returning 0 container for this event"<<endmsg;
     if (m_ignoreBeamSpot) delete beamposition;
     return result;
   }          	 	 

//looping over container
   for(unsigned int i=0;i<indexOfSorted.size();++i)
   { 
     const  Trk::TrackParameters * lexPerigee = m_extrapolator->extrapolate(*preselectedTracks[indexOfSorted[i]],
									    perigeeSurface,Trk::anyDirection,true, Trk::pion); 
					   
    double currentTrackZ0 = lexPerigee->parameters()[Trk::z0];
    delete lexPerigee;
  
//    double currentTrackZ0 = preselectedTracks[indexOfSorted[i]]->perigeeParameters()->parameters()[Trk::z0];
   
    if(fabs(currentTrackZ0 - lastTrackZ0)<m_sepDistance)
    {
  
//the distance is below separation, adding to the same cluster  
     tmp_cluster.push_back(preselectedTracks[indexOfSorted[i]]);
    }else{
    
//the distance is above separation, starting new cluster    
     preClusters.push_back(tmp_cluster);
     tmp_cluster.clear();
     tmp_cluster.push_back(preselectedTracks[indexOfSorted[i]]);
    }//end of gap size check  
    lastTrackZ0 = currentTrackZ0;
   }//end of loop over the sorted container
   
 //storing the last (or the only cluster)  
   preClusters.push_back(tmp_cluster);

//step 4 iterative cleaning of clusters
   for(std::vector< std::vector<const Trk::Track *> >::const_iterator i = preClusters.begin();
                                                              i != preClusters.end();++i)
   {    
//------------------------------Debug code -------------------------------------------------------   
/*    std::vector<const Trk::Track *>::const_iterator cb = i->begin();
    std::vector<const Trk::Track *>::const_iterator ce = i->end();
    std::cout<<"*********Starting next cluster of size "<<i->size()<<std::endl;
    for(;cb!=ce;++cb)
    {
     std::cout<<"Track Z0 in cluster: "<<(*cb)->perigeeParameters()->parameters()[Trk::z0]<<std::endl;
    }//end of loop over the cluster entries
*/    
//-------------------------------end of debug code-------------------------------------------------
   
   if(i->size()>m_nRemaining)
    {
    
//iterative cleaning until outlying tracks remain 
     std::vector<const Trk::Track *> tracks_to_clean = *i;  
     bool clean_again = false;
//     unsigned int clean_count = 1; 
     do
     {
      std::pair<std::vector<const Trk::Track *>, std::vector<const Trk::Track *> > clusterAndOutl = 
                                                m_cleaningTool->clusterAndOutliers(tracks_to_clean, beamposition);
      
//if core size is miningfull, storing it 
      std::vector<const Trk::Track *> core_cluster = clusterAndOutl.first;
      std::vector<const Trk::Track *> core_outl = clusterAndOutl.second;    
      
//--------------Debug output-----------------------------------------------------
//      std::cout<<"Cleaning iteration "<<clean_count<<std::endl;
//      std::cout<<"Reduced cluster size: "<<core_cluster.size()<<std::endl;
//      std::cout<<"Outliers size:        "<<core_outl.size()<<std::endl;
//      ++clean_count;           
//-------------------End of debug output -----------------------------------------
      if(core_cluster.size()==0)
      {
       msg(MSG::INFO)  << "Core cluster has 0 size, remaining tracks are discarded. "<< endmsg;
       clean_again = false;
      }else{
      
//storing clusters with >1 track (optional)      
       if(core_cluster.size()>1) result.push_back(core_cluster);	
      
//checking the outliers, whether more cleaning is to be done      
       if(core_outl.size()>m_nRemaining)
       {
        clean_again = true;
        tracks_to_clean.clear();
        tracks_to_clean = core_outl;
	
       }else if(core_outl.size()>1){
        clean_again = false;
        msg(MSG::INFO)  << "There were remaining outliers of size: "<< core_outl.size()<< endmsg;
        msg(MSG::INFO)  << "Not evident, whether these tracks form a cluster. Rejected..."<< endmsg;
	
       }else  clean_again = false;//end of outlier size check  
      }//end of core cluster 0 check   
     
     }while(clean_again);//end of loop
              
    }else if(i->size()==2){
    
//case of two track cluster. accepting without cleaning  
     result.push_back(*i);
//     std::cout<<"Adding a small cluster of size: "<<i->size()<<std::endl;
    }//end of cluster size check
   }//end of loop over all the clusters
  }//end of preselection size check 
  if (m_ignoreBeamSpot) delete beamposition;
  return result;  
 }//end of clustering method
 
 std::vector< std::vector<const Trk::TrackParticleBase *> > DivisiveMultiSeedFinder::seeds(const std::vector<const Trk::TrackParticleBase*>& tracks )const
 {
 
 // std::cout<<"Number of tracks received: "<<tracks.size()<<std::endl;
  
  //step 1: preselection 
  std::vector<const Trk::TrackParticleBase*> preselectedTracks(0);
  std::vector<const Trk::TrackParticleBase*>::const_iterator tr = tracks.begin();
  std::vector<const Trk::TrackParticleBase*>::const_iterator tre = tracks.end(); 
  
//selecting with respect to the beam spot

  Trk::RecVertex beamrecposition(m_beamService->beamVtx());    
  for(;tr!=tre;++tr) if(m_trkFilter->decision(**tr, &beamrecposition)) preselectedTracks.push_back(*tr);
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Beam spot position is: "<< beamrecposition.position()<<endmsg;
  Trk::Vertex* beamposition=&beamrecposition;


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
    beamposition=myVertex;

  }

 // std::cout<<"Number after pre-selection : "<<preselectedTracks.size()<<std::endl;
  
//step 2: sorting in z0
//output container  
  std::vector< std::vector<const Trk::TrackParticleBase *> > result(0);
  if(preselectedTracks.size() !=0)
  {
   std::vector<int> indexOfSorted = m_sortingTool->sortedIndex(preselectedTracks, beamposition);

//step 3 preclustering
   std::vector< std::vector<const Trk::TrackParticleBase *> > preClusters(0);

//left-handed track position
   std::vector<const Trk::TrackParticleBase *> tmp_cluster(0); 
   
   Trk::PerigeeSurface perigeeSurface(beamposition->position());
   const Trk::TrackParameters * exPerigee = m_extrapolator->extrapolate(preselectedTracks[indexOfSorted[0]]->definingParameters(),
									perigeeSurface,Trk::anyDirection,true, Trk::pion);
         
   double lastTrackZ0  = -999.;
   if(exPerigee) { lastTrackZ0 = exPerigee->parameters()[Trk::z0]; delete exPerigee; }
   else
   {
     msg(MSG::WARNING)<<"Impossible to extrapolate the first track; returning 0 container for this event"<<endmsg;
     if (m_ignoreBeamSpot) delete beamposition;     
     return result;
   }          	 	 
    
//looping over container
   for(unsigned int i=0;i<indexOfSorted.size();++i)
   { 
     const  Trk::TrackParameters * lexPerigee = m_extrapolator->extrapolate(preselectedTracks[indexOfSorted[i]]->definingParameters(),
									    perigeeSurface,Trk::anyDirection,true, Trk::pion); 
					   
    double currentTrackZ0 = lexPerigee->parameters()[Trk::z0];
    delete lexPerigee;
   
    if(fabs(currentTrackZ0 - lastTrackZ0)<m_sepDistance)
    {
  
//the distance is below separation, adding to the same cluster  
     tmp_cluster.push_back(preselectedTracks[indexOfSorted[i]]);
     
  //   std::cout<<"Adding to a cluster "<<std::endl;
    }else{
    
//     std::cout<<"Breaking a cluster "<<std::endl;
//the distance is above separation, starting new cluster    
     preClusters.push_back(tmp_cluster);
     tmp_cluster.clear();
     tmp_cluster.push_back(preselectedTracks[indexOfSorted[i]]);
    }//end of gap size check  
    lastTrackZ0 = currentTrackZ0;
   }//end of loop over the sorted container
   

//storing the last (or the only) cluster   
   preClusters.push_back(tmp_cluster);
   msg(MSG::DEBUG)<<"The preselection returns clusters: "<<preClusters.size()<<endmsg;

//step 4 iterative cleaning of clusters
 //  std::cout<<"Number of clusters before cleaning: "<<preClusters.size()<<std::endl;

   for(std::vector< std::vector<const Trk::TrackParticleBase *> >::const_iterator i = preClusters.begin();
                                                              i != preClusters.end();++i)
   {    
//------------------------------Debug code -------------------------------------------------------   
/*    std::vector<const Trk::Track *>::const_iterator cb = i->begin();
    std::vector<const Trk::Track *>::const_iterator ce = i->end();
    std::cout<<"*********Starting next cluster of size "<<i->size()<<std::endl;
    for(;cb!=ce;++cb)
    {
     std::cout<<"Track Z0 in cluster: "<<(*cb)->perigeeParameters()->parameters()[Trk::z0]<<std::endl;
    }//end of loop over the cluster entries
*/    
//-------------------------------end of debug code-------------------------------------------------
   
   if(i->size()>m_nRemaining)
    {
    
//iterative cleaning until outlying tracks remain 
     std::vector<const Trk::TrackParticleBase *> tracks_to_clean = *i;  
     bool clean_again = false;
//     unsigned int clean_count = 1; 
     do
     {
      std::pair<std::vector<const Trk::TrackParticleBase *>, 
      std::vector<const Trk::TrackParticleBase *> > clusterAndOutl = 
                                                m_cleaningTool->clusterAndOutliers(tracks_to_clean, beamposition);
      
//if core size is miningfull, storing it 
      std::vector<const Trk::TrackParticleBase *> core_cluster = clusterAndOutl.first;
      std::vector<const Trk::TrackParticleBase *> core_outl = clusterAndOutl.second;    
      
//--------------Debug output-----------------------------------------------------
//      std::cout<<"Cleaning iteration "<<clean_count<<std::endl;
//      std::cout<<"Reduced cluster size: "<<core_cluster.size()<<std::endl;
//      std::cout<<"Outliers size:        "<<core_outl.size()<<std::endl;
//      ++clean_count;           
//-------------------End of debug output -----------------------------------------
      if(core_cluster.size()==0)
      {
      
       msg(MSG::INFO)  << "Core cluster has 0 size, remaining tracks are discarded. "<< endmsg;
       clean_again = false;
      }else{
      
//storing clusters with >1 track (optional)      
       if(core_cluster.size()>1) result.push_back(core_cluster);	
      
//checking the outliers, whether more cleaning is to be done      
       if(core_outl.size()>m_nRemaining)
       {
        clean_again = true;
        tracks_to_clean.clear();
        tracks_to_clean = core_outl;
	
       }else if(core_outl.size()>1)
       {
        clean_again = false;
        msg(MSG::INFO)  << "There were remaining outliers of size: "<< core_outl.size()<< endmsg;
        msg(MSG::INFO)  << "Not evident, whether these tracks form a cluster. Rejected..."<< endmsg;
	
       }else  clean_again = false;//end of outlier size check  
      }//end of core cluster 0 check   
     
     }while(clean_again);//end of loop
              
    }else if(i->size()==2)
    {
    
//case of two track cluster. accepting without cleaning  
     result.push_back(*i);
    }//end of cluster size check
   }//end of loop over all the clusters
  }//end of preselection size check 

  if (m_ignoreBeamSpot) delete beamposition;
  return result;
 }
 
  std::vector< std::vector<const Trk::TrackParameters *> > DivisiveMultiSeedFinder::seeds(const std::vector<const xAOD::TrackParticle*>& tracks )const
  {
  // std::cout<<"Number of tracks received: "<<tracks.size()<<std::endl;
  
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
	  }

 // std::cout<<"Number after pre-selection : "<<preselectedTracks.size()<<std::endl;
  
//step 2: sorting in z0
//output container  
  std::vector< std::vector<const Trk::TrackParameters *> > result(0);
  if(preselectedTracks.size() !=0)
  {
  
    std::vector<int> indexOfSorted =  m_sortingTool->sortedIndex(preselectedTracks, beamposition);

   //need new sort method, either for xAODTrackParticles, or TrackParameters. Neither currently supported...

//step 3 preclustering
    std::vector< std::vector<const xAOD::TrackParticle *> > preClusters(0);

//left-handed track position
   std::vector<const xAOD::TrackParticle *> tmp_cluster(0); 
   
   Trk::PerigeeSurface perigeeSurface(beamposition->position());
   const Trk::TrackParameters * exPerigee = m_extrapolator->extrapolate(*preselectedTracks[indexOfSorted[0]],
   perigeeSurface,Trk::anyDirection,true, Trk::pion);
   
   double lastTrackZ0  = -999.;
   if(exPerigee) { 
     lastTrackZ0 = exPerigee->parameters()[Trk::z0];delete exPerigee;
   }
   else
     {
       msg(MSG::WARNING)<<"Impossible to extrapolate the first track; returning 0 container for this event"<<endmsg;  
       delete beamposition; 
       delete myVertex;
       return result;
     }          	 	 
   
   //looping over container
   for(unsigned int i=0;i<indexOfSorted.size();++i)
     { 
       const Trk::TrackParameters * lexPerigee = m_extrapolator->extrapolate(*preselectedTracks[indexOfSorted[i]],
									     perigeeSurface,Trk::anyDirection,true, Trk::pion);
   
       
       double currentTrackZ0 = lexPerigee->parameters()[Trk::z0];
       delete lexPerigee;
	
       if(fabs(currentTrackZ0 - lastTrackZ0)<m_sepDistance)
	 {
	   
	   //the distance is below separation, adding to the same cluster  
	   tmp_cluster.push_back(preselectedTracks[indexOfSorted[i]]);
	   
	   //   std::cout<<"Adding to a cluster "<<std::endl;
	 }else{
	 
	 //     std::cout<<"Breaking a cluster "<<std::endl;
	 //the distance is above separation, starting new cluster    
	 preClusters.push_back(tmp_cluster);
	 tmp_cluster.clear();
	 tmp_cluster.push_back(preselectedTracks[indexOfSorted[i]]);
       }//end of gap size check  
       lastTrackZ0 = currentTrackZ0;
     }//end of loop over the sorted container
   
   
   //storing the last (or the only) cluster   
   preClusters.push_back(tmp_cluster);
   msg(MSG::DEBUG)<<"The preselection returns clusters: "<<preClusters.size()<<endmsg;
   
   //step 4 iterative cleaning of clusters
   //  std::cout<<"Number of clusters before cleaning: "<<preClusters.size()<<std::endl;
   
   for(std::vector< std::vector<const xAOD::TrackParticle *> >::const_iterator i = preClusters.begin();
       i != preClusters.end();++i)
     {    
//------------------------------Debug code -------------------------------------------------------   
/*    std::vector<const Trk::Track *>::const_iterator cb = i->begin();
    std::vector<const Trk::Track *>::const_iterator ce = i->end();
    std::cout<<"*********Starting next cluster of size "<<i->size()<<std::endl;
    for(;cb!=ce;++cb)
    {
     std::cout<<"Track Z0 in cluster: "<<(*cb)->perigeeParameters()->parameters()[Trk::z0]<<std::endl;
    }//end of loop over the cluster entries
*/    
//-------------------------------end of debug code-------------------------------------------------
   
   if(i->size()>m_nRemaining)
    {
    
//iterative cleaning until outlying tracks remain 
     std::vector<const xAOD::TrackParticle *> tracks_to_clean = *i;  
     bool clean_again = false;
//     unsigned int clean_count = 1; 
     do
     {
      std::pair<std::vector<const Trk::TrackParameters *>, 
      std::vector<const xAOD::TrackParticle *> > clusterAndOutl = m_cleaningTool->clusterAndOutliers(tracks_to_clean, beamposition);
      
//if core size is miningfull, storing it 
      std::vector<const Trk::TrackParameters *> core_cluster = clusterAndOutl.first;
      std::vector<const xAOD::TrackParticle *> core_outl = clusterAndOutl.second;    
      
//--------------Debug output-----------------------------------------------------
//      std::cout<<"Cleaning iteration "<<clean_count<<std::endl;
//      std::cout<<"Reduced cluster size: "<<core_cluster.size()<<std::endl;
//      std::cout<<"Outliers size:        "<<core_outl.size()<<std::endl;
//      ++clean_count;           
//-------------------End of debug output -----------------------------------------
      if(core_cluster.size()==0)
      {
      
       msg(MSG::INFO)  << "Core cluster has 0 size, remaining tracks are discarded. "<< endmsg;
       clean_again = false;
      }else{
      
//storing clusters with >1 track (optional)      
	if(core_cluster.size()>1) result.push_back(core_cluster);	
      
//checking the outliers, whether more cleaning is to be done      
       if(core_outl.size()>m_nRemaining)
       {
        clean_again = true;
        tracks_to_clean.clear();
        tracks_to_clean = core_outl;
	
       }else if(core_outl.size()>1)
       {
        clean_again = false;
        msg(MSG::INFO)  << "There were remaining outliers of size: "<< core_outl.size()<< endmsg;
        msg(MSG::INFO)  << "Not evident, whether these tracks form a cluster. Rejected..."<< endmsg;
	
       }else  clean_again = false;//end of outlier size check  
      }//end of core cluster 0 check   
     
     }while(clean_again);//end of loop
             
    }else if(i->size()==2)
     {
      //case of two track cluster. accepting without cleaning  
       std::vector<const Trk::TrackParameters *> twotrack;
       twotrack.push_back(&((*i)[0]->perigeeParameters()));
       twotrack.push_back(&((*i)[1]->perigeeParameters()));
      result.push_back(twotrack);
	
      }//end of cluster size check
   }//end of loop over all the clusters
  }//end of preselection size check 
  delete beamposition; 
  delete myVertex;
  return result;
    
  }
  /*
  std::vector<int> DivisiveMultiSeedFinder::m_z0sort(std::vector<const xAOD::TrackParticle*>& tracks,xAOD::Vertex * reference) const
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
  /*
  std::pair<std::vector<const Trk::TrackParameters *>, 
	    std::vector<const xAOD::TrackParticle *> > DivisiveMultiSeedFinder::m_clusterAndOutliers(std::vector<const xAOD::TrackParticle *> cluster, xAOD::Vertex * reference) const
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
	      
	      z_center = z_center/cluster_size;
	      
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
  */  
}//end of namespace definitions

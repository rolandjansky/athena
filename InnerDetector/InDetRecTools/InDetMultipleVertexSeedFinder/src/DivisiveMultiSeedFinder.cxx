/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetMultipleVertexSeedFinder/DivisiveMultiSeedFinder.h"
#include "TrkTrack/Track.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "InDetMultipleVertexSeedFinderUtils/InDetTrackZ0SortingTool.h"
#include "InDetMultipleVertexSeedFinderUtils/InDetTrackClusterCleaningTool.h"
#include "TrkParticleBase/TrackParticleBase.h"
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
   
  ATH_CHECK(m_beamSpotKey.initialize());
   
  if ( m_extrapolator.retrieve().isFailure() ) 
  {                              
   msg(MSG::ERROR) << "Failed to retrieve tool " << m_extrapolator << endmsg;
   return StatusCode::FAILURE;                                                  
  } else msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endmsg; 
   
  return StatusCode::SUCCESS;
 }//end of initialize mtehod

 
 DivisiveMultiSeedFinder::DivisiveMultiSeedFinder(const std::string& t, const std::string& n, const
 IInterface*p):AthAlgTool(t,n,p), 
               m_sepDistance(0.5), 
               m_nRemaining(1),
               m_ignoreBeamSpot(false),
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


  
//extrapolator
  declareProperty("Extrapolator",m_extrapolator);   
  
 }//end of constructor
 
 DivisiveMultiSeedFinder::~DivisiveMultiSeedFinder()
 {}

 std::vector< std::vector<const Trk::Track *> > DivisiveMultiSeedFinder::seeds(const std::vector<const Trk::Track*>& tracks )const
 {
  const EventContext& ctx = Gaudi::Hive::currentContext();
//step 1: preselection 
  std::vector<const Trk::Track*> preselectedTracks(0);
  std::vector<const Trk::Track*>::const_iterator tr = tracks.begin();
  std::vector<const Trk::Track*>::const_iterator tre = tracks.end(); 
  
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx};
  Trk::RecVertex beamrecposition(beamSpotHandle->beamVtx());  
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
  if(!preselectedTracks.empty())
  {
    //std::vector<int> indexOfSorted = m_z0sort(preselectedTracks, beamposition);
    std::vector<int> indexOfSorted = m_sortingTool->sortedIndex(preselectedTracks, beamposition);
//step 3 preclustering
   std::vector< std::vector<const Trk::Track *> > preClusters(0);

//left-handed track position
   std::vector<const Trk::Track *> tmp_cluster(0); 
   
   Trk::PerigeeSurface perigeeSurface(beamposition->position());

   const Trk::TrackParameters* exPerigee =
     m_extrapolator
       ->extrapolate(ctx, *preselectedTracks[indexOfSorted[0]], perigeeSurface, Trk::anyDirection, true, Trk::pion)
       .release();

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
     const Trk::TrackParameters* lexPerigee =
       m_extrapolator
         ->extrapolate(ctx, *preselectedTracks[indexOfSorted[i]], perigeeSurface, Trk::anyDirection, true, Trk::pion)
         .release();

     double currentTrackZ0 = lexPerigee->parameters()[Trk::z0];
     delete lexPerigee;

     if (std::fabs(currentTrackZ0 - lastTrackZ0) < m_sepDistance) {

       // the distance is below separation, adding to the same cluster
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
      if(core_cluster.empty())
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
  const EventContext& ctx = Gaudi::Hive::currentContext();
 // std::cout<<"Number of tracks received: "<<tracks.size()<<std::endl;
  
  //step 1: preselection 
  std::vector<const Trk::TrackParticleBase*> preselectedTracks(0);
  std::vector<const Trk::TrackParticleBase*>::const_iterator tr = tracks.begin();
  std::vector<const Trk::TrackParticleBase*>::const_iterator tre = tracks.end(); 
  
//selecting with respect to the beam spot
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
  Trk::RecVertex beamrecposition(beamSpotHandle->beamVtx());    
  for(;tr!=tre;++tr) if(m_trkFilter->decision(**tr, &beamrecposition)) preselectedTracks.push_back(*tr);
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Beam spot position is: "<< beamrecposition.position()<<endmsg;
  Trk::Vertex* beamposition=&beamrecposition;


  if (m_ignoreBeamSpot)
  {
    std::vector<const Trk::TrackParameters*> perigeeList;
    std::vector<const Trk::TrackParticleBase*>::const_iterator trackBegin=tracks.begin();
    std::vector<const Trk::TrackParticleBase*>::const_iterator trackEnd=tracks.end();
    for (std::vector<const Trk::TrackParticleBase*>::const_iterator trackIter=trackBegin;trackIter!=trackEnd;++trackIter)
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

  
//step 2: sorting in z0
//output container  
  std::vector< std::vector<const Trk::TrackParticleBase *> > result(0);
  if(!preselectedTracks.empty())
  {
   std::vector<int> indexOfSorted = m_sortingTool->sortedIndex(preselectedTracks, beamposition);

//step 3 preclustering
   std::vector< std::vector<const Trk::TrackParticleBase *> > preClusters(0);

//left-handed track position
   std::vector<const Trk::TrackParticleBase *> tmp_cluster(0); 
   
   Trk::PerigeeSurface perigeeSurface(beamposition->position());
   const Trk::TrackParameters* exPerigee = m_extrapolator
                                             ->extrapolate(ctx,
                                                           preselectedTracks[indexOfSorted[0]]->definingParameters(),
                                                           perigeeSurface,
                                                           Trk::anyDirection,
                                                           true,
                                                           Trk::pion).release();

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
     const Trk::TrackParameters* lexPerigee =
       m_extrapolator->extrapolate(ctx,
                                   preselectedTracks[indexOfSorted[i]]->definingParameters(),
                                   perigeeSurface,
                                   Trk::anyDirection,
                                   true,
                                   Trk::pion).release();

     double currentTrackZ0 = lexPerigee->parameters()[Trk::z0];
     delete lexPerigee;

     if (std::fabs(currentTrackZ0 - lastTrackZ0) < m_sepDistance) {

       // the distance is below separation, adding to the same cluster
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
      if(core_cluster.empty())
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
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // std::cout<<"Number of tracks received: "<<tracks.size()<<std::endl;
  
  //step 1: preselection 
  std::vector<const xAOD::TrackParticle*> preselectedTracks(0);
  
//selecting with respect to the beam spot


  xAOD::Vertex * beamposition = new xAOD::Vertex();
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey,ctx };
  beamposition->setPosition(beamSpotHandle->beamVtx().position());
  beamposition->setCovariancePosition(beamSpotHandle->beamVtx().covariancePosition());


   for (std::vector<const xAOD::TrackParticle*>::const_iterator itr  = tracks.begin(); itr != tracks.end(); ++itr) {
     if (m_trkFilter->decision(**itr,beamposition)) preselectedTracks.push_back(*itr);
   }
   

  
    std::vector<const Trk::TrackParameters*> perigeeList;
    std::vector<const xAOD::TrackParticle*>::const_iterator trackBegin=tracks.begin();
    std::vector<const xAOD::TrackParticle*>::const_iterator trackEnd=tracks.end();
    for (std::vector<const xAOD::TrackParticle*>::const_iterator trackIter=trackBegin;trackIter!=trackEnd;++trackIter)
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

  
//step 2: sorting in z0
//output container  
  std::vector< std::vector<const Trk::TrackParameters *> > result(0);
  if(!preselectedTracks.empty())
  {
  
    std::vector<int> indexOfSorted =  m_sortingTool->sortedIndex(preselectedTracks, beamposition);

   //need new sort method, either for xAODTrackParticles, or TrackParameters. Neither currently supported...

//step 3 preclustering
    std::vector< std::vector<const xAOD::TrackParticle *> > preClusters(0);

//left-handed track position
   std::vector<const xAOD::TrackParticle *> tmp_cluster(0); 
   
   Trk::PerigeeSurface perigeeSurface(beamposition->position());
   const Trk::TrackParameters* exPerigee =
     m_extrapolator
       ->extrapolate(ctx, *preselectedTracks[indexOfSorted[0]], perigeeSurface, Trk::anyDirection, true, Trk::pion)
       .release();

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
     const Trk::TrackParameters* lexPerigee = m_extrapolator->extrapolate(
       ctx, *preselectedTracks[indexOfSorted[i]], perigeeSurface, Trk::anyDirection, true, Trk::pion).release();

     double currentTrackZ0 = lexPerigee->parameters()[Trk::z0];
     delete lexPerigee;

     if (std::fabs(currentTrackZ0 - lastTrackZ0) < m_sepDistance) {

       // the distance is below separation, adding to the same cluster
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
      if(core_cluster.empty())
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
 
}//end of namespace definitions

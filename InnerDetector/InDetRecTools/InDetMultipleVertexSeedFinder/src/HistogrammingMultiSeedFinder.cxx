/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetMultipleVertexSeedFinder/HistogrammingMultiSeedFinder.h"
#include "TrkTrack/Track.h"
//#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "InDetMultipleVertexSeedFinderUtils/InDetTrackClusterCleaningTool.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "xAODTracking/Vertex.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace InDet
{
 StatusCode HistogrammingMultiSeedFinder::initialize()
 {
 
  if(m_trkFilter.retrieve().isFailure())
  {
   msg(MSG::ERROR)<<" Unable to retrieve "<<m_trkFilter<<endmsg;
   return StatusCode::FAILURE;
  }else msg(MSG::INFO)<<"Track summary tool retrieved"<<endmsg; 
   
  if(m_cleaningTool.retrieve().isFailure())
  {
   msg(MSG::ERROR)<<" Unable to retrieve "<<m_cleaningTool<<endmsg;
   return StatusCode::FAILURE;
  }else msg(MSG::INFO)<<"Cluster cleaning tool retrieved"<<endmsg;  
   
  if(m_beamService.retrieve().isFailure())
  {
   msg(MSG::ERROR)<<"Unable to retrieve "<<m_beamService<<endmsg;
   return StatusCode::FAILURE;
  }else msg(MSG::INFO)<<"BeamSpot service retrieved"<<endmsg;  
   
  if(m_vtxSeedFinder.retrieve().isFailure())
  {
    msg(MSG::ERROR) << "Unable to retrieve " << m_vtxSeedFinder <<endmsg;
    return StatusCode::FAILURE;
  }else msg(MSG::INFO)<< "Vertex seed finder retriever" << endmsg; 
 
  if ( m_extrapolator.retrieve().isFailure() ) 
  {                              
   msg(MSG::ERROR) << "Failed to retrieve tool " << m_extrapolator << endmsg;
   return StatusCode::FAILURE;                                                  
  } else msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endmsg;
                
  return StatusCode::SUCCESS;
 }//end of initialize mtehod
 
 StatusCode HistogrammingMultiSeedFinder::finalize()
 {
  msg(MSG::INFO)  << "Finalize successful" << endmsg;
  return StatusCode::SUCCESS;
 }

 HistogrammingMultiSeedFinder::HistogrammingMultiSeedFinder(const std::string& t, const std::string& n, const IInterface*p):
         AthAlgTool(t,n,p), 
         m_sepNBins(2),
         m_nBins(500),
         m_nRemaining(1), 
         m_histoRange(200.), 
         m_ignoreBeamSpot(false),
         m_vtxSeedFinder("Trk::CrossDistancesSeedFinder"),
         m_beamService("BeamCondSvc",n),
         m_extrapolator("Trk::Extrapolator") 
 {
  declareInterface<IMultiPVSeedFinder>(this); 
  declareProperty("maxSeparNumBins",     m_sepNBins);
  declareProperty("nBins",                  m_nBins);
  declareProperty("nRemainTracks",     m_nRemaining);
  declareProperty("HistoRange",        m_histoRange);
  declareProperty("IgnoreBeamSpot",	m_ignoreBeamSpot);


//track filter  
  declareProperty("TrackSelector", m_trkFilter );

//cleaning tool
  declareProperty("CleaningTool",  m_cleaningTool);

//beam spot service  
  declareProperty("BeamSpotSvc", m_beamService);

//vertex finder tool (needed when no beam spot is available)
  declareProperty("VertexSeedFinder",m_vtxSeedFinder);

//extrapolator
  declareProperty("Extrapolator",m_extrapolator); 
 }//end of constructor

 HistogrammingMultiSeedFinder::~HistogrammingMultiSeedFinder()
 {}

 std::vector< std::vector<const Trk::Track *> > HistogrammingMultiSeedFinder::seeds(const std::vector<const Trk::Track*>& tracks )const
 {
 
 //step 1: preselection 
  std::vector<const Trk::Track*> preselectedTracks(0);
  std::vector<const Trk::Track*>::const_iterator tr = tracks.begin();
  std::vector<const Trk::Track*>::const_iterator tre = tracks.end(); 
  
//beamposition
  Trk::RecVertex beamrecposition(m_beamService->beamVtx());  
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Beam spot position is: "<< beamrecposition.position()<<endmsg;
  for(;tr!=tre;++tr) if(m_trkFilter->decision(**tr,&beamrecposition)) preselectedTracks.push_back(*tr);
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

//step 2: histogramming tracks
//output container  
  std::vector< std::vector<const Trk::Track *> > result(0);
  if(preselectedTracks.size() !=0)
  { 
   std::map<unsigned int, std::vector<const Trk::Track *> > histo;  
  
//getting the bin size  
   double bin_size  = 2. * m_histoRange/ m_nBins;
  
   std::vector<const Trk::Track*>::const_iterator p_tr = preselectedTracks.begin();
   std::vector<const Trk::Track*>::const_iterator p_tre = preselectedTracks.end();
   
   
//creating a perigee surfce to extrapolate tracks
   Trk::PerigeeSurface perigeeSurface(beamposition->position());   
  
   for(;p_tr != p_tre; ++p_tr)
   {
    const  Trk::TrackParameters * lexPerigee = m_extrapolator->extrapolate(**p_tr,perigeeSurface,Trk::anyDirection,true, Trk::pion); 
 
    double currentTrackZ0 = lexPerigee->parameters()[Trk::z0];
    delete lexPerigee;
//    double currentTrackZ0 = (*p_tr)->perigeeParameters()->parameters()[Trk::z0];
    unsigned int bin_number = int(floor((currentTrackZ0 + m_histoRange)/bin_size)) +1;
 
//now checking whether this bin entry already exists and adding track, if not, creating one.    
    std::map<unsigned int, std::vector<const Trk::Track *> >::iterator map_pos = histo.find(bin_number);
    if( map_pos != histo.end())
    {
    
//this bin already exists, adding entry    
      map_pos->second.push_back(*p_tr);
    }else{
    
//this bin is not their yet, adding bin  
     std::vector<const Trk::Track *> tmp_vec(0);
     tmp_vec.push_back(*p_tr);
     histo.insert( std::map<unsigned int, std::vector<const Trk::Track *> >::value_type(bin_number, tmp_vec));
    }//end of checking bin existence     
   }//end of loop over all sorted tracks

//------------------------- Debug output -------------------------------------------------------------------
//debug output: checking the bin contents of the histogram 
/*  std::cout<<"**********Checking the histogram ************"<<std::endl;
  for(std::map<unsigned int, std::vector<const Trk::Track *> >::iterator i = histo.begin(); i != histo.end(); ++i)
  {
   std::cout<<"Currennt bin N "<< i->first<<std::endl;
   std::cout<<"Containes entries: "<< i->second.size()<<std::endl;  
  }//end of debug histogram check
*/  
//------------------------- End of debug output -------------------------------------------------------------------
   
//step 3: merging clusters    
//bins closer to each other than several empty bins become 
//parts of the same cluster  

  std::vector<std::vector<const Trk::Track *> > preClusters(0);
  std::vector<const Trk::Track *> tmp_cluster(0);
  unsigned int previous_bin = histo.begin()->first;
  for(std::map<unsigned int, std::vector<const Trk::Track *> >::iterator i = histo.begin(); i != histo.end(); ++i)
  {
   unsigned int current_bin = i->first;
   if((current_bin - previous_bin)>m_sepNBins )
   {
   
//forming a new cluster   
    preClusters.push_back(tmp_cluster);
    tmp_cluster.clear();
   } 
   
// in any case filling tracks into this bin.   
   for(std::vector<const Trk::Track *>::const_iterator j = i->second.begin();j != i->second.end();++j) tmp_cluster.push_back(*j); 
   previous_bin = current_bin;
  }//end of loop over the map entries
  preClusters.push_back(tmp_cluster);

//step 4 iterative cleaning of formed clusters  
  for(std::vector<std::vector<const Trk::Track *> >::const_iterator i = preClusters.begin(); i !=preClusters.end();++i)
  {
   if(i->size()>m_nRemaining)
   {
    std::vector<const Trk::Track *> tracks_to_clean = *i;  
    bool clean_again = false;
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
	
      }else if(core_outl.size()>1)
      {
       clean_again = false;
     
       msg(MSG::INFO)  << "There were remaining outliers of size: "<< core_outl.size()<< endmsg;
       msg(MSG::INFO)  << "Not evident, whether these tracks form a cluster. Rejected..."<< endmsg;	
      }else  clean_again = false;//end of outlier size check  
     }//end of core cluster 0 check     
    }while(clean_again);//end of loop
   }else if(i->size()==2){
    
//case of two track cluster. accepting without cleaning  
    result.push_back(*i);
   }//end of cluster size check
  }//end of loop over all the preclusters
 }//end of  preselection not zero check
  if (m_ignoreBeamSpot) delete beamposition;
  return result; 
 }//end of seed finding method
 
 std::vector< std::vector<const Trk::TrackParticleBase *> > HistogrammingMultiSeedFinder::seeds(const std::vector<const Trk::TrackParticleBase*>& tracks )const
 {
   
//step 1: preselection 
  std::vector<const Trk::TrackParticleBase*> preselectedTracks(0);
  std::vector<const Trk::TrackParticleBase*>::const_iterator tr = tracks.begin();
  std::vector<const Trk::TrackParticleBase*>::const_iterator tre = tracks.end();
  
//beamposition
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

  std::vector< std::vector<const Trk::TrackParticleBase *> > result(0);
  
  if(preselectedTracks.size() !=0)
  { 
   std::map<unsigned int, std::vector<const Trk::TrackParticleBase *> > histo;  
  
//getting the bin size  
   double bin_size  = 2.* m_histoRange/ m_nBins;
   
   if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Beam spot position is: "<< beamposition->position()<<endmsg;

//getting a perigee surface to be  used
   Trk::PerigeeSurface perigeeSurface(beamposition->position());

   std::vector<const Trk::TrackParticleBase*>::const_iterator p_tr = preselectedTracks.begin();
   std::vector<const Trk::TrackParticleBase*>::const_iterator p_tre = preselectedTracks.end();
   for(;p_tr != p_tre; ++p_tr)
   {

     const  Trk::TrackParameters * lexPerigee = m_extrapolator->extrapolate((*p_tr)->definingParameters(),
									    perigeeSurface,Trk::anyDirection,true, Trk::pion); 
    double currentTrackZ0 = lexPerigee->parameters()[Trk::z0];
    delete lexPerigee;

//    double currentTrackZ0 = (*p_tr)->perigee()->parameters()[Trk::z0];
    unsigned int bin_number = int(floor((currentTrackZ0 +m_histoRange)/bin_size)) +1;
 
//now checking whether this bin entry already exists and adding track, if not, creating one.    
    std::map<unsigned int, std::vector<const Trk::TrackParticleBase *> >::iterator map_pos = histo.find(bin_number);
    if( map_pos != histo.end())
    {
    
//this bin already exists, adding entry    
      map_pos->second.push_back(*p_tr);
    }else{
    
//this bin is not their yet, adding bin  
     std::vector<const Trk::TrackParticleBase *> tmp_vec(0);
     tmp_vec.push_back(*p_tr);
     histo.insert( std::map<unsigned int, std::vector<const Trk::TrackParticleBase *> >::value_type(bin_number, tmp_vec));
    }//end of checking bin existence         
   }//end of loop over all sorted track particles
   
//step 3: merging clusters    
//bins closer to each other than several empty bins become 
//parts of the same cluster  

   std::vector<std::vector<const Trk::TrackParticleBase *> > preClusters(0);
   std::vector<const Trk::TrackParticleBase *> tmp_cluster(0);
   unsigned int previous_bin = histo.begin()->first;
   for(std::map<unsigned int, std::vector<const Trk::TrackParticleBase *> >::iterator i = histo.begin(); i != histo.end(); ++i)
   {
    unsigned int current_bin = i->first;
    if((current_bin - previous_bin)>m_sepNBins )
    {
   
//forming a new cluster   
     preClusters.push_back(tmp_cluster);
     tmp_cluster.clear();
    }  
   
// in any case filling tracks into this bin.   
    for(std::vector<const Trk::TrackParticleBase *>::const_iterator j = i->second.begin();j != i->second.end();++j) tmp_cluster.push_back(*j); 
    previous_bin = current_bin;
   }//end of loop over the map entries
   preClusters.push_back(tmp_cluster);

  //step 4 iterative cleaning of formed clusters  
  for(std::vector<std::vector<const Trk::TrackParticleBase *> >::const_iterator i = preClusters.begin(); i !=preClusters.end();++i)
  {
   if(i->size()>m_nRemaining)
   {
    std::vector<const Trk::TrackParticleBase *> tracks_to_clean = *i;  
    bool clean_again = false;
    do
    {
     std::pair<std::vector<const Trk::TrackParticleBase *>, std::vector<const Trk::TrackParticleBase *> > clusterAndOutl = 
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
       MsgStream log(msgSvc(), name());
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
        MsgStream log(msgSvc(), name());
        msg(MSG::INFO)  << "There were remaining outliers of size: "<< core_outl.size()<< endmsg;
        msg(MSG::INFO)  << "Not evident, whether these tracks form a cluster. Rejected..."<< endmsg;	
       }else  clean_again = false;//end of outlier size check  
      }//end of core cluster 0 check     
     }while(clean_again);//end of loop
    }else if(i->size()==2){
    
//case of two track cluster. accepting without cleaning  
    result.push_back(*i);
    }//end of cluster size check
   }//end of loop over all the preclusters
  }//end of preselected trackparticles non zero check
  if (m_ignoreBeamSpot) delete beamposition;
  return result; 
 }//end of method working on the TrackParticleBase's
  

  std::vector< std::vector<const Trk::TrackParameters *> > HistogrammingMultiSeedFinder::seeds(const std::vector<const xAOD::TrackParticle*>& tracks )const
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
	//hmm, will this leak? I think so.
      }

 // std::cout<<"Number after pre-selection : "<<preselectedTracks.size()<<std::endl;
  
    //step 2: sorting in z0
    //output container  
    std::vector< std::vector<const Trk::TrackParameters *> > result(0);
    if(preselectedTracks.size() !=0)
      {  
	std::map<unsigned int, std::vector<const xAOD::TrackParticle *> > histo;  
	
	//getting the bin size  
	double bin_size  = 2.* m_histoRange/ m_nBins;
	
	if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Beam spot position is: "<< beamposition->position()<<endmsg;
	
	//getting a perigee surface to be  used
	Trk::PerigeeSurface perigeeSurface(beamposition->position());
	
	std::vector<const xAOD::TrackParticle*>::const_iterator p_tr = preselectedTracks.begin();
	std::vector<const xAOD::TrackParticle*>::const_iterator p_tre = preselectedTracks.end();
	for(;p_tr != p_tre; ++p_tr)
	  {
	    
	    const  Trk::TrackParameters * lexPerigee = m_extrapolator->extrapolate(**p_tr,
										   perigeeSurface,Trk::anyDirection,true, Trk::pion); 
	    double currentTrackZ0 = lexPerigee->parameters()[Trk::z0];
	    delete lexPerigee;
	    
	    //    double currentTrackZ0 = (*p_tr)->perigee()->parameters()[Trk::z0];
	    unsigned int bin_number = int(floor((currentTrackZ0 +m_histoRange)/bin_size)) +1;
	    
	    //now checking whether this bin entry already exists and adding track, if not, creating one.    
	    std::map<unsigned int, std::vector<const xAOD::TrackParticle *> >::iterator map_pos = histo.find(bin_number);
	    if( map_pos != histo.end())
	      {
		
		//this bin already exists, adding entry    
		map_pos->second.push_back(*p_tr);
	      }else{
	      
	      //this bin is not their yet, adding bin  
	      std::vector<const xAOD::TrackParticle *> tmp_vec(0);
	      tmp_vec.push_back(*p_tr);
	      histo.insert( std::map<unsigned int, std::vector<const xAOD::TrackParticle *> >::value_type(bin_number, tmp_vec));
	    }//end of checking bin existence         
	  }//end of loop over all sorted track particles
	
	//step 3: merging clusters    
	//bins closer to each other than several empty bins become 
	//parts of the same cluster  
	
	std::vector<std::vector<const xAOD::TrackParticle *> > preClusters(0);
	std::vector<const xAOD::TrackParticle *> tmp_cluster(0);
	unsigned int previous_bin = histo.begin()->first;
	for(std::map<unsigned int, std::vector<const xAOD::TrackParticle *> >::iterator i = histo.begin(); i != histo.end(); ++i)
	  {
	    unsigned int current_bin = i->first;
	    if((current_bin - previous_bin)>m_sepNBins )
	      {
	   
		//forming a new cluster   
		preClusters.push_back(tmp_cluster);
		tmp_cluster.clear();
	      }  
	    
	    // in any case filling tracks into this bin.   
	    for(std::vector<const xAOD::TrackParticle *>::const_iterator j = i->second.begin();j != i->second.end();++j) tmp_cluster.push_back(*j); 
	    previous_bin = current_bin;
	  }//end of loop over the map entries
	preClusters.push_back(tmp_cluster);
	
   
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
  std::pair<std::vector<const Trk::TrackParameters *>, 
	    std::vector<const xAOD::TrackParticle *> > HistogrammingMultiSeedFinder::m_clusterAndOutliers(std::vector<const xAOD::TrackParticle *> cluster, xAOD::Vertex * reference) const
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

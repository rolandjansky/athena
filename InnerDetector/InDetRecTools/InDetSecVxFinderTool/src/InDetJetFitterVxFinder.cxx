/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetJetFitterVxFinder.cxx  -  Description
                             -------------------

    begin : March 2007
    authors: Giacinto Piacquadio (University of Freiburg),
             Christian Weiser (University of Freiburg)
    email : nicola.giacinto.piacquadio@cern.ch,
            christian.weiser@cern.ch
    changes: new!
 
  2007 (c) Atlas Detector Software

  Look at the header file for more information.
     
 ***************************************************************************/

#include "InDetSecVxFinderTool/InDetJetFitterVxFinder.h"
#include "VxJetVertex/VxJetCandidate.h"
#include "VxJetVertex/PairOfVxVertexOnJetAxis.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxJetVertex/VxClusteringTable.h"
#include "TrkJetVxFitter/JetFitterHelper.h"
#include "TrkJetVxFitter/JetFitterInitializationHelper.h"
#include "TrkJetVxFitter/JetFitterRoutines.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include <TMath.h>
#include "TrkEventPrimitives/FitQuality.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"


namespace InDet
{

  struct TrackParticle_pair
  {
    double first;
    const Trk::TrackParticleBase* second;
    TrackParticle_pair(double p1, const Trk::TrackParticleBase* p2)
      : first (p1), second (p2) {}
    bool operator< (const TrackParticle_pair& other) const
    { return first > other.first; }
  };

  struct Track_pair
  {
    double first;
    const Trk::Track* second;
    Track_pair(double p1, const Trk::Track* p2)
      : first (p1), second (p2) {}
    bool operator< (const Track_pair& other) const
    { return first > other.first; }
  };  

  InDetJetFitterVxFinder::InDetJetFitterVxFinder(const std::string& t, const std::string& n, const IInterface*  p) :
          AthAlgTool(t,n,p)
  { 
    declareInterface< ISecVertexInJetFinder >(this) ;
  }
  

  InDetJetFitterVxFinder::~InDetJetFitterVxFinder() {}
  

  StatusCode InDetJetFitterVxFinder::initialize() {
    
    
    StatusCode sc = AthAlgTool::initialize();
    if(sc.isFailure())
      {
	msg(MSG::ERROR) << " Unable to initialize the AlgTool" << endmsg;
	return sc;
      }
    
    //retrieving the udator itself 	 
    sc =  m_helper.retrieve();
    if(sc.isFailure()) 	  { 	 
      msg(MSG::ERROR) << " Unable to retrieve "<<m_helper<<endmsg; 	 
      return StatusCode::FAILURE; 	 
    }else msg(MSG::INFO) << "JetFitter Helper retrieved"<<endmsg; 
    
    
    sc = m_initializationHelper.retrieve();
    if(sc.isFailure()) 	  { 	 
      msg(MSG::ERROR) << " Unable to retrieve "<<m_initializationHelper<<endmsg; 	 
      return StatusCode::FAILURE; 	 
    }else msg(MSG::INFO) << "JetFitter Initialization Helper retrieved"<<endmsg; 
    
    sc = m_routines.retrieve();
    if(sc.isFailure()) 	  { 	 
      msg(MSG::ERROR) << " Unable to retrieve the JetFitter routines"<<m_routines<<endmsg; 	 
      return StatusCode::FAILURE; 	 
    }else msg(MSG::INFO) << "JetFitter Routines class retrieved"<<endmsg; 

    if(m_trkFilter.retrieve().isFailure()) {
      msg(MSG::ERROR) << " Unable to retrieve "<<m_trkFilter<<endmsg;
      return StatusCode::FAILURE;
    } else msg(MSG::INFO) << "Track filter retrieved"<<endmsg; 
    
    msg(MSG::INFO) << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }
  

   StatusCode InDetJetFitterVxFinder::finalize() {

    msg(MSG::INFO) <<  "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;

  } 

  const Trk::VxSecVertexInfo* InDetJetFitterVxFinder::findSecVertex(const Trk::RecVertex & primaryVertex,
							      const TLorentzVector & jetMomentum,
							      const std::vector<const Trk::TrackParticleBase*> & myTracks) const {
    Amg::Vector3D myDirection(jetMomentum.Vect().X(),jetMomentum.Vect().Y(),jetMomentum.Vect().Z());

    std::vector<TrackParticle_pair> tracks;

    std::vector<const Trk::TrackParticleBase*>::const_iterator tracksBegin=myTracks.begin();
    std::vector<const Trk::TrackParticleBase*>::const_iterator tracksEnd=myTracks.end();
    for (std::vector<const Trk::TrackParticleBase*>::const_iterator tracksIter=tracksBegin;
	 tracksIter!=tracksEnd;++tracksIter) {
      if (m_trkFilter->decision(**tracksIter,&primaryVertex)==true) {
	tracks.push_back(TrackParticle_pair((*tracksIter)->perigee()->momentum().perp(),*tracksIter));
      }
    }

    std::vector<std::vector<const Trk::TrackParticleBase*> > bunchesOfTracks;
    
    std::sort(tracks.begin(),tracks.end());

    std::vector<const Trk::TrackParticleBase*> tracksToAdd;
    
    std::vector<TrackParticle_pair>::const_iterator tracks2Begin=tracks.begin();
    std::vector<TrackParticle_pair>::const_iterator tracks2End=tracks.end();
    for (std::vector<TrackParticle_pair>::const_iterator tracks2Iter=tracks2Begin;
	 tracks2Iter!=tracks2End;++tracks2Iter) {
      	if (msgLvl(MSG::VERBOSE)) msg() << " track: " << (*tracks2Iter).first << " and : " <<  (*tracks2Iter).second << endmsg;
      
      tracksToAdd.push_back((*tracks2Iter).second);
      if (tracksToAdd.size() % m_maxTracksToFitAtOnce == 0) {
        if (msgLvl(MSG::VERBOSE)) msg() << " new bunch " << endmsg;
	bunchesOfTracks.push_back(tracksToAdd);
	tracksToAdd.clear();
      }
    }

    bunchesOfTracks.push_back(tracksToAdd);

    std::vector<std::vector<const Trk::TrackParticleBase*> >::const_iterator BunchesBegin=bunchesOfTracks.begin();
    std::vector<std::vector<const Trk::TrackParticleBase*> >::const_iterator BunchesEnd=bunchesOfTracks.end();

    std::vector<const Trk::TrackParticleBase*>::const_iterator tracksToAddBegin;
    std::vector<const Trk::TrackParticleBase*>::const_iterator tracksToAddEnd;
    std::vector<const Trk::TrackParticleBase*>::const_iterator tracksToAddIter;


    Trk::VxJetCandidate* myJetCandidate=0;

    for (std::vector<std::vector<const Trk::TrackParticleBase*> >::const_iterator BunchesIter=BunchesBegin;
	 BunchesIter!=BunchesEnd;++BunchesIter) {
      
      if (BunchesIter==BunchesBegin) {
        if (msgLvl(MSG::VERBOSE)) msg() <<  " initial fit with  " << (*BunchesIter).size() << " tracks " << endmsg;
	myJetCandidate=m_initializationHelper->initializeJetCandidate(*BunchesIter,&primaryVertex,&myDirection);
	m_routines->initializeToMinDistancesToJetAxis(myJetCandidate);
	doTheFit(myJetCandidate);
      } else {
        if (msgLvl(MSG::VERBOSE)) msg() << " other fit with " << (*BunchesIter).size() << " tracks " << endmsg;
	std::vector<Trk::VxVertexOnJetAxis*> setOfVertices=myJetCandidate->getVerticesOnJetAxis();
	std::vector<Trk::VxTrackAtVertex*>* setOfTracks=myJetCandidate->vxTrackAtVertex();
	tracksToAddBegin=(*BunchesIter).begin();
	tracksToAddEnd=(*BunchesIter).end();
	for (tracksToAddIter=tracksToAddBegin;tracksToAddIter!=tracksToAddEnd;++tracksToAddIter) {
	  std::vector<Trk::VxTrackAtVertex*> temp_vector_tracksAtVertex;
	  ElementLink<Trk::TrackParticleBaseCollection> link;
	  link.setElement(const_cast<Trk::TrackParticleBase*>(*tracksToAddIter));
	  Trk::LinkToTrackParticleBase * linkTT = new Trk::LinkToTrackParticleBase(link);
	  Trk::VxTrackAtVertex* newVxTrack=new Trk::VxTrackAtVertex(linkTT);
	  temp_vector_tracksAtVertex.push_back(newVxTrack);
	  setOfTracks->push_back(newVxTrack);
	  setOfVertices.push_back(new Trk::VxVertexOnJetAxis(temp_vector_tracksAtVertex));
	}
        if (msgLvl(MSG::VERBOSE)) msg() << " new overall number of tracks to fit : " << setOfVertices.size() << endmsg;
	myJetCandidate->setVerticesOnJetAxis(setOfVertices);
	m_initializationHelper->updateTrackNumbering(myJetCandidate);
	doTheFit(myJetCandidate);
      }
    }
    
    std::vector<Trk::VxCandidate*> myCandidates;
    myCandidates.push_back(myJetCandidate);
    
//    return new Trk::VxSecVertexInfo(myCandidates);//ownership of the single objects is taken over!
    return 0;
    
  }

  const Trk::VxSecVertexInfo* InDetJetFitterVxFinder::findSecVertex(const Trk::RecVertex & primaryVertex,
                                                                    const TLorentzVector & jetMomentum,
                                                                    const std::vector<const Trk::TrackParticleBase*> & firstInputTracks,
                                                                    const std::vector<const Trk::TrackParticleBase*> & secondInputTracks,
                                                                    const Amg::Vector3D & vtxSeedDirection) const 
  {

    Amg::Vector3D myDirection(jetMomentum.Vect().X(),jetMomentum.Vect().Y(),jetMomentum.Vect().Z());

    std::vector<std::vector<const Trk::TrackParticleBase*> > bunchesOfTracks;
    
    std::vector<const Trk::TrackParticleBase*> tracksToAdd;
    
    std::vector<const Trk::TrackParticleBase*>::const_iterator tracks2Begin=firstInputTracks.begin();
    std::vector<const Trk::TrackParticleBase*>::const_iterator tracks2End=firstInputTracks.end();
    for (std::vector<const Trk::TrackParticleBase*>::const_iterator tracks2Iter=tracks2Begin;
	 tracks2Iter!=tracks2End;++tracks2Iter) {
      if (msgLvl(MSG::VERBOSE)) msg() << " adding track to fit " << endmsg;
      tracksToAdd.push_back(*tracks2Iter);
    }
    
    bunchesOfTracks.push_back(tracksToAdd);
    tracksToAdd.clear();
  
    std::vector<const Trk::TrackParticleBase*>::const_iterator tracks3Begin=secondInputTracks.begin();
    std::vector<const Trk::TrackParticleBase*>::const_iterator tracks3End=secondInputTracks.end();
    for (std::vector<const Trk::TrackParticleBase*>::const_iterator tracks3Iter=tracks3Begin;
	 tracks3Iter!=tracks3End;++tracks3Iter) {
      if (msgLvl(MSG::VERBOSE)) msg() << " adding track to fit " << endmsg;
      tracksToAdd.push_back(*tracks3Iter);
    }

    if (tracksToAdd.size()!=0) 
    {
      bunchesOfTracks.push_back(tracksToAdd);
    }
    tracksToAdd.clear();
    

    //now it just uses these bunches...
    //now I have just to make sure that no clustering is done at first iteration
    //while it needs to be done at second iteration (there will be only two iterations)


    std::vector<std::vector<const Trk::TrackParticleBase*> >::const_iterator BunchesBegin=bunchesOfTracks.begin();
    std::vector<std::vector<const Trk::TrackParticleBase*> >::const_iterator BunchesEnd=bunchesOfTracks.end();

    std::vector<const Trk::TrackParticleBase*>::const_iterator tracksToAddBegin;
    std::vector<const Trk::TrackParticleBase*>::const_iterator tracksToAddEnd;
    std::vector<const Trk::TrackParticleBase*>::const_iterator tracksToAddIter;


    Trk::VxJetCandidate* myJetCandidate=0;

    for (std::vector<std::vector<const Trk::TrackParticleBase*> >::const_iterator BunchesIter=BunchesBegin;
	 BunchesIter!=BunchesEnd;++BunchesIter) {
      
      if (BunchesIter==BunchesBegin) {
        if (msgLvl(MSG::VERBOSE)) msg() << " initial fit with  " << (*BunchesIter).size() << " tracks " << endmsg;
	myJetCandidate=m_initializationHelper->initializeJetCandidate(*BunchesIter,&primaryVertex,&myDirection,&vtxSeedDirection);
	m_routines->initializeToMinDistancesToJetAxis(myJetCandidate);
        if ((*BunchesIter).size()>0) 
        {
          doTheFit(myJetCandidate,true);
        }
      } else {
        if (msgLvl(MSG::VERBOSE)) msg() <<  " other fit with " << (*BunchesIter).size() << " tracks " << endmsg;
	std::vector<Trk::VxVertexOnJetAxis*> setOfVertices=myJetCandidate->getVerticesOnJetAxis();
	std::vector<Trk::VxTrackAtVertex*>* setOfTracks=myJetCandidate->vxTrackAtVertex();
	tracksToAddBegin=(*BunchesIter).begin();
	tracksToAddEnd=(*BunchesIter).end();
	for (tracksToAddIter=tracksToAddBegin;tracksToAddIter!=tracksToAddEnd;++tracksToAddIter) {
	  std::vector<Trk::VxTrackAtVertex*> temp_vector_tracksAtVertex;
	  ElementLink<Trk::TrackParticleBaseCollection> link;
	  link.setElement(const_cast<Trk::TrackParticleBase*>(*tracksToAddIter));
	  Trk::LinkToTrackParticleBase * linkTT = new Trk::LinkToTrackParticleBase(link);
	  Trk::VxTrackAtVertex* newVxTrack=new Trk::VxTrackAtVertex(linkTT);
	  temp_vector_tracksAtVertex.push_back(newVxTrack);
	  setOfTracks->push_back(newVxTrack);
	  setOfVertices.push_back(new Trk::VxVertexOnJetAxis(temp_vector_tracksAtVertex));
	}
        if (msgLvl(MSG::VERBOSE)) msg() << " new overall number of tracks to fit : " << setOfVertices.size() << endmsg;
	myJetCandidate->setVerticesOnJetAxis(setOfVertices);
	m_initializationHelper->updateTrackNumbering(myJetCandidate);
	m_routines->initializeToMinDistancesToJetAxis(myJetCandidate);
	doTheFit(myJetCandidate);
      }
    }
    
    std::vector<Trk::VxCandidate*> myCandidates;
    myCandidates.push_back(myJetCandidate);
    
//    return new Trk::VxSecVertexInfo(myCandidates);//ownership of the single objects is taken over!
    return 0;
    
  }

  void InDetJetFitterVxFinder::doTheFit(Trk::VxJetCandidate* myJetCandidate,
                                        bool performClustering) const {

    
    int numClusteringLoops=0;
    bool noMoreVerticesToCluster(false);

    do {//reguards clustering

      if (msgLvl(MSG::VERBOSE)) msg() << "InDetJetFitterVxFinder:      ------>>>>         new cycle of fit" << endmsg;

      int numLoops=0;
      bool noMoreTracksToDelete(false);
      do {//reguards eliminating incompatible tracks...
	
	m_routines->performTheFit(myJetCandidate,10,false,30,0.001);
	
	const std::vector<Trk::VxVertexOnJetAxis*> & vertices=myJetCandidate->getVerticesOnJetAxis();
	
	std::vector<Trk::VxVertexOnJetAxis*>::const_iterator verticesBegin=vertices.begin();
	std::vector<Trk::VxVertexOnJetAxis*>::const_iterator verticesEnd=vertices.end();
	
	
	//delete incompatible tracks...
	float max_prob(1.);
	Trk::VxVertexOnJetAxis* worseVertex(0);
	for (std::vector<Trk::VxVertexOnJetAxis*>::const_iterator verticesIter=verticesBegin;
	     verticesIter!=verticesEnd;++verticesIter) {
	  if (*verticesIter==0) {
	    if (msgLvl(MSG::WARNING)) msg() <<  "One vertex is empy. Problem when trying to delete incompatible vertices. No further vertices deleted." << endmsg;
	  } else {
	    const Trk::FitQuality & fitQuality=(*verticesIter)->fitQuality();
	    if (TMath::Prob(fitQuality.chiSquared(),(int)std::floor(fitQuality.numberDoF()+0.5))<max_prob) {
	      max_prob=TMath::Prob(fitQuality.chiSquared(),(int)std::floor(fitQuality.numberDoF()+0.5));
	      worseVertex=*verticesIter;
	    }
	  }
	}
	if (max_prob<m_vertexProbCut) {
	  if (msgLvl(MSG::DEBUG)) msg() << "Deleted vertex " << worseVertex->getNumVertex() << " with probability " << max_prob << endmsg;
	  //	  std::cout << "Deleted vertex " << worseVertex->getNumVertex() << " with probability " << max_prob << std::endl;
	  if (worseVertex==myJetCandidate->getPrimaryVertex()) {
            if (msgLvl(MSG::INFO)) msg() << " The most incompatible vertex is the primary vertex. Please check..." << endmsg;
	  }

	  m_routines->deleteVertexFromJetCandidate(worseVertex,myJetCandidate);

	} else {
	  noMoreTracksToDelete=true;
	  if (msgLvl(MSG::VERBOSE)) msg() << "No tracks to delete: maximum probability is " << max_prob << endmsg;
	}
	
	numLoops+=1;
      } while (numLoops<m_maxNumDeleteIterations&&!(noMoreTracksToDelete));
      
      if (!performClustering) break;

      if (!m_useFastClustering) {
	m_routines->fillTableWithFullProbOfMerging(myJetCandidate,5,false,10,0.01);
      } else {
	m_routines->fillTableWithFastProbOfMerging(myJetCandidate);
      }
      const Trk::VxClusteringTable* clusteringTablePtr(myJetCandidate->getClusteringTable());


      

      if (clusteringTablePtr==0) {
	if (msgLvl(MSG::WARNING)) msg() <<  " No Clustering Table while it should have been calculated... no more clustering performed during vertexing " << endmsg;
	noMoreVerticesToCluster=true;
      } else {

	if (msgLvl(MSG::VERBOSE)) msg() << " clustering table is " << *clusteringTablePtr << endmsg;

	//now iterate over the full map and decide wether you want to do the clustering OR not...
	float probVertex(0.);
	Trk::PairOfVxVertexOnJetAxis pairOfVxVertexOnJetAxis=clusteringTablePtr->getMostCompatibleVertices(probVertex);
	//a PairOfVxVertexOnJetAxis is a std::pair<VxVertexOnJetAxis*,VxVertexOnJetAxis*>
	
	if (probVertex>0.&&probVertex>m_vertexClusteringProbabilityCut) {
	  if (msgLvl(MSG::VERBOSE)) msg() << " merging vtx number " << (*pairOfVxVertexOnJetAxis.first).getNumVertex() << 
	    " and " << (*pairOfVxVertexOnJetAxis.second).getNumVertex() << endmsg;
	  //	  const Trk::VxVertexOnJetAxis & mergedVertex=
	  m_helper->mergeVerticesInJetCandidate(*pairOfVxVertexOnJetAxis.first,
						*pairOfVxVertexOnJetAxis.second,
						*myJetCandidate);
	  //now you need to update the numbering scheme
	  m_initializationHelper->updateTrackNumbering(myJetCandidate);//maybe this should be moved to a lower level...

	} else {
	  noMoreVerticesToCluster=true;
	}
      }
      numClusteringLoops+=1;
    } while (numClusteringLoops<m_maxClusteringIterations&&!(noMoreVerticesToCluster));
    
    //now a section should follow where the "complicate" VxJetCandidate is transformed in a conventional "VxCandidate" 
    //so that it can be used also by the normal B-Tagging algorithms...
    //TO BE COMPLETED
    
    //return myJetCandidate;

  }


}//end namespace Rec

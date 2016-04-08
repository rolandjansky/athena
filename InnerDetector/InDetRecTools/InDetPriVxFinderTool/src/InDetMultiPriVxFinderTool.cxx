/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetMultiPriVxFinderTool.cxx  -  Description
                             -------------------
    begin   : 01-11-2013
    authors : Matthew Rudolph, Lars Egholm Peterson

 ***************************************************************************/
#include "InDetPriVxFinderTool/InDetMultiPriVxFinderTool.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "VxVertex/VxCandidate.h"
#include <map>
#include <vector>
#include "TrkSurfaces/PlaneSurface.h"

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkVertexFitterInterfaces/IImpactPoint3dEstimator.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "DataModel/DataVector.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"

#include "TrkTrackLink/ITrackLink.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"



#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"


namespace InDet
{

InDetMultiPriVxFinderTool::InDetMultiPriVxFinderTool(const std::string& t, const std::string& n, const IInterface*  p)
        : AthAlgTool(t,n,p),
          m_iVertexFitter("Trk::AdaptiveVertexFitter"),
	  m_trkFilter("InDet::InDetTrackSelection"),
          m_SeedFinder("Trk::ImagingSeedFinder"),
          m_ImpactPoint3dEstimator("Trk::ImpactPoint3dEstimator"),
	  m_VertexEdmFactory("Trk::VertexInternalEdmFactory"),
          m_iBeamCondSvc("BeamCondSvc",n),
          m_useBeamConstraint(false),
          m_significanceCutSeeding(10),
          m_maximumChi2cutForSeeding(6.*6.),
          m_maxVertices(25),
          m_createSplitVertices(false),
	  m_splitVerticesTrkInvFraction(2),
          m_reassignTracksAfterFirstFit(false)
{
    declareInterface<IVertexFinder>(this);

    declareProperty("VertexFitterTool", m_iVertexFitter);
    declareProperty("TrackSelector",m_trkFilter);
    declareProperty("BeamPositionSvc", m_iBeamCondSvc);
    declareProperty("SeedFinder", m_SeedFinder);
    declareProperty("ImpactPoint3dEstimator",m_ImpactPoint3dEstimator);
    declareProperty("InternalEdmFactory", m_VertexEdmFactory);

    declareProperty("useBeamConstraint",m_useBeamConstraint);
    declareProperty("significanceCutSeeding",m_significanceCutSeeding);
    declareProperty("maximumChi2cutForSeeding",m_maximumChi2cutForSeeding);
    declareProperty("maxVertices",m_maxVertices);
    declareProperty("createSplitVertices",m_createSplitVertices);
    declareProperty("splitVerticesTrkInvFraction", m_splitVerticesTrkInvFraction, "inverse fraction to split tracks (1:N)");
    declareProperty("reassignTracksAfterFirstFit",m_reassignTracksAfterFirstFit);
}

InDetMultiPriVxFinderTool::~InDetMultiPriVxFinderTool()
{}

StatusCode InDetMultiPriVxFinderTool::initialize()
{
    StatusCode sc;


    if (m_createSplitVertices==true && m_useBeamConstraint==true)
    {
      msg(MSG::FATAL) << " Split vertices cannot be obtained if beam spot constraint is true! Change settings..." << endreq;
      return StatusCode::FAILURE;
    }
    
    /* Get the right vertex fitting tool from ToolSvc */
    if ( m_iVertexFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVertexFitter << endreq;
      return StatusCode::FAILURE;
    } 

    if ( m_SeedFinder.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_SeedFinder << endreq;
      return StatusCode::FAILURE;
    }

    if ( m_ImpactPoint3dEstimator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_ImpactPoint3dEstimator << endreq;
      return StatusCode::FAILURE;
    }


    if ( m_VertexEdmFactory.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrievel tool " << m_VertexEdmFactory);
      return StatusCode::FAILURE;
    }

    sc = m_iBeamCondSvc.retrieve();
    if (sc.isFailure())
      {
        msg(MSG::ERROR) << "Could not find BeamCondSvc." << endreq;
        return sc;
      }

    if(m_trkFilter.retrieve().isFailure()) {
      msg(MSG::ERROR) <<" Unable to retrieve "<<m_trkFilter<<endreq;
      return StatusCode::FAILURE;
    }
    

    // since some parameters special to an inherited class this method
    // will be overloaded by the inherited class
    m_printParameterSettings();

    msg(MSG::INFO) << "Initialization successful" << endreq;
    return StatusCode::SUCCESS;
}
  
  
  //Find vertex from TrackCollection
VxContainer* InDetMultiPriVxFinderTool::findVertex(const TrackCollection* trackTES) 
{

  if(msgLvl(MSG::DEBUG)) msg() << " Number of input tracks before track selection: " << trackTES->size() << endreq;


  //keep track of the links that pass selection
  std::vector<Trk::ITrackLink*> selectedTracks;

  typedef DataVector<Trk::Track>::const_iterator TrackDataVecIter;

  Root::TAccept selectionPassed;
  for (TrackDataVecIter itr  = (*trackTES).begin(); itr != (*trackTES).end(); itr++) {
    if (m_useBeamConstraint) {
      Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx());
      selectionPassed=m_trkFilter->accept(**itr,&beamposition);
    }
    else
    {
      Trk::Vertex null(Amg::Vector3D(0,0,0));
      selectionPassed=m_trkFilter->accept(**itr,&null);
    }
    if (selectionPassed)
    {
      ElementLink<TrackCollection> link;
      link.setElement(const_cast<Trk::Track*>(*itr));
      Trk::LinkToTrack * linkTT = new Trk::LinkToTrack(link);
      linkTT->setStorableObject(*trackTES);
      selectedTracks.push_back(linkTT);
    }
  }

  if(msgLvl(MSG::DEBUG)) msg() << "Of " << trackTES->size() << " tracks "
      << selectedTracks.size() << " survived the preselection." << endreq;

  VxContainer* returnContainer=findVertex(selectedTracks);
  
  return returnContainer;

}

VxContainer* InDetMultiPriVxFinderTool::findVertex(const Trk::TrackParticleBaseCollection* trackTES) {
 
  if(msgLvl(MSG::DEBUG)) msg() << " Number of input tracks before track selection: " << trackTES->size() << endreq;

  std::vector<Trk::ITrackLink*> selectedTracks;

  typedef DataVector<Trk::TrackParticleBase>::const_iterator TrackParticleDataVecIter;

  Root::TAccept selectionPassed;
  for (TrackParticleDataVecIter itr  = (*trackTES).begin(); itr != (*trackTES).end(); itr++) {
    if (m_useBeamConstraint) {
      Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx());
      selectionPassed=m_trkFilter->accept(*((*itr)->originalTrack()),&beamposition);
    }
    else
    {
      Trk::Vertex null(Amg::Vector3D(0,0,0));
      selectionPassed=m_trkFilter->accept(*((*itr)->originalTrack()),&null);
    }
    
    if (selectionPassed)
    {
      ElementLink<Trk::TrackParticleBaseCollection> link;
      link.setElement(const_cast<Trk::TrackParticleBase*>(*itr));
      Trk::LinkToTrackParticleBase * linkTT = new Trk::LinkToTrackParticleBase(link);
      linkTT->setStorableObject(*trackTES);
      selectedTracks.push_back(linkTT);
    }
  }

  if(msgLvl(MSG::DEBUG)) msg() << "Of " << trackTES->size() << " tracks "
      << selectedTracks.size() << " survived the preselection." << endreq;

  VxContainer* returnContainer=findVertex(selectedTracks);


  return returnContainer;
  
}

  //xAOD version -- for now uses same track link method as others, then converts at the end
std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetMultiPriVxFinderTool::findVertex(const xAOD::TrackParticleContainer* trackParticles)
{

  ATH_MSG_DEBUG(" Number of input tracks before track selection: " << trackParticles->size());

  std::vector<Trk::ITrackLink*> selectedTracks;



  typedef DataVector<xAOD::TrackParticle>::const_iterator TrackParticleDataVecIter;

  Root::TAccept selectionPassed;
  for (TrackParticleDataVecIter itr  = trackParticles->begin(); itr != trackParticles->end(); ++itr) {
    if (m_useBeamConstraint) {
      xAOD::Vertex beamposition;
      beamposition.makePrivateStore();
      beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
      beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());
      selectionPassed=m_trkFilter->accept(**itr,&beamposition);
    }
    else
    {
      xAOD::Vertex null;
      null.makePrivateStore();
      null.setPosition(Amg::Vector3D(0,0,0));
      AmgSymMatrix(3) vertexError;
      vertexError.setZero();
      null.setCovariancePosition(vertexError);
      selectionPassed=m_trkFilter->accept(**itr,&null);
    }
    
    if (selectionPassed)
    {
      ElementLink<xAOD::TrackParticleContainer> link;
      link.setElement(const_cast<xAOD::TrackParticle*>(*itr));
      Trk::LinkToXAODTrackParticle * linkTT = new Trk::LinkToXAODTrackParticle(link);
      linkTT->setStorableObject(*trackParticles);
      selectedTracks.push_back(linkTT);
    }
  }

  ATH_MSG_DEBUG("Of " << trackParticles->size() << " tracks " << selectedTracks.size() << " survived the preselection.");

  //Find vertex using track links, returns VxContainer
  VxContainer* returnContainer=findVertex(selectedTracks);

  //now convert VxContainer to XAOD Vertex
  xAOD::VertexContainer *xAODContainer(0);
  xAOD::VertexAuxContainer *xAODAuxContainer(0);
  if (m_VertexEdmFactory->createXAODVertexContainer(*returnContainer, xAODContainer, xAODAuxContainer) != StatusCode::SUCCESS) {
    ATH_MSG_WARNING("Cannot convert output vertex container to xAOD. Returning null pointer.");
  }
  delete returnContainer;

  return std::make_pair(xAODContainer, xAODAuxContainer);
  
}


VxContainer* InDetMultiPriVxFinderTool::findVertex(const std::vector<Trk::ITrackLink*> & trackVector) const
{
  
  //copy the input vector.  will drop used tracks from here as they are added to vertices
  // then what is left is used to then delete all unused ITrackLink objects at the end.
  std::vector<Trk::ITrackLink*> seedTracks=trackVector;
  
  //actual container that will get the vertices and be returned
  VxContainer* theVxContainer = new VxContainer;
  
  //prepare iterators for tracks
  std::vector<Trk::ITrackLink*>::iterator seedBegin = seedTracks.begin();
  std::vector<Trk::ITrackLink*>::iterator seedEnd = seedTracks.end();

  //create parameters objects to pass to seed finder
  std::vector<const Trk::TrackParameters*> perigeeList;
  for (std::vector<Trk::ITrackLink*>::iterator seedtrkAtVtxIter=seedBegin;
       seedtrkAtVtxIter!=seedEnd;++seedtrkAtVtxIter) {
    perigeeList.push_back((*seedtrkAtVtxIter)->parameters());
  }

  //used to store seed info
  std::vector<Trk::Vertex> seedVertices;

  ATH_MSG_VERBOSE("Find multi seeds for fitting");

  //find the list of seeds    
  Trk::RecVertex theconstraint;
  if (m_useBeamConstraint) {
    theconstraint=m_iBeamCondSvc->beamVtx();
    seedVertices=m_SeedFinder->findMultiSeeds(perigeeList,&theconstraint);
  } else {
    seedVertices = m_SeedFinder->findMultiSeeds(perigeeList);
  }

  ATH_MSG_VERBOSE("Assign tracks to " << seedVertices.size() << " seeds");


   //create track link collections for each seed to then be fit (not going straight to params to allow keeping track of links without searching later)
  std::vector< std::vector<Trk::ITrackLink*> > tracksToFitCollection;
  tracksToFitCollection.resize(seedVertices.size());

  //loop over tracks to assign to seed -- don't automatically iterate because may be deleting them from the vector
  for(std::vector<Trk::ITrackLink*>::iterator trkit=seedTracks.begin(); trkit!=seedTracks.end();) {
    //get its parameters
    const Trk::TrackParameters * currParams = (*trkit)->parameters();
    double mindistance=100.;
    std::size_t closestSeedIndex = seedVertices.size(); //so if we dont find one we can check later
    //    std::vector<Trk::Vertex>::iterator closestSeed = seedVertices.end();
    for(std::size_t seedit=0;seedit<seedVertices.size();seedit++) {
      double error;
      double distance = distanceAndError( currParams, &seedVertices[seedit], error);
      ATH_MSG_VERBOSE("dist = " << distance << ", err = " << error );
      if(distance/error<m_significanceCutSeeding && distance < mindistance) {
	mindistance = distance;
	closestSeedIndex = seedit;
      }
    } //end loop over seeds to find closest one

    if( closestSeedIndex != seedVertices.size() ) {
      //add to list of tracks in fit collections, drop from seed tracks
      tracksToFitCollection[closestSeedIndex].push_back( *trkit );
      trkit = seedTracks.erase(trkit);
    } else {
      trkit++; //skip it (will be left in seedTracks for later deletion
    }

  }

  ATH_MSG_VERBOSE("Reassign tracks attached to seeds with 1 track");

  //first look to reassign single tracks
  // can repeat to attempt to avoid any singletons ?
  //-->disabled for now
  unsigned int nsingle;
  do {
    nsingle = 0;
    
    for(std::size_t i = 0; i<tracksToFitCollection.size(); ++i) {
      if( tracksToFitCollection[i].size() == 1 ) {
        nsingle++;
        //get its parameters
        const Trk::TrackParameters * currParams = tracksToFitCollection[i][0]->parameters();
        //look for another seed to attach to
        double mindistance=100.;
        std::size_t closestSeedIndex = seedVertices.size();
        for(std::size_t j = 0; j<seedVertices.size(); j++) {
          //skip the one its already assigned to
          if( j == i )
            continue;

          double error;
          double distance = distanceAndError( currParams, &seedVertices[j], error);
          if(distance/error<m_significanceCutSeeding && distance < mindistance) {
            mindistance = distance;
            closestSeedIndex = j;
          }

        }

        //add to list of tracks for new vertex if the new vertex already has a track.
        //then erase from this collection in case another track gets moved here we don't want to have the same track in 2 places
        if( closestSeedIndex != seedVertices.size() && tracksToFitCollection[closestSeedIndex].size() > 0) {
          tracksToFitCollection[closestSeedIndex].push_back( tracksToFitCollection[i][0] );
          tracksToFitCollection[i].clear();
        } //if didn't find another one to attach to, just leave it there.  if nothing else gets added here will be skipped and scheduled for deletion at the fitting step


      } //if only 1 track
    } //loop over collection of tracks to fit vectors
  } while (  0 );
  //past this point not using seedVertices anymore, just the track collections

  ATH_MSG_VERBOSE("Call the fitter on the track collections");

  //Go through the list and actually fit them
  for(std::vector< std::vector<Trk::ITrackLink*> >::iterator cit=tracksToFitCollection.begin(); cit!=tracksToFitCollection.end(); cit++ ) {

    //not enough tracks // !!! Move from tracksToFitCollection to seedTracks for deletion
    if(cit->size() < 2){

      for(std::vector<Trk::ITrackLink*>::iterator linkit= cit->begin(); linkit!=cit->end(); linkit++) {
	seedTracks.push_back( *linkit );
      }

      //empty vertex track collection 
      cit->clear();
      continue;
    }

    //create the perigees the fitter actually wants
    std::vector<const Trk::TrackParameters*> perigeesToFit;
    for(std::vector<Trk::ITrackLink*>::iterator trkit= cit->begin(); trkit!=cit->end(); trkit++) {
      perigeesToFit.push_back( (*trkit)->parameters() );
    }


    Trk::VxCandidate * myVxCandidate = 0;
    if(m_useBeamConstraint) {
      myVxCandidate = m_iVertexFitter->fit( perigeesToFit, theconstraint);
    } else {
      myVxCandidate = m_iVertexFitter->fit( perigeesToFit );
    }


    double ndf = 0.;
    int ntracks = 0;
    countTracksAndNdf(myVxCandidate,ndf,ntracks);

    bool goodVertex = 
        myVxCandidate != 0 &&
        ((!m_useBeamConstraint && ndf>0 && ntracks>=2) ||
         (m_useBeamConstraint && ndf>3 && ntracks>=2));

    if (goodVertex) {
      //not going to reassign tracks now so will add the track links
      std::vector<Trk::VxTrackAtVertex*>* myVxTracksAtVtx=myVxCandidate->vxTrackAtVertex();
      if(myVxTracksAtVtx) { //protection, but not sure when this would happen...
	//not sure if there's any guarantee that all the tracks given to fitter were used, but can at least only search among those...
	//if we find out that is guaranteed we can just directly set the links
	//we will erase links from the small vector, and if there are any left add them back to seedTracks to be deleted at the end.

	std::vector<Trk::VxTrackAtVertex*>::iterator tracksBegin=myVxTracksAtVtx->begin();
	std::vector<Trk::VxTrackAtVertex*>::iterator tracksEnd=myVxTracksAtVtx->end();
       
        for (std::vector<Trk::VxTrackAtVertex*>::iterator tracksIter=tracksBegin; tracksIter!=tracksEnd;++tracksIter) {
	  bool found=false;
	  for(std::vector<Trk::ITrackLink*>::iterator linkit= cit->begin(); linkit!=cit->end(); linkit++) {
	    if((*linkit)->parameters()==(*tracksIter)->initialPerigee()) {
	      found=true;
	      (*tracksIter)->setOrigTrack( *linkit );
	      cit->erase( linkit );
	      break; //done looking for that link
	    }
	  }
	  if (!found) {
	    msg(MSG::ERROR) << " Cannot find vector element to fix links (step 4)! " << endreq;
	  }
	}
      }


      //its a good vertex and the links are all set, so add it to output
      theVxContainer->push_back (myVxCandidate);


    } else {

      if (myVxCandidate) {
	delete myVxCandidate;
	myVxCandidate=0;
      }
    }

    //either bad vertex, so put all the track links back in collection scheduled for deletion
    // or some of the track links weren't used/ didn't get matched to VxTrackAtVertex collection
    // either way the ITrackLink pointer not passed to some VxTrackAtVertex belonging to a VxCandidate
    for(std::vector<Trk::ITrackLink*>::iterator linkit= cit->begin(); linkit!=cit->end(); linkit++) {
      seedTracks.push_back( *linkit );
    }

  } //end loop over vector of vector of tracks in which fit was performed


  msg(MSG::DEBUG) << " Vtx size: "<<theVxContainer->size()<< endreq;;
  for (size_t i = 0 ; i < theVxContainer->size(); i++) {
    if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << " Vtx: " << i << 
	" x= " << (*theVxContainer)[i]->recVertex().position().x() << 
	" y= " << (*theVxContainer)[i]->recVertex().position().y() << 
	" z= " << (*theVxContainer)[i]->recVertex().position().z() << 
	" ntracks= " << (*theVxContainer)[i]->vxTrackAtVertex()->size() << 
	" chi2= " << (*theVxContainer)[i]->recVertex().fitQuality().chiSquared()
		      << " ndf = " << (*theVxContainer)[i]->recVertex().fitQuality().doubleNumberDoF() 
		     << endreq;
   } 
   
  }
  //This bit was done in iterative finder -- not sure why but will copy it here too
  if ( theVxContainer->size() >= 1) {
    Trk::VxCandidate * primaryVtx = theVxContainer->front();
    if (primaryVtx->vxTrackAtVertex()->size()>0) {
      primaryVtx->setVertexType(Trk::PriVtx);
      Trk::VxCandidate * dummyVxCandidate = new Trk::VxCandidate ( primaryVtx->recVertex(),
								   std::vector<Trk::VxTrackAtVertex*>());
      dummyVxCandidate->setVertexType(Trk::NoVtx);
      theVxContainer->push_back ( dummyVxCandidate );
    } else {
      primaryVtx->setVertexType(Trk::NoVtx);
    }
      
  } else if ( theVxContainer->size() == 0 ) {  //---- if no vertex is there let dummy be at beam spot
    
    Trk::VxCandidate * dummyVxCandidate = new Trk::VxCandidate ( m_iBeamCondSvc->beamVtx(),
								 std::vector<Trk::VxTrackAtVertex*>());
    dummyVxCandidate->setVertexType(Trk::NoVtx);
    theVxContainer->push_back ( dummyVxCandidate );
  }
  
  // msg(MSG::DEBUG) << " Vtx size, with a dummy vertex: "<<theVxContainer->size()<< endreq;
  // loop over the pile up to set it as pile up (EXCLUDE first and last vertex, do not do that in split mode)
  for (size_t i = 0 ; i < theVxContainer->size()-1; i++) {
  	if (i>0) {
      		(*theVxContainer)[i]->setVertexType(Trk::PileUp);
    	}
  }
  
  // for (size_t i = 0 ; i < theVxContainer->size(); i++) {
  //   if(msgLvl(MSG::DEBUG)) {
  //     msg(MSG::DEBUG) << " Vtx: " << i << 
  //       " x= " << (*theVxContainer)[i]->recVertex().position().x() << 
  //       " y= " << (*theVxContainer)[i]->recVertex().position().y() << 
  //       " z= " << (*theVxContainer)[i]->recVertex().position().z() << 
  //       " ntracks= " << (*theVxContainer)[i]->vxTrackAtVertex()->size() << 
  //       " chi2= " << (*theVxContainer)[i]->recVertex().fitQuality().chiSquared()
  //       	      << " ndf = " << (*theVxContainer)[i]->recVertex().fitQuality().doubleNumberDoF() << endreq;
		      
  //       if((*theVxContainer)[i]->vertexType()==Trk::NoVtx) msg(MSG::DEBUG) << " Vtx: " << i << "dummy"<< endreq;
  //       else  msg(MSG::DEBUG) << " not a dummy vertex "<< endreq;
  //   }
  // }
  
  //ok so in iterative finder, any ITrackLink in origTracks which is a copy of the vector<ITrackLink*> input that is NOT used ends up deleted.
  //but as they are used, the ITrackLink* elements are just erased from the vector so I guess those aren't deleted. have used seedTrack vector for this abov


  for (std::vector<Trk::ITrackLink*>::iterator trkit=  seedTracks.begin();
       trkit != seedTracks.end(); ++trkit)
  {   
    if ((*trkit)!=0) {
      delete *trkit;
      *trkit=0;
    }
  }

  //  msg(MSG::DEBUG) << "Returning VX Container " << MyTrackVxContainer << endreq;
  return theVxContainer;

}

StatusCode InDetMultiPriVxFinderTool::finalize()
{
    return StatusCode::SUCCESS;
}

void InDetMultiPriVxFinderTool::m_printParameterSettings()
{
    msg(MSG::INFO) << "VxPrimary initialize(): Parametersettings " << endreq;
    msg(MSG::INFO) << "VertexFitter " << m_iVertexFitter << endreq;
    msg(MSG::INFO) << endreq;
}

void InDetMultiPriVxFinderTool::m_SGError(std::string errService)
{
    msg(MSG::FATAL) << errService << " not found. Exiting !" << endreq;
    return;
}


void InDetMultiPriVxFinderTool::countTracksAndNdf(Trk::VxCandidate * myVxCandidate,
                                                      double & ndf, int & ntracks) const
{
  if (myVxCandidate)
  {
    ndf = myVxCandidate->recVertex().fitQuality().doubleNumberDoF();
    
    std::vector<Trk::VxTrackAtVertex*>* myVxTracksAtVtx=myVxCandidate->vxTrackAtVertex();
    
    std::vector<Trk::VxTrackAtVertex*>::iterator tracksBegin=myVxTracksAtVtx->begin();
    std::vector<Trk::VxTrackAtVertex*>::iterator tracksEnd=myVxTracksAtVtx->end();
    
    for (std::vector<Trk::VxTrackAtVertex*>::iterator tracksIter=tracksBegin;
         tracksIter!=tracksEnd;++tracksIter)
    {
      
      if ((*tracksIter)->weight()>0.01)
      {
        ntracks+=1;
      }
    }
  }
}




double InDetMultiPriVxFinderTool::distanceAndError(const Trk::TrackParameters* params, const Trk::Vertex * vertex, double & error) const {

  ATH_MSG_VERBOSE("run distanceAndError( " << params << " , " << vertex << ", &error), " << m_ImpactPoint3dEstimator);

      //find distance safely
      bool isOK=false;
      double distance=0.;
      try {
	Trk::PlaneSurface* mySurface=m_ImpactPoint3dEstimator->Estimate3dIP(params,vertex);
	delete mySurface;
	isOK=true;
      }
      catch (error::ImpactPoint3dEstimatorProblem err) {
	msg(MSG::WARNING) << " ImpactPoin3dEstimator failed to find minimum distance between track and vertex seed: " << err.p << endreq;
      }
      
      if (isOK) {
	distance=m_ImpactPoint3dEstimator->getDistance();
      }  
      if (distance<0) {
	msg(MSG::WARNING) << " Distance between track and seed vtx is negative: " << distance << endreq;
      }


      //      const Trk::MeasuredPerigee* myPerigee= dynamic_cast<const Trk::MeasuredPerigee*>(params);
      //very approximate error
      error= 0.;
	
      if(params) {
        error = std::sqrt( (*params->covariance())(Trk::d0,Trk::d0) + (*params->covariance())(Trk::z0,Trk::z0) );
	// error = std::sqrt(myPerigee->localErrorMatrix().covariance()[Trk::d0][Trk::d0]+
	// 		  myPerigee->localErrorMatrix().covariance()[Trk::z0][Trk::z0]);
      }
      
      if (error==0.) {
	msg(MSG::ERROR) << " Error is zero! " << distance << endreq;
	error=1.;
      }
      msg(MSG::VERBOSE) << " Distance between track and seed vtx: " << distance << " d/s(d) = " << 
	distance/error << " err " << error << endreq;

      return distance;

}

  
} // end namespace InDet

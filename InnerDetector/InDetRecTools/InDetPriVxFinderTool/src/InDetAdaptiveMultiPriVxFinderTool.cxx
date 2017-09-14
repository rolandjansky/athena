/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetAdaptivePriVxFinderTool.cxx  -  Description
                             -------------------
    begin   : 29-05-2006
    authors : Giacinto Piacquadio (Freiburg Univ),
              this is a modified version of the adaptive primary vertex finder to fit more vertexes at the same time
    changes :
	      06/12/2006   Kirill.Prokofiev@cern.ch 
	      EDM cleanup and switching to the FitQuality use      
              2007-02-15  bug fix by scott s snyder  <snyder@bnl.gov>
              Fix memory leak.  Don't use a map for sorting.
              2007-10-xx  Giacinto Piacquadio
                          Many improvements (MultiVertexFinder v. 2) 

 ***************************************************************************/
// #include <algorithm>
#include "VxVertex/RecVertex.h"
#include "VxVertex/Vertex.h"
#include "InDetPriVxFinderTool/InDetAdaptiveMultiPriVxFinderTool.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "VxMultiVertex/MVFVxCandidate.h"
#include <map>
#include <vector>
#include <utility>
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "NN.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "VxVertex/VxContainer.h"
#include "VxMultiVertex/MVFVxTrackAtVertex.h"
#include "VxMultiVertex/TrackToVtxLink.h"
#include "DataModel/DataVector.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkVertexFitters/AdaptiveMultiVertexFitter.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
// #include <limits>
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include <TMath.h>

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"

//added for cuts in case of displaced vertex
// #include "TrkExInterfaces/IExtrapolator.h"

//#define MULTIFINDER_DEBUG

namespace InDet
{
InDetAdaptiveMultiPriVxFinderTool::InDetAdaptiveMultiPriVxFinderTool(const std::string& t, const std::string& n, const IInterface*  p)
  : AthAlgTool(t,n,p),
    m_MultiVertexFitter("Trk::AdaptiveMultiVertexFitter"),
    m_SeedFinder("Trk::ZScanSeedFinder"),
    m_trkFilter("InDet::InDetTrackSelection"),
    m_VertexEdmFactory("Trk::VertexInternalEdmFactory"),
    m_iBeamCondSvc("BeamCondSvc",n),
    m_useBeamConstraint(true),
    m_TracksMaxZinterval(5.),
    m_maxVertexChi2(18.42),
    m_realMultiVertex(true),
    m_useFastCompatibility(true),
    m_selectiontype(0),
    m_finalCutMaxVertexChi2(18.42),
    m_cutVertexDependence(3.),
    m_minweight(0.0001),
    m_maxIterations(100),
    m_testingclass(0),
    m_addSingleTrackVertices(false),
    m_do3dSplitting(false),
    m_zBfieldApprox(0.60407),
    m_maximumVertexContamination(0.5)
{
    
    declareInterface<IVertexFinder>(this);//by GP: changed from InDetMultiAdaptivePriVxFinderTool to IPriVxFinderTool
    /* Retrieve StoreGate container and tool names from job options */
    declareProperty("SeedFinder"       , m_SeedFinder);
    declareProperty("VertexFitterTool", m_MultiVertexFitter);
    declareProperty("TrackSelector",m_trkFilter);
    declareProperty("InternalEdmFactory", m_VertexEdmFactory);

    //finder options
    declareProperty("TracksMaxZinterval",m_TracksMaxZinterval);
    declareProperty("maxVertexChi2",m_maxVertexChi2);
    declareProperty("finalCutMaxVertexChi2",m_finalCutMaxVertexChi2);
    declareProperty("cutVertexDependence",m_cutVertexDependence);
    declareProperty("MinWeight",m_minweight);
    declareProperty("realMultiVertex",m_realMultiVertex);
    declareProperty("useFastCompatibility",m_useFastCompatibility);
    declareProperty("useBeamConstraint",m_useBeamConstraint);
    declareProperty("BeamPositionSvc",m_iBeamCondSvc);
    declareProperty("addSingleTrackVertices",m_addSingleTrackVertices);
    //********* signal vertex selection (for pile up) ****
    declareProperty("selectiontype",m_selectiontype);
    //==0 for sum p_t^2
    //==1 for NN
    //==2 for min bias compatibility estimation (in the future)
    declareProperty("maxIterations",m_maxIterations);
    declareProperty("do3dSplitting",m_do3dSplitting);
    declareProperty("zBfieldApprox",m_zBfieldApprox);
    declareProperty("maximumVertexContamination",m_maximumVertexContamination);
}

InDetAdaptiveMultiPriVxFinderTool::~InDetAdaptiveMultiPriVxFinderTool()
{}

StatusCode InDetAdaptiveMultiPriVxFinderTool::initialize()
{
    StatusCode sc;

    /* Get the right vertex fitting tool */
    if ( m_MultiVertexFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_MultiVertexFitter << endreq;
      return StatusCode::FAILURE;
    } 
    
    if ( m_SeedFinder.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_SeedFinder << endreq;
      return StatusCode::FAILURE;
    }

    sc = m_iBeamCondSvc.retrieve();
    if (sc.isFailure())
      {
	msg(MSG::ERROR) << "Could not find BeamCondSvc." << endreq;
	return sc;
      }
    
    if(m_trkFilter.retrieve().isFailure()) {
      msg(MSG::ERROR) << " Unable to retrieve "<<m_trkFilter<<endreq;
      return StatusCode::FAILURE;
    } 
 
    // since some parameters special to an inherited class this method
    // will be overloaded by the inherited class
    m_printParameterSettings();

    msg(MSG::INFO) << "Initialization successful" << endreq;
    return StatusCode::SUCCESS;
}


namespace {

struct VxCandidates_pair
{
  double first;
  Trk::VxCandidate* second;
  VxCandidates_pair(double p1, Trk::VxCandidate* p2)
    : first (p1), second (p2) {}
  bool operator< (const VxCandidates_pair& other) const
  { return first > other.first; }
};


} // anonymous namespace


VxContainer* InDetAdaptiveMultiPriVxFinderTool::findVertex(const TrackCollection* trackTES) 
{


  Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx());

  std::vector<const Trk::ITrackLink*> selectedTracks;

  typedef DataVector<Trk::Track>::const_iterator TrackDataVecIter;

  Root::TAccept selectionPassed;
  for (TrackDataVecIter itr  = (*trackTES).begin(); itr != (*trackTES).end(); itr++) {
    if (m_useBeamConstraint) {
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
  
  std::vector<const Trk::ITrackLink*>::iterator ibegin=selectedTracks.begin();
  std::vector<const Trk::ITrackLink*>::iterator iend=selectedTracks.end();

  for (std::vector<const Trk::ITrackLink*>::iterator iiter=ibegin;iiter!=iend;++iiter) {
    if ((*iiter)!=0) {
      delete *iiter;
      *iiter=0;
    }
  }

  return returnContainer;

}

VxContainer* InDetAdaptiveMultiPriVxFinderTool::findVertex(const Trk::TrackParticleBaseCollection* trackTES) {
 
  std::vector<const Trk::ITrackLink*> selectedTracks;

  Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx());

  typedef DataVector<Trk::TrackParticleBase>::const_iterator TrackParticleDataVecIter;

  Root::TAccept selectionPassed;
  for (TrackParticleDataVecIter itr  = (*trackTES).begin(); itr != (*trackTES).end(); itr++) {
    if (m_useBeamConstraint) {
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

  std::vector<const Trk::ITrackLink*>::iterator ibegin=selectedTracks.begin();
  std::vector<const Trk::ITrackLink*>::iterator iend=selectedTracks.end();

  for (std::vector<const Trk::ITrackLink*>::iterator iiter=ibegin;iiter!=iend;++iiter) {
    if ((*iiter)!=0) {
      delete *iiter;
      *iiter=0;
    }
  }

  return returnContainer;

}

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetAdaptiveMultiPriVxFinderTool::findVertex(const xAOD::TrackParticleContainer* trackParticles)
{
  std::vector<const Trk::ITrackLink*> selectedTracks;

  xAOD::Vertex beamposition;
  beamposition.makePrivateStore();

  beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
  beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());

  typedef DataVector<xAOD::TrackParticle>::const_iterator TrackParticleDataVecIter;

  Root::TAccept selectionPassed;
  for (TrackParticleDataVecIter itr  = (*trackParticles).begin(); itr != (*trackParticles).end(); itr++) {
    if (m_useBeamConstraint) {
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
      linkTT->setStorableObject(*trackParticles); //@TODO: really?!
      selectedTracks.push_back(linkTT);
    }
  }

  ATH_MSG_DEBUG("Of " << trackParticles->size() << " tracks " << selectedTracks.size() << " survived the preselection.");

  VxContainer* returnContainer=findVertex(selectedTracks);

  std::vector<const Trk::ITrackLink*>::iterator ibegin=selectedTracks.begin();
  std::vector<const Trk::ITrackLink*>::iterator iend=selectedTracks.end();

  //now convert VxContainer to XAOD Vertex
  xAOD::VertexContainer *xAODContainer(0);
  xAOD::VertexAuxContainer *xAODAuxContainer(0);
  if (m_VertexEdmFactory->createXAODVertexContainer(*returnContainer, xAODContainer, xAODAuxContainer) != StatusCode::SUCCESS) {
    ATH_MSG_WARNING("Cannot convert output vertex container to xAOD. Returning null pointer.");
  }

  //clean-up and return
  for (std::vector<const Trk::ITrackLink*>::iterator iiter=ibegin;iiter!=iend;++iiter) {
    if ((*iiter)!=0) {
      delete *iiter;
      *iiter=0;
    }
  }
  delete returnContainer;

  return std::make_pair(xAODContainer, xAODAuxContainer);

}


VxContainer* InDetAdaptiveMultiPriVxFinderTool::findVertex(const std::vector<const Trk::ITrackLink*> & trackVector) {

  
  ////////std::vector<Trk::VxTrackAtVertex> * trkAtVtx;
  if (m_selectiontype==1) {//if you have to use NN, load the class
    m_testingclass=new NN();//check later for MEMORY LEAK
  }
  
  //---- Start of preselection of tracks according to perigee parameters ---------------//
  std::vector<const Trk::ITrackLink*> origTracks=trackVector;
  std::vector<const Trk::ITrackLink*> seedTracks=trackVector;
  //  origTracks.clear();
  //  seedTracks.clear();
  
  //now all tracks are in origTracks... std::vector<const Track*> origTracks;
  std::vector<VxCandidates_pair> myVxCandidates;
  
  std::vector<Trk::TrackToVtxLink*> myTrackToVtxLinks;

  //double vertexPt=0.;
  VxContainer* theVxContainer = new VxContainer;
  //    trackToVtxLinkTES = new TrackToVtxLinkContainer;
  
  Trk::Vertex actualVertex;
  
  std::vector<Trk::VxTrackAtVertex*> tracksToAdd;
  
  
  std::map<const Trk::ITrackLink*,Trk::TrackToVtxLink*> TrackLinkOf;
  
  
  //create a map between ITrackLink* and TrackToVtxLink*
  std::vector<const Trk::ITrackLink*>::const_iterator trkbegin=origTracks.begin();
  std::vector<const Trk::ITrackLink*>::const_iterator trkend=origTracks.end();
  
  
  for (std::vector<const Trk::ITrackLink*>::const_iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {
    //      std::cout << "Creating VtxLink for a new track" << std::endl;
    Trk::TrackToVtxLink* newTrkToVtxLink(new Trk::TrackToVtxLink(new std::vector<Trk::VxCandidate*>));
    TrackLinkOf[*trkiter]=newTrkToVtxLink;
    myTrackToVtxLinks.push_back(newTrkToVtxLink);
  }
  
  
  //prepare iterators for tracks only necessary for seeding
  std::vector<const Trk::ITrackLink*>::iterator seedtrkbegin=seedTracks.begin();
  std::vector<const Trk::ITrackLink*>::iterator seedtrkend=seedTracks.end();
  
  int iteration=0;
  unsigned int seedtracknumber=seedTracks.size();
  do {
    
    if (seedtracknumber==0) 
    {
      if(msgLvl(MSG::DEBUG)) msg() << " No tracks available after track selection for seeding. No finding done." << endreq;
      break;
    }

    if(msgLvl(MSG::DEBUG)) msg() << "ITERATION NUMBER " << iteration << endreq;
    iteration+=1;
    //now use all the perigee parameters you have so far

    if (m_realMultiVertex==true) {
      trkbegin=origTracks.begin();
      trkend=origTracks.end();
    } else {
      trkbegin=seedTracks.begin();
      trkend=seedTracks.end();
    }
    
    std::vector<const Trk::TrackParameters*> perigeeList;
    
    for (std::vector<const Trk::ITrackLink*>::iterator seedtrkAtVtxIter=seedtrkbegin;
	 seedtrkAtVtxIter!=seedtrkend;++seedtrkAtVtxIter) {
      perigeeList.push_back((*seedtrkAtVtxIter)->parameters());
    }
    
    Trk::RecVertex* theconstraint=0;
    if (m_useBeamConstraint) {
      theconstraint=new Trk::RecVertex(m_iBeamCondSvc->beamVtx());
      actualVertex=m_SeedFinder->findSeed(perigeeList,theconstraint);
    } else {
      actualVertex=m_SeedFinder->findSeed(perigeeList);
      Amg::MatrixX looseConstraintCovariance(3,3);
      looseConstraintCovariance.setIdentity();
      looseConstraintCovariance = looseConstraintCovariance * 1e+8;
      theconstraint= new Trk::RecVertex(actualVertex.position(),looseConstraintCovariance,-3.,0.);
    }
    
    if (actualVertex.position().z()==0.) {
      if (msgLvl(MSG::WARNING))
      {
        msg() << "No seed found: no primary vertex finding performed on this event" << endreq;
        msg() << "Error in Seed Finder. Number of input tracks: " << perigeeList.size() << " but no seed returned." << endreq;
      }
      break;
    }
    
    //new MVxCandidate with this
    Trk::VxCandidate* actualcandidate(new Trk::MVFVxCandidate(theconstraint,
							      new Trk::Vertex(actualVertex),
							      new Trk::Vertex(actualVertex)));

    //    myVxCandidates.push_back(actualcandidate); do I need it here?

    //get link to the tracks
    std::vector<Trk::VxTrackAtVertex*>* tracksOfVertex(actualcandidate->vxTrackAtVertex());
    
    //now iterate on all tracks and find out if they are sufficiently close to the found vertex
    
    if(msgLvl(MSG::VERBOSE)) msg() << "Adding all the tracks which are near the seed ti the candidate " << endreq;
    
    for (std::vector<const Trk::ITrackLink*>::const_iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {

//      if (fabs((*trkiter)->parameters()->position()[Trk::z]-actualVertex.position().z())<m_TracksMaxZinterval) {

      if (fabs(estimateDeltaZ(*(*trkiter)->parameters(),actualVertex))<m_TracksMaxZinterval)
      {
	//accessing corresponding link to vertices
   	Trk::TrackToVtxLink* actuallink=TrackLinkOf[*trkiter];
	std::vector<Trk::VxCandidate*>* actualvtxlink=actuallink->vertexes();
	//adding vertex to candidates of track
	actualvtxlink->push_back(actualcandidate);
#ifdef MULTIFINDER_DEBUG
	std::cout << "Adding a VxTrackAtVertex with tracklink " << actuallink << " to the vertex candidate vector of track " << tracksOfVertex << std::endl;
#endif
	tracksOfVertex->push_back(new Trk::MVFVxTrackAtVertex((*trkiter)->clone(),
							      actuallink));
      }
    }

    //now consider to recovery from the case where no tracks were added to the vertex
    if (tracksOfVertex->size()==0) {
      
      //you need to define a new seed (because the old one is probably in between two ones...)
      double zdistance=1e8;
      const Trk::ITrackLink* nearestTrack=0;
      
      for (std::vector<const Trk::ITrackLink*>::const_iterator seedtrkiter=seedtrkbegin;
	   seedtrkiter!=seedtrkend;++seedtrkiter) {
	if (fabs((*seedtrkiter)->parameters()->position()[Trk::z]-actualVertex.position().z())<zdistance) {
	  zdistance=fabs((*seedtrkiter)->parameters()->position()[Trk::z]-actualVertex.position().z());
	  nearestTrack=*seedtrkiter;
	}
      }

      if (nearestTrack!=0) {

	double newz=(nearestTrack->parameters())->position()[Trk::z];
	Trk::VxCandidate* oldcandidate=actualcandidate;

	actualVertex=Trk::Vertex(Amg::Vector3D(0.,0.,newz));	
	actualcandidate=new Trk::MVFVxCandidate(new Trk::RecVertex(*theconstraint),
						new Trk::Vertex(actualVertex),
						new Trk::Vertex(actualVertex));
	delete oldcandidate;oldcandidate=0;

	//get link to the tracks
	tracksOfVertex=actualcandidate->vxTrackAtVertex();

	for (std::vector<const Trk::ITrackLink*>::const_iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {
//	  if (fabs((*trkiter)->parameters()->position()[Trk::z]-actualVertex.position().z())<m_TracksMaxZinterval) {

	  if (fabs(estimateDeltaZ(*((*trkiter)->parameters()),
                                  actualVertex))<m_TracksMaxZinterval)
          {
	    //accessing corresponding link to vertices
	    Trk::TrackToVtxLink* actuallink=TrackLinkOf[*trkiter];
	    std::vector<Trk::VxCandidate*>* actualvtxlink=actuallink->vertexes();
	    //adding vertex to candidates of track
	    actualvtxlink->push_back(actualcandidate);
#ifdef MULTIFINDER_DEBUG
	    std::cout << "SPECIAL Adding a VxTrackAtVertex with tracklink " << actuallink << " to the vertex candidate vector of track " << tracksOfVertex << std::endl;
#endif
	    tracksOfVertex->push_back(new Trk::MVFVxTrackAtVertex((*trkiter)->clone(),
								  actuallink));
	  }
	}

	if (tracksOfVertex->size()==0) {
	  if (msgLvl(MSG::ERROR)) msg() << " Internal error in the Multi Vertex Finder. No tracks while at least one track was expected." << endreq;
	  break;
	}
      } else {
	  if (msgLvl(MSG::ERROR)) msg() << "Internal error in the Multi Vertex Finder. Nearest track is missing while it is expected." << endreq;
	  break;
      }
    }
      
    

    //now at least you have all the tracks you really want to fit and the connections to the TrackToVtxLinks
    //      std::cout << "Calling MultiVertexFitter addVtxTofit() " << std::endl;
    
    if (msgLvl(MSG::DEBUG)) msg() << "running addVtxTofit() " << endreq;
#ifdef MULTIFINDER_DEBUG
    std::cout << "The actual canidate has " << tracksOfVertex->size() << " tracks in the pointer" << std::endl;
#endif

#ifdef MULTIFINDER_DEBUG
    std::cout << "The actual canidate has recvertex z: " << actualcandidate->recVertex().position()[Trk::z] << std::endl;
#endif     
    m_MultiVertexFitter->addVtxTofit(actualcandidate);
#ifdef MULTIFINDER_DEBUG
    std::cout << "The actual canidate has recvertex z after fit: " << actualcandidate->recVertex().position()[Trk::z] << std::endl;
#endif     
    
    std::vector<Trk::VxTrackAtVertex*>::const_iterator trkAtVtxbegin=tracksOfVertex->begin();
    std::vector<Trk::VxTrackAtVertex*>::const_iterator trkAtVtxend=tracksOfVertex->end();
    
    //now check that there is at least one track added to the fit
    //(this is not always the case because only tracks above a certain compatibility threshold are considered)
    bool atleastonecompatibletrack=false;
    int numberOfCompatibleTracks=0;
    for (std::vector<Trk::VxTrackAtVertex*>::const_iterator trkAtVtxIter=trkAtVtxbegin;
	 trkAtVtxIter!=trkAtVtxend;++trkAtVtxIter) { 
#ifdef MULTIFINDER_DEBUG
      std::cout << " compatibility: " << static_cast<Trk::MVFVxTrackAtVertex*>(*trkAtVtxIter)->vtxCompatibility() << " weight " << static_cast<Trk::MVFVxTrackAtVertex*>(*trkAtVtxIter)->weight() << std::endl;
#endif
      if ( ( static_cast<Trk::MVFVxTrackAtVertex*>(*trkAtVtxIter)->vtxCompatibility()<m_maxVertexChi2&&m_useFastCompatibility)||
	   ( static_cast<Trk::MVFVxTrackAtVertex*>(*trkAtVtxIter)->weight()>m_minweight 
	     && static_cast<Trk::MVFVxTrackAtVertex*>(*trkAtVtxIter)->trackQuality().chiSquared()<m_maxVertexChi2 
	     && !m_useFastCompatibility ) ) {
	
	const Trk::ITrackLink* foundTrack=0;
	for (std::vector<const Trk::ITrackLink*>::const_iterator seedtrkiter=seedtrkbegin;seedtrkiter!=seedtrkend;++seedtrkiter) {
	  if ((*seedtrkiter)->parameters()==(*trkAtVtxIter)->trackOrParticleLink()->parameters()) {
	    foundTrack=*seedtrkiter;
	  }
	}
	if (foundTrack!=0) {
	  atleastonecompatibletrack=true;
	  numberOfCompatibleTracks+=1;
#ifdef MULTIFINDER_DEBUG
	  std::cout << " found in seed " << std::endl;
#endif
          if (m_addSingleTrackVertices)
          {
            
            if (m_useBeamConstraint) break;
            if (numberOfCompatibleTracks>1&&(!m_useBeamConstraint)) break;            
          }
          else
          {
            
            if (numberOfCompatibleTracks>1) break;
          }
          
	}
      }
    }

    bool newVertexIsFine=false;

    if (m_addSingleTrackVertices)
    {
      if (m_useBeamConstraint)
      {
        if (numberOfCompatibleTracks>0)
        {
          newVertexIsFine=true;
        }
      }
      else
      {
        if (numberOfCompatibleTracks>1)
        {
          newVertexIsFine=true;
        }
      }
    }
    else
    {
      if (numberOfCompatibleTracks>1)
      {
        newVertexIsFine=true;
      }
    }
  
    //this now should be so powerful to do everything by itself
    //problem now is to delete the really compatible tracks to this fit from the tracks 
    //which still remain to be fitted
    
    if (atleastonecompatibletrack) {
      for (std::vector<Trk::VxTrackAtVertex*>::const_iterator trkAtVtxIter=trkAtVtxbegin;
	   trkAtVtxIter!=trkAtVtxend;++trkAtVtxIter) {
	//for now using the compatibility at stage before end...
#ifdef MULTIFINDER_DEBUG
	std::cout << "The compatibility value of the track " << *trkAtVtxIter << " has compatibility " << 
	  (static_cast<Trk::MVFVxTrackAtVertex*>(*trkAtVtxIter))->vtxCompatibility() << std::endl;
#endif
	if ( ( static_cast<Trk::MVFVxTrackAtVertex*>(*trkAtVtxIter)->vtxCompatibility()<m_maxVertexChi2&&m_useFastCompatibility)||
	     ( static_cast<Trk::MVFVxTrackAtVertex*>(*trkAtVtxIter)->weight()>m_minweight 
	       && static_cast<Trk::MVFVxTrackAtVertex*>(*trkAtVtxIter)->trackQuality().chiSquared()<m_maxVertexChi2 
	       && !m_useFastCompatibility ) ) {
#ifdef MULTIFINDER_DEBUG
	  std::cout << "Eliminating track " << std::endl;
#endif

	std::vector<const Trk::ITrackLink*>::iterator foundTrack=seedtrkend;
	for (std::vector<const Trk::ITrackLink*>::iterator seedtrkiter=seedtrkbegin;seedtrkiter!=seedtrkend;++seedtrkiter) {
	  if ((*seedtrkiter)->parameters()==(*trkAtVtxIter)->trackOrParticleLink()->parameters()) {
	    foundTrack=seedtrkiter;
	  }
	}
	  
#ifdef MULTIFINDER_DEBUG
	  std::cout << "Trying to find track now" << std::endl;
#endif
	  if (foundTrack!=seedtrkend) {
#ifdef MULTIFINDER_DEBUG
	    std::cout << "Track found: eliminating it" << std::endl;
#endif
	    seedTracks.erase(foundTrack);
	    
	    //update end end begin??? should I? yes, he can copy, regenerate, you don't know!
	    seedtrkbegin=seedTracks.begin();
	    seedtrkend=seedTracks.end();
	    
	    
#ifdef MULTIFINDER_DEBUG
	    std::cout << "Remaining seeds: " << seedTracks.size() << std::endl;
#endif
	  }
	}
      }
    } else {//no compatible track found...
      //in this case put out the highest seeding track which didn't give any good result...
      double highestcompatibility=0;
      Trk::VxTrackAtVertex* trackHighestCompatibility=0;
      
#ifdef MULTIFINDER_DEBUG
      std::cout <<"analyzing new vertex" << std::endl;
#endif

      for (std::vector<Trk::VxTrackAtVertex*>::const_iterator trkAtVtxIter=trkAtVtxbegin;
	   trkAtVtxIter!=trkAtVtxend;++trkAtVtxIter) {
#ifdef MULTIFINDER_DEBUG
	std::cout <<"new track" << std::endl;
#endif
	const Trk::ITrackLink* foundTrack=0;
	for (std::vector<const Trk::ITrackLink*>::const_iterator seedtrkiter=seedtrkbegin;seedtrkiter!=seedtrkend;++seedtrkiter) {
	  if ((*seedtrkiter)->parameters()==(*trkAtVtxIter)->trackOrParticleLink()->parameters()) {
	    foundTrack=*seedtrkiter;
	  }
	}

	if (foundTrack!=0) {
	  double compatibility=static_cast<Trk::MVFVxTrackAtVertex*>(*trkAtVtxIter)->vtxCompatibility();
#ifdef MULTIFINDER_DEBUG
	  std::cout << " found: comp: " << compatibility << std::endl;
#endif
	  if (compatibility>highestcompatibility) {
	    highestcompatibility=compatibility;
	    trackHighestCompatibility=*trkAtVtxIter;
	  }
	}
      }
#ifdef MULTIFINDER_DEBUG
      std::cout <<"track highestcomp:" <<trackHighestCompatibility << " comp " << highestcompatibility <<std::endl;
#endif

      if (trackHighestCompatibility!=0) {
	std::vector<const Trk::ITrackLink*>::iterator foundTrack=seedtrkend;
	for (std::vector<const Trk::ITrackLink*>::iterator seedtrkiter=seedtrkbegin;seedtrkiter!=seedtrkend;++seedtrkiter) {
	  if ((*seedtrkiter)->parameters()==trackHighestCompatibility->trackOrParticleLink()->parameters()) {
	    foundTrack=seedtrkiter;
	  }
	}

	if (foundTrack!=seedtrkend) {
	  seedTracks.erase(foundTrack);
	  seedtrkbegin=seedTracks.begin();
	  seedtrkend=seedTracks.end();
	} else {
	  std::cout << " Cannot find previous determined track " << std::endl;
	  throw;
	}
      } else {
	
	//alternative method: delete seed track nearest in z to the seed
	
	double zdistance=1e8;
	const Trk::ITrackLink* nearestTrack=0;
	
	for (std::vector<const Trk::ITrackLink*>::const_iterator seedtrkiter=seedtrkbegin;
	     seedtrkiter!=seedtrkend;++seedtrkiter) {
	  if (fabs((*seedtrkiter)->parameters()->position()[Trk::z]-actualVertex.position().z())<zdistance) {
	    zdistance=fabs((*seedtrkiter)->parameters()->position()[Trk::z]-actualVertex.position().z());
	    nearestTrack=*seedtrkiter;
	  }
	}

	if (nearestTrack!=0) {
	  std::vector<const Trk::ITrackLink*>::iterator foundTrackToDelete=
	    std::find(seedtrkbegin,seedtrkend,nearestTrack);
	  if (foundTrackToDelete!=seedtrkend) {
	    seedTracks.erase(foundTrackToDelete);
	    seedtrkbegin=seedTracks.begin();
	    seedtrkend=seedTracks.end();
	  } else {
	    if (msgLvl(MSG::ERROR)) msg() << " Internal error in the Multi Vertex Finder. No nearest track found while it was expected to be found." << endreq;
	    break;
	  }
	} else {
	  if (msgLvl(MSG::ERROR)) msg() << " Internal error in the Multi Vertex Finder. No further seeding track was found (3 methods used): it was expected to be found " << endreq;
	  break;
	}
      }
    }
    
    
    ///////////////
    //now break the cycle if you didn't diminuish the number of seeds...
    
#ifdef MULTIFINDER_DEBUG
    std::cout << "Remaining seeds: " << seedTracks.size() << " previous round " << seedtracknumber << std::endl;
#endif
    
    bool deleteLastVertex=false;

    if (!newVertexIsFine) {
      deleteLastVertex=true;
    } else {
      
      double contamination=0.;
      
      double contaminationNom=0;
      double contaminationDeNom=0;
      
      std::vector<Trk::VxTrackAtVertex*>::iterator TRKtrkbegin((actualcandidate)->vxTrackAtVertex()->begin());
      std::vector<Trk::VxTrackAtVertex*>::iterator TRKtrkend((actualcandidate)->vxTrackAtVertex()->end());
      
      for (std::vector<Trk::VxTrackAtVertex*>::iterator TRKtrkiter=TRKtrkbegin;TRKtrkiter!=TRKtrkend;++TRKtrkiter) {
        
        double trackWeight=(static_cast<Trk::MVFVxTrackAtVertex*>(*TRKtrkiter))->weight();
        contaminationNom+=trackWeight*(1.-trackWeight);
        contaminationDeNom+=trackWeight*trackWeight;
      }
      
      if (contaminationDeNom>0)
      {
        contamination=contaminationNom/contaminationDeNom;
      }
      
//      std::cout << " contamination: " << contamination << std::endl;
      
      if (contamination>m_maximumVertexContamination)
      {
        deleteLastVertex=true;
      }
      
     //now try to understand if the vertex was merged with another one...
      std::vector<VxCandidates_pair>::iterator vxbegin=myVxCandidates.begin();
      std::vector<VxCandidates_pair>::iterator vxend=myVxCandidates.end();
      
      for (std::vector<VxCandidates_pair>::iterator vxiter=vxbegin;vxiter!=vxend;++vxiter) {
#ifdef MULTIFINDER_DEBUG
	std::cout << " Estimating comp of z: " << static_cast<Trk::MVFVxCandidate*>((*vxiter).second)->recVertex().position()[Trk::z] << 
	  " and" << static_cast<Trk::MVFVxCandidate*>(actualcandidate)->recVertex().position()[Trk::z] << std::endl;
#endif
	//in case of no beam spot constraint you should use the full 3d significance on the distance
	double dependence=0;

        

        if (!m_do3dSplitting)
        {
          dependence=fabs((*vxiter).second->recVertex().position()[Trk::z]-actualcandidate->recVertex().position()[Trk::z])/
	    TMath::Sqrt( (*vxiter).second->recVertex().covariancePosition()(Trk::z,Trk::z)+
			 actualcandidate->recVertex().covariancePosition()(Trk::z,Trk::z));
        }
        else
        {
	  Amg::MatrixX sumCovariances=
              (*vxiter).second->recVertex().covariancePosition()+
              actualcandidate->recVertex().covariancePosition();

	  /*
          int failInversion;
          sumCovariances.invert(failInversion);
          if (failInversion==0)
          {
	  */
	  sumCovariances = sumCovariances.inverse().eval();

	  Amg::Vector3D hepVectorPosition;
	  hepVectorPosition[0]=
	    ((*vxiter).second->recVertex().position()-
	     actualcandidate->recVertex().position()).x();
	  hepVectorPosition[1]=
	    ((*vxiter).second->recVertex().position()-
	     actualcandidate->recVertex().position()).y();
	  hepVectorPosition[2]=
	    ((*vxiter).second->recVertex().position()-
	     actualcandidate->recVertex().position()).z();
	  dependence=sqrt(hepVectorPosition.dot(sumCovariances*hepVectorPosition));
	  /*
          }
          else
          {
            if (msgLvl(MSG::ERROR)) msg() << " Inversion of matrix during splitting phase failed... Please report... " << endreq;
            dependence=100;
          }
	  */
        }
	
#ifdef MULTIFINDER_DEBUG
	std::cout << " dependence is: " << dependence << "cut at " << m_cutVertexDependence << std::endl;
#endif
	if (dependence<m_cutVertexDependence) {
	  deleteLastVertex=true;
	  break;
	}
      }
    }
    


    ////////////
    //Ok all tracks in seed were deleted. You can go ahead and discover further vertices...
    //please clean the track to vertices links before (required by real multivertexfit)
    if (deleteLastVertex) {
      
      std::vector<Trk::VxTrackAtVertex*>::iterator trkAtVtxBegin=actualcandidate->vxTrackAtVertex()->begin();
      std::vector<Trk::VxTrackAtVertex*>::iterator trkAtVtxEnd=actualcandidate->vxTrackAtVertex()->end();
      
      for (std::vector<Trk::VxTrackAtVertex*>::iterator trkIterator=trkAtVtxBegin;trkIterator!=trkAtVtxEnd;++trkIterator) {
	
#ifdef MULTIFINDER_DEBUG
	std::cout << "Deleting one vertex from tracklink " << (static_cast<Trk::MVFVxTrackAtVertex*>(*trkIterator))->linkToVertices() << std::endl;
#endif
	
	(static_cast<Trk::MVFVxTrackAtVertex*>(*trkIterator))->linkToVertices()->vertexes()->pop_back();
      }
      seedtracknumber=seedTracks.size();

#ifdef MULTIFINDER_DEBUG
      std::cout << " redoing fit " << std::endl;
#endif      
      m_MultiVertexFitter->addVtxTofit(actualcandidate);

      delete actualcandidate;actualcandidate=0;

    } else {
    
      seedtracknumber=seedTracks.size();
#ifdef MULTIFINDER_DEBUG
      std::cout << " new vertex to be stored: number of tracks " << actualcandidate->vxTrackAtVertex()->size() << std::endl;
#endif   
      myVxCandidates.push_back
	(VxCandidates_pair (0,//estimateSignalCompatibility(actualcandidate),
			    actualcandidate));
    }
    
  } while ( (
                (m_addSingleTrackVertices&&seedTracks.size()>0)||
                ((!m_addSingleTrackVertices)&&seedTracks.size()>1) )
            && iteration<m_maxIterations);

  if (iteration>= m_maxIterations) {
    if (msgLvl(MSG::WARNING)) msg() << " Primary vertex finding: maximum number of iterations reached (more vertices -> set the maximum to a higher value)" << endreq;
  }

  //correction of a bug: you can estimate the probability of being 
  //the primary interaction vertex only after the whole multivertexfit 
  //is finished!!! (the first is influenced by the fit of the second and so 
  //on...)
  std::vector<VxCandidates_pair>::iterator vtxBegin=myVxCandidates.begin();
  std::vector<VxCandidates_pair>::iterator vtxEnd=myVxCandidates.end();
  
  //before filling the container, you have to decide what is your most probable signal vertex
  for (std::vector<VxCandidates_pair>::iterator vtxIter=vtxBegin;vtxIter!=vtxEnd;++vtxIter) {
    (*vtxIter).first=estimateSignalCompatibility((*vtxIter).second);
  }

  std::sort (myVxCandidates.begin(), myVxCandidates.end());

  if (myVxCandidates.size()==0) {

#ifdef MULTIFINDER_DEBUG
    std::cout << "No candidate fitted: return a candidate with the beam spot constraint" << std::endl;
#endif
    
    Trk::VxCandidate * beamspotCandidate = new Trk::VxCandidate(m_iBeamCondSvc->beamVtx(),
								std::vector<Trk::VxTrackAtVertex*>());
    myVxCandidates.push_back(VxCandidates_pair(0,beamspotCandidate));
  }
  

  vtxBegin=myVxCandidates.begin();
  vtxEnd=myVxCandidates.end();

  for (std::vector<VxCandidates_pair>::const_iterator vtxIter=vtxBegin;vtxIter!=vtxEnd;++vtxIter) {

    Trk::VxCandidate* cand = vtxIter->second;
    std::vector<Trk::VxTrackAtVertex*>::iterator trkBegin=cand->vxTrackAtVertex()->begin();
    std::vector<Trk::VxTrackAtVertex*>::iterator trkEnd=cand->vxTrackAtVertex()->end();
    
    for (std::vector<Trk::VxTrackAtVertex*>::iterator trkIter=trkBegin;trkIter!=trkEnd;){//++trkIter) {
      //setting link to TrackToVtxLink to 0 (all TrackToVtxLink will be deleted some lines later)
      (static_cast<Trk::MVFVxTrackAtVertex*>(*trkIter))->setLinkToVertices(0);
      //cleaning up incompatible vertices
      if ( ( static_cast<Trk::MVFVxTrackAtVertex*>(*trkIter)->vtxCompatibility()>m_maxVertexChi2&&m_useFastCompatibility ) ||
	   ( ( static_cast<Trk::MVFVxTrackAtVertex*>(*trkIter)->weight()<m_minweight 
	       || static_cast<Trk::MVFVxTrackAtVertex*>(*trkIter)->trackQuality().chiSquared()>m_maxVertexChi2 ) 
	     && !m_useFastCompatibility  ) ) {
	delete *trkIter;
	trkIter=cand->vxTrackAtVertex()->erase(trkIter);
	trkEnd=cand->vxTrackAtVertex()->end();
      } else {
	++trkIter;
      }
    }
    theVxContainer->push_back(cand);
  }
  
  if (m_selectiontype==1&&m_testingclass!=0) delete m_testingclass;


  std::vector<Trk::TrackToVtxLink*>::iterator begin=myTrackToVtxLinks.begin();
  std::vector<Trk::TrackToVtxLink*>::iterator end=myTrackToVtxLinks.end();

  //delete all TrackToVtxLink objects
  for (std::vector<Trk::TrackToVtxLink*>::iterator iterator=begin;iterator!=end;++iterator ) {
    delete *iterator;
  }

  //---- add dummy vertex at the end ------------------------------------------------------//
  //---- if one or more vertices are already there: let dummy have same position as primary vertex
  if ( theVxContainer->size() >= 1 )
  {
    Trk::VxCandidate * primaryVtx = theVxContainer->front();
    if (primaryVtx->vxTrackAtVertex()->size()>0)
    {
      primaryVtx->setVertexType(Trk::PriVtx);
      Trk::VxCandidate * dummyVxCandidate = new Trk::VxCandidate ( primaryVtx->recVertex(),
                                                                   std::vector<Trk::VxTrackAtVertex*>());
      dummyVxCandidate->setVertexType(Trk::NoVtx);
      theVxContainer->push_back ( dummyVxCandidate );
    }
    else
    {
      primaryVtx->setVertexType(Trk::NoVtx);
    }
    
  }
  //---- if no vertex is there let dummy be at beam spot
  else if ( theVxContainer->size() == 0 )
  {
    Trk::VxCandidate * dummyVxCandidate = new Trk::VxCandidate ( m_iBeamCondSvc->beamVtx(),
                                                                 std::vector<Trk::VxTrackAtVertex*>());
    dummyVxCandidate->setVertexType(Trk::NoVtx);
    theVxContainer->push_back ( dummyVxCandidate );
  }
  
  // loop over the pile up to set it as pile up (EXCLUDE first and last vertex: loop from 1 to size-1)
  for (unsigned int i = 0 ; i < theVxContainer->size()-1 ; i++)
  {
    if (i>0)
    {
      (*theVxContainer)[i]->setVertexType(Trk::PileUp);
    }
#ifdef MULTIFINDER_DEBUG
    std::cout << "z position: " << (*theVxContainer)[i]->recVertex().position().z() << " ntracks: " << (*theVxContainer)[i]->vxTrackAtVertex()->size() << std::endl;
#endif
   
  }
  return theVxContainer;
  
}

 double InDetAdaptiveMultiPriVxFinderTool::estimateSignalCompatibility(Trk::VxCandidate *mycand) 
 {

     
  std::vector<Trk::VxTrackAtVertex*>* tracks = mycand->vxTrackAtVertex();
  std::vector<Trk::VxTrackAtVertex*>::iterator begintracks=tracks->begin();
  std::vector<Trk::VxTrackAtVertex*>::iterator endtracks=tracks->end();

  if (m_selectiontype==0) 
  {
  
//use just sum over pt squared
//first get all the variables you need for to discriminate between signal and minimum bias event

    double total_pt_squared=0;
    int total_num_tracks=0;

    for(std::vector<Trk::VxTrackAtVertex*>::iterator i = begintracks; i!=endtracks; i++) {

#ifdef MULTIFINDER_DEBUG
      std::cout << " compatibility is: " << static_cast<Trk::MVFVxTrackAtVertex*>(*i)->vtxCompatibility() << std::endl;
      std::cout << " c2 compatibility is: " << static_cast<Trk::MVFVxTrackAtVertex*>(*i)->trackQuality().chiSquared() << std::endl;
#endif

      if ( ( static_cast<Trk::MVFVxTrackAtVertex*>(*i)->vtxCompatibility()<m_finalCutMaxVertexChi2&&m_useFastCompatibility)||
	   ( static_cast<Trk::MVFVxTrackAtVertex*>(*i)->weight()>m_minweight 
	     && static_cast<Trk::MVFVxTrackAtVertex*>(*i)->trackQuality().chiSquared()<m_finalCutMaxVertexChi2 
	     && !m_useFastCompatibility ) ) {
        const Trk::TrackParameters* perigee(0);
        if ((*i)->perigeeAtVertex()!=0) {
          perigee=(*i)->perigeeAtVertex();
        } else {
#ifdef MULTIFINDER_DEBUG
	  std::cout << " only initialPerigee is available" << std::endl;
#endif
          perigee=(*i)->initialPerigee();
        }
	if (perigee==0) {
	  if (msgLvl(MSG::ERROR)) msg() << "Neutrals not supported. Skipping track in pT calculation..." << endreq;
	  continue;
	}
        total_pt_squared+=std::pow(std::fabs(1./perigee->parameters()[Trk::qOverP])*sin(perigee->parameters()[Trk::theta]),2);
        total_num_tracks+=1;
      }
    }//finishing iterating on VxTrackAtVertex associated to **vtxIter VxCandidate
    
//    mycand->setPt(total_pt_squared*std::sqrt((double)total_num_tracks));

    return  total_pt_squared*std::sqrt((double)total_num_tracks);
  } else if (m_selectiontype==1) {//use NN
    double pt_track1=0.;
    double pt_track2=0.;
    double pt_track3=0.;
    double pt_sum_linear=0.;
    double pt_sum_quadratic=0.;

    int total_num_tracks=0;
    int prognumber=0;

    for(std::vector<Trk::VxTrackAtVertex*>::iterator i = begintracks; i!=endtracks; i++) {
      if ( ( static_cast<Trk::MVFVxTrackAtVertex*>(*i)->vtxCompatibility()<m_finalCutMaxVertexChi2&&m_useFastCompatibility)||
	   ( static_cast<Trk::MVFVxTrackAtVertex*>(*i)->weight()>m_minweight 
	     && static_cast<Trk::MVFVxTrackAtVertex*>(*i)->trackQuality().chiSquared()<m_finalCutMaxVertexChi2 
	     && !m_useFastCompatibility ) ) {
        const Trk::TrackParameters* perigee(0);
        if ((*i)->perigeeAtVertex()!=0) {
          perigee=(*i)->perigeeAtVertex();
        } else {
#ifdef MULTIFINDER_DEBUG
	  std::cout << " only initialPerigee is available" << std::endl;
#endif
          perigee=(*i)->initialPerigee();
        }
	if (perigee==0) {
	  if (msgLvl(MSG::ERROR)) msg() << "Neutrals not supported. Skipping track in pT calculation..." << endreq;
	  continue;
	}
        double actualpt(std::fabs(1./perigee->parameters()[Trk::qOverP])*sin(perigee->parameters()[Trk::theta]));
        pt_sum_quadratic+=std::pow(actualpt,2);
        pt_sum_linear+=actualpt;
        if (prognumber==0) {
	  pt_track1=actualpt;
          prognumber+=1;
        } else if (prognumber==1) {
          pt_track2=actualpt;
          prognumber+=1;
        } else if (prognumber==2) {
          pt_track3=actualpt;
          prognumber+=1;
        }
        total_num_tracks+=1;
      }
    }
    if (total_num_tracks==0||pt_track2==0||pt_track3==0) {
//      mycand->setPt(0.);
      return 0.;
    } else {
//      mycand->setPt(m_testingclass->value(0,pt_track1,pt_track2,pt_track3,
//					  pt_sum_linear,pt_sum_quadratic,
//					  total_num_tracks));

      return m_testingclass->value(0,pt_track1,pt_track2,pt_track3,
					  pt_sum_linear,pt_sum_quadratic,
					  total_num_tracks);
    }
  }
  return 0;
}




  StatusCode InDetAdaptiveMultiPriVxFinderTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  void InDetAdaptiveMultiPriVxFinderTool::m_printParameterSettings()
  {
    msg(MSG::INFO) << "VxPrimary initialize(): Parametersettings " << endreq;
 
    msg(MSG::INFO) << "" << endreq;
    msg(MSG::INFO) << "Trackselection cuts handled by the TrackSelectorTool: " << m_trkFilter << endreq;
    msg(MSG::INFO) << "Finder settings: " << endreq;
    msg(MSG::INFO) << "Maximum distance between simultaneously fitted vertices: TracksMaxZinterval " << m_TracksMaxZinterval << endreq;
    msg(MSG::INFO) << "Seeding: minimum weight for a track being an outlier:  maxVertexChi2 " << m_maxVertexChi2 << endreq;
    msg(MSG::INFO) << "Signal identification: final cut on track chi2: finalCutMaxVertexChi2 = " << m_finalCutMaxVertexChi2 << endreq;
    msg(MSG::INFO) << "Activate complete multi vertex fitting feature: realMultiVertex " << m_realMultiVertex << endreq;
    msg(MSG::INFO) << "Merging vertices: upper cut on significance to merge two vertices: cutVertexDependence = " << m_cutVertexDependence << endreq;
    msg(MSG::INFO) << "Maximum number of iterations: maxIterations = " << m_maxIterations << endreq;
    msg(MSG::INFO) << "Selection type (0 is sqrt(Ntr)*Sum_{tr} pT^2): selectiontype = " << m_selectiontype << endreq;
    msg(MSG::INFO) << "Use fast compatibility (if false use refitted chi2 instead of approximation): useFastCompatibility = "<<  m_useFastCompatibility << endreq;
    msg(MSG::INFO) << "MinWeight (if track weight in the fit is lower, don't perform the Kalman Update) = " << m_minweight << endreq;

    msg(MSG::INFO) << "" << endreq;
}

void InDetAdaptiveMultiPriVxFinderTool::m_SGError(std::string errService)
{
    msg(MSG::FATAL) << errService << " not found. Exiting !" << endreq;
    return;
}

/* COMMENTED OUT
double InDetAdaptiveMultiPriVxFinderTool::estimateDeltaZ(const Trk::TrackParameters& myPerigee, const Trk::Vertex& myTransvVertex)
{

  Amg::Vector3D lp=myTransvVertex.position();

  Amg::Vector3D expPoint(3,0);
  Trk::GlobalPosition predStatePosition = myPerigee.position();
  //extrapolatedPerigee->position();
  expPoint[0] = predStatePosition.x();
  expPoint[1] = predStatePosition.y();
  expPoint[2] = predStatePosition.z(); 
  
  //phi_v and functions  
  double phi_v = myPerigee.parameters()[Trk::phi];
  double sin_phi_v = sin(phi_v);
  double cos_phi_v = cos(phi_v);
  
  //q over p  
  double q_ov_p = myPerigee.parameters()[Trk::qOverP];
  int sgn_h = (q_ov_p<0.)? -1:1;
       
  //theta and functions  
  double th = myPerigee.parameters()[Trk::theta];
  double sin_th = sin(th);
  double tan_th = tan(th);
  
  //momentum
  Amg::Vector3D expMomentum(3,0);
  expMomentum[0] = phi_v;
  expMomentum[1] = th;
  expMomentum[2] = q_ov_p;
          
  double B_z=m_zBfieldApprox;
  
  //signed radius and rotation variables    
  double rho =  sin_th / (q_ov_p * B_z);  
  double X = expPoint[0] - lp.x() + rho*sin_phi_v;
  double Y = expPoint[1] - lp.y() - rho*cos_phi_v;
  double SS = (X * X + Y * Y);
  double S = sqrt(SS);
  
  //calculated parameters at expansion point

  //calculation of phi at expansion point    
  double phiAtEp;
  int sgnY = (Y<0)? -1:1;
  int sgnX = (X<0)? -1:1;  
  double m_pi = TMath::Pi();//acos(-1.);
  
  if(fabs(X)>fabs(Y)) phiAtEp = sgn_h*sgnX* acos(-sgn_h * Y / S);
  else
  {
    phiAtEp = asin(sgn_h * X / S);    
    if( (sgn_h * sgnY)> 0) phiAtEp =  sgn_h * sgnX * m_pi - phiAtEp;
  }

//  std::cout << " DeltaZ: " << expPoint[2] - lp.z() << std::endl;

//  std::cout << " DeltaZ (PCA): " << expPoint[2] - lp.z() + rho*(phi_v - phiAtEp)/tan_th << std::endl;

  return expPoint[2] - lp.z() + rho*(phi_v - phiAtEp)/tan_th;    
 


}
*/

double InDetAdaptiveMultiPriVxFinderTool::estimateDeltaZ(const Trk::TrackParameters& myPerigee, const Trk::Vertex& myTransvVertex)
{

  Amg::Vector3D lp=myTransvVertex.position();
  
  Amg::Vector3D expPoint;
  Amg::Vector3D predStatePosition = myPerigee.position();
  expPoint[0] = predStatePosition.x();
  expPoint[1] = predStatePosition.y();
  expPoint[2] = predStatePosition.z(); 
  
  //phi_v and functions  
  double phi_v = myPerigee.parameters()[Trk::phi];
  double th = myPerigee.parameters()[Trk::theta];
  double tan_th = tan(th);
//  double sin_th = sin(th);
  double sin_phi_v = sin(phi_v);
  double cos_phi_v = cos(phi_v);
  double q_ov_p = myPerigee.parameters()[Trk::qOverP];

  //momentum
  Amg::Vector3D expMomentum;
  expMomentum[0] = phi_v;
  expMomentum[1] = th;
  expMomentum[2] = q_ov_p;
  
  double X = expPoint[0] - lp.x();
  double Y = expPoint[1] - lp.y();
  
//  std::cout << " Z: " << expPoint[2] << std::endl;
//  std::cout << " Z (PCA) Diff: " << expPoint[2] - lp.z() - 1./tan_th*(X*cos_phi_v+Y*sin_phi_v) << std::endl;
  
//  std::cout << " D0: " << myPerigee.parameters()[Trk::d0] << std::endl;
//  std::cout << " D0 (PCA): " << Y*cos_phi_v-X*sin_phi_v  << std::endl;
  
  return expPoint[2] - lp.z() - 1./tan_th*(X*cos_phi_v+Y*sin_phi_v);

}
  

  
} // end namespace InDet


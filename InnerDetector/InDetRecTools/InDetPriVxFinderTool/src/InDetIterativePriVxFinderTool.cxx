/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetIterativePriVxFinderTool.cxx  -  Description
                             -------------------
    begin   : 23-02-2010
    authors : Giacinto Piacquadio (CERN PH-ADE-ID)

 ***************************************************************************/
#include "InDetPriVxFinderTool/InDetIterativePriVxFinderTool.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "VxVertex/VxCandidate.h"
#include <map>
#include <vector>
#include <utility>
#include "TrkSurfaces/PlaneSurface.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h"
//#include <TMath.h>

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
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"

//#define INDETADAPTIVEPRIVXFINDERTOOL_DEBUG



namespace InDet
{

InDetIterativePriVxFinderTool::InDetIterativePriVxFinderTool(const std::string& t, const std::string& n, const IInterface*  p)
        : AthAlgTool(t,n,p),
          m_iVertexFitter("Trk::AdaptiveVertexFitter"),
	  m_trkFilter("InDet::InDetTrackSelection"),
          m_SeedFinder("Trk::ZScanSeedFinder"),
//          m_distFinder("Trk::SeedNewtonTrkDistanceFinder"),
//          m_TrackCompatibilityEstimator("Trk::Chi2TrackCompatibilityEstimator"),
//          m_ImpactPoint3dAtaPlaneFactory("Trk::ImpactPoint3dAtaPlaneFactory"),
//          m_ImpactPoint3dAtaPlaneFactory("Trk::ImpactPoint3dAtaPlaneFactory"),
          m_ImpactPoint3dEstimator("Trk::ImpactPoint3dEstimator"),
	  m_VertexEdmFactory("Trk::VertexInternalEdmFactory"),
          m_iBeamCondSvc("BeamCondSvc",n),
          m_useBeamConstraint(false),
          m_significanceCutSeeding(10),
          m_maximumChi2cutForSeeding(6.*6.),
          m_maxVertices(25),
          m_createSplitVertices(false),
	  m_splitVerticesTrkInvFraction(2),
          m_reassignTracksAfterFirstFit(false),
	  m_doMaxTracksCut(false), 
	  m_maxTracks(5000)
{
    declareInterface<IVertexFinder>(this);

    declareProperty("VertexFitterTool", m_iVertexFitter);
    declareProperty("TrackSelector",m_trkFilter);
    declareProperty("BeamPositionSvc", m_iBeamCondSvc);
    declareProperty("SeedFinder"       , m_SeedFinder);
//    declareProperty("DistanceFinder",m_distFinder);
//    declareProperty("TrackCompatibilityEstimator",m_TrackCompatibilityEstimator);
//    declareProperty("ImpactPoint3dAtaPlaneFactory",m_ImpactPoint3dAtaPlaneFactory);
    declareProperty("ImpactPoint3dEstimator",m_ImpactPoint3dEstimator);
    declareProperty("LinearizedTrackFactory",m_LinearizedTrackFactory);
    declareProperty("InternalEdmFactory", m_VertexEdmFactory);

    declareProperty("useBeamConstraint",m_useBeamConstraint);
    declareProperty("significanceCutSeeding",m_significanceCutSeeding);
    declareProperty("maximumChi2cutForSeeding",m_maximumChi2cutForSeeding);
    declareProperty("maxVertices",m_maxVertices);
    declareProperty("createSplitVertices",m_createSplitVertices);
    declareProperty("splitVerticesTrkInvFraction", m_splitVerticesTrkInvFraction, "inverse fraction to split tracks (1:N)");
    declareProperty("reassignTracksAfterFirstFit",m_reassignTracksAfterFirstFit);
    declareProperty( "doMaxTracksCut", m_doMaxTracksCut); 
    declareProperty( "MaxTracks",  m_maxTracks);
}

InDetIterativePriVxFinderTool::~InDetIterativePriVxFinderTool()
{}

StatusCode InDetIterativePriVxFinderTool::initialize()
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
    /*
    if ( m_distFinder.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_distFinder << endreq;
      return StatusCode::FAILURE;
    }
    */
    if ( m_LinearizedTrackFactory.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_LinearizedTrackFactory << endreq;
      return StatusCode::FAILURE;
    }

    if ( m_ImpactPoint3dEstimator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_ImpactPoint3dEstimator << endreq;
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
    /*
     if ( m_TrackCompatibilityEstimator.retrieve().isFailure() ) {
       msg(MSG::ERROR) << "Failed to retrieve tool " << m_TrackCompatibilityEstimator << endreq;
       return StatusCode::FAILURE;
     } else {
       msg(MSG::INFO) << "Retrieved tool " << m_TrackCompatibilityEstimator << endreq;
     }
     
     if ( m_ImpactPoint3dAtaPlaneFactory.retrieve().isFailure() ) {
       msg(MSG::ERROR) << "Failed to retrieve tool " << m_ImpactPoint3dAtaPlaneFactory << endreq;
       return StatusCode::FAILURE;
     } else {
       msg(MSG::INFO) << "Retrieved tool " << m_ImpactPoint3dAtaPlaneFactory << endreq;
     }
    */

    if ( m_VertexEdmFactory.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrievel tool " << m_VertexEdmFactory);
      return StatusCode::FAILURE;
    }

    // since some parameters special to an inherited class this method
    // will be overloaded by the inherited class
    m_printParameterSettings();

    msg(MSG::INFO) << "Initialization successful" << endreq;
    return StatusCode::SUCCESS;
}

  namespace
  {
    
    struct VxCandidates_pair
    {
      double first;
      Trk::VxCandidate* second;
      VxCandidates_pair(double p1, Trk::VxCandidate* p2)
              : first (p1), second (p2) {}
      bool operator< (const VxCandidates_pair& other) const
            { return first > other.first; }
    };
  } //anonymous namespace
  
  
VxContainer* InDetIterativePriVxFinderTool::findVertex(const TrackCollection* trackTES) 
{

  if(msgLvl(MSG::DEBUG)) msg() << " Number of input tracks before track selection: " << trackTES->size() << endreq;

  Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx());

  std::vector<Trk::ITrackLink*> selectedTracks;

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
  
  return returnContainer;

}

VxContainer* InDetIterativePriVxFinderTool::findVertex(const Trk::TrackParticleBaseCollection* trackTES) {
 
  if(msgLvl(MSG::DEBUG)) msg() << " Number of input tracks before track selection: " << trackTES->size() << endreq;

  std::vector<Trk::ITrackLink*> selectedTracks;

  Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx());

  typedef DataVector<Trk::TrackParticleBase>::const_iterator TrackParticleDataVecIter;

  Root::TAccept selectionPassed;
  for (TrackParticleDataVecIter itr  = (*trackTES).begin(); itr != (*trackTES).end(); itr++) {
    if (m_useBeamConstraint) {
      selectionPassed=m_trkFilter->accept(*((*itr)->originalTrack()), &beamposition);
    }
    else
    {
      Trk::Vertex null(Amg::Vector3D(0,0,0));
      selectionPassed=m_trkFilter->accept(*((*itr)->originalTrack()), &null);
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

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetIterativePriVxFinderTool::findVertex(const xAOD::TrackParticleContainer* trackParticles)
{
  ATH_MSG_DEBUG(" Number of input tracks before track selection: " << trackParticles->size());

  std::vector<Trk::ITrackLink*> selectedTracks;

  xAOD::Vertex beamposition;
  beamposition.makePrivateStore();
  beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
  beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());

  typedef DataVector<xAOD::TrackParticle>::const_iterator TrackParticleDataVecIter;

  Root::TAccept selectionPassed;
  for (TrackParticleDataVecIter itr  = trackParticles->begin(); itr != trackParticles->end(); ++itr) {
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

VxContainer* InDetIterativePriVxFinderTool::findVertex(const std::vector<Trk::ITrackLink*> & trackVector) const
{
  


  
  //two things need to be added
  //1) the seeding mechanism
  //2) the iterative removal of tracks
  
  std::vector<Trk::ITrackLink*> origTracks=trackVector;
  std::vector<Trk::ITrackLink*> seedTracks=trackVector;
  
  std::vector<VxCandidates_pair> myVxCandidates;
  
  VxContainer* theVxContainer = new VxContainer;
  
  //bail out early with only Dummy vertex if multiplicity cut is applied and exceeded
  if (m_doMaxTracksCut && (trackVector.size() > m_maxTracks)){ 
    if (msgLvl(MSG::WARNING)) msg() << trackVector.size() << " tracks - exceeds maximum (" << m_maxTracks << "), skipping vertexing and returning only dummy..." << endreq; 
    Trk::VxCandidate * dummyVxCandidate = new Trk::VxCandidate(m_iBeamCondSvc->beamVtx(),std::vector<Trk::VxTrackAtVertex*>());
    dummyVxCandidate->setVertexType(Trk::NoVtx);
    theVxContainer->push_back ( dummyVxCandidate );
    return theVxContainer;
  } 
 
  int iterations=-1;
  unsigned int seedtracknumber=seedTracks.size();
  
  //used to store seed info
  Trk::Vertex actualVertex;

  //prepare iterators for tracks only necessary for seeding
  std::vector<Trk::ITrackLink*>::iterator seedBegin;
  std::vector<Trk::ITrackLink*>::iterator seedEnd;

  
  do
  {
    
    seedBegin=seedTracks.begin();
    seedEnd=seedTracks.end();

    if (seedtracknumber==0)
    {
      if(msgLvl(MSG::DEBUG)) msg() << " New iteration. No tracks available after track selection for seeding. No finding done." << endreq;
      break;
    }
    
    iterations+=1;
    if(msgLvl(MSG::DEBUG)) msg() << "ITERATION NUMBER " << iterations << endreq;
    
    //now find a new SEED
    
    std::vector<const Trk::TrackParameters*> perigeeList;
    for (std::vector<Trk::ITrackLink*>::iterator seedtrkAtVtxIter=seedBegin;
         seedtrkAtVtxIter!=seedEnd;++seedtrkAtVtxIter) {
      perigeeList.push_back((*seedtrkAtVtxIter)->parameters());
    }
    
    Trk::RecVertex theconstraint;
    if (m_useBeamConstraint) {
      theconstraint=m_iBeamCondSvc->beamVtx();
      actualVertex=m_SeedFinder->findSeed(perigeeList,&theconstraint);
    } else {
      actualVertex=m_SeedFinder->findSeed(perigeeList);
      Amg::MatrixX looseConstraintCovariance(3,3);
      looseConstraintCovariance.setIdentity();
      looseConstraintCovariance = looseConstraintCovariance * 1e+8;
      theconstraint=Trk::RecVertex(actualVertex.position(),looseConstraintCovariance,-3.,0.);
    }
    
    if (msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG) << " seed at x: " << actualVertex.position().x() << 
          " at y: " <<      actualVertex.position().y() << 
          " at z: " <<      actualVertex.position().z() << endreq;
    }
    
    if (actualVertex.position().z()==0.) {
      if (msgLvl(MSG::DEBUG))
      {
        msg(MSG::DEBUG) << "No seed found: no further vertices in event" << endreq;
        msg(MSG::DEBUG) << "Number of input tracks: " << perigeeList.size() << " but no seed returned." << endreq;
      }
      break;
    }
    
    //now question (remove tracks which are too far away??? I think so...)
    std::vector<const Trk::TrackParameters*> perigeesToFit;
    std::vector<const Trk::TrackParameters*> perigeesToFitSplitVertex;
    
    int numberOfTracks(perigeeList.size());
    
    std::vector<const Trk::TrackParameters*>::const_iterator perigeeListBegin=perigeeList.begin();
    std::vector<const Trk::TrackParameters*>::const_iterator perigeeListEnd=perigeeList.end();
    
    int counter=0;

    for (std::vector<const Trk::TrackParameters*>::const_iterator perigeeListIter=perigeeListBegin;
         perigeeListIter!=perigeeListEnd;++perigeeListIter)
    {
      if (numberOfTracks<=2)
      {
        perigeesToFit.push_back(*perigeeListIter);
        counter+=1;
      }
      else if (numberOfTracks<=4 && !m_createSplitVertices)
      {
        perigeesToFit.push_back(*perigeeListIter);
        counter+=1;
      } 
      else if (numberOfTracks<=4*m_splitVerticesTrkInvFraction && m_createSplitVertices)
      {
	// few tracks are left, put them into the fit regardless their position!
        if (counter % m_splitVerticesTrkInvFraction == 0)
        {
          perigeesToFit.push_back(*perigeeListIter);
          counter+=1;
        }
        else
        {
          perigeesToFitSplitVertex.push_back(*perigeeListIter);
          counter+=1;
        }
      }
      else
      { //check first whether it is not too far away!
        
        bool isOK=false;
        double distance=0.;
        try
        {
          Trk::PlaneSurface* mySurface=m_ImpactPoint3dEstimator->Estimate3dIP(*perigeeListIter,&actualVertex);
          delete mySurface;
          isOK=true;
        }
        catch (error::ImpactPoint3dEstimatorProblem err)
        {
          msg(MSG::WARNING) << " ImpactPoin3dEstimator failed to find minimum distance between track and vertex seed: " << err.p << endreq;
        }
        
        
        if (isOK)
        {
          distance=m_ImpactPoint3dEstimator->getDistance();
        }
        
        if (distance<0)
        {
          msg(MSG::WARNING) << " Distance between track and seed vtx is negative: " << distance << endreq;
        }
        
        
        const Trk::TrackParameters* myPerigee=(*perigeeListIter);
        
      //very approximate error
        double error= 0.;
	
	if(myPerigee && myPerigee->covariance())
	{
	  error  =  std::sqrt((*myPerigee->covariance())(Trk::d0,Trk::d0)+
			      (*myPerigee->covariance())(Trk::z0,Trk::z0));
        }//end of the security check
	
        if (error==0.)
        {
          msg(MSG::ERROR) << " Error is zero! " << distance << endreq;
          error=1.;
        }
        
        if(msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << " Distance between track and seed vtx: " << distance << " d/s(d) = " << 
              distance/error << " err " << error << endreq;
        }
        

        if (distance/error<m_significanceCutSeeding)
        {
          if (counter % m_splitVerticesTrkInvFraction == 0 || !m_createSplitVertices)
          {
            perigeesToFit.push_back(*perigeeListIter);
            counter+=1;
          }
          else
          {
            perigeesToFitSplitVertex.push_back(*perigeeListIter);
            counter+=1;
          }
        }
      }
    }
    
    if(msgLvl(MSG::VERBOSE))
    {
      msg(MSG::VERBOSE) << " Considering n. " << perigeesToFit.size() << " tracks for the fit. " << endreq;
      if (m_createSplitVertices)
      {
        msg(MSG::VERBOSE) << " and n. " << perigeesToFitSplitVertex.size() << " tracks for split vertex fit. " << endreq;
      }
    }
    
    if (perigeesToFit.size()==0)
    {
      if(msgLvl(MSG::DEBUG))
      {
        msg(MSG::DEBUG) << " No good seed found. Exiting search for vertices..." << endreq;
      }
      
      break;
    }
    

    //now you have perigeeToFit and perigeeToFitSplitVertices
    //AND HERE YOU DO THE FIT
    //to reassign vertices you look ino what is already in myVxCandidate
    //you do it only ONCE!

    Trk::VxCandidate * myVxCandidate = 0;
    Trk::VxCandidate * myVxCandidateSplitVertex = 0;

    if (m_useBeamConstraint && perigeesToFit.size()>0)
    {
       myVxCandidate=m_iVertexFitter->fit(perigeesToFit, theconstraint);
    }
    else if (!m_useBeamConstraint && perigeesToFit.size()>1)
    {
      myVxCandidate=m_iVertexFitter->fit(perigeesToFit);
    }
    if (m_createSplitVertices && perigeesToFitSplitVertex.size()>1)
    {
      myVxCandidateSplitVertex=m_iVertexFitter->fit(perigeesToFitSplitVertex);
    }

    double ndf = 0.;
    int ntracks = 0;
    countTracksAndNdf(myVxCandidate,ndf,ntracks);

    double ndfSplitVertex = 0.;
    int ntracksSplitVertex  = 0;
    countTracksAndNdf(myVxCandidateSplitVertex,ndfSplitVertex,ntracksSplitVertex);

    bool goodVertex = 
        myVxCandidate != 0 &&
        ((!m_useBeamConstraint && ndf>0 && ntracks>=2) ||
         (m_useBeamConstraint && ndf>3 && ntracks>=2));

    if (msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG) << " VxCandidate is pointer: " << myVxCandidate << 
          " ndf = " << ndf << " ntracks (with weight>0.01) " << ntracks << " beam constraint " << (m_useBeamConstraint?"yes":"no") << endreq;
    }

    if (!goodVertex)
    {

      removeAllFrom(perigeesToFit,seedTracks);

      if (msgLvl(MSG::DEBUG))
      {
        msg(MSG::DEBUG) << " Going to new iteration with: " << seedTracks.size() << " seed tracks after BAD VERTEX. " << endreq;
      }     
    }
    else
    {

      if (m_reassignTracksAfterFirstFit && (!m_createSplitVertices))
      {

        
        if(msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << " N tracks used for fit before reallocating: "  << perigeesToFit.size() << endreq;
        }
        //now you HAVE a good vertex
        //but you want to add the tracks which you missed...
        
        int numberOfAddedTracks=0;

        //iterate on remaining vertices and cross-check if tracks need to be attached 
        //to new vertex
        VxContainer::iterator vxBegin=theVxContainer->begin();
        VxContainer::iterator vxEnd=theVxContainer->end();
        
        for (VxContainer::iterator vxIter=vxBegin;vxIter!=vxEnd;++vxIter)
        {
          
          //now iterate on tracks at vertex
          
          std::vector<Trk::VxTrackAtVertex*>* myVxTracksAtVtx=(*vxIter)->vxTrackAtVertex();
          
          if (!myVxTracksAtVtx) continue;
    
          std::vector<Trk::VxTrackAtVertex*>::iterator tracksBegin=myVxTracksAtVtx->begin();
          std::vector<Trk::VxTrackAtVertex*>::iterator tracksEnd=myVxTracksAtVtx->end();

          if(msgLvl(MSG::VERBOSE))
          {
            msg(MSG::VERBOSE) << " Iterating over new vertex to look for tracks to reallocate... "  << endreq;
          }
          
          for (std::vector<Trk::VxTrackAtVertex*>::iterator tracksIter=tracksBegin;
               tracksIter!=tracksEnd;)
          {
            
            //only try with tracks which are not too tightly assigned to another vertex
            if ((*tracksIter)->weight()>0.01)
            {
              ++tracksIter;
              continue;
            }
            
            const Trk::TrackParameters* trackPerigee=(*tracksIter)->initialPerigee();
                        
            if (trackPerigee==0)
            {
              msg(MSG::ERROR) << " Cast to perigee gives 0 pointer " << endreq;
            }
            
            double chi2_newvtx=compatibility(*trackPerigee,myVxCandidate->recVertex());
            double chi2_oldvtx=compatibility(*trackPerigee,(*vxIter)->recVertex());

            if (msgLvl(MSG::VERBOSE))
            {
              msg(MSG::VERBOSE) << "Compatibility to old vtx is : " << chi2_oldvtx << 
                  " to new vtx is: " << chi2_newvtx << endreq;
            }

            bool remove=false;

            if (chi2_newvtx<chi2_oldvtx)
            {
              
              if(msgLvl(MSG::DEBUG))
              {
                msg(MSG::DEBUG) << " Found track of old vertex (chi2= " << chi2_oldvtx << 
                    ") more compatible to new one (chi2= " << chi2_newvtx << ")" << endreq;
              }
              
              perigeesToFit.push_back(trackPerigee);
              //but you need to re-add it to the seedTracks too...

              bool isFound=false;

              std::vector<Trk::ITrackLink*>::iterator  origBegin=origTracks.begin();
              std::vector<Trk::ITrackLink*>::iterator  origEnd=origTracks.end();
              
              for (std::vector<Trk::ITrackLink*>::iterator origIter=origBegin;
                   origIter!=origEnd;++origIter)
              {
                if ((*origIter)->parameters()==trackPerigee)
                {
                  if (msgLvl(MSG::VERBOSE))
                  {
                    msg(MSG::VERBOSE) << " found the old perigee to be re-added to seedTracks in order to be deleted again!" << endreq;
                  }
                  isFound=true;
                  seedTracks.push_back(*origIter);
                  break;
                }
              }

              if (!isFound)
              {
                msg(MSG::WARNING) << " Cannot find old perigee to re-add back to seed tracks... " << endreq;
              }             
              
              numberOfAddedTracks+=1;

              remove=true;
            }
            

            if (remove)
            {

              //now you have to delete the track from the old vertex...
              //easy???
              tracksIter=myVxTracksAtVtx->erase(tracksIter);
              tracksBegin=myVxTracksAtVtx->begin();
              tracksEnd=myVxTracksAtVtx->end();
            }
            else
            {
              ++tracksIter;
            }
          }//end of iterating on tracks
          
        }//end of iterating on already found vertices in event

        if(msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << " N tracks used for fit after reallocating: "  << perigeesToFit.size() << endreq;
        }
        if(msgLvl(MSG::DEBUG))
        {
          if (numberOfAddedTracks>0)
          {
            msg(MSG::DEBUG) << "Added : " << numberOfAddedTracks << " from different vertices " << endreq;
          }
        }

        //now you have to delete the previous VxCandidate, do a new fit, then check if 
        //you still have a good vertex

        if (numberOfAddedTracks>0)
        {
          
          delete myVxCandidate;
          myVxCandidate=0;
          
          if (m_useBeamConstraint && perigeesToFit.size()>0)
          {
            myVxCandidate=m_iVertexFitter->fit(perigeesToFit, theconstraint);
          }
          else if (!m_useBeamConstraint && perigeesToFit.size()>1)
          {
            myVxCandidate=m_iVertexFitter->fit(perigeesToFit);
          }        
          
          ndf = 0.;
          ntracks = 0;
          countTracksAndNdf(myVxCandidate,ndf,ntracks);
          
          goodVertex = 
              myVxCandidate != 0 &&
              ((!m_useBeamConstraint && ndf>0 && ntracks>=2) ||
               (m_useBeamConstraint && ndf>3 && ntracks>=2));
          
          if (msgLvl(MSG::DEBUG))
          {
            msg(MSG::DEBUG) << " Refitted VxCandidate is pointer: " << myVxCandidate << 
                " ndf = " << ndf << " ntracks (with weight>0.01) " << ntracks << " beam constraint " << (m_useBeamConstraint?"yes":"no") << endreq;
          }

          if (!goodVertex)
          {
            
            removeAllFrom(perigeesToFit,seedTracks);
            
            if (msgLvl(MSG::DEBUG))
            {
              msg(MSG::DEBUG) << " Going to new iteration with: " << seedTracks.size() << " seed tracks after BAD VERTEX. " << endreq;
            }     
            
            msg(MSG::DEBUG) << " Adding tracks resulted in an invalid vertex. Should be rare. " << endreq;
          }
        }//end if tracks were added...

      }//end reassign tracks from previous vertices and refitting if needed


      //need to re-ask goodVertex since it can be changed in the mean time
      if (goodVertex)
      {
        if (msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << "Number of seeds before: " << seedTracks.size() << endreq;
        }
        
        removeCompatibleTracks(myVxCandidate,
                               perigeesToFit,
                               seedTracks);
        
        if (msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << "Number of seeds after removal of outliers: " << seedTracks.size() << endreq;
        }
      }
      
      
    }//end else if not good Vertex
    

    

    bool goodVertexSplitVertex=false;
    

    if (m_createSplitVertices)
    {
    
      goodVertexSplitVertex = 
          myVxCandidateSplitVertex != 0 &&
          ndfSplitVertex>0 && ntracksSplitVertex>=2;
    
      
      if (msgLvl(MSG::DEBUG))
      {
        msg(MSG::DEBUG) << " VxCandidateSplitVertex is pointer: " << myVxCandidateSplitVertex << 
            " ndf = " << ndfSplitVertex << " ntracks (with weight>0.01) " << ntracksSplitVertex << endreq;
      }
      
      if (!goodVertexSplitVertex)
      {
        
        removeAllFrom(perigeesToFitSplitVertex,seedTracks);
        
        if (msgLvl(MSG::DEBUG))
        {
          msg(MSG::DEBUG) << " SPLIT VERTEX Going to new iteration with: " << seedTracks.size() << " seed tracks after BAD Split VERTEX. " << endreq;
        }     
      }
      else
      {
        if (msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << "SPLIT VERTEX Number of seeds before: " << seedTracks.size() << endreq;
        }
        
        removeCompatibleTracks(myVxCandidateSplitVertex,
                               perigeesToFitSplitVertex,
                               seedTracks);
        
        if (msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << "SPLIT VERTEX Number of seeds after removal of outliers: " << seedTracks.size() << endreq;
        }
        
      }//end else if not good Vertex
    }//end if create split vertices
    

    if (!m_createSplitVertices)
    {
      if (goodVertex)
      {
        theVxContainer->push_back (myVxCandidate);
      }
      else
      {
        if (myVxCandidate)
        {
          delete myVxCandidate;
          myVxCandidate=0;
        }
      }
    }
    else
    {
      if (goodVertex)
      {
        // type does not seem to be set earlier
        myVxCandidate->setVertexType(Trk::PriVtx);
        theVxContainer->push_back (myVxCandidate);
      } else
      {
        if (myVxCandidate)
        {
          delete myVxCandidate;
          myVxCandidate=0;
        }
        Trk::VxCandidate * dummyVxCandidate = new Trk::VxCandidate (
            m_iBeamCondSvc->beamVtx(),
        std::vector<Trk::VxTrackAtVertex*>()
                                                                   );
        dummyVxCandidate->setVertexType(Trk::NoVtx);
        theVxContainer->push_back ( dummyVxCandidate );
      }
      if (goodVertexSplitVertex)
      {
        // type does not seem to be set earlier
        myVxCandidateSplitVertex->setVertexType(Trk::PriVtx);
        theVxContainer->push_back (myVxCandidateSplitVertex);
      } else
      {
        if (myVxCandidateSplitVertex)
        {
          delete myVxCandidateSplitVertex;
          myVxCandidateSplitVertex=0;
        }
        Trk::VxCandidate * dummyVxCandidate = new Trk::VxCandidate (
            m_iBeamCondSvc->beamVtx(),
        std::vector<Trk::VxTrackAtVertex*>()
                                                                   );
        dummyVxCandidate->setVertexType(Trk::NoVtx);
        theVxContainer->push_back ( dummyVxCandidate );
      }
    }
  }
  while (seedTracks.size()>1 && iterations<m_maxVertices);

  if (iterations>=m_maxVertices)
  {
    msg(MSG::WARNING) << "Reached maximum iterations " << endreq;
  }
  
  //unfortunately you have still a problem with the track to links!!!
  
  //---- add dummy vertex at the end ------------------------------------------------------//
  //---- if one or more vertices are already there: let dummy have same position as primary vertex
  if (!m_createSplitVertices)
  {
    if ( theVxContainer->size() >= 1)
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
    // loop over the pile up to set it as pile up (EXCLUDE first and last vertex, do not do that in split mode)
    for (unsigned int i = 0 ; i < theVxContainer->size()-1 ; i++)
    {
      if(msgLvl(MSG::DEBUG))
      {
        msg(MSG::DEBUG) << " Vtx: " << i << 
            " x= " << (*theVxContainer)[i]->recVertex().position().x() << 
            " y= " << (*theVxContainer)[i]->recVertex().position().y() << 
            " z= " << (*theVxContainer)[i]->recVertex().position().z() << 
            " ntracks= " << (*theVxContainer)[i]->vxTrackAtVertex()->size() << 
            " chi2= " << (*theVxContainer)[i]->recVertex().fitQuality().chiSquared()
                        << " ndf = " << (*theVxContainer)[i]->recVertex().fitQuality().doubleNumberDoF() << endreq;
      }
  
      if (i>0)
      {
        (*theVxContainer)[i]->setVertexType(Trk::PileUp);
      }
    }
  }
  
  VxContainer::const_iterator vxBegin=theVxContainer->begin();
  VxContainer::const_iterator vxEnd=theVxContainer->end();
  
  //prepare iterators for tracks only necessary for seeding
  std::vector<Trk::ITrackLink*>::iterator origtrkbegin=origTracks.begin();
  std::vector<Trk::ITrackLink*>::iterator origtrkend=origTracks.end();
  
  for (VxContainer::const_iterator vxIter=vxBegin;vxIter!=vxEnd;++vxIter)
  {
    std::vector<Trk::VxTrackAtVertex*>* myVxTracksAtVtx=(*vxIter)->vxTrackAtVertex();

    if (!myVxTracksAtVtx) continue;
    
    std::vector<Trk::VxTrackAtVertex*>::iterator tracksBegin=myVxTracksAtVtx->begin();
    std::vector<Trk::VxTrackAtVertex*>::iterator tracksEnd=myVxTracksAtVtx->end();
    
    if(msgLvl(MSG::VERBOSE))
    {
      msg(MSG::VERBOSE) << " Iterating over new vertex in fixing links... "  << endreq;
    }
    
    bool found=false;
    
    for (std::vector<Trk::VxTrackAtVertex*>::iterator tracksIter=tracksBegin;
         tracksIter!=tracksEnd;++tracksIter)
    {
      if(msgLvl(MSG::VERBOSE))
      {
        msg(MSG::VERBOSE) << " Iterating over new track in fixing links... " << endreq;
      }
      
      //now look for corresponding ITrackLink
      for (std::vector<Trk::ITrackLink*>::iterator origtrkiter=origtrkbegin;
           origtrkiter!=origtrkend;++origtrkiter)
      {
        if ((*origtrkiter)->parameters()==(*tracksIter)->initialPerigee())
        {
          found=true;
          ( *tracksIter)->setOrigTrack ( *origtrkiter );
          origTracks.erase(origtrkiter);
          origtrkbegin=origTracks.begin();
          origtrkend=origTracks.end();
          break;
        }
      }
      if (!found)
      {
        msg(MSG::ERROR) << " Cannot find vector element to fix links (step 4)! " << endreq;
      }
    }//end iterate on tracks at vtx
  }//end iterate on vertices
  
  for (std::vector<Trk::ITrackLink*>::iterator origtrkiter=origtrkbegin;
       origtrkiter!=origtrkend;++origtrkiter)
  {   
    if ((*origtrkiter)!=0) {
      delete *origtrkiter;
      *origtrkiter=0;
    }
  }
  
  return theVxContainer;
}

StatusCode InDetIterativePriVxFinderTool::finalize()
{
    return StatusCode::SUCCESS;
}

void InDetIterativePriVxFinderTool::m_printParameterSettings()
{
    msg(MSG::INFO) << "VxPrimary initialize(): Parametersettings " << endreq;
    msg(MSG::INFO) << "VertexFitter " << m_iVertexFitter << endreq;
    msg(MSG::INFO) << endreq;
}

void InDetIterativePriVxFinderTool::m_SGError(std::string errService)
{
    msg(MSG::FATAL) << errService << " not found. Exiting !" << endreq;
    return;
}

double InDetIterativePriVxFinderTool::compatibility(const Trk::TrackParameters & measPerigee,
                                                    const Trk::RecVertex & vertex) const
{
  
  Trk::LinearizedTrack* myLinearizedTrack=m_LinearizedTrackFactory->linearizedTrack(&measPerigee,vertex);
  
  Amg::Vector3D vertexPosition;
  vertexPosition[0]=vertex.position()[0];
  vertexPosition[1]=vertex.position()[1];
  vertexPosition[2]=vertex.position()[2];
  
  AmgMatrix(2,2) weightReduced=myLinearizedTrack->expectedCovarianceAtPCA().block<2,2>(0,0);
  
  AmgMatrix(2,2) errorVertexReduced=(myLinearizedTrack->positionJacobian()*
					  (vertex.covariancePosition()*myLinearizedTrack->positionJacobian().transpose())).block<2,2>(0,0);
  
  weightReduced+=errorVertexReduced;
  
  weightReduced = weightReduced.inverse().eval();
  Amg::Vector2D trackParameters2D = myLinearizedTrack->expectedParametersAtPCA().block<2,1>(0,0);
  double returnValue=trackParameters2D.dot(weightReduced*trackParameters2D);
  
  delete myLinearizedTrack;
  myLinearizedTrack=0;

  return returnValue;
}

void InDetIterativePriVxFinderTool::removeAllFrom(std::vector<const Trk::TrackParameters*> & perigeesToFit,
                                                  std::vector<Trk::ITrackLink*> & seedTracks) const
{
  //remove all perigeesToFit and go on...
  
  std::vector<Trk::ITrackLink*>::iterator  seedBegin=seedTracks.begin();
  std::vector<Trk::ITrackLink*>::iterator  seedEnd=seedTracks.end();
  
  std::vector<const Trk::TrackParameters*>::iterator perigeesToFitBegin=perigeesToFit.begin();
  std::vector<const Trk::TrackParameters*>::iterator perigeesToFitEnd=perigeesToFit.end();
  
  for (std::vector<const Trk::TrackParameters*>::iterator perigeesToFitIter=perigeesToFitBegin;
       perigeesToFitIter!=perigeesToFitEnd;++perigeesToFitIter)
  {
    if (msgLvl(MSG::VERBOSE))
    {
      msg(MSG::VERBOSE) << " Iterating on ne track in original perigeesToFit list of BAD VERTEX..." << endreq;
    }
    
    bool found=false;
    
    for (std::vector<Trk::ITrackLink*>::iterator seedIter=seedTracks.begin();
         seedIter!=seedEnd;++seedIter)
    {
      if ((*seedIter)->parameters()==*perigeesToFitIter)
      {
        if (msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << " found and deleted from seeds!" << endreq;
        }
        found=true;
        seedTracks.erase(seedIter);
        seedBegin=seedTracks.begin();
        seedEnd=seedTracks.end();
        break;
      }
    }
    
    if (!found)
    {
      msg(MSG::ERROR) << " Cannot find vector element to delete when removing BAD vertex! " << endreq;
    }
    
  }//for cycle
  
}

void InDetIterativePriVxFinderTool::countTracksAndNdf(Trk::VxCandidate * myVxCandidate,
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



void InDetIterativePriVxFinderTool::removeCompatibleTracks(Trk::VxCandidate * myVxCandidate,
                                                           std::vector<const Trk::TrackParameters*> & perigeesToFit,
                                                           std::vector<Trk::ITrackLink*> & seedTracks) const
{


  //now you have your new vertex with its new tracks
  //now you have to get the compatibility also of all tracks which DIDN'T BELONG to the vertex!
  std::vector<Trk::VxTrackAtVertex*>* tracksAtVertex=myVxCandidate->vxTrackAtVertex();
  
  std::vector<Trk::VxTrackAtVertex*>::const_iterator tracksAtVertexBegin=tracksAtVertex->begin();
  std::vector<Trk::VxTrackAtVertex*>::const_iterator tracksAtVertexEnd=tracksAtVertex->end();
  
  std::vector<Trk::ITrackLink*>::iterator  seedBegin=seedTracks.begin();
  std::vector<Trk::ITrackLink*>::iterator  seedEnd=seedTracks.end();
  
  std::vector<const Trk::TrackParameters*>::iterator perigeesToFitBegin=perigeesToFit.begin();
  std::vector<const Trk::TrackParameters*>::iterator perigeesToFitEnd=perigeesToFit.end();
  
  for (std::vector<Trk::VxTrackAtVertex*>::const_iterator tracksAtVertexIter=tracksAtVertexBegin;
       tracksAtVertexIter!=tracksAtVertexEnd;++tracksAtVertexIter)
  {
    
    if (msgLvl(MSG::VERBOSE))
    {
      msg(MSG::VERBOSE) << " new track..." << endreq;
    }
    
    bool found=false;
    
    for (std::vector<Trk::ITrackLink*>::iterator seedIter=seedBegin;
         seedIter!=seedEnd;++seedIter)
    {
      if ((*seedIter)->parameters()==(*tracksAtVertexIter)->initialPerigee() )
      {
        found=true;
        if ((*tracksAtVertexIter)->weight()>0.01)
        {
          if (msgLvl(MSG::VERBOSE))
          {
            msg(MSG::VERBOSE) << " found and deleted from seeds!" << endreq;
          }
          seedTracks.erase(seedIter);
          seedBegin=seedTracks.begin();
          seedEnd=seedTracks.end();
        }
        break;
      }
    }
    
    if (!found)
    {
      msg(MSG::ERROR) << " Cannot find vector element to delete (step 1)! " << endreq;
    }
    
    found=false;
    for (std::vector<const Trk::TrackParameters*>::iterator perigeesToFitIter=perigeesToFitBegin;
         perigeesToFitIter!=perigeesToFitEnd;++perigeesToFitIter)
    {
      if (*perigeesToFitIter==(*tracksAtVertexIter)->initialPerigee())
      {
        found=true;
        
        if ((*tracksAtVertexIter)->weight()>0.01)
        {
          if (msgLvl(MSG::VERBOSE))
          {
            msg(MSG::VERBOSE) << " found and deleted from perigeesToFit!" << endreq;
          }
          perigeesToFit.erase(perigeesToFitIter);
          perigeesToFitBegin=perigeesToFit.begin();
          perigeesToFitEnd=perigeesToFit.end();
        }
        break;
      }
    }
    
    if (!found)
    {
      msg(MSG::ERROR) << " Cannot find vector element to delete (step 2)! " << endreq;
    }
    
  }//finishing iterating on tracks at vertex
  
  if (msgLvl(MSG::DEBUG))
  {
    msg(MSG::DEBUG) << " Outliers still to be considered: " << perigeesToFit.size() << endreq;
  }
  if (msgLvl(MSG::VERBOSE))
  {
    msg(MSG::VERBOSE) << "Number of seeds after removal of inliers: " << seedTracks.size() << endreq;
  }
  
  std::vector<Trk::VxTrackAtVertex*>* myVxTracksAtVertex=myVxCandidate->vxTrackAtVertex();
  
  std::vector<Trk::VxTrackAtVertex*>::iterator tracksBegin=myVxTracksAtVertex->begin();
  std::vector<Trk::VxTrackAtVertex*>::iterator tracksEnd=myVxTracksAtVertex->end();
  
  for (std::vector<const Trk::TrackParameters*>::iterator perigeesToFitIter=perigeesToFitBegin;
       perigeesToFitIter!=perigeesToFitEnd;++perigeesToFitIter)
  {
    
    bool found=false;
    
    //compute the chi2 with respect to the last fitted vertex!
    //(easy since track was NOT used in the last vertex fit...)
    
    const Trk::TrackParameters* myPerigee=(*perigeesToFitIter);
    
    if (myPerigee==0)
    {
      msg(MSG::ERROR) << " Cast to perigee gives 0 pointer " << endreq;
    }
    
    double chi2=compatibility(*myPerigee,myVxCandidate->recVertex());
    
    if (msgLvl(MSG::VERBOSE))
    {
      msg(MSG::VERBOSE) << "Compatibility is : " << chi2 << endreq;
    }
    
    //check if still sufficiently compatible to previous vertex
    //(CAN BE VERY LOOSE TO BE CONSERVATIVE AGAINST FAR OUTLIERS)
    if (chi2<m_maximumChi2cutForSeeding)
    {
      if (msgLvl(MSG::VERBOSE))
      {
        msg(MSG::VERBOSE) << " Found track with compatibility: " << chi2 << 
            " to be removed from the seeds... " << endreq;
      }
      
      for (std::vector<Trk::ITrackLink*>::iterator seedIter=seedTracks.begin();
           seedIter!=seedEnd;++seedIter)
      {
        if ((*seedIter)->parameters()==*perigeesToFitIter)
        {
          if (msgLvl(MSG::VERBOSE))
          {
            msg(MSG::VERBOSE) << " found and deleted from seeds!" << endreq;
          }
          found=true;
          seedTracks.erase(seedIter);
          seedBegin=seedTracks.begin();
          seedEnd=seedTracks.end();
          break;
        }
      }
      
      if (!found)
      {
        msg(MSG::ERROR) << " Cannot find vector element to delete (step 3)! " << endreq;
      }
    }
    else
    {
      //look if the track is attached to the vertex. If this is the case you should 
      //delete the track from the vertex!
      
      if (msgLvl(MSG::VERBOSE))
      {
        msg(MSG::VERBOSE) << " Found track with compatibility: " << chi2 << 
            " to be further considered and thus to be removed from previous vertex if it was there... " << endreq;
      }
      
      bool found=false;
      
      for (std::vector<Trk::VxTrackAtVertex*>::iterator tracksIter=tracksBegin;
           tracksIter!=tracksEnd;++tracksIter)
      {
        if ((*tracksIter)->initialPerigee()==*perigeesToFitIter)
        {
          
          if (msgLvl(MSG::VERBOSE))
          {
            msg(MSG::VERBOSE) << " OK, removing track with compatibility:"  << (*tracksIter)->trackQuality().chiSquared() << 
                " or vtx compatibility" << (*tracksIter)->vtxCompatibility() << " which was found attached to the vertex... " << endreq;
          }
          delete *tracksIter; // delete has to happen BEFORE the erase (because the iterator will point to the next object in the vector AFTER the erase!)
          myVxTracksAtVertex->erase(tracksIter);
          tracksBegin=myVxTracksAtVertex->begin();
          tracksEnd=myVxTracksAtVertex->end();
          found=true;
          break;
        }
        
      }
      
      if (!found)
      {
        if (msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << "Track not found: probably it was already not attached to the vertex" << endreq;
        }
      }
    }
  }//iterate on all perigeesToFit
  
}


  
} // end namespace InDet

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
              2016-03-09  D. Casper
                          Change default value of TracksMaxZinterval to 1 mm
                          (trial working point for high luminosities)

 ***************************************************************************/
#include "VxVertex/RecVertex.h"
#include "VxVertex/Vertex.h"
#include "InDetPriVxFinderTool/InDetAdaptiveMultiPriVxFinderTool.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include <map>
#include <vector>
#include <utility>
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "NN.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "VxMultiVertex/MvfFitInfo.h"
#include "VxMultiVertex/MVFVxTrackAtVertex.h"
#include "VxMultiVertex/TrackToVtxLink.h"
#include "DataModel/DataVector.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkVertexFitters/AdaptiveMultiVertexFitter.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
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


namespace InDet
{
InDetAdaptiveMultiPriVxFinderTool::InDetAdaptiveMultiPriVxFinderTool(const std::string& t, const std::string& n, const IInterface*  p)
  : AthAlgTool(t,n,p),
    m_MultiVertexFitter("Trk::AdaptiveMultiVertexFitter"),
    m_SeedFinder("Trk::ZScanSeedFinder"),
    m_trkFilter("InDet::InDetTrackSelection"),
    m_VertexEdmFactory("Trk::VertexInternalEdmFactory"),
    m_iBeamCondSvc("BeamCondSvc",n),
    m_etaDependentCutsSvc("",n),
    m_useBeamConstraint(true),
    m_TracksMaxZinterval(0.5),
    m_maxVertexChi2(18.42),
    m_realMultiVertex(true),
    m_useFastCompatibility(true),
    m_selectiontype(0),
    m_finalCutMaxVertexChi2(18.42),
    m_cutVertexDependence(3.),
    m_minweight(0.02),
    m_maxIterations(200),
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
    declareProperty("InDetEtaDependentCutsSvc", m_etaDependentCutsSvc);
}

InDetAdaptiveMultiPriVxFinderTool::~InDetAdaptiveMultiPriVxFinderTool()
{}

StatusCode InDetAdaptiveMultiPriVxFinderTool::initialize()
{
    /* Get the right vertex fitting tool */
    ATH_CHECK( m_MultiVertexFitter.retrieve() );
    
    ATH_CHECK( m_SeedFinder.retrieve() );

    ATH_CHECK( m_iBeamCondSvc.retrieve() );
    
    ATH_CHECK( m_trkFilter.retrieve() );
 
    // since some parameters special to an inherited class this method
    // will be overloaded by the inherited class
    if (msgLvl(MSG::DEBUG)) m_printParameterSettings();
    
    if (not m_etaDependentCutsSvc.name().empty()){
      ATH_CHECK(m_etaDependentCutsSvc.retrieve());
      ATH_MSG_INFO("m_etaDependentCutsSvc not empty");
    }


    ATH_MSG_INFO("Initialization successful");

    return StatusCode::SUCCESS;
}

namespace
{

  struct xAODVertex_pair
  {
    double first;
    xAOD::Vertex* second;
    xAODVertex_pair(double p1, xAOD::Vertex* p2)
            : first (p1), second (p2) {}
    bool operator< (const xAODVertex_pair& other) const
          { return first > other.first; }
  };
} //anonymous namespace

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetAdaptiveMultiPriVxFinderTool::findVertex(const TrackCollection* trackTES) 
{

  // TODO: change trkFilter to allow for this replacement
  /*
  xAOD::Vertex beamposition;
  beamposition.makePrivateStore();
  beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
  beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());
  */

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
      selectionPassed=m_trkFilter->accept(**itr,&null); // TODO: change trkFilter?
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

  ATH_MSG_DEBUG("Of " << trackTES->size() << " tracks "
		<< selectedTracks.size() << " survived the preselection.");

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> returnContainers = findVertex( selectedTracks );
  
  std::vector<const Trk::ITrackLink*>::iterator ibegin=selectedTracks.begin();
  std::vector<const Trk::ITrackLink*>::iterator iend=selectedTracks.end();

  for (std::vector<const Trk::ITrackLink*>::iterator iiter=ibegin;iiter!=iend;++iiter) {
    if ((*iiter)!=0) {
      delete *iiter;
      *iiter=0;
    }
  }

  return returnContainers;

}

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetAdaptiveMultiPriVxFinderTool::findVertex(const Trk::TrackParticleBaseCollection* trackTES) {
 
  std::vector<const Trk::ITrackLink*> selectedTracks;

  // TODO: change trkFilter to allow for this replacement
  /*
  xAOD::Vertex beamposition;
  beamposition.makePrivateStore();
  beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
  beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());
  */

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
      selectionPassed=m_trkFilter->accept(*((*itr)->originalTrack()),&null); // TODO: change trkFilter?
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

  ATH_MSG_DEBUG("Of " << trackTES->size() << " tracks "
		<< selectedTracks.size() << " survived the preselection.");

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> returnContainers = findVertex( selectedTracks );

  std::vector<const Trk::ITrackLink*>::iterator ibegin=selectedTracks.begin();
  std::vector<const Trk::ITrackLink*>::iterator iend=selectedTracks.end();

  for (std::vector<const Trk::ITrackLink*>::iterator iiter=ibegin;iiter!=iend;++iiter) {
    if ((*iiter)!=0) {
      delete *iiter;
      *iiter=0;
    }
  }

  return returnContainers;

}

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetAdaptiveMultiPriVxFinderTool::findVertex(const xAOD::TrackParticleContainer* trackParticles)
{
  std::vector<const Trk::ITrackLink*> selectedTracks;

  xAOD::Vertex beamposition;
  beamposition.makePrivateStore();
  beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
  beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());

  typedef DataVector<xAOD::TrackParticle>::const_iterator TrackParticleDataVecIter;

  bool selectionPassed;
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

    /// eta dependent cuts ////
    bool etaCuts = not m_etaDependentCutsSvc.name().empty(); 
    if (etaCuts) selectionPassed &= vtxEtaDependentCut(*itr); 
    if (selectionPassed) {
      ElementLink<xAOD::TrackParticleContainer> link;
      link.setElement(const_cast<xAOD::TrackParticle*>(*itr));
      Trk::LinkToXAODTrackParticle * linkTT = new Trk::LinkToXAODTrackParticle(link);
      linkTT->setStorableObject(*trackParticles); //@TODO: really?!
      selectedTracks.push_back(linkTT);
    }

  }

  ATH_MSG_DEBUG("Of " << trackParticles->size() << " tracks " << selectedTracks.size() << " survived the preselection.");

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> returnContainers = findVertex( selectedTracks );

  std::vector<const Trk::ITrackLink*>::iterator ibegin=selectedTracks.begin();
  std::vector<const Trk::ITrackLink*>::iterator iend=selectedTracks.end();

  //clean-up and return
  for (std::vector<const Trk::ITrackLink*>::iterator iiter=ibegin;iiter!=iend;++iiter) {
    if ((*iiter)!=0) {
      delete *iiter;
      *iiter=0;
    }
  }

  return returnContainers;

}

bool InDetAdaptiveMultiPriVxFinderTool::vtxEtaDependentCut(const xAOD::TrackParticle* trk)
{
  // vertex's eta dependent cuts method.
  // If the truck does not pass one of the cuts than etaSelectionPassed flag become false 
  // and the truck is not stored
  
  ATH_MSG_DEBUG("in vtxEtaDependentCut ");
  bool etaSelectionPassed_temp = true ; 
  double trackEta = (trk)->eta();
  const xAOD::ParametersCovMatrix_t covTrk = trk->definingParametersCovMatrix();
  
  if ( fabs((trk)->pt()) < m_etaDependentCutsSvc->getMinPtAtEta(trackEta) ) {
    etaSelectionPassed_temp = false;
    ATH_MSG_DEBUG("track pt: " << fabs((trk)->pt()) << " LOWER than min pt: " << m_etaDependentCutsSvc->getMinPtAtEta(trackEta) << "");
  }
  else if ( fabs((trk)->d0()) > m_etaDependentCutsSvc->getMaxPrimaryImpactAtEta(trackEta) ) {
    etaSelectionPassed_temp = false;
    ATH_MSG_DEBUG("track d0: " << fabs((trk)->d0()) << " HIGHER than max d0: "<< m_etaDependentCutsSvc->getMaxPrimaryImpactAtEta(trackEta) << "" );
  }
  else if ( fabs((trk)->z0()) > m_etaDependentCutsSvc->getMaxZImpactAtEta(trackEta) ) {
    etaSelectionPassed_temp = false;
    ATH_MSG_DEBUG("track z0: " << fabs((trk)->z0()) << " HIGHER than max z0: "<< m_etaDependentCutsSvc->getMaxZImpactAtEta(trackEta) << "" );
  }
  else if ( fabs((trk)->d0())/Amg::error(covTrk,Trk::d0) > m_etaDependentCutsSvc->getSigIPd0MaxAtEta(trackEta) ) {
    etaSelectionPassed_temp = false;
    ATH_MSG_DEBUG("track error d0: " << fabs(Amg::error(covTrk, 0)) << " HIGHER than max err d0: "<< m_etaDependentCutsSvc->getSigIPd0MaxAtEta(trackEta) << "" );
  }
  else if ( getCount(*trk,xAOD::numberOfSCTHits)+getCount(*trk,xAOD::numberOfPixelHits) < m_etaDependentCutsSvc->getMinSiHitsAtEta(trackEta) ) {
    if ( getCount(*trk,xAOD::numberOfSCTHits)+getCount(*trk,xAOD::numberOfPixelHits) < 0 ) {
      etaSelectionPassed_temp = false; // track rejected, missing summaryValue() infos
      ATH_MSG_WARNING("Failed to access Silicon Hits Counts");
    } 
    else {
      etaSelectionPassed_temp = false;
      ATH_MSG_DEBUG("SilHits: " << getCount(*trk,xAOD::numberOfSCTHits)+getCount(*trk,xAOD::numberOfPixelHits) << " LOWER than min SilHits: " << m_etaDependentCutsSvc->getMinSiHitsAtEta(trackEta) << "");
    }
  }
  else if ( getCount(*trk,xAOD::numberOfPixelHits) < m_etaDependentCutsSvc->getMinPixelHitsAtEta(trackEta) ) {
    if ( getCount(*trk,xAOD::numberOfSCTHits)+getCount(*trk,xAOD::numberOfPixelHits) < 0 ) {
      etaSelectionPassed_temp = false; // track rejected, missing summaryValue() infos
      ATH_MSG_WARNING("Failed to access Pixel Hits Counts");
    }
    else {
      etaSelectionPassed_temp = false;
      ATH_MSG_DEBUG("PixHits: " << getCount(*trk,xAOD::numberOfPixelHits) << " LOWER than min PixHits: " << m_etaDependentCutsSvc->getMinPixelHitsAtEta(trackEta) << ""); 
    }
  }
  ATH_MSG_DEBUG(" etaSelectionPassed (y/n): " << etaSelectionPassed_temp << "");
  return etaSelectionPassed_temp ; 
}
  
std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetAdaptiveMultiPriVxFinderTool::findVertex(const std::vector<const Trk::ITrackLink*> & trackVector)
{

  // TODO: put this in a better place
  // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
  // For optimization of access speed
  xAOD::Vertex::Decorator< Trk::MvfFitInfo* > MvfFitInfo("MvfFitInfo");
  xAOD::Vertex::Decorator< bool > isInitialized("isInitialized");
  //xAOD::Vertex::Decorator< Trk::VxCandidate* > VTAV("VTAV");
  xAOD::Vertex::Decorator< std::vector< Trk::VxTrackAtVertex* > > VTAV("VTAV");

  if (m_selectiontype==1) {//if you have to use NN, load the class
    m_testingclass=new NN();//check later for MEMORY LEAK
  }
  
  //---- Start of preselection of tracks according to perigee parameters ---------------//
  std::vector<const Trk::ITrackLink*> origTracks=trackVector;
  std::vector<const Trk::ITrackLink*> seedTracks=trackVector;
  //  origTracks.clear();
  //  seedTracks.clear();
  
  //now all tracks are in origTracks... std::vector<const Track*> origTracks;
  std::vector<xAODVertex_pair> myxAODVertices;

  std::vector<Trk::TrackToVtxLink*> myTrackToVtxLinks;

  xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer;
  xAOD::VertexAuxContainer* theVertexAuxContainer = new xAOD::VertexAuxContainer;
  theVertexContainer->setStore( theVertexAuxContainer );

  Amg::Vector3D actualVertex;


  std::map<const Trk::ITrackLink*,Trk::TrackToVtxLink*> TrackLinkOf;
  
  //create a map between ITrackLink* and TrackToVtxLink*
  std::vector<const Trk::ITrackLink*>::const_iterator trkbegin=origTracks.begin();
  std::vector<const Trk::ITrackLink*>::const_iterator trkend=origTracks.end();
  
  
  for (std::vector<const Trk::ITrackLink*>::const_iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {
    Trk::TrackToVtxLink* newTrkToVtxLink(new Trk::TrackToVtxLink(new std::vector<xAOD::Vertex*>)); // makePrivateStore() is called for each vertex to add in iteration
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
      ATH_MSG_DEBUG("No tracks available after track selection for seeding. No finding done.");
      break;
    }

    iteration+=1;
    ATH_MSG_DEBUG("Starting iteration number " << iteration << " with " << seedtracknumber << " seed tracks.");
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

    xAOD::Vertex* constraintVertex=0;
    if (m_useBeamConstraint) {
      constraintVertex = new xAOD::Vertex();
      constraintVertex->makePrivateStore();
      constraintVertex->setPosition( m_iBeamCondSvc->beamVtx().position() );
      constraintVertex->setCovariancePosition( m_iBeamCondSvc->beamVtx().covariancePosition() );
      constraintVertex->setFitQuality( m_iBeamCondSvc->beamVtx().fitQuality().chiSquared(), m_iBeamCondSvc->beamVtx().fitQuality().doubleNumberDoF() );
      actualVertex = m_SeedFinder->findSeed(perigeeList,constraintVertex);
    } else {
      actualVertex = m_SeedFinder->findSeed(perigeeList);
      Amg::MatrixX looseConstraintCovariance(3,3);
      looseConstraintCovariance.setIdentity();
      looseConstraintCovariance = looseConstraintCovariance * 1e+8;
      constraintVertex = new xAOD::Vertex();
      constraintVertex->makePrivateStore();
      constraintVertex->setPosition( actualVertex );
      constraintVertex->setCovariancePosition( looseConstraintCovariance );
      constraintVertex->setFitQuality( 0.,-3. );
    }

    // TODO: memory leak here with theconstraint when the loop breaks?
    if (actualVertex.z()==0.) {
      ATH_MSG_DEBUG( "No seed found: no further primary vertex finding performed on this event" );
      ATH_MSG_DEBUG( perigeeList.size() << " tracks passed to seed finder, but no seed returned." );
      // TODO: Do I need this?
      delete constraintVertex;
      constraintVertex = 0;
      break;
    }

    //new xAOD::Vertex with this
    xAOD::Vertex* actualcandidate = new xAOD::Vertex;
    actualcandidate->makePrivateStore();
    actualcandidate->setVertexType( xAOD::VxType::NotSpecified ); // to mimic the initialization present in the old EDM constructor
    // now add decorations!
    MvfFitInfo( *actualcandidate ) = new Trk::MvfFitInfo( constraintVertex,
                                                          new Amg::Vector3D(actualVertex),
                                                          new Amg::Vector3D(actualVertex));
    isInitialized( *actualcandidate ) = false;

    std::vector<Trk::VxTrackAtVertex*> vector_of_tracks(0);
    VTAV(*actualcandidate) = vector_of_tracks; // TODO: maybe needed before push_back?

    //get link to the tracks
    //std::vector<Trk::VxTrackAtVertex>* tracksOfVertex = &( actualcandidate->vxTrackAtVertex() );

    //now iterate on all tracks and find out if they are sufficiently close to the found vertex
    
    ATH_MSG_VERBOSE("Adding all the tracks which are near the seed to the candidate ");
    
    for (std::vector<const Trk::ITrackLink*>::const_iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {

      // if (fabs((*trkiter)->parameters()->position()[Trk::z]-actualVertex.position().z())<m_TracksMaxZinterval) {

      if (fabs(estimateDeltaZ(*(*trkiter)->parameters(),actualVertex))<m_TracksMaxZinterval)
      {
 	//accessing corresponding link to vertices
   	Trk::TrackToVtxLink* actuallink=TrackLinkOf[*trkiter];
	std::vector<xAOD::Vertex*>* actualvtxlink=actuallink->vertices();
	//adding vertex to candidates of track
	actualvtxlink->push_back(actualcandidate);
        ATH_MSG_VERBOSE("Adding an MVFVxTrackAtVertex with tracklink " << actuallink << 
			" to the vertex candidate VTAV decoration");
        VTAV(*actualcandidate).push_back(new Trk::MVFVxTrackAtVertex((*trkiter)->clone(),
                                                                     actuallink));
      }
    }

    ATH_MSG_VERBOSE(VTAV(*actualcandidate).size() << " tracks added to vertex candidate due to proximity");

    //now consider to recovery from the case where no tracks were added to the vertex
    if (VTAV(*actualcandidate).size()==0) {

      //you need to define a new seed (because the old one is probably in between two ones...)
      double zdistance=1e8;
      const Trk::ITrackLink* nearestTrack=0;
      
      for (std::vector<const Trk::ITrackLink*>::const_iterator seedtrkiter=seedtrkbegin;
	   seedtrkiter!=seedtrkend;++seedtrkiter) {
	if (fabs((*seedtrkiter)->parameters()->position()[Trk::z]-actualVertex.z())<zdistance) {
	  zdistance=fabs((*seedtrkiter)->parameters()->position()[Trk::z]-actualVertex.z());
	  nearestTrack=*seedtrkiter;
	}
      }

      if (nearestTrack!=0) {

	double newz=(nearestTrack->parameters())->position()[Trk::z];
	xAOD::Vertex* oldcandidate=actualcandidate; // to placehold old pointers

	actualVertex=Amg::Vector3D(0.,0.,newz);
        actualcandidate = new xAOD::Vertex();
        actualcandidate->makePrivateStore();
        actualcandidate->setVertexType( xAOD::VxType::NotSpecified ); // to mimic the initialization present in the old EDM constructor
        // TODO: Think about where everything is deleted! Does Trk::MvfFitInfo destructor and do MVFVxTrackAtVertex destructors get called when actualcandidate gets deleted?
        // now add decorations!
        MvfFitInfo( *actualcandidate ) = new Trk::MvfFitInfo( new xAOD::Vertex(*constraintVertex),
                                                              new Amg::Vector3D(actualVertex),
                                                              new Amg::Vector3D(actualVertex));
        isInitialized(*actualcandidate) = false;
        VTAV(*actualcandidate) = vector_of_tracks; // TODO: maybe needed before push_back?
        //delete MvfFitInfo( *oldcandidate ); MvfFitInfo( *oldcandidate ) = 0;
        ////delete VTAV( *oldcandidate ); VTAV( *oldcandidate ) = 0;
	//delete oldcandidate;oldcandidate=0; // TODO: does this also call the destructor of the decorations?
	releaseCandidate( oldcandidate );

        //get link to the tracks
        //tracksOfVertex = &( actualcandidate->vxTrackAtVertex() );

	for (std::vector<const Trk::ITrackLink*>::const_iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {
          // if (fabs((*trkiter)->parameters()->position()[Trk::z]-actualVertex.z())<m_TracksMaxZinterval) {

	  if (fabs(estimateDeltaZ(*((*trkiter)->parameters()),
                                  actualVertex))<m_TracksMaxZinterval)
          {
            //accessing corresponding link to vertices
            Trk::TrackToVtxLink* actuallink=TrackLinkOf[*trkiter];
            std::vector<xAOD::Vertex*>* actualvtxlink=actuallink->vertices();
            //adding vertex to candidates of track
            actualvtxlink->push_back(actualcandidate);
            ATH_MSG_VERBOSE("Adding an MVFVxTrackAtVertex with tracklink " << actuallink << 
			    " to the vertex candidate VTAV decoration");
            VTAV(*actualcandidate).push_back(new Trk::MVFVxTrackAtVertex((*trkiter)->clone(),
                                                                         actuallink));
	  }
	}

	if (VTAV(*actualcandidate).size()==0) {
	  ATH_MSG_DEBUG("No tracks found near seed, while at least one track was expected.");
	  break;
	}
      } else {
	ATH_MSG_DEBUG("Nearest track to seed is missing.");
	break;
      }
    }
      
    
    ATH_MSG_VERBOSE("Running addVtxTofit(); The current candidate has " << VTAV(*actualcandidate).size() << 
		    " tracks in the vector");
    
    m_MultiVertexFitter->addVtxTofit(actualcandidate);

    ATH_MSG_VERBOSE("After fit the current candidate has z: " << actualcandidate->position()[Trk::z]);

    //get link to the tracks (they are now all properly in the std::vector<Trk::VxTrackAtVertex> of the xAOD::Vertex)
    // TODO: maybe I shouldn't be using xAOD::Vertex vector at all for VxTrackAtVertex...
    //std::vector<Trk::VxTrackAtVertex>* tracksOfVertex = &( actualcandidate->vxTrackAtVertex() );

    std::vector<Trk::VxTrackAtVertex*>::iterator trkAtVtxbegin=VTAV(*actualcandidate).begin();
    std::vector<Trk::VxTrackAtVertex*>::iterator trkAtVtxend=VTAV(*actualcandidate).end();

    //now check that there is at least one track added to the fit
    //(this is not always the case because only tracks above a certain compatibility threshold are considered)
    bool atleastonecompatibletrack=false;
    int numberOfCompatibleTracks=0;
    for (std::vector<Trk::VxTrackAtVertex*>::iterator trkAtVtxIter=trkAtVtxbegin;
	 trkAtVtxIter!=trkAtVtxend;++trkAtVtxIter) { 
      ATH_MSG_VERBOSE("Compatibility: " << (*trkAtVtxIter)->vtxCompatibility() << 
		      " weight " << (*trkAtVtxIter)->weight());
      if ( ( (*trkAtVtxIter)->vtxCompatibility()<m_maxVertexChi2&&m_useFastCompatibility) ||
	   ( (*trkAtVtxIter)->weight()>m_minweight 
	     && (*trkAtVtxIter)->trackQuality().chiSquared()<m_maxVertexChi2 
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
	  ATH_MSG_VERBOSE("Found compatible track");
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

    ATH_MSG_VERBOSE("newVertexIsFine = " << newVertexIsFine);

    //this now should be so powerful to do everything by itself
    //problem now is to delete the really compatible tracks to this fit from the tracks 
    //which still remain to be fitted
    
    if (atleastonecompatibletrack) {
      for (std::vector<Trk::VxTrackAtVertex*>::iterator trkAtVtxIter=trkAtVtxbegin;
	   trkAtVtxIter!=trkAtVtxend;++trkAtVtxIter) {
	//for now using the compatibility at stage before end...
	ATH_MSG_VERBOSE("The compatibility value of the track " << *trkAtVtxIter << 
			" is " <<  (*trkAtVtxIter)->vtxCompatibility());
	if ( ( (*trkAtVtxIter)->vtxCompatibility()<m_maxVertexChi2&&m_useFastCompatibility) ||
	     ( (*trkAtVtxIter)->weight()>m_minweight 
	       && (*trkAtVtxIter)->trackQuality().chiSquared()<m_maxVertexChi2 
	       && !m_useFastCompatibility ) ) {
	  ATH_MSG_VERBOSE("Eliminating incompatible track");

	  std::vector<const Trk::ITrackLink*>::iterator foundTrack=seedtrkend;
	  for (std::vector<const Trk::ITrackLink*>::iterator seedtrkiter=seedtrkbegin;seedtrkiter!=seedtrkend;++seedtrkiter) {
	    if ((*seedtrkiter)->parameters()==(*trkAtVtxIter)->trackOrParticleLink()->parameters()) {
	      foundTrack=seedtrkiter;
	    }
	  }
	  ATH_MSG_VERBOSE("Trying to find track now");
	  if (foundTrack!=seedtrkend) {
	    ATH_MSG_VERBOSE("Track found: eliminating it");
	    seedTracks.erase(foundTrack);
	    
	    //update end and begin??? should I? yes, he can copy, regenerate, you don't know!
	    seedtrkbegin=seedTracks.begin();
	    seedtrkend=seedTracks.end();
	    
	    ATH_MSG_VERBOSE("Remaining seeds: " << seedTracks.size());
	  }
	}
      }
    } else {//no compatible track found...
      //in this case put out the highest seeding track which didn't give any good result...
      double highestcompatibility=0;
      Trk::VxTrackAtVertex* trackHighestCompatibility=0;
      
      ATH_MSG_VERBOSE("Analyzing new vertex");

      for (std::vector<Trk::VxTrackAtVertex*>::iterator trkAtVtxIter=trkAtVtxbegin;
	   trkAtVtxIter!=trkAtVtxend;++trkAtVtxIter) {
	ATH_MSG_VERBOSE("Checking new track for compatibility");
	const Trk::ITrackLink* foundTrack=0;
	for (std::vector<const Trk::ITrackLink*>::const_iterator seedtrkiter=seedtrkbegin;seedtrkiter!=seedtrkend;++seedtrkiter) {
	  if ((*seedtrkiter)->parameters()==(*trkAtVtxIter)->trackOrParticleLink()->parameters()) {
	    foundTrack=*seedtrkiter;
	  }
	}

	if (foundTrack!=0) {
	  double compatibility=(*trkAtVtxIter)->vtxCompatibility();
	  ATH_MSG_VERBOSE("New track has compatibility: " << compatibility);
	  if (compatibility>highestcompatibility) {
	    highestcompatibility=compatibility;
	    trackHighestCompatibility= *trkAtVtxIter;
	  }
	}
      }
      ATH_MSG_VERBOSE("Highest compatibility track:" <<trackHighestCompatibility << 
		      "with compatibility: " << highestcompatibility);

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
	  ATH_MSG_FATAL("Cannot find previously determined track ");
	  throw;
	}
      } else {
	
	//alternative method: delete seed track nearest in z to the seed
	
	double zdistance=1e8;
	const Trk::ITrackLink* nearestTrack=0;
	
	for (std::vector<const Trk::ITrackLink*>::const_iterator seedtrkiter=seedtrkbegin;
	     seedtrkiter!=seedtrkend;++seedtrkiter) {
	  if (fabs((*seedtrkiter)->parameters()->position()[Trk::z]-actualVertex.z())<zdistance) {
	    zdistance=fabs((*seedtrkiter)->parameters()->position()[Trk::z]-actualVertex.z());
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
	    ATH_MSG_DEBUG("No nearest track found while it was expected.");
	    break;
	  }
	} else {
	  ATH_MSG_DEBUG("No further seeding track was found (3 methods used) while it was expected.");
	  break;
	}
      }
    }
    
    
    ///////////////
    //now break the cycle if you didn't diminish the number of seeds...
    
    ATH_MSG_DEBUG("Remaining seeds: " << seedTracks.size() << " previous round " << seedtracknumber);
    
    bool deleteLastVertex=false;

    if (!newVertexIsFine) {
      deleteLastVertex=true;
    } else {
      
      double contamination=0.;
      
      double contaminationNum=0;
      double contaminationDeNom=0;
      
      std::vector<Trk::VxTrackAtVertex*>::iterator TRKtrkbegin(VTAV(*actualcandidate).begin());
      std::vector<Trk::VxTrackAtVertex*>::iterator TRKtrkend(VTAV(*actualcandidate).end());
      
      for (std::vector<Trk::VxTrackAtVertex*>::iterator TRKtrkiter=TRKtrkbegin;TRKtrkiter!=TRKtrkend;++TRKtrkiter) {
        
        double trackWeight=(*TRKtrkiter)->weight();
        contaminationNum+=trackWeight*(1.-trackWeight);
        contaminationDeNom+=trackWeight*trackWeight;
      }
      
      if (contaminationDeNom>0)
      {
        contamination=contaminationNum/contaminationDeNom;
      }
      
      
      if (contamination>m_maximumVertexContamination)
      {
	ATH_MSG_VERBOSE("Contamination estimator " << contamination << " fails cut of " << m_maximumVertexContamination);
        deleteLastVertex=true;
      }

      //now try to understand if the vertex was merged with another one...
      std::vector<xAODVertex_pair>::iterator vxbegin=myxAODVertices.begin();
      std::vector<xAODVertex_pair>::iterator vxend=myxAODVertices.end();
      
      for (std::vector<xAODVertex_pair>::iterator vxiter=vxbegin;vxiter!=vxend;++vxiter) {
	ATH_MSG_VERBOSE("Estimating compatibility of z positions: " << ((*vxiter).second)->position()[Trk::z] << 
			" and " << actualcandidate->position()[Trk::z]);
	//in case of no beam spot constraint you should use the full 3d significance on the distance
	double dependence=0;

        if (!m_do3dSplitting)
        {
          dependence=fabs((*vxiter).second->position()[Trk::z]-actualcandidate->position()[Trk::z])/
	    TMath::Sqrt( (*vxiter).second->covariancePosition()(Trk::z,Trk::z)+
			 actualcandidate->covariancePosition()(Trk::z,Trk::z));
        }
        else
        {
	  Amg::MatrixX sumCovariances=
              (*vxiter).second->covariancePosition()+
              actualcandidate->covariancePosition();

	  sumCovariances = sumCovariances.inverse().eval();

	  Amg::Vector3D hepVectorPosition;
	  hepVectorPosition[0]=
	    ((*vxiter).second->position()-
	     actualcandidate->position()).x();
	  hepVectorPosition[1]=
	    ((*vxiter).second->position()-
	     actualcandidate->position()).y();
	  hepVectorPosition[2]=
	    ((*vxiter).second->position()-
	     actualcandidate->position()).z();
	  dependence=sqrt(hepVectorPosition.dot(sumCovariances*hepVectorPosition));
        }
	
	ATH_MSG_VERBOSE("Significance of vertex pair is: " << dependence << "vs. cut at " << m_cutVertexDependence);

	if (dependence<m_cutVertexDependence) {
          ATH_MSG_VERBOSE("Deleting last vertex since it was found to be merged with another!");
	  deleteLastVertex=true;
	  break;
	}
      }
    }
    
    ATH_MSG_VERBOSE("Decision to delete last vertex: " << deleteLastVertex);

    ////////////
    //Ok all tracks in seed were deleted. You can go ahead and discover further vertices...
    //please clean the track to vertices links before (required by real multivertexfit)
    if (deleteLastVertex) {
      
      std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkAtVtxBegin = VTAV( *actualcandidate ).begin();
      std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkAtVtxEnd = VTAV( *actualcandidate ).end();
      
      for (std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkIterator=MVFtrkAtVtxBegin;
	   MVFtrkIterator!=MVFtrkAtVtxEnd;++MVFtrkIterator) {
	ATH_MSG_VERBOSE("Deleting one vertex from tracklink " << 
			(static_cast<Trk::MVFVxTrackAtVertex*>(*MVFtrkIterator))->linkToVertices());
        (static_cast<Trk::MVFVxTrackAtVertex*>(*MVFtrkIterator))->linkToVertices()->vertices()->pop_back();
      }
      seedtracknumber=seedTracks.size();

      ATH_MSG_VERBOSE("Redoing fit after scrapping last vertex");

      m_MultiVertexFitter->addVtxTofit(actualcandidate); // TODO: I think this is fine still, but think about it more

      releaseCandidate( actualcandidate );

    } else {
    
      seedtracknumber=seedTracks.size();
      ATH_MSG_VERBOSE("Storing new vertex with " << actualcandidate->vxTrackAtVertex().size() << " tracks");
      myxAODVertices.push_back
	(xAODVertex_pair (0,//estimateSignalCompatibility(actualcandidate),
			    actualcandidate));
    }
    
  } while ( (
                (m_addSingleTrackVertices&&seedTracks.size()>0)||
                ((!m_addSingleTrackVertices)&&seedTracks.size()>1) )
            && iteration<m_maxIterations);

  if (iteration>= m_maxIterations) {
    ATH_MSG_WARNING("Maximum number of iterations (" << m_maxIterations << 
		    ") reached; to reconstruct more vertices, set maxIterations to a higher value.");
  }

  ATH_MSG_DEBUG("Primary vertex finding complete with " << iteration << 
		" iterations and " << myxAODVertices.size() << " vertices found.");

  //correction of a bug: you can estimate the probability of being 
  //the primary interaction vertex only after the whole multivertexfit 
  //is finished!!! (the first is influenced by the fit of the second and so 
  //on...)
  std::vector<xAODVertex_pair>::iterator vtxBegin=myxAODVertices.begin();
  std::vector<xAODVertex_pair>::iterator vtxEnd=myxAODVertices.end();

  // To make sure that the right tracks are in the std::vector<Trk::VxTrackAtVertex> of each vertex - up until now, they are kept in the VTAV decoration
  for (std::vector<xAODVertex_pair>::iterator vtxIter=vtxBegin;vtxIter!=vtxEnd;++vtxIter) {

    xAOD::Vertex* cand = vtxIter->second;
    std::vector<Trk::VxTrackAtVertex>* tracksOfVertex = &( cand->vxTrackAtVertex() );
    tracksOfVertex->clear();

    std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkBegin = VTAV( *cand ).begin();
    std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkEnd = VTAV( *cand ).end();
    for (std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkIter=MVFtrkBegin;MVFtrkIter!=MVFtrkEnd;++MVFtrkIter) {
      tracksOfVertex->push_back( **MVFtrkIter );
    }

  }

  //before filling the container, you have to decide what is your most probable signal vertex
  for (std::vector<xAODVertex_pair>::iterator vtxIter=vtxBegin;vtxIter!=vtxEnd;++vtxIter) {
    (*vtxIter).first=estimateSignalCompatibility((*vtxIter).second);
  }

  std::sort (myxAODVertices.begin(), myxAODVertices.end());

  if (msgLvl(MSG::VERBOSE))
  {
    ATH_MSG_VERBOSE("Vertex positions after sorting");
    for (std::vector<xAODVertex_pair>::iterator vtxIter=vtxBegin;vtxIter!=vtxEnd;++vtxIter) {
      ATH_MSG_VERBOSE("z position: " << (*vtxIter).second->position().z());
    }
  }

  if (myxAODVertices.size()==0) {
    ATH_MSG_WARNING("No vertices found: returning a place-holder at the beam spot center.");
    
    xAOD::Vertex * beamspotCandidate = new xAOD::Vertex;
    beamspotCandidate->makePrivateStore();
    beamspotCandidate->setPosition( m_iBeamCondSvc->beamVtx().position() );
    beamspotCandidate->setCovariancePosition( m_iBeamCondSvc->beamVtx().covariancePosition() );
    beamspotCandidate->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
    // TODO: I don't need to set fitQuality too do I?

    myxAODVertices.push_back(xAODVertex_pair(0,beamspotCandidate));
  }
  

  vtxBegin=myxAODVertices.begin();
  vtxEnd=myxAODVertices.end();

  for (std::vector<xAODVertex_pair>::const_iterator vtxIter=vtxBegin;vtxIter!=vtxEnd;++vtxIter) {

    xAOD::Vertex* cand = vtxIter->second;

    std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkBegin = VTAV( *cand ).begin();
    std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkEnd = VTAV( *cand ).end();
    // TODO: here, I must clean up VTAV decoration separately from vector of VxTrackAtVertex
    for (std::vector<Trk::VxTrackAtVertex*>::iterator MVFtrkIter=MVFtrkBegin;MVFtrkIter!=MVFtrkEnd;++MVFtrkIter) {
      //setting link to TrackToVtxLink to 0 (all TrackToVtxLink will be deleted some lines later)
      (static_cast<Trk::MVFVxTrackAtVertex*>(*MVFtrkIter))->setLinkToVertices(0);
      delete *MVFtrkIter;
      *MVFtrkIter = 0;
    }


    //delete VTAV( *cand );
    delete MvfFitInfo( *cand );
    
    std::vector<Trk::VxTrackAtVertex>::iterator trkAtVtxBegin = cand->vxTrackAtVertex().begin();
    std::vector<Trk::VxTrackAtVertex>::iterator trkAtVtxEnd = cand->vxTrackAtVertex().end();
    for (std::vector<Trk::VxTrackAtVertex>::iterator trkAtVtxIter=trkAtVtxBegin;trkAtVtxIter!=trkAtVtxEnd;){//++trkAtVtxIter) {
      //cleaning up incompatible vertices
      if ( ( (*trkAtVtxIter).vtxCompatibility()>m_maxVertexChi2&&m_useFastCompatibility ) ||
             ( ( (*trkAtVtxIter).weight()<m_minweight
                 || (*trkAtVtxIter).trackQuality().chiSquared()>m_maxVertexChi2 )
               && !m_useFastCompatibility ) ) {
        trkAtVtxIter=cand->vxTrackAtVertex().erase(trkAtVtxIter);
        trkAtVtxEnd=cand->vxTrackAtVertex().end();
      } else {
        ++trkAtVtxIter;
      }
    }
    theVertexContainer->push_back(cand);
  }

  // If track links are to xAOD::TrackParticles, set the links directly in xAOD::Vertex with their weights
  // Needed for weight calculator in sorting tool
  xAOD::VertexContainer::const_iterator vxBegin=theVertexContainer->begin();
  xAOD::VertexContainer::const_iterator vxEnd=theVertexContainer->end();
  for (xAOD::VertexContainer::const_iterator vxIter=vxBegin;vxIter!=vxEnd;++vxIter)
  {
    std::vector<Trk::VxTrackAtVertex>* myVxTracksAtVtx = &((*vxIter)->vxTrackAtVertex());

    if (!myVxTracksAtVtx) continue;

    std::vector<Trk::VxTrackAtVertex>::iterator tracksBegin=myVxTracksAtVtx->begin();
    std::vector<Trk::VxTrackAtVertex>::iterator tracksEnd=myVxTracksAtVtx->end();
    for (std::vector<Trk::VxTrackAtVertex>::iterator tracksIter=tracksBegin;
         tracksIter!=tracksEnd;++tracksIter)
    {
      // only set link if track link is to an xAOD::TrackParticle
      Trk::LinkToXAODTrackParticle* linkToXAODTP = dynamic_cast<Trk::LinkToXAODTrackParticle*>( (*tracksIter).trackOrParticleLink() );
      if (linkToXAODTP)
      {
	ATH_MSG_VERBOSE("Iterating over new vertex in fixing xAOD::TrackParticle links... ");

        (*vxIter)->addTrackAtVertex(*linkToXAODTP, (*tracksIter).weight());
      } // TODO: esle write in a warning? (if tracks were TrkTracks or Trk::TrackParticleBase) - sorting tool expects there to be xAOD::TrackParticleLinks!
    }
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
  if ( theVertexContainer->size() >= 1 )
  {
    xAOD::Vertex * primaryVtx = theVertexContainer->front();
    if (primaryVtx->vxTrackAtVertex().size()>0)
    {
      primaryVtx->setVertexType(xAOD::VxType::PriVtx);
      xAOD::Vertex * dummyxAODVertex = new xAOD::Vertex;
      theVertexContainer->push_back( dummyxAODVertex ); // have to add vertex to container here first so it can use its aux store
      dummyxAODVertex->setPosition( primaryVtx->position() );
      dummyxAODVertex->setCovariancePosition( primaryVtx->covariancePosition() );
      dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
      dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
    }
    else
    {
      primaryVtx->setVertexType(xAOD::VxType::NoVtx);
    }
    
  }
  //---- if no vertex is there let dummy be at beam spot
  else if ( theVertexContainer->size() == 0 )
  {
    xAOD::Vertex * dummyxAODVertex = new xAOD::Vertex;
    theVertexContainer->push_back( dummyxAODVertex ); // have to add vertex to container here first so it can use its aux store
    dummyxAODVertex->setPosition( m_iBeamCondSvc->beamVtx().position() );
    dummyxAODVertex->setCovariancePosition( m_iBeamCondSvc->beamVtx().covariancePosition() );
    dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
    dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
  }
  
  // loop over the pile up to set it as pile up (EXCLUDE first and last vertex: loop from 1 to size-1)
  for (unsigned int i = 0 ; i < theVertexContainer->size()-1 ; i++)
  {
    if (i>0)
    {
      (*theVertexContainer)[i]->setVertexType(xAOD::VxType::PileUp);
    }
    ATH_MSG_VERBOSE("Vertex at z =" << (*theVertexContainer)[i]->position().z() << 
		    " with ntracks: " << (*theVertexContainer)[i]->vxTrackAtVertex().size());
   
  }
  return std::make_pair(theVertexContainer, theVertexAuxContainer);
  
}

 double InDetAdaptiveMultiPriVxFinderTool::estimateSignalCompatibility(xAOD::Vertex *mycand) 
 {

  // TODO: put this in a better place
  // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
  // For optimization of access speed
  xAOD::Vertex::Decorator< std::vector<Trk::VxTrackAtVertex*> > VTAV("VTAV");

  std::vector<Trk::VxTrackAtVertex*>::iterator begintracks=VTAV(*mycand).begin();
  std::vector<Trk::VxTrackAtVertex*>::iterator endtracks=VTAV(*mycand).end();

  if (m_selectiontype==0) 
  {
  
    //use just sum over pt squared
    //first get all the variables you need for to discriminate between signal and minimum bias event

    double total_pt_squared=0;
    int total_num_tracks=0;

    for(std::vector<Trk::VxTrackAtVertex*>::iterator i = begintracks; i!=endtracks; i++) {

      ATH_MSG_VERBOSE("Compatibility is: " << (*i)->vtxCompatibility() << 
		      " and chi^2 is: " << (*i)->trackQuality().chiSquared());

      if ( ( (*i)->vtxCompatibility()<m_finalCutMaxVertexChi2&&m_useFastCompatibility) ||
	   ( (*i)->weight()>m_minweight 
	     && (*i)->trackQuality().chiSquared()<m_finalCutMaxVertexChi2 
	     && !m_useFastCompatibility ) ) {
        const Trk::TrackParameters* perigee(0);
        if ((*i)->perigeeAtVertex()!=0) {
          perigee=(*i)->perigeeAtVertex();
        } else {
	  ATH_MSG_VERBOSE("Only initialPerigee is available");
          perigee=(*i)->initialPerigee();
        }
	if (perigee==0) {
	  ATH_MSG_ERROR("Neutrals are not supported. Skipping track in pT calculation...");
	  continue;
	}
        total_pt_squared+=std::pow(std::fabs(1./perigee->parameters()[Trk::qOverP])*sin(perigee->parameters()[Trk::theta]),2);
        total_num_tracks+=1;
      }
    }//finishing iterating on VxTrackAtVertex associated to **vtxIter xAOD::Vertex

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
      if ( ( (*i)->vtxCompatibility()<m_finalCutMaxVertexChi2&&m_useFastCompatibility) ||
	   ( (*i)->weight()>m_minweight 
	     && (*i)->trackQuality().chiSquared()<m_finalCutMaxVertexChi2 
	     && !m_useFastCompatibility ) ) {
        const Trk::TrackParameters* perigee(0);
        if ((*i)->perigeeAtVertex()!=0) {
          perigee=(*i)->perigeeAtVertex();
        } else {
	  ATH_MSG_VERBOSE("Only initialPerigee is available");
          perigee=(*i)->initialPerigee();
        }
	if (perigee==0) {
	  ATH_MSG_ERROR("Neutrals not supported. Skipping track in pT calculation...");
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
      return 0.;
    } else {
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
    ATH_MSG_DEBUG("Adaptive Multi-Vertex Finder: Parameter settings ");
    ATH_MSG_DEBUG("Trackselection cuts handled by the TrackSelectorTool: " << m_trkFilter);
    ATH_MSG_DEBUG("Finder settings: ");
    ATH_MSG_DEBUG("Maximum distance to include track in simultaneous vertex fits: TracksMaxZinterval " << m_TracksMaxZinterval);
    ATH_MSG_DEBUG("Seeding: minimum chi^2 for a track being an outlier:  maxVertexChi2 " << m_maxVertexChi2);
    ATH_MSG_DEBUG("Signal identification: final cut on track chi2: finalCutMaxVertexChi2 = " << m_finalCutMaxVertexChi2);
    ATH_MSG_DEBUG( "Activate complete multi vertex fitting feature: realMultiVertex " << m_realMultiVertex);
    ATH_MSG_DEBUG("Merging vertices: upper cut on significance to merge two vertices: cutVertexDependence = " << 
		  m_cutVertexDependence);
    ATH_MSG_DEBUG("Maximum number of iterations (and vertices): maxIterations = " << m_maxIterations);
    ATH_MSG_DEBUG("Selection type (0 is sqrt(Ntr)*Sum_{tr} pT^2): selectiontype = " << m_selectiontype);
    ATH_MSG_DEBUG("Use fast compatibility (if false use refitted chi2 instead of approximation): useFastCompatibility = "<<  
		  m_useFastCompatibility);
    ATH_MSG_DEBUG("MinWeight (if track weight in the fit is lower, don't perform the Kalman Update) = " << m_minweight);
    ATH_MSG_DEBUG("");
}

void InDetAdaptiveMultiPriVxFinderTool::m_SGError(std::string errService)
{
  ATH_MSG_FATAL(errService << " not found. Exiting !");
}

double InDetAdaptiveMultiPriVxFinderTool::estimateDeltaZ(const Trk::TrackParameters& myPerigee, 
							 const Amg::Vector3D& myTransvVertex)
{

  Amg::Vector3D lp=myTransvVertex;
  
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
  
  return expPoint[2] - lp.z() - 1./tan_th*(X*cos_phi_v+Y*sin_phi_v);

}
  
  void InDetAdaptiveMultiPriVxFinderTool::releaseCandidate(xAOD::Vertex*& candidate)
  {
    if (candidate == nullptr) return;
    // decorators
    xAOD::Vertex::Decorator< Trk::MvfFitInfo* > MvfFitInfo("MvfFitInfo");
    xAOD::Vertex::Decorator< std::vector< Trk::VxTrackAtVertex* > > VTAV("VTAV");
    
    if ( VTAV.isAvailable(*candidate) )
    {
      for ( auto tav : VTAV( *candidate ) ) 
      {
	if (tav == nullptr) continue;
	(static_cast<Trk::MVFVxTrackAtVertex*>(tav))->setLinkToVertices(nullptr);
	delete tav;
	tav = nullptr;
      }
      VTAV( *candidate ).clear();
    }

    if ( MvfFitInfo.isAvailable(*candidate) && MvfFitInfo(*candidate) != nullptr )
    {
      delete MvfFitInfo(*candidate);
      MvfFitInfo(*candidate) = nullptr;
    }

    delete candidate;
    candidate = nullptr;
  }
  
} // end namespace InDet


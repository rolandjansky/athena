/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetAdaptivePriVxFinderTool.cxx  -  Description
                             -------------------
    begin   : 28-01-2004
    authors : Giacinto Piacquadio (Freiburg Univ),
              this is a modified version of the primary vertex finder of Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    changes :
	      06/12/2006   Kirill.Prokofiev@cern.ch 
	      EDM cleanup and switching to the FitQuality use 

              2016-04-26   David Shope <david.richard.shope@cern.ch>
              EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex

                findVertex will now always return an xAOD::VertexContainer,
                even when using a TrackCollection or a TrackParticleBaseCollection
                as input.
 
 ***************************************************************************/
#include "InDetPriVxFinderTool/InDetAdaptivePriVxFinderTool.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParameters/TrackParameters.h"
#include <map>
#include <vector>
#include <utility>
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

//#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "DataModel/DataVector.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"

#include "TrkTrackLink/ITrackLink.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
//#define INDETADAPTIVEPRIVXFINDERTOOL_DEBUG

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"

namespace InDet
{

// namespace {
// void  deleteMeasuredPerigeeIf(bool IsToDelete,const Trk::TrackParameters* & WhatToDelete) {
//     if (IsToDelete) {
//       delete WhatToDelete;
//       WhatToDelete=0;
//     }
//   }
// }

InDetAdaptivePriVxFinderTool::InDetAdaptivePriVxFinderTool(const std::string& t, const std::string& n, const IInterface*  p)
        : AthAlgTool(t,n,p),
          m_iVertexFitter("Trk::AdaptiveVertexFitter"),
	  m_trkFilter("InDet::InDetTrackSelection"),
	  m_VertexEdmFactory("Trk::VertexInternalEdmFactory"),
          m_iBeamCondSvc("BeamCondSvc",n)
{
    declareInterface<IVertexFinder>(this);//by GP: changed from InDetAdaptivePriVxFinderTool to IPriVxFinderTool
    /* Retrieve StoreGate container and tool names from job options */
    declareProperty("VertexFitterTool", m_iVertexFitter);
    declareProperty("TrackSelector",m_trkFilter);
    declareProperty("InternalEdmFactory", m_VertexEdmFactory);
    declareProperty("BeamPositionSvc", m_iBeamCondSvc);
    /* Cuts for track preselection */
}

InDetAdaptivePriVxFinderTool::~InDetAdaptivePriVxFinderTool()
{}

StatusCode InDetAdaptivePriVxFinderTool::initialize()
{
    StatusCode sc;

    /* Get the right vertex fitting tool from ToolSvc */
    if ( m_iVertexFitter.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVertexFitter << endreq;
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

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetAdaptivePriVxFinderTool::findVertex ( const TrackCollection* trackTES )
  {
    // TODO: change trkFilter to allow for this replacement
    /*
    xAOD::Vertex beamposition;
    beamposition.makePrivateStore();
    beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
    beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());
    */

    Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx());

    //---- Start of preselection of tracks ---------------//
    std::vector<const Trk::TrackParameters*> origParameters;
    origParameters.clear();
    for ( TrackCollection::const_iterator itr = trackTES->begin(); itr != trackTES->end(); itr++ )
    {
      if ( m_trkFilter->accept(**itr,&beamposition)==false ) continue;
      origParameters.push_back ( ( *itr )->perigeeParameters() );
    }
    if(msgLvl(MSG::DEBUG)) msg() << "Of " << trackTES->size() << " tracks " << origParameters.size() << " survived the preselection." << endreq;

    //---- do the actual vertex finding on TrackParameters obejcts ---------------//
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> returnContainers = m_findVertex( origParameters );

    //---- validate the element links ---------------//
    for ( xAOD::VertexContainer::const_iterator vxContItr = returnContainers.first->begin() ; vxContItr != returnContainers.first->end() ; vxContItr++ )
    {
      std::vector<Trk::VxTrackAtVertex> * tmpVxTAVtx = &(*vxContItr)->vxTrackAtVertex();
      for ( std::vector<Trk::VxTrackAtVertex>::iterator itr = tmpVxTAVtx->begin(); itr != tmpVxTAVtx->end(); itr++ )
      {
        const Trk::TrackParameters* initialPerigee = ( *itr ).initialPerigee();
        Trk::Track*          correspondingTrack ( 0 );
        // find the track to that perigee ...
        for ( TrackCollection::const_iterator itr1 = trackTES->begin(); itr1 != trackTES->end(); itr1++ )
        {
          if ( initialPerigee == (*itr1)->perigeeParameters() )
          {
            correspondingTrack=(*itr1);
            continue;
          }
        }

        // validate the track link
        if ( correspondingTrack != 0 )
        {
          Trk::LinkToTrack* link = new Trk::LinkToTrack;
          link->setStorableObject( *trackTES );
          link->setElement( correspondingTrack );
          ( *itr ).setOrigTrack ( link );
        }
        else msg(MSG::WARNING) << "No corresponding track found for this initial perigee! Vertex will have no link to the track." << endreq;
        // TODO: also mention that links stored directly in xAOD::Vertices are not set because a TrackCollection was given as input
      }
    }

    return returnContainers;

  }

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetAdaptivePriVxFinderTool::findVertex ( const Trk::TrackParticleBaseCollection* trackTES )
  {
    // TODO: change trkFilter to allow for this replacement
    /*
    xAOD::Vertex beamposition;
    beamposition.makePrivateStore();
    beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
    beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());
    */

    Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx());

    //---- Start of preselection of tracks ---------------//
    std::vector<const Trk::TrackParameters*> origParameters;
    origParameters.clear();

    // unsigned int size = trackTES->size();
    // if (msgLvl(MSG::VERBOSE)) msg() << "TrackParticleBaseContainer @ " << trackTES << endreq;
    // if (msgLvl(MSG::VERBOSE)) msg() << "Size of the container: " << size << endreq;
    for ( Trk::TrackParticleBaseCollection::const_iterator itr  = trackTES->begin(); itr != trackTES->end(); itr++ )
    {
      if ( m_trkFilter->accept(*((*itr)->originalTrack()), &beamposition) == false ) continue;
      origParameters.push_back ( & ( *itr )->definingParameters() );
      // std::cout << "originalPerigee at " << & ( *itr )->definingParameters() << std::endl;
    }

    if(msgLvl(MSG::DEBUG)) msg() << "Of " << trackTES->size() << " tracks "
    << origParameters.size() << " survived the preselection." << endreq;

    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> returnContainers = m_findVertex( origParameters );
    
    // now we have to make the link to the original track ...
    //     unsigned int count ( 1 );
    for ( xAOD::VertexContainer::const_iterator vxContItr = returnContainers.first->begin() ; vxContItr != returnContainers.first->end() ; vxContItr++ )
    {
      // std::cout << "Check vertex " << count << std::endl; count++;
      std::vector<Trk::VxTrackAtVertex> * tmpVxTAVtx = &(*vxContItr)->vxTrackAtVertex();
      for ( std::vector<Trk::VxTrackAtVertex>::iterator itr = tmpVxTAVtx->begin(); itr != tmpVxTAVtx->end(); itr++ )
      {
        const Trk::TrackParameters* initialPerigee = ( *itr ).initialPerigee();
        Trk::TrackParticleBase*    correspondingTrack ( 0 );
        // find the track to that perigee ...
        for ( Trk::TrackParticleBaseCollection::const_iterator itr1 = trackTES->begin(); itr1 != trackTES->end(); itr1++ )
        {
          if ( initialPerigee == &((*itr1)->definingParameters()) )
          {
            // std::cout << "vxtrack has perigee " << *initialPerigee << std::endl;
            // std::cout << "track has perigee " << *((*itr1)->perigeeParameters()) << std::endl;
            correspondingTrack=(*itr1);
            continue;
          }
        }

        if ( correspondingTrack != 0 )
        {
          Trk::LinkToTrackParticleBase* link = new Trk::LinkToTrackParticleBase;
          link->setStorableObject( *trackTES );
          link->setElement ( correspondingTrack );
          ( *itr ).setOrigTrack ( link );
        }
        else if (msgLvl(MSG::WARNING)) msg() << "No corresponding track found for this initial perigee! Vertex will have no link to the track." << endreq;
        // TODO: also mention that links stored directly in xAOD::Vertices are not set because a TrackParticleBaseCollection was given as input
      }
    }

    // log << MSG::VERBOSE << "... end findVertex(const Trk::TrackParticleBaseCollection* )" << endreq;
    return returnContainers;

  }

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetAdaptivePriVxFinderTool::findVertex(const xAOD::TrackParticleContainer* trackParticles)
  {
    ATH_MSG_DEBUG(" Number of input tracks before track selection: " << trackParticles->size());

    xAOD::Vertex beamposition;
    beamposition.makePrivateStore();
    beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
    beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());

    //---- Start of preselection of tracks ---------------//
    std::vector<const Trk::TrackParameters*> origParameters;
    origParameters.clear();

    typedef DataVector<xAOD::TrackParticle>::const_iterator TrackParticleDataVecIter;
    for (TrackParticleDataVecIter itr  = trackParticles->begin(); itr != trackParticles->end(); ++itr) {
      if ( m_trkFilter->accept(**itr,&beamposition) == false ) continue;
      origParameters.push_back ( & ( *itr )->perigeeParameters() );
      ATH_MSG_DEBUG("originalPerigee at " << & ( *itr )->perigeeParameters());
    }

    //beamposition.releasePrivateStore(); //TODO: should I add this here? it was in InDetPriVxFinderTool method

    ATH_MSG_DEBUG("Of " << trackParticles->size() << " tracks " << origParameters.size() << " survived the preselection.");

    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> returnContainers = m_findVertex( origParameters );

    for ( xAOD::VertexContainer::const_iterator vxContItr = returnContainers.first->begin() ; vxContItr != returnContainers.first->end() ; ++vxContItr )
    {
      std::vector<Trk::VxTrackAtVertex> * tmpVxTAVtx = &(*vxContItr)->vxTrackAtVertex();

      //assigning the input tracks to the fitted vertices through VxTrackAtVertices
      for ( std::vector<Trk::VxTrackAtVertex>::iterator itr = tmpVxTAVtx->begin(); itr != tmpVxTAVtx->end(); itr++ )
      {
        const Trk::TrackParameters* initialPerigee = ( *itr ).initialPerigee();
	const xAOD::TrackParticle*    correspondingTrack ( 0 );
        // find the track to that perigee ...
	for (TrackParticleDataVecIter itr1 = trackParticles->begin(); itr1 != trackParticles->end(); ++itr1) 
        {
          if ( initialPerigee == &((*itr1)->perigeeParameters()) )
	  {
            correspondingTrack=(*itr1);
	    continue;
          }
        }
        if ( correspondingTrack != 0 )
        {
          Trk::LinkToXAODTrackParticle* link = new Trk::LinkToXAODTrackParticle;
          link->setStorableObject( *trackParticles );
          link->setElement ( correspondingTrack );
          ( *itr ).setOrigTrack ( link );
        }
        else ATH_MSG_WARNING("No corresponding track found for this initial perigee! Vertex will have no link to the track.");
      } //end of loop over vxTrackAtVertices to assign links

      //now set links to xAOD::TrackParticles directly in xAOD::Vertices
      unsigned int VTAVsize = (*vxContItr)->vxTrackAtVertex().size();
      for (unsigned int i = 0 ; i < VTAVsize ; ++i)
      {
        Trk::VxTrackAtVertex* VTAV = &( (*vxContItr)->vxTrackAtVertex().at(i) );
        //TODO: Will this pointer really hold 0 if no VxTrackAtVertex is found?
        if (not VTAV){
          ATH_MSG_WARNING (" Trying to set link to xAOD::TrackParticle. The VxTrackAtVertex is not found");
          continue;
        }

        Trk::ITrackLink* trklink = VTAV->trackOrParticleLink();

        // See if the trklink is to an xAOD::TrackParticle
        Trk::LinkToXAODTrackParticle* linkToXAODTP = dynamic_cast<Trk::LinkToXAODTrackParticle*>(trklink);
        if (linkToXAODTP)
        {

          //Now set the new link to the xAOD vertex
          (*vxContItr)->addTrackAtVertex(*linkToXAODTP, VTAV->weight());

        } else {
          ATH_MSG_WARNING ("Skipping track. Trying to set link to something else than xAOD::TrackParticle. Neutrals not supported.");
        }
      } //end of loop for setting links to xAOD::TrackParticles directly in xAOD::Vertices

    } //end loop over fitted vertices

    return returnContainers;

  }


  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetAdaptivePriVxFinderTool::m_findVertex ( std::vector<const Trk::TrackParameters*>& origParameters )
  {
    std::vector<Trk::VxTrackAtVertex> * trkAtVtx;

    double vertexPt=0.;
    xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer;
    xAOD::VertexAuxContainer* theVertexAuxContainer = new xAOD::VertexAuxContainer;
    theVertexContainer->setStore( theVertexAuxContainer );

    xAOD::Vertex * myxAODVertex = 0;

    //---- Start of fitting section ------------------------------------------------------//
    if (origParameters.size() >= 1) {
      xAOD::Vertex beamposition;
      beamposition.makePrivateStore(); 
      beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
      beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());
      beamposition.setFitQuality(m_iBeamCondSvc->beamVtx().fitQuality().chiSquared(), m_iBeamCondSvc->beamVtx().fitQuality().doubleNumberDoF());
      myxAODVertex = m_iVertexFitter->fit(origParameters, beamposition);
      /* @TODO? The fit tool does not return tracks chi2 ordered anymore
                We have to do it */

    } else if(msgLvl(MSG::DEBUG)) msg() << "Less than two tracks or fitting without constraint - drop candidate vertex." << endreq;
    // end if preselection for first iteration
 
    if (origParameters.size() >= 1 ) {
      /* Store the primary vertex */
      trkAtVtx = &( myxAODVertex->vxTrackAtVertex() );
      // do a loop through the element links to tracks in myVxCandidate.vxTrackAtVertex[]
      // if ELEMENTLINKS are used
      vertexPt=0.;
      for (unsigned int i = 0; i < trkAtVtx->size() ; ++i) 
      {
	const Trk::TrackParameters* tmpTP = dynamic_cast<const Trk::TrackParameters*> ( ( * ( trkAtVtx ) ) [i].initialPerigee() );
        //Second step: calculating the sunm of the pt's	     
        if(tmpTP) vertexPt += tmpTP->pT();
      }
    } else {
      if (myxAODVertex!=0) { delete myxAODVertex; myxAODVertex=0; }
    }

    
    if (myxAODVertex!=0) {
      theVertexContainer->push_back(myxAODVertex);
      if (msgLvl(MSG::DEBUG)) /* Print info only if requested */ {
	double xVtxError=Amg::error(myxAODVertex->covariancePosition(), 0);
	double yVtxError=Amg::error(myxAODVertex->covariancePosition(), 1);
	double zVtxError=Amg::error(myxAODVertex->covariancePosition(), 2);
        msg() << "PVtx at ("
	    << myxAODVertex->position()[0] << "+/-" << xVtxError << ", "
	    << myxAODVertex->position()[1] << "+/-" << yVtxError << ", "
	    << myxAODVertex->position()[2] << "+/-" << zVtxError << ") with chi2 = "
	    << myxAODVertex->chiSquared() << " ("
	    << myxAODVertex->vxTrackAtVertex().size() << " tracks)" << endreq;
      }
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
        primaryVtx->setVertexType((xAOD::VxType::VertexType)Trk::NoVtx);
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
    for (unsigned int i = 1 ; i < theVertexContainer->size()-1 ; i++)
    {
      (*theVertexContainer)[i]->setVertexType(xAOD::VxType::PileUp);
    }
    
    return std::make_pair(theVertexContainer, theVertexAuxContainer);
}

StatusCode InDetAdaptivePriVxFinderTool::finalize()
{
    return StatusCode::SUCCESS;
}

void InDetAdaptivePriVxFinderTool::m_printParameterSettings()
{
    msg(MSG::INFO) << "VxPrimary initialize(): Parametersettings " << endreq;
    msg(MSG::INFO) << "VertexFitter " << m_iVertexFitter << endreq;
    msg(MSG::INFO) << endreq;
}

void InDetAdaptivePriVxFinderTool::m_SGError(std::string errService)
{
    msg(MSG::FATAL) << errService << " not found. Exiting !" << endreq;
    return;
}


  
} // end namespace InDet

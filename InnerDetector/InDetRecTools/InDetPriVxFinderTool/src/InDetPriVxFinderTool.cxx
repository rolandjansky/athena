/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetPriVxFinderTool.cxx  -  Description
                             -------------------
    begin   : 28-01-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :
              06/12/2006   Kirill.Prokofiev@cern.ch
              EDM cleanup and switching to the FitQuality use

              2016-04-26   David Shope <david.richard.shope@cern.ch>
              EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex

                findVertex will now always return an xAOD::VertexContainer,
                even when using a TrackCollection or a TrackParticleBaseCollection
                as input.

 ***************************************************************************/
#include "InDetPriVxFinderTool/InDetPriVxFinderTool.h"

#include <map>
#include <utility>

#include "TrkEventPrimitives/ParamDefs.h"

#include "TrkTrack/LinkToTrack.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBase.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "InDetRecToolInterfaces/IMultiPVSeedFinder.h"

#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"

#include "GeoPrimitives/GeoPrimitives.h"

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
  //   void  deleteMeasuredPerigeeIf(bool IsToDelete,const Trk::TrackParameters* & WhatToDelete) 
  //   {
  //     if (IsToDelete) 
  //     {
  //       delete WhatToDelete;
  //       WhatToDelete=0;
  //     }
  //   }
  // }


  InDetPriVxFinderTool::InDetPriVxFinderTool ( const std::string& t, const std::string& n, const IInterface*  p )
      : AthAlgTool ( t,n,p ),
        m_iPriVxSeedFinder( "InDet::SlidingWindowMultiSeedFinder" ),
	m_iVertexFitter ( "Trk::FastVertexFitter" ),
        m_trkFilter( "InDet::InDetTrackSelection"),
	m_VertexEdmFactory("Trk::VertexInternalEdmFactory"),
	m_iBeamCondSvc("BeamCondSvc",n),
	m_useBeamConstraint ( false ),
	m_chi2CutMethod ( 1 ),
	m_enableMultipleVertices ( true ),
	m_clusterLength ( 3. ),
	m_maxChi2PerTrack ( 5. ),
	m_createSplitVertices(false),
	m_splitVerticesTrkInvFraction(2)
  {
    declareInterface<IVertexFinder> ( this );//by GP: changed from InDetAdaptivePriVxFinderTool to IPriVxFinderTool
    declareProperty ( "PriVxSeedFinder", m_iPriVxSeedFinder );
    declareProperty ( "VertexFitterTool", m_iVertexFitter );
    declareProperty ( "TrackSelector", m_trkFilter);
    declareProperty("InternalEdmFactory", m_VertexEdmFactory);
    declareProperty ( "useBeamConstraint", m_useBeamConstraint );
    declareProperty ( "chi2CutMethod", m_chi2CutMethod );
    declareProperty ( "maxChi2PerTrack", m_maxChi2PerTrack );
    declareProperty ( "enableMultipleVertices", m_enableMultipleVertices );
    declareProperty ( "clusterLength", m_clusterLength );
    declareProperty ( "BeamPositionSvc", m_iBeamCondSvc);
    declareProperty("createSplitVertices",m_createSplitVertices);
    declareProperty ( "splitVerticesTrkInvFraction", m_splitVerticesTrkInvFraction, "inverse fraction to split tracks (1:N)");
    
  }

  InDetPriVxFinderTool::~InDetPriVxFinderTool()
  {}

  StatusCode InDetPriVxFinderTool::initialize()
  {
    //check if the split was requested and it is still possible to make it
    if (m_createSplitVertices==true && m_useBeamConstraint==true)
    {
      msg(MSG::FATAL) << " Split vertices cannot be obtained if beam spot constraint is true! Change settings..." << endreq;
      return StatusCode::FAILURE;
    }  
  
  
    /* Get the right vertex seed finder tool from ToolSvc */
    // seed finder only needed if multiple vertex finding is on
    if (m_enableMultipleVertices)
    {
      if ( m_iPriVxSeedFinder.retrieve().isFailure() )
      {
        msg(MSG::FATAL) << "Failed to retrieve tool " << m_iPriVxSeedFinder << endreq;
        return StatusCode::FAILURE;
      } else
      {
        msg(MSG::INFO) << "Retrieved tool " << m_iPriVxSeedFinder << endreq;
      }
    } else
    // track selector is only needed if no seeding is done (otherwise the seeder does the track selection)
    {
      if(m_trkFilter.retrieve().isFailure())
      {
        msg(MSG::ERROR) << "Failed to retrieve tool "<<m_trkFilter<<endreq;
        return StatusCode::FAILURE;
      } else
      {
        msg(MSG::INFO) << "Retrieved tool " << m_trkFilter << endreq;
      }
    }

    /* Get the right vertex fitting tool from ToolSvc */
    if ( m_iVertexFitter.retrieve().isFailure() )
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVertexFitter << endreq;
      return StatusCode::FAILURE;
    } else
    {
      msg(MSG::INFO) << "Retrieved tool " << m_iVertexFitter << endreq;
    }

    if ( m_iBeamCondSvc.retrieve().isFailure() )
    {
      msg(MSG::ERROR) << "Could not find BeamCondSvc." << endreq;
      return StatusCode::FAILURE;;
    }

    msg(MSG::INFO) << "Initialization successful" << endreq;
    return StatusCode::SUCCESS;
  }

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetPriVxFinderTool::findVertex ( const TrackCollection* trackTES )
  {
    //    std::cout<<" find vertex called "<<std::endl;

    // TODO: change trkFilter to allow for this replacement
    /*
    xAOD::Vertex beamposition;
    beamposition.makePrivateStore();
    beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
    beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());
    */

    Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx());

    //---- create a vector of track particle base objects ---------------//
    std::vector<const Trk::Track*> origTracks;
    origTracks.clear();
    
    for ( TrackCollection::const_iterator itr  = trackTES->begin(); itr != trackTES->end(); itr++ )
    {
      // if it should not look for multiple vertices it needs to do the track cuts here
      if ( !m_enableMultipleVertices)
      {
        if (m_trkFilter->accept(**itr,&beamposition)==false) continue;
      }
      origTracks.push_back ( *itr );
    }//end of loop over available tracks with pre-selection

    std::vector< std::vector<const Trk::Track *> > seedsVector;

    // put all tracks in one seed if no seeding is wanted
    //unless the split regime is required
    if (!m_enableMultipleVertices)
    {
     
      // std::cout<<"single vertex mode called "<<std::endl;
     
      //in the case of the split vertices, we should make the splitting first.
      //well, off we go
      if(m_createSplitVertices)
      {
     
        //checking that we can actually split it at all
        std::vector<const Trk::Track *> right_seed;
        std::vector<const Trk::Track *> left_seed;

        unsigned int rem_size = origTracks.size();
        //loop over all pre-selected tracks      
        for( std::vector<const Trk::Track *>::const_iterator i = origTracks.begin(); i != origTracks.end(); ++i)
        {
          if(rem_size % m_splitVerticesTrkInvFraction == 0) right_seed.push_back(*i);
          else left_seed.push_back(*i);
          --rem_size;
      
        }//end of loop over all the pre-selected tracks
      
        if(right_seed.size()>1 && left_seed.size()>1)
        {
          seedsVector.push_back(right_seed);
          seedsVector.push_back(left_seed);

          // std::cout<<"Seed vectors of sizes created: "<<right_seed.size()<<" and "<<left_seed.size()<<std::endl;
       
        }//otherwise making the vector empty and thus no seeds produced at all
      
      }else seedsVector.push_back(origTracks); // if not creating split vertices
      
      
    }else{ // if not enabling multiple vertices

      // find vertex seeds
      seedsVector = m_iPriVxSeedFinder->seeds(origTracks);

    }
    if (msgLvl(MSG::DEBUG)) msg() << "Seed vector has " << seedsVector.size() << " seeds." << endreq;

    // fill track particle seeds into a track parameter base format
    std::vector< std::vector<const Trk::TrackParameters*> > origParameters;
    origParameters.clear();

    //this is a loop over all the seeds. Production of the split vertices
    //after the seeding should be done here..
    for (unsigned int icluster = 0 ; icluster < seedsVector.size() ; icluster++)
    {
      if (msgLvl(MSG::DEBUG)) msg() << "Seed vector " << icluster << " has " << seedsVector[icluster].size() << " tracks." << endreq;

      std::vector<const Trk::TrackParameters*> tmpVector;
      for (unsigned int itrack = 0 ; itrack < seedsVector[icluster].size() ; itrack++)
      {
        tmpVector.push_back(seedsVector[icluster].at(itrack)->perigeeParameters());
      }
      if (msgLvl(MSG::DEBUG)) msg() << "Orig parameters " << icluster << " has " << tmpVector.size() << " tracks." << endreq;
      origParameters.push_back(tmpVector);
    }
    
    if (msgLvl(MSG::DEBUG)) msg() << "Orig parameters has " << origParameters.size() << " seeds." << endreq;

    //---- do the actual vertex finding on TrackParameters obejcts ---------------//
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> returnContainers = m_findVertex ( origParameters );

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
        else if (msgLvl(MSG::WARNING)) msg() << "No corresponding track found for this initial perigee! Vertex will have no link to the track." << endreq;
        // TODO: also mention that links stored directly in xAOD::Vertices are not set because a TrackCollection was given as input
      }
    }

    return returnContainers;

  }

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetPriVxFinderTool::findVertex ( const Trk::TrackParticleBaseCollection* trackTES )
  {
    //    std::cout<<"Calling find vertex from trackparticles "<<std::endl;

    // TODO: change trkFilter to allow for this replacement
    /*
    xAOD::Vertex beamposition;
    beamposition.makePrivateStore();
    beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
    beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());
    */

    Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx());

    //---- create a vector of track particle base objects ---------------//
    std::vector<const Trk::TrackParticleBase*> origTrackParticles;
    origTrackParticles.clear();
    for ( Trk::TrackParticleBaseCollection::const_iterator itr  = trackTES->begin(); itr != trackTES->end(); itr++ )
    {
      // if it should not look for multiple vertices it needs to do the track cuts here
      if (!m_enableMultipleVertices)
      {
      
        if (m_trkFilter->accept(*((*itr)->originalTrack()), &beamposition)==false) continue;
      }
      origTrackParticles.push_back ( *itr );
    }//endo of loop over all available trajectories

    std::vector< std::vector<const Trk::TrackParticleBase *> > seedsVector;

    // put all trackparticles in one seed if no seeding is wanted
    if (!m_enableMultipleVertices)
    {
              
      // std::cout<<"single vertex mode called "<<std::endl;
     
      //in the case of the split vertices, we should make the splitting first.
      //well, off we go
      if(m_createSplitVertices)
      {

        // std::cout<<"Split mode called "<<std::endl;
      
        //checking that we can actually split it at all
        std::vector<const Trk::TrackParticleBase *> right_seed;
        std::vector<const Trk::TrackParticleBase *> left_seed;
        unsigned int rem_size = origTrackParticles.size();
      
        // std::cout<<"Size of the original vector is: "<<rem_size <<std::endl;
 
        //loop over all pre-selected tracks      
        for( std::vector<const Trk::TrackParticleBase *>::const_iterator i = origTrackParticles.begin(); 
              i != origTrackParticles.end(); ++i)
        {
          if(rem_size % m_splitVerticesTrkInvFraction == 0) right_seed.push_back(*i);
          else left_seed.push_back(*i);
          --rem_size;
      
        }//end of loop over all the pre-selected tracks

        if(right_seed.size() && left_seed.size())
        {
          seedsVector.push_back(right_seed);
          seedsVector.push_back(left_seed);
       
          //       std::cout<<"First seed size: "<< right_seed.size()<<std::endl;
          //       std::cout<<"Second seed size: "<< left_seed.size()<<std::endl;
        }
     
      }else seedsVector.push_back(origTrackParticles); //pushing back all the trajectories - single vertes

    }else{ // if not enabling multiple vertices

      // find vertex seeds
      seedsVector = m_iPriVxSeedFinder->seeds(origTrackParticles);

    }
    //     if (msgLvl(MSG::DEBUG)) msg() << "Seed vector has " << seedsVector.size() << " seeds." << endreq;
    // fill track particle seeds into a track parameter base format
    //    std::cout<<"Seeds produced "<<std::endl;
    
    std::vector< std::vector<const Trk::TrackParameters*> > origParameters;
    origParameters.clear();
    for (unsigned int icluster = 0 ; icluster < seedsVector.size() ; icluster++)
    {
      //       if (msgLvl(MSG::DEBUG)) msg() << "Seed vector " << icluster << " has " << seedsVector[icluster].size() << " tracks." << endreq;
      std::vector<const Trk::TrackParameters*> tmpVector;
      for (unsigned int itrack = 0 ; itrack < seedsVector[icluster].size() ; itrack++)
      {
        tmpVector.push_back(&(seedsVector[icluster].at(itrack)->definingParameters()));
      }
      //       if (msgLvl(MSG::DEBUG)) msg() << "Orig parameters " << icluster << " has " << tmpVector.size() << " tracks." << endreq;
      origParameters.push_back(tmpVector);
    }
    
    //     if (msgLvl(MSG::DEBUG)) msg() << "Orig parameters has " << origParameters.size() << " seeds." << endreq;

    // find vertices from the seeds
    
    //    std::cout<<"Calling parameters based find vertices "<<std::endl;
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

    // std::cout<<"returning the container back to the user "<<std::endl;
    return returnContainers;
  }

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetPriVxFinderTool::findVertex(const xAOD::TrackParticleContainer* trackParticles)
  {
    ATH_MSG_DEBUG(" Number of input tracks before track selection: " << trackParticles->size());
   
    xAOD::Vertex beamposition;
    beamposition.makePrivateStore();
    beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
    beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());	  

    std::vector<const xAOD::TrackParticle*> origTPs;
    origTPs.clear();
    std::vector<const Trk::TrackParameters*> origParameters;
    origParameters.clear();
   


    typedef DataVector<xAOD::TrackParticle>::const_iterator TrackParticleDataVecIter;
    for (TrackParticleDataVecIter itr  = trackParticles->begin(); itr != trackParticles->end(); ++itr) {
      if (!m_enableMultipleVertices){
	if (m_trkFilter->accept(**itr,&beamposition)==false) continue;
      }
      origTPs.push_back (*itr);
      origParameters.push_back ( & ( *itr )->perigeeParameters() );
      ATH_MSG_DEBUG("originalPerigee at " << & ( *itr )->perigeeParameters());
    }
    
    beamposition.releasePrivateStore();
    std::vector< std::vector<const Trk::TrackParameters*> > seedsVector;

    if (!m_enableMultipleVertices)
    {
	
      //     std::cout<<"single vertex mode called "<<std::endl;
	
      //in the case of the split vertices, we should make the splitting first.
      //well, of fwe go
      if(m_createSplitVertices)
      {
	//checking that we can actually split it at all
	std::vector<const Trk::TrackParameters*> right_seed;
	std::vector<const Trk::TrackParameters*> left_seed;	
	unsigned int rem_size = origParameters.size();
	//loop over all pre-selected tracks     
	for( std::vector<const Trk::TrackParameters*>::const_iterator i = origParameters.begin(); i != origParameters.end(); ++i)
	{
	  if(rem_size % m_splitVerticesTrkInvFraction == 0) right_seed.push_back(*i);
	  else left_seed.push_back(*i);
	  --rem_size;
	
	}//end of loop over all the pre-selected tracks
	
	if(right_seed.size()>1 && left_seed.size()>1)
	{
	  seedsVector.push_back(right_seed);
	  seedsVector.push_back(left_seed);
	
	  // std::cout<<"Seed vectors of sizes created: "<<right_seed.size()<<" and "<<left_seed.size()<<std::endl;
	
	}//otherwise making the vector empty and thus no seeds produced at all
	    
	    
      }else seedsVector.push_back(origParameters); // if not creating split vertices
	
	
    }else{ // if not enabling multiple vertices

      // find vertex seeds	
      //the seed finder taking TrackParamters is not yet actually implemented... just the interface!
      seedsVector = m_iPriVxSeedFinder->seeds(origTPs);	      

    }
    
    if (msgLvl(MSG::DEBUG)) msg() << "Seed vector has " << seedsVector.size() << " seeds." << endreq;

    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> returnContainers = m_findVertex( seedsVector );

    //---- validate the element links ---------------//
    for ( xAOD::VertexContainer::const_iterator vxContItr = returnContainers.first->begin() ; vxContItr != returnContainers.first->end() ; vxContItr++ )
    {
      std::vector<Trk::VxTrackAtVertex> * tmpVxTAVtx = &(*vxContItr)->vxTrackAtVertex();

      //assigning the input tracks to the fitted vertices through VxTrackAtVertices
      for ( std::vector<Trk::VxTrackAtVertex>::iterator itr = tmpVxTAVtx->begin(); itr != tmpVxTAVtx->end(); itr++ )
      {
        const Trk::TrackParameters* initialPerigee = ( *itr ).initialPerigee();
    	const xAOD::TrackParticle*    correspondingTrack ( 0 );
    	// find the track to that perigee ...
    	for (TrackParticleDataVecIter itr1  = trackParticles->begin(); itr1 != trackParticles->end(); ++itr1)
    	{
          if ( initialPerigee ==  & (*itr1)->perigeeParameters() )
    	  {
    	    correspondingTrack=(*itr1);
    	    continue;
    	  }
    	}

    	// validate the track link
    	if ( correspondingTrack != 0 )
    	{
    	  Trk::LinkToXAODTrackParticle* link = new Trk::LinkToXAODTrackParticle;
    	  link->setStorableObject( *trackParticles );
    	  link->setElement( correspondingTrack );
    	  ( *itr ).setOrigTrack ( link );
    	}
    	else if (msgLvl(MSG::WARNING)) msg() << "No corresponding track found for this initial perigee! Vertex will have no link to the track." << endreq;
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
  

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetPriVxFinderTool::m_findVertex ( std::vector< std::vector<const Trk::TrackParameters*> >& zTrackColl )
  {
  
    //   std::cout<<"Starting the main part of the finding "<<std::endl;
   
    //---- Constraint vertex section: if enabled in jobOptions a constraint is assigned --//    
    Amg::Vector3D vertex = Amg::Vector3D( 0.,0.,0. ); //for fit() we need Amg::Vector3D or Trk::RecVertex
    std::vector<Trk::VxTrackAtVertex> * trkAtVtx=nullptr;

    // Finding hot spots of z0's in case of pile up.
    std::vector<const Trk::TrackParameters*> zTracks;
    typedef std::vector<const Trk::TrackParameters*>::const_iterator zTrkIter;
    zTracks.clear();
    
    typedef std::vector<std::vector<const Trk::TrackParameters* > >::iterator zTrkCollIter;

    // VertexContainer which takes xAOD::Vertex and is stored in StoreGate
    xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer;
    xAOD::VertexAuxContainer* theVertexAuxContainer = new xAOD::VertexAuxContainer;
    theVertexContainer->setStore( theVertexAuxContainer );

    std::map<double,xAOD::Vertex*> vertexMap;
    std::vector<xAOD::Vertex*> splitVtxVector;
    
    double vertexPt;

    xAOD::Vertex * myxAODVertex = 0;
    // std::cout<<"Getting to the loop;  size is: "<< zTrackColl.size()<<std::endl;

    for ( unsigned int i=0; i<zTrackColl.size(); i++ )
    {
    
      // std::cout<<"Inside the loop"<<std::endl;
      if (msgLvl(MSG::DEBUG)) msg() << "Fitting vertex of Z-Cluster " << i << " with " << zTrackColl[i].size() << " Tracks" << endreq;
      myxAODVertex = 0;
      std::vector<const Trk::TrackParameters*> origParameters;
      origParameters.clear();
      origParameters=zTrackColl[i];

      //---- Start of fitting section ------------------------------------------------------//
      if ( origParameters.size() == 1 && m_useBeamConstraint )
      {

        xAOD::Vertex theconstraint;
        theconstraint.makePrivateStore();
        theconstraint.setPosition( m_iBeamCondSvc->beamVtx().position() );
        theconstraint.setCovariancePosition( m_iBeamCondSvc->beamVtx().covariancePosition() );
        theconstraint.setFitQuality( m_iBeamCondSvc->beamVtx().fitQuality().chiSquared(), m_iBeamCondSvc->beamVtx().fitQuality().doubleNumberDoF() );
        myxAODVertex = m_iVertexFitter->fit ( origParameters, theconstraint );

      }
      else if(origParameters.size() < 2  && m_createSplitVertices)
      {
        // in the case this is a split vertex and it has only one track      
        // we make a dummy vertex and push it back to the container       
       
        myxAODVertex = new xAOD::Vertex;
        myxAODVertex->makePrivateStore();
        myxAODVertex->setPosition( m_iBeamCondSvc->beamVtx().position() );
        myxAODVertex->setCovariancePosition( m_iBeamCondSvc->beamVtx().covariancePosition() );
        myxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
        myxAODVertex->setVertexType(xAOD::VxType::NoVtx);
	
        // std::cout<<"this is a case when the split seeds are too small, dummy returned"<<std::endl;
	
      }
      else if ( origParameters.size() > 1 )
      {
        // std::cout<<"choosing 2nd option"<<std::endl;
        
        // if(msgLvl(MSG::VERBOSE)) msg() << "First call of fitting tool!" << endreq;
        if ( m_useBeamConstraint )
        {
          xAOD::Vertex theconstraint;
          theconstraint.makePrivateStore();
          theconstraint.setPosition( m_iBeamCondSvc->beamVtx().position() );
          theconstraint.setCovariancePosition( m_iBeamCondSvc->beamVtx().covariancePosition() );
          theconstraint.setFitQuality( m_iBeamCondSvc->beamVtx().fitQuality().chiSquared(), m_iBeamCondSvc->beamVtx().fitQuality().doubleNumberDoF() );
          myxAODVertex = m_iVertexFitter->fit ( origParameters, theconstraint );
        }
        else
        {
          myxAODVertex = m_iVertexFitter->fit ( origParameters, vertex );
        }

        if ( myxAODVertex != 0 )
        {
          // std::cout<< "non-zero xAOD::Vertex!"<<std::endl;
	  
          /* Get the vertex position */
          // std::cout<< "position: "<< myxAODVertex->position() <<std::endl;
	  
          Amg::Vector3D vertex ( myxAODVertex->position() );
          trkAtVtx = &( myxAODVertex->vxTrackAtVertex() );
          // std::cout<< "number of fitted tracks"<< trkAtVtx->size()<< std::endl;
	  
          /* The fit tool does not return tracks chi2 ordered anymore We have to do it */
          std::vector<int> indexOfSortedChi2;
          // std::cout<< "Calling the sort " <<std::endl;
	  
          m_sortTracksInChi2 ( indexOfSortedChi2,myxAODVertex );
         
          // std::cout<< "Sort done " <<std::endl;


	   
          /*
            If more than 2 tracks were used,
            do a chi2 selection of tracks used in the fit:
            Version 1:
            - get rid of all tracks with chi2 > m_maxChi2PerTrack in one go
            - no refit after removing one track
            - refit after all tracks with too high chi2 were removed
          */
          if ( m_chi2CutMethod == 1 && origParameters.size() >2 )
          {  
	  
            origParameters.clear();
            /* At least two tracks are kept anyway  (0 and 1)
               The others (2, ...) are tested for their chi2 values */
            // first track
            Trk::VxTrackAtVertex* tmpVTAV = &( *trkAtVtx ) [indexOfSortedChi2[0]];
            origParameters.push_back ( tmpVTAV->initialPerigee() );

            // second track
            tmpVTAV = &( *trkAtVtx ) [indexOfSortedChi2[1]];
            origParameters.push_back ( tmpVTAV->initialPerigee() );
	    
            for ( unsigned int i=2; i<trkAtVtx->size(); ++i )
            {
              if ( ( *trkAtVtx ) [indexOfSortedChi2[i]].trackQuality().chiSquared() > m_maxChi2PerTrack )
                continue;
              tmpVTAV = &( *trkAtVtx ) [indexOfSortedChi2[i]];
              origParameters.push_back ( tmpVTAV->initialPerigee() );
            }
	    

            // delete old xAOD::Vertex first
            if ( myxAODVertex!=0 ) { delete myxAODVertex; myxAODVertex=0; }
         
            if(msgLvl(MSG::VERBOSE)) msg() << "Second call of fitting tool!" << endreq;
            if ( m_useBeamConstraint )
	    {
              xAOD::Vertex theconstraint;
              theconstraint.makePrivateStore();
              theconstraint.setPosition( m_iBeamCondSvc->beamVtx().position() );
              theconstraint.setCovariancePosition( m_iBeamCondSvc->beamVtx().covariancePosition() );
              theconstraint.setFitQuality( m_iBeamCondSvc->beamVtx().fitQuality().chiSquared(), m_iBeamCondSvc->beamVtx().fitQuality().doubleNumberDoF() );
	      myxAODVertex = m_iVertexFitter->fit ( origParameters, theconstraint );
	    }
            else
	    {
	      myxAODVertex = m_iVertexFitter->fit ( origParameters, vertex );
	    }

	    if(myxAODVertex)
	    {
              trkAtVtx = &( myxAODVertex->vxTrackAtVertex() );
	    }   
          }//end of chi2 cut method 1       


	  
          /*
            Version 2:
            - get rid of tracks one by one starting with the one with highest chi2 > m_maxChi2PerTrack
            - refit after this track has been removed
            and do a chi2 cut again until all chi2 < m_maxChi2PerTrack
          */
          else if ( m_chi2CutMethod == 2 && origParameters.size() >2 )
          {
	  
            while ( ( *trkAtVtx ) [indexOfSortedChi2[origParameters.size()-1]].trackQuality().chiSquared() > m_maxChi2PerTrack && origParameters.size() >= 3)
            {
              
              // std::cout<<"Next while iteration "<<std::endl;
              // no pop back because origParameters vector is not ordered in chi2 anymore
              origParameters.erase ( origParameters.begin() + ( * ( indexOfSortedChi2.end()-1 ) ) );

              // delete old xAOD::Vertex first
              delete myxAODVertex; myxAODVertex=0; 
              
	 
              if(msgLvl(MSG::VERBOSE)) msg() << "Second call of fitting tool!" << endreq;
               
	      if ( m_useBeamConstraint )
              {
                xAOD::Vertex theconstraint;
                theconstraint.makePrivateStore();
                theconstraint.setPosition( m_iBeamCondSvc->beamVtx().position() );
                theconstraint.setCovariancePosition( m_iBeamCondSvc->beamVtx().covariancePosition() );
                theconstraint.setFitQuality( m_iBeamCondSvc->beamVtx().fitQuality().chiSquared(), m_iBeamCondSvc->beamVtx().fitQuality().doubleNumberDoF() );
                myxAODVertex = m_iVertexFitter->fit ( origParameters, theconstraint );
              }
              else
              {
                myxAODVertex = m_iVertexFitter->fit ( origParameters, vertex );
              }
	       
	      if(myxAODVertex)
	      {
                trkAtVtx = &( myxAODVertex->vxTrackAtVertex() );

                //re-soring tracks to avoid gaps
                m_sortTracksInChi2 ( indexOfSortedChi2, myxAODVertex );
	      }else{
	      
                //we're down to non-fittable combination. so far, just give up. In future one may 
                // remove 2 worst tracks and re-start, for instance 
                break;	      

              } //end of existance of xAOD::Vertex check	      
	    }//end of while stateme
          } //end of mode selection
        } else  if (msgLvl(MSG::DEBUG))
          msg() << "FastVertexFitter returned zero pointer ... could not fit vertex for this z cluster!" << endreq;
      } // end if "more than one track after m_preSelect(...)"
      else if (msgLvl(MSG::DEBUG)) msg() << "Less than two tracks or fitting without constraint - drop candidate vertex." << endreq;
      // end if preselection for first iteration
      
      if ( ( origParameters.size() > 1 || ( m_useBeamConstraint && origParameters.size() == 1 ) ) && myxAODVertex && !m_createSplitVertices)
      {
        if(msgLvl(MSG::VERBOSE)) msg() << "Storing the fitted vertex." << endreq;
        
	/* Store the primary vertex */
        trkAtVtx = &( myxAODVertex->vxTrackAtVertex() );

        vertexPt=0.;
        for ( unsigned int i = 0; i < trkAtVtx->size() ; ++i )
        {
          const Trk::TrackParameters* tmpTP = dynamic_cast<const Trk::TrackParameters*> ( ( * ( trkAtVtx ) ) [i].initialPerigee() );
          if(tmpTP) vertexPt += tmpTP->pT();
        }

        vertexMap[vertexPt]=myxAODVertex;
      } else if(m_createSplitVertices){ 
      
        //storing a split vertex, if did not work - storing a dummy      
        if(myxAODVertex) 
        {
          // std::cout<<"Normal xAOD::Vertex found in the split mode "<<std::endl;
          myxAODVertex->setVertexType(xAOD::VxType::PriVtx);
          splitVtxVector.push_back(myxAODVertex);
        }
        else{
          // std::cout<<"No candidate reconstructed, storing the dummy "<<std::endl;
          xAOD::Vertex * dummyxAODVertex = new xAOD::Vertex;
          dummyxAODVertex->makePrivateStore();
          dummyxAODVertex->setPosition( m_iBeamCondSvc->beamVtx().position() );
          dummyxAODVertex->setCovariancePosition( m_iBeamCondSvc->beamVtx().covariancePosition() );
          dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
          dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
          splitVtxVector.push_back(dummyxAODVertex);
        }//end of successful reconstruction check
      }else if ( myxAODVertex) { delete myxAODVertex; myxAODVertex=0; }
    }//end of loop over the pre-defined seeds

    //no sorting for the split vertices  -otherwise, why splitting at all?
    // std::cout<<"Past loop over the clusters "<<std::endl;
    
    
    if(!m_createSplitVertices)
    {
      for ( std::map<double,xAOD::Vertex*>::reverse_iterator i=vertexMap.rbegin(); i!=vertexMap.rend();i++ )
      {
        if(msgLvl(MSG::VERBOSE)) msg() << "Sorting the fitted vertices. " << vertexMap.size() << " have been found." << endreq;
        theVertexContainer->push_back ( ( *i ).second );
        if(msgLvl(MSG::DEBUG)) /* Print info only if requested */
        {
     	  double xVtxError=sqrt ( ( *i ).second->covariancePosition()(0,0) );
     	  double yVtxError=sqrt ( ( *i ).second->covariancePosition()(1,1) );
     	  double zVtxError=sqrt ( ( *i ).second->covariancePosition()(2,2) );
     	  msg() << "PVtx at ("
     	  << ( *i ).second->position() [0] << "+/-" << xVtxError << ", "
     	  << ( *i ).second->position() [1] << "+/-" << yVtxError << ", "
     	  << ( *i ).second->position() [2] << "+/-" << zVtxError << ") with chi2 = "
     	  << ( *i ).second->chiSquared() << " ("
     	  << ( *i ).second->vxTrackAtVertex().size() << " tracks)" << endreq;
        }
      }//end of sorting loop
    } else for(std::vector<xAOD::Vertex *>::iterator l_vt = splitVtxVector.begin(); l_vt != splitVtxVector.end();++l_vt)
    	theVertexContainer->push_back(*l_vt);
    
    //     std::cout<<"Past the second loop "<<std::endl;
    //---- add dummy vertex at the end ------------------------------------------------------//
    //---- if one or more vertices are already there: let dummy have same position as primary vertex
    if ( theVertexContainer->size() >= 1 && !m_createSplitVertices)
    {
      xAOD::Vertex * primaryVtx = theVertexContainer->front();
      primaryVtx->setVertexType(xAOD::VxType::PriVtx);
      xAOD::Vertex * dummyxAODVertex = new xAOD::Vertex;
      theVertexContainer->push_back( dummyxAODVertex ); // have to add vertex to container here first so it can use its aux store
      dummyxAODVertex->setPosition( primaryVtx->position() );
      dummyxAODVertex->setCovariancePosition( primaryVtx->covariancePosition() );
      dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
      dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
    }else if(theVertexContainer->size() == 1 && m_createSplitVertices)
    {
      //if there's ony one vertex and we make a split, we create an additional dummy vertex.      
      xAOD::Vertex * dummyxAODVertex = new xAOD::Vertex;
      theVertexContainer->push_back( dummyxAODVertex ); // have to add vertex to container here first so it can use its aux store
      dummyxAODVertex->setPosition( m_iBeamCondSvc->beamVtx().position() );
      dummyxAODVertex->setCovariancePosition( m_iBeamCondSvc->beamVtx().covariancePosition() );
      dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
      dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
    }

    //---- if no vertex is there let dummy be at beam spot
    else if ( theVertexContainer->size() == 0 )
    {
      // std::cout<<"Zero size vx container! "<<std::endl;
      xAOD::Vertex * dummyxAODVertex = new xAOD::Vertex;
      theVertexContainer->push_back( dummyxAODVertex ); // have to add vertex to container here first so it can use its aux store
      dummyxAODVertex->setPosition( m_iBeamCondSvc->beamVtx().position() );
      dummyxAODVertex->setCovariancePosition( m_iBeamCondSvc->beamVtx().covariancePosition() );
      dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
      dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
      if(m_createSplitVertices)
      {
        // adding a second dummy vertex if this was a split run
        xAOD::Vertex * new_dummyxAODVertex = new xAOD::Vertex;
        theVertexContainer->push_back( new_dummyxAODVertex ); // have to add vertex to container here first so it can use its aux store
        new_dummyxAODVertex->setPosition( m_iBeamCondSvc->beamVtx().position() );
        new_dummyxAODVertex->setCovariancePosition( m_iBeamCondSvc->beamVtx().covariancePosition() );
        new_dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
        new_dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
      }
    }

    // loop over the pile up to set it as pile up (EXCLUDE first and last vertex: loop from 1 to size-1)
    if(!m_createSplitVertices)
    {
      for (unsigned int i = 1 ; i < theVertexContainer->size()-1 ; i++)
      {
        (*theVertexContainer)[i]->setVertexType(xAOD::VxType::PileUp);
      } 
    }

    // std::cout<<"returning the container "<<std::endl;
    return std::make_pair(theVertexContainer, theVertexAuxContainer);
  }//end m_find vertex ethod

  StatusCode InDetPriVxFinderTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  void InDetPriVxFinderTool::m_sortTracksInChi2 ( std::vector<int>& indexOfSortedChi2, xAOD::Vertex * myxAODVertex )
  {
    // we need an index vector here which tells us the right order from smallest to
    // largest of the chi2PerTrack vector
    // then loop over the index vector and replace all iRP with iRP = index[i]
    std::map<double, int> mapOfChi2;
    
    //  std::cout<<"entering the sort "<<std::endl;
    //  std::cout<<"vertex pointer is: "<<myVxCandidate<<std::endl;
    //  if(!myVxCandidate->vxTrackAtVertex()) std::cout<<"empty veretx?? "<<std::endl;
    //  if(!myVxCandidate->vxTrackAtVertex()->size()) std::cout<<"No tracks ate vertex? "<<std::endl;
    //  std::cout<<"And the size of the container is: "<<myVxCandidate->vxTrackAtVertex()->size()<<std::endl;
    for ( unsigned int i=0; i < myxAODVertex->vxTrackAtVertex().size(); ++i )
    {
      /* possible bug 1 */
      // TODO: I don't think vxTrackAtVertexAvailable() does the same thing as a null pointer check!
      // if(myxAODVertex->vxTrackAtVertexAvailable()) std::cout<<"vxTrackAtVertex is not available! "<<std::endl;
      // else std::cout<<"non zero pointer 1"<<std::endl;
      double chi2PerTrack = ( myxAODVertex->vxTrackAtVertex() ) [i].trackQuality().chiSquared();
      // std::cout<<"The chi2 valie is "<<chi2PerTrack<<std::endl;
      mapOfChi2.insert ( std::map<double, int>::value_type ( chi2PerTrack, i ) );
    }
    // std::cout<<"sort loop done "<<std::endl;
    indexOfSortedChi2.clear();
    std::map<double, int>::const_iterator mItr = mapOfChi2.begin();

    for ( ; mItr != mapOfChi2.end() ; ++mItr )
    {
      //  std::cout<<"Inside the loop "<<std::endl;
      indexOfSortedChi2.push_back ( ( *mItr ).second );
    }
    // std::cout<<"Sorting performed "<<std::endl;
    
  }//end of sort method

  void InDetPriVxFinderTool::m_sortTracksInZ0 ( std::vector<const Trk::TrackParameters*> tv,std::vector<int>& indexOfSortedZ0 )
  {
    // we need an index vector here which tells us the right order from smallest to
    // largest of the z0 vector
    // then loop over the index vector and replace all iRP with iRP = index[i]
    std::map<double, int> mapOfZ0;
    unsigned int j=0;
    for ( std::vector<const Trk::TrackParameters*>::const_iterator i=tv.begin(); i != tv.end(); i++ )
    {
      /* possible bug 1 */
      const Trk::TrackParameters* tmpTP = dynamic_cast<const Trk::TrackParameters*> ( *i );
      double z0 = tmpTP->position() [Trk::z];
      mapOfZ0.insert ( std::map<double, int>::value_type ( z0, j ) );
      j++;
    }
    indexOfSortedZ0.clear();
    std::map<double, int>::const_iterator mItr = mapOfZ0.begin();
    for ( ; mItr != mapOfZ0.end() ; ++mItr )
    {
      indexOfSortedZ0.push_back ( ( *mItr ).second );
    }
  }

}
// end namespace InDet

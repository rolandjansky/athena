/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/***************************************************************************
              AdaptiveMultiVertexFitter.cxx  -  Description in header file
 ***************************************************************************/

#include "TrkVertexFitters/AdaptiveMultiVertexFitter.h"

#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "TrkVertexFitterInterfaces/IVertexTrackCompatibilityEstimator.h"
#include "TrkVertexFitterInterfaces/IVertexUpdator.h"
#include "TrkVertexFitterInterfaces/IVertexSmoother.h"
#include "TrkVertexFitterInterfaces/IVertexAnnealingMaker.h"
#include "TrkVertexFitterInterfaces/IImpactPoint3dEstimator.h"


#include "VxMultiVertex/MvfFitInfo.h"
#include "VxMultiVertex/MVFVxTrackAtVertex.h"
#include "VxVertex/LinearizedTrack.h"
#include "VxMultiVertex/TrackToVtxLink.h"
#include <limits>

// xAOD Includes
#include "xAODTracking/Vertex.h"

//#define MULTIFITTER_DEBUG
//#define DEBUG_2

namespace Trk
{

  AdaptiveMultiVertexFitter::AdaptiveMultiVertexFitter(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_maxIterations(30),
    m_maxDistToLinPoint(0.5),
    m_initialError(0.0001),
    m_doSmoothing(false),
    m_minweight(0.001),
    m_maxRelativeShift(0.01),
    m_LinearizedTrackFactory("Trk::FullLinearizedTrackFactory"),
    m_TrackCompatibilityEstimator("Trk::Chi2TrackCompatibilityEstimator"),
    m_ImpactPoint3dEstimator("Trk::ImpactPoint3dEstimator/ImpactPoint3dEstimator"),
    m_VertexUpdator("Trk::KalmanVertexUpdator"),
    m_VertexSmoother("Trk::SequentialVertexSmoother"),
    m_AnnealingMaker("Trk::DetAnnealingMaker")
  {
    declareProperty("MaxIterations",        m_maxIterations);
    declareProperty("MaxDistToLinPoint",    m_maxDistToLinPoint);
    declareProperty("InitialError",m_initialError);
    declareProperty("LinearizedTrackFactory",m_LinearizedTrackFactory);
    declareProperty("TrackCompatibilityEstimator",m_TrackCompatibilityEstimator);
    declareProperty("ImpactPoint3dEstimator",m_ImpactPoint3dEstimator);
    declareProperty("VertexUpdator",m_VertexUpdator);
    declareProperty("VertexSmoother",m_VertexSmoother);
    declareProperty("AnnealingMaker",m_AnnealingMaker);
    declareProperty("DoSmoothing",m_doSmoothing);
    declareProperty("MinWeight",m_minweight);
    declareProperty("MaxRelativeShift",m_maxRelativeShift);
    declareInterface<AdaptiveMultiVertexFitter>(this);
  }

  AdaptiveMultiVertexFitter::~AdaptiveMultiVertexFitter() {}

  StatusCode AdaptiveMultiVertexFitter::initialize()
  {
    
    if ( m_LinearizedTrackFactory.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_LinearizedTrackFactory << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_LinearizedTrackFactory << endmsg;
    }

    if ( m_TrackCompatibilityEstimator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_TrackCompatibilityEstimator << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_TrackCompatibilityEstimator << endmsg;
    }
    
    if ( m_ImpactPoint3dEstimator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_ImpactPoint3dEstimator << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_ImpactPoint3dEstimator << endmsg;
    }
   
    if ( m_VertexUpdator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexUpdator << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_VertexUpdator << endmsg;
    }
   
    //loading smoother in case required   
    if(m_doSmoothing) {
      if ( m_VertexSmoother.retrieve().isFailure() ) {
        msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexSmoother << endmsg;
        return StatusCode::FAILURE;
      } else {
        msg(MSG::INFO) << "Retrieved tool " << m_VertexSmoother << endmsg;
      }
    }//end of smoothing options
   
    if ( m_AnnealingMaker.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_AnnealingMaker << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_AnnealingMaker << endmsg;
    }
   
    msg(MSG::INFO)  << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  StatusCode AdaptiveMultiVertexFitter::finalize()
  {
    msg(MSG::INFO)  << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;
  }

  void AdaptiveMultiVertexFitter::fit(std::vector<xAOD::Vertex*> & allvertexes) 
  {

    // TODO: put this in a better place
    // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
    // For optimization of access speed
    xAOD::Vertex::Decorator< MvfFitInfo* > MvfFitInfo("MvfFitInfo");
    xAOD::Vertex::Decorator< bool > isInitialized("isInitialized");
    xAOD::Vertex::Decorator< std::vector<Trk::VxTrackAtVertex*> > VTAV("VTAV");
 
    msg(MSG::DEBUG) << " Now fitting all vertices " << endmsg;
 
    std::vector<xAOD::Vertex*>::iterator vtxbegin=allvertexes.begin();
    std::vector<xAOD::Vertex*>::iterator vtxend=allvertexes.end();
    
    //create a vector of vertices, to store the old position...
    std::map<xAOD::Vertex*,Amg::Vector3D> oldpositions;
    std::map<xAOD::Vertex*,bool> relinearizations;


    //count number of steps
    int num_steps(0);
    
    //reset the annealing
    m_AnnealingMaker->reset();
    #ifdef MULTIFITTER_DEBUG
    std::cout << "Entering cycle of AdaptiveMultiVertexFitter" << std::endl;
    #endif

    // TODO: get rid of following line
    //std::cout << "Entering cycle of AdaptiveMultiVertexFitter to fit " << allvertexes.size() << " vertices." << std::endl;

    bool shiftIsSmall(true);

    //now start to iterate
    do {

      // TODO: get rid of following line
      //std::cout << "step number: " << num_steps << std::endl;

      for (std::vector<xAOD::Vertex*>::iterator iter=vtxbegin;iter!=vtxend;++iter) {
	
	//now store all the "old positions"; if vertex is added for the first time this corresponds to the 
	//seed (at the same time fitted vertex will be updated with the constraint information)
	//check if you need to reestimate compatibility + linearization
        #ifdef MULTIFITTER_DEBUG
	std::cout << "Now considering candidate with ptr " << *iter << std::endl;
        #endif
	relinearizations[*iter]=false;
	if (isInitialized( *(*iter) )) {
          #ifdef MULTIFITTER_DEBUG
	  std::cout << "Candidate already has a position: storing it in oldpositions " << std::endl;
	  std::cout << "vertex position z: " << (**iter).position()[2] << std::endl;
          #endif
	  oldpositions[*iter] = (*iter)->position();
	} else {
	  isInitialized( *(*iter) ) = true;
          #ifdef MULTIFITTER_DEBUG
	  std::cout << "Candidate has no position so far: using as old position the seedVertex" << std::endl;
          #endif
	  if (MvfFitInfo( *(*iter) )->seedVertex()==0) { // TODO: Is there a way of checking whether a decoration exists on an object?
	    msg(MSG::ERROR) << "Candidate has no seed...CRASHING now!!!" << endmsg;
	  }
	  oldpositions[*iter] = *(MvfFitInfo( *(*iter) )->seedVertex());
	  //	(*iter)->recVertex()=new RecVertex(*((*iter)->constraintVertex)); // TODO: outdated, but already previously commented out
	}
	if (MvfFitInfo( *(*iter) )->linearizationVertex()==0) { // TODO: Is there a way of checking whether a decoration exists on an object?
	  msg(MSG::ERROR) << " Candidate has no linearization point...CRASHING now!!! "<<endmsg;
	}
	//std::cout << "linearization (z) position is: " << MvfFitInfo( *(*iter) ).linearizationVertex()->z() << std::endl;
	if ((oldpositions[*iter] - *MvfFitInfo( *(*iter) )->linearizationVertex()).perp()>m_maxDistToLinPoint) {
          #ifdef MULTIFITTER_DEBUG
	  std::cout << "Candidate has to be relinearized " << std::endl;
          #endif
	  relinearizations[*iter]=true;
	  prepareCompatibility(*iter);
	}

        #ifdef MULTIFITTER_DEBUG
	std::cout << "Setting the Vertex to the initial constraint" << std::endl;
        #endif
	//reput everything to the constraint level
        // TODO: previously,
	//(*iter)->setRecVertex(*(static_cast<Trk::MVFVxCandidate*>(*iter))->vertexFitInfo().constraintVertex());
	//(*iter)->recVertex().scaleError(m_AnnealingMaker->getWeight(1.));

        // TODO: now,
        (*iter)->setPosition( MvfFitInfo( *(*iter) )->constraintVertex()->position() );
        (*iter)->setCovariancePosition( MvfFitInfo( *(*iter) )->constraintVertex()->covariancePosition() );
        (*iter)->setFitQuality( MvfFitInfo(*(*iter))->constraintVertex()->chiSquared(), MvfFitInfo( *(*iter) )->constraintVertex()->numberDoF() );
        (*iter)->setCovariancePosition( (*iter)->covariancePosition() * 1./float(m_AnnealingMaker->getWeight(1.)) );

        #ifdef MULTIFITTER_DEBUG
	std::cout << "Running TrackCompatibilityEstimator on each track" << std::endl;
        #endif
	
	//prepare the iterators for the tracks
        std::vector<Trk::VxTrackAtVertex*>::iterator trkbegin = VTAV( *(*iter) ).begin();
        std::vector<Trk::VxTrackAtVertex*>::iterator trkend = VTAV( *(*iter) ).end();
	
	//iterate and update the vertex with the track information
	for (std::vector<Trk::VxTrackAtVertex*>::iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {
          #ifdef MULTIFITTER_DEBUG
	  std::cout << "Adding compatibility info to a track of " << VTAV( *(*iter) ).size() << std::endl;
          #endif

          // TODO: this was done previously, find out how to check now! (to see if xAOD::Vertex has been properly decorated)
          /*
	  if (dynamic_cast<Trk::MVFVxTrackAtVertex*>(*trkiter)==0) {
	    msg(MSG::ERROR) << "You provided wrong VxTrackAtVertex objects to the adaptive multi vertex fitter " << 
	      endmsg << "They should be MVFVxTrackAtVertex objects, check the finder please" << endmsg <<
	      "------>>>>>>>>>>>>>>    NO FIT done" << endmsg;
	    return;
	  }
          */

          //now recover from cases where the linearization position is !=0, but you added 
          //more tracks later on...

          if ( (*trkiter)->ImpactPoint3dAtaPlane()==0 ) 
          {
            bool success=m_ImpactPoint3dEstimator->addIP3dAtaPlane( **trkiter, *MvfFitInfo( *(*iter) )->linearizationVertex() );
            if (!success)
            {
              msg(MSG::WARNING) << "Adding compatibility to vertex information failed. Newton distance finder didn't converge..." << endmsg;
            }
          }

	  //first -> estimate the compatibility of the track to the vertex
	  m_TrackCompatibilityEstimator->estimate( **trkiter, oldpositions[*iter] );
          #ifdef MULTIFITTER_DEBUG
	  std::cout << "End of compatibility for a track" << std::endl;
          #endif
	}

      }
      #ifdef MULTIFITTER_DEBUG
      std::cout << "Finished first candidates cycle" << std::endl;
      #endif

      //after having estimated the compatibility of all the vertices, you have to run again on all vertices, 
      //to compute the weights
      
      for (std::vector<xAOD::Vertex*>::iterator iter=vtxbegin;iter!=vtxend;++iter) {      

        //TODO: crude and quite possibly time consuming, but best solution I could think of...
        //      updated VxTrackAtVertices are stored in VTAV decoration:
        //      so each time a vertex is to be updated with its tracks in this loop,
        //      delete VxTrackAtVertex vector and add correctly updated VxTrackAtVertex (from VTAV) to the vector just before calling the vertex updator
        std::vector<Trk::VxTrackAtVertex>* tracksOfVertex = &( (*iter)->vxTrackAtVertex() );
        tracksOfVertex->clear();
	
	//prepare the iterators for the tracks
        std::vector<Trk::VxTrackAtVertex*>::iterator trkbegin = VTAV( *(*iter) ).begin();
        std::vector<Trk::VxTrackAtVertex*>::iterator trkend = VTAV( *(*iter) ).end();
	
	msg(MSG::VERBOSE) << "Beginning lin&update of vertex with pointer: " << *iter << endmsg;
        #ifdef MULTIFITTER_DEBUG
	std::cout << "Beginning lin&update of vertex with pointer: " << *iter << std::endl;
        #endif

	for (std::vector<Trk::VxTrackAtVertex*>::iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {

	  //set the weight according to all other track's weight
          #ifdef MULTIFITTER_DEBUG
	  std::cout << "Calling collect weight for track " << *trkiter << std::endl;
          #endif
	  std::vector<double>* allweights(collectWeights(*(static_cast<Trk::MVFVxTrackAtVertex*>(*trkiter))->linkToVertices()));//CHECK FOR LEAK LATER	
          #ifdef MULTIFITTER_DEBUG
	  std::cout << "The vtxcompatibility for the track is: " << (*trkiter)->vtxCompatibility() << std::endl;
          #endif
	  (*trkiter)->setWeight(m_AnnealingMaker->getWeight((*trkiter)->vtxCompatibility(),*allweights));
          #ifdef MULTIFITTER_DEBUG
	  std::cout << "The resulting weight for the track is " << m_AnnealingMaker->getWeight((*trkiter)->vtxCompatibility(),*allweights)
		    << std::endl;
	  std::cout << "Deleting allweights" << std::endl;
          #endif
	  delete allweights;
          #ifdef MULTIFITTER_DEBUG
	  std::cout << "checking weight now" << std::endl;
          #endif
	  if ((*trkiter)->weight()>m_minweight) {
            #ifdef MULTIFITTER_DEBUG
	    std::cout << "check passed" << std::endl;
            #endif
	    //now take care if linearization has been done at least once
	    if ((*trkiter)->linState()==0) {
              #ifdef MULTIFITTER_DEBUG
	      std::cout << "first linearization of the track" << std::endl;
              #endif
	      //linearization never done so far: do it now!
	      msg(MSG::VERBOSE) << "Linearizing track for the first time" << endmsg;
	      m_LinearizedTrackFactory->linearize(**trkiter,oldpositions[*iter]);
	      
	    } else if (relinearizations[*iter]==true) {
	      msg(MSG::VERBOSE) << "Relinearizing track " << endmsg;
	      m_LinearizedTrackFactory->linearize(**trkiter,oldpositions[*iter]);
              MvfFitInfo( *(*iter) )->setLinearizationVertex( new Amg::Vector3D(oldpositions[*iter]) );
	    }
	    //now you can proceed with the update
	    msg(MSG::VERBOSE) << "Update vertex with new track " << endmsg;
            #ifdef MULTIFITTER_DEBUG
	    std::cout << "Update of the track " << *trkiter << " to the vertex " << *iter << std::endl;
            #endif

            // TODO: obviously not ideal that I have to do this
            tracksOfVertex->push_back( **trkiter );

            // TODO: add() returns an xAOD::Vertex* - is it really ok to just have this line without *iter = m_VertexUpdator->add() ? Must be...
	    m_VertexUpdator->add(**iter,**trkiter);

            // TODO: Do I now need to update MVFVxTrackAtVertex with VxTrackAtVertex here? Nope, this changes output vertices in an inexplicable way
            //(*trkiter)->setVtxCompatibility( tracksOfVertex->end()->vtxCompatibility() );
            //(*trkiter)->setWeight( tracksOfVertex->end()->weight() );
            //(*trkiter)->setTrackQuality( tracksOfVertex->end()->trackQuality() );

	  } else {
	    msg(MSG::VERBOSE) << "Weight lower than " << m_minweight << ", so the track will not be considered in this turn" << endmsg;
            #ifdef MULTIFITTER_DEBUG
            std::cout << "Weight lower than " << m_minweight << ", so the track will not be considered in this turn" << std::endl; // TODO: get rid of this
            #endif
	  }    
	} //iterator on tracks

        // TODO: get rid of following line
        //std::cout << "New vertex position z: " << (*iter)->position().z() << std::endl;
      
	//show some info about the position
	msg(MSG::DEBUG) << "Vertex pointer " << *iter << 
	  " New position x: " << (*iter)->position().x() << " y: " << (*iter)->position().y()
	    << " z: " << (*iter)->position().z() << endmsg;
      } //iterator on vertexes
      
      //call now one more step of annealing
      if (!(m_AnnealingMaker->isEquilibrium()))
      {
        m_AnnealingMaker->anneal();
      }

      //August 2009: sometimes the fitter has not converged when the annealing has finished
      //iterate on all vertex candidates and check whether they moved significantly from 
      //last iteration

      shiftIsSmall=true;

      for (std::vector<xAOD::Vertex*>::iterator iter=vtxbegin;iter!=vtxend;++iter) {      
	
        // std::cout << " step: " << num_steps << " considering shift of: " << *iter << " vtx z: " <<  (*iter)->recVertex().position().z() <<std::endl;

        Amg::Vector3D vrtpos(3,0);

        vrtpos[0]=oldpositions[*iter][0]-(*iter)->position()[0];
        vrtpos[1]=oldpositions[*iter][1]-(*iter)->position()[1];
        vrtpos[2]=oldpositions[*iter][2]-(*iter)->position()[2];

	AmgSymMatrix(3) weightMatrixVertex;
	weightMatrixVertex = (*iter)->covariancePosition().inverse();
        double relativeShift=
            vrtpos.dot(weightMatrixVertex*vrtpos);

        if (relativeShift>m_maxRelativeShift)
        {
          shiftIsSmall=false;
          // std::cout << " - " << std::endl;
          break;
        }
      }
      
      num_steps+=1;

    } while (num_steps<m_maxIterations && (!(m_AnnealingMaker->isEquilibrium()) || !shiftIsSmall) );

    if (num_steps>=m_maxIterations)
    {
      msg(MSG::WARNING) << "Didn't converge fully after " << num_steps << endmsg;
    }
    
    msg(MSG::VERBOSE) << "In principle the big multivertex fit step is finished now..." << endmsg;

    // TODO: get rid of this line
    //std::cout << "In principle the big multivertex fit step is finished now..." << std::endl;

    // TODO: I don't think that I need to fiddle with updating MVFVxTracksAtVertex - vector of VxTrackAtVertex should be available in the usual way now and be enough
    // TODO: maybe try commenting this out
    if(m_doSmoothing) {
      msg(MSG::VERBOSE) << "Now SMOOTHING!" << endmsg;
      for (std::vector<xAOD::Vertex*>::iterator iter=vtxbegin;iter!=vtxend;++iter) {   
	if ((*iter)->vxTrackAtVertexAvailable() && ((*iter)->vxTrackAtVertex()).size() > 0 ) 
	  m_VertexSmoother->smooth(**iter);
      }
    } else { // TODO: I added this during xAOD migration
      for (std::vector<xAOD::Vertex*>::iterator iter=vtxbegin;iter!=vtxend;++iter) {
        std::vector<Trk::VxTrackAtVertex*>::iterator trkbegin = VTAV( *(*iter) ).begin();
        std::vector<Trk::VxTrackAtVertex*>::iterator trkend = VTAV( *(*iter) ).end();
        for (std::vector<Trk::VxTrackAtVertex*>::iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {
          if ((*trkiter)->initialPerigee() )
            (*trkiter)->setPerigeeAtVertex( ( (*trkiter)->initialPerigee() )->clone() );
        }
      }
    }

  }
      
  
  std::vector<double>*   AdaptiveMultiVertexFitter::collectWeights(const Trk::TrackToVtxLink & tracklink) {
    #ifdef MULTIFITTER_DEBUG
    std::cout << "Collecting weights for tracklink " << &tracklink << std::endl;
    #endif

    // TODO: put this in a better place
    // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
    // For optimization of access speed
    xAOD::Vertex::Decorator< std::vector<Trk::VxTrackAtVertex*> > VTAV("VTAV");

    std::vector<xAOD::Vertex*>::const_iterator begin(tracklink.vertices()->begin());
    std::vector<xAOD::Vertex*>::const_iterator end(tracklink.vertices()->end());
    
    std::vector<Trk::VxTrackAtVertex*>::const_iterator trkbegin;
    std::vector<Trk::VxTrackAtVertex*>::const_iterator trkend;
    
    std::vector<double>* myvector(new std::vector<double>);
    myvector->reserve( std::distance(begin, end) );
    for (std::vector<xAOD::Vertex*>::const_iterator iter=begin;iter!=end;++iter) {
      //really stupid, now you have still to find the weight value (right track in this vertex)
      //but if you later remove tracks from candidate, you cannot do much better (maybe update on demand...)
      trkbegin = VTAV( *(*iter) ).begin();
      trkend = VTAV( *(*iter) ).end();
      for (std::vector<Trk::VxTrackAtVertex*>::const_iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {
	if ((static_cast<Trk::MVFVxTrackAtVertex*>(*trkiter))->linkToVertices()==&tracklink) {
          #ifdef MULTIFITTER_DEBUG
	  std::cout << "found one weight: it's : " << (*trkiter)->vtxCompatibility() << " for track " << *trkiter << std::endl;
          #endif
	  myvector->push_back( (*trkiter)->vtxCompatibility() );
	  break; //gain time in avoiding to look for other tracks if you already found the right one
	}
      }
    }
    return myvector;
  }

  void AdaptiveMultiVertexFitter::addVtxTofit(xAOD::Vertex* newvertex) 
  {

 
    msg(MSG::VERBOSE) << " Now entered addVtxTofit " << endmsg;

    // TODO: put this in a better place
    // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
    // For optimization of access speed
    xAOD::Vertex::Decorator< std::vector<Trk::VxTrackAtVertex*> > VTAV("VTAV");

    if (VTAV( *newvertex ).size()==0) {
      msg(MSG::ERROR) << "The candidate you're adding has no tracks: please fix the problem" << endmsg;
    }

    std::vector<xAOD::Vertex*> allvertexestofit;

    prepareCompatibility(newvertex);

    msg(MSG::VERBOSE) << " Finished prepareCompatibility " << endmsg;

    #ifdef MULTIFITTER_DEBUG
    std::cout << "Finished to prepare compatibility" << std::endl;
    #endif

    std::vector<xAOD::Vertex*>::iterator vtxbegin;
    std::vector<xAOD::Vertex*>::iterator vtxend;

    std::vector<xAOD::Vertex*>::iterator vtxbeginIter;
    std::vector<xAOD::Vertex*>::iterator vtxendIter;

    msg(MSG::VERBOSE) << "Iterating on tracks" << endmsg;
    #ifdef MULTIFITTER_DEBUG
    std::cout << "Starting to iterate on tracks" << std::endl;
    #endif

    std::vector<xAOD::Vertex*> addedVerticesLastIteration;
    std::vector<xAOD::Vertex*> addedVerticesNewIteration;
    addedVerticesLastIteration.push_back(newvertex);

    do 
    {

      vtxbeginIter=addedVerticesLastIteration.begin();
      vtxendIter=addedVerticesLastIteration.end();


      for (std::vector<xAOD::Vertex*>::iterator vtxiterIter=vtxbeginIter;vtxiterIter!=vtxendIter;++vtxiterIter)
      {

        
        //now you have to check what are the other vertices which still have to do with your new one
        std::vector<Trk::VxTrackAtVertex*>::iterator trackbegin = VTAV( *(*vtxiterIter) ).begin();
        std::vector<Trk::VxTrackAtVertex*>::iterator trackend = VTAV( *(*vtxiterIter) ).end();

        for (std::vector<Trk::VxTrackAtVertex*>::iterator trackiter=trackbegin;trackiter!=trackend;++trackiter) {
          
          vtxbegin = (static_cast<Trk::MVFVxTrackAtVertex*>(*trackiter))->linkToVertices()->vertices()->begin();
          vtxend = (static_cast<Trk::MVFVxTrackAtVertex*>(*trackiter))->linkToVertices()->vertices()->end();

          #ifdef MULTIFITTER_DEBUG
          std::cout << "One track" << std::endl;
          #endif
          for (std::vector<xAOD::Vertex*>::iterator vtxiter=vtxbegin;vtxiter!=vtxend;++vtxiter) {
            #ifdef MULTIFITTER_DEBUG
            std::cout << "One vertex" << std::endl;
            #endif
            msg(MSG::VERBOSE) << "Iterating on vertices" << endmsg;
            //add the vertex if it's still not there
            if (findAmongVertexes(*vtxiter,allvertexestofit)==false) {
              allvertexestofit.push_back(*vtxiter);
              if (*vtxiter!=*vtxiterIter)
              {
                addedVerticesNewIteration.push_back(*vtxiter);
              }
              msg(MSG::VERBOSE) << "adding a vertex to the fit " << endmsg;
              #ifdef MULTIFITTER_DEBUG
              std::cout << "Adding one vertex to the fit" << std::endl;
              #endif
            }
          }
          
        }//iterate on tracks at considered vertex
      }//end iteration on addedVerticesLastIteration

      addedVerticesLastIteration=addedVerticesNewIteration;
      addedVerticesNewIteration.clear();
      
    } while (addedVerticesLastIteration.size()>0);
    
    
    msg(MSG::VERBOSE) << "now fitting everything together" << endmsg;
    #ifdef MULTIFITTER_DEBUG
    std::cout << "now fitting everything together " << std::endl;
    #endif
    fit(allvertexestofit);
    
  }


  void AdaptiveMultiVertexFitter::prepareCompatibility(xAOD::Vertex* newvertex) {
    
    msg(MSG::VERBOSE) << "Entered prepareCompatibility() " << endmsg;

    // TODO: put this in a better place
    // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
    // For optimization of access speed
    xAOD::Vertex::Decorator< MvfFitInfo* > MvfFitInfo("MvfFitInfo");
    xAOD::Vertex::Decorator< std::vector<Trk::VxTrackAtVertex*> > VTAV("VTAV");

    const Amg::Vector3D* seedPoint = MvfFitInfo(*newvertex)->seedVertex();

    std::vector<Trk::VxTrackAtVertex*>::iterator begin = VTAV(*newvertex).begin();
    std::vector<Trk::VxTrackAtVertex*>::iterator end = VTAV(*newvertex).end();
    
    msg(MSG::VERBOSE) << "Now adding compatibility info to the track" << endmsg;
    #ifdef MULTIFITTER_DEBUG
    std::cout <<  "Now adding compatibility info to the tracks" << std::endl;                                                 
    #endif
    for (std::vector<Trk::VxTrackAtVertex*>::iterator iter=begin;iter!=end;++iter) {
      #ifdef MULTIFITTER_DEBUG
      std::cout << "Adding compatibility info to a single track" << std::endl;
      #endif
      bool success=m_ImpactPoint3dEstimator->addIP3dAtaPlane( **iter, *seedPoint );
       if (!success)
       {
         msg(MSG::WARNING) << "Adding compatibility to vertex information failed. Newton distance finder didn't converge..." << endmsg;
       }
    }
  }


  bool AdaptiveMultiVertexFitter::findAmongVertexes(const xAOD::Vertex* vertex,const std::vector<xAOD::Vertex*> previousvertexes) {
    std::vector<xAOD::Vertex*>::const_iterator begin=previousvertexes.begin();
    std::vector<xAOD::Vertex*>::const_iterator end=previousvertexes.end();
    for (std::vector<xAOD::Vertex*>::const_iterator iter=begin;iter!=end;++iter) {
      #ifdef MULTIFITTER_DEBUG
      std::cout << "iterating on verteces in findAmongVertexes() " << std::endl;
      #endif
      if (*iter==vertex) return true;
    }
    #ifdef MULTIFITTER_DEBUG
    std::cout << "vertex " << vertex << " not found " << std::endl;
    #endif
    return false;
  }


}

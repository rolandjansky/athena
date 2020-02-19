/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    ATH_CHECK(m_LinearizedTrackFactory.retrieve());
    ATH_CHECK(m_TrackCompatibilityEstimator.retrieve());
    ATH_CHECK(m_ImpactPoint3dEstimator.retrieve());
    ATH_CHECK(m_VertexUpdator.retrieve());
    //loading smoother in case required   
    if(m_doSmoothing) ATH_CHECK(m_VertexSmoother.retrieve());
    ATH_CHECK(m_AnnealingMaker.retrieve());
    ATH_MSG_VERBOSE( "Initialize successful" );
    return StatusCode::SUCCESS;
  }
  
  StatusCode AdaptiveMultiVertexFitter::finalize()
  {
    ATH_MSG_VERBOSE( "Finalize successful" );
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
    ATH_MSG_DEBUG( " Now fitting all vertices " );
    std::vector<xAOD::Vertex*>::iterator vtxbegin=allvertexes.begin();
    std::vector<xAOD::Vertex*>::iterator vtxend=allvertexes.end();
    
    //create a vector of vertices, to store the old position...
    std::map<xAOD::Vertex*,Amg::Vector3D> oldpositions;
    std::map<xAOD::Vertex*,bool> relinearizations;
    //count number of steps
    int num_steps(0);
    //reset the annealing
    Trk::IVertexAnnealingMaker::AnnealingState astate;
    m_AnnealingMaker->reset(astate);
    ATH_MSG_DEBUG("Entering cycle of AdaptiveMultiVertexFitter" ) ;
    bool shiftIsSmall(true);
    //now start to iterate
    do {
      for (std::vector<xAOD::Vertex*>::iterator iter=vtxbegin;iter!=vtxend;++iter) {
      //now store all the "old positions"; if vertex is added for the first time this corresponds to the 
      //seed (at the same time fitted vertex will be updated with the constraint information)
      //check if you need to reestimate compatibility + linearization
      ATH_MSG_DEBUG ("Now considering candidate with ptr " << *iter );
      relinearizations[*iter]=false;
      if (isInitialized( *(*iter) )) {
        ATH_MSG_DEBUG ("Candidate already has a position: storing it in oldpositions ");
        ATH_MSG_DEBUG ( "vertex position z: " << (**iter).position()[2]);
        oldpositions[*iter] = (*iter)->position();
      } else {
        isInitialized( *(*iter) ) = true;
        ATH_MSG_DEBUG ( "Candidate has no position so far: using as old position the seedVertex" );
        if (MvfFitInfo( *(*iter) )->seedVertex()==0) { // TODO: Is there a way of checking whether a decoration exists on an object?
          ATH_MSG_ERROR( "Candidate has no seed...CRASHING now!!!" );
        }
        oldpositions[*iter] = *(MvfFitInfo( *(*iter) )->seedVertex());
      }
      if (MvfFitInfo( *(*iter) )->linearizationVertex()==0) { // TODO: Is there a way of checking whether a decoration exists on an object?
        ATH_MSG_ERROR( " Candidate has no linearization point...CRASHING now!!! ");
      }
      if ((oldpositions[*iter] - *MvfFitInfo( *(*iter) )->linearizationVertex()).perp()>m_maxDistToLinPoint) {
        ATH_MSG_DEBUG( "Candidate has to be relinearized " );
        relinearizations[*iter]=true;
        prepareCompatibility(*iter);
      }
      ATH_MSG_DEBUG( "Setting the Vertex to the initial constraint" );
      //reput everything to the constraint level
      (*iter)->setPosition( MvfFitInfo( *(*iter) )->constraintVertex()->position() );
      (*iter)->setCovariancePosition( MvfFitInfo( *(*iter) )->constraintVertex()->covariancePosition() );
      (*iter)->setFitQuality( MvfFitInfo(*(*iter))->constraintVertex()->chiSquared(), MvfFitInfo( *(*iter) )->constraintVertex()->numberDoF() );
      (*iter)->setCovariancePosition( (*iter)->covariancePosition() * 1./float(m_AnnealingMaker->getWeight(astate,1.)) );
      ATH_MSG_DEBUG( "Running TrackCompatibilityEstimator on each track" );
      //prepare the iterators for the tracks
      std::vector<Trk::VxTrackAtVertex*>::iterator trkbegin = VTAV( *(*iter) ).begin();
      std::vector<Trk::VxTrackAtVertex*>::iterator trkend = VTAV( *(*iter) ).end();
      //iterate and update the vertex with the track information
      for (std::vector<Trk::VxTrackAtVertex*>::iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {
        ATH_MSG_DEBUG( "Adding compatibility info to a track of " << VTAV( *(*iter) ).size() );
        //now recover from cases where the linearization position is !=0, but you added 
        //more tracks later on...
        if ( (*trkiter)->ImpactPoint3dAtaPlane()==0 ) 
        {
          bool success=m_ImpactPoint3dEstimator->addIP3dAtaPlane( **trkiter, *MvfFitInfo( *(*iter) )->linearizationVertex() );
          if (!success)
          {
            ATH_MSG_WARNING( "Adding compatibility to vertex information failed. Newton distance finder didn't converge..." );
          }
        }

        //first -> estimate the compatibility of the track to the vertex
        m_TrackCompatibilityEstimator->estimate( **trkiter, oldpositions[*iter] );
        ATH_MSG_DEBUG( "End of compatibility for a track" );
      }

      }
      ATH_MSG_DEBUG( "Finished first candidates cycle" );
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
        ATH_MSG_VERBOSE( "Beginning lin&update of vertex with pointer: " << *iter );
        ATH_MSG_DEBUG( "Beginning lin&update of vertex with pointer: " << *iter );
        for (std::vector<Trk::VxTrackAtVertex*>::iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {
          //set the weight according to all other track's weight
          ATH_MSG_DEBUG( "Calling collect weight for track " << *trkiter );
          std::vector<double>* allweights(collectWeights(*(static_cast<Trk::MVFVxTrackAtVertex*>(*trkiter))->linkToVertices()));//CHECK FOR LEAK LATER  
          ATH_MSG_DEBUG( "The vtxcompatibility for the track is: " << (*trkiter)->vtxCompatibility() );
          (*trkiter)->setWeight(m_AnnealingMaker->getWeight(astate,(*trkiter)->vtxCompatibility(),*allweights));
          ATH_MSG_DEBUG( "The resulting weight for the track is " << m_AnnealingMaker->getWeight(astate,(*trkiter)->vtxCompatibility(),*allweights));
          ATH_MSG_DEBUG( "Deleting allweights" );
          delete allweights;
          ATH_MSG_DEBUG( "checking weight now" );
          if ((*trkiter)->weight()>m_minweight) {
            ATH_MSG_DEBUG( "check passed" );
            //now take care if linearization has been done at least once
            if ((*trkiter)->linState()==0) {
              ATH_MSG_DEBUG( "first linearization of the track" );
              //linearization never done so far: do it now!
              ATH_MSG_VERBOSE( "Linearizing track for the first time");
              m_LinearizedTrackFactory->linearize(**trkiter,oldpositions[*iter]);
            } else if (relinearizations[*iter]==true) {
              ATH_MSG_VERBOSE( "Relinearizing track " );
              m_LinearizedTrackFactory->linearize(**trkiter,oldpositions[*iter]);
              MvfFitInfo( *(*iter) )->setLinearizationVertex( new Amg::Vector3D(oldpositions[*iter]) );
            }
            //now you can proceed with the update
            ATH_MSG_VERBOSE( "Update vertex with new track " );
            ATH_MSG_DEBUG( "Update of the track " << *trkiter << " to the vertex " << *iter );
            // TODO: obviously not ideal that I have to do this
            tracksOfVertex->push_back( **trkiter );
            // TODO: add() returns an xAOD::Vertex* - is it really ok to just have this line without *iter = m_VertexUpdator->add() ? Must be...
            m_VertexUpdator->add(**iter,**trkiter);
          } else {
            ATH_MSG_VERBOSE( "Weight lower than " << m_minweight << ", so the track will not be considered in this turn");
            ATH_MSG_DEBUG( "Weight lower than " << m_minweight << ", so the track will not be considered in this turn" ); // TODO: get rid of this
          }    
        } //iterator on tracks      
        //show some info about the position
        ATH_MSG_DEBUG( "Vertex pointer " << *iter << 
          " New position x: " << (*iter)->position().x() << " y: " << (*iter)->position().y()
            << " z: " << (*iter)->position().z() );
      } //iterator on vertexes
      //call now one more step of annealing
      if (!(m_AnnealingMaker->isEquilibrium(astate)))
      {
        m_AnnealingMaker->anneal(astate);
      }
      //August 2009: sometimes the fitter has not converged when the annealing has finished
      //iterate on all vertex candidates and check whether they moved significantly from 
      //last iteration
      shiftIsSmall=true;

      for (std::vector<xAOD::Vertex*>::iterator iter=vtxbegin;iter!=vtxend;++iter) {      
        Amg::Vector3D vrtpos;
        vrtpos[0]=oldpositions[*iter][0]-(*iter)->position()[0];
        vrtpos[1]=oldpositions[*iter][1]-(*iter)->position()[1];
        vrtpos[2]=oldpositions[*iter][2]-(*iter)->position()[2];

        AmgSymMatrix(3) weightMatrixVertex;
        weightMatrixVertex = (*iter)->covariancePosition().inverse();
        double relativeShift=vrtpos.dot(weightMatrixVertex*vrtpos);
        if (relativeShift>m_maxRelativeShift)
        {
          shiftIsSmall=false;
          break;
        }
      }
      num_steps+=1;
    } while (num_steps<m_maxIterations && (!(m_AnnealingMaker->isEquilibrium(astate)) || !shiftIsSmall) );

    if (num_steps>=m_maxIterations)
    {
       ATH_MSG_DEBUG( "Didn't converge fully after " << num_steps );
    }
    ATH_MSG_VERBOSE( "In principle the big multivertex fit step is finished now..." );
    // TODO: I don't think that I need to fiddle with updating MVFVxTracksAtVertex - vector of VxTrackAtVertex should be available in the usual way now and be enough
    // TODO: maybe try commenting this out
    if(m_doSmoothing) {
      ATH_MSG_VERBOSE( "Now SMOOTHING!" );
      for (std::vector<xAOD::Vertex*>::iterator iter=vtxbegin;iter!=vtxend;++iter) {   
        if ((*iter)->vxTrackAtVertexAvailable() && ((*iter)->vxTrackAtVertex()).size() > 0 ) {
          m_VertexSmoother->smooth(**iter);
        }
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
    ATH_MSG_DEBUG( "Collecting weights for tracklink " << &tracklink );
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
          ATH_MSG_DEBUG( "found one weight: it's : " << (*trkiter)->vtxCompatibility() << " for track " << *trkiter );
          myvector->push_back( (*trkiter)->vtxCompatibility() );
          break; //gain time in avoiding to look for other tracks if you already found the right one
        }
      }
    }
    return myvector;
  }

  void AdaptiveMultiVertexFitter::addVtxTofit(xAOD::Vertex* newvertex) 
  {
    ATH_MSG_VERBOSE( " Now entered addVtxTofit ");
    // TODO: put this in a better place
    // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
    // For optimization of access speed
    xAOD::Vertex::Decorator< std::vector<Trk::VxTrackAtVertex*> > VTAV("VTAV");

    if (VTAV( *newvertex ).size()==0) {
      ATH_MSG_ERROR( "The candidate you're adding has no tracks: please fix the problem" );
    }
    std::vector<xAOD::Vertex*> allvertexestofit;
    prepareCompatibility(newvertex);
    ATH_MSG_VERBOSE( " Finished prepareCompatibility " );
    ATH_MSG_DEBUG( "Finished to prepare compatibility" );
    std::vector<xAOD::Vertex*>::iterator vtxbegin;
    std::vector<xAOD::Vertex*>::iterator vtxend;
    //
    std::vector<xAOD::Vertex*>::iterator vtxbeginIter;
    std::vector<xAOD::Vertex*>::iterator vtxendIter;
    ATH_MSG_VERBOSE( "Iterating on tracks" );
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
          ATH_MSG_DEBUG( "One track" );
          for (std::vector<xAOD::Vertex*>::iterator vtxiter=vtxbegin;vtxiter!=vtxend;++vtxiter) {
            ATH_MSG_VERBOSE( "Iterating on vertices" );
            //add the vertex if it's still not there
            if (findAmongVertexes(*vtxiter,allvertexestofit)==false) {
              allvertexestofit.push_back(*vtxiter);
              if (*vtxiter!=*vtxiterIter)
              {
                addedVerticesNewIteration.push_back(*vtxiter);
              }
              ATH_MSG_VERBOSE( "adding a vertex to the fit " );
            }
          }
        }//iterate on tracks at considered vertex
      }//end iteration on addedVerticesLastIteration

      addedVerticesLastIteration=addedVerticesNewIteration;
      addedVerticesNewIteration.clear();
    } while (addedVerticesLastIteration.size()>0);
    ATH_MSG_VERBOSE( "now fitting everything together" );
    fit(allvertexestofit);
  }


  void AdaptiveMultiVertexFitter::prepareCompatibility(xAOD::Vertex* newvertex) {
    ATH_MSG_VERBOSE( "Entered prepareCompatibility() " );
    // TODO: put this in a better place
    // Prepare objects holding the decoration of xAOD::Vertex with MVF auxdata
    // For optimization of access speed
    xAOD::Vertex::Decorator< MvfFitInfo* > MvfFitInfo("MvfFitInfo");
    xAOD::Vertex::Decorator< std::vector<Trk::VxTrackAtVertex*> > VTAV("VTAV");
    const Amg::Vector3D* seedPoint = MvfFitInfo(*newvertex)->seedVertex();
    std::vector<Trk::VxTrackAtVertex*>::iterator begin = VTAV(*newvertex).begin();
    std::vector<Trk::VxTrackAtVertex*>::iterator end = VTAV(*newvertex).end();
    ATH_MSG_VERBOSE( "Now adding compatibility info to the track" );
    for (std::vector<Trk::VxTrackAtVertex*>::iterator iter=begin;iter!=end;++iter) {
      ATH_MSG_DEBUG( "Adding compatibility info to a single track" );
      bool success=m_ImpactPoint3dEstimator->addIP3dAtaPlane( **iter, *seedPoint );
       if (!success)
       {
         ATH_MSG_WARNING( "Adding compatibility to vertex information failed. Newton distance finder didn't converge..." );
       }
    }
  }


  bool AdaptiveMultiVertexFitter::findAmongVertexes(const xAOD::Vertex* vertex,const std::vector<xAOD::Vertex*> previousvertexes) {
    std::vector<xAOD::Vertex*>::const_iterator begin=previousvertexes.begin();
    std::vector<xAOD::Vertex*>::const_iterator end=previousvertexes.end();
    for (std::vector<xAOD::Vertex*>::const_iterator iter=begin;iter!=end;++iter) {
      ATH_MSG_DEBUG( "iterating on vertices in findAmongVertexes() " );
      if (*iter==vertex) return true;
    }
    ATH_MSG_DEBUG( "vertex " << vertex << " not found " );
    return false;
  }
}

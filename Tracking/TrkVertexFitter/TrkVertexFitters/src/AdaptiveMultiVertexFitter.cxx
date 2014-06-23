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


#include "VxMultiVertex/MVFVxCandidate.h"
#include "VxMultiVertex/MVFVxTrackAtVertex.h"
#include "VxVertex/LinearizedTrack.h"
#include "VxMultiVertex/TrackToVtxLink.h"
#include <limits>

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
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_LinearizedTrackFactory << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_LinearizedTrackFactory << endreq;
    }

    if ( m_TrackCompatibilityEstimator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_TrackCompatibilityEstimator << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_TrackCompatibilityEstimator << endreq;
    }
    
    if ( m_ImpactPoint3dEstimator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_ImpactPoint3dEstimator << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_ImpactPoint3dEstimator << endreq;
    }
   
    if ( m_VertexUpdator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexUpdator << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_VertexUpdator << endreq;
    }
   
    //loading smoother in case required   
    if(m_doSmoothing) {
      if ( m_VertexSmoother.retrieve().isFailure() ) {
        msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexSmoother << endreq;
        return StatusCode::FAILURE;
      } else {
        msg(MSG::INFO) << "Retrieved tool " << m_VertexSmoother << endreq;
      }
    }//end of smoothing options
   
    if ( m_AnnealingMaker.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_AnnealingMaker << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_AnnealingMaker << endreq;
    }
   
    msg(MSG::INFO)  << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }
  
  StatusCode AdaptiveMultiVertexFitter::finalize()
  {
    msg(MSG::INFO)  << "Finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }


  void AdaptiveMultiVertexFitter::fit(std::vector<VxCandidate*> & allvertexes) 
  {
 
    msg(MSG::DEBUG) << " Now fitting all verteces " << endreq;
 
    std::vector<VxCandidate*>::iterator vtxbegin=allvertexes.begin();
    std::vector<VxCandidate*>::iterator vtxend=allvertexes.end();
    
    //create a vector of vertexes, to store the old position...
    std::map<VxCandidate*,Vertex> oldpositions;
    std::map<VxCandidate*,bool> relinearizations;


    //count number of steps
    int num_steps(0);
    
    //reset the annealing
    m_AnnealingMaker->reset();
#ifdef MULTIFITTER_DEBUG
    std::cout << "Entering cycle of AdaptiveMultiVertexFitter" << std::endl;
#endif

    bool shiftIsSmall(true);

    //now start to iterate
    do {

      for (std::vector<VxCandidate*>::iterator iter=vtxbegin;iter!=vtxend;++iter) {
	
	//now store all the "old positions"; if vertex is added for the first time this corresponds to the 
	//seed (at the same time fitted vertex will be updated with the constraint information)
	//check if you need to reestimate compatibility + linearization
#ifdef MULTIFITTER_DEBUG
	std::cout << "Now considering candidate with ptr " << *iter << std::endl;
#endif
	relinearizations[*iter]=false;
	if ((static_cast<Trk::MVFVxCandidate*>(*iter))->isInitialized()) {
#ifdef MULTIFITTER_DEBUG
	  std::cout << "Candidate has already recVertex: storing it in oldpositions " << std::endl;
	  std::cout << "recvertex position z: " << (static_cast<Vertex>((*iter)->recVertex())).position()[2] << std::endl;
#endif
	  oldpositions[*iter]= static_cast<Vertex>((*iter)->recVertex());
	  //*((*iter)->recVertex());
	} else {
	  (static_cast<Trk::MVFVxCandidate*>(*iter))->setInitialized(true);
#ifdef MULTIFITTER_DEBUG
	  std::cout << "Candidate has no recVertex so far: using as old position the seedVertex" << std::endl;
#endif
	  if ((static_cast<Trk::MVFVxCandidate*>(*iter))->vertexFitInfo().seedVertex()==0) {
	    msg(MSG::ERROR) << "Candidate has no seed...CRASHING now!!!" << endreq;
	  }
	  oldpositions[*iter]=*((static_cast<Trk::MVFVxCandidate*>(*iter))->vertexFitInfo().seedVertex());
	  //	(*iter)->recVertex()=new RecVertex(*((*iter)->constraintVertex));
	}
	if ((static_cast<Trk::MVFVxCandidate*>(*iter))->vertexFitInfo().linearizationVertex()==0) {
	  msg(MSG::ERROR) << " Candidate has no linearization point...CRASHING now!!! "<<endreq;
	}
	//	std::cout << "MVFVxCandidate is  is: " << *iter << std::endl;
	//std::cout << "linearization is : " << (static_cast<Trk::MVFVxCandidate*>(*iter))->linearizationVertex() << std::endl;
	//std::cout << "position is: " << (static_cast<Trk::MVFVxCandidate*>(*iter))->linearizationVertex()->position().z() << std::endl;
	if ((oldpositions[*iter].position()-(static_cast<Trk::MVFVxCandidate*>(*iter))->vertexFitInfo().linearizationVertex()->position()).perp()>m_maxDistToLinPoint) {
#ifdef MULTIFITTER_DEBUG
	  std::cout << "Candidate has to be relinearized " << std::endl;
#endif
	  relinearizations[*iter]=true;
	  prepareCompatibility(*iter);
	}
	
#ifdef MULTIFITTER_DEBUG
	std::cout << "Setting the recVertex to the initial constraint" << std::endl;
#endif
	//	if ((*iter)->recVertex()!=0) delete (*iter)->recVertex();
	//reput everything to the constraint level
	(*iter)->setRecVertex(*(static_cast<Trk::MVFVxCandidate*>(*iter))->vertexFitInfo().constraintVertex());
	(*iter)->recVertex().scaleError(m_AnnealingMaker->getWeight(1.));
	
#ifdef MULTIFITTER_DEBUG
	std::cout << "Running TrackCompatibilityEstimator on each track" << std::endl;
#endif
	
	//prepare the iterators for the tracks
	std::vector<VxTrackAtVertex*>::iterator trkbegin((*iter)->vxTrackAtVertex()->begin());
	std::vector<VxTrackAtVertex*>::iterator trkend((*iter)->vxTrackAtVertex()->end());
	
	//iterate and update the vertex with the track information
	for (std::vector<VxTrackAtVertex*>::iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {
#ifdef MULTIFITTER_DEBUG
	  std::cout << "Adding compatibility info to a track of " << (*iter)->vxTrackAtVertex()->size() << std::endl;
#endif

	  if (dynamic_cast<Trk::MVFVxTrackAtVertex*>(*trkiter)==0) {
	    msg(MSG::ERROR) << "You provided wrong VxTrackAtVertex objects to the adaptive multi vertex fitter " << 
	      endreq << "They should be MVFVxTrackAtVertex objects, check the finder please" << endreq <<
	      "------>>>>>>>>>>>>>>    NO FIT done" << endreq;
	    return;
	  }


          //now recover from cases where the linearization position is !=0, but you added 
          //more tracks later on...

          if ((*trkiter)->ImpactPoint3dAtaPlane()==0) 
          {
            bool success=m_ImpactPoint3dEstimator->addIP3dAtaPlane(**trkiter,*(static_cast<Trk::MVFVxCandidate*>(*iter))->vertexFitInfo().linearizationVertex());
            if (!success)
            {
              msg(MSG::WARNING) << "Adding compatibility to vertex information failed. Newton distance finder didn't converge..." << endreq;
            }
          }

	  //first -> estimate the compatibility of the track to the vertex
	  m_TrackCompatibilityEstimator->estimate(**trkiter,oldpositions[*iter]);
#ifdef MULTIFITTER_DEBUG
	  std::cout << "End of compatibility for a track" << std::endl;
#endif
	}

      }
#ifdef MULTIFITTER_DEBUG
      std::cout << "Finished first candidates cycle" << std::endl;
#endif

      //after having estimated the compatibility of all the vertexes, you have to run again on all vertexes, 
      //to compute the weights
      
      for (std::vector<VxCandidate*>::iterator iter=vtxbegin;iter!=vtxend;++iter) {      
	
	//prepare the iterators for the tracks
	std::vector<VxTrackAtVertex*>::iterator trkbegin((*iter)->vxTrackAtVertex()->begin());
	std::vector<VxTrackAtVertex*>::iterator trkend((*iter)->vxTrackAtVertex()->end());
	
	msg(MSG::VERBOSE) << "Beginning lin&update of vertex with pointer: " << *iter << endreq;
#ifdef MULTIFITTER_DEBUG
	std::cout << "Beginning lin&update of vertex with pointer: " << *iter << std::endl;
#endif

	for (std::vector<VxTrackAtVertex*>::iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {

	  //set the weight according to all other track's weight
#ifdef MULTIFITTER_DEBUG
	  std::cout << "Colling collect weight for track " << *trkiter << std::endl;
#endif
	  std::vector<double>* allweights(collectWeights(*(static_cast<Trk::MVFVxTrackAtVertex*>(*trkiter))->linkToVertices()));//CHECK FOR LEAK LATER	
#ifdef MULTIFITTER_DEBUG
	  std::cout << "The vtxcompatibility for the track is: " << (static_cast<Trk::MVFVxTrackAtVertex*>(*trkiter))->vtxCompatibility() << std::endl;
#endif
	  (static_cast<Trk::MVFVxTrackAtVertex*>(*trkiter))->setWeight(m_AnnealingMaker->getWeight((static_cast<Trk::MVFVxTrackAtVertex*>(*trkiter))->vtxCompatibility(),*allweights));
#ifdef MULTIFITTER_DEBUG
	  std::cout << "The resulting weight for the track is " << m_AnnealingMaker->getWeight((static_cast<Trk::MVFVxTrackAtVertex*>(*trkiter))->vtxCompatibility(),*allweights)
		    << std::endl;
	  std::cout << "Deleting allweights" << std::endl;
#endif
	  delete allweights;
#ifdef MULTIFITTER_DEBUG
	  std::cout << "checking weight now" << std::endl;
#endif
	  if ((static_cast<Trk::MVFVxTrackAtVertex*>(*trkiter))->weight()>m_minweight) {
#ifdef MULTIFITTER_DEBUG
	    std::cout << "check passed" << std::endl;
#endif
	    //nowe take care if linearization has been done at least once
	    if ((*trkiter)->linState()==0) {
#ifdef MULTIFITTER_DEBUG
	      std::cout << "first linearization of the track" << std::endl;
#endif
	      //linearization never done sofar: do it now!
	      msg(MSG::VERBOSE) << "Linearizing track for the first time" << endreq;
	      m_LinearizedTrackFactory->linearize(**trkiter,oldpositions[*iter]);
	      
	    } else if (relinearizations[*iter]==true) {
	      msg(MSG::VERBOSE) << "Relinearizing track " << endreq;
	      m_LinearizedTrackFactory->linearize(**trkiter,oldpositions[*iter]);
	      (static_cast<Trk::MVFVxCandidate*>(*iter))->vertexFitInfo().setLinearizationVertex(new Vertex(oldpositions[*iter]));
	    }
	    //now you can proceed with the update
	    msg(MSG::VERBOSE) << "Update vertex with new track " << endreq;
#ifdef MULTIFITTER_DEBUG
	    std::cout << "Update of the track " << *trkiter << " to the vertex " << *iter << std::endl;
#endif
	    m_VertexUpdator->add(**iter,**trkiter);
	  } else {
	    msg(MSG::VERBOSE) << "Weight lower than " << m_minweight << ", so the track will not be considered in this turn" << endreq;
	  }    
	}	//iterator on tracks
      
	//show some info about the position
	msg(MSG::DEBUG) << "Vertex pointer " << *iter << 
	  " New position x: " << (*iter)->recVertex().position().x() << " y: " << (*iter)->recVertex().position().y()
	    << " z: " << (*iter)->recVertex().position().z() << endreq;
      }//iterator on vertexes
      
      //call now one more step of annealing
      if (!(m_AnnealingMaker->isEquilibrium()))
      {
        m_AnnealingMaker->anneal();
      }

      //August 2009: sometimes the fitter has not converged when the annealing has finished
      //iterate on all vertex candidates and check whether they moved significantly from 
      //last iteration

      shiftIsSmall=true;

      for (std::vector<VxCandidate*>::iterator iter=vtxbegin;iter!=vtxend;++iter) {      
	
//        std::cout << " step: " << num_steps << " considering shift of: " << *iter << " vtx z: " <<  (*iter)->recVertex().position().z() <<std::endl;

        Amg::Vector3D vrtpos(3,0);

        vrtpos[0]=oldpositions[*iter].position()[0]-(*iter)->recVertex().position()[0];
        vrtpos[1]=oldpositions[*iter].position()[1]-(*iter)->recVertex().position()[1];
        vrtpos[2]=oldpositions[*iter].position()[2]-(*iter)->recVertex().position()[2];

	AmgSymMatrix(3) weightMatrixVertex;
	weightMatrixVertex = (*iter)->recVertex().covariancePosition().inverse();
        double relativeShift=
            vrtpos.dot(weightMatrixVertex*vrtpos);

        if (relativeShift>m_maxRelativeShift)
        {
          shiftIsSmall=false;
//          std::cout << " - " << std::endl;
          break;
        }
      }
      
      num_steps+=1;

    } while (num_steps<m_maxIterations && (!(m_AnnealingMaker->isEquilibrium()) || !shiftIsSmall) );

    if (num_steps>=m_maxIterations)
    {
      msg(MSG::WARNING) << "Didn't converge fully after " << num_steps << endreq;
    }
    
    msg(MSG::VERBOSE) << "In principle the big multivertex fit step is finished now..." << endreq;
   
    if(m_doSmoothing) {
      msg(MSG::VERBOSE) << "Now SMOOTHING!" << endreq;
      for (std::vector<VxCandidate*>::iterator iter=vtxbegin;iter!=vtxend;++iter) {   
	m_VertexSmoother->smooth(**iter);
      }
    }


 
  }
      
  
  std::vector<double>*   AdaptiveMultiVertexFitter::collectWeights(const TrackToVtxLink & tracklink) {
#ifdef MULTIFITTER_DEBUG
    std::cout << "Collecting weights for tracklink " << &tracklink << std::endl;
#endif
    std::vector<VxCandidate*>::const_iterator begin(tracklink.vertexes()->begin());
    std::vector<VxCandidate*>::const_iterator end(tracklink.vertexes()->end());
    
    std::vector<VxTrackAtVertex*>::const_iterator trkbegin;
    std::vector<VxTrackAtVertex*>::const_iterator trkend;
    
    std::vector<double>* myvector(new std::vector<double>);
    myvector->reserve( std::distance(begin, end) );
    for (std::vector<VxCandidate*>::const_iterator iter=begin;iter!=end;++iter) {
      //really stupid, now you have still to find the weight value (right track in this vertex)
      //but if you later remove tracks from candidate, you cannot do much better (maybe update on demand...)
      trkbegin=(*iter)->vxTrackAtVertex()->begin();
      trkend=(*iter)->vxTrackAtVertex()->end();
      for (std::vector<VxTrackAtVertex*>::const_iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) {
	if ((static_cast<Trk::MVFVxTrackAtVertex*>(*trkiter))->linkToVertices()==&tracklink) {
#ifdef MULTIFITTER_DEBUG
	  std::cout << "found one weight: it's : " << (static_cast<Trk::MVFVxTrackAtVertex*>(*trkiter))->vtxCompatibility() << " for track " << *trkiter << std::endl;
#endif
	  myvector->push_back((*trkiter)->vtxCompatibility());
	  break; //gain time in avoiding to look for other tracks if you already found the right one
	}
      }
    }
    return myvector;
  }

  void AdaptiveMultiVertexFitter::addVtxTofit(VxCandidate* newvertex) 
  {

 
    msg(MSG::VERBOSE) << " Now entered addVtxTofit " << endreq;
 
    if (newvertex->vxTrackAtVertex()->size()==0) {
      msg(MSG::ERROR) << "The candidate you're adding has no tracks: please fix the problem" << endreq;
    }

    std::vector<VxCandidate*> allvertexestofit;

    prepareCompatibility(newvertex);

    msg(MSG::VERBOSE) << " Finished prepareCompatibility " << endreq;

#ifdef MULTIFITTER_DEBUG
    std::cout << "Finished to prepare compatibility" << std::endl;
#endif

    std::vector<VxCandidate*>::iterator vtxbegin;
    std::vector<VxCandidate*>::iterator vtxend;

    std::vector<VxCandidate*>::iterator vtxbeginIter;
    std::vector<VxCandidate*>::iterator vtxendIter;

    msg(MSG::VERBOSE) << "Iterating on tracks" << endreq;
#ifdef MULTIFITTER_DEBUG
    std::cout << "Starting to iterate on tracks" << std::endl;
#endif

    std::vector<VxCandidate*> addedVerticesLastIteration;
    std::vector<VxCandidate*> addedVerticesNewIteration;
    addedVerticesLastIteration.push_back(newvertex);

    do 
    {

      vtxbeginIter=addedVerticesLastIteration.begin();
      vtxendIter=addedVerticesLastIteration.end();


      for (std::vector<VxCandidate*>::iterator vtxiterIter=vtxbeginIter;vtxiterIter!=vtxendIter;++vtxiterIter)
      {

        
        //now you have to check what are the other verteces which still have to do with your new one
        std::vector<VxTrackAtVertex*>::iterator trackbegin=(*vtxiterIter)->vxTrackAtVertex()->begin();
        std::vector<VxTrackAtVertex*>::iterator trackend=(*vtxiterIter)->vxTrackAtVertex()->end();

        for (std::vector<VxTrackAtVertex*>::iterator trackiter=trackbegin;trackiter!=trackend;++trackiter) {
          
          vtxbegin=(static_cast<Trk::MVFVxTrackAtVertex*>(*trackiter))->linkToVertices()->vertexes()->begin();
          vtxend=(static_cast<Trk::MVFVxTrackAtVertex*>(*trackiter))->linkToVertices()->vertexes()->end();
#ifdef MULTIFITTER_DEBUG
          std::cout << "One track" << std::endl;
#endif
          for (std::vector<VxCandidate*>::iterator vtxiter=vtxbegin;vtxiter!=vtxend;++vtxiter) {
#ifdef MULTIFITTER_DEBUG
            std::cout << "One vertex" << std::endl;
#endif
            msg(MSG::VERBOSE) << "Iterating on vertices" << endreq;
            //add the vertex if it's still not there
            if (findAmongVertexes(*vtxiter,allvertexestofit)==false) {
              allvertexestofit.push_back(*vtxiter);
              if (*vtxiter!=*vtxiterIter)
              {
                addedVerticesNewIteration.push_back(*vtxiter);
              }
              msg(MSG::VERBOSE) << "adding a vertex to the fit " << endreq;
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
    
    
    msg(MSG::VERBOSE) << "now fitting everything together" << endreq;
#ifdef MULTIFITTER_DEBUG
    std::cout << "now fitting everything together " << std::endl;
#endif
    fit(allvertexestofit);
    
  }


  void AdaptiveMultiVertexFitter::prepareCompatibility(VxCandidate* newvertex) {
    
    msg(MSG::VERBOSE) << "Entered prepareCompatibility() " << endreq;
    
    const Vertex* seedPoint((static_cast<Trk::MVFVxCandidate*>(newvertex))->vertexFitInfo().seedVertex());
    
    std::vector<VxTrackAtVertex*>::iterator begin(newvertex->vxTrackAtVertex()->begin());
    std::vector<VxTrackAtVertex*>::iterator end(newvertex->vxTrackAtVertex()->end());
    
    msg(MSG::VERBOSE) << "Now adding compatibility info to the track" << endreq;
#ifdef MULTIFITTER_DEBUG
    std::cout <<  "Now adding compatibility info to the tracks" << std::endl;                                                 
#endif
    for (std::vector<VxTrackAtVertex*>::iterator iter=begin;iter!=end;++iter) {
#ifdef MULTIFITTER_DEBUG
      std::cout << "Adding compatibility info to a single track" << std::endl;
#endif
      bool success=m_ImpactPoint3dEstimator->addIP3dAtaPlane(**iter,*seedPoint);
       if (!success)
       {
         msg(MSG::WARNING) << "Adding compatibility to vertex information failed. Newton distance finder didn't converge..." << endreq;
       }
    }
  }


  bool AdaptiveMultiVertexFitter::findAmongVertexes(const VxCandidate* vertex,const std::vector<VxCandidate*> previousvertexes) {
    std::vector<VxCandidate*>::const_iterator begin=previousvertexes.begin();
    std::vector<VxCandidate*>::const_iterator end=previousvertexes.end();
    for (std::vector<VxCandidate*>::const_iterator iter=begin;iter!=end;++iter) {
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

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/***************************************************************************
              AdaptiveVertexFitter.cxx  -  Description in header file
 ***************************************************************************/

#include "TrkVertexFitters/AdaptiveVertexFitter.h"

#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "TrkVertexFitterInterfaces/IVertexTrackCompatibilityEstimator.h"
#include "TrkVertexFitterInterfaces/IVertexUpdator.h"
#include "TrkVertexFitterInterfaces/IVertexSmoother.h"
#include "TrkVertexFitterInterfaces/IVertexAnnealingMaker.h"
#include "TrkVertexFitterInterfaces/IImpactPoint3dEstimator.h"

#include "VxVertex/LinearizedTrack.h"
#include "TrkParametersBase/ParametersBase.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "TrkLinks/LinkToXAODNeutralParticle.h"

//xAOD includes
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/NeutralParticle.h"

namespace Trk
{
  
  AdaptiveVertexFitter::AdaptiveVertexFitter(const std::string& t, const std::string& n, const IInterface*  p) : 
    base_class(t,n,p),
    m_SeedFinder("Trk::CrossDistancesSeedFinder"),
    m_LinearizedTrackFactory("Trk::FullLinearizedTrackFactory"),
    m_TrackCompatibilityEstimator("Trk::Chi2TrackCompatibilityEstimator"),
    m_ImpactPoint3dEstimator("Trk::ImpactPoint3dEstimator/ImpactPoint3dEstimator"),
    m_VertexUpdator("Trk::KalmanVertexUpdator"),
    m_VertexSmoother("Trk::DummyVertexSmoother"),
    m_AnnealingMaker("Trk::DetAnnealingMaker"),
    m_maxIterations(50),
    m_maxDistToLinPoint(0.5),
    m_initialError(0.0001),
    m_onlyzseed(false),
    m_doSmoothing(true)
  {
    declareProperty("MaxIterations",        m_maxIterations);
    declareProperty("MaxDistToLinPoint",    m_maxDistToLinPoint);
    declareProperty("InitialError",m_initialError);
    declareProperty("SeedFinder",m_SeedFinder);
    declareProperty("LinearizedTrackFactory",m_LinearizedTrackFactory);
    declareProperty("TrackCompatibilityEstimator",m_TrackCompatibilityEstimator);
    declareProperty("ImpactPoint3dEstimator",m_ImpactPoint3dEstimator);
    declareProperty("VertexUpdator",m_VertexUpdator);
    declareProperty("VertexSmoother",m_VertexSmoother);
    declareProperty("AnnealingMaker",m_AnnealingMaker);
    declareProperty("onlyzseed",m_onlyzseed);
    declareProperty("DoSmoothing",m_doSmoothing);
    declareInterface<IVertexFitter>(this);
  }

  AdaptiveVertexFitter::~AdaptiveVertexFitter() = default;

  StatusCode AdaptiveVertexFitter::initialize()
  {
    StatusCode s = AlgTool::initialize();
    if (s.isFailure()) 
    {
      msg(MSG::FATAL) << "AlgTool::initialize() failed" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_SeedFinder.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_SeedFinder << endmsg;
      return StatusCode::FAILURE;
    } 
      msg(MSG::INFO) << "Retrieved tool " << m_SeedFinder << endmsg;
    

    if ( m_LinearizedTrackFactory.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_LinearizedTrackFactory << endmsg;
      return StatusCode::FAILURE;
    } 
      msg(MSG::INFO) << "Retrieved tool " << m_LinearizedTrackFactory << endmsg;
    

    if ( m_TrackCompatibilityEstimator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_TrackCompatibilityEstimator << endmsg;
      return StatusCode::FAILURE;
    } 
      msg(MSG::INFO) << "Retrieved tool " << m_TrackCompatibilityEstimator << endmsg;
    
    
    if ( m_ImpactPoint3dEstimator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_ImpactPoint3dEstimator << endmsg;
      return StatusCode::FAILURE;
    } 
      msg(MSG::INFO) << "Retrieved tool " << m_ImpactPoint3dEstimator << endmsg;
    
    
    if ( m_VertexUpdator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexUpdator << endmsg;
      return StatusCode::FAILURE;
    } 
      msg(MSG::INFO) << "Retrieved tool " << m_VertexUpdator << endmsg;
    
    
   //loading smoother in case required   
   if(m_doSmoothing)
   {
    if ( m_VertexSmoother.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexSmoother << endmsg;
      return StatusCode::FAILURE;
    } 
      msg(MSG::INFO) << "Retrieved tool " << m_VertexSmoother << endmsg;
    
   }//end of smoothing options
   
  if ( m_AnnealingMaker.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_AnnealingMaker << endmsg;
    return StatusCode::FAILURE;
  } 
    msg(MSG::INFO) << "Retrieved tool " << m_AnnealingMaker << endmsg;
  
  
    msg(MSG::INFO)  << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  StatusCode AdaptiveVertexFitter::finalize()
  {
    msg(MSG::INFO)  << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;
  }

  xAOD::Vertex*
  AdaptiveVertexFitter::_fit(
    const std::vector<const Trk::TrackParameters*>& perigeeList,
    const std::vector<const Trk::NeutralParameters*>& neutralPerigeeList,
    const xAOD::Vertex& constraint,     // initialized to xAOD::Vertex()
    const Amg::Vector3D& startingPoint, // initialized to Amg::Vector3D()
    bool IsConstraint,                  // initialized to false
    bool IsStartingPoint) const         // initialized to false
  {

    //check the number of tracks
    if ( (perigeeList.size() + neutralPerigeeList.size())<2 && !IsConstraint ) {
      msg(MSG::WARNING) << "Not enough tracks (>2) to fit in this event (without constraint)." << endmsg;
      return nullptr;
    }
    if ((perigeeList.size() + neutralPerigeeList.size())<1 && IsConstraint) {
      msg(MSG::WARNING) << "Not enough tracks (>1) to fit in this event (with constraint)." << endmsg;
      return nullptr;
    }


    Amg::Vector3D SeedPoint;
    //now find the best point for seeding and linearization of the tracks
    if (IsStartingPoint) {
      SeedPoint=startingPoint;
    } else {
      if (perigeeList.size()>1) {
        if (IsConstraint) {
          SeedPoint=m_SeedFinder->findSeed(perigeeList,&constraint);
        } else {
          SeedPoint=m_SeedFinder->findSeed(perigeeList);
        }
      }
      else if (IsConstraint) {
        SeedPoint=constraint.position();
      }
      else {
        SeedPoint.setZero();
      }
    }

    //in case m_onlyzseed is on, just use only the z component given by the seed finder
    if (m_onlyzseed&&IsConstraint) {
      SeedPoint=constraint.position();
    }
    

    //now create a Global Position with the linearized track and the linearization point 
    //which in this case is taken from the found seed for the vertex
    //use the linPoint to linearize tracks
    
    std::vector<VxTrackAtVertex> theLinTracks;

    std::vector<const Trk::TrackParameters*>::const_iterator perigeesBegin=perigeeList.begin();
    std::vector<const Trk::TrackParameters*>::const_iterator perigeesEnd=perigeeList.end();

    ATH_MSG_DEBUG("Inside fitter with track perigee parameters.");
    ATH_MSG_DEBUG("Seed point: " << SeedPoint);
    int myDebugNTrk(0);

    for (std::vector<const Trk::TrackParameters*>::const_iterator perigeesIter=perigeesBegin;perigeesIter!=perigeesEnd;++perigeesIter) {
      //      const MeasuredPerigee* castToMP=dynamic_cast<const MeasuredPerigee*>(*perigeesIter);
      //      if (castToMP==0) {
      //        msg << MSG::ERROR << "Cast to MeasuredPerigee not successfull. Treatment of neutrals not supported. Skipping track..." << endmsg;
      //        continue;
      //      }

      ATH_MSG_DEBUG("Track #" << myDebugNTrk++ << ". TrackParameters: x = "  << (*perigeesIter)->position().x() << ", y = "<< (*perigeesIter)->position().y() <<", z = "<< (*perigeesIter)->position().z() << ". Covariance: " << *(*perigeesIter)->covariance());
     
      VxTrackAtVertex* LinTrackToAdd = new VxTrackAtVertex(0., nullptr, nullptr, (*perigeesIter), nullptr); //TODO: Must think now about when to delete this memory! -David S.
      
      //m_LinearizedTrackFactory->linearize(*LinTrackToAdd,SeedPoint); why linearize it? maybe you don't need it at all!!!!! <19-05-2006>
      bool success=m_ImpactPoint3dEstimator->addIP3dAtaPlane(*LinTrackToAdd,SeedPoint);
      if (!success)
      {
	msg(MSG::WARNING) << "Adding compatibility to vertex information failed. Newton distance finder didn't converge..." << endmsg;
      }

      theLinTracks.push_back(*LinTrackToAdd);
     
      delete LinTrackToAdd; //TODO: is here ok?

    }

    std::vector<const Trk::NeutralParameters*>::const_iterator neutralPerigeesBegin=neutralPerigeeList.begin();
    std::vector<const Trk::NeutralParameters*>::const_iterator neutralPerigeesEnd  =neutralPerigeeList.end();

    ATH_MSG_DEBUG("Inside fitter with neutral perigee parameters.");
    ATH_MSG_DEBUG("Seed point: " << SeedPoint);
    int myDebugNNeutral(0);
    for (std::vector<const Trk::NeutralParameters*>::const_iterator neutralPerigeesIter=neutralPerigeesBegin;neutralPerigeesIter!=neutralPerigeesEnd;++neutralPerigeesIter) {

      ATH_MSG_DEBUG("Neutral #" << myDebugNNeutral++ << ". NeutralParameters: x = "  << (*neutralPerigeesIter)->position().x() << ", y = "<< (*neutralPerigeesIter)->position().y() <<", z = "<< (*neutralPerigeesIter)->position().z());
      ATH_MSG_DEBUG("     Covariance: " << *(*neutralPerigeesIter)->covariance());
      ATH_MSG_DEBUG("     Momentum: x = "  << (*neutralPerigeesIter)->momentum().x() << ", y = "<< (*neutralPerigeesIter)->momentum().y() <<", z = "<< (*neutralPerigeesIter)->momentum().z());

      VxTrackAtVertex* LinTrackToAdd = new VxTrackAtVertex(0., nullptr, nullptr, nullptr, (*neutralPerigeesIter) ); //TODO: Must think now about when to delete this memory! -David S.
     
      bool success = m_ImpactPoint3dEstimator->addIP3dAtaPlane(*LinTrackToAdd,SeedPoint);
      if (!success)
      {
	msg(MSG::WARNING) << "Adding compatibility to vertex information failed. Newton distance finder didn't converge..." << endmsg;
      }
      
      theLinTracks.push_back(*LinTrackToAdd);

      delete LinTrackToAdd; //TODO: is here ok?
    }



    //Initialize the vertex seed with an arbitrary great error matrix (just starting point,
    //not really prior information in it)
    if(msgLvl(MSG::VERBOSE))
    {
      msg(MSG::VERBOSE) << "Error at the beginning is set to " << m_initialError << endmsg;
    }



   
    xAOD::Vertex ConstraintVertex;
    ConstraintVertex.makePrivateStore();
    //use the previous prior vertex info for the initial vertex if there
    if (IsConstraint ) {
      ConstraintVertex.setPosition( constraint.position() );
      ConstraintVertex.setCovariancePosition( constraint.covariancePosition() );
      ConstraintVertex.setFitQuality( 0., 0. );
    } else {
      AmgSymMatrix(3) startingCovMatrix;
      startingCovMatrix.setIdentity();
      startingCovMatrix = startingCovMatrix / m_initialError;
      //now initialize with starting position and covariance matrix
      ConstraintVertex.setPosition( SeedPoint );
      ConstraintVertex.setCovariancePosition( startingCovMatrix );
      ConstraintVertex.setFitQuality( 0., -3. );
    }

    //now put the linearizedtracks into VxTrackAtVertex
    return dothefit(ConstraintVertex,SeedPoint,theLinTracks);


  }

  xAOD::Vertex*
  AdaptiveVertexFitter::_fit(
    const std::vector<const Trk::Track*>& VectorTrk,
    const xAOD::Vertex& constraint,     // initialized to xAOD::Vertex()
    const Amg::Vector3D& startingPoint, // initialized to Amg::Vector3D()
    bool IsConstraint,                  // initialized to false
    bool IsStartingPoint) const
  { // initialized to false

    ATH_MSG_DEBUG("Called Adaptive vertex with Trk::Track. N. Tracks = " << VectorTrk.size());

    std::vector<const Trk::TrackParameters*> perigeeList;
    for (std::vector<const Trk::Track*>::const_iterator iter=VectorTrk.begin();
         iter!=VectorTrk.end();++iter) {
      if (std::isnan((*iter)->perigeeParameters()->parameters()[Trk::d0])) {
        continue;
      }
      perigeeList.push_back((*iter)->perigeeParameters());
    }

    xAOD::Vertex * FittedVertex = _fit(perigeeList,constraint,startingPoint,IsConstraint,IsStartingPoint);

    if (FittedVertex==nullptr) {
      return FittedVertex;
    }

    //these following lines are really absurd... Why has this to be the case... <19-06-2006>
    //We need a link which can be dynamically to a Track or a TrackParticle....... )-:

    const std::vector<const Trk::Track*>::const_iterator trkbegin=VectorTrk.begin();
    const std::vector<const Trk::Track*>::const_iterator trkend=VectorTrk.end();

    const std::vector<VxTrackAtVertex>::iterator vtxbegin=FittedVertex->vxTrackAtVertex().begin();
    const std::vector<VxTrackAtVertex>::iterator vtxend=FittedVertex->vxTrackAtVertex().end();

    for (std::vector<const Trk::Track*>::const_iterator trkiter=trkbegin;trkiter!=trkend;++trkiter)
    {
      for (std::vector<Trk::VxTrackAtVertex>::iterator vtxiter=vtxbegin;vtxiter!=vtxend;++vtxiter)
      {
        if(((*trkiter)->perigeeParameters()->momentum() -
            (*vtxiter).initialPerigee()->momentum()).mag()< 1e-8  &&
            ((*trkiter)->perigeeParameters()->position() -
            (*vtxiter).initialPerigee()->position()).mag()< 1e-8)
        {

          ElementLink<TrackCollection> link;
          link.setElement(*trkiter);
          LinkToTrack * linkTT = new LinkToTrack(link);
          (*vtxiter).setOrigTrack(linkTT);
        }
      }
    }

    //*******************************************************************
    // TODO: Starting from a vector of Trk::Tracks, can't store
    // separately from the vxTrackAtVertex vector the links to the
    // original tracks in xAOD::Vertex (only links to xAOD::TrackParticles and
    // xAOD::NeutralParticles can be stored)
    //*******************************************************************

    return FittedVertex;
  }

  xAOD::Vertex*
  AdaptiveVertexFitter::dothefit(
    const xAOD::Vertex& ConstraintVertex,
    const Amg::Vector3D& SeedVertex,
    std::vector<Trk::VxTrackAtVertex>& myLinTracks) const
  {
  
    //now reset the annealing maker
    Trk::IVertexAnnealingMaker::AnnealingState astate;
    m_AnnealingMaker->reset(astate);

    //Count the steps for the fit and the number of relinearizations needed in the fit
    int num_steps(0);
    int num_relinearizations(0);


    xAOD::Vertex* ActualVertex = new xAOD::Vertex();
    ActualVertex->makePrivateStore(); // xAOD::VertexContainer will take ownership of AuxStore when ActualVertex is added to it
    ActualVertex->setPosition( ConstraintVertex.position() );
    ActualVertex->setCovariancePosition( ConstraintVertex.covariancePosition() / m_AnnealingMaker->getWeight(astate, 1.) );
    ActualVertex->setFitQuality( ConstraintVertex.chiSquared(), ConstraintVertex.numberDoF() );
    ActualVertex->vxTrackAtVertex() = myLinTracks;
    ActualVertex->setVertexType(xAOD::VxType::NotSpecified); // to mimic the initialization present in the old EDM constructor

    Amg::Vector3D NewVertex = SeedVertex;

    Amg::Vector3D ActualSeedPosition = SeedVertex;

    if(msgLvl(MSG::VERBOSE))
    {
      msg(MSG::VERBOSE) << "Num max of steps is " << m_maxIterations << endmsg;
      msg(MSG::VERBOSE) << "m_AnnealingMaker->isEquilibrium() is " << m_AnnealingMaker->isEquilibrium(astate) << endmsg;
    }

    std::vector<Trk::VxTrackAtVertex>::iterator lintracksBegin = ActualVertex->vxTrackAtVertex().begin();
    std::vector<Trk::VxTrackAtVertex>::iterator lintracksEnd   = ActualVertex->vxTrackAtVertex().end();

    std::vector<Trk::VxTrackAtVertex>::iterator iter;

    bool relinearization = false;

    do {

      ActualVertex->setPosition( ConstraintVertex.position() );
      ActualVertex->setCovariancePosition( ConstraintVertex.covariancePosition() / m_AnnealingMaker->getWeight(astate, 1.) );
      ActualVertex->setFitQuality( ConstraintVertex.chiSquared(), ConstraintVertex.numberDoF() );

      if(msgLvl(MSG::DEBUG))
      {
        msg(MSG::DEBUG) << "Correction applied to constraint weight is: " << m_AnnealingMaker->getWeight(astate, 1.) << endmsg;
      }

      //To reweight here through an extrapolation is not ideal, but maybe I'll change this in the future...
      if(msgLvl(MSG::DEBUG))
      {
        msg(MSG::DEBUG) << "New fit step: step number " << num_steps << endmsg;
      }

      //consider relinearization if you are too far away from the old lin point


      relinearization=false;
      if ( (ActualSeedPosition - SeedVertex).perp() > m_maxDistToLinPoint ) {
        relinearization=true;
        if(msgLvl(MSG::DEBUG))
        {
          msg(MSG::DEBUG) << "Actual vertex too far away from linearization point: have to linearize tracks again" << endmsg;
        }

        //collect all measured perigees
        
        for (iter=lintracksBegin;iter!=lintracksEnd;++iter) {
          //m_LinearizedTrackFactory->linearize(*iter,ActualPosition);
          bool success=m_ImpactPoint3dEstimator->addIP3dAtaPlane(*iter,NewVertex);
          if (!success)
          {
            msg(MSG::WARNING) << "Adding compatibility to vertex information failed. Newton distance finder didn't converge..." << endmsg;
          }
        }
        
        num_relinearizations+=1;
      }


      lintracksBegin=ActualVertex->vxTrackAtVertex().begin();
      lintracksEnd=ActualVertex->vxTrackAtVertex().end();

      //now reweight tracks (find chi2 compatibility with actual vertex position)
      for (iter=lintracksBegin;iter!=lintracksEnd;++iter) {

        //estimate the compatibility of the track to the vertex and store it in iter->linState()->m_vtxCompatibility
        m_TrackCompatibilityEstimator->estimate(*iter,NewVertex);

        //use the obtained estimate and ask the Annealing Maker what is the corresponding weight at the actual temperature step
        iter->setWeight( m_AnnealingMaker->getWeight( astate, iter->vtxCompatibility() ) );
        if(msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << "Before annealing: " << iter->vtxCompatibility() <<
              " Annealing RESULT is:" << iter->weight() << " at T: " <<
              m_AnnealingMaker->actualTemp(astate) << endmsg;
        }


      }

      //now update with all the tracks info
      for (iter=lintracksBegin;iter!=lintracksEnd;++iter) {
        if(msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << "Updating vertex with a new track" << endmsg;
        }
        try {

          if ( iter->weight() > 1e-3 ) {
            //now take care if linearization has been done at least once 
            //or if vertex has changed so much so that you need relinearization!
            if ( iter->linState() == nullptr) {
              //linearization has not been done so far: do it now!
              if(msgLvl(MSG::VERBOSE))
              {
                msg(MSG::VERBOSE) << "Linearizing track for the first time" << endmsg;
              }

              m_LinearizedTrackFactory->linearize( *iter, ActualVertex->position() );
              ATH_MSG_DEBUG( "Linearized track to Seed Point. " << *iter );
            } else if (relinearization) {
              //do it again in case of updated vertex too far from previous one 
              if(msgLvl(MSG::VERBOSE))
              {
                msg(MSG::VERBOSE) << "Relinearizing track " << endmsg;
              }

              m_LinearizedTrackFactory->linearize( *iter, NewVertex );
              ActualSeedPosition=NewVertex;
              ATH_MSG_DEBUG("Linearized track to new seed Point " << NewVertex << ". " << *iter );
            }
            ActualVertex=m_VertexUpdator->add(*ActualVertex,*iter);
          } else {
            if(msgLvl(MSG::VERBOSE))
            {
              msg(MSG::VERBOSE) << "Weight lower than 1e-3, so the track will not be considered anymore" << endmsg;
            }

          }
        } catch (...) {
          msg(MSG::WARNING) << "You have just lost a track" << endmsg;
        }
      }

      //show some info about the position
      if(msgLvl(MSG::DEBUG))
      {
        msg(MSG::DEBUG) << "New position x: " << ActualVertex->position().x() << " y: " << ActualVertex->position().y()
                        << " z: " << ActualVertex->position().z() << endmsg;
      }


      NewVertex = ActualVertex->position();

      //now go one step down in the annealing process (lower the temperature - single step)
      if(msgLvl(MSG::VERBOSE))
      {
        msg(MSG::VERBOSE) << "Now calling one step of annealing" << endmsg;
      }

      m_AnnealingMaker->anneal(astate);
      num_steps+=1;

      //continue to fit until max iteration number has been reached or "thermal equilibrium"
      //has been obtained in the annealing process

    } while (num_steps<m_maxIterations && !(m_AnnealingMaker->isEquilibrium(astate)) );

    //Here smooth the vertex (refitting of the track)
    
    //without smoothing... for the moment...
    if(m_doSmoothing) {
      m_VertexSmoother->smooth(*ActualVertex);
    } else {
      for (iter=lintracksBegin;iter!=lintracksEnd;++iter) {
        //      const MeasuredPerigee* castToMP=dynamic_cast<const MeasuredPerigee*>(iter->initialPerigee());
        //      if (castToMP==0) {
        //        msg(MSG::WARNING) << "Couldn't cast a track to MeasuredPerigee to smooth it. Neutrals not supported. " <<
        //          " Skipping track. .. " << endmsg;
        //        continue;
        //      }
        if ( iter->initialPerigee() )
          iter->setPerigeeAtVertex( ( iter->initialPerigee() )->clone() );
        else if ( iter->initialNeutralPerigee() )
          iter->setPerigeeAtVertex( ( iter->initialNeutralPerigee() )->clone() );
      }
    }

    if(msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG) << "chi2: " << ActualVertex->chiSquared()
                      << "the ndf of the vertex is at fit end: " << ActualVertex->numberDoF() << endmsg;
    }

    // TODO: get rid of following line
    //std::cout << "Number of steps: " << num_steps << ". Number of relinearizations: " << num_relinearizations << "." << std::endl << std::endl;

    //Give back all info into the ActualVertex xAOD::Vertex
    return ActualVertex;
  }

  xAOD::Vertex*
  AdaptiveVertexFitter::fit(
    const std::vector<const Trk::TrackParameters*>& perigeeList,
    const std::vector<const Trk::NeutralParameters*>& neutralPerigeeList,
    const Amg::Vector3D& startingPoint) const
  {

    return _fit(perigeeList,neutralPerigeeList,xAOD::Vertex(),startingPoint,false,true);
  }

  xAOD::Vertex*
  AdaptiveVertexFitter::fit(
    const std::vector<const Trk::TrackParameters*>& perigeeList,
    const std::vector<const Trk::NeutralParameters*>& neutralPerigeeList,
    const xAOD::Vertex& constraint) const
  {
    return _fit(perigeeList,neutralPerigeeList,constraint,Amg::Vector3D(),true);
  }

  xAOD::Vertex*
  AdaptiveVertexFitter::fit(
    const std::vector<const Trk::TrackParameters*>& perigeeList,
    const std::vector<const Trk::NeutralParameters*>& neutralPerigeeList,
    const xAOD::Vertex& constraint,
    const Amg::Vector3D& startingPoint) const
  {
    return _fit(perigeeList,neutralPerigeeList,constraint,startingPoint,true,true);
  }

  xAOD::Vertex*
  AdaptiveVertexFitter::fit(
    const std::vector<const Trk::TrackParameters*>& perigeeList,
    const std::vector<const Trk::NeutralParameters*>& neutralPerigeeList) const
  {
    return _fit(perigeeList,neutralPerigeeList,xAOD::Vertex(),Amg::Vector3D(),false,false);
  }

  xAOD::Vertex*
  AdaptiveVertexFitter::fit(
    const std::vector<const Trk::TrackParameters*>& perigeeList,
    const Amg::Vector3D& startingPoint) const
  {
    const std::vector<const Trk::NeutralParameters*> neutralPerigeeList;
    return fit(perigeeList, neutralPerigeeList, startingPoint);
  }

  xAOD::Vertex*
  AdaptiveVertexFitter::fit(
    const std::vector<const Trk::TrackParameters*>& perigeeList,
    const xAOD::Vertex& constraint) const
  {
    const std::vector<const Trk::NeutralParameters*> neutralPerigeeList;
    return fit(perigeeList, neutralPerigeeList, constraint);
  }

  xAOD::Vertex*
  AdaptiveVertexFitter::fit(
    const std::vector<const Trk::TrackParameters*>& perigeeList,
    const xAOD::Vertex& constraint,
    const Amg::Vector3D& startingPoint) const
  {
    const std::vector<const Trk::NeutralParameters*> neutralPerigeeList;
    return fit(perigeeList, neutralPerigeeList, constraint, startingPoint);
  }

  xAOD::Vertex * AdaptiveVertexFitter::fit(const std::vector<const Trk::TrackParameters*> & perigeeList) const
  {
    const std::vector<const Trk::NeutralParameters*> neutralPerigeeList; 
    return fit(perigeeList,neutralPerigeeList);
  }

  xAOD::Vertex * AdaptiveVertexFitter::_fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
		                            const xAOD::Vertex& constraint,
					    const Amg::Vector3D & startingPoint,
					    bool IsConstraint,
					    bool IsStartingPoint) const {
    const std::vector<const Trk::NeutralParameters*> neutralPerigeeList;
    return _fit(perigeeList, neutralPerigeeList, constraint, startingPoint, IsConstraint, IsStartingPoint);
  }

//xAOD interfaced methods. Required to un-block the current situation 
// with the xAOD tracking design.

  xAOD::Vertex*
  AdaptiveVertexFitter::fit(
    const std::vector<const xAOD::TrackParticle*>& vectorTrk,
    const std::vector<const xAOD::NeutralParticle*>& vectorNeut,
    const Amg::Vector3D& startingPoint) const
  {

    if (vectorTrk.empty()) {
      msg(MSG::INFO) << "Empty vector of tracks passed" << endmsg;
      return nullptr;
    }

    if (vectorNeut.empty()) {
      msg(MSG::VERBOSE) << "Empty vector of neutrals passed" << endmsg;
    }

    // making a list of perigee out of the vector of tracks
    std::vector<const Trk::TrackParameters*> measuredPerigees;

    for (std::vector<const xAOD::TrackParticle*>::const_iterator i =
           vectorTrk.begin();
         i != vectorTrk.end();
         ++i) {
      const Trk::TrackParameters* tmpMeasPer = &((*i)->perigeeParameters());

      if (tmpMeasPer != nullptr)
        measuredPerigees.push_back(tmpMeasPer);
      else
        msg(MSG::INFO)
          << "Failed to dynamic_cast this track parameters to perigee"
          << endmsg; // TODO: Failed to implicit cast the perigee parameters to
                     // track parameters?
    }

    // making a list of perigee out of the vector of neutrals
    std::vector<const Trk::NeutralParameters*> measuredNeutralPerigees;

    for (std::vector<const xAOD::NeutralParticle*>::const_iterator i =
           vectorNeut.begin();
         i != vectorNeut.end();
         ++i) {
      const Trk::NeutralParameters* tmpMeasPer = &((*i)->perigeeParameters());

      if (tmpMeasPer != nullptr)
        measuredNeutralPerigees.push_back(tmpMeasPer);
      else
        msg(MSG::INFO)
          << "Failed to dynamic_cast this neutral parameters to perigee"
          << endmsg; // TODO: Failed to implicit cast the perigee parameters to
                     // neutral parameters?
    }

    xAOD::Vertex* fittedVertex = _fit(measuredPerigees,
                                      measuredNeutralPerigees,
                                      xAOD::Vertex(),
                                      startingPoint,
                                      false,
                                      true);

    // assigning the input tracks to the fitted vertex through VxTrackAtVertices
    if (fittedVertex == nullptr) {
      return fittedVertex;
    }

    if (fittedVertex
          ->vxTrackAtVertexAvailable()) // TODO: I don't think
                                        // vxTrackAtVertexAvailable() does the
                                        // same thing as a null pointer check!
    {
      if (!fittedVertex->vxTrackAtVertex().empty()) {
        for (unsigned int i = 0; i < vectorTrk.size(); ++i) {

          LinkToXAODTrackParticle* linkTT = new LinkToXAODTrackParticle();
          const xAOD::TrackParticleContainer* cont =
            dynamic_cast<const xAOD::TrackParticleContainer*>(
              vectorTrk[i]->container());
          if (cont) {
            if (!linkTT->toIndexedElement(*cont, vectorTrk[i]->index())) {
              msg(MSG::WARNING)
                << "Failed to set the EL for this particle correctly" << endmsg;
            }
          } else {
            msg(MSG::WARNING)
              << "Failed to identify a  container for this TP" << endmsg;
          } // end of the dynamic cast check

          // vxtrackatvertex takes ownership!
          (fittedVertex->vxTrackAtVertex())[i].setOrigTrack(linkTT);
        } // end of loop for setting orig tracks in.

        for (unsigned int i = 0; i < vectorNeut.size(); ++i) {
          LinkToXAODNeutralParticle* linkTT = new LinkToXAODNeutralParticle();
          const xAOD::NeutralParticleContainer* cont =
            dynamic_cast<const xAOD::NeutralParticleContainer*>(
              vectorNeut[i]->container());
          if (cont) {
            if (!linkTT->toIndexedElement(*cont, vectorNeut[i]->index())) {
              msg(MSG::WARNING)
                << "Failed to set the EL for this particle correctly" << endmsg;
            }
          } else {
            msg(MSG::WARNING)
              << "Failed to identify a  container for this TP" << endmsg;
          } // end of the dynamic cast check

          // vxtrackatvertex takes ownership!
          (fittedVertex->vxTrackAtVertex())[i + vectorTrk.size()].setOrigTrack(
            linkTT);
        } // end of loop for setting orig neutrals in.

      } // end of protection against unsuccessfull updates (no tracks were
        // added)
    } // end of vector of tracks check

    // now set links to xAOD::TrackParticles directly in the xAOD::Vertex
    unsigned int VTAVsize = fittedVertex->vxTrackAtVertex().size();
    for (unsigned int i = 0; i < VTAVsize; ++i) {
      Trk::VxTrackAtVertex* VTAV = &(fittedVertex->vxTrackAtVertex().at(i));
      // TODO: Will this pointer really hold 0 if no VxTrackAtVertex is found?
      if (not VTAV) {
        ATH_MSG_WARNING(" Trying to set link to xAOD::TrackParticle. The "
                        "VxTrackAtVertex is not found");
        continue;
      }

      Trk::ITrackLink* trklink = VTAV->trackOrParticleLink();

      // See if the trklink is to an xAOD::TrackParticle
      Trk::LinkToXAODTrackParticle* linkToXAODTP =
        dynamic_cast<Trk::LinkToXAODTrackParticle*>(trklink);
      if (linkToXAODTP) {

        // Now set the new link to the xAOD vertex
        fittedVertex->addTrackAtVertex(*linkToXAODTP, VTAV->weight());

      } else {

        // See if the trklink is to an xAOD::NeutralParticle
        Trk::LinkToXAODNeutralParticle* linkToXAODTPneutral =
          dynamic_cast<Trk::LinkToXAODNeutralParticle*>(trklink);
        if (!linkToXAODTPneutral) {
          ATH_MSG_WARNING(
            "Skipping track. Trying to set link to something else than "
            "xAOD::TrackParticle or xAOD::NeutralParticle.");
        } else {
          // Now set the newlink to the new xAOD vertex
          fittedVertex->addNeutralAtVertex(*linkToXAODTPneutral,
                                           VTAV->weight());
        }
      }
    } // end of loop

    return fittedVertex;
   
 }//end of the xAOD starting point fit method

 xAOD::Vertex*
 AdaptiveVertexFitter::fit(
   const std::vector<const xAOD::TrackParticle*>& vectorTrk,
   const std::vector<const xAOD::NeutralParticle*>& vectorNeut,
   const xAOD::Vertex& constraint) const
 {

   if(vectorTrk.empty())
   {
     msg(MSG::INFO)<<"Empty vector of tracks passed"<<endmsg;
     return nullptr;
   }

   if(vectorNeut.empty())
   {
     msg(MSG::INFO)<<"Empty vector of neutrals passed"<<endmsg;
   }
   
   //making a list of perigee out of the vector of tracks
   std::vector<const Trk::TrackParameters*> measuredPerigees;
   
   for(std::vector<const xAOD::TrackParticle*>::const_iterator i = vectorTrk.begin(); i!= vectorTrk.end();++i)
   {
     const Trk::TrackParameters * tmpMeasPer = &((*i)->perigeeParameters());
  
     if(tmpMeasPer!=nullptr) measuredPerigees.push_back(tmpMeasPer);
     else  msg(MSG::INFO)<<"Failed to dynamic_cast this track parameters to perigee"<<endmsg; //TODO: Failed to implicit cast the perigee parameters to track parameters?
   }

   //making a list of perigee out of the vector of neutrals
   std::vector<const Trk::NeutralParameters*> measuredNeutralPerigees;

   for(std::vector<const xAOD::NeutralParticle*>::const_iterator i = vectorNeut.begin(); i!= vectorNeut.end();++i)
   {
     const Trk::NeutralParameters * tmpMeasPer = &((*i)->perigeeParameters());
  
     if(tmpMeasPer!=nullptr) measuredNeutralPerigees.push_back(tmpMeasPer);
     else  msg(MSG::INFO)<<"Failed to dynamic_cast this neutral parameters to perigee"<<endmsg; //TODO: Failed to implicit cast the perigee parameters to neutral parameters?
   }
   
   
   xAOD::Vertex* fittedVertex = _fit(measuredPerigees, measuredNeutralPerigees, constraint, Amg::Vector3D(),true);


   //assigning the input tracks to the fitted vertex through VxTrackAtVertices
   if(fittedVertex !=nullptr)
   {
     if( fittedVertex->vxTrackAtVertexAvailable() ) // TODO: I don't think vxTrackAtVertexAvailable() does the same thing as a null pointer check
     {
       if(!fittedVertex->vxTrackAtVertex().empty())
       {
         for(unsigned int i = 0; i <vectorTrk.size(); ++i)
         {

           LinkToXAODTrackParticle* linkTT = new LinkToXAODTrackParticle();
           const xAOD::TrackParticleContainer* cont = dynamic_cast< const xAOD::TrackParticleContainer* >( vectorTrk[ i ]->container() );
           if(  cont )
           {
             if( ! linkTT->toIndexedElement( *cont, vectorTrk[ i ]->index() ) )
             {
	       msg(MSG::WARNING)<<"Failed to set the EL for this particle correctly"<<endmsg;
             }
           } else {
             msg(MSG::WARNING)<<"Failed to identify a  container for this TP"<<endmsg;
           }//end of the dynamic cast check 


           // vxtrackatvertex takes ownership!
           ( fittedVertex->vxTrackAtVertex() )[i].setOrigTrack(linkTT);
         }//end of loop for setting orig tracks in.

         for(unsigned int i = 0; i <vectorNeut.size(); ++i)
         {
           LinkToXAODNeutralParticle* linkTT = new LinkToXAODNeutralParticle();
           const xAOD::NeutralParticleContainer* cont = dynamic_cast< const xAOD::NeutralParticleContainer* >( vectorNeut[ i ]->container() );
           if(  cont )
           {
             if( ! linkTT->toIndexedElement( *cont, vectorNeut[ i ]->index() ) )
             {
	       msg(MSG::WARNING)<<"Failed to set the EL for this particle correctly"<<endmsg;
	     }
           } else {
             msg(MSG::WARNING)<<"Failed to identify a  container for this NP"<<endmsg;
           }//end of the dynamic cast check 

           // vxtrackatvertex takes ownership!
           ( fittedVertex->vxTrackAtVertex() )[vectorTrk.size()+i].setOrigTrack(linkTT);
         }//end of loop for setting orig neutrals in.

       }//end of protection against unsuccessfull updates (no tracks were added)
     }//end of vector of tracks check
   }//end of pointer check


   //now set links to xAOD::TrackParticles directly in the xAOD::Vertex
   unsigned int VTAVsize = fittedVertex->vxTrackAtVertex().size();
   for (unsigned int i = 0 ; i < VTAVsize ; ++i)
   {
     Trk::VxTrackAtVertex* VTAV = &( fittedVertex->vxTrackAtVertex().at(i) );
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
       fittedVertex->addTrackAtVertex(*linkToXAODTP, VTAV->weight());

     } else {

       // See if the trklink is to an xAOD::NeutralParticle
       Trk::LinkToXAODNeutralParticle* linkToXAODTPneutral = dynamic_cast<Trk::LinkToXAODNeutralParticle*>(trklink);
       if (!linkToXAODTPneutral) {
         ATH_MSG_WARNING ("Skipping track. Trying to set link to something else than xAOD::TrackParticle or xAOD::NeutralParticle.");
       } else {
         //Now set the new link to the new xAOD vertex
         fittedVertex->addNeutralAtVertex(*linkToXAODTPneutral, VTAV->weight());
       }

     }
   } //end of loop

   return fittedVertex;

 }//end of the xAOD constrained fit method


}//end of namespace Trk definitions

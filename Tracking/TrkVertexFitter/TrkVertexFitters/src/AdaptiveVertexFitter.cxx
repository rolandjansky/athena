/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "VxVertex/VxCandidate.h"
#include "VxVertex/LinearizedTrack.h"
#include "TrkParametersBase/ParametersBase.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"

//xAOD includes
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

namespace Trk
{
  
  AdaptiveVertexFitter::AdaptiveVertexFitter(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_SeedFinder("Trk::CrossDistancesSeedFinder"),
    m_LinearizedTrackFactory("Trk::FullLinearizedTrackFactory"),
    m_TrackCompatibilityEstimator("Trk::Chi2TrackCompatibilityEstimator"),
    m_ImpactPoint3dEstimator("Trk::ImpactPoint3dEstimator/ImpactPoint3dEstimator"),
    m_VertexUpdator("Trk::KalmanVertexUpdator"),
    m_VertexSmoother("Trk::DummyVertexSmoother"),
    m_AnnealingMaker("Trk::DetAnnealingMaker"),
    m_xaodConverter("Trk::VxCandidateXAODVertex"),
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
    declareProperty("XAODConverter",m_xaodConverter);
    declareProperty("onlyzseed",m_onlyzseed);
    declareProperty("DoSmoothing",m_doSmoothing);
    declareInterface<IVertexFitter>(this);
  }

  AdaptiveVertexFitter::~AdaptiveVertexFitter() {}

  StatusCode AdaptiveVertexFitter::initialize()
  {
    StatusCode s = AlgTool::initialize();
    if (s.isFailure()) 
    {
      msg(MSG::FATAL) << "AlgTool::initialize() failed" << endreq;
      return StatusCode::FAILURE;
    }
    if ( m_SeedFinder.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_SeedFinder << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_SeedFinder << endreq;
    }

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
   if(m_doSmoothing)
   {
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
  
  //XAOD Converter  
  if ( m_xaodConverter.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_xaodConverter << endreq;
    return StatusCode::FAILURE;
   } else {
    msg(MSG::INFO) << "Retrieved tool " << m_xaodConverter << endreq;
   }
  
  
    msg(MSG::INFO)  << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }
  
  StatusCode AdaptiveVertexFitter::finalize()
  {
    msg(MSG::INFO)  << "Finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }

  VxCandidate * AdaptiveVertexFitter::_fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
					   const RecVertex& constraint,//initialized to RecVertex()
					   const Vertex & startingPoint,//initialized to Vertex()
					   bool IsConstraint,//initialized to false
					   bool IsStartingPoint)//initialized to false
  {

    //check the number of tracks
    if ( perigeeList.size()<2 && IsConstraint==false ) {
      msg(MSG::WARNING) << "Not enough tracks (>2) to fit in this event (without constraint)." << endreq;
      return 0;
    }
    if (perigeeList.size()<1 && IsConstraint==true) {
      msg(MSG::WARNING) << "Not enough tracks (>1) to fit in this event (with constraint)." << endreq;
      return 0;
    }
    
    Vertex* SeedPoint(0);
    //now find the best point for seeding and linearization of the tracks
    if (IsStartingPoint) {
      SeedPoint=new Vertex(startingPoint);
    } else {
      if (perigeeList.size()>1) {
	if (IsConstraint) {
	  SeedPoint=new Vertex(m_SeedFinder->findSeed(perigeeList,&constraint));
	} else {
	  SeedPoint=new Vertex(m_SeedFinder->findSeed(perigeeList));
 	}
      } else {
	SeedPoint=new Vertex(static_cast<Vertex>(constraint));
      }
    }

    
    //in case m_onlyzseed is on, just use only the z component given by the seed finder
    if (m_onlyzseed&&IsConstraint) {
      *SeedPoint=Vertex(Amg::Vector3D(constraint.position().x(),constraint.position().y(),SeedPoint->position().z()));
    }

    //now create a Global Position with the linearized track and the linearization point 
    //which in this case is taken from the found seed for the vertex
    //use the linPoint to linearize tracks
    
    std::vector<VxTrackAtVertex*> theLinTracks;

    std::vector<const Trk::TrackParameters*>::const_iterator perigeesBegin=perigeeList.begin();
    std::vector<const Trk::TrackParameters*>::const_iterator perigeesEnd=perigeeList.end();

    ATH_MSG_DEBUG("Inside fitter with perigee parameters.");
    ATH_MSG_DEBUG("Seed point: " << *SeedPoint);    
    int myDebugNTrk(0);
    for (std::vector<const Trk::TrackParameters*>::const_iterator perigeesIter=perigeesBegin;perigeesIter!=perigeesEnd;++perigeesIter) {
      //      const MeasuredPerigee* castToMP=dynamic_cast<const MeasuredPerigee*>(*perigeesIter);
      //      if (castToMP==0) {
      //	msg << MSG::ERROR << "Cast to MeasuredPerigee not successfull. Treatment of neutrals not supported. Skipping track..." << endreq;
      //	continue;
      //      }
      
      ATH_MSG_DEBUG("Track #" << myDebugNTrk++ << ". TrackParameters: x="  << (*perigeesIter)->position().x() << ", y="<< (*perigeesIter)->position().y() <<", z="<< (*perigeesIter)->position().z() << ". Covariance: " << *(*perigeesIter)->covariance());
      VxTrackAtVertex* LinTrackToAdd=new VxTrackAtVertex(0.,0,(*perigeesIter));
      //m_LinearizedTrackFactory->linearize(*LinTrackToAdd,*SeedPoint); why linearize it? maybe you don't need it at all!!!!! <19-05-2006>
      bool success=m_ImpactPoint3dEstimator->addIP3dAtaPlane(*LinTrackToAdd,*SeedPoint);
      if (!success)
      {
        msg(MSG::WARNING) << "Adding compatibility to vertex information failed. Newton distance finder didn't converge..." << endreq;
      }
      theLinTracks.push_back(LinTrackToAdd);
    }
    
    //Initialize the vertex seed with an arbitrary great error matrix (just starting point,
    //not really prior information in it)
    if(msgLvl(MSG::VERBOSE))
    {
      msg(MSG::VERBOSE) << "Error at the beginning is set to " << m_initialError << endreq;
    }
    
    

    RecVertex* ConstraintVertex(0);
    //use the previous prior vertex info for the initial vertex if there
    if (IsConstraint ) {
      ConstraintVertex=new RecVertex(constraint.position(),constraint.covariancePosition(),0.,0.);
    } else {
      AmgSymMatrix(3) startingCovMatrix;
      startingCovMatrix.setIdentity();
      startingCovMatrix = startingCovMatrix / m_initialError;
      //now initialize with starting position and covariance matrix
      ConstraintVertex=new RecVertex(SeedPoint->position(),startingCovMatrix,-3.,0.);
    }
    

    
    //now put the linearizedtracks into VxTrackAtVertex
    return dothefit(*ConstraintVertex,*SeedPoint,theLinTracks);
  }
  
  VxCandidate * AdaptiveVertexFitter::_fit(const std::vector<const Trk::Track*> & VectorTrk,
					   const RecVertex& constraint,//initialized to RecVertex()
					   const Vertex & startingPoint,//initialized to Vertex()
					   bool IsConstraint,//initializzed to false
					   bool IsStartingPoint) {//initialized to false
    

    ATH_MSG_DEBUG("Called Adaptive vertex with Trk::Track. N. Tracks = " << VectorTrk.size());
    
    std::vector<const Trk::TrackParameters*> perigeeList;
    for (std::vector<const Trk::Track*>::const_iterator iter=VectorTrk.begin();
	 iter!=VectorTrk.end();iter++) {
      if (std::isnan((*iter)->perigeeParameters()->parameters()[Trk::d0])) {
	continue;
      }  
      perigeeList.push_back((*iter)->perigeeParameters());
    }
    
    VxCandidate * FittedVertex=_fit(perigeeList,constraint,startingPoint,IsConstraint,IsStartingPoint);
    
    if (FittedVertex==0) {
      return FittedVertex;
    }
    
    //these following lines are really absurd... Why has this to be the case... <19-06-2006>
    //We need a link which can be dynamically to a Track or a TrackParticle....... )-:

    const std::vector<const Trk::Track*>::const_iterator trkbegin=VectorTrk.begin();
    const std::vector<const Trk::Track*>::const_iterator trkend=VectorTrk.end();

    const std::vector<VxTrackAtVertex*>::iterator vtxbegin=FittedVertex->vxTrackAtVertex()->begin();
    const std::vector<VxTrackAtVertex*>::iterator vtxend=FittedVertex->vxTrackAtVertex()->end();
        

    for (std::vector<const Trk::Track*>::const_iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) 
    {
      for (std::vector<Trk::VxTrackAtVertex*>::iterator vtxiter=vtxbegin;vtxiter!=vtxend;++vtxiter) 
      {
        if(((*trkiter)->perigeeParameters()->momentum() - 
	    (*vtxiter)->initialPerigee()->momentum()).mag()< 1e-8  && 
	    ((*trkiter)->perigeeParameters()->position() - 
	    (*vtxiter)->initialPerigee()->position()).mag()< 1e-8)
	{

         ElementLink<TrackCollection> link;
	 link.setElement(const_cast<Trk::Track*>(*trkiter));
	 LinkToTrack * linkTT = new LinkToTrack(link);
         (*vtxiter)->setOrigTrack(linkTT);
	}
      }
    }

    return FittedVertex;
  }
  
  VxCandidate * AdaptiveVertexFitter::_fit(const std::vector<const Trk::TrackParticleBase*> & VectorTrk,
					   const RecVertex& constraint,//initialized to RecVertex()
					   const Vertex & startingPoint,//initialized to Vertex()
					   bool IsConstraint,//initializzed to false
					   bool IsStartingPoint) {//initialized to false
    
    
    std::vector<const Trk::TrackParameters*> perigeeList;
    for (std::vector<const Trk::TrackParticleBase*>::const_iterator iter=VectorTrk.begin();
	 iter!=VectorTrk.end();iter++) {
      if (std::isnan((*iter)->perigee()->parameters()[Trk::d0])) {
	continue;
      }  
      perigeeList.push_back((*iter)->perigee());
    }
    
    VxCandidate * FittedVertex=_fit(perigeeList,constraint,startingPoint,IsConstraint,IsStartingPoint);
    
    if (FittedVertex==0) {
      return FittedVertex;
    }
    
    //these following lines are really absurd... Why has this to be the case... <19-06-2006>
    //We need a link which can be dynamically to a Track or a TrackParticle....... )-:

    const std::vector<const Trk::TrackParticleBase*>::const_iterator trkbegin=VectorTrk.begin();
    const std::vector<const Trk::TrackParticleBase*>::const_iterator trkend=VectorTrk.end();

    const std::vector<VxTrackAtVertex*>::iterator vtxbegin=FittedVertex->vxTrackAtVertex()->begin();
    const std::vector<VxTrackAtVertex*>::iterator vtxend=FittedVertex->vxTrackAtVertex()->end();
        

    for (std::vector<const Trk::TrackParticleBase*>::const_iterator trkiter=trkbegin;trkiter!=trkend;++trkiter) 
    {
      for (std::vector<Trk::VxTrackAtVertex*>::iterator vtxiter=vtxbegin;vtxiter!=vtxend;++vtxiter) 
      {
        if(((*trkiter)->perigee()->momentum() - 
	    (*vtxiter)->initialPerigee()->momentum()).mag()< 1e-8  && 
	    ((*trkiter)->perigee()->position() - 
	    (*vtxiter)->initialPerigee()->position()).mag()< 1e-8)
	{

         ElementLink<TrackParticleBaseCollection> link;
	 link.setElement(const_cast<Trk::TrackParticleBase*>(*trkiter));
	 LinkToTrackParticleBase * linkTT = new LinkToTrackParticleBase(link);
         (*vtxiter)->setOrigTrack(linkTT);
	}
      }
    }

    return FittedVertex;
  }


  VxCandidate * AdaptiveVertexFitter::dothefit(const RecVertex & ConstraintVertex, const Vertex & SeedVertex, std::vector<Trk::VxTrackAtVertex*> & myLinTracks) 
  {
       
    //now reset the annealing maker
    m_AnnealingMaker->reset();
    
    //Count the steps for the fit and the number of relinearizations needed in the fit
    int num_steps(0);
    int num_relinearizations(0);

    Trk::VxCandidate* ActualVxVertex = new VxCandidate(RecVertex(ConstraintVertex.position(),
								 ConstraintVertex.covariancePosition() / m_AnnealingMaker->getWeight(1.),
								 ConstraintVertex.fitQuality().doubleNumberDoF(),
								 ConstraintVertex.fitQuality().chiSquared()),
						                 myLinTracks);
    
    Trk::Vertex NewVertex= SeedVertex;
    
    Trk::Vertex ActualSeedPosition=SeedVertex;
      
    if(msgLvl(MSG::VERBOSE))
    {
      msg(MSG::VERBOSE) << "Num max of steps is " <<  m_maxIterations << endreq;
      msg(MSG::VERBOSE) << "m_AnnealingMaker->isEquilibrium() is " << m_AnnealingMaker->isEquilibrium() << endreq;
    }
    
    
    std::vector<Trk::VxTrackAtVertex*>::iterator lintracksBegin=ActualVxVertex->vxTrackAtVertex()->begin();
    std::vector<Trk::VxTrackAtVertex*>::iterator lintracksEnd=ActualVxVertex->vxTrackAtVertex()->end();

    std::vector<VxTrackAtVertex*>::iterator iter;

    bool relinearization=false;

    do {
      
      //I don't like it, but let's do it...
      RecVertex newVertex(ConstraintVertex.position(),
			  ConstraintVertex.covariancePosition() / m_AnnealingMaker->getWeight(1.),
			  ConstraintVertex.fitQuality().doubleNumberDoF(),
			  ConstraintVertex.fitQuality().chiSquared());
      
      ActualVxVertex->setRecVertex(newVertex);
      
      if(msgLvl(MSG::DEBUG))
      {
        msg(MSG::DEBUG) << "Correction applied to constraint weight is: " << m_AnnealingMaker->getWeight(1.) << endreq;
      }
      
      
      //To reweight here through an extrapolation is not ideal, but maybe I'll change this in the future...
      if(msgLvl(MSG::DEBUG))
      {
        msg(MSG::DEBUG) << "New fit step: step number " << num_steps << endreq;
      }
      
      //consider relinearization if you are too far away from the old lin point
      

      relinearization=false;
      if ((ActualSeedPosition.position()-SeedVertex.position()).perp()>m_maxDistToLinPoint) {
	relinearization=true;
        if(msgLvl(MSG::DEBUG))
        {
          msg(MSG::DEBUG) << "Actual vertex too far away from linearization point: have to linearize tracks again" << endreq;
        }
        
	//collect all measured perigees
	for (iter=lintracksBegin;iter!=lintracksEnd;iter++) {
	  //m_LinearizedTrackFactory->linearize(**iter,ActualPosition);
	  bool success=m_ImpactPoint3dEstimator->addIP3dAtaPlane(**iter,NewVertex);
          if (!success)
          {
            msg(MSG::WARNING) << "Adding compatibility to vertex information failed. Newton distance finder didn't converge..." << endreq;
          }
	}
	num_relinearizations+=1;
      }
      

      lintracksBegin=ActualVxVertex->vxTrackAtVertex()->begin();
      lintracksEnd=ActualVxVertex->vxTrackAtVertex()->end();

      //now reweight tracks (find chi2 compatibility with actual vertex position)
      for (iter=lintracksBegin;iter!=lintracksEnd;iter++) {
	
	//estimate the compatibility of the track to the vertex and store it in *iter->linState()->m_vtxCompatibility
	m_TrackCompatibilityEstimator->estimate(**iter,NewVertex);
	
	//use the obtained estimate and ask the Annealing Maker what is the corresponding weight at the actual temperature step
	(*iter)->setWeight(m_AnnealingMaker->getWeight((*iter)->vtxCompatibility()));
	if(msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << "Before annealing: " << (*iter)->vtxCompatibility() <<
              " Annealing RESULT is:" << (*iter)->weight() << " at T: " <<
              m_AnnealingMaker->actualTemp() << endreq;
        }
        
	
      }
      
      //now update with all the tracks info
      for (iter=lintracksBegin;iter!=lintracksEnd;iter++) {
        if(msgLvl(MSG::VERBOSE))
        {
          msg(MSG::VERBOSE) << "Updating vertex with a new track" << endreq;
        }
	try {
	  
	  if ((static_cast<VxTrackAtVertex*>(*iter))->weight()>1e-3) {
	    //now take care if linearization has been done at least once 
	    //or if vertex has changed so much so that you need relinearization!
	    if ((*iter)->linState()==0) {
	      //linearization has not been done so far: do it now!
              if(msgLvl(MSG::VERBOSE))
              {
                msg(MSG::VERBOSE) << "Linearizing track for the first time" << endreq;
              }
              
	      m_LinearizedTrackFactory->linearize(**iter,ActualVxVertex->recVertex().position());	   
	      //ATH_MSG_DEBUG("Linearized track to Seed Point. " << *((*iter)->linState()));
	      ATH_MSG_DEBUG("Linearized track to Seed Point. " << *(*iter));
	    } else if (relinearization==true) {
	      //do it again in case of updated vertex too far from previous one 
              if(msgLvl(MSG::VERBOSE))
              {
                msg(MSG::VERBOSE) << "Relinearizing track " << endreq;
              }
              
	      m_LinearizedTrackFactory->linearize(**iter,NewVertex.position());	      
	      ActualSeedPosition=NewVertex;
	      //ATH_MSG_DEBUG("Linearized track to new seed Point " << NewVertex << ". " << *((*iter)->linState()));
	      ATH_MSG_DEBUG("Linearized track to new seed Point " << NewVertex << ". " << *(*iter));
	    }
	    ActualVxVertex=m_VertexUpdator->add(*ActualVxVertex,**iter);
	  } else {
            if(msgLvl(MSG::VERBOSE))
            {
              msg(MSG::VERBOSE) << "Weight lower than 1e-3, so the track will not be considered anymore" << endreq;
            }
            
	  }	  
	} catch (...) {
	  msg(MSG::WARNING) << "You have just lost a track" << endreq;
	}
      }

      

      //show some info about the position
      if(msgLvl(MSG::DEBUG))
      {
        msg(MSG::DEBUG) << "New position x: " << ActualVxVertex->recVertex().position().x() << " y: " << ActualVxVertex->recVertex().position().y()  
                        << " z: " << ActualVxVertex->recVertex().position().z() << endreq;
      }
      
      NewVertex=static_cast<Vertex>(ActualVxVertex->recVertex());

      //now go one step down in the annealing process (lower the temperature - single step)
      if(msgLvl(MSG::VERBOSE))
      {
        msg(MSG::VERBOSE) << "Now calling one step of annealing" << endreq;
      }
      
      m_AnnealingMaker->anneal();
      num_steps+=1;

      //continue to fit until max iteration number has been reached or "thermal equilibrium"
      //has been obtained in the annealing process
      
    } while (num_steps<m_maxIterations && !(m_AnnealingMaker->isEquilibrium()) );
    
    //Here smooth the vertex (refitting of the track)
    
    //without smoothing... for the moment...
    if(m_doSmoothing) {
      m_VertexSmoother->smooth(*ActualVxVertex);
    } else {
      for (iter=lintracksBegin;iter!=lintracksEnd;iter++) {
	//	const MeasuredPerigee* castToMP=dynamic_cast<const MeasuredPerigee*>((*iter)->initialPerigee());
	//	if (castToMP==0) {
	//	  msg(MSG::WARNING) << "Couldn't cast a track to MeasuredPerigee to smooth it. Neutrals not supported. " <<
	//	    " Skipping track. .. " << endreq;
	//	  continue;
	//	}
	(*iter)->setPerigeeAtVertex(((*iter)->initialPerigee())->clone());
      }
    }
      
    if(msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG) << "chi2: " << ActualVxVertex->recVertex().fitQuality().chiSquared() 
                      << "the ndf of the vertex is at fit end: " << ActualVxVertex->recVertex().fitQuality().doubleNumberDoF() << endreq;
    }
    
    
    delete &ConstraintVertex;
    delete &SeedVertex;
   

    //Give back all info into the ActualVxVertex VxCandidate       
    return ActualVxVertex;
  }
  

  VxCandidate * AdaptiveVertexFitter::fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
		    const Vertex& startingPoint) 
		    {
    return _fit(perigeeList,RecVertex(),startingPoint,false,true);
  }

  VxCandidate * AdaptiveVertexFitter::fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
		    const RecVertex& constraint) {
    return _fit(perigeeList,constraint,Vertex(),true);
  }

  VxCandidate * AdaptiveVertexFitter::fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
					  const RecVertex& constraint,
					  const Vertex & startingPoint) {
    return _fit(perigeeList,constraint,startingPoint,true,true);
  }

  VxCandidate * AdaptiveVertexFitter::fit(const std::vector<const Trk::TrackParameters*> & perigeeList) {
    return _fit(perigeeList,RecVertex(),Vertex(),false,false);
  }

  VxCandidate * AdaptiveVertexFitter::fit(const std::vector<const Trk::Track*> & vectorTrk,
		    const Vertex & startingPoint) {
    return _fit(vectorTrk,RecVertex(),startingPoint,false,true);
  }

  VxCandidate * AdaptiveVertexFitter::fit(const std::vector<const Trk::TrackParticleBase*> & vectorTrk,
		    const Vertex & startingPoint) {
    return _fit(vectorTrk,RecVertex(),startingPoint,false,true);
  }

  VxCandidate * AdaptiveVertexFitter::fit(const std::vector<const Trk::Track*>& vectorTrk,
		    const RecVertex& constraint) {
    return _fit(vectorTrk,constraint,Vertex(),true);
  }

  VxCandidate * AdaptiveVertexFitter::fit(const std::vector<const Trk::TrackParticleBase*>& vectorTrk,
		    const RecVertex& constraint) {
    return _fit(vectorTrk,constraint,Vertex(),true);
  }

  VxCandidate * AdaptiveVertexFitter::fit(const std::vector<const Trk::Track*> & vectorTrk) {
    return _fit(vectorTrk,RecVertex(),Vertex(),false,false);
  }

  VxCandidate * AdaptiveVertexFitter::fit(const std::vector<const Trk::TrackParticleBase*> & vectorTrk) {
    return _fit(vectorTrk,RecVertex(),Vertex(),false,false);
  }
  


//xAOD interfaced methods. Required to un-block the current situation 
// with the xAOD tracking design.
 xAOD::Vertex * AdaptiveVertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,const Vertex& startingPoint)
 {	
   ATH_MSG_DEBUG("Called Adaptive vertex with xAOD::TrackParticle. N. Tracks = " << vectorTrk.size());

   if(vectorTrk.size() == 0)
   {
    msg(MSG::INFO)<<"Empty vector of tracks passed"<<endreq;
    return 0;
   }
   
//making a list of perigee out of the vector of tracks  
   std::vector<const Trk::TrackParameters*> measuredPerigees;
   
   for(std::vector<const xAOD::TrackParticle*>::const_iterator i = vectorTrk.begin(); i!= vectorTrk.end();++i)
   {
    const Trk::TrackParameters * tmpMeasPer = &((*i)->perigeeParameters());
  
    if(tmpMeasPer!=0) measuredPerigees.push_back(tmpMeasPer);
    else  msg(MSG::INFO)<<"Failed to dynamic_cast this track parameters to perigee"<<endreq;
   }

   Trk::VxCandidate* fittedVxCandidate = _fit(measuredPerigees, RecVertex(),startingPoint,false,true);				 
   
    //assigning the input tracks to the fitted vertex   
   if(fittedVxCandidate !=0)
   {
    if(fittedVxCandidate->vxTrackAtVertex() != 0)
    {
     if(fittedVxCandidate->vxTrackAtVertex()->size() !=0)
     {
      for(unsigned int i = 0; i <vectorTrk.size(); ++i)
      {

       LinkToXAODTrackParticle* linkTT= new LinkToXAODTrackParticle();
       const xAOD::TrackParticleContainer* cont = dynamic_cast< const xAOD::TrackParticleContainer* >( vectorTrk[ i ]->container() );
       if(  cont )
       {
        if( ! linkTT->toIndexedElement( *cont, vectorTrk[ i ]->index() ) )
	{
	 msg(MSG::WARNING)<<"Failed to set the EL for this particle correctly"<<endreq;
	}
       }else{
        msg(MSG::WARNING)<<"Failed to identify a  container for this TP"<<endreq;
       }//end of the dynamic cast check 

//       LinkToXAODTrackParticle * linkTT = new LinkToXAODTrackParticle;
//       linkTT->setElement(vectorTrk[i]);

// vxtrackatvertex takes ownership!
       (*(fittedVxCandidate->vxTrackAtVertex()))[i]->setOrigTrack(linkTT);
      }//end of loop for setting orig tracks in.
     }//end of protection against unsuccessfull updates (no tracks were added)
    }//end of vector of tracks check
   }//end of pointer check

   xAOD::Vertex * return_vertex(0);   
//   if(fittedVxCandidate) m_xaodConverter->createXAODVertex(*fittedVxCandidate,return_vertex);
   if(fittedVxCandidate) 
   {
     if( m_xaodConverter->createXAODVertex(*fittedVxCandidate,return_vertex).isFailure() ) 
     {
       msg(MSG::FATAL)<<"Failed to create xAODVertex for VxCandidate"<<endreq;
       return 0;
      }//end of protection check
   }//end of protection check
  
  
   return return_vertex;  
   
 }//end of the xAOD starting point fit method

    
 xAOD::Vertex * AdaptiveVertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, const RecVertex& constraint)
 {

   if(vectorTrk.size() == 0)
   {
    msg(MSG::INFO)<<"Empty vector of tracks passed"<<endreq;
    return 0;
   }
   
//making a list of perigee out of the vector of tracks  
   std::vector<const Trk::TrackParameters*> measuredPerigees;
   
   for(std::vector<const xAOD::TrackParticle*>::const_iterator i = vectorTrk.begin(); i!= vectorTrk.end();++i)
   {
    const Trk::TrackParameters * tmpMeasPer = &((*i)->perigeeParameters());
  
    if(tmpMeasPer!=0) measuredPerigees.push_back(tmpMeasPer);
    else  msg(MSG::INFO)<<"Failed to dynamic_cast this track parameters to perigee"<<endreq;
   }
   
   
   Trk::VxCandidate* fittedVxCandidate = _fit(measuredPerigees,constraint,Vertex(),true);
 
 //assigning the input tracks to the fitted vertex   
   if(fittedVxCandidate !=0)
   {
    if(fittedVxCandidate->vxTrackAtVertex() != 0)
    {
     if(fittedVxCandidate->vxTrackAtVertex()->size() !=0)
     {
      for(unsigned int i = 0; i <vectorTrk.size(); ++i)
      {

//       LinkToXAODTrackParticle * linkTT = new LinkToXAODTrackParticle;
//       linkTT->setElement(vectorTrk[i]);


       LinkToXAODTrackParticle* linkTT = new LinkToXAODTrackParticle();
       const xAOD::TrackParticleContainer* cont = dynamic_cast<const xAOD::TrackParticleContainer* >( vectorTrk[ i ]->container() );
       if(  cont )
       {
        if( ! linkTT->toIndexedElement( *cont, vectorTrk[ i ]->index() ) )
	{
	 msg(MSG::WARNING)<<"Failed to set the EL for this particle correctly"<<endreq;
	}
       }else{
        msg(MSG::WARNING)<<"Failed to identify a  container for this TP"<<endreq;
       }//end of the dynamic cast check 



// vxtrackatvertex takes ownership!
       (*(fittedVxCandidate->vxTrackAtVertex()))[i]->setOrigTrack(linkTT);
      }//end of loop for setting orig tracks in.
     }//end of protection against unsuccessfull updates (no tracks were added)
    }//end of vector of tracks check
   }//end of pointer check

   xAOD::Vertex * return_vertex(0);   
//   if(fittedVxCandidate) m_xaodConverter->createXAODVertex(*fittedVxCandidate,return_vertex);
   if(fittedVxCandidate) 
   {
     if( m_xaodConverter->createXAODVertex(*fittedVxCandidate,return_vertex).isFailure() ) 
     {
       msg(MSG::FATAL)<<"Failed to create xAODVertex for VxCandidate"<<endreq;
       return 0;
      }//end of protection check
   }//end of protection check
  
  
   return return_vertex;

 }//end of the xAOD constrained fit method


}//end of namespace Trk definitions

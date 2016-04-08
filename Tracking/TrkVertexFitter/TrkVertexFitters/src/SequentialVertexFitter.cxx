/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexFitters/SequentialVertexFitter.h"
#include "TrkTrack/Track.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/LinearizedTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParametersBase/ParametersBase.h"

//flexible interfaces
#include "TrkVertexFitterInterfaces/IVertexUpdator.h"
#include "TrkVertexFitterInterfaces/IVertexSmoother.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "TrkLinks/LinkToXAODNeutralParticle.h"
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"


//xAOD includes
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

//ugly so far: the fitter is connected directly to the
//perigee track parametrization, which is not right.
//should be replaced later with arbitrary parametrization....

namespace Trk{

//usual tool related methods
 StatusCode SequentialVertexFitter::initialize()
 {
  
//uploading the corresponding tools
//updator 
  if ( m_Updator.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_Updator << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_Updator << endreq;
  }
  
//smoother
  if(m_doSmoothing)
  {
    if ( m_Smoother.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_Smoother << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_Smoother << endreq;
    }
  } else msg(MSG::DEBUG) << "Smoothing turned off" << endreq;

//Linearized Track Factory 
  if ( m_LinTrkFactory.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_LinTrkFactory << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_LinTrkFactory << endreq;
  }
  
//XAOD Converter  
  if ( m_xaodConverter.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_xaodConverter << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_xaodConverter << endreq;
  }
  
  msg(MSG::INFO)<<"Initialization successfull"<<endreq;
  return StatusCode::SUCCESS;
 }//end of initialize method


 StatusCode SequentialVertexFitter::finalize()
 {
  msg(MSG::INFO)  << "Finalize successful" << endreq;
  return StatusCode::SUCCESS;
 }

//class constructor implementation
 SequentialVertexFitter::SequentialVertexFitter(const std::string& t, const std::string& n, const IInterface*  p):
    AthAlgTool(t,n,p),
    m_Updator("Trk::KalmanVertexUpdator"),
    
//    m_Smoother("Trk::KalmanVertexSmoother"),
    m_Smoother("Trk::DummyVertexSmoother"),
    m_LinTrkFactory("Trk::FullPerigeeLinearizedTrackFactory"),
    m_xaodConverter("Trk::VxCandidateXAODVertex"),
    m_doSmoothing(true),
    m_maxStep(20),
    m_maxShift(0.0001),
    m_useLooseConvergence(false),
    m_maxDeltaChi2(1e-3),
    m_maxR(1150.),//max R of ID
    m_maxZ(2727.)
{
//convergence stuff 
  declareProperty("MaxIterations",m_maxStep);
  declareProperty("MaxShift",m_maxShift);
  declareProperty("useLooseConvergence",m_useLooseConvergence);
  declareProperty("maxDeltaChi2",m_maxDeltaChi2);

//updator-related stuff  
  declareProperty("VertexUpdator",m_Updator);
  
//smoother-related stuff 
  declareProperty("DoSmoothing",m_doSmoothing);
  declareProperty("VertexSmoother",m_Smoother);
  
  declareProperty("ID_maxR",m_maxR);
  declareProperty("ID_maxZ",m_maxZ);

//linearizedTrackFactory-related stuff  
  declareProperty("LinearizedTrackFactory", m_LinTrkFactory);
  declareProperty("XAODConverter",m_xaodConverter);
  declareInterface<IVertexFitter>(this);
 }

//destructor
 SequentialVertexFitter::~SequentialVertexFitter(){}
 
//implementation of the fit methods 
//converion from the vectro of tracks and starting point
  VxCandidate * SequentialVertexFitter::fit(const std::vector<const Trk::Track*> & vectorTrk,
                             const Vertex& startingPoint)
  { 
   return fit(vectorTrk, RecVertex(startingPoint));
  }   
 
//conversion from the vector of tracks and starting point
  VxCandidate * SequentialVertexFitter::fit(const std::vector<const Trk::Track*>& vectorTrk,
                             const RecVertex& constraint) 
  {
   if(vectorTrk.size() == 0)
   {
    msg(MSG::INFO)<<"Empty vector of tracks passed"<<endreq;
    return 0;
   }
   
//making a list of perigee out of the vector of tracks  
//Also check for duplicate tracks (will otherwise be discarded by the fit); they do not
// fit the hypothesis of statistically independent tracks. Print a warning and discard the duplicate
// it this happens.
   std::vector<const Trk::TrackParameters*> measuredPerigees;
   std::vector<const Trk::Track*> trkToFit;
   for(std::vector<const Trk::Track*>::const_iterator i = vectorTrk.begin(); i!= vectorTrk.end();++i)
   {
     //check for duplicates
     bool foundDuplicate(false);
     for (std::vector<const Trk::Track*>::const_iterator j = vectorTrk.begin(); j!= i; ++j) {
       if (*i == *j) {
	 ATH_MSG_WARNING("Duplicate track given as input to the fitter. Ignored.");
	 foundDuplicate = true;
	 break;
       }
     }
     if (foundDuplicate) continue; //skip track
//    const Trk::MeasuredPerigee *tmpMeasPer(dynamic_cast<const Trk::MeasuredPerigee*>((*i)->perigeeParameters()));
    const Trk::TrackParameters * tmpMeasPer = (*i)->perigeeParameters();
  
    if(tmpMeasPer!=0) {
      trkToFit.push_back(*i);
      measuredPerigees.push_back(tmpMeasPer);
    } else  {
      msg(MSG::INFO)<<"Failed to dynamic_cast this track parameters to perigee"<<endreq;
    }
   }
   
   Trk::VxCandidate* fittedVxCandidate = fit( measuredPerigees, constraint );

//assigning the input tracks to the fitted vertex   
   if(fittedVxCandidate !=0)
   {
    if(fittedVxCandidate->vxTrackAtVertex() != 0)
    {
     if(fittedVxCandidate->vxTrackAtVertex()->size() !=0)
     {
      for(unsigned int i = 0; i <trkToFit.size(); ++i)
      {
//      (*(fittedVxCandidate->vxTrackAtVertex()))[i]->setOrigTrack(trkToFit[i]);
       LinkToTrack * linkTT = new LinkToTrack;
       linkTT->setElement(const_cast<Trk::Track*>(trkToFit[i]));
       (*(fittedVxCandidate->vxTrackAtVertex()))[i]->setOrigTrack(linkTT);
      }//end of loop for setting orig tracks in.
     }//end of protection against unsuccessfull updates (no tracks were added)
    }
   }
   return fittedVxCandidate;
  }
   
//implementation of the fit methods 
//converion from the vectro of tracks and starting point
  VxCandidate * SequentialVertexFitter::fit(const std::vector<const Trk::TrackParticleBase*> & vectorTrk,
                             const Vertex& startingPoint)
  { 
   return fit(vectorTrk, RecVertex(startingPoint));
  }   
 
//conversion from the vector of tracks and starting point
  VxCandidate * SequentialVertexFitter::fit(const std::vector<const Trk::TrackParticleBase*>& vectorTrk,
                             const RecVertex& constraint) 
  {
   if(vectorTrk.size() == 0)
   {
    msg(MSG::INFO)<<"Empty vector of tracks passed"<<endreq;
    return 0;
   }
   
//making a list of perigee out of the vector of tracks  
//Also check for duplicate tracks (will otherwise be discarded by the fit); they do not
// fit the hypothesis of statistically independent tracks. Print a warning and discard the duplicate
// it this happens.
   std::vector<const Trk::TrackParameters*> measuredPerigees;
   std::vector<const Trk::TrackParticleBase*> trkToFit;
   for(std::vector<const Trk::TrackParticleBase*>::const_iterator i = vectorTrk.begin(); i!= vectorTrk.end();++i)
   {
     //check for duplicates
     bool foundDuplicate(false);
     for (std::vector<const Trk::TrackParticleBase*>::const_iterator j = vectorTrk.begin(); j!= i; ++j) {
       if (*i == *j) {
	 ATH_MSG_WARNING("Duplicate track given as input to the fitter. Ignored.");
	 foundDuplicate = true;
	 break;
       }
     }
     if (foundDuplicate) continue; //skip track
//    const Trk::MeasuredPerigee *tmpMeasPer(dynamic_cast<const Trk::MeasuredPerigee*>((*i)->perigeeParameters()));
    const Trk::TrackParameters * tmpMeasPer = &((*i)->definingParameters());
  
    if(tmpMeasPer!=0) {
      trkToFit.push_back(*i);
      measuredPerigees.push_back(tmpMeasPer);
    } else  {
      msg(MSG::INFO)<<"Failed to dynamic_cast this track parameters to perigee"<<endreq;
    }
   }
   
   Trk::VxCandidate* fittedVxCandidate = fit( measuredPerigees, constraint );

//assigning the input tracks to the fitted vertex   
   if(fittedVxCandidate !=0)
   {
    if(fittedVxCandidate->vxTrackAtVertex() != 0)
    {
     if(fittedVxCandidate->vxTrackAtVertex()->size() !=0)
     {
      for(unsigned int i = 0; i <trkToFit.size(); ++i)
      {
//      (*(fittedVxCandidate->vxTrackAtVertex()))[i]->setOrigTrack(trkToFit[i]);
       LinkToTrackParticleBase * linkTT = new LinkToTrackParticleBase;
       linkTT->setElement(trkToFit[i]);
       // vxtrackatvertex takes ownership!
       (*(fittedVxCandidate->vxTrackAtVertex()))[i]->setOrigTrack(linkTT);
      }//end of loop for setting orig tracks in.
     }//end of protection against unsuccessfull updates (no tracks were added)
    }
   }
   return fittedVxCandidate;
  }//end of TrackParticleBase-based method
   
//conversion from the perigeeList and starting point   
  VxCandidate * SequentialVertexFitter::fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
					    const std::vector<const Trk::NeutralParameters*> & neutralPerigeeList,
                              const Vertex& startingPoint) 
  {   
  
//    std::cout << " Starting point: " << startingPoint << std::endl;

    Trk::VxCandidate * fittedVxCandidate  = fit(perigeeList, neutralPerigeeList, RecVertex(startingPoint));
   
//setting the initial perigees  
   if(fittedVxCandidate !=0 )
   {
    if(fittedVxCandidate->vxTrackAtVertex() != 0)
    {
     if(fittedVxCandidate->vxTrackAtVertex()->size() !=0)
     {
      for(unsigned int i = 0; i <perigeeList.size(); ++i)
      {
       Trk::TrackParameters* iPer = const_cast<Trk::TrackParameters*>(perigeeList[i]);
       (*(fittedVxCandidate->vxTrackAtVertex()))[i]->setInitialPerigee(iPer);
      }
      //same for neutrals
      for(unsigned int i = 0; i <neutralPerigeeList.size(); ++i)      {
	Trk::NeutralParameters* iPer = const_cast<Trk::NeutralParameters*>(neutralPerigeeList[i]);
	(*(fittedVxCandidate->vxTrackAtVertex()))[perigeeList.size()+i]->setInitialPerigee(iPer);
      }
     } //end of protection against unsuccessfull updates (no tracks or neutrals were added)
    }
   }
   return fittedVxCandidate; 
  
  }
  
//additional new fitting methods  
  VxCandidate * SequentialVertexFitter::fit(const std::vector<const Trk::TrackParameters*>& perigeeList, const std::vector<const Trk::NeutralParameters*> & neutralPerigeeList)
   {    
   
//this method will later be modifyed to use the a finder
//uses a default starting point so  far.
    Amg::Vector3D start_point(0.,0.,0.);
    const Trk::Vertex start_vertex(start_point); 
    return fit(perigeeList, neutralPerigeeList, start_vertex);
   }

   VxCandidate * SequentialVertexFitter::fit(const std::vector<const Trk::Track*>& vectorTrk)
   {

//this method will later be modifyed to use the a finder
//uses a default starting point so  far.
    Amg::Vector3D start_point(0.,0.,0.);
    const Trk::Vertex start_vertex(start_point); 

//    std::cout << " start vertex is: " << start_vertex << std::endl;

    return fit(vectorTrk, RecVertex(start_vertex));
   }
    
//method where the actual fit is done  
  VxCandidate * SequentialVertexFitter::fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
					    const std::vector<const Trk::NeutralParameters*> & neutralPerigeeList,
					    const RecVertex& constraint)   { 

    //   std::cout << " Number of tracks provided: " << perigeeList.size() << std::endl;

//security check  
   if(perigeeList.size() == 0 ) 
   {
    msg()<<"Empty vector of tracks passed, returning 0"<<endreq;
    return 0;
   }
  
//identifying the input parameters of the fit
//and making initial vertex candidate to be updated with tracks
   Amg::Vector3D priorVertexPosition = constraint.position();
   AmgSymMatrix(3) initialVertexError = constraint.covariancePosition();
   AmgSymMatrix(3) priorErrorMatrix;
  
   double in_chi = 0.;
   double in_ndf = 0.;  
   
   bool priorInfo=false;

//checking whether the prior information has a defined error matrix:   
   if(initialVertexError.trace() == 0.)
   {
   
//no prior estimate, using the hughe error method   
//creating a covariance matrix:
    float diag = 1000000.0;
    AmgSymMatrix(3) in_m;
    in_m.setIdentity();
    priorErrorMatrix = in_m * diag;
    
//   we're  now working without prior knowledge, 
//   so the effective ndf is reduced by 3.
    in_ndf-=3.;
   }else {
     priorInfo=true;
//     std::cout << " prior information exists: WRONG! " << std::endl;
     priorErrorMatrix = initialVertexError;
   }
   
   //   std::cout <<  "STARTING FIT" << std::endl;
   
//initial RecVertex    
   Trk::RecVertex  initialRecVertex(priorVertexPosition, priorErrorMatrix , in_ndf, in_chi);    
     
//converting the input perigee to the Vertex tracks  
   std::vector<Trk::VxTrackAtVertex*> tracks_to_fit = linearizeTracks(perigeeList, neutralPerigeeList, initialRecVertex); 
   std::vector<Trk::VxTrackAtVertex*> fittedTracks(0);
   //   std::cout << "Linearized tracks to fit: " << tracks_to_fit.size() << std::endl;
 
//creating an initial vertex to update     
//   Trk::VxCandidate * initialVertex  = new Trk::VxCandidate(initialRecVertex, fittedTracks);   
   Trk::VxCandidate * returnVertex  =  new Trk::VxCandidate(initialRecVertex, fittedTracks);  
       
//the actual fitting loop
   Amg::Vector3D newPosition = initialRecVertex.position();
   Amg::Vector3D previousPreviousPosition=newPosition;
   Amg::Vector3D previousPosition=newPosition;
   double newChi2=initialRecVertex.fitQuality().chiSquared();
   double previousChi2;
   unsigned int n_iter = 0; 

   std::vector<Trk::VxTrackAtVertex*>::iterator tracksBegin=tracks_to_fit.begin();
   std::vector<Trk::VxTrackAtVertex*>::iterator tracksEnd=tracks_to_fit.end();

   do
   {

     if (!priorInfo)
     {
       Trk::RecVertex  newInitialRecVertex(newPosition, priorErrorMatrix , in_ndf, in_chi);    
       returnVertex->setRecVertex(newInitialRecVertex);
     }
     else
     {
       returnVertex->setRecVertex(initialRecVertex);
     }
    
//optional relinearization  
     if(n_iter !=0 && returnVertex != 0) {
       //std::cout << " relinearization " << std::endl;
       reLinearizeTracks(tracks_to_fit, returnVertex->recVertex());
     }
     
//    std::cout << " relinarzation " << std::endl;

//loop over available tracks
    int cnt =0;
     
    for(std::vector<Trk::VxTrackAtVertex*>::iterator i = tracksBegin; i != tracksEnd;++i)
    {   
     returnVertex = m_Updator->add(*returnVertex, **i);
     //std::cout << " Vertex after add of track: " << *returnVertex << std::endl;
     //std::cout << " Adding track " << cnt << std::endl;
     ++cnt;
    }//end of loop over available tracks
    //std::cout << "Number of tracks at vertex: " << returnVertex->vxTrackAtVertex()->size() <<std::endl;
          
//now the updated state is stored in returnVertex     
    previousPreviousPosition = previousPosition;
    previousPosition = newPosition;
    previousChi2=newChi2;
    newPosition = returnVertex->recVertex().position();
    newChi2=returnVertex->recVertex().fitQuality().chiSquared();
//    std::cout << " new position " << newPosition << std::endl;
    ++n_iter;

    /*
      std::cout << " iter: " << n_iter << std::setprecision(6) << " chi2: " << newChi2 << " deltaChi2: " << fabs((newChi2-previousChi2)*2./(newChi2+previousChi2+2.)) << " deltaR: "<< (previousPosition - newPosition).perp() << std::endl;
      std::cout << " new Position: " << newPosition << std::endl;
      std::cout << " error x " << sqrt(returnVertex->recVertex().errorPosition().covariance()[0][0]) << 
          " error y " << sqrt(returnVertex->recVertex().errorPosition().covariance()[1][1]) 
                << " error z " << sqrt(returnVertex->recVertex().errorPosition().covariance()[2][2])  << std::endl;
    */
   }while( (n_iter != m_maxStep)&& 
           ( ((previousPosition - newPosition).perp()>m_maxShift&&m_useLooseConvergence==true)
             ||(m_useLooseConvergence==false&&fabs((newChi2-previousChi2)*2./(newChi2+previousChi2+2.))>m_maxDeltaChi2) ) );
   
   if (n_iter==m_maxStep) msg(MSG::DEBUG) << " Fit didn't converge after " << m_maxStep << " steps. Deltachi2: "<< 
                              fabs((newChi2-previousChi2)*2./(newChi2+previousChi2+2.)) << " DeltaR " << (previousPosition - newPosition).perp() << endreq;

   /*
   if (n_iter!=m_maxStep)
   {
     std::cout << " Converged after : " << n_iter << " steps. " << std::endl;
   }
   */
   
//   delete initialVertex;

   //GP: memory leak (delete the used tracks!)
    for(std::vector<Trk::VxTrackAtVertex*>::iterator i = tracksBegin; i != tracksEnd;++i)
    {   
      delete *i;
      *i=0;
    }//end of loop over available tracks

   //FIT FAILED
    if (n_iter==m_maxStep)
    {
      msg(MSG::DEBUG) << " Fit failed. " << endreq;
      delete returnVertex;
      returnVertex=0;
      return 0;
    }



//smoothing and related  
   if(returnVertex !=0)
   {
    if(m_doSmoothing)m_Smoother->smooth(*returnVertex);     
    //std::cout << " after smoothing: " << *returnVertex << std::endl;
   }else msg(MSG::INFO)<<"Sequential vertex fit fails:: zero pointer returned"<<endreq;

//here the vertex is returned. It is foreseen that a vertex is _always_ 
//returned (initial guess in worst case) unless there is a runtime crash   


//some debug output which may be useful later

   /*
   std::cout<<"Returning the vertex "<<std::endl;
   std::cout<<"With fitted position "<<returnVertex->recVertex().position()<<std::endl;
   std::cout<<"With position error "<<returnVertex->recVertex().errorPosition()<<std::endl;
   std::cout<<"Number of fitted tracks "<<returnVertex->vxTrackAtVertex()->size()<<std::endl;
   std::cout<<"chi2 of the vertex: "<<returnVertex->recVertex().fitQuality().chiSquared()<<std::endl;
   std::cout<<"ndf of the vertex: "<<returnVertex->recVertex().fitQuality().numberDoF()<<std::endl;
   std::cout<<"checking the track contents: "<<std::endl;
  
   const std::vector<Trk::VxTrackAtVertex*> * ftrk = returnVertex->vxTrackAtVertex();
   
   for(std::vector<Trk::VxTrackAtVertex*>::const_iterator i = ftrk->begin(); i != ftrk->end(); ++i)
   {
    std::cout<<"Vertex compatibility "<<(*i)->vtxCompatibility()<<std::endl;
    std::cout<<"Track weight: "<< (*i)->weight()<<std::endl;
   }
   */
   
   return returnVertex; 

  }//end of the actual fit method


//initial linearization of tracks--------------------------------------------------------------------------------------------
 std::vector<Trk::VxTrackAtVertex*>  SequentialVertexFitter::linearizeTracks(const std::vector<const Trk::TrackParameters*> & perigeeList, 
									     const std::vector<const Trk::NeutralParameters*> & neutralPerigeeList,
									     const RecVertex & vrt) const
 { 
 
 
//defining the output vector 
  std::vector<Trk::VxTrackAtVertex*> out_tracks(0);   
  for(std::vector<const Trk::TrackParameters*>::const_iterator i = perigeeList.begin();i!= perigeeList.end();++i)
  {
  
//creating new meas perigees, since the will be deleted 
//by VxTrackAtVertex in destructor
   const Trk::Perigee * m_loc_per = dynamic_cast<const Trk::Perigee *>(*i);
   if( m_loc_per != 0)
   {
    Trk::Perigee * mPer  = new Trk::Perigee(*m_loc_per);
    const Trk::Perigee * inPer = m_loc_per;
    //new MeasuredPerigee(*m_loc_per);
    Trk::VxTrackAtVertex * vTrack = new Trk::VxTrackAtVertex(0., mPer, NULL, inPer, NULL);
   
//linearization itself   
    m_LinTrkFactory->linearize(*vTrack,vrt);
    vTrack->setWeight(1.);
    
    out_tracks.push_back(vTrack); 
   }else{
    msg(MSG::WARNING)<<"Cannot linearize tracks; treatment of neutrals not yet supported"<<endreq;
   } 
  }//end of loop over all the perigee states
  //same for neutrals
  for(std::vector<const Trk::NeutralParameters*>::const_iterator i = neutralPerigeeList.begin();i!= neutralPerigeeList.end();++i)  {
    //creating new meas perigees, since the will be deleted 
    //by VxTrackAtVertex in destructor
    const Trk::NeutralPerigee * m_loc_per = dynamic_cast<const Trk::NeutralPerigee *>(*i);
    if( m_loc_per != 0)   {
      Trk::NeutralPerigee * mPer  = new Trk::NeutralPerigee(*m_loc_per);
      const Trk::NeutralPerigee * inPer = m_loc_per;
      Trk::VxTrackAtVertex * vTrack = new Trk::VxTrackAtVertex(0., NULL, mPer, NULL, inPer);
      //linearization itself   
      m_LinTrkFactory->linearize(*vTrack,vrt);
      vTrack->setWeight(1.);
      out_tracks.push_back(vTrack); 
    }else{
      msg(MSG::WARNING)<<"Cannot linearize tracks; treatment of neutrals not yet supported"<<endreq;
    } 
  }//end of loop over all the neutral perigee states
 
  return out_tracks;  
 }//end of linearize method  

//relinearization of tracks during iterations------------------------------------------------------ 
 void SequentialVertexFitter::reLinearizeTracks(std::vector<Trk::VxTrackAtVertex*>& tracks, 
                         const Vertex & vrt) const
 {

   Vertex vrtPos=vrt;

   Amg::Vector3D linVertexPos=vrt.position();
   if (linVertexPos.perp()>m_maxR || fabs(linVertexPos.z())>m_maxZ)
   {
     msg(MSG::DEBUG) << " Linearization position outside ID. Setting back to (0,0,0) " << endreq;
     linVertexPos.x()=0;
     linVertexPos.y()=0;
     linVertexPos.z()=0;
     vrtPos=Vertex(linVertexPos);
   }
  std::vector<Trk::VxTrackAtVertex*>& out_tracks = tracks;
  std::vector<Trk::VxTrackAtVertex*>::iterator i_end =out_tracks.end(); 
  for(std::vector<Trk::VxTrackAtVertex*>::iterator i = out_tracks.begin(); i!=i_end; ++i)
  {
    m_LinTrkFactory->linearize(**i,vrtPos);
  }//end of loop over all tracks
 }//end of relinearize method


//xAOD interfaced methods. Required to un-block the current situation 
// with the xAOD tracking design.
 xAOD::Vertex * SequentialVertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,const std::vector<const xAOD::NeutralParticle*>& vectorNeut,const Vertex& startingPoint)
 {			 
   return fit(vectorTrk, vectorNeut, RecVertex(startingPoint));				 
 }//end of the xAOD starting point fit method

    
 xAOD::Vertex * SequentialVertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, 
					    const std::vector<const xAOD::NeutralParticle*>& vectorNeut, 
					    const RecVertex& constraint) {
   if(vectorTrk.size() == 0)
   {
    msg(MSG::INFO)<<"Empty vector of tracks passed"<<endreq;
    return 0;
   }
   
//making a list of perigee out of the vector of tracks  
//Also check for duplicate tracks (will otherwise be discarded by the fit); they do not
// fit the hypothesis of statistically independent tracks. Print a warning and discard the duplicate
// it this happens.
   std::vector<const Trk::TrackParameters*> measuredPerigees;
   std::vector<const xAOD::TrackParticle*> trkToFit;
   
   for(std::vector<const xAOD::TrackParticle*>::const_iterator i = vectorTrk.begin(); i!= vectorTrk.end();++i)
   {
     //check for duplicates
     bool foundDuplicate(false);
     for (std::vector<const xAOD::TrackParticle*>::const_iterator j = vectorTrk.begin(); j!= i; ++j) {
       if (*i == *j) {
	 ATH_MSG_WARNING("Duplicate track given as input to the fitter. Ignored.");
	 foundDuplicate = true;
	 break;
       }
     }
     if (foundDuplicate) continue; //skip track

    const Trk::TrackParameters * tmpMeasPer = &((*i)->perigeeParameters());
  
    if(tmpMeasPer!=0) {
      trkToFit.push_back(*i);
      measuredPerigees.push_back(tmpMeasPer);
    } else {
      msg(MSG::INFO)<<"Failed to dynamic_cast this track parameters to perigee"<<endreq;
    }
   }

   //Same for neutrals
   std::vector<const Trk::NeutralParameters*> measuredNeutralPerigees;
   std::vector<const xAOD::NeutralParticle*> neutToFit;
   for(std::vector<const xAOD::NeutralParticle*>::const_iterator i = vectorNeut.begin(); i!= vectorNeut.end();++i)
   {
     //check for duplicates
     bool foundDuplicate(false);
     for (std::vector<const xAOD::NeutralParticle*>::const_iterator j = vectorNeut.begin(); j!= i; ++j) {
       if (*i == *j) {
	 ATH_MSG_WARNING("Duplicate neutral given as input to the fitter. Ignored.");
	 foundDuplicate = true;
	 break;
       }
     }
     if (foundDuplicate) continue; //skip track

    const Trk::NeutralParameters * tmpMeasPer = &((*i)->perigeeParameters());
  
    if(tmpMeasPer!=0) {
      neutToFit.push_back(*i);
      measuredNeutralPerigees.push_back(tmpMeasPer);
    } else {
      msg(MSG::INFO)<<"Failed to dynamic_cast this track parameters to perigee"<<endreq;
    }
   }

   
   
   Trk::VxCandidate* fittedVxCandidate = fit( measuredPerigees, measuredNeutralPerigees, constraint );

//assigning the input tracks to the fitted vertex   
   if(fittedVxCandidate !=0)
   {
    if(fittedVxCandidate->vxTrackAtVertex() != 0)
    {
     if(fittedVxCandidate->vxTrackAtVertex()->size() !=0)
     {
      for(unsigned int i = 0; i <trkToFit.size(); ++i)
      {
       
       LinkToXAODTrackParticle* linkTT= new LinkToXAODTrackParticle();
       const xAOD::TrackParticleContainer* cont = dynamic_cast< const xAOD::TrackParticleContainer* >( trkToFit[ i ]->container() );
       if(  cont )
       {
        if( ! linkTT->toIndexedElement( *cont, trkToFit[ i ]->index() ) )
	{
	 msg(MSG::WARNING)<<"Failed to set the EL for this particle correctly"<<endreq;
	}
       }else{
        msg(MSG::WARNING)<<"Failed to identify a  container for this TP"<<endreq;
       }//end of the dynamic cast check 


//       LinkToXAODTrackParticle * linkTT = new LinkToXAODTrackParticle;
//       linkTT->setElement(trkToFit[i]);

// vxtrackatvertex takes ownership!
       (*(fittedVxCandidate->vxTrackAtVertex()))[i]->setOrigTrack(linkTT);
      }//end of loop for setting orig tracks in.
      //same for neutrals
      for(unsigned int i = 0; i <neutToFit.size(); ++i)      {
	LinkToXAODNeutralParticle* linkTT= new LinkToXAODNeutralParticle();
	const xAOD::NeutralParticleContainer* cont = dynamic_cast< const xAOD::NeutralParticleContainer* >( neutToFit[ i ]->container() );
	if(  cont )       {
	  if( ! linkTT->toIndexedElement( *cont, neutToFit[ i ]->index() ) )	{
	    msg(MSG::WARNING)<<"Failed to set the EL for this particle correctly"<<endreq;
	  }
	}else{
	  msg(MSG::WARNING)<<"Failed to identify a  container for this TP"<<endreq;
	}//end of the dynamic cast check 
	// vxtrackatvertex takes ownership!
	(*(fittedVxCandidate->vxTrackAtVertex()))[trkToFit.size()+i]->setOrigTrack(linkTT);
      }//end of loop for setting orig neutrals in.
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



}//end of Trk namespace definition





/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexFitters/SequentialVertexFitter.h"
#include "TrkTrack/Track.h"
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


//xAOD includes
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include <cmath>

//ugly so far: the fitter is connected directly to the
//perigee track parametrization, which is not right.
//should be replaced later with arbitrary parametrization....

namespace Trk{

//usual tool related methods
 StatusCode SequentialVertexFitter::initialize()
 {
  
//uploading the corresponding tools

//updator 
  ATH_CHECK( m_Updator.retrieve() ); 
  
//smoother
  ATH_CHECK( m_Smoother.retrieve( EnableTool {m_doSmoothing} ) );

//Linearized Track Factory 
  ATH_CHECK( m_LinTrkFactory.retrieve() );
  return StatusCode::SUCCESS;
 }//end of initialize method


 StatusCode SequentialVertexFitter::finalize()
 {
  return StatusCode::SUCCESS;
 }

//class constructor implementation
 SequentialVertexFitter::SequentialVertexFitter(const std::string& t, const std::string& n, const IInterface*  p):
    base_class(t,n,p),
    m_Updator("Trk::KalmanVertexUpdator", this),
    
//    m_Smoother("Trk::KalmanVertexSmoother"),
    m_Smoother("Trk::DummyVertexSmoother",this),
    m_LinTrkFactory("Trk::FullPerigeeLinearizedTrackFactory",this),
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
 }

//destructor
 SequentialVertexFitter::~SequentialVertexFitter()= default;
 
   
  //conversion from the perigeeList and starting point   
  xAOD::Vertex * SequentialVertexFitter::fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
                                             const std::vector<const Trk::NeutralParameters*> & neutralPerigeeList,
                                             const Amg::Vector3D& startingPoint)  const
  {
    xAOD::Vertex constraint;
    constraint.makePrivateStore();
    constraint.setPosition( startingPoint );
    constraint.setCovariancePosition( AmgSymMatrix(3)::Zero(3,3) );
    constraint.setFitQuality( 0.,0.);
    xAOD::Vertex * FittedVertex  = fit(perigeeList, neutralPerigeeList, constraint);
   
    //setting the initial perigees  
    if(FittedVertex !=nullptr )
    {
      if(FittedVertex->vxTrackAtVertexAvailable())
      {
        if(!FittedVertex->vxTrackAtVertex().empty())
        {
          for(unsigned int i = 0; i <perigeeList.size(); ++i)
          {
            const Trk::TrackParameters* iPer = perigeeList[i];
            (FittedVertex->vxTrackAtVertex())[i].setInitialPerigee(iPer);
          }
          //same for neutrals
          for(unsigned int i = 0; i <neutralPerigeeList.size(); ++i)      {
            const Trk::NeutralParameters* iPer = neutralPerigeeList[i];
            (FittedVertex->vxTrackAtVertex())[perigeeList.size()+i].setInitialPerigee(iPer);
          }
        } //end of protection against unsuccessfull updates (no tracks or neutrals were added)
      }
    }

    return FittedVertex; 
  
  }
  
  //additional new fitting methods  
  xAOD::Vertex * SequentialVertexFitter::fit(const std::vector<const Trk::TrackParameters*>& perigeeList, 
                                             const std::vector<const Trk::NeutralParameters*> & neutralPerigeeList) const
  {    
   
    //this method will later be modifyed to use the a finder
    //uses a default starting point so  far.
    const Amg::Vector3D start_point(0.,0.,0.);
    return fit(perigeeList, neutralPerigeeList, start_point);

  }

  
  //method where the actual fit is done
  xAOD::Vertex * SequentialVertexFitter::fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
                                             const std::vector<const Trk::NeutralParameters*> & neutralPerigeeList,
                                             const xAOD::Vertex& constraint) const
  {

    //security check
    if(perigeeList.empty())
    {
      ATH_MSG_INFO( "Empty vector of tracks passed, returning 0" );
      return nullptr;
    }

    //identifying the input parameters of the fit
    //and making initial xAOD::Vertex to be updated with tracks
    const Amg::Vector3D& priorVertexPosition = constraint.position();
    const AmgSymMatrix(3)& initialVertexError = constraint.covariancePosition();
    AmgSymMatrix(3) priorErrorMatrix;

    double in_chi = 0.;
    double in_ndf = 0.;

    bool priorInfo=false;

    //checking whether the prior information has a defined error matrix:
    if(initialVertexError.trace() == 0.)
    {

      //no prior estimate, using the huge error method
      //creating a covariance matrix:
      float diag = 1000000.0;
      AmgSymMatrix(3) in_m;
      in_m.setIdentity();
      priorErrorMatrix = in_m * diag;

      //   we're  now working without prior knowledge,
      //   so the effective ndf is reduced by 3.
      in_ndf-=3.;

    } else {
      priorInfo=true;
      priorErrorMatrix = initialVertexError;
    }
    // creating an initial vertex to update
    std::unique_ptr<xAOD::Vertex> returnVertex = std::make_unique<xAOD::Vertex>();	
    returnVertex->makePrivateStore(); // xAOD::VertexContainer will take ownership of AuxStore when returnVertex is added to it
    returnVertex->setPosition( priorVertexPosition );
    returnVertex->setCovariancePosition( priorErrorMatrix );
    returnVertex->setFitQuality( in_chi, in_ndf );
    returnVertex->setVertexType( xAOD::VxType::NotSpecified ); // to mimic the initialization present in the old EDM constructor

    //converting the input perigee to the Vertex tracks
    std::vector<Trk::VxTrackAtVertex> tracks_to_fit = linearizeTracks(perigeeList, neutralPerigeeList, *returnVertex);
    std::vector<Trk::VxTrackAtVertex> fittedTracks(0);
    returnVertex->vxTrackAtVertex() = fittedTracks;

    //the actual fitting loop
    Amg::Vector3D newPosition = returnVertex->position();
    Amg::Vector3D previousPreviousPosition = newPosition;
    Amg::Vector3D previousPosition = newPosition;
    double newChi2= returnVertex->chiSquared();
    double previousChi2{};
    unsigned int n_iter = 0;
    double deltaR{};
    bool fitFailed{false};

    std::vector<Trk::VxTrackAtVertex>::iterator tracksBegin = tracks_to_fit.begin();
    std::vector<Trk::VxTrackAtVertex>::iterator tracksEnd = tracks_to_fit.end();
    auto deltaChi2 = [](double chi1, double chi0){
      return std::abs((chi1-chi0)*2./(chi1+chi0+2.));
    };
    do {
      if (!priorInfo){
        returnVertex->setPosition( newPosition );
        returnVertex->setCovariancePosition( priorErrorMatrix );
        returnVertex->setFitQuality( in_chi, in_ndf );
      } else {
        returnVertex->setPosition( priorVertexPosition );
        returnVertex->setCovariancePosition( priorErrorMatrix );
        returnVertex->setFitQuality( in_chi, in_ndf );            
      }

      //optional relinearization
      if(n_iter !=0) {
        reLinearizeTracks(tracks_to_fit, returnVertex->position());
      }
      //loop over available tracks
      int cnt = 0;

      for(std::vector<Trk::VxTrackAtVertex>::iterator i = tracksBegin; i != tracksEnd;++i)
      {                  
        xAOD::Vertex *new_vertex = m_Updator->add(*returnVertex, *i);
        if (new_vertex != returnVertex.get()) {
            returnVertex.reset( new_vertex );
        }
        ++cnt;
      }//end of loop over available tracks

      //now the updated state that is stored in returnVertex
      previousPreviousPosition = previousPosition;
      previousPosition = newPosition;
      previousChi2 = newChi2;
      newPosition = returnVertex->position();
      newChi2 = returnVertex->chiSquared();
      ++n_iter;
      //the fit always fails if a negative chisquared was returned at some point or the z-coordinate of the vertex was outside ID
      fitFailed = (n_iter == m_maxStep) or (newChi2 <0.) or std::abs(returnVertex->z())>m_maxZ;
      deltaR = (previousPosition - newPosition).perp();
      // the rest after 'fitFailed' won't be evaluated if the fit failed, 
      // so hopefully no more FPE in the division
    } while ( (not fitFailed) && 
              ( ( deltaR > m_maxShift && m_useLooseConvergence )
                || ( !m_useLooseConvergence && deltaChi2(newChi2, previousChi2) > m_maxDeltaChi2 ) ) );

    if (fitFailed) {    
      ATH_MSG_DEBUG( " Fit failed. " );
      ATH_MSG_DEBUG( " Fit didn't converge after " << n_iter );
      ATH_MSG_DEBUG( " steps. Deltachi2: " << deltaChi2(newChi2, previousChi2) ); 	         
      ATH_MSG_DEBUG( " DeltaR "  << deltaR); 
      returnVertex.reset();
      return nullptr;
    }
    //smoothing and related
    if(returnVertex !=nullptr){
      if(m_doSmoothing)m_Smoother->smooth(*returnVertex);
    } else {
      ATH_MSG_INFO( "Sequential vertex fit fails:: zero pointer returned" );
    }
    //here the vertex is returned. It is foreseen that a vertex is _always_
    //returned (initial guess in worst case) unless there is a runtime crash
    return returnVertex.release();

  }//end of the actual fit method
 
  //initial linearization of tracks------------------------------------------------------------------------------------------
  std::vector<Trk::VxTrackAtVertex> SequentialVertexFitter::linearizeTracks(const std::vector<const Trk::TrackParameters*> & perigeeList,
                                                                            const std::vector<const Trk::NeutralParameters*> & neutralPerigeeList,
                                                                            const xAOD::Vertex & vrt) const
  {

    //defining the output vector
    std::vector<Trk::VxTrackAtVertex> out_tracks(0);
    for(std::vector<const Trk::TrackParameters*>::const_iterator i = perigeeList.begin();i!= perigeeList.end();++i)
    {
  
      //creating new meas perigees, since the will be deleted 
      //by VxTrackAtVertex in destructor
      const Trk::Perigee * loc_per = dynamic_cast<const Trk::Perigee *>(*i);
      if( loc_per != nullptr)
      {
        Trk::Perigee * mPer = new Trk::Perigee(*loc_per);
        const Trk::Perigee * inPer = loc_per;
        //new MeasuredPerigee(*loc_per);
        Trk::VxTrackAtVertex * vTrack = new Trk::VxTrackAtVertex(0., mPer, nullptr, inPer, nullptr);

        //linearization itself
        m_LinTrkFactory->linearize( *vTrack, vrt.position() );
        vTrack->setWeight(1.);

        out_tracks.push_back(*vTrack);

        // Added the following line during EDM Migration
        delete vTrack; //TODO: is this ok? 
      } else {
        ATH_MSG_WARNING( "Cannot linearize tracks; treatment of neutrals not yet supported" );
      }
    }//end of loop over all the perigee states

    //same for neutrals
    for(std::vector<const Trk::NeutralParameters*>::const_iterator i = neutralPerigeeList.begin();i!= neutralPerigeeList.end();++i)
    {

      //creating new meas perigees, since the will be deleted
      //by VxTrackAtVertex in destructor
      const Trk::NeutralPerigee * loc_per = dynamic_cast<const Trk::NeutralPerigee *>(*i);
      if( loc_per != nullptr)
      {
        Trk::NeutralPerigee * mPer  = new Trk::NeutralPerigee(*loc_per);
        const Trk::NeutralPerigee * inPer = loc_per;
        Trk::VxTrackAtVertex * vTrack = new Trk::VxTrackAtVertex(0., nullptr, mPer, nullptr, inPer);

        //linearization itself
        m_LinTrkFactory->linearize( *vTrack, vrt.position() );
        vTrack->setWeight(1.);

        out_tracks.push_back(*vTrack);

        // Added the following line during EDM Migration
        delete vTrack; //TODO: is this ok?
      } else {
        ATH_MSG_WARNING( "Cannot linearize tracks; treatment of neutrals not yet supported" );
      }
    }//end of loop over all the neutral perigee states

    return out_tracks;
  }//end of linearize method

 //relinearization of tracks during iterations------------------------------------------------------
 void SequentialVertexFitter::reLinearizeTracks(std::vector<Trk::VxTrackAtVertex>& tracks, const Amg::Vector3D & vrt) const
 {
   Amg::Vector3D linVertexPos = vrt;
   if ( linVertexPos.perp() > m_maxR || std::abs(linVertexPos.z()) > m_maxZ )
   {
     ATH_MSG_DEBUG( " Linearization position outside ID. Setting back to (0,0,0) " );
     linVertexPos.x()=0;
     linVertexPos.y()=0;
     linVertexPos.z()=0;
   }

   std::vector<Trk::VxTrackAtVertex>& out_tracks = tracks;
   std::vector<Trk::VxTrackAtVertex>::iterator i_end = out_tracks.end();
   for(std::vector<Trk::VxTrackAtVertex>::iterator i = out_tracks.begin(); i!=i_end; ++i)
   {
     m_LinTrkFactory->linearize(*i,linVertexPos);
   }//end of loop over all tracks
 }//end of relinearize method


 //xAOD interfaced methods. Required to un-block the current situation 
 // with the xAOD tracking design.
 xAOD::Vertex * SequentialVertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,const std::vector<const xAOD::NeutralParticle*>& vectorNeut,const Amg::Vector3D& startingPoint) const
 {
   xAOD::Vertex constraint;
   constraint.makePrivateStore();
   constraint.setPosition( startingPoint );
   constraint.setCovariancePosition( AmgSymMatrix(3)::Zero(3,3) );
   constraint.setFitQuality( 0.,0.);			 
   return fit(vectorTrk, vectorNeut, constraint);				 
 }//end of the xAOD starting point fit method

    
 xAOD::Vertex * SequentialVertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, const std::vector<const xAOD::NeutralParticle*>& vectorNeut, const xAOD::Vertex& constraint) const
 {

   if(vectorTrk.empty())
   {
     ATH_MSG_INFO( "Empty vector of tracks passed" );
     return nullptr;
   }

   if(vectorNeut.empty())
   {
     ATH_MSG_INFO( "Empty vector of neutrals passed" );
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
	 ATH_MSG_WARNING( "Duplicate track given as input to the fitter. Ignored." );
	 foundDuplicate = true;
	 break;
       }
     }
     if (foundDuplicate) continue; //skip track

     const Trk::TrackParameters * tmpMeasPer = &((*i)->perigeeParameters());
  
     if(tmpMeasPer!=nullptr) {
       trkToFit.push_back(*i);
       measuredPerigees.push_back(tmpMeasPer);
     } else {
       ATH_MSG_INFO( "Failed to dynamic_cast this track parameters to perigee" ); //TODO: Failed to implicit cast the perigee parameters to track parameters?
     }
   }

   //making a list of perigee out of the vector of neutrals
   std::vector<const Trk::NeutralParameters*> measuredNeutralPerigees;
   std::vector<const xAOD::NeutralParticle*> neutToFit;
   for(std::vector<const xAOD::NeutralParticle*>::const_iterator i = vectorNeut.begin(); i!= vectorNeut.end();++i)
   {
     //check for duplicates
     bool foundDuplicate(false);
     for (std::vector<const xAOD::NeutralParticle*>::const_iterator j = vectorNeut.begin(); j!= i; ++j) {
       if (*i == *j) {
	 ATH_MSG_WARNING( "Duplicate neutral given as input to the fitter. Ignored." );
	 foundDuplicate = true;
	 break;
       }
     }
     if (foundDuplicate) continue; //skip track

     const Trk::NeutralParameters * tmpMeasPer = &((*i)->perigeeParameters());
  
     if(tmpMeasPer!=nullptr) {
       neutToFit.push_back(*i);
       measuredNeutralPerigees.push_back(tmpMeasPer);
     } else {
       ATH_MSG_INFO( "Failed to dynamic_cast this neutral parameters to perigee" ); //TODO: Failed to implicit cast the perigee parameters to neutral parameters?
     }
   }


   xAOD::Vertex* fittedVertex = fit( measuredPerigees, measuredNeutralPerigees, constraint );


   //assigning the input tracks to the fitted vertex through vxTrackAtVertices
   if(fittedVertex ==nullptr)
   {
     return fittedVertex;
   }

     if( fittedVertex->vxTrackAtVertexAvailable() ) // TODO: I don't think vxTrackAtVertexAvailable() does the same thing as a null pointer check!
     {
       if(!fittedVertex->vxTrackAtVertex().empty())
       {
         for(unsigned int i = 0; i <trkToFit.size(); ++i)
         {
           
           LinkToXAODTrackParticle* linkTT= new LinkToXAODTrackParticle();
           const xAOD::TrackParticleContainer* cont = dynamic_cast< const xAOD::TrackParticleContainer* >( trkToFit[ i ]->container() );
           if(  cont )
           {
             if( ! linkTT->toIndexedElement( *cont, trkToFit[ i ]->index() ) )
      	     {
	       ATH_MSG_WARNING( "Failed to set the EL for this particle correctly" );
	     }
           } else{
             ATH_MSG_WARNING( "Failed to identify a  container for this TP" );
           }//end of the dynamic cast check 


           // vxtrackatvertex takes ownership!
           ( fittedVertex->vxTrackAtVertex() )[i].setOrigTrack(linkTT);
         }//end of loop for setting orig tracks in.

         for(unsigned int i = 0; i <neutToFit.size(); ++i)
         {
           LinkToXAODNeutralParticle* linkTT = new LinkToXAODNeutralParticle();
           const xAOD::NeutralParticleContainer* cont = dynamic_cast< const xAOD::NeutralParticleContainer* >( neutToFit[ i ]->container() );
           if(  cont )
           {
             if( ! linkTT->toIndexedElement( *cont, neutToFit[ i ]->index() ) )
             {
               ATH_MSG_WARNING( "Failed to set the EL for this particle correctly" );
             }
           } else{
             ATH_MSG_WARNING( "Failed to identify a  container for this NP" );
           }//end of the dynamic cast check 

           // vxtrackatvertex takes ownership!
           ( fittedVertex->vxTrackAtVertex() )[trkToFit.size()+i].setOrigTrack(linkTT);
         }//end of loop for setting orig neutrals in.

       }//end of protection against unsuccessfull updates (no tracks were added)
     }//end of vector of tracks check


   //now set links to xAOD::TrackParticles directly in the xAOD::Vertex
   unsigned int VTAVsize = (fittedVertex && fittedVertex->vxTrackAtVertexAvailable()) ? fittedVertex->vxTrackAtVertex().size() : 0 ;
   for (unsigned int i = 0 ; i < VTAVsize ; ++i)
   {
     Trk::VxTrackAtVertex* VTAV = &( fittedVertex->vxTrackAtVertex().at(i) );
     //TODO: Will this pointer really hold 0 if no VxTrackAtVertex is found?
     if (not VTAV){
       ATH_MSG_WARNING( "Trying to set link to xAOD::TrackParticle. The VxTrackAtVertex is not found" );
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
         ATH_MSG_WARNING( "Skipping track. Trying to set link to something else than xAOD::TrackParticle or xAOD::NeutralParticle." );
       } else {
         //Now set the new link to the new xAOD vertex
         fittedVertex->addNeutralAtVertex(*linkToXAODTPneutral, VTAV->weight());
       }

     }
   } //end of loop

   return fittedVertex;

 }//end of the xAOD constrained fit method


}//end of Trk namespace definition

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAlignGenTools/BeamspotVertexPreProcessor.h"
#include "TrkFitterInterfaces/IGlobalTrackFitter.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "TrkAlignEvent/AlignTrack.h"
#include "TrkAlignEvent/AlignVertex.h"
#include "TrkVertexOnTrack/VertexOnTrack.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/SmartDataPtr.h"

// new xAOD, seems we need to keep old as well
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"

//++ new one

#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkAlignInterfaces/IAlignModuleTool.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include <ext/algorithm>
#include <functional>


namespace Trk {

//________________________________________________________________________
BeamspotVertexPreProcessor::BeamspotVertexPreProcessor(const std::string & type,
                                                       const std::string & name,
                                                       const IInterface  * parent)
  : AthAlgTool(type,name,parent)
  , m_trackFitter("Trk::GlobalChi2Fitter/InDetTrackFitter")
  , m_SLTrackFitter("")
  , m_extrapolator("Trk::Extrapolator/AtlasExtrapolator")
  , m_trkSelector("")
  , m_BSTrackSelector("")
  , m_alignModuleTool("Trk::AlignModuleTool/AlignModuleTool")
  , m_PVContainerName("PrimaryVertices")
  , m_runOutlierRemoval(false)
  , m_selectVertices(true)
  , m_particleNumber(3)                          // 3=pion, 0=non-interacting
  , m_doTrkSelection (true)
  , m_doBSTrackSelection(false)
  , m_doAssociatedToPVSelection(true)
  , m_constraintMode(0)
  , m_compareMethod("compareAddress")
  , m_doBeamspotConstraint(true)
  , m_doPrimaryVertexConstraint(false)
  , m_doFullVertexConstraint(false)
  , m_doNormalRefit(true)
  , m_maxPt(0.)
  , m_refitTracks(true)
  , m_storeFitMatrices(true)
  , m_useSingleFitter(false)
  , m_BSScalingFactor(1.)
  , m_PVScalingFactor(1.)
  , m_minTrksInVtx(3)
  , m_nTracks(0)
  , m_trackTypeCounter(AlignTrack::NTrackTypes,0)
  , m_nFailedNormalRefits(0)
  , m_nFailedBSRefits(0)
  , m_nFailedPVRefits(0)
{
  declareInterface<IAlignTrackPreProcessor>(this);
  declareProperty("RefitTracks",               m_refitTracks      );
  declareProperty("PVContainerName",           m_PVContainerName    );
  declareProperty("TrackFitter",               m_trackFitter      );
  declareProperty("SLTrackFitter",             m_SLTrackFitter      );
  declareProperty("UseSingleFitter",           m_useSingleFitter    );
  declareProperty("Extrapolator",              m_extrapolator             );
  declareProperty("RunOutlierRemoval",         m_runOutlierRemoval        );
  declareProperty("AlignModuleTool",           m_alignModuleTool          );
  declareProperty("ParticleNumber",            m_particleNumber           );
  declareProperty("TrackSelector",             m_trkSelector              );
  declareProperty("DoTrackSelection",          m_doTrkSelection           );
  declareProperty("BSConstraintTrackSelector", m_BSTrackSelector          );
  declareProperty("DoBSTrackSelection",        m_doBSTrackSelection       );
  declareProperty("DoAssociatedToPVSelection", m_doAssociatedToPVSelection);
  declareProperty("DoBSConstraint",            m_doBeamspotConstraint     ,"Constrain tracks to the beamspot (x,y) position");
  declareProperty("DoPVConstraint",            m_doPrimaryVertexConstraint,"Constrain tracks to the associated primary vertex (x,y,z) position");
  declareProperty("DoFullVertex",              m_doFullVertexConstraint   ,"Full 3D vertex constraint.  Note DoPVConstraint needs to be set to true to use this option. If DoBSConstraint vertex position will be constrained to the BS" );
  declareProperty("ConstraintMode",            m_constraintMode           );
  declareProperty("StoreFitMatrices",          m_storeFitMatrices         );
  declareProperty("BeamspotScalingFactor",     m_BSScalingFactor          );
  declareProperty("PrimaryVertexScalingFactor",m_PVScalingFactor          );
  declareProperty("MinTrksInVtx",              m_minTrksInVtx             );
  declareProperty("doNormalRefit"             ,m_doNormalRefit            );
  declareProperty("maxPt"                     ,m_maxPt            );


  std::vector<std::string> defaultInterestedVertexContainers;
  defaultInterestedVertexContainers.emplace_back("PrimaryVertices");       // MD: Maybe only the first container?
  //defaultInterestedVertexContainers.push_back("V0UnconstrVertices");  //   : does not seem to exist in files -> check later again
  m_interestedVertexContainers = defaultInterestedVertexContainers;

  m_logStream = nullptr;
}

//________________________________________________________________________
BeamspotVertexPreProcessor::~BeamspotVertexPreProcessor()
{
}

//________________________________________________________________________
StatusCode BeamspotVertexPreProcessor::initialize()
{
  ATH_MSG_INFO("BeamspotVertexPreProcessor::initialize()");

  // configure main track selector if requested
  if (!m_trkSelector.empty()) {
    if (m_trkSelector.retrieve().isFailure())
      msg(MSG::ERROR)<<"Failed to retrieve tool "<<m_trkSelector<<". No Track Selection will be done."<<endmsg;
    else
      ATH_MSG_INFO("Retrieved " << m_trkSelector);
  }

  if (m_refitTracks) {
    // configure main track fitter
    if(m_trackFitter.retrieve().isFailure()) {
       msg(MSG::FATAL) << "Could not get " << m_trackFitter << endmsg;
       return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Retrieved " << m_trackFitter);

    // configure straight-line track fitter if requested
    if (!m_useSingleFitter) {
      if (m_SLTrackFitter.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not get " << m_SLTrackFitter << endmsg;
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Retrieved " << m_SLTrackFitter);
    }

     // TrackToVertexIPEstimator
    if (m_ITrackToVertexIPEstimator.retrieve().isFailure()) {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Can not retrieve TrackToVertexIPEstimator of type " << m_ITrackToVertexIPEstimator.typeAndName() << endmsg;
        return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO ( "Retrieved TrackToVertexIPEstimator Tool " << m_ITrackToVertexIPEstimator.typeAndName() );
    }

    // configure Atlas extrapolator
    if (m_extrapolator.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve tool "<<m_extrapolator<<endmsg;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Retrieved " << m_extrapolator);

    // configure beam-spot conditions service
    ATH_CHECK(m_beamSpotKey.initialize());


    // configure beam-spot track selector if requested
    if(m_doBSTrackSelection) {
      if(m_BSTrackSelector.empty()) {
        msg(MSG::FATAL) << "Requested BeamSpot track selection but Track Selector not configured"<< endmsg;
        return StatusCode::FAILURE;
      }
      if (m_BSTrackSelector.retrieve().isFailure()) {
         msg(MSG::FATAL) << "Could not get " << m_BSTrackSelector<< endmsg;
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Retrieved " << m_BSTrackSelector);
    }

  }  // end of 'if (m_refitTracks)'

  else if (m_doBeamspotConstraint) {
    msg(MSG::FATAL)<<"Requested beam-spot constraint but RefitTracks is False."<<endmsg;
    return StatusCode::FAILURE;
  }

  if( m_doFullVertexConstraint && m_doPrimaryVertexConstraint ) {
    if ( m_alignModuleTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_alignModuleTool);
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_INFO("Retrieved tool " << m_alignModuleTool);

      ATH_MSG_INFO("************************************************************************");
      ATH_MSG_INFO("*                                                                      *");
      ATH_MSG_INFO("* You have requested the Full Vertex Constraint option.                *");
      ATH_MSG_INFO("* It is your duty to assure that all detector elements                 *");
      ATH_MSG_INFO("* used for track fitting are also loaded in the alignment framework!!! *");
      ATH_MSG_INFO("*                                                                      *");
      ATH_MSG_INFO("* Also make sure the accurate track covariance matrix                  *");
      ATH_MSG_INFO("* is returned by the GlobalChi2Fitter!                                 *");
      ATH_MSG_INFO("*                                                                      *");
      ATH_MSG_INFO("************************************************************************");
    }

  }
  return StatusCode::SUCCESS;
}


bool CompareTwoTracks::operator()(VxTrackAtVertex vtxTrk){ // MD: took away deref*

  ITrackLink* trkLink = vtxTrk.trackOrParticleLink();
  LinkToTrackParticleBase* linkToTrackParticle = dynamic_cast<Trk::LinkToTrackParticleBase*>(trkLink);
  if(!linkToTrackParticle) return false;
    const TrackParticleBase* tpb = *(linkToTrackParticle->cptr());

  const Track* originalTrk = tpb->originalTrack();

  bool equal = false;
  // compare the addresses of these two tracks directly
  if(m_method.find("compareAddress") != std::string::npos){
     if (m_track == originalTrk) equal = true;
     //std::cout << " comparing two Tracks' addresses directly, the address of the comparing track : "<< m_track <<" the address of the compared track : "<< originalTrk << " compare result : " << equal << std::endl;
  }

  // compare the perigee parameters of these two tracks, should safer
  if(m_method.find("comparePerigee") != std::string::npos){
    const Trk::Perigee * measPer1 = m_track->perigeeParameters();
    const Trk::Perigee * measPer2 = originalTrk->perigeeParameters();
    if(! (measPer1 && measPer2 )) equal = false;
    else{
      float diff = fabs(std::numeric_limits<float>::epsilon());
      if( ! (( fabs(measPer1->parameters()[Trk::d0]     - measPer2->parameters()[Trk::d0])     <= diff)
         && ( fabs(measPer1->parameters()[Trk::z0]     - measPer2->parameters()[Trk::z0])     <= diff)
         && ( fabs(measPer1->parameters()[Trk::phi]    - measPer2->parameters()[Trk::phi])    <= diff)
         && ( fabs(measPer1->parameters()[Trk::theta]  - measPer2->parameters()[Trk::theta])  <= diff)
         && ( fabs(measPer1->parameters()[Trk::qOverP] - measPer2->parameters()[Trk::qOverP]) <= diff)))
              equal = false;
    }
     //std::cout << " comparing two Tracks' perigee parameter, the perigee of the comparing track is: "<< *measPer1 <<" the perigee of the compared track is: "<< *measPer2 << " compare result is: " << equal << std::endl;
  }
  return equal;
}



bool BeamspotVertexPreProcessor::selectVertices(const xAOD::Vertex * vtx) const {

    if(0 == vtx->vertexType()) {
      ATH_MSG_DEBUG("this primary vertex has been rejected as type dummy");
      return false;
    }
    if (vtx->numberDoF() <= 0){
      ATH_MSG_WARNING(" VERY STRANGE!!!, this primary vertex has been rejected as non-positive DoF "<< vtx->numberDoF() <<" the type of this vertex: "<<  vtx->vertexType() );
      return false;
    }
    if (int(vtx->vxTrackAtVertex().size()) < m_minTrksInVtx){
      ATH_MSG_DEBUG(" this primary vertex vxTrackAtVertex size:  "<< vtx->vxTrackAtVertex().size() );
      return false;
    }
    return true;
}


bool BeamspotVertexPreProcessor::selectUpdatedVertices(const xAOD::Vertex * vtx) const {

    if (vtx->numberDoF() <= 0){
      ATH_MSG_WARNING(" VERY STRANGE!!! , the updated vertex has been rejected as non-positive DoF: "<< vtx->numberDoF() <<" the type of this vertex:"<<  vtx->vertexType() );
      return false;
    }

    if (int(vtx->vxTrackAtVertex().size()) < m_minTrksInVtx){
      ATH_MSG_DEBUG(" the updated vertex has been rejected as vxTrackAtVertex size:  "<< vtx->vxTrackAtVertex().size() );
      return false;
    }

    if ((vtx->covariancePosition())(0,0)<=0  ||
        (vtx->covariancePosition())(1,1)<=0  ||
        (vtx->covariancePosition())(2,2)<=0){
      ATH_MSG_WARNING(" VERY STRANGE!!! , this updated vertex has been rejected as negative diagonal error matrix ");
      return false;
    }
    return true;
}


bool BeamspotVertexPreProcessor::isAssociatedToPV(const Trk::Track * track, const xAOD::VertexContainer* vertices)
{
  if(!vertices)
      return false;

  xAOD::VertexContainer::const_iterator vtxEnd   = vertices->end();
  xAOD::VertexContainer::const_iterator vtxIter  = vertices->begin();

  for ( ; vtxIter != vtxEnd  && (*vtxIter)->vertexType() == 1; ++vtxIter ){
        if (isAssociatedToVertex(track, *vtxIter)) return true;
  }
  return false;
}


//____________________________________________________________________________
bool BeamspotVertexPreProcessor::isAssociatedToVertex(const Trk::Track * track, const xAOD::Vertex * vertex)
{
  if(!vertex)
    return false;

  std::vector<VxTrackAtVertex >  vertexTracks = vertex->vxTrackAtVertex();
  Trk::CompareTwoTracks thisCompare(track, m_compareMethod);

  std::vector<VxTrackAtVertex >::const_iterator iVxTrackBegin = vertexTracks.begin();
  std::vector<VxTrackAtVertex >::const_iterator iVxTrackEnd   = vertexTracks.end();

  std::vector<VxTrackAtVertex>::const_iterator findResult = std::find_if(iVxTrackBegin, iVxTrackEnd, thisCompare);

  return findResult != iVxTrackEnd;
}


void BeamspotVertexPreProcessor::prepareAllTracksVector(){

  // do clean up firstly
  m_allTracksVector.clear();
  const xAOD::VertexContainer* thisContainer = nullptr;
  //xAODVertices
  std::vector<std::string>::const_iterator strs_iter = m_interestedVertexContainers.begin();
  std::vector<std::string>::const_iterator strs_end  = m_interestedVertexContainers.end();

  for(; strs_iter != strs_end; ++strs_iter){

    if (evtStore()->contains<xAOD::VertexContainer>(*strs_iter)) {
      if (evtStore()->retrieve(thisContainer,*strs_iter).isFailure() ) {
        ATH_MSG_DEBUG ("Could not retrieve xAOD vertex container with key "+(*strs_iter));
        continue;
      }
      else {

        xAOD::VertexContainer::const_iterator vtxEnd   = thisContainer->end();
        xAOD::VertexContainer::const_iterator vtxIter  = thisContainer->begin();

        for(; vtxIter != vtxEnd; ++vtxIter){
          if(m_selectVertices && !selectVertices(*vtxIter)) {
            ATH_MSG_DEBUG("this vertex did not pass the primary vertex selection...");
            continue;
          }
          // MD: extra check to make sure - maybe not needed?
          if ((*vtxIter)->vxTrackAtVertexAvailable()){

            std::vector<VxTrackAtVertex> vtxTracks = (*vtxIter)->vxTrackAtVertex();
            m_allTracksVector.emplace_back(*vtxIter,vtxTracks);
          }
          else {
            ATH_MSG_DEBUG("this vertex did not pass the vxTrackAtVertexAvailable() call...");
            continue;
          }
        }
      }
    }
  }
  ATH_MSG_DEBUG("m_allTracksVector size: "<<m_allTracksVector.size());
}


const xAOD::Vertex* BeamspotVertexPreProcessor::findVertexCandidate(const Track* track) const {

  const xAOD::Vertex* findVxCandidate = nullptr;
  //VxTrackAtVertex* findVxTrack = 0;

  std::vector< std::pair< const xAOD::Vertex*, std::vector<VxTrackAtVertex> > >::const_iterator iter    = m_allTracksVector.begin();
  std::vector< std::pair< const xAOD::Vertex*, std::vector<VxTrackAtVertex> > >::const_iterator iterEnd = m_allTracksVector.end();

  for(; iter != iterEnd; ++iter){
    std::pair< const xAOD::Vertex*, std::vector<VxTrackAtVertex> > thisPair = *iter;
    //ATH_MSG_DEBUG(" this VxCandidate* and vector<VxTrackAtVertex*>* Pair: "<< *(thisPair.first));

    std::vector<VxTrackAtVertex>::iterator iVxTrackBegin  = (thisPair.second).begin();
    std::vector<VxTrackAtVertex>::iterator iVxTrackEnd    = (thisPair.second).end();
    Trk::CompareTwoTracks thisCompare(track, m_compareMethod);

    std::vector<VxTrackAtVertex>::iterator findResult = std::find_if(iVxTrackBegin, iVxTrackEnd, thisCompare);

    if(findResult != iVxTrackEnd){
      ATH_MSG_DEBUG("the found VxTrackAtVertex: "<<*findResult);
      findVxCandidate      = thisPair.first;
      //findVxTrack          = findResult;
      break;
    }
  }

  //if  ( !(findVxCandidate && findVxTrack) )
  //  ATH_MSG_DEBUG("the track don't not belongs to any interested Vertex! ");

  return findVxCandidate;
}


const VertexOnTrack* BeamspotVertexPreProcessor::provideVotFromVertex(const Track* track, const xAOD::Vertex* &vtx) const {

  const EventContext& ctx = Gaudi::Hive::currentContext();
  const VertexOnTrack * vot       = nullptr;
  const xAOD::Vertex* tmpVtx      = nullptr;
  const xAOD::Vertex* updatedVtx  = nullptr;

  const xAOD::Vertex* findVtx = findVertexCandidate(track);

  ATH_MSG_DEBUG("findVtx in provideVotFromVertex: "<<findVtx);

  if (!( nullptr==findVtx) ) {
    vtx    = findVtx;

    if( m_doFullVertexConstraint ) {
      updatedVtx = new xAOD::Vertex(*vtx);
      //updatedVtx = vtx->clone();   // no clone option for xAODvertex
    } else {
      tmpVtx = new xAOD::Vertex(*vtx);
      //tmpVtx = vtx->clone();  // no clone option for xAODvertex
      updatedVtx = m_ITrackToVertexIPEstimator->getUnbiasedVertex(track->perigeeParameters(), vtx ); // MD: new function call
    }


    if(updatedVtx){

      if(!selectUpdatedVertices(updatedVtx))
        return vot;

      if( !m_doFullVertexConstraint )
        ATH_MSG_DEBUG(" updated Vertex by KalmanVertexUpdator: "<<updatedVtx);


      ///vertex as perigeeSurface
      Amg::Vector3D  globPos(updatedVtx->position()); //look
      const PerigeeSurface* surface = new PerigeeSurface(globPos);


      const Perigee *  perigee = dynamic_cast<const Perigee*>(m_extrapolator->extrapolate(ctx,*track, *surface));
      if (!perigee) {
        const Perigee * trackPerigee = track->perigeeParameters();
        if ( trackPerigee && trackPerigee->associatedSurface() == *surface )
          perigee = trackPerigee->clone();
      }
      //if the perigee is still nonsense ...
      if (not perigee){
        //clean up
        if (updatedVtx!= tmpVtx) delete updatedVtx;
        delete tmpVtx;
        delete surface;
        //WARNING
        ATH_MSG_WARNING("Perigee is nullptr in "<<__FILE__<<":"<<__LINE__);
        //exit
        return vot;
      }

      // create the Jacobian matrix from Cartisian to Perigee
      AmgMatrix(2,3) Jacobian;
      Jacobian.setZero();
      //perigee is dereferenced here, must not be nullptr!
      double ptInv                               =  1./perigee->momentum().perp();
      Jacobian(0,0)                              = -ptInv*perigee->momentum().y();
      Jacobian(0,1)                              =  ptInv*perigee->momentum().x();
      Jacobian(1,2)                              =  1.0;

      ATH_MSG_DEBUG(" Jacobian matrix from Cartesian to Perigee: "<< Jacobian);

      AmgSymMatrix(3) vtxCov = updatedVtx->covariancePosition(); // MD: that was NULL before?
      //std::cout  << " before PV scaling : "<< vtxCov << std::endl;

      vtxCov *= m_PVScalingFactor * m_PVScalingFactor;
      //std::cout  << " after PV scaling : "<< vtxCov << std::endl;

      Amg::MatrixX errorMatrix;
      if( m_doFullVertexConstraint ) {
        AmgSymMatrix(3)  tmpCov;
        tmpCov.setZero();
        tmpCov(0,0) = 1.e-10 ;
        tmpCov(1,1) = 1.e-10;
        tmpCov(2,2) = 1.e-10;
        errorMatrix = Amg::MatrixX( tmpCov.similarity(Jacobian) );
      } else {
        errorMatrix = Amg::MatrixX( vtxCov.similarity(Jacobian) );
      }
      delete perigee;

      // in fact, in most of the normal situation, pointer tmpVtx and updatedVtx are the same. You can check the source code
      // But for safety, I would like to delete them seperately
      // sroe(2016.09.23): This would result in an illegal double delete, if they really point to the same thing!
      // http://stackoverflow.com/questions/9169774/what-happens-in-a-double-delete
      if (tmpVtx != updatedVtx){
        delete updatedVtx;
      }
      delete tmpVtx;
      tmpVtx=nullptr;
      updatedVtx=nullptr;

      LocalParameters localParams = Trk::LocalParameters(Amg::Vector2D(0,0));

      // VertexOnTrack Object
      vot = new VertexOnTrack(localParams, errorMatrix, *surface);
      ATH_MSG_DEBUG("the VertexOnTrack created from vertex: "<<*vot);
      // garbage collection:
      delete surface;
    }
  }

  return vot;

}


const VertexOnTrack* BeamspotVertexPreProcessor::provideVotFromBeamspot(const Track* track) const{

  const EventContext& ctx = Gaudi::Hive::currentContext();
  const VertexOnTrack * vot = nullptr;
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx }; 
  Amg::Vector3D  bpos = beamSpotHandle->beamPos();
  ATH_MSG_DEBUG("beam spot: "<<bpos);
  float beamSpotX = bpos.x();
  float beamSpotY = bpos.y();
  float beamSpotZ = bpos.z();
  float beamTiltX = beamSpotHandle->beamTilt(0);
  float beamTiltY = beamSpotHandle->beamTilt(1);
  float beamSigmaX = m_BSScalingFactor * beamSpotHandle->beamSigma(0);
  float beamSigmaY = m_BSScalingFactor * beamSpotHandle->beamSigma(1);
  //float beamSigmaZ = m_scalingFactor * beamSpotHandle->beamSigma(2);

  ATH_MSG_DEBUG("running refit with beam-spot");


  float z0 = track->perigeeParameters()->parameters()[Trk::z0];
  float beamX = beamSpotX + tan(beamTiltX) * (z0-beamSpotZ);
  float beamY = beamSpotY + tan(beamTiltY) * (z0-beamSpotZ);
  Amg::Vector3D  BSC(beamX, beamY, z0);
  ATH_MSG_DEBUG("constructing beam point (x,y,z) = ( "<<beamX<<" , "<<beamY<<" , "<<z0<<" )");

  const PerigeeSurface * surface = nullptr;
  Amg::MatrixX  errorMatrix;
  LocalParameters beamSpotParameters;

  // covariance matrix of the beam-spot
  AmgSymMatrix(2) beamSpotCov;
  beamSpotCov.setZero();
  beamSpotCov(0,0) = beamSigmaX * beamSigmaX;
  beamSpotCov(1,1) = beamSigmaY * beamSigmaY;

  if(m_constraintMode == 0) {

    const Amg::Vector3D&  globPos(BSC);
    surface = new PerigeeSurface(globPos);

    // create a measurement for the beamspot
    DefinedParameter Par0(0.,Trk::d0);
    beamSpotParameters = LocalParameters(Par0);

    // calculate perigee parameters wrt. beam-spot
    const Perigee * perigee = dynamic_cast<const Perigee*>(m_extrapolator->extrapolate(ctx, *track, *surface));
    if (!perigee) {
      const Perigee * trackPerigee = track->perigeeParameters();
      if ( trackPerigee && trackPerigee->associatedSurface() == *surface )
        perigee = trackPerigee->clone();
    }
    if (not perigee){
      //clean up
      delete surface;
      //WARNING
      ATH_MSG_WARNING("Perigee is nullptr in "<<__FILE__<<":"<<__LINE__);
      //exit
      return vot;
    }

    Eigen::Matrix<double,1,2> jacobian;
    jacobian.setZero();
    //perigee is dereferenced here, must not be nullptr
    double ptInv   =  1./perigee->momentum().perp();
    jacobian(0,0) = -ptInv * perigee->momentum().y();
    jacobian(0,1) =  ptInv * perigee->momentum().x();

    // MD: changed -> reversed order of matrix multiplication
    errorMatrix = Amg::MatrixX( jacobian*(beamSpotCov*jacobian.transpose()));
    if( errorMatrix.cols() != 1  )
         ATH_MSG_FATAL("Similarity transpose done incorrectly");
    delete perigee;
  }
  if (surface){
    vot = new VertexOnTrack(beamSpotParameters, errorMatrix, *surface);
  } else {
    ATH_MSG_WARNING("surface is nullptr in "<<__FILE__<<":"<<__LINE__);
  }
  if (vot){
    ATH_MSG_DEBUG(" the VertexOnTrack objects created from BeamSpot are " << *vot);
  }
  // garbage collection:
  delete surface;

  return vot;
}


void BeamspotVertexPreProcessor::provideVtxBeamspot(const AlignVertex* b, AmgSymMatrix(3)* q, Amg::Vector3D* v) const {

  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
  Amg::Vector3D bpos = beamSpotHandle->beamPos();
  ATH_MSG_DEBUG("beam spot: "<<bpos);
  float beamSpotX = bpos.x();
  float beamSpotY = bpos.y();
  float beamSpotZ = bpos.z();
  float beamTiltX = beamSpotHandle->beamTilt(0);
  float beamTiltY = beamSpotHandle->beamTilt(1);
  float beamSigmaX = m_BSScalingFactor * beamSpotHandle->beamSigma(0);
  float beamSigmaY = m_BSScalingFactor * beamSpotHandle->beamSigma(1);
  float beamSigmaZ = m_BSScalingFactor * beamSpotHandle->beamSigma(2);


  float z0 = b->originalPosition()->z();
  (*v)(0) = beamSpotX + tan(beamTiltX) * (z0-beamSpotZ);
  (*v)(1) = beamSpotY + tan(beamTiltY) * (z0-beamSpotZ);
  (*v)(2) = beamSpotZ;
  (*q)(0,0) = beamSigmaX*beamSigmaX;
  (*q)(1,1) = beamSigmaY*beamSigmaY;
  (*q)(2,2) = beamSigmaZ*beamSigmaZ;

  ATH_MSG_DEBUG("VTX constraint point (x,y,z) = ( "<< (*v)[0] <<" , "<< (*v)[1] <<" , "<< (*v)[2] <<" )");
  ATH_MSG_DEBUG("VTX constraint size  (x,y,z) = ( "<< beamSigmaX <<" , "<< beamSigmaY <<" , "<< beamSigmaZ <<" )");
}

const Track*
BeamspotVertexPreProcessor::doConstraintRefit(
  ToolHandle<Trk::IGlobalTrackFitter>& fitter,
  const Track* track,
  const VertexOnTrack* vot,
  const ParticleHypothesis& particleHypothesis) const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  const Track* newTrack = nullptr;

  if(vot){

    std::vector<const MeasurementBase *> measurementCollection;
    measurementCollection.push_back(vot);
    // add all other measurements
    DataVector<const MeasurementBase>::const_iterator imeas     = track->measurementsOnTrack()->begin();
    DataVector<const MeasurementBase>::const_iterator imeas_end = track->measurementsOnTrack()->end();
    for ( ; imeas != imeas_end ; ++imeas)  measurementCollection.push_back(*imeas);

    if( m_doFullVertexConstraint ) {
      // get track parameters at the vertex:
      const PerigeeSurface&         surface=vot->associatedSurface();
      ATH_MSG_DEBUG(" Track reference surface will be:  " << surface);
      const TrackParameters* parsATvertex=m_extrapolator->extrapolate(ctx, *track, surface);

      ATH_MSG_DEBUG(" Track will be refitted at this surface  ");
      newTrack = (fitter->fit(ctx, measurementCollection, 
                             *parsATvertex, m_runOutlierRemoval, particleHypothesis)).release();
      delete parsATvertex;
    } else {
      newTrack = (fitter->fit(ctx,
                             measurementCollection, *(track->trackParameters()->front()), 
                             m_runOutlierRemoval, particleHypothesis)).release();
    }
     //     delete vot;
  }

  return newTrack;
}

bool BeamspotVertexPreProcessor::doBeamspotConstraintTrackSelection(const Track* track) {

  const xAOD::VertexContainer* vertices = nullptr;
  const xAOD::Vertex* vertex = nullptr;
  bool haveVertex = false;

  // retrieve the primary vertex if needed
  if(m_doAssociatedToPVSelection) {

    if(evtStore()->retrieve( vertices, m_PVContainerName ).isFailure()) {
      msg(MSG::ERROR)<<"Cannot retrieve the \'"<<m_PVContainerName<<"\' vertex collection from StoreGate"<<endmsg;
      m_doAssociatedToPVSelection = false;
    } else {
      // if there is no vertex, we can't associate the tracks to it
      if(vertices) {
        ATH_MSG_DEBUG("Primary vertex collection for this event has "<<vertices->size()<<" vertices");
        if (vertices->size()<2){
          ATH_MSG_DEBUG("Only Dummy vertex present, no Primary vertices.");
        } else {
          vertex = (*vertices)[0];
          haveVertex = true;
        }
      }
      else
        ATH_MSG_DEBUG("Could not retrieve primary vertex collection from the StoreGate");
    }
  }


  if( ( m_doAssociatedToPVSelection && haveVertex && vertex && isAssociatedToPV(track,vertices) ) ||
      ( m_doBSTrackSelection        && m_BSTrackSelector->accept(*track) ) ){

    if (m_maxPt > 0 )
      {
	const Trk::Perigee* perigee = track->perigeeParameters();
	if (!perigee) {
	  ATH_MSG_DEBUG("NO perigee on this track");
	  return false;
	}
	const double qoverP = perigee->parameters()[Trk::qOverP] * 1000.;
	double pt = 0.;
	if (qoverP != 0 )
	  pt = fabs(1.0/qoverP)*sin(perigee->parameters()[Trk::theta]);
	ATH_MSG_DEBUG( " pt  : "<< pt );
	if (pt > m_maxPt)
	  return false;
      } //maxPt selection
    ATH_MSG_DEBUG("this track passes the beamspot track selection, will do beamspot constraint on it ");
    return true;
  }
  else return false;
}


AlignTrack* BeamspotVertexPreProcessor::doTrackRefit(const Track* track) {

  AlignTrack * alignTrack = nullptr;
  const Track* newTrack = nullptr;
  const VertexOnTrack* vot = nullptr;
  const xAOD::Vertex*    vtx = nullptr;
  AlignTrack::AlignTrackType type = AlignTrack::Unknown;
  // configuration of the material effects needed for track fitter
  ParticleSwitcher particleSwitch;
  ParticleHypothesis particleHypothesis = particleSwitch.particle[m_particleNumber];

  // initialization the GX2 track fitter
  ToolHandle<Trk::IGlobalTrackFitter> fitter = m_trackFitter;
  if (!m_useSingleFitter && AlignTrack::isSLTrack(track) )
      fitter = m_SLTrackFitter;

  IGlobalTrackFitter::AlignmentCache alignCache;

  ATH_MSG_DEBUG( "doTrackRefit ** START ** ");

  if(m_doPrimaryVertexConstraint){
    vot = provideVotFromVertex(track, vtx);
    if( !vot )  ATH_MSG_INFO( "VoT not found for this track! ");
    if( !vtx )  ATH_MSG_INFO( "VTX pointer not found for this track! ");
    if(vot){
      newTrack = doConstraintRefit(fitter, track, vot, particleHypothesis);
      type = AlignTrack::VertexConstrained;
      // this track failed the PV constraint reift
      if (!newTrack)  {
        ++m_nFailedPVRefits;
        msg(MSG::ERROR)<<"VertexConstraint track refit failed! "<<endmsg;
      }
    }
  }

  if( !newTrack && m_doBeamspotConstraint && doBeamspotConstraintTrackSelection(track) ){
      vot = provideVotFromBeamspot(track);
      if(vot){
        newTrack = doConstraintRefit(fitter, track, vot, particleHypothesis);
        type = AlignTrack::BeamspotConstrained;
        // this track failed the BS constraint reift
        if (!newTrack)  {
          ++m_nFailedBSRefits;
          msg(MSG::ERROR)<<"BSConstraint track refit failed! "<<endmsg;
        }
      }
  }


  //Refit to get full fitter covariance matrix
  // @TODO This is a little inefficienct and should
  // be addressed when the alignment code is made MT safe
  if(newTrack){
    Trk::Track* tmpTrk =  fitter->alignmentFit(alignCache,*newTrack,m_runOutlierRemoval,particleHypothesis);
    delete newTrack;
    newTrack = tmpTrk;
    if(!tmpTrk){
      if(type == AlignTrack::VertexConstrained)
      {
        ++m_nFailedPVRefits;
        ATH_MSG_ERROR("VertexConstraint track refit2 failed! ");
      }else if(type == AlignTrack::BeamspotConstrained)
      {
        ++m_nFailedPVRefits;
        ATH_MSG_ERROR("BSConstraint track refit2 failed! ");
      }
    }
  }

  if(!newTrack && m_doNormalRefit){
      newTrack = fitter->alignmentFit(alignCache,*track,m_runOutlierRemoval,particleHypothesis);
      type = AlignTrack::NormalRefitted;
      // this track failed the normal reift
      if (!newTrack)   {
        ++m_nFailedNormalRefits;
        msg(MSG::ERROR)<<"Normal track refit failed! "<<endmsg;
      }
  }

  



  if(newTrack) {
    alignTrack = new AlignTrack(*newTrack);
    // set original track pointer
    alignTrack->setOriginalTrack(track);
    // set the refit type
    alignTrack->setType(type);


    if (msgLvl(MSG::DEBUG) || msgLvl(MSG::VERBOSE)) {
      msg(MSG::DEBUG)<<"before refit: "<<endmsg;
      msg(MSG::DEBUG)<< *track <<endmsg;
      if (msgLvl(MSG::VERBOSE))   AlignTrack::dumpLessTrackInfo(*track,msg(MSG::DEBUG));

      msg(MSG::DEBUG)<<"after refit: "<<endmsg;
      msg(MSG::DEBUG)<< *newTrack <<endmsg;
      if (msgLvl(MSG::VERBOSE))   AlignTrack::dumpLessTrackInfo(*newTrack,msg(MSG::DEBUG));
    }

    if(AlignTrack::VertexConstrained == type || AlignTrack::BeamspotConstrained == type) alignTrack->doFindPerigee();

    if (m_storeFitMatrices) {
      alignTrack->setFullCovarianceMatrix(alignCache.m_fullCovarianceMatrix);
      alignTrack->setDerivativeMatrix(alignCache.m_derivMatrix);
    }
    delete newTrack;

    if( m_doFullVertexConstraint && vtx!=nullptr && type == AlignTrack::VertexConstrained ){
    // try to log the track-vertex association in the AlignVertex object:
      bool ifound=false;
      for (AlignVertex* ivtx : m_AlignVertices) {
        if( (ivtx->originalVertex())==vtx ) {
          ifound = true;
          ivtx->addAlignTrack(alignTrack);
        }
      }
      if( !ifound ) {
        AlignVertex*  avtx=new AlignVertex(vtx);
        ATH_MSG_DEBUG(" New AlignVertex has ben created.");

        // Beam Spot constraint on the vertex:
        if( m_doBeamspotConstraint && (xAOD::VxType::PriVtx == vtx->vertexType() || xAOD::VxType::PileUp == vtx->vertexType()) && vtx->vxTrackAtVertex().size()>4 ) {     // a beam line verex
          ATH_MSG_DEBUG(" The Beam Spot constraint will be added to the vertex.." );
          AmgSymMatrix(3)     qtemp;
          AmgVector(3)        vtemp;
          provideVtxBeamspot(avtx, &qtemp, &vtemp);
          (qtemp)(2,2) = 1000000.0;                  // disable Z constraint
          avtx->setConstraint( &qtemp, &vtemp);
        }

        avtx->addAlignTrack(alignTrack);
        m_AlignVertices.push_back(avtx);
      }
    }
     // increment counters
     ++m_trackTypeCounter[type];
     ++m_nTracks;

  }
  // garbage collection:
  if(vot)  delete vot;

  ATH_MSG_DEBUG( "doTrackRefit ** COMPLETED ** "); 
  return alignTrack;
}




//____________________________________________________________________________
DataVector<Track> * BeamspotVertexPreProcessor::processTrackCollection(const DataVector<Track> * tracks)
{
  ATH_MSG_DEBUG("BeamspotVertexPreProcessor::processTrackCollection()");

  if( !tracks || (tracks->empty()) )
    return nullptr;

  // Clear the AlignVertex container (will destruct the objects it owns as well!)
  m_AlignVertices.clear();


  if(m_doPrimaryVertexConstraint)
    prepareAllTracksVector();

  // the output collection of AlignTracks
  // we define it as collection of Tracks but fill AlignTracks inside
  DataVector<Track> * newTrks = new DataVector<Track>;

  int index(0);
  // loop over tracks
  TrackCollection::const_iterator itr     = tracks->begin();
  TrackCollection::const_iterator itr_end = tracks->end();
  
  ATH_MSG_DEBUG( "Starting loop on input track collection: "<<index);
  for ( ; itr != itr_end; ++itr, ++index) {
    ATH_MSG_DEBUG("Processing track "<<index);
    const Track* track = *itr;
    AlignTrack * alignTrack = nullptr;
    if (not track) continue;

    // check whether the track passes the basic selection
    if (m_doTrkSelection) {
      ATH_MSG_DEBUG( "Testing track selection on track: "<<index);
      if ((not m_trkSelector.empty()) and (not m_trkSelector->accept(*track))) continue;
    } // appliying track selection

    if(m_refitTracks){
      ATH_MSG_DEBUG( "Refitting track: "<<index );
      alignTrack = doTrackRefit(track);

      // 2nd track check after refit
      if(alignTrack && !m_trkSelector.empty()) {
	// refitted track loses the summary information, restoring it here
	alignTrack->setTrackSummary( std::make_unique<Trk::TrackSummary> (*track->trackSummary()) );
	// do not check for FullVertex tracks:
        if( !(alignTrack->getVtx()) ) {
	  if( m_doTrkSelection && !m_trkSelector->accept(*alignTrack))
	    continue;
	}
      }
      else {
        ATH_MSG_DEBUG( "Refit of track " << index << " ended with no alignTrack" );
      }
    } else {
      ATH_MSG_DEBUG( "No Track refit for track " << index << " --> building new aligntrack");
      alignTrack = new AlignTrack(*track);
      alignTrack->setOriginalTrack(track);
      alignTrack->setType(AlignTrack::Original);
    }
    // add the new align track to the collection
    if (alignTrack)  newTrks->push_back(alignTrack);
  } // end of loop over tracks

  ATH_MSG_INFO( "Processing of input track collection completed (size: " << tracks->size() << "). Size of the alignTrack collection: " << newTrks->size() ); 
  // delete the collection if it's empty
  if (newTrks->empty()) {
    delete newTrks;
    return nullptr;
  }

  return newTrks;
}

//____________________________________________________________________________
void BeamspotVertexPreProcessor::accumulateVTX(const AlignTrack* alignTrack) {

  if( !m_doFullVertexConstraint ) return;

  AlignVertex*  alignVertex = alignTrack->getVtx();

  ATH_MSG_DEBUG( " In accumulateVTX ");
  if( !alignVertex ) {
    ATH_MSG_DEBUG( "This alignTrack is not associated to any vertex -> return. ");
    return;
  }

  // get pointers so we can reuse them if they're valid
  Amg::MatrixX                 * ptrWeights   = alignTrack->weightMatrix();
  Amg::MatrixX                 * ptrWeightsFD = alignTrack->weightMatrixFirstDeriv();
  Amg::VectorX                 * ptrResiduals = alignTrack->residualVector();
  std::vector<AlignModuleDerivatives> * ptrDerivs    = alignTrack->derivatives();

  // check if pointers are valid
  if (!ptrWeights || !ptrWeightsFD || !ptrResiduals || !ptrDerivs) {
    msg(MSG::ERROR)<<"something missing from alignTrack!"<<endmsg;
    if (!ptrWeights)   msg(MSG::ERROR)<<"no weights!"<<endmsg;
    if (!ptrWeightsFD) msg(MSG::ERROR)<<"no weights for first deriv!"<<endmsg;
    if (!ptrResiduals) msg(MSG::ERROR)<<"no residuals!"<<endmsg;
    if (!ptrDerivs)    msg(MSG::ERROR)<<"no derivatives!"<<endmsg;
    return;
  }

  // get vectors
  Amg::VectorX&                    residuals     = *ptrResiduals;
  std::vector<AlignModuleDerivatives>  derivatives   = *ptrDerivs;

  // get weight matrices
  Amg::MatrixX&                weights           = *ptrWeights;
  Amg::MatrixX&                weightsFirstDeriv = *ptrWeightsFD;
  ATH_MSG_VERBOSE("weights="<<weights);
  ATH_MSG_VERBOSE("weightsFirstDeriv="<<weightsFirstDeriv);

  // get all alignPars and all derivatives
  ATH_MSG_DEBUG("accumulateVTX: The derivative vector size is  " << derivatives.size() );
  std::vector<AlignModuleDerivatives>::iterator derivIt     = derivatives.begin();
  std::vector<AlignModuleDerivatives>::iterator derivIt_end = derivatives.end();

  std::vector<Amg::VectorX*> allDerivatives[3];
  //    CLHEP::HepVector*  VTXDerivatives[3];
  Amg::VectorX  VTXDerivatives[3];
  const int    WSize(weights.cols());
  Amg::MatrixX*  WF = new Amg::MatrixX(3,WSize);
  std::vector<AlignModuleVertexDerivatives>* derivX = new std::vector<AlignModuleVertexDerivatives>(0);

  for ( ; derivIt!=derivIt_end ; ++derivIt) {

    // get AlignModule
    const AlignModule* module=derivIt->first;


    // get alignment parameters
    if( module ) {
      Amg::MatrixX*   F = new Amg::MatrixX(3,WSize);
      std::vector<Amg::VectorX>& deriv_vec = derivIt->second;
      ATH_MSG_VERBOSE( "accumulateVTX: The deriv_vec size is  " << deriv_vec.size() );
      DataVector<AlignPar>* alignPars = m_alignModuleTool->getAlignPars(module);
      int nModPars = alignPars->size();
      if ((nModPars+3) != (int)deriv_vec.size() ) {
        ATH_MSG_ERROR("accumulateVTX: Derivatives w.r.t. the vertex seem to be missing");
        delete derivX;
        delete WF;
        delete F;
        return;
      }
      for (int i=0;i<3;i++) {
        allDerivatives[i].push_back(&deriv_vec[nModPars+i]);
        for (int j=0;j<WSize;j++) {
          (*F)(i,j) = deriv_vec[nModPars+i][j];
        }
      }

      // prepare the X object in the AlignVertex:
      (*WF) += (*F) * weights;
      delete F;

    } else {
      ATH_MSG_ERROR("accumulateVTX: Derivatives do not have a valid pointer to the module.");
      delete derivX;
      delete WF;
      return;
    }
  }


  // second loop to fill the X object:
  derivIt     = derivatives.begin();
  for ( ; derivIt!=derivIt_end ; ++derivIt) {

    // get AlignModule
    const AlignModule* module=derivIt->first;


    // get alignment parameters
    if( module ) {
      std::vector<Amg::VectorX>& deriv_vec = derivIt->second;
      std::vector<Amg::VectorX>* drdaWF = new std::vector<Amg::VectorX>(0);
      ATH_MSG_DEBUG( "accumulateVTX: The deriv_vec size is  " << deriv_vec.size() );
      DataVector<AlignPar>* alignPars = m_alignModuleTool->getAlignPars(module);
      int nModPars = alignPars->size();
      if ((nModPars+3) != (int)deriv_vec.size() ) {
        ATH_MSG_ERROR("accumulateVTX: Derivatives w.r.t. the vertex seem to be missing");
        delete derivX;
        delete WF;
        delete drdaWF;
        return;
      }
      for (int i=0;i<nModPars;i++) {
        drdaWF->push_back(2.0 * (*WF) * deriv_vec[i]);
      }

      // now add contribution from this track to the X object:
      derivX->push_back(make_pair(module,*drdaWF));
      ATH_MSG_DEBUG("accumulateVTX: derivX incremented by:  " << (*drdaWF) );
      delete drdaWF;

    } else {
      ATH_MSG_ERROR("accumulateVTX: Derivatives do not have a valid pointer to the module.");
      delete derivX;
      delete WF;
      return;
    }
  }



  // prepare derivatives w.r.t. the vertex position:
  int nmodules = allDerivatives[0].size();
  msg(MSG::DEBUG) << "accumulateVTX: allDerivatives size is  " << nmodules << endmsg;
  for( int ii=0; ii<3; ++ii ) {
    VTXDerivatives[ii] = (*(allDerivatives[ii])[0]);
    for( int jj=1; jj<nmodules; ++jj ) {
      VTXDerivatives[ii] += (*(allDerivatives[ii])[jj]);
    }
  }


  AmgVector(3)     vtxV;
  AmgSymMatrix(3)  vtxM;

  Amg::VectorX RHM= weightsFirstDeriv * residuals;
  ATH_MSG_DEBUG("RHM: "<<RHM);

  for (int ipar=0;ipar<3;ipar++) {

    // calculate first derivative
    //  CLHEP::HepMatrix derivativesT = (*(VTXDerivatives[ipar])).T();
    Amg::MatrixX derivativesT = (VTXDerivatives[ipar]).transpose();
    ATH_MSG_DEBUG("derivativesT (size "<<derivativesT.cols()<<"): "<<derivativesT);

    Amg::MatrixX  tempV = (2.* derivativesT * RHM);
    vtxV[ipar] = tempV(0,0);

    for (int jpar=ipar;jpar<3;jpar++) {

      // calculate second derivatives
      Amg::MatrixX RHM2 = weights * (VTXDerivatives[jpar]);

      Amg::MatrixX  tempM = (2.* derivativesT * RHM2);
      vtxM(ipar,jpar) = tempM(0,0);

    }

  }

  // increment the vtx algebra objects:

  alignVertex->incrementVector(vtxV);
  alignVertex->incrementMatrix(vtxM);
  //   ATH_MSG_DEBUG("accumulateVTX: derivX size = "<< derivX->size());
  alignVertex->addDerivatives(derivX);

  // garbage collection:

  delete    WF;
  delete    derivX;
}


//____________________________________________________________________________
void BeamspotVertexPreProcessor::solveVTX() {

  if( m_doFullVertexConstraint ){
    ATH_MSG_DEBUG("In solveVTX. Number of vertices = " << m_AlignVertices.size() );
    for (AlignVertex* ivtx : m_AlignVertices) {
      if( ivtx->Ntracks()>1 ) {
        ivtx->fitVertex();
       } else {
         msg(MSG::WARNING) << "This vertex contains " << ivtx->Ntracks() << " tracks. No solution possible." <<endmsg;
       }

       ATH_MSG_DEBUG( "This vertex contains " << ivtx->Ntracks() << " tracks.");
       if( msgLvl(MSG::DEBUG) )  ivtx->dump(msg(MSG::DEBUG));
    }
  }
}

//____________________________________________________________________________
void BeamspotVertexPreProcessor::printSummary()
{
  if(m_logStream) {

    *m_logStream<<"*************************************************************"<<std::endl;
    *m_logStream<<"******        BeamspotVertexPreProcessor summary       ******"<<std::endl;
    *m_logStream<<"*"<<std::endl;
    *m_logStream<<"* number of created AlignTracks :  "<<m_nTracks<<std::endl;
    if(m_nTracks>0) {
      *m_logStream<<"* --------------------------------------------"<<std::endl;
      for(int i=0; i<AlignTrack::NTrackTypes; ++i) {
        if(m_trackTypeCounter[i]>0)
          *m_logStream<<"*           "<<(AlignTrack::AlignTrackType)i<<":  "<<m_trackTypeCounter[i]<<std::endl;
        }
    }
    *m_logStream<<"*"<<std::endl;
    *m_logStream<<"* number of failed normal refits :              " << m_nFailedNormalRefits << std::endl;
    *m_logStream<<"* number of failed refits with primary vertex : " << m_nFailedPVRefits     << std::endl;
    *m_logStream<<"* number of failed refits with beam-spot :      " << m_nFailedBSRefits     << std::endl;
    *m_logStream<<"*"<<std::endl;
  }
}

//____________________________________________________________________________
StatusCode BeamspotVertexPreProcessor::finalize()
{
  ATH_MSG_INFO("BeamspotVertexPreProcessor::finalize()");

  return StatusCode::SUCCESS;
}

//____________________________________________________________________________
}

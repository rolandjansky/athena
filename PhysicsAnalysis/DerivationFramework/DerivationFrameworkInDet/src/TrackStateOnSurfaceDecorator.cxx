/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackStateOnSurfaceDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author:Anthony Morley
//

#include "DerivationFrameworkInDet/TrackStateOnSurfaceDecorator.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"

#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/TrackStateValidationAuxContainer.h"


#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkParameters/TrackParameters.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrkPrepRawData/PrepRawData.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"

#include "TrkToolInterfaces/IUpdator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkEventUtils/TrackStateOnSurfaceComparisonFunction.h"

#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"

#include "xAODEventInfo/EventInfo.h"
#include "CommissionEvent/ComTime.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "xAODTracking/VertexContainer.h"   
#include "TrkTrack/Track.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkEventPrimitives/PropDirection.h"

#include "TRT_ToT_Tools/ITRT_ToT_dEdx.h"  

#include <vector>
#include <string>

namespace DerivationFramework {

  TrackStateOnSurfaceDecorator::TrackStateOnSurfaceDecorator(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_idHelper(0),
    m_pixId(0),
    m_sctId(0),
    m_trtId(0),
    m_updator("Trk::KalmanUpdator"),
    m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
    m_holeSearchTool("InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool"),
    m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
    m_trtcaldbSvc("TRT_CalDbSvc",n),
    m_TRTdEdxTool("InDet::TRT_ToT_Tools/TRT_ToT_dEdx")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    // --- Steering and configuration flags
    declareProperty("IsSimulation",           m_isSimulation=true);

    declareProperty("StoreHoles",             m_storeHoles =true);
    declareProperty("StoreOutliers",          m_storeOutliers = true);
    declareProperty("StoreTRT",               m_storeTRT =false);
    declareProperty("StoreSCT",               m_storeSCT = true);
    declareProperty("StorePixel",             m_storePixel =true);
    declareProperty("AddPulls",               m_addPulls =true);
    declareProperty("AddSurfaceInfo",         m_addSurfaceInfo =true);
    declareProperty("AddPRD",                 m_addPRD =true);
    declareProperty("AddExtraEventInfo",      m_addExtraEventInfo=true);

    // --- Configuration keys
    declareProperty("DecorationPrefix",       m_sgName="IDDET1_");
    declareProperty("ContainerName",          m_containerName="InDetTrackParticles");
    declareProperty("PixelMapName",           m_pixelMapName = "PixelClustersOffsets");
    declareProperty("SctMapName",             m_sctMapName = "SCT_ClustersOffsets");
    declareProperty("TrtMapName",             m_trtMapName = "TRT_DriftCirclesOffsets");
    declareProperty("PixelClustersName",      m_pixelClustersName = "PixelClusters");
    declareProperty("SctClustersName",        m_sctClustersName = "SCT_Clusters");
    declareProperty("TrtDriftCirclesName",    m_trtDCName = "TRT_DriftCircles");
    declareProperty("PixelMsosName",          m_pixelMsosName = "PixelMSOSs");
    declareProperty("SctMsosName",            m_sctMsosName = "SCT_MSOSs");
    declareProperty("TrtMsosName",            m_trtMsosName = "TRT_MSOSs");    

    // -- Tools 
    declareProperty("Updator",                m_updator);    
    declareProperty("ResidualPullCalculator", m_residualPullCalculator);
    declareProperty("HoleSearch",             m_holeSearchTool);
    declareProperty("TRT_CalDbSvc",           m_trtcaldbSvc);
    declareProperty("TrackExtrapolator",      m_extrapolator);
    declareProperty("TRT_ToT_dEdx",           m_TRTdEdxTool);
  }

  StatusCode TrackStateOnSurfaceDecorator::initialize()
  {
    ATH_MSG_DEBUG("Initialize");

    if (m_sgName=="") {
      ATH_MSG_WARNING("No decoration prefix name provided for the output of TrackStateOnSurfaceDecorator!");
    }
    ATH_MSG_DEBUG("Prefix for decoration: " << m_sgName);
    
    if (m_containerName=="") {
      ATH_MSG_ERROR("No TrackParticle collection provided for TrackStateOnSurfaceDecorator!");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Input TrackParticle container: " << m_containerName);

    // need Atlas id-helpers to identify sub-detectors, take them from detStore
    if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
      ATH_MSG_ERROR("Could not get AtlasDetectorID helper");
      return StatusCode::FAILURE;
    }   

    if( m_storePixel && detStore()->retrieve(m_pixId,"PixelID").isFailure() ){
      ATH_MSG_ERROR("Unable to retrieve pixel ID helper");
      return StatusCode::FAILURE;
    }

    if( m_storeSCT && detStore()->retrieve(m_sctId,"SCT_ID").isFailure() ){
      ATH_MSG_ERROR("Could not retrieve SCT helper");
      return StatusCode::FAILURE; 
    } 

    if( m_storeTRT && detStore()->retrieve(m_trtId,"TRT_ID").isFailure() ){
      ATH_MSG_ERROR("Could not retrieve TRT helper");
      return StatusCode::FAILURE; 
    } 
    
    if( m_storeTRT){
      CHECK(m_trtcaldbSvc.retrieve());
    }

    if( m_addPulls ){
      CHECK(m_updator.retrieve());
      CHECK(m_residualPullCalculator.retrieve());
    }
    
    if( m_storeHoles ) {
      CHECK( m_holeSearchTool.retrieve() );
    }

    CHECK(m_extrapolator.retrieve());

    if ( m_storeTRT && !m_TRTdEdxTool.empty() ) {
      CHECK( m_TRTdEdxTool.retrieve() );
	}

    ATH_MSG_DEBUG("Initialization finished.");

    return StatusCode::SUCCESS;
  }

  StatusCode TrackStateOnSurfaceDecorator::finalize()
  {
    ATH_MSG_DEBUG("Finalize");
    return StatusCode::SUCCESS;
  }

  StatusCode TrackStateOnSurfaceDecorator::addBranches() const
  {
    ATH_MSG_DEBUG("Adding TSOS decorations the track particles");

    static SG::AuxElement::Decorator< std::vector< ElementLink< xAOD::TrackStateValidationContainer > > >  dectsos_msosLink(m_sgName+"msosLink");

 
    // --- Retrieve track container (absolutely needed for decoration)
    const xAOD::TrackParticleContainer* tracks=0;
    CHECK( evtStore()->retrieve( tracks, m_containerName ) );
    if( ! tracks ) {
        ATH_MSG_ERROR ("Couldn't retrieve TrackParticles with key: " << m_containerName );
        return StatusCode::FAILURE;
    }
    
    
    const std::vector<unsigned int>*  pixelClusterOffsets(0);
    const std::vector<unsigned int>*  sctClusterOffsets(0);
    const std::vector<unsigned int>*  trtClusterOffsets(0);
    
    const xAOD::TrackMeasurementValidationContainer* pixelClusters(0);
    const xAOD::TrackMeasurementValidationContainer* sctClusters(0);
    const xAOD::TrackMeasurementValidationContainer* trtDCs(0);
 
    
    // Create the xAOD container and its auxiliary store
    xAOD::TrackStateValidationContainer*    msosPixel=0; 
    xAOD::TrackStateValidationContainer*    msosSCT=0; 
    xAOD::TrackStateValidationContainer*    msosTRT=0;

    int nPixelMSOS(0);
    int nSCT_MSOS(0);
    int nTRT_MSOS(0);

    // --- Add event-level information
    if (m_addExtraEventInfo) {
      ATH_MSG_DEBUG("Adding EventInfo decorations");
      const xAOD::EventInfo* eventInfo = 0;
      if (evtStore()->retrieve(eventInfo).isFailure()) {
        ATH_MSG_ERROR(" Cannot access to event info.");
        return StatusCode::FAILURE;
      }

      //Add TRT event phase
      const ComTime *trtPhase = 0;
      float trtPhase_time=0.0;
      if ( evtStore()->contains<ComTime>("TRT_Phase") ){
        StatusCode sc = evtStore()->retrieve(trtPhase, "TRT_Phase");  
        if (sc.isFailure() || !trtPhase) {
          //do not throw errors, since it could not be there
          ATH_MSG_DEBUG("Failed to retrieve TRT phase information.");
        } else {
          trtPhase_time = trtPhase->getTime();
        } 
      } //TRT phase
      eventInfo->auxdecor<float>(m_sgName+"TrtPhaseTime") = trtPhase_time;
      
    } //extra event info

    
    // --- Add track states containers
    if(m_addPRD){
      // Get clusters and the mapping between xAOD::PRD and Trk::PRD
      // Store the MSOS's in a conatiner based on the type of the detector 
      if(m_storePixel){
        ATH_MSG_DEBUG("Creating Pixel track state container");
        CHECK( evtStore()->retrieve( pixelClusterOffsets, m_pixelMapName ) );
        CHECK( evtStore()->retrieve( pixelClusters, m_pixelClustersName ) );
      
        msosPixel = new xAOD::TrackStateValidationContainer();
        xAOD::TrackStateValidationAuxContainer* aux = new xAOD::TrackStateValidationAuxContainer();
        CHECK( evtStore()->record( msosPixel, m_pixelMsosName ) );
        CHECK( evtStore()->record( aux, m_pixelMsosName + "Aux." ) );
        msosPixel->setStore( aux );
      }
      if(m_storeSCT){
        ATH_MSG_DEBUG("Creating SCT track state container");
        CHECK( evtStore()->retrieve( sctClusterOffsets, m_sctMapName ) );
        CHECK( evtStore()->retrieve( sctClusters, m_sctClustersName ) );

        msosSCT = new xAOD::TrackStateValidationContainer();
        xAOD::TrackStateValidationAuxContainer* aux = new xAOD::TrackStateValidationAuxContainer();
        CHECK( evtStore()->record( msosSCT, m_sctMsosName ) );
        CHECK( evtStore()->record( aux, m_sctMsosName + "Aux." ) );
        msosSCT->setStore( aux );
      }
      if(m_storeTRT){
        ATH_MSG_DEBUG("Creating TRT track state container");
        CHECK( evtStore()->retrieve( trtClusterOffsets, m_trtMapName ) );    
        CHECK( evtStore()->retrieve( trtDCs, m_trtDCName ) );

        msosTRT = new xAOD::TrackStateValidationContainer();
        xAOD::TrackStateValidationAuxContainer* aux = new xAOD::TrackStateValidationAuxContainer();
        CHECK( evtStore()->record( msosTRT, m_trtMsosName ) );
        CHECK( evtStore()->record( aux, m_trtMsosName + "Aux." ) );
        msosTRT->setStore( aux );
      }
    }

    // -- Run over each track and decorate it
    for (const auto& track : *tracks) {
      //-- Start with things that do not need a Trk::Track object

      // -- Now things that require a Trk::Track object
      if( !track->trackLink().isValid() || track->track() == 0 ) {
        ATH_MSG_WARNING("Track particle without Trk::Track");
        continue;
      }
      ATH_MSG_DEBUG("We have a Trk::Track");

      // We now have a valid Trk::Track
      const Trk::Track* trkTrack = track->track();

      //  This is the vector in which we will store the element links to the MSOS's
      std::vector< ElementLink< xAOD::TrackStateValidationContainer > > msosLink;
      

      if ( m_storeTRT && !m_TRTdEdxTool.empty() ) {
	// for dEdx studies
	SG::AuxElement::Decorator< float > decoratorTRTdEdx("ToT_dEdx");
	SG::AuxElement::Decorator< float > decoratorTRTusedHits("ToT_usedHits");    
	SG::AuxElement::Decorator< float > decoratorTRTdEdx_noHT_divByL("ToT_dEdx_noHT_divByL");
	SG::AuxElement::Decorator< float > decoratorTRTusedHits_noHT_divByL("ToT_usedHits_noHT_divByL");    

	decoratorTRTdEdx (*track)     = m_TRTdEdxTool->dEdx( trkTrack, true, true, true);
        ITRT_ToT_dEdx::EGasType gasType;
	decoratorTRTusedHits (*track) = m_TRTdEdxTool->usedHits( trkTrack, true, true);
	decoratorTRTdEdx_noHT_divByL (*track)     = m_TRTdEdxTool->dEdx( trkTrack, true, false, true);
	decoratorTRTusedHits_noHT_divByL (*track) = m_TRTdEdxTool->usedHits( trkTrack, true, false);
      }

      // -- Add Track states to the current track, filtering on their type
      std::vector<const Trk::TrackStateOnSurface*> tsoss;
      for (const auto& trackState: *(trkTrack->trackStateOnSurfaces())){
        //Get rid of any holes that already exist  --  we are doing the search again
        if( trackState->types()[Trk::TrackStateOnSurface::Hole] )
          continue;
        tsoss.push_back(trackState);
      } 

      std::unique_ptr<const DataVector<const Trk::TrackStateOnSurface>>  holes; 
      if(m_storeHoles){
        holes =  std::unique_ptr<const DataVector<const Trk::TrackStateOnSurface>>( m_holeSearchTool->getHolesOnTrack(*trkTrack, trkTrack->info().particleHypothesis()) ); 
        for (auto hole: *holes){
          tsoss.push_back(hole);
        }
        if(trkTrack->perigeeParameters()){
          Trk::TrackStateOnSurfaceComparisonFunction CompFunc( trkTrack->perigeeParameters()->momentum() );
          stable_sort( tsoss.begin(), tsoss.end(), CompFunc );
        } else {
          ATH_MSG_ERROR("Track has not perigee parameters");
        }
      }

      //Loop over the TrkStateOnSurfaces
      for (const auto& trackState: tsoss){

        //Only store Holes, Measurement &  Outliers 
        if( !trackState->types()[Trk::TrackStateOnSurface::Hole] && 
            !trackState->types()[Trk::TrackStateOnSurface::Measurement] &&
            !trackState->types()[Trk::TrackStateOnSurface::Outlier] ) {
          continue;
        }

        // Check if we want to store this types of TSOS
        if(!m_storeOutliers && trackState->types()[Trk::TrackStateOnSurface::Outlier] )
          continue;


        if(!m_storeHoles && trackState->types()[Trk::TrackStateOnSurface::Hole] )
          continue;
                
        // Check that the surface has detector element
        if(!trackState->surface().associatedDetectorElement()){
          continue;
        }

        // Check that the surface ID is valid
        Identifier surfaceID = trackState->surface().associatedDetectorElement()->identify();
        if( !surfaceID.is_valid() ){
          ATH_MSG_WARNING("Invalid surface ID");
          continue;
        }

        //Determine what detector the hit is in
        bool isPixel(false); 
        bool isSCT(false);  
        bool isTRT(false);  
      
        if( m_idHelper->is_trt(surfaceID) ){
          isTRT = true;
          if(!m_storeTRT)
            continue;
        }else if( m_idHelper->is_sct(surfaceID) ){
          isSCT = true;
          if(!m_storeSCT)
            continue;
        }else if( m_idHelper->is_pixel(surfaceID) ){
          isPixel = true; 
          if(!m_storePixel)
            continue;
        } 
        
        if( !isPixel && !isSCT && !isTRT ){
          continue;
        } 
         
        //Create new MSOS to fill with information
        xAOD::TrackStateValidation*  msos =  new xAOD::TrackStateValidation();

        //Put it in the obeject in the correct conatiner -  one for each detector type.
        if(isTRT){        
          //Add the msos to the container 
          msosTRT->push_back( msos );
          //Set the det id
          msos->setDetType( Trk::TrackState::TRT );
          //Build the element link to the MSOS
          ElementLink< xAOD::TrackStateValidationContainer > elink( *msosTRT, nTRT_MSOS ); 
          elink.toPersistent();
          msosLink.push_back(elink);
          ++nTRT_MSOS;
        }else if(isSCT){
          //Add the msos to the container 
          msosSCT->push_back( msos );
          //Set the det id
          msos->setDetType( Trk::TrackState::SCT );
          //Build the element link to the MSOS
          ElementLink< xAOD::TrackStateValidationContainer > elink( *msosSCT, nSCT_MSOS ); 
          elink.toPersistent();
          msosLink.push_back(elink);
          ++nSCT_MSOS;
        }else if(isPixel){
          //Add the msos to the container 
          msosPixel->push_back( msos );
          //Set the det id
          msos->setDetType( Trk::TrackState::Pixel );
          //Build the element link to the MSOS
          ElementLink< xAOD::TrackStateValidationContainer > elink( *msosPixel, nPixelMSOS ); 
          elink.toPersistent();
          msosLink.push_back(elink);
          ++nPixelMSOS;
        }

        //fill type
        if( trackState->types()[Trk::TrackStateOnSurface::Hole] ){   
          msos->setType( Trk::TrackStateOnSurface::Hole );
        } else if (trackState->types()[Trk::TrackStateOnSurface::Measurement]){
          msos->setType( Trk::TrackStateOnSurface::Measurement );
        } else if ( trackState->types()[Trk::TrackStateOnSurface::Outlier] ) {
          msos->setType( Trk::TrackStateOnSurface::Outlier );
        }

        //Fill surface id
        msos->setDetElementId(  surfaceID.get_compact() );


        const Trk::TrackParameters* tp = trackState->trackParameters();       

        const Trk::MeasurementBase* measurement=trackState->measurementOnTrack();
	if (m_storeTRT) {
	  const InDet::TRT_DriftCircleOnTrack *driftcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(measurement);
	  if (!measurement) {
	    msos->auxdata<float>("HitZ")=-3000;
	    msos->auxdata<float>("HitR")=-1;
	    msos->auxdata<float>("rTrkWire")=-1;
	  }
	  else { 
	    if (!driftcircle) {
	      msos->auxdata<float>("HitZ")=-3000;
	      msos->auxdata<float>("HitR")=-1;
	      msos->auxdata<float>("rTrkWire")=-1;
	    }
	    else {
	      if (tp) {
		const Amg::Vector3D& gp = driftcircle->globalPosition();
		msos->auxdata<float>("HitZ")=gp.z();
		msos->auxdata<float>("HitR")=gp.perp();
		msos->auxdata<float>("rTrkWire")= fabs(trackState->trackParameters()->parameters()[Trk::driftRadius]);
	      }
	      else {
		msos->auxdata<float>("HitZ") =driftcircle->associatedSurface().center().z();
		msos->auxdata<float>("HitR") =driftcircle->associatedSurface().center().perp();
		msos->auxdata<float>("rTrkWire")=0;
	      }
	    }
	  }
	}

        // Track extrapolation
        std::unique_ptr<const Trk::TrackParameters> extrap( m_extrapolator->extrapolate(*trkTrack,trackState->surface()) );
//        const Trk::TrackParameters* extrap = m_extrapolator->extrapolate(*trkTrack,trackState->surface(),Trk::PropDirection::anyDirection,false);
//        const Trk::TrackParameters* extrap = m_extrapolator->extrapolate(*trkTrack,trackState->surface(),Trk::PropDirection::anyDirection,true,Trk::ParticleHypothesis::pion,Trk::MaterialUpdateMode::removeNoise);
//        const Trk::TrackParameters* extrap = m_extrapolator->extrapolate(*trkTrack,trackState->surface(),Trk::PropDirection::anyDirection,true,Trk::ParticleHypothesis::nonInteractingMuon,Trk::MaterialUpdateMode::addNoise);

//        Amg::Transform3D *newDetElem = new Amg::Transform3D();
//        *newDetElem = trackState->surface().transform();
//        Trk::PerigeeSurface newSurface(newDetElem);
//        const Trk::TrackParameters* extrap = m_extrapolator->extrapolateDirectly(track->perigeeParameters(),newSurface);

        // Set local positions on the surface
        if (tp) { 
          msos->setLocalPosition( tp->parameters()[0], tp->parameters()[1] ); 

          if (extrap.get()) {
            ATH_MSG_DEBUG("    Original position " << tp->parameters()[0] << " " << tp->parameters()[1]);
            ATH_MSG_DEBUG("Extrapolated position " << extrap->parameters()[0] << " " << extrap->parameters()[1]);
          }

        }
        else {
          if (extrap.get()) {
            msos->setLocalPosition( extrap->parameters()[0], extrap->parameters()[1] ); 
          }
          else { 
            ATH_MSG_DEBUG("Track extrapolation failed."); 
          }
        }

        // Set calculate local incident angles
        const Trk::TrkDetElementBase *de = trackState->surface().associatedDetectorElement();
        const InDetDD::SiDetectorElement *side = dynamic_cast<const InDetDD::SiDetectorElement *>(de);
        if (side && (isSCT || isPixel)) {
          Amg::Vector3D mynormal = side->normal();
          Amg::Vector3D myphiax = side->phiAxis();
          Amg::Vector3D myetaax = side->etaAxis();
          if (tp) {
            Amg::Vector3D mytrack = tp->momentum();
            float trketacomp = mytrack.dot(myetaax);
            float trkphicomp = mytrack.dot(myphiax);
            float trknormcomp = mytrack.dot(mynormal);  

            ATH_MSG_DEBUG("     Original incident angle " << trketacomp << " " << trkphicomp << " " << trknormcomp);
            if (extrap.get()) {
              Amg::Vector3D metrack = extrap->momentum();
              float trketacompX = metrack.dot(myetaax);
              float trkphicompX = metrack.dot(myphiax);
              float trknormcompX = metrack.dot(mynormal);  
              ATH_MSG_DEBUG("Extrapolated incident angle " << trketacompX << " " << trkphicompX << " " << trknormcompX);
            }
            msos->setLocalAngles( atan2(trketacomp,trknormcomp), atan2(trkphicomp,trknormcomp) );
          }
          else {
            if (extrap.get()) {
              Amg::Vector3D metrack = extrap->momentum();
              float trketacompX = metrack.dot(myetaax);
              float trkphicompX = metrack.dot(myphiax);
              float trknormcompX = metrack.dot(mynormal);  
              msos->setLocalAngles( atan2(trketacompX,trknormcompX), atan2(trkphicompX,trknormcompX) );
            }
          }
        } 
 
        //Get the measurement base object
        //const Trk::MeasurementBase* measurement=trackState->measurementOnTrack();
        if(!measurement)
          continue;
      
        const Trk::RIO_OnTrack* hit = measurement ? dynamic_cast<const Trk::RIO_OnTrack*>(measurement) : 0;

        if(!hit){
          const Trk::CompetingRIOsOnTrack *crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(measurement);
          if(crot){
            hit = &crot->rioOnTrack( crot->indexOfMaxAssignProb() );
          }
        }
        
        if(m_addPRD && hit){
          // Build an element link to the xAOD PRD
          const Trk::PrepRawData* prd = hit->prepRawData();
          if(prd && prd->getHashAndIndex().isValid() ){            
            if(isTRT){
              msos->setTrackMeasurementValidationLink( buildElementLink( prd, trtClusterOffsets, trtDCs) );
            }else if(isSCT){
              msos->setTrackMeasurementValidationLink( buildElementLink( prd, sctClusterOffsets, sctClusters) );
            }else if(isPixel){
              msos->setTrackMeasurementValidationLink( buildElementLink( prd, pixelClusterOffsets, pixelClusters) );
            }
          } 
        }


//        if (isPixel ) {
//           tsos_bec.push_back( (int)m_pixId->barrel_ec(surfaceID) );
//           tsos_layer.push_back( (int)m_pixId->layer_disk(surfaceID) );
//           tsos_moduleeta.push_back( (int)m_pixId->eta_module(surfaceID) );
//           tsos_modulephi.push_back( (int)m_pixId->phi_module(surfaceID) );
//        } else if ( isSCT ) {
//           tsos_bec.push_back( (int)m_sctId->barrel_ec(surfaceID) );
//           tsos_layer.push_back( (int)m_sctId->layer_disk(surfaceID) );
//           tsos_moduleeta.push_back( (int)m_sctId->barrel_ec(surfaceID) );
//           tsos_modulephi.push_back( (int)m_sctId->phi_module(surfaceID) );
//        } else if ( isTRT ) {
//           tsos_bec.push_back( (int)m_trtId->barrel_ec(surfaceID) );
//           tsos_layer.push_back( (int)m_trtId->layer_or_wheel(surfaceID) );
//           tsos_moduleeta.push_back( (int)m_trtId->straw_layer(surfaceID) );
//           tsos_modulephi.push_back( (int)m_trtId->phi_module(surfaceID) );
//        }
        
        

 
 
        // Add the drift time for the tracks position -- note the position is biased 
        if (isTRT) {
          TRTCond::RtRelation const *rtr = m_trtcaldbSvc->getRtRelation(surfaceID);
          if(rtr) {
            if (tp){
              msos->auxdata<float>("driftTime") = rtr->drifttime(fabs(tp->parameters()[0]));
            }
            else {
              if (extrap.get()) {
                msos->auxdata<float>("driftTime") = rtr->drifttime(fabs(extrap->parameters()[0]));
              }
            }
          }
	}

        if (m_addPulls) {

          const Trk::ResidualPull *biased = 0;
          const Trk::ResidualPull *unbiased = 0;
          if (tp) { 
            biased   = m_residualPullCalculator->residualPull(measurement, tp, Trk::ResidualPull::Biased);
	    if (m_storeTRT) msos->auxdata<float>("TrackError_biased") = sqrt(fabs((*tp->covariance())(Trk::locX,Trk::locX)));

            std::unique_ptr<const Trk::TrackParameters> unbiasedTp( m_updator->removeFromState(*tp, measurement->localParameters(), measurement->localCovariance()) );   
            if(unbiasedTp.get()) {
	      if (m_storeTRT) msos->auxdata<float>("TrackError_unbiased") = sqrt(fabs((*unbiasedTp.get()->covariance())(Trk::locX,Trk::locX)));
              unbiased = m_residualPullCalculator->residualPull(measurement, unbiasedTp.get(), Trk::ResidualPull::Unbiased);
	    }
          }
          else {
            if (extrap.get()) {
	      if (m_storeTRT) msos->auxdata<float>("TrackError_unbiased") = sqrt(fabs((*extrap.get()->covariance())(Trk::locX,Trk::locX)));
              biased   = m_residualPullCalculator->residualPull(measurement, extrap.get(), Trk::ResidualPull::Biased);
              unbiased = m_residualPullCalculator->residualPull(measurement, extrap.get(), Trk::ResidualPull::Unbiased);
            }
          }

          if (biased) {
            if(biased->dimension()>Trk::locY){  
              msos->setBiasedResidual( biased->residual()[Trk::locX], biased->residual()[Trk::locY] );
              msos->setBiasedPull( biased->pull()[Trk::locX], biased->pull()[Trk::locY] );            
            } else {
              msos->setBiasedResidual( biased->residual()[Trk::locX], 0 );
              msos->setBiasedPull( biased->pull()[Trk::locX], 0 );            
            }
            delete biased;
          } 

          if (unbiased) {
            if(unbiased->dimension()>Trk::locY){  
              msos->setUnbiasedResidual( unbiased->residual()[Trk::locX], unbiased->residual()[Trk::locY] );
              msos->setUnbiasedPull( unbiased->pull()[Trk::locX], unbiased->pull()[Trk::locY] );            
            } else {
              msos->setUnbiasedResidual( unbiased->residual()[Trk::locX], 0 );
              msos->setUnbiasedPull( unbiased->pull()[Trk::locX], 0 );            
            }
            delete unbiased;
          }

        }

      } //end loop over TSOS's
        
      ATH_MSG_DEBUG("The number of TSOS's " << msosLink.size() );

      dectsos_msosLink( *track ) = msosLink;
      
      ATH_MSG_DEBUG("Finished dressing TrackParticle");
      

    } // end of loop over tracks              
    return StatusCode::SUCCESS;
    }  
  
  
  ElementLink< xAOD::TrackMeasurementValidationContainer >  TrackStateOnSurfaceDecorator::buildElementLink( const Trk::PrepRawData* prd, 
                                                                  const std::vector<unsigned int>* offsets, 
                                                                  const xAOD::TrackMeasurementValidationContainer* xaodPrdCont) const
  {
                                                                  
    const IdentContIndex& contIndex = prd->getHashAndIndex();
    if( contIndex.collHash() >= offsets->size() ){
      ATH_MSG_ERROR(" Offsets are incorrect " <<  contIndex.collHash() << " " <<  offsets->size() <<" "<< contIndex.objIndex());
      return ElementLink< xAOD::TrackMeasurementValidationContainer >(0,0);
    }
    
    unsigned int xaodIndex = offsets->at( contIndex.collHash() ) + contIndex.objIndex(); 
    ElementLink< xAOD::TrackMeasurementValidationContainer > el( *xaodPrdCont, xaodIndex ); 
    el.toPersistent();
    
    return el;
 
  }
  
  
}

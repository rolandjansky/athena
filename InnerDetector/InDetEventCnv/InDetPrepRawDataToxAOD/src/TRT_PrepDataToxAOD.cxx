/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_PrepDataToxAOD.cxx
//   Implementation file for class TRT_PrepDataToxAOD
///////////////////////////////////////////////////////////////////

#include "TRT_PrepDataToxAOD.h"

#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
//#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "xAODTracking/TrackMeasurementValidation.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODTracking/TrackMeasurementValidationAuxContainer.h"


#include "Identifier/Identifier.h"
#include "InDetIdentifier/TRT_ID.h"


#include "TrkSurfaces/Surface.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "InDetSimData/InDetSimDataCollection.h"


/////////////////////////////////////////////////////////////////////
//
//         Constructor with parameters:
//
/////////////////////////////////////////////////////////////////////
TRT_PrepDataToxAOD::TRT_PrepDataToxAOD(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_driftFunctionTool("TRT_DriftFunctionTool"),
  m_trtcaldbSvc("TRT_CalDbSvc", name),
  m_neighbourSvc("TRT_StrawNeighbourSvc", name),
  m_TRTStrawSummarySvc("InDetTRTStrawStatusSummarySvc",name),
  m_driftcirclecontainer("TRT_DriftCircles"),
  m_SDOcontainer("TRT_SDO_Map"),
  m_multiTruth("PRD_MultiTruthTRT")
{ 
  declareProperty("TRTDriftFunctionTool", m_driftFunctionTool);
  declareProperty("TRTCalDbSvc",           m_trtcaldbSvc);
  declareProperty("NeighbourSvc",          m_neighbourSvc);
  declareProperty("TRTStrawSummarySvc",    m_TRTStrawSummarySvc);
  declareProperty("NeighbourSvc",          m_neighbourSvc);
  declareProperty("DriftCircleContaines",  m_driftcirclecontainer);
  declareProperty("MC_TRTUncompressedHit", m_SDOcontainer);
  declareProperty("PRD_MultiTruth",        m_multiTruth);
  declareProperty("UseTruthInfo",          m_useTruthInfo=false);
}

/////////////////////////////////////////////////////////////////////
//
//        Initialize method: 
//
/////////////////////////////////////////////////////////////////////
StatusCode TRT_PrepDataToxAOD::initialize()
{
  StatusCode sc = detStore()->retrieve(m_TRTHelper, "TRT_ID");
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Unable to retrieve TRT ID Helper." );
    return sc;       
  } else {
    ATH_MSG_INFO( "retrieved m_TRTHelper " << m_TRTHelper ); 
  }


  sc = m_neighbourSvc.retrieve() ;
  if(StatusCode::SUCCESS!= sc ) {
    ATH_MSG_FATAL("Could not get TRTStrawNeighbourSvc !");
    return sc; 
  } else {
    ATH_MSG_INFO( "retrieved m_neighbourSvc " << m_neighbourSvc); 
  }


  sc = m_trtcaldbSvc.retrieve();
  if(StatusCode::SUCCESS!= sc ) {
    ATH_MSG_FATAL("Could not get TRTCalDbSvc !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "retrieved TRTCalDbSvc"  << m_trtcaldbSvc ); 
  }

  sc = m_TRTStrawSummarySvc.retrieve();
  if (StatusCode::SUCCESS!= sc ){ 
    ATH_MSG_ERROR ("Failed to retrieve StrawStatus Summary " << m_TRTStrawSummarySvc);
    ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
    return sc;  
  } else {
    if ( !m_TRTStrawSummarySvc.empty()) msg(MSG::INFO) << "Retrieved tool " << m_TRTStrawSummarySvc << endreq;
  }

  CHECK(m_driftFunctionTool.retrieve());

  return sc;
}

/////////////////////////////////////////////////////////////////////
//
//        Execute method: 
//
/////////////////////////////////////////////////////////////////////
StatusCode TRT_PrepDataToxAOD::execute() 
{
  const InDet::TRT_DriftCircleContainer* trtPrds = 0;     
  if( evtStore()->retrieve(trtPrds,m_driftcirclecontainer).isFailure() ) {
    ATH_MSG_ERROR("Cannot retrieve TRT PrepDataContainer " << m_driftcirclecontainer);
    return StatusCode::FAILURE;
  }


  // On ESD and AOD
  const PRD_MultiTruthCollection* prdmtColl = 0;
  if (m_useTruthInfo) ATH_CHECK(evtStore()->retrieve(prdmtColl, m_multiTruth));

  // On RDO
  const InDetSimDataCollection* sdoCollection = 0;
  if (m_useTruthInfo) ATH_CHECK(evtStore()->retrieve(sdoCollection, m_SDOcontainer));


  // Create the xAOD container and its auxiliary store:
  xAOD::TrackMeasurementValidationContainer* xaod = new xAOD::TrackMeasurementValidationContainer();
  CHECK( evtStore()->record( xaod, m_driftcirclecontainer ) );
  xAOD::TrackMeasurementValidationAuxContainer* aux = new xAOD::TrackMeasurementValidationAuxContainer();
  CHECK( evtStore()->record( aux, m_driftcirclecontainer + "Aux." ) );
  xaod->setStore( aux );
  
  std::vector<unsigned int>* offsets = new std::vector<unsigned int>( m_TRTHelper->straw_layer_hash_max() , 0 );
  CHECK( evtStore()->record( offsets, m_driftcirclecontainer + "Offsets" ) );
  
  InDet::TRT_DriftCircleContainer::const_iterator it = trtPrds->begin();
  InDet::TRT_DriftCircleContainer::const_iterator it_end = trtPrds->end();
  unsigned int counter(0);
  for( ; it!=it_end; ++it ) {

    //Fill Offset container
    if( m_TRTHelper->straw_layer_hash_max() <= (*it)->identifyHash() ) 
      ATH_MSG_ERROR("My assumption about the maximum size of the was wrong");
    (*offsets)[ (*it)->identifyHash() ] = counter;

    // skip empty collections
    if( (*it)->empty() ) continue;
    
    // loop over collection and convert to xAOD
    for( const auto& prd : **it ){
      
      ++counter;
      
      // create and add xAOD object
      xAOD::TrackMeasurementValidation* xprd = new xAOD::TrackMeasurementValidation();
      xaod->push_back(xprd);

      Identifier surfaceID = prd->identify();

      xprd->setIdentifier(surfaceID.get_compact());


      // Save ID info:
      xprd->auxdata<int>("bec")         =   m_TRTHelper->barrel_ec( surfaceID )     ;
      xprd->auxdata<int>("layer")       =   m_TRTHelper->layer_or_wheel(surfaceID ) ;  
      xprd->auxdata<int>("phi_module")  =   m_TRTHelper->phi_module( surfaceID )    ;
      xprd->auxdata<int>("strawlayer")  =   m_TRTHelper->straw_layer( surfaceID )   ;
      xprd->auxdata<int>("strawnumber") =   m_TRTHelper->straw( surfaceID )         ;
      int chip=0;
      int board=-1;
      m_neighbourSvc->getChip(surfaceID,chip);
      if(abs(m_TRTHelper->barrel_ec(surfaceID))<2)  
        board=m_neighbourSvc->chipToBoardBarrel(chip,m_TRTHelper->layer_or_wheel(surfaceID));
      else if (chip<12) 
        board=0;
      else {
        chip=chip-20;
        board=1;
      }
      xprd->auxdata<int>("board") =   board ; 
      xprd->auxdata<int>("chip")  =   chip  ;

      
      //Set Local Position
      const Amg::Vector2D& locpos = prd->localPosition();
      float locY(0.);
      float locX = locpos.x();
      if ( !(std::isinf(locpos.y()) || std::isnan(locpos.y())) ){
        if (locpos.y()>=1e-07)
          locY=locpos.y();
      } else { 
        locY = -9999.;
      }

      // Set local error matrix
      xprd->setLocalPosition(locX,locY); 
      
      const Amg::MatrixX& localCov = prd->localCovariance();
      if(localCov.size() == 1){
        xprd->setLocalPositionError( localCov(0,0), 0., 0. ); 
      } else if(localCov.size() == 2){
        xprd->setLocalPositionError( localCov(0,0), localCov(1,1), localCov(0,1) );     
      } else {
        xprd->setLocalPositionError(0.,0.,0.);
      }
   
            
      //Set Global position
      const Amg::Vector3D* gpos = prd->detectorElement()->surface(surfaceID).localToGlobal(prd->localPosition());
      if(gpos){
        xprd->setGlobalPosition(gpos->x(),gpos->y(),gpos->z());
        delete gpos;
      }


      //TRT hit bit word
      unsigned int word = prd->getWord();

      //TRTCond::RtRelation const *rtr = m_trtcaldbSvc->getRtRelation(surfaceID);
      double tot = prd->timeOverThreshold();
      bool isvalid=false;
      xprd->auxdata<float>("drifttime")  = prd->driftTime(isvalid)   ;
      xprd->auxdata<int>("status")      = isvalid;
      xprd->auxdata<float>("tot")        = tot ;
      xprd->auxdata<char>("isHT")        = prd->highLevel()    ;
      xprd->auxdata<float>("T0")         = m_trtcaldbSvc->getT0(surfaceID)   ; 


      // Save time info:
      xprd->auxdata<float>("leadingEdge")  = prd->rawDriftTime();

      //Drift Time corrections
      xprd->auxdata<float>("driftTimeToTCorrection") = m_driftFunctionTool->driftTimeToTCorrection(tot,surfaceID);
      if(prd->highLevel()) 
        xprd->auxdata<float>("driftTimeHTCorrection") = m_driftFunctionTool->driftTimeHTCorrection(surfaceID);
      else 
        xprd->auxdata<float>("driftTimeHTCorrection") = 0;
      
      
      //HT Bit patterens
      unsigned int theWord = word & 0x04020100; //HT bits mask
      char highThreshold = 0;
      //this is the agreed-upon binning for HT bit patterns...
      if (theWord == 0x04000000) //pattern 1
        highThreshold = 1;
      else if (theWord == 0x00020000) //pattern 2
        highThreshold = 2;
      else if (theWord == 0x00000100) //pattern 3
        highThreshold = 3;
      else if (theWord == 0x04020000) //pattern 4
        highThreshold = 4;
      else if (theWord == 0x00020100) //pattern 5
        highThreshold = 5;
      else if (theWord == 0x04000100) //pattern 6
        highThreshold = 6;
      else if (theWord == 0x04020100) //pattern 7
        highThreshold = 7;
      
      xprd->auxdata<char>("highThreshold") = highThreshold;
      
      //Full bit patternword from the TRT
      xprd->auxdata<unsigned int>("bitPattern") = word;
      
      char isArgonStraw = 0;
      if (!m_TRTStrawSummarySvc.empty()) {
        if (m_TRTStrawSummarySvc->getStatusHT(surfaceID) != TRTCond::StrawStatus::Good) {
          isArgonStraw = 1;
        }
      }
      xprd->auxdata<char>("isArgon")              = isArgonStraw            ;


      // Use the MultiTruth Collection to get a list of all true particle contributing to the DC
      if(prdmtColl){
        std::vector<int> barcodes;
        auto range = prdmtColl->equal_range(surfaceID);
        for (auto i = range.first; i != range.second; ++i) {
          barcodes.push_back( i->second.barcode() );
        }
        xprd->auxdata< std::vector<int> >("truth_barcode") = barcodes;
      }

      if( sdoCollection ){
        // find hit
        auto pos = sdoCollection->find(surfaceID);
        int sdo_word = -1000000;
        if( pos != sdoCollection->end() ) {
          sdo_word = pos->second.word();
        }           
        xprd->auxdata<int>("sdo_word")      = sdo_word;
      }

    }
  }
  ATH_MSG_DEBUG( " recorded TRT_PrepData obejcts: size " << xaod->size() );



  // Code to test that something was added to SG:
  StatusCode sc = StatusCode::SUCCESS;
  if (msgLvl(MSG::DEBUG)){
    const xAOD::TrackMeasurementValidationContainer* xaod2;
    sc = evtStore()->retrieve(xaod2, m_driftcirclecontainer);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG("Failed to retrieve " << m_driftcirclecontainer);
    }
    else   ATH_MSG_DEBUG("xAOD info retreived " << m_driftcirclecontainer << "\t" << xaod2->size());
 
    const xAOD::TrackMeasurementValidationAuxContainer* aux2;
    sc = evtStore()->retrieve(aux2, (m_driftcirclecontainer+ "Aux.")); 
    if (sc.isFailure()) {
      ATH_MSG_DEBUG("Failed to retrieve " << m_driftcirclecontainer<< "Aux.");
    }
    else   ATH_MSG_DEBUG("xAOD info retrieved " << m_driftcirclecontainer << "Aux. \t");
  }
 
  return sc; 
}

/////////////////////////////////////////////////////////////////////
//
//        Finalize method: 
//
/////////////////////////////////////////////////////////////////////
StatusCode TRT_PrepDataToxAOD::finalize()
{
  return StatusCode::SUCCESS;
}

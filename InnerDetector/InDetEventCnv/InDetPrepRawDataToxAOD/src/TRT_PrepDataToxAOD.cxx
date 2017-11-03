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

#define AUXDATA(OBJ, TYP, NAME) \
  static const SG::AuxElement::Accessor<TYP> acc_##NAME (#NAME);  acc_##NAME(*(OBJ))

/////////////////////////////////////////////////////////////////////
//
//         Constructor with parameters:
//
/////////////////////////////////////////////////////////////////////
TRT_PrepDataToxAOD::TRT_PrepDataToxAOD(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_driftFunctionTool("TRT_DriftFunctionTool", this),
  m_trtcaldbSvc("TRT_CalDbSvc", name),
  m_neighbourSvc("TRT_StrawNeighbourSvc", name),
  m_TRTStrawSummarySvc("InDetTRTStrawStatusSummarySvc",name),
  m_TRTHelper(0),
  m_trtman(0),
  m_firstEventWarnings(true)
{ 
  // --- Steering and configuration flags
  declareProperty("UseTruthInfo",          m_useTruthInfo=false);
  declareProperty("WriteSDOs",             m_writeSDOs = true);

  // --- Configuration keys
  declareProperty("DriftCircleContainer",  m_driftcirclecontainer="TRT_DriftCircles");
  declareProperty("MC_TRTUncompressedHit", m_SDOcontainer="TRT_SDO_Map");
  declareProperty("PRD_MultiTruth",        m_multiTruth="PRD_MultiTruthTRT");

  // --- Services and Tools
  declareProperty("TRTDriftFunctionTool",  m_driftFunctionTool);
  declareProperty("TRTCalDbSvc",           m_trtcaldbSvc);
  declareProperty("NeighbourSvc",          m_neighbourSvc);
  declareProperty("TRTStrawSummarySvc",    m_TRTStrawSummarySvc);

}

/////////////////////////////////////////////////////////////////////
//
//        Initialize method: 
//
/////////////////////////////////////////////////////////////////////
StatusCode TRT_PrepDataToxAOD::initialize()
{
  ATH_MSG_DEBUG("Initialize");

  // --- Retrieve services and tools
  CHECK ( detStore()->retrieve(m_TRTHelper, "TRT_ID") );

  CHECK ( detStore()->retrieve(m_trtman, "TRT") );

  CHECK ( m_neighbourSvc.retrieve() );

  CHECK ( m_trtcaldbSvc.retrieve() );

  CHECK ( m_TRTStrawSummarySvc.retrieve() );

  CHECK( m_driftFunctionTool.retrieve() );

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////
//
//        Execute method: 
//
/////////////////////////////////////////////////////////////////////
StatusCode TRT_PrepDataToxAOD::execute() 
{
  //This is needed for the algorithm. If not there, it fails
  const InDet::TRT_DriftCircleContainer* m_trtPrds = 0;
  if( evtStore()->retrieve(m_trtPrds,m_driftcirclecontainer).isFailure() ) {
    ATH_MSG_ERROR("Cannot retrieve TRT PrepDataContainer " << m_driftcirclecontainer);
    return StatusCode::FAILURE;
  }

  //This is optional for the algorithm. If not there, just print a one-time warning
  // On ESD 
  const PRD_MultiTruthCollection* m_prdmtColl = 0;
  if (m_useTruthInfo) {
    if ( evtStore()->contains<PRD_MultiTruthCollection>(m_multiTruth) ) {
      if ( evtStore()->retrieve(m_prdmtColl, m_multiTruth).isFailure() ) {
	  ATH_MSG_ERROR("ERROR in retrieving PRD MultiTruth collection although available (" << m_multiTruth << ").");
	  return StatusCode::FAILURE;
      }
    } else {
	if (m_firstEventWarnings) {
	  ATH_MSG_WARNING("PRD MultiTruth collection not available (" << m_multiTruth << "). Skipping this info although requested.");
	  m_prdmtColl = 0;
	}
    }
  }

  //This is optional for the algorithm. If not there, just print a one-time warning
  // On RDO
  const InDetSimDataCollection* m_sdoCollection = 0;
  if (m_useTruthInfo && m_writeSDOs) {
    if ( evtStore()->contains<InDetSimDataCollection>(m_SDOcontainer) ) {
      if ( evtStore()->retrieve(m_sdoCollection, m_SDOcontainer).isFailure() ) {
	ATH_MSG_ERROR("ERROR in retrieving SDO container despite being available. Collection = " << m_SDOcontainer);
	return StatusCode::FAILURE;
      }
    } else {
      if (m_firstEventWarnings) {
	ATH_MSG_WARNING("SDO Collection not available (" << m_SDOcontainer << "). Skipping this info although requested.");
	m_sdoCollection = 0;
      }
    }
  }


  // Create the xAOD container and its auxiliary store:
  xAOD::TrackMeasurementValidationContainer* xaod = new xAOD::TrackMeasurementValidationContainer();
  CHECK( evtStore()->record( xaod, m_driftcirclecontainer ) );
  xAOD::TrackMeasurementValidationAuxContainer* aux = new xAOD::TrackMeasurementValidationAuxContainer();
  CHECK( evtStore()->record( aux, m_driftcirclecontainer + "Aux." ) );
  xaod->setStore( aux );
  
  std::vector<unsigned int>* offsets = new std::vector<unsigned int>( m_TRTHelper->straw_layer_hash_max() , 0 );
  CHECK( evtStore()->record( offsets, m_driftcirclecontainer + "Offsets" ) );
  
  InDet::TRT_DriftCircleContainer::const_iterator it = m_trtPrds->begin();
  InDet::TRT_DriftCircleContainer::const_iterator it_end = m_trtPrds->end();
  unsigned int counter(0);
  for( ; it!=it_end; ++it ) {

    //Fill Offset container
    if( m_TRTHelper->straw_layer_hash_max() <= (*it)->identifyHash() ) 
      ATH_MSG_ERROR("My assumption about the maximum size of the hash was wrong");
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
      AUXDATA(xprd, int, bec)         =   m_TRTHelper->barrel_ec( surfaceID )     ;
      AUXDATA(xprd, int, layer)       =   m_TRTHelper->layer_or_wheel(surfaceID ) ;  
      AUXDATA(xprd, int, phi_module)  =   m_TRTHelper->phi_module( surfaceID )    ;
      AUXDATA(xprd, int, strawlayer)  =   m_TRTHelper->straw_layer( surfaceID )   ;
      AUXDATA(xprd, int, strawnumber) =   m_TRTHelper->straw( surfaceID )         ;
      const InDetDD::TRT_BaseElement* element = m_trtman->getElement(surfaceID);
      AUXDATA(xprd, float, strawphi)    =   element->center(surfaceID).phi();

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
      AUXDATA(xprd, int, TRTboard) =   board ; 
      AUXDATA(xprd, int, TRTchip)  =   chip  ;

      
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
        xprd->setLocalPositionError( sqrt(localCov(0,0)), 0., 0. ); 
      } else if(localCov.size() == 2){
        xprd->setLocalPositionError( sqrt(localCov(0,0)), sqrt(localCov(1,1)), sqrt(localCov(0,1)) );     
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
      AUXDATA(xprd, float, drifttime)  = prd->driftTime(isvalid)   ;
      AUXDATA(xprd, int, status)      = isvalid;
      AUXDATA(xprd, float, tot)        = tot ;
      AUXDATA(xprd, char, isHT)        = prd->highLevel()    ;
      AUXDATA(xprd, float, T0)         = m_trtcaldbSvc->getT0(surfaceID)   ; 


      // Save time info:
      AUXDATA(xprd, float, leadingEdge)  = prd->rawDriftTime();

      //Drift Time corrections
      AUXDATA(xprd, float, driftTimeToTCorrection) = m_driftFunctionTool->driftTimeToTCorrection(tot,surfaceID);
      if(prd->highLevel())  {
        AUXDATA(xprd, float, driftTimeHTCorrection) = m_driftFunctionTool->driftTimeHTCorrection(surfaceID);
      }
      else  {
        AUXDATA(xprd, float, driftTimeHTCorrection) = 0;
      }
      
      
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
      
      AUXDATA(xprd, char, highThreshold) = highThreshold;
      
      //Full bit patternword from the TRT
      AUXDATA(xprd, unsigned int, bitPattern) = word;
      
      char gas_type = kUnset;
      if (!m_TRTStrawSummarySvc.empty()) {
        int stat = m_TRTStrawSummarySvc->getStatusHT(surfaceID);
        
        if       ( stat==1 || stat==4 ) { gas_type = kArgon; }
        else if  ( stat==5 )            { gas_type = kKrypton; }
        else if  ( stat==2 || stat==3 ) { gas_type = kXenon; }
        else if  ( stat==6 )              gas_type = kEmAr;
        else if  ( stat==7 )              gas_type = kEmKr;
      }
      AUXDATA(xprd, char, gasType)              = gas_type;

      // Use the MultiTruth Collection to get a list of all true particle contributing to the DC
      if(m_prdmtColl){
        std::vector<int> barcodes;
        auto range = m_prdmtColl->equal_range(surfaceID);
        for (auto i = range.first; i != range.second; ++i) {
          barcodes.push_back( i->second.barcode() );
        }
        AUXDATA(xprd,  std::vector<int> , truth_barcode) = barcodes;
      }

      if( m_sdoCollection ){
        // find hit
        auto pos = m_sdoCollection->find(surfaceID);
        int sdo_word = -1000000;
        if( pos != m_sdoCollection->end() ) {
          sdo_word = pos->second.word();
        }           
        AUXDATA(xprd, int, sdo_word)      = sdo_word;
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

  // --- end of event. Disable one-time warnings
  m_firstEventWarnings = false;
 
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

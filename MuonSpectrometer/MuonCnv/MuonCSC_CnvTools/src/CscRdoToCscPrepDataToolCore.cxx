/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Ketevi A. Assamagan, Woochun Park
/// BNL, April 03 2005

/// algorithm to decode RDO into PrepRawData

#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"

#include "CscRdoToCscPrepDataToolCore.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "EventPrimitives/EventPrimitives.h" 

#include "GaudiKernel/ThreadLocalContext.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

static const InterfaceID IID_ICscRdoToCscPrepDataToolCore
("CscRdoToCscPrepDataToolCore",1,0);

const InterfaceID& CscRdoToCscPrepDataToolCore::interfaceID()
{ 
  return IID_ICscRdoToCscPrepDataToolCore; 
}

CscRdoToCscPrepDataToolCore::CscRdoToCscPrepDataToolCore
(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_cscCalibTool( "CscCalibTool/CscCalibTool", this),
    m_cscRdoDecoderTool ("Muon::CscRDO_Decoder/CscRDO_Decoder", this),
    m_cabling( "CSCcablingSvc" ,name),
    m_fullEventDone(false) {

  declareInterface<IMuonRdoToPrepDataTool>(this);
  declareProperty("CSCHashIdOffset",     m_cscOffset = 22000);
  declareProperty("DecodeData",          m_decodeData = true ); 
  // tools 
  declareProperty("CscCalibTool",        m_cscCalibTool );
  declareProperty("CscRdoDecoderTool",   m_cscRdoDecoderTool );
  // DataHandle
  declareProperty("RDOContainer",        m_rdoContainerKey = std::string("CSCRDO"),"CscRawDataContainer to retrieve");
  declareProperty("OutputCollection", 	 m_outputCollectionKey = std::string("CSC_Measurements"),"Muon::CscStripPrepDataContainer to record");
}  

StatusCode CscRdoToCscPrepDataToolCore::initialize(){
  ATH_CHECK(m_cscCalibTool.retrieve());
  ATH_CHECK(m_cscRdoDecoderTool.retrieve());
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_cabling.retrieve());
  // check if initializing of DataHandle objects success
  ATH_CHECK(m_rdoContainerKey.initialize());
  ATH_CHECK(m_outputCollectionKey.initialize());
  ATH_CHECK(m_muDetMgrKey.initialize());
  return StatusCode::SUCCESS;
}

//_________________________________________
StatusCode CscRdoToCscPrepDataToolCore::decode(std::vector<IdentifierHash>&, std::vector<IdentifierHash>&) {
  return StatusCode::FAILURE;
}

//*****************************************
//************** Process for the givenId EF Filter case...
/// This decode function is for single-thread running only
StatusCode CscRdoToCscPrepDataToolCore::decode ATLAS_NOT_THREAD_SAFE (const CscRawDataContainer* rdoContainer, IdentifierHash givenHashId, std::vector<IdentifierHash>& decodedIdhs) {  
  IdContext cscContext = m_idHelperSvc->cscIdHelper().module_context();

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
  
  // if CSC decoding is switched off stop here
  if( !m_decodeData ) {
    ATH_MSG_DEBUG ( "Stored empty container; Decoding CSC RDO into CSC PrepRawData is switched off" );
    return StatusCode::SUCCESS;
  }

  //These collections can be empty for the trigger
  if(!m_outputCollection || m_outputCollection->size()==0){
    ATH_MSG_DEBUG("Stored empty collection.");
    return StatusCode::SUCCESS;
  }

  // identifiers of collections already decoded and stored in the container will be skipped
  if (m_outputCollection->numberOfCollections() != 0) {
    decodedIdhs.push_back(givenHashId);
    ATH_MSG_DEBUG ("A collection already exists in the container for offline id hash. "
                   << (int) givenHashId);
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG ( "Decoding CSC RDO into CSC PrepRawData" );
  /// create the CSC RDO decoder

  //**********************************************
  // retrieve specific collection for the givenID
  uint16_t idColl = 0xffff;
  m_cabling->hash2CollectionId(givenHashId,idColl);
  const CscRawDataCollection * rawCollection = rdoContainer->indexFindPtr(idColl);
  if (nullptr ==  rawCollection) {
    // unsigned int coll_hash = idColl;  
    ATH_MSG_DEBUG ( "Specific CSC RDO collection retrieving failed for collection hash = " << idColl );
    return StatusCode::SUCCESS;
  }


  ATH_MSG_DEBUG ( "Retrieved " << rawCollection->size() << " CSC RDOs.");
  //************************************************
  Identifier oldId;
  CscStripPrepDataCollection * collection = 0;
  IdentifierHash cscHashId;

  unsigned int samplingTime = rawCollection->rate();
  unsigned int numSamples   = rawCollection->numSamples();
  bool samplingPhase        = rawCollection->samplingPhase();
  std::vector<float> charges;
  charges.reserve(4);
  std::vector<uint16_t> samples;
  samples.reserve(4);

  if ( int(samplingTime) != int(m_cscCalibTool->getSamplingTime())) {
    ATH_MSG_WARNING ( " CSC sampling time from Collection is NOT consistant to calibTool parameter!!!!!!! " );
  }
  
  /** for each Rdo, loop over RawData, converter RawData to PrepRawData
      retrieve/create PrepRawData collection, and insert PrepRawData into collection */
  CscRawDataCollection::const_iterator itD   =  rawCollection->begin(); 
  CscRawDataCollection::const_iterator itD_e =  rawCollection->end();
      
  for (; itD!=itD_e; ++itD) { 
    const CscRawData * data = (*itD);
    uint16_t width = data->width();
    uint16_t totalSamples = (data->samples()).size();
    
    Identifier stationId = m_cscRdoDecoderTool->stationIdentifier(data);
    uint32_t hashOffset = data->hashId();

    ATH_MSG_DEBUG ( " Size of online cluster in this RawData: " 
		    << " Width = " << width << " Samples = " << totalSamples
		    << " stationId : " << stationId << "  hashOffset : " << hashOffset);

    if (m_idHelperSvc->cscIdHelper().get_hash(stationId, cscHashId, &cscContext)) {
      ATH_MSG_WARNING ( "Unable to get CSC digiti collection hash id "
			<< "context begin_index = " << cscContext.begin_index()
			<< " context end_index  = " << cscContext.end_index()
			<< " the identifier is "     );
      stationId.show();
    }

    if (oldId != stationId) {
      auto it_coll = m_outputCollection->indexFindPtr(cscHashId);
      if (nullptr == it_coll) {
	CscStripPrepDataCollection * newCollection = new CscStripPrepDataCollection(cscHashId);
	newCollection->setIdentifier(stationId);
	collection = newCollection;
	if ( m_outputCollection->addCollection(newCollection, cscHashId).isFailure() )
	  ATH_MSG_WARNING ("Couldn't record CscStripPrepdataCollection with key="
			   << (unsigned int) cscHashId << " in StoreGate!" );
	decodedIdhs.push_back(cscHashId); //Record that this collection contains data
      } else {  // It won't be needed because we already skipped decoded one (should be checked it's true)

//Hack for transition to athenaMT classes
	CscStripPrepDataCollection * oldCollection = const_cast<CscStripPrepDataCollection*>( it_coll );
	collection = oldCollection;
	cscHashId = collection->identifyHash();
      }
      oldId = stationId;
    }
    
    for (unsigned int j=0; j<width; ++j) {
      const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(data, j);
      ATH_MSG_DEBUG ( "        LOOP over width  " << j <<  " " << channelId );

      const CscReadoutElement* descriptor = muDetMgr->getCscReadoutElement(channelId);
      //calculate local positions on the strip planes
      if ( !descriptor ) {
        ATH_MSG_WARNING ( "Invalid descriptor for " << m_idHelperSvc->cscIdHelper().show_to_string(channelId)
                          << " Skipping channel " );
        continue;
      } else if (!descriptor->containsId(channelId)) {
        ATH_MSG_WARNING ("Identifier from the cabling service <"
                         <<m_idHelperSvc->cscIdHelper().show_to_string(channelId)
                         <<"> inconsistent with the geometry of detector element <"
                         <<m_idHelperSvc->cscIdHelper().show_to_string(descriptor->identify())
                         <<">  =>>ignore this hit");
        continue;
      }

      float timeOfFirstSample = 0.0;
      bool extractSamples = data->samples(j, numSamples, samples);
      if (!extractSamples) {
        ATH_MSG_WARNING ( "Unable to extract samples for strip " << j 
                          << " Online Cluster width = " << width 
                          << " for number of Samples = " << numSamples 
                          << " continuing ..." ); 
        continue;
      }	  
      
      //      uint32_t stripHash = hashOffset+j;
      IdentifierHash stripHash;
      if (m_idHelperSvc->cscIdHelper().get_channel_hash(channelId, stripHash)) {
        ATH_MSG_WARNING ( "Unable to get CSC strip hash id");
        channelId.show();
      }
      
      bool adctocharge = m_cscCalibTool->adcToCharge(samples, stripHash, charges);
      if ( !adctocharge ) {
        ATH_MSG_WARNING ( " CSC conversion ADC to Charge failed " 
                          << "CSC PrepData not build ... skipping " );
        continue;
      }
      if ( samples.size() >=4 ) 
        ATH_MSG_DEBUG ( "ADC: " << m_idHelperSvc->cscIdHelper().show_to_string(channelId) 
                        << " " << samples[0] << " " << samples[1] << " " << samples[2] << " " << samples[3]
                        << " Charges: "  
                        << " " << charges[0] << " " << charges[1] << " " << charges[2] << " " << charges[3] ); 
      
      int measuresPhi    = m_idHelperSvc->cscIdHelper().measuresPhi(channelId);

      Amg::Vector2D localWirePos1( descriptor->xCoordinateInTrackingFrame(channelId ),0.);
      
      int chamberLayer   = m_idHelperSvc->cscIdHelper().chamberLayer(channelId);
      float stripWidth   = descriptor->cathodeReadoutPitch( chamberLayer, measuresPhi );
      double errPos      = stripWidth / sqrt(12.0);

      AmgSymMatrix(2) covariance;
      covariance.setIdentity();
      covariance *= errPos*errPos;
      Amg::MatrixX * errClusterPos = new Amg::MatrixX(covariance);

      /** new CscStripPrepRawData */
      CscStripPrepData * newPrepData = new CscStripPrepData(channelId,
                                                            cscHashId,
                                                            localWirePos1,
                                                            errClusterPos,
                                                            descriptor,
                                                            charges,
                                                            timeOfFirstSample,
                                                            samplingTime);
      
      if (samplingPhase) newPrepData->set_samplingPhase();
      newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
      collection->push_back(newPrepData);
      
    }
  }

  return StatusCode::SUCCESS;
}



//************** Process for all in case of Offline
/// This decode function is for single-thread running only
StatusCode CscRdoToCscPrepDataToolCore::decode ATLAS_NOT_THREAD_SAFE (const CscRawDataContainer* rdoContainer, std::vector<IdentifierHash>& decodedIdhs)
{
  
  typedef CscRawDataContainer::const_iterator collection_iterator;
  
  IdContext cscContext = m_idHelperSvc->cscIdHelper().module_context();
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
 
  // if CSC decoding is switched off stop here
  if( !m_decodeData ) {
    ATH_MSG_DEBUG ( "Stored empty container. "
                    << "Decoding CSC RDO into CSC PrepRawData is switched off" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG ( "Decoding CSC RDO into CSC PrepRawData" );

  collection_iterator rdoColl     = rdoContainer->begin(); 
  collection_iterator lastRdoColl = rdoContainer->end();
  std::vector<float> charges;
  charges.reserve(4);
  std::vector<uint16_t> samples;
  samples.reserve(4);

  Identifier oldId;
  CscStripPrepDataCollection * collection = 0;
  IdentifierHash cscHashId;
  for (; rdoColl!=lastRdoColl; ++rdoColl) {
    if ( (*rdoColl)->size() > 0 ) {
      ATH_MSG_DEBUG ( " Number of RawData in this rdo " << (*rdoColl)->size() );
      
      const  CscRawDataCollection* cscCollection = *rdoColl; 
      unsigned int samplingTime = cscCollection->rate();
      unsigned int numSamples   = cscCollection->numSamples();
      bool samplingPhase        = cscCollection->samplingPhase();

      if ( int(samplingTime) != int(m_cscCalibTool->getSamplingTime())) {
        ATH_MSG_WARNING ( " CSC sampling time from Collection is NOT consistant to calibTool parameter!!!!!!! " );
      }
      /** for each Rdo, loop over RawData, converter RawData to PrepRawData
          retrieve/create PrepRawData collection, and insert PrepRawData into collection */
      CscRawDataCollection::const_iterator itD   =  cscCollection->begin(); 
      CscRawDataCollection::const_iterator itD_e =  cscCollection->end();
      
      for (; itD!=itD_e; ++itD) { 
        const CscRawData * data = (*itD);
        uint16_t width = data->width();
        uint16_t totalSamples = (data->samples()).size();
        
        Identifier stationId = m_cscRdoDecoderTool->stationIdentifier(data);
        uint32_t hashOffset = data->hashId();

        ATH_MSG_DEBUG ( "DecodeAll*Size of online cluster in this RawData: " 
			<< " Width = " << width << " Samples = " << totalSamples
			<< " stationId : " << stationId << "  hashOffset : " << hashOffset);

	if (m_idHelperSvc->cscIdHelper().get_hash(stationId, cscHashId, &cscContext)) {
	  ATH_MSG_WARNING ( "Unable to get CSC digiti collection hash id "
			    << "context begin_index = " << cscContext.begin_index()
			    << " context end_index  = " << cscContext.end_index()
			    << " the identifier is "     );
	  stationId.show();
	}

	if (oldId != stationId) {
	  auto it_coll = m_outputCollection->indexFindPtr(cscHashId);
	  if (nullptr == it_coll) {
	    CscStripPrepDataCollection * newCollection = new CscStripPrepDataCollection(cscHashId);
	    newCollection->setIdentifier(stationId);
	    collection = newCollection;
	    if ( m_outputCollection->addCollection(newCollection, cscHashId).isFailure() )
	      ATH_MSG_WARNING( "Couldn't record CscStripPrepdataCollection with key=" << (unsigned int) cscHashId
			       << " in StoreGate!" );
	    decodedIdhs.push_back(cscHashId); //Record that this collection contains data
	    
	  } else {  
//Hack for transition to athenaMT classes
            CscStripPrepDataCollection * oldCollection = const_cast<CscStripPrepDataCollection*>( it_coll );

	    collection = oldCollection;
	    cscHashId = collection->identifyHash();
	    
	  }
	  oldId = stationId;
	}

        
        for (unsigned int j=0; j<width; ++j) {
          const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(data, j);
          ATH_MSG_DEBUG ( "DecodeAll**LOOP over width  " << j <<  " " << channelId );

          const CscReadoutElement* descriptor = muDetMgr->getCscReadoutElement(channelId);
          //calculate local positions on the strip planes
          if ( !descriptor ) {
            ATH_MSG_WARNING ( "Invalid descriptor for " << m_idHelperSvc->cscIdHelper().show_to_string(channelId)
                              << " Skipping channel " );
            continue;
          }
          else if (!descriptor->containsId(channelId)) {
            ATH_MSG_WARNING ("Identifier from the cabling service <"
                             <<m_idHelperSvc->cscIdHelper().show_to_string(channelId)
                             <<"> inconsistent with the geometry of detector element <"
                             <<m_idHelperSvc->cscIdHelper().show_to_string(descriptor->identify())
                             <<">  =>>ignore this hit");
            continue;
          }
          
          float timeOfFirstSample = 0.0;
          bool extractSamples = data->samples(j, numSamples, samples);
          if (!extractSamples) {
            ATH_MSG_WARNING ( "Unable to extract samples for strip " << j 
                              << " Online Cluster width = " << width 
                              << " for number of Samples = " << numSamples 
                              << " continuing ..." ); 
            continue;
          }	 
          
          IdentifierHash stripHash;
          if (m_idHelperSvc->cscIdHelper().get_channel_hash(channelId, stripHash)) {
            ATH_MSG_WARNING ( "Unable to get CSC strip hash id");
            channelId.show();
          }


          Identifier channelIdFromHash;
          m_idHelperSvc->cscIdHelper().get_id(stripHash, channelIdFromHash, &cscContext);
          
          
          bool adctocharge = m_cscCalibTool->adcToCharge(samples, stripHash, charges);
          if ( !adctocharge ) {
            ATH_MSG_WARNING ( " CSC conversion ADC to Charge failed " 
                              << "CSC PrepData not build ... skipping " );
            continue;
          }
          if ( samples.size() >=4 ) 
            ATH_MSG_DEBUG ( "DecodeAll*** ADC: " << m_idHelperSvc->cscIdHelper().show_to_string(channelId) << " "
                            << (int) stripHash << " "
                            << m_idHelperSvc->cscIdHelper().show_to_string(channelIdFromHash) 
                            << " " << samples[0] << " " << samples[1] << " " << samples[2] << " " << samples[3]
                            << " Charges: "  
                            << " " << charges[0] << " " << charges[1] << " " << charges[2] << " " << charges[3] ); 
          if (m_idHelperSvc->cscIdHelper().get_hash(stationId, cscHashId, &cscContext)) {
            ATH_MSG_WARNING ( "Unable to get CSC hash id from CSC RDO collection "
                              << "context begin_index = " << cscContext.begin_index()
                              << " context end_index  = " << cscContext.end_index() 
                              << " the identifier is "      );
            stationId.show();
          }
                    
          // Check if this strip is already decoded.. Then we don't have to decode it again
          bool IsThisStripDecoded =0; 
          for ( CscStripPrepDataCollection::const_iterator idig=collection->begin();
                idig!=collection->end(); ++idig ) {
            const CscStripPrepData& dig = **idig;
            Identifier did = dig.identify();
            if (did ==channelId) {
              IsThisStripDecoded =1;
              break;
            }
          }
          if (IsThisStripDecoded) continue;

	  int measuresPhi    = m_idHelperSvc->cscIdHelper().measuresPhi(channelId);

          Amg::Vector2D localWirePos1( descriptor->xCoordinateInTrackingFrame(channelId ),0.);
 
          int chamberLayer   = m_idHelperSvc->cscIdHelper().chamberLayer(channelId);
          float stripWidth   = descriptor->cathodeReadoutPitch( chamberLayer, measuresPhi );
          double errPos      = stripWidth / sqrt(12.0);
          
	  AmgSymMatrix(2) covariance;
	  covariance.setIdentity();
	  covariance *= errPos*errPos;
	  Amg::MatrixX * errClusterPos = new Amg::MatrixX(covariance);

          /** new CscPrepRawData */
          CscStripPrepData * newPrepData = new CscStripPrepData(  channelId,
                                                                  cscHashId,
                                                                  localWirePos1,
                                                                  errClusterPos,
                                                                  descriptor,
                                                                  charges,
                                                                  timeOfFirstSample,
                                                                  samplingTime);
          
          if (samplingPhase) newPrepData->set_samplingPhase();
          newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
          collection->push_back(newPrepData);
          
        }
      }
    }
  }
  return  StatusCode::SUCCESS;
}


void CscRdoToCscPrepDataToolCore::printPrepData()
{
  ATH_MSG_INFO ( "***************************************************************" );
  ATH_MSG_INFO ( "****** Listing Csc(Strip)PrepData collections content *********" );

  if (m_outputCollection->size() <= 0)
    ATH_MSG_INFO ( "No Csc(Strip)PrepRawData collections found" );
  else {
    ATH_MSG_INFO ( "Number of Csc(Strip)PrepRawData collections found in this event is "
		   << m_outputCollection->size() );
    
    int ict = 0;
    int ncoll = 0;
    ATH_MSG_INFO ( "-------------------------------------------------------------" );
    for (IdentifiableContainer<Muon::CscStripPrepDataCollection>::const_iterator
	   icscColl = m_outputCollection->begin();
         icscColl!=m_outputCollection->end(); ++icscColl ) {

      const Muon::CscStripPrepDataCollection* cscColl = *icscColl;
        
      if ( cscColl->size() <= 0 ) continue;

      ATH_MSG_INFO ("PrepData Collection ID " << m_idHelperSvc->cscIdHelper().show_to_string(cscColl->identify())
                    <<" with size = "<<cscColl->size());
      CscStripPrepDataCollection::const_iterator it_cscStripPrepData;
      int icc = 0;
      int iccphi = 0;
      int icceta = 0;
      for (it_cscStripPrepData=cscColl->begin(); it_cscStripPrepData != cscColl->end(); ++it_cscStripPrepData) {
        icc++;
        ict++;
        if (m_idHelperSvc->cscIdHelper().measuresPhi((*it_cscStripPrepData)->identify()))
          iccphi++;
        else
          icceta++;
        
        ATH_MSG_INFO ( ict <<" in this coll. " << icc << " prepData id = "
                       << m_idHelperSvc->cscIdHelper().show_to_string((*it_cscStripPrepData)->identify()) );
      }
      ncoll++;
      ATH_MSG_INFO ( "*** Collection " << ncoll << " Summary: "
                     << iccphi <<" phi hits / " << icceta << " eta hits " );
      ATH_MSG_INFO ( "-------------------------------------------------------------" );

    }
  }
}  


void CscRdoToCscPrepDataToolCore::printInputRdo() {
  return;
}
  

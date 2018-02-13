/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Raw2DigiTool.h"
#include "AFP_RawEv/AFP_RawContainer.h"
#include "AFP_RawEv/AFP_ROBID.h"
#include "xAODForward/AFPStationID.h"

#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHitAuxContainer.h"

#include "xAODForward/AFPToFHit.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFHitAuxContainer.h"

#include <vector>

AFP_Raw2DigiTool::AFP_Raw2DigiTool(const std::string &type,
				   const std::string &name,
				   const IInterface *parent)
  : AthAlgTool(type, name, parent),
    m_totToChargeTransformation ("totToChargeTransformation", "1909 + x*363 + x*x*141")
{
  declareInterface<IAFP_Raw2DigiTool>(this);

  declareProperty( "rawDataContainerName", m_rawDataContainerName = "AFP_RawData");
  declareProperty( "AFPSiHitsContainerName", m_AFPSiHitsContainerName = "AFPSiHitContainer" );
  declareProperty( "AFPHitsContainerNameToF", m_AFPHitsContainerNameToF = "AFPToFHitContainer" );
}		

AFP_Raw2DigiTool::~AFP_Raw2DigiTool() {}

StatusCode AFP_Raw2DigiTool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode AFP_Raw2DigiTool::recoAll()
{
  CHECK( recoSiHits() );
  CHECK( recoToFHits() );

  return StatusCode::SUCCESS;
}

StatusCode AFP_Raw2DigiTool::recoSiHits()
{
  ATH_MSG_DEBUG("AFP_Raw2DigiTool recoSiHits ");

  // create output containers
  xAOD::AFPSiHitContainer* siHitContainer = new xAOD::AFPSiHitContainer();
  CHECK( evtStore()->record(siHitContainer, m_AFPSiHitsContainerName) );
  xAOD::AFPSiHitAuxContainer* siHitAuxContainer = new xAOD::AFPSiHitAuxContainer();
  CHECK( evtStore()->record(siHitAuxContainer, m_AFPSiHitsContainerName + "Aux.") );
  siHitContainer->setStore(siHitAuxContainer);

  // retrieve raw data
  const AFP_RawContainer *container = nullptr;
  if (evtStore()->retrieve(container, m_rawDataContainerName).isFailure()) {
    ATH_MSG_WARNING("AFP_Raw2DigiTool: Could not find raw data container");
    return StatusCode::SUCCESS;
  }
  else
    ATH_MSG_DEBUG("AFP_Raw2DigiTool: Raw data container retrieved");

  for (const AFP_SiRawCollection& collection: container->collectionsSi())
    for (const AFP_SiRawData& data : collection.dataRecords())
      newXAODHitSi (siHitContainer, collection, data);

  return StatusCode::SUCCESS;
}

StatusCode AFP_Raw2DigiTool::recoToFHits()
{
  ATH_MSG_DEBUG("AFP_Raw2DigiTool recoToFHits ");

  // create output containers
  xAOD::AFPToFHitContainer* tofHitContainer = new xAOD::AFPToFHitContainer();
  CHECK( evtStore()->record(tofHitContainer, m_AFPHitsContainerNameToF) );
  xAOD::AFPToFHitAuxContainer* tofHitAuxContainer = new xAOD::AFPToFHitAuxContainer();
  CHECK( evtStore()->record(tofHitAuxContainer, m_AFPHitsContainerNameToF + "Aux.") );
  tofHitContainer->setStore(tofHitAuxContainer);

  // retrieve raw data
  const AFP_RawContainer *container = nullptr;
  if (evtStore()->retrieve(container, m_rawDataContainerName).isFailure()) {
    ATH_MSG_WARNING("AFP_Raw2DigiTool: Could not find raw data container");
    return StatusCode::SUCCESS;
  }
  else
    ATH_MSG_DEBUG("AFP_Raw2DigiTool: Raw data container retrieved");
  
  for (const AFP_ToFRawCollection& collection: container->collectionsToF())
    for (const AFP_ToFRawData& data : collection.dataRecords())
      if (data.hitDiscConfig() == 3 && data.header() == 2) 
	newXAODHitToF (tofHitContainer, collection, data);

  return StatusCode::SUCCESS;
}


StatusCode AFP_Raw2DigiTool::finalize(){
  ATH_MSG_INFO("Finalizing " << name() << "...");


  return StatusCode::SUCCESS;
}


void AFP_Raw2DigiTool::newXAODHitToF (xAOD::AFPToFHitContainer* tofHitContainer, const AFP_ToFRawCollection& collection, const AFP_ToFRawData& data) const
{
  xAOD::AFPToFHit* xAODToFHit = new xAOD::AFPToFHit();
  tofHitContainer->push_back(xAODToFHit);

  const uint32_t hitLink = data.link();
  const unsigned int robID = collection.robId();
  xAODToFHit->setTime(data.time()*s_timeConversionFactor);
  xAODToFHit->setPulseLength(data.pulseLength()*s_pulseLengthFactor); 
  xAODToFHit->setHptdcChannel(data.channel());

  // set station ID
  if (robID == AFP_ROBID::sideC)
    xAODToFHit->setStationID(xAOD::AFPStationID::farC);
  else if (robID == AFP_ROBID::sideA)
    xAODToFHit->setStationID(xAOD::AFPStationID::farA);
  else
    xAODToFHit->setStationID(-1);

  // set hptdcID
  if (hitLink == 12)
    xAODToFHit->setHptdcID(2);
  else if (hitLink == 13)
    xAODToFHit->setHptdcID(1);
  else
    xAODToFHit->setHptdcID(-1);

  // set barID
  setBarAndTrainID(xAODToFHit);
}

void AFP_Raw2DigiTool::newXAODHitSi (xAOD::AFPSiHitContainer* siHitContainer, const AFP_SiRawCollection& collection, const AFP_SiRawData& data) const
{
  xAOD::AFPSiHit* xAODSiHit = new xAOD::AFPSiHit();
  siHitContainer->push_back(xAODSiHit);

  const unsigned int hitLink = data.link();
  const unsigned int robID = collection.robId();
  // decoding according to Outlink numbers in: https://twiki.cern.ch/twiki/bin/view/Atlas/AFPRunning#Tracker
  if (hitLink <=3 ) {	// not checking if larger or equal 0, because this is uint, which is always positive
    // set near station ID selecting side based on ROB
    if (robID == AFP_ROBID::sideA)
      xAODSiHit->setStationID(xAOD::AFPStationID::nearA);
    else if (robID == AFP_ROBID::sideC)
      xAODSiHit->setStationID(xAOD::AFPStationID::nearC);
    else  {
      ATH_MSG_WARNING("Unrecognised robID: in dec="<<std::dec<<robID<<",  in hex=0x"<<std::hex<<robID<<std::dec);
      xAODSiHit->setStationID(-1);
    }
  } else if (hitLink >= 8 && hitLink <= 11) {
    // set far station ID selecting side based on ROB
    if (robID == AFP_ROBID::sideA)
      xAODSiHit->setStationID(xAOD::AFPStationID::farA);
    else if (robID == AFP_ROBID::sideC)
      xAODSiHit->setStationID(xAOD::AFPStationID::farC);
    else  {
      ATH_MSG_WARNING("Unrecognised robID: in dec="<<std::dec<<robID<<",  in hex=0x"<<std::hex<<robID<<std::dec);
      xAODSiHit->setStationID(-1);
    }
  } else {
    ATH_MSG_WARNING("Unrecognised value in hitLink = " << hitLink);
    xAODSiHit->setStationID(-1);
  } // end of switch
  
  ATH_MSG_DEBUG("Filled information about station and detector in xAODSiHit object");

  xAODSiHit->setPixelLayerID( hitLink%4 );
  xAODSiHit->setPixelColIDChip( data.column() );
  xAODSiHit->setPixelRowIDChip( data.row() );

  const unsigned int ToT_value = decodeTimeOverThresholdSi( data.timeOverThreshold(), data.hitDiscConfig() );
  xAODSiHit->setTimeOverThreshold(ToT_value);
  xAODSiHit->setDepositedCharge( m_totToChargeTransformation(ToT_value) );

  ATH_MSG_DEBUG("Filled xAOD::AFPSiHit");
}

void AFP_Raw2DigiTool::setBarAndTrainID(xAOD::AFPToFHit* tofHit) const
{
  // mapping is implemented according to https://twiki.cern.ch/twiki/bin/view/Atlas/AFPHPTDC#Channel_Mapping

  const int hptdcChannel = tofHit->hptdcChannel();
  const int hptdcID = tofHit->hptdcID();
  
  if (hptdcID == 1) {
    switch (hptdcChannel) {
    case 0:
      tofHit->setTrainID(0);
      tofHit->setBarInTrainID(0);
      break;
    case 2:
      tofHit->setTrainID(1);
      tofHit->setBarInTrainID(2);
      break;
    case 3:
      tofHit->setTrainID(0);
      tofHit->setBarInTrainID(3);
      break;
    case 5: 
      tofHit->setTrainID(1);
      tofHit->setBarInTrainID(1);
      break;
    case 6:
      tofHit->setTrainID(0);
      tofHit->setBarInTrainID(2);
      break;
    case 8:
      tofHit->setTrainID(1);
      tofHit->setBarInTrainID(0);
      break;
    case 9:
      tofHit->setTrainID(0);
      tofHit->setBarInTrainID(1);
      break;
    case 11:
      tofHit->setTrainID(1);
      tofHit->setBarInTrainID(3);
      break;
    }
  }
  else if (hptdcID == 2) {
    switch (hptdcChannel) {
    case 0:
      tofHit->setTrainID(2);
      tofHit->setBarInTrainID(0);
      break;
    case 2:
      tofHit->setTrainID(3);
      tofHit->setBarInTrainID(2);
      break;
    case 3:
      tofHit->setTrainID(2);
      tofHit->setBarInTrainID(3);
      break;
    case 5:
      tofHit->setTrainID(3);
      tofHit->setBarInTrainID(1);
      break;
    case 6:
      tofHit->setTrainID(2);
      tofHit->setBarInTrainID(2);
      break;
    case 8:
      tofHit->setTrainID(3);
      tofHit->setBarInTrainID(0);
      break;
    case 9:
      tofHit->setTrainID(2);
      tofHit->setBarInTrainID(1);
      break;
    case 11:
      tofHit->setTrainID(3);
      tofHit->setBarInTrainID(3);
      break;
    }
  }
}

unsigned int AFP_Raw2DigiTool::decodeTimeOverThresholdSi (const unsigned int input, const unsigned int discConfig) const
{
  if (input <= 13)
    return input + discConfig + 1;
  else if (input == 14) 
    return discConfig;
  else
    return 0;
}

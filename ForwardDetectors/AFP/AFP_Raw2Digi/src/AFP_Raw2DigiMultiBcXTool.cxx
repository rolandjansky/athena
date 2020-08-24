/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Raw2DigiMultiBcXTool.h"
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
#include <set>
#include <map>
#include <sstream>
#include <algorithm>

AFP_Raw2DigiMultiBcXTool::AFP_Raw2DigiMultiBcXTool(const std::string &type,
				   const std::string &name,
				   const IInterface *parent)
  : base_class(type, name, parent),
    m_totToChargeTransformation ("totToChargeTransformation", "1909 + x*363 + x*x*141"),
    m_firstEventSi (true),
    m_firstEventToF (true)
{
  declareProperty( "rawDataContainerName", m_rawDataContainerName = "AFP_RawData");
  declareProperty( "AFPSiHitsContainerName", m_AFPSiHitsContainerName = "AFPSiHitContainer" );
  declareProperty( "AFPHitsContainerNameToF", m_AFPHitsContainerNameToF = "AFPToFHitContainer" );
}		

AFP_Raw2DigiMultiBcXTool::~AFP_Raw2DigiMultiBcXTool() {}

StatusCode AFP_Raw2DigiMultiBcXTool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode AFP_Raw2DigiMultiBcXTool::recoAll()
{
  ATH_CHECK( recoSiHits() );
  ATH_CHECK( recoToFHits() );
  
  return StatusCode::SUCCESS;
}

StatusCode AFP_Raw2DigiMultiBcXTool::recoSiHits()
{
  // retrieve raw data
  const AFP_RawContainer *container = nullptr;
  if (evtStore()->retrieve(container, m_rawDataContainerName).isFailure()) {
    ATH_MSG_WARNING("AFP_Raw2DigiMultiBcXTool: Could not find raw data container");
    return StatusCode::SUCCESS;
  }
  else
    ATH_MSG_DEBUG("AFP_Raw2DigiMultiBcXTool: Raw data container retrieved");

  if (m_firstEventSi) {
    createContainersNames (container->collectionsSi(), m_AFPSiHitsContainerName, m_containersNameSi);
    m_firstEventSi = false;
  } // end if (m_firstEventSi)
  
  // --- create containers ---
  std::vector<xAOD::AFPSiHitContainer*> hitContainers;
  hitContainers.reserve(m_containersNameSi.size());
  for (const std::string& name : m_containersNameSi) {
    xAOD::AFPSiHitContainer* container = new xAOD::AFPSiHitContainer();
    ATH_CHECK( evtStore()->record(container, name) );
    xAOD::AFPSiHitAuxContainer* auxContainer = new xAOD::AFPSiHitAuxContainer();
    ATH_CHECK( evtStore()->record(auxContainer, name + "Aux.") );
    container->setStore(auxContainer);
    hitContainers.push_back(container);
  }
  
  //  --- fill xAOD containers ---
  if (container->collectionsSi().size() < 3) {
    ATH_MSG_WARNING("Less than 3 collections in container->collectionsSi(). Too few. Skipping event.");
    return StatusCode::SUCCESS;
  }

  fillXAODWithCollections(container->collectionsSi(), hitContainers, &AFP_Raw2DigiMultiBcXTool::siCollectionToXAOD);
  return StatusCode::SUCCESS;
}


void AFP_Raw2DigiMultiBcXTool::siCollectionToXAOD (const AFP_SiRawCollection& collection, const int diff, std::vector<xAOD::AFPSiHitContainer*>& hitContainers) const
{
  try {
    xAOD::AFPSiHitContainer* xAODContainer = hitContainers.at(diff);
    for (const AFP_SiRawData& data : collection.dataRecords())
      newXAODHitSi (xAODContainer, collection, data);
  }
  catch (const std::out_of_range& except) {
    ATH_MSG_WARNING ("Skipping collection with bcId="<<collection.bcId()<<", because bcId diff equals "<<diff<<" and is larger than vector size "<<hitContainers.size());
  }
}


StatusCode AFP_Raw2DigiMultiBcXTool::recoToFHits()
{
  ATH_MSG_DEBUG("AFP_Raw2DigiMultiBcXTool recoToFHits ");

  // retrieve raw data
  const AFP_RawContainer *container = nullptr;
  if (evtStore()->retrieve(container, m_rawDataContainerName).isFailure()) {
    ATH_MSG_WARNING("AFP_Raw2DigiMultiBcXTool: Could not find raw data container");
    return StatusCode::SUCCESS;
  }
  else
    ATH_MSG_DEBUG("AFP_Raw2DigiMultiBcXTool: Raw data container retrieved");

  if (m_firstEventToF) {
    createContainersNames (container->collectionsToF(), m_AFPHitsContainerNameToF, m_containersNameToF);
    m_firstEventToF = false;
  } // end if (m_firstEventToF)
  
  // --- create containers ---
  std::vector<xAOD::AFPToFHitContainer*> hitContainers;
  hitContainers.reserve(m_containersNameToF.size());
  for (const std::string& name : m_containersNameToF) {
    xAOD::AFPToFHitContainer* container = new xAOD::AFPToFHitContainer();
    ATH_CHECK( evtStore()->record(container, name) );
    xAOD::AFPToFHitAuxContainer* auxContainer = new xAOD::AFPToFHitAuxContainer();
    ATH_CHECK( evtStore()->record(auxContainer, name + "Aux.") );
    container->setStore(auxContainer);
    hitContainers.push_back(container);
  }
  
  //  --- fill xAOD containers ---
  if (container->collectionsToF().size() < 3) {
    ATH_MSG_WARNING("Less than 3 collections in container->collectionsToF(). Too few. Skipping event.");
    return StatusCode::SUCCESS;
  }

  fillXAODWithCollections(container->collectionsToF(), hitContainers, &AFP_Raw2DigiMultiBcXTool::tofCollectionToXAOD);  
  return StatusCode::SUCCESS;
}

void AFP_Raw2DigiMultiBcXTool::tofCollectionToXAOD (const AFP_ToFRawCollection& collection, const int diff, std::vector<xAOD::AFPToFHitContainer*>& hitContainers) const
{
  try {
    xAOD::AFPToFHitContainer* xAODContainer = hitContainers.at(diff);
    for (const AFP_ToFRawData& data : collection.dataRecords())
      if (data.hitDiscConfig() == 3 && data.header() == 2) 
	newXAODHitToF (xAODContainer, collection, data);
  }
  catch (const std::out_of_range& except) {
    ATH_MSG_WARNING ("Skipping collection with bcId="<<collection.bcId()<<", because bcId diff equals "<<diff<<" and is larger than vector size "<<hitContainers.size());
  }
}


StatusCode AFP_Raw2DigiMultiBcXTool::finalize(){
  ATH_MSG_INFO("Finalizing " << name() << "...");


  return StatusCode::SUCCESS;
}


void AFP_Raw2DigiMultiBcXTool::newXAODHitToF (xAOD::AFPToFHitContainer* tofHitContainer, const AFP_ToFRawCollection& collection, const AFP_ToFRawData& data) const
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

void AFP_Raw2DigiMultiBcXTool::newXAODHitSi (xAOD::AFPSiHitContainer* siHitContainer, const AFP_SiRawCollection& collection, const AFP_SiRawData& data) const
{
  if (siHitContainer != nullptr) {
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
  else
    ATH_MSG_WARNING("Empty xAOD continer passed. Not creating xAOD object.");
}

void AFP_Raw2DigiMultiBcXTool::setBarAndTrainID(xAOD::AFPToFHit* tofHit) const
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
    default:
      ATH_MSG_WARNING("Unrecognised hptdcChannel: "<<hptdcChannel);
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
    default:
      ATH_MSG_WARNING("Unrecognised hptdcChannel: "<<hptdcChannel);
    }
  }
}

unsigned int AFP_Raw2DigiMultiBcXTool::decodeTimeOverThresholdSi (const unsigned int input, const unsigned int discConfig) const
{
  if (input <= 13)
    return input + discConfig + 1;
  else if (input == 14) 
    return discConfig;
  else
    return 0;
}

unsigned int AFP_Raw2DigiMultiBcXTool::findFirstBcId (const std::set<unsigned int>& allBcIds) const
{
  // By default assume easy situation we are not at the edge of bcId values
  unsigned int firstBcID = *allBcIds.begin();

  std::set<unsigned int>::const_iterator last = allBcIds.end();
  last--;			// go one back, because end() is past last element
  if ( (*allBcIds.begin()) == 0 &&  (*last) == m_maxBcId) {
    // we are unlucky values at the edge of bcId are used. Go one by
    // one starting from the smallest and find when there is a gap in
    // bcIds and return the first one after the gap
    std::set<unsigned int>::const_iterator iter = allBcIds.begin();
    unsigned int previous = *iter;
    while ( (++iter) != allBcIds.end()) {
      if ((*iter) - 1 == previous)
	previous = *iter;
      else
	break;
    }

    if (iter != allBcIds.end())
      firstBcID = *iter;
    else
      firstBcID = *allBcIds.begin();
  }
  
  return firstBcID;
}

void AFP_Raw2DigiMultiBcXTool::removeNotContinues (std::set<unsigned int>& allBcIds) const
{
  std::set<unsigned int>::iterator iter = allBcIds.begin();
  while ( (iter) != allBcIds.end()) {
    int neighbours = 0;
    if (iter != allBcIds.begin()) {
      std::set<unsigned int>::const_iterator previous = iter;
      --previous;
      if (*previous + 1 == *iter)
	++neighbours;
    }
    else {
      std::set<unsigned int>::const_iterator last = allBcIds.end();
      last--;			// go one back, because end() is past last element
      if (*iter == 0 && *last == m_maxBcId)
	++neighbours;
    }

    std::set<unsigned int>::const_iterator next = iter;
    ++next;
    if (next != allBcIds.end()) {
      if (*next - 1 == *iter)
	++neighbours;
    }
    else 
      if (*allBcIds.begin() == 0 && *iter == m_maxBcId)
	++neighbours;
    
    if (neighbours == 0) {
      std::set<unsigned int>::iterator toDel = iter;
      ++iter;
      allBcIds.erase(toDel);
    }
    else 
      ++iter;
  }
}

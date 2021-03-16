/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonCablingData/MdtRODMap.h"

#include "MuonCablingData/MdtMezzanineType.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include<cmath>

MuonMDT_CablingMap::MuonMDT_CablingMap() :
  MdtMapBase<MdtSubdetectorMap>(0,"MdtSubdetectorMap")
{ 
  m_chamberToROD = std::make_unique<ChamberToRODMap>();
  m_RODToChamber = std::make_unique<RODToChamberMap>();
  m_listOfROD = std::make_unique<ListOfROD>();
  m_listOfMezzanineTypes = std::make_unique<MezzanineTypes>();

  // initialize the message service

  // retrieve the MdtIdHelper
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StoreGateSvc* detStore= nullptr;
  StatusCode sc = svcLocator->service("DetectorStore",detStore);
  if (sc != StatusCode::SUCCESS) {
    *m_log << MSG::ERROR << "Could not find the detctor store" << endmsg;
  }
  sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc != StatusCode::SUCCESS) {
    *m_log << MSG::ERROR << "Could not retrieve the MdtIdHelper" << endmsg;
  }
  
}

MuonMDT_CablingMap::~MuonMDT_CablingMap() = default;

// cleanup function
bool MuonMDT_CablingMap::cleanup()
{
  
  return true;
}

// add a CSM map to this ROD
bool MuonMDT_CablingMap::setSubdetectorMap(uint8_t subdetectorId, MdtSubdetectorMap* subdetectorMap)
{ 
  bool subdetectorAdded = addItem(subdetectorId,subdetectorMap);

  if (!subdetectorAdded) {
    *m_log << MSG::ERROR << "Could not add subdetector " << MSG::hex 
	   << subdetectorId << MSG::dec << " to the MDT cabling map" << endmsg;
  }

  return subdetectorAdded;
}


/** add a new line from the database, describing a mezzanine type */
bool MuonMDT_CablingMap::addMezzanineLine(const int type, const int layer,
					  const int sequence)
{
  bool added = true;

  if (m_debug) {
    *m_log << MSG::VERBOSE << "Now in MuonMDT_CablingMap::addMezzanineLine" << endmsg;
  }

  MdtMezzanineType* mezType;
  
  MezzanineTypes::const_iterator it = m_listOfMezzanineTypes->find(type);

  // check if the type already exists, if yes, retrieve it
  bool isNewMezzanine;
  if (it != m_listOfMezzanineTypes->end()) {
    // if the attempt is to initialize all layers of a mezzanine already
    // initialized, return an error message
    if (!layer) {
      *m_log << MSG::ERROR << "The mezzanine type " << type 
	     << "has been already initialized" << endmsg;
      return false;
    }
    mezType = it->second.get();
    isNewMezzanine = false;
  }
  // if it does not exist, create the new type
  else {
    if (m_debug) {
      *m_log << MSG::VERBOSE << "Creating a new mezzanine type: " << type << endmsg;
    }
    mezType = new MdtMezzanineType(type);
    // insert the new mezzanine in the map
    m_listOfMezzanineTypes->insert(MezzanineTypes::value_type(type,mezType));
    isNewMezzanine=true;
  }
  
  int ntubes = 0;
  int number = sequence;
  int nOfLayers;
  int newseq[8];
  // now decode the sequence, up to 8 tubes per sequence
  int tube = number%10;

  while ( tube!=0 ) {
    // add the tube to the tube sequence
    if (ntubes>7) {
      *m_log << MSG::ERROR << "More than 8 tubes in a layer, not possible !" 
	     << endmsg;
      return false;
    }
    if (m_debug) {
      *m_log << MSG::VERBOSE << "Adding tube number: " << tube << " to the layer " 
	     << layer << " of mezzanine type " << type << endmsg;
    }
    
    newseq[ntubes]=tube;

    ++ntubes;
    number = (number-tube)/10;
    //    tube = (int) (((number/10.)-((double)((int)(number/10.))))*10.);    

    tube = number%10;
  }

  if (ntubes!=8 && ntubes!=6) {
    *m_log << MSG::ERROR << "in type " << type 
	   << ": number of tubes per mezzanine layer can be only 6 or 8 ! what are you doing ???";
    return false;
  }
  else {
    nOfLayers = 24/ntubes;
    if (layer>nOfLayers) {
      *m_log << MSG::ERROR << "The maximum number of layers for this mezzanine is: "
	     << nOfLayers << " so you can't initialize layer: " 
	     << layer << endmsg;
      return false;
    }

  }

  if (m_debug) {
    *m_log << MSG::VERBOSE << "Found " << ntubes << " tubes in layer " << layer << endmsg;
    *m_log << MSG::VERBOSE << "This is a " << nOfLayers 
	   << " layers mezzanine - OK, OK..." << endmsg;
  }
  
  // now swap the sequence to have it as in the DB and create the real layers
  MdtLayer newLayer;
  for (int i=0 ; i<ntubes ; ++i) {
    newLayer.push_back(newseq[ntubes-i-1]);
  }
  
  // the mezzanine is new, add the 
  if (isNewMezzanine) {
    // check if the mapping is valid for all layers
    if (layer==0) {
      // setting all the layers of this mezzanine
      for (int ll=1 ; ll<nOfLayers+1 ; ++ll) {
	mezType->addLayer(ll,newLayer);
      } 
    }
    else {
      mezType->addLayer(layer,newLayer);
    }
  }
  // this is an existing mezzanine so just add the single layer, but first 
  // check that the layer does not exist yet
  else {
    if (mezType->hasLayer(layer)) {
      *m_log << MSG::ERROR << "Layer: " << layer << " has been already initialized for mezzanine " << type << endmsg;
      return false;
    }
    mezType->addLayer(layer,newLayer);
  }

  return added;
}


/** Add a new mezzanine to the map */
bool MuonMDT_CablingMap::addMezzanine( int mezType, int station, int eta, int phi,
				       int multilayer, int layer, int tube,
				       int subdetId, int rod, int csm, 
				       int tdc, int chan )
{

  uint8_t mezzanineType = (uint8_t) mezType;
  uint8_t subdetectorId = (uint8_t) subdetId;
  uint8_t rodId = (uint8_t) rod;
  uint8_t csmId = (uint8_t) csm;
  uint8_t tdcId = (uint8_t) tdc;
  uint8_t channel = (uint8_t) chan;

  /** check if the subdetector exists, otherwise create it */
  MdtSubdetectorMap* subdetectorMap = this->getSubdetectorMap(subdetectorId);
  if (!subdetectorMap) {
    if (m_debug) *m_log << MSG::VERBOSE << "Subdetector: 0x" << MSG::hex << (int) subdetectorId 
	   << MSG::dec << " not found: create it" << endmsg;
  
    subdetectorMap = new MdtSubdetectorMap(subdetectorId);

    this->setSubdetectorMap(subdetectorId, subdetectorMap);
  }
  else {
    if (m_debug) *m_log << MSG::VERBOSE << "Found the subdetector: 0x" << MSG::hex << (int) subdetectorId 
	   << MSG::dec << endmsg;
  }

  /** look for the correct ROD in the subdetector */
  MdtRODMap* rodMap = subdetectorMap->getRODMap(rodId);
  if (!rodMap) {
    if (m_debug) *m_log << MSG::VERBOSE << "ROD with id: 0x" << MSG::hex << (int) rodId 
	   << MSG::dec << " not found, create it" << endmsg;
    rodMap = new MdtRODMap(rodId);
    subdetectorMap->setRODMap(rodId,rodMap);
  }
  else {
    if (m_debug) *m_log << MSG::VERBOSE << "Found the RODid: 0x" << MSG::hex << (int) rodId << MSG::dec
	   << endmsg;
  }
  
  /** look for the correct CSM in the ROD */
  MdtCsmMap* csmMap = rodMap->getCsmMap(csmId);
  if (!csmMap) {
    if (m_debug) *m_log << MSG::VERBOSE << "CSM with id: 0x" << MSG::hex << (int) csmId 
	   << MSG::dec << " not found, create it" << endmsg;
    csmMap = new MdtCsmMap(csmId);
    rodMap->setCsmMap(csmId,csmMap);

    //    if (!added) {

    //*m_log << MSG::ERROR << "Could not add to the chamber->ROD map the station: " 
    //     << station << " eta: " << eta
    //     << " phi: " << phi << " subdet: 0x" << MSG::hex << (int) subdetectorId 
    //     << MSG::dec << "rod: 0x" << MSG::hex << (int) rodId << endmsg;
      //return false;
    //}

  }
  else {
    if (m_debug) *m_log << MSG::VERBOSE << "Found the CSMid: 0x" << MSG::hex << (int) csmId 
	   << MSG::dec << endmsg;
  }

  // if it's a new CSM (here), update the chamber to ROD map at the same time
  bool added = addChamberToRODMap(station,eta,phi,subdetectorId,rodId);
  if (!added) {
    if (m_debug) {
      *m_log << MSG::VERBOSE << "Station already in the map !" << endmsg;
    }
  }
  
  /** check that the tdc does not exist yet */
  MdtAmtMap* amtMap = csmMap->getTdcMap(tdcId);
  if (!amtMap) {
    // find the corresponding mezzanine type
    const MdtMezzanineType* mezzaType; 
    MezzanineTypes::const_iterator it = m_listOfMezzanineTypes->find(mezzanineType);
    if ( it != m_listOfMezzanineTypes->end() ) {
      mezzaType = (*it).second.get();
    }
    else {
      *m_log << MSG::ERROR << "Mezzanine Type: " << (int) mezzanineType << " not found in the list !" << endmsg;
      return false;
    }

    // create the mezzanine !
    amtMap = new MdtAmtMap(mezzaType, tdcId, channel, station, eta, phi, multilayer,layer,tube,m_mdtIdHelper, m_log);
    // store it in the csm
    csmMap->setAmtMap(tdcId,amtMap);

  }
  else {
    *m_log << MSG::ERROR << "Tdc with Id: 0x" << MSG::hex << (int) tdcId << MSG::dec 
	   << " already found, cannot be added" << endmsg;
    return false;
  }
  
  
  return true;
}



MdtSubdetectorMap* MuonMDT_CablingMap::getSubdetectorMap(uint8_t subdetectorId) const{
  return getItem(subdetectorId);
}


bool MuonMDT_CablingMap::addChamberToRODMap(int station, int eta, int phi,
					    uint8_t subdetectorId, uint8_t rodId)
{
  int sub = (int) subdetectorId;
  int rod = (int) rodId;

  IdentifierHash chamberId;
  bool hashFound = getStationCode(station, eta, phi,chamberId);
  if (!hashFound) {
    *m_log << MSG::ERROR << "Could not found hashId for station: " << station 
	   << " eta: " << eta << " phi: " << phi << endmsg;
    return false;
  }

  uint32_t hardId = (sub<<16) | rod;
  if (m_debug) {
    *m_log << MSG::VERBOSE << "Adding the chamber with Id: " << chamberId
	   << " and subdetector+rod ID: 0x" << MSG::hex << hardId << MSG::dec << endmsg;
  }

  // check if the chamber has already been put in the map
  ChamberToRODMap::const_iterator it = m_chamberToROD->find(chamberId);
  if ( it != m_chamberToROD->end() ) {
    //    *m_log << MSG::ERROR << "Station with Id: " << chamberId << " already in the map !" << endmsg;
    return false;
  }

  m_chamberToROD->insert(std::pair<IdentifierHash,uint32_t>(chamberId,hardId));

  //new function to do the opposite of the above
  RODToChamberMap::iterator Rod_it = m_RODToChamber->find(hardId);
  if ( Rod_it == m_RODToChamber->end() ) {
    std::vector<IdentifierHash> Idvec;    
    Idvec.push_back(chamberId);
    m_RODToChamber->insert(std::make_pair(hardId, Idvec));
  }
  else{
    Rod_it->second.push_back(chamberId);
  }

  // now check if the ROD is already in the list of ROD vector
  bool rodInitialized = false;
  for (unsigned int i=0 ; i<m_listOfROD->size() ; ++i) {
    if (hardId==(*m_listOfROD)[i]) {
      rodInitialized=true;
    }
  }
  if (!rodInitialized) {
    if (m_debug) {
      *m_log << MSG::VERBOSE << "Adding the ROD 0x" << MSG::hex << hardId << MSG::dec
	     << " to the list" << endmsg;
    }
    m_listOfROD->push_back(hardId);
  }

  return true;
}

/*
 * get an integer station code for each chamber
 *
 */
bool MuonMDT_CablingMap::getStationCode(int station, int eta, int phi, 
					IdentifierHash& mdtHashId) const
{
  // create the station identifier
  Identifier elementId = m_mdtIdHelper->elementID(station,eta,phi);

  if(m_mdtIdHelper->get_module_hash(elementId, mdtHashId) ) {
    *m_log << MSG::ERROR << "Could not find HashId for module: " << endmsg;
    elementId.show();
    return false;
  }
  
  return true;
}



/*
 * return the ROBid of a given station, identified through station, eta, phi
 *
 */
uint32_t MuonMDT_CablingMap::getROBId(int station, int eta, int phi) const
{

  int rodId = 0;

  // convert to hash id
  IdentifierHash mdtHashId;
  bool hashFound = getStationCode(station,eta,phi,mdtHashId);
  if (!hashFound) {
    *m_log << MSG::ERROR << "Could not find HashId for station: " << station 
	   << " eta: " << eta << " phi: " << phi << endmsg;
    return rodId;
  }

  rodId = getROBId(mdtHashId);

  return rodId;
}

// get the ROBid given the identifier hash

uint32_t MuonMDT_CablingMap::getROBId(const IdentifierHash stationCode) const
{
  int rodId = 0;

  ChamberToRODMap::const_iterator it = m_chamberToROD->find(stationCode);
  if (it != m_chamberToROD->end()) {
    rodId = it->second;
  }
  else {
    *m_log << MSG::WARNING << "Rod ID not found !" << endmsg;
    return 0;
  }

  return rodId;
}

//get the robs corresponding to a vector of hashIds, copied from Svc before the readCdo migration

std::vector<uint32_t> MuonMDT_CablingMap::getROBId(const std::vector<IdentifierHash>& mdtHashVector) const
{
  std::vector<uint32_t> robVector;

  for ( unsigned int i = 0 ; i<mdtHashVector.size() ; ++i ) {

    int robId = getROBId(mdtHashVector[i]);
    if (robId==0) {

      *m_log << MSG::ERROR << "ROB id not found for Hash Id: " << mdtHashVector[i] << endmsg;

    } else {
      *m_log << MSG::VERBOSE << "Found ROB id 0x" << MSG::hex << robId << MSG::dec << " for hashId " << mdtHashVector[i] << endmsg;
    }
    robVector.push_back(robId);
  }
  *m_log << MSG::VERBOSE << "Size of ROB vector is: " << robVector.size() << endmsg;

  return robVector;
}

const std::vector<IdentifierHash>& MuonMDT_CablingMap::getChamberHashVec(const uint32_t ROBId) const
{
  RODToChamberMap::const_iterator Rob_it = m_RODToChamber->find(ROBId);
  if(Rob_it != m_RODToChamber->end()) {
    return Rob_it->second;
  }
  else {
    *m_log << MSG::WARNING << "Rod ID not found !" << endmsg;
    return m_emptyIdHashVec;
  }
}

const std::vector<IdentifierHash> MuonMDT_CablingMap::getChamberHashVec(const std::vector< uint32_t> &ROBId_list) const
{
   std::vector<IdentifierHash> HashVec;
  
  for(unsigned int i = 0; i< ROBId_list.size(); ++i)
    {
      RODToChamberMap::const_iterator Rob_it = m_RODToChamber->find(ROBId_list[i]);
      if(Rob_it != m_RODToChamber->end()) {
	HashVec.insert(HashVec.end(), Rob_it->second.begin(), Rob_it->second.end());
      }
      else {
	*m_log << MSG::WARNING << "Rod ID "<<ROBId_list[i]<<" not found, continuing with the rest of the ROBId" << endmsg;
      }

    }
  return HashVec;
}

/*
 * get the full list of ROBid
 *
 */
std::vector<uint32_t> MuonMDT_CablingMap::getAllROBId() const
{
  return *m_listOfROD;
}



/*
 *
 * translate an online Id into an offline Id
 *
 */
bool MuonMDT_CablingMap::getOfflineId(uint8_t subdetectorId,
				      uint8_t rodId,
				      uint8_t csmId,
				      uint8_t tdcId,
				      uint8_t channelId,
				      int& stationName, int& stationEta, int& stationPhi,
				      int& multiLayer, int& layer, int& tube) const
{
  
  // get the subdetector
  MdtSubdetectorMap* subdetectorMap = this->getSubdetectorMap(subdetectorId);
  if (!subdetectorMap) {
    *m_log << MSG::WARNING << "Subdetector: 0x" << MSG::hex << (int) subdetectorId << MSG::dec 
	   << " not found in the map" << endmsg;
    return false;
  }
  // get the rod
  MdtRODMap* rodMap = subdetectorMap->getRODMap(rodId);
  if (!rodMap) {
    *m_log << MSG::WARNING << "MROD: 0x" << MSG::hex << (int) rodId << MSG::dec 
	   << " not found in the map of subdetector: 0x" << MSG::hex << (int) subdetectorId 
	   << MSG::dec << endmsg;
    return false;
  }
  // retrieve the csm
  MdtCsmMap* csmMap = rodMap->getCsmMap(csmId);
  if (!csmMap) {
    *m_log << MSG::WARNING << "CSM: 0x" << MSG::hex << (int) csmId << MSG::dec 
	   << " not found in MROD: 0x" << MSG::hex << (int) rodId 
	   << MSG::dec << " of subdetector: 0x" << MSG::hex << (int) subdetectorId << MSG::dec << endmsg;
    return false;
  }  
  // retrieve the tdc
  MdtAmtMap* amtMap = nullptr;
  // if it's the dummy TDC (i.e. the 0xff used to convert the full station)
  
 if (tdcId==0xff && channelId==0xff) {

   std::map<uint8_t, std::unique_ptr<MdtAmtMap>, std::less<uint8_t> >* listOfAmt = csmMap->getListOfElements();
   std::map<uint8_t, std::unique_ptr<MdtAmtMap>, std::less<uint8_t> >::const_iterator it_amt = listOfAmt->begin();


   for ( ; it_amt != listOfAmt->end() ; ++it_amt) {
     
     // get the second tdc of that chamber
     amtMap = (*it_amt).second.get();
     
     if (!amtMap) {
       *m_log << MSG::WARNING << "CSM: 0x" << MSG::hex << (int) csmId 
	      << MSG::dec << " of MROD: 0x" << MSG::hex << (int) rodId << MSG::dec 
	      << " subdetector: 0x" << MSG::hex << (int) subdetectorId << MSG::dec  
	      << " not identified !!" <<   endmsg;
       return false;
     }
     
     channelId=0;
     
     bool foundTdc = amtMap->offlineId(channelId, stationName, stationEta, stationPhi, 
				       multiLayer, layer, tube);
   
     if (foundTdc) return true;
   }

   // if no channel 0 has been found, there must be an error !!
   return false;

 } 
 
 else {
   
   amtMap = csmMap->getTdcMap(tdcId);
   if (!amtMap) {
     *m_log << MSG::WARNING << "Tdc: 0x" << MSG::hex << (int) tdcId << MSG::dec 
	     << " not found in CSM: 0x" << MSG::hex << (int) csmId 
	     << MSG::dec << " of MROD: 0x" << MSG::hex << (int) rodId << MSG::dec 
	     << " subdetector: 0x" << MSG::hex << (int) subdetectorId << MSG::dec << endmsg;
      return false;
    }

  }
  
  // retrieve the full information
  bool found = amtMap->offlineId(channelId, stationName, stationEta, stationPhi, 
				 multiLayer, layer, tube);
  
  if (!found) {
    
    //    *m_log << MSG::WARNING << "Channel: 0x" << MSG::hex << (int) channelId << MSG::dec  
    //   << " Tdc: 0x" << MSG::hex << (int) tdcId << MSG::dec 
    //   << " not found in CSM: 0x" << MSG::hex << (int) csmId 
    //   << MSG::dec << " of MROD: 0x" << MSG::hex << (int) rodId << MSG::dec 
    //   << " subdetector: 0x" << MSG::hex << (int) subdetectorId << MSG::dec << endmsg;
    return false;
  }

  if (m_debug) {

    *m_log << MSG::VERBOSE << "Channel: 0x" << MSG::hex << (int) channelId << MSG::dec  
	   << " Tdc: 0x" << MSG::hex << (int) tdcId << MSG::dec 
	   << " CSM: 0x" << MSG::hex << (int) csmId 
	   << MSG::dec << " of MROD: 0x" << MSG::hex << (int) rodId << MSG::dec 
	   << " subdetector: 0x" << MSG::hex << (int) subdetectorId 
	   << MSG::dec << endmsg;

    *m_log << MSG::VERBOSE << "Mapped to: Station: " << stationName << " eta: " << stationEta
	   << " phi: " << stationPhi << " multiLayer: " << multiLayer 
	   << " layer: " << layer << " tube: " << tube << endmsg;
  }

  return true;
}


/** get the online id from the offline id */
bool MuonMDT_CablingMap::getOnlineId(int stationName, int stationEta, int stationPhi,
				     int multiLayer, int layer, int tube,
				     uint8_t& subdetectorId, uint8_t& rodId, 
				     uint8_t& csmId, uint8_t& tdcId, 
				     uint8_t& channelId) const
{

  
  // get the station ROD from the ROD map
  int module = getROBId(stationName, stationEta, stationPhi);

  rodId = (module & 0xff);
  subdetectorId = (module>>16) & 0xff;

  if (m_debug) {

    *m_log << MSG::VERBOSE << "Station: " << stationName << " eta: " << stationEta 
	   << " phi: " << stationPhi << " mapped to subdet: 0x" << MSG::hex << subdetectorId
	   << MSG::dec << " rodId: 0x" << MSG::hex << rodId << MSG::dec << endmsg;

  }
  
  // get correct subdetector and rod

  // get the subdetector
  MdtSubdetectorMap* subdetectorMap = this->getSubdetectorMap(subdetectorId);
  if (!subdetectorMap) {
    *m_log << MSG::WARNING << "Subdetector: 0x" << MSG::hex << (int) subdetectorId 
	   << MSG::dec << " not found in the map" << endmsg;
    return false;
  }
  // get the rod
  MdtRODMap* rodMap = subdetectorMap->getRODMap(rodId);
  if (!rodMap) {
    *m_log << MSG::WARNING << "MROD: 0x" << MSG::hex << (int) rodId << MSG::dec 
	   << " not found in the map of subdetector: 0x" << MSG::hex 
	   << (int) subdetectorId 
	   << MSG::dec << endmsg;
    return false;
  }
  

  // loop on the CSMs of this ROD
  std::map<uint8_t, std::unique_ptr<MdtCsmMap>, std::less<uint8_t> >* listOfCsm= rodMap->getListOfElements();
  
  std::map<uint8_t, std::unique_ptr<MdtCsmMap>, std::less<uint8_t> >::const_iterator it_csm;
 
  std::map<uint8_t, std::unique_ptr<MdtAmtMap>, std::less<uint8_t> >* listOfAmt;
  std::map<uint8_t, std::unique_ptr<MdtAmtMap>, std::less<uint8_t> >::const_iterator it_amt;

  if (m_debug) {
    *m_log << MSG::VERBOSE << "number of csm to loop: " << listOfCsm->size()
	   << endmsg;
  }

  for ( it_csm = listOfCsm->begin(); it_csm != listOfCsm->end() ; ++it_csm) {
    
    csmId = ((*it_csm).second)->moduleId();
    
    if (m_debug) {
      *m_log << MSG::VERBOSE << "Csm number: " << MSG::hex << (int) csmId << MSG::dec << endmsg;
    }

    listOfAmt = ((*it_csm).second)->getListOfElements();

    //    std::cout << "number of amt to loop: " << listOfAmt->size() << std::endl;


    for (it_amt=listOfAmt->begin() ; it_amt !=listOfAmt->end() ; ++it_amt) {

      int test_station=0;
      int test_eta=0;
      int test_phi=0;
      int test_multi=0;
      int test_layer=0;
      int test_tube=0;
      
      MdtAmtMap* amtMap = (*it_amt).second.get();

      tdcId = amtMap->moduleId();

      //  std::cout << "Tdc number: 0x" << std::hex << (int) tdcId << std::dec << std::endl;

      // convert channel 0
      bool found = amtMap->offlineId(0,test_station,test_eta,test_phi,
				     test_multi,test_layer,test_tube);
      if (!found) {

	found = amtMap->offlineId(23,test_station,test_eta,test_phi,
				     test_multi,test_layer,test_tube);
	if (!found) {
	  if (m_debug) {
	    *m_log << MSG::VERBOSE << "Error converting offline->online" << endmsg;
	  }
	  return false;
	}
      }

      //      std::cout << "Test from channel 0: " << test_station << " " << test_eta
      //	<< " " << test_phi << " " << test_multi << std::endl;
      
      //std::cout << "I am looking for: " << stationName << " " << stationEta
      //	<< " " << stationPhi << " " << multiLayer << std::endl;
      
      

      // if it's not the right station, exit from the loop 
      if (test_station == stationName && test_eta == stationEta &&
	  test_phi == stationPhi && test_multi==multiLayer ) {
	
	//	std::cout << "found !! " << std::endl;
	// look for the correct channel
	for (int i=0 ; i<24 ; ++i) {
	  found = amtMap->offlineId(i,test_station,test_eta,test_phi,
				    test_multi,test_layer,test_tube);
	  //	  if (!found) {
	  //  *m_log << MSG::WARNING << "Error converting offline->online, looping on TDC channels" << endmsg;
	  // return false;
	  //}
	  
	  //  std::cout << "test_layer: " << test_layer << " test_tube: " << test_tube
	  ///    << std::endl;
	  //std::cout << "I am looking for: " << layer << "  tube: " << tube
	  //    << std::endl;

	  // if it's the good one, then set the channel
	  if (found && test_layer==layer && test_tube==tube) {
	    
	    channelId = i;
	    return true;
	  }
	}
      }  // if to check that it's the right station


    }

  }


  return false;
}

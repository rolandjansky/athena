/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonCablingData/MdtRODMap.h"

#include "MuonCablingData/MdtMezzanineType.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include<cmath>

std::ostream& operator<<(std::ostream& ostr, const MuonMDT_CablingMap::CablingData & obj){
      ostr<<"StationIndex: "<<(int)obj.stationIndex
          <<" eta: "<<(int)obj.eta
          <<" phi: "<<(int)obj.phi
          <<" multilayer: "<<(int)obj.multilayer
          <<" tube-layer: "<<(int)obj.layer
          <<" tube: "<<(int)obj.tube
          <<"  ----  MROD: "<<(int)obj.mrod
          <<" CSM: "<<(int)obj.csm
          <<" mezzanine-type: "<<(int)obj.mezzanine_type
          <<" subdetector id: "<<(int)obj.subdetectorId
          <<" TDC: "<<(int)obj.tdcId
          <<" tdc-channel: "<<(int)obj.channelId;
      return ostr;
 } 
 

MuonMDT_CablingMap::MuonMDT_CablingMap() :
  MdtMapBase<MdtSubdetectorMap>(0,"MdtSubdetectorMap")
{ 
  // initialize the message service

  // retrieve the MdtIdHelper
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StoreGateSvc* detStore= nullptr;
  StatusCode sc = svcLocator->service("DetectorStore",detStore);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error("Could not find the detctor store");
  }
  sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error("Could not retrieve the MdtIdHelper");
  }
}

MuonMDT_CablingMap::~MuonMDT_CablingMap() = default;

bool MuonMDT_CablingMap::convert(const CablingData& cabling_data, Identifier& id, bool check_valid )const{
  bool valid{!check_valid};
  id = check_valid ? m_mdtIdHelper->channelID(cabling_data.stationIndex, cabling_data.eta, cabling_data.phi,
                                cabling_data.multilayer, cabling_data.layer, cabling_data.tube, valid) :
                     m_mdtIdHelper->channelID(cabling_data.stationIndex, cabling_data.eta, cabling_data.phi,
                                cabling_data.multilayer, cabling_data.layer, cabling_data.tube);
  return valid;
}
bool MuonMDT_CablingMap::convert(const Identifier& module_id , CablingData& cabling_data) const{
  if (!m_mdtIdHelper->is_mdt(module_id)) return false;
  cabling_data.stationIndex = m_mdtIdHelper->stationName(module_id);
  cabling_data.eta = m_mdtIdHelper->stationEta(module_id);
  cabling_data.phi = m_mdtIdHelper->stationPhi(module_id);
  cabling_data.tube = m_mdtIdHelper->tube(module_id);
  cabling_data.multilayer = m_mdtIdHelper->multilayer(module_id);
  cabling_data.layer = m_mdtIdHelper->tubeLayer(module_id);
  return true;
}
  
  

// add a CSM map to this ROD
bool MuonMDT_CablingMap::setSubdetectorMap(uint8_t subdetectorId, MdtSubdetectorMap* subdetectorMap, MsgStream &log)
{ 
  bool subdetectorAdded = addItem(subdetectorId,subdetectorMap, log);

  if (!subdetectorAdded) {
    log << MSG::ERROR << "Could not add subdetector " << MSG::hex 
        << static_cast<unsigned> (subdetectorId) << MSG::dec << " to the MDT cabling map" << endmsg;
  }

  return subdetectorAdded;
}


/** add a new line from the database, describing a mezzanine type */
bool MuonMDT_CablingMap::addMezzanineLine(const int type, const int layer,
					  const int sequence, MsgStream &log)
{
  bool added = true;
  bool debug = (log.level() <= MSG::VERBOSE);
  if (debug) {
    log << MSG::VERBOSE << "Now in MuonMDT_CablingMap::addMezzanineLine" << endmsg;
  }

  MdtMezzanineType* mezType;
  
  MezzanineTypes::const_iterator it = m_listOfMezzanineTypes.find(type);

  // check if the type already exists, if yes, retrieve it
  bool isNewMezzanine;
  if (it != m_listOfMezzanineTypes.end()) {
    // if the attempt is to initialize all layers of a mezzanine already
    // initialized, return an error message
    if (!layer) {
      log << MSG::ERROR << "The mezzanine type " << type 
	     << "has been already initialized" << endmsg;
      return false;
    }
    mezType = it->second.get();
    isNewMezzanine = false;
  }
  // if it does not exist, create the new type
  else {
    if (debug) {
      log << MSG::VERBOSE << "Creating a new mezzanine type: " << type << endmsg;
    }
    mezType = new MdtMezzanineType(type);
    // insert the new mezzanine in the map
    m_listOfMezzanineTypes.insert(MezzanineTypes::value_type(type,mezType));
    isNewMezzanine=true;
  }
  
  int ntubes = 0;
  int number = sequence;
  int nOfLayers;
  std::array<int,8> newseq{0};
  // now decode the sequence, up to 8 tubes per sequence
  int tube = number%10;

  while ( tube!=0 ) {
    // add the tube to the tube sequence
    if (ntubes>7) {
      log << MSG::ERROR << "More than 8 tubes in a layer, not possible !" 
	     << endmsg;
      return false;
    }
    if (debug) {
      log << MSG::VERBOSE << "Adding tube number: " << tube << " to the layer " 
	     << layer << " of mezzanine type " << type << endmsg;
    }
    
    newseq[ntubes]=tube;

    ++ntubes;
    number = (number-tube)/10;
    //    tube = (int) (((number/10.)-((double)((int)(number/10.))))*10.);    

    tube = number%10;
  }

  if (ntubes!=8 && ntubes!=6) {
    log << MSG::ERROR << "in type " << type 
	   << ": number of tubes per mezzanine layer can be only 6 or 8 ! what are you doing ???" << endmsg;
    return false;
  }
  else {
    nOfLayers = 24/ntubes;
    if (layer>nOfLayers) {
      log << MSG::ERROR << "The maximum number of layers for this mezzanine is: "
	     << nOfLayers << " so you can't initialize layer: " 
	     << layer << endmsg;
      return false;
    }

  }

  if (debug) {
    log << MSG::VERBOSE << "Found " << ntubes << " tubes in layer " << layer << endmsg;
    log << MSG::VERBOSE << "This is a " << nOfLayers 
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
	mezType->addLayer(ll,newLayer, log);
      } 
    }
    else {
      mezType->addLayer(layer,newLayer, log);
    }
  }
  // this is an existing mezzanine so just add the single layer, but first 
  // check that the layer does not exist yet
  else {
    if (mezType->hasLayer(layer)) {
      log << MSG::ERROR << "Layer: " << layer << " has been already initialized for mezzanine " << type << endmsg;
      return false;
    }
    mezType->addLayer(layer,newLayer, log);
  }

  return added;
}


/** Add a new mezzanine to the map */

bool MuonMDT_CablingMap::addMezzanine(const CablingData& map_data, MsgStream &log ) {
  bool debug = (log.level() <= MSG::VERBOSE);
  /** check if the subdetector exists, otherwise create it */
  MdtSubdetectorMap* subdetectorMap = getSubdetectorMap(map_data.subdetectorId);
  if (!subdetectorMap) {
    if (debug){
     log << MSG::VERBOSE << "Subdetector: 0x" << MSG::hex << (int) map_data.subdetectorId 
	   << MSG::dec << " not found: create it" << endmsg;
   }
  
    subdetectorMap = new MdtSubdetectorMap(map_data.subdetectorId);
    setSubdetectorMap(map_data.subdetectorId, subdetectorMap, log);
  }
  else {
    if (debug){
      log << MSG::VERBOSE << "Found the subdetector: 0x"
          << MSG::hex <<  static_cast<unsigned> (map_data.subdetectorId) << MSG::dec << endmsg;
    }
  }

  /** look for the correct ROD in the subdetector */
  MdtRODMap* rodMap = subdetectorMap->getRODMap(map_data.mrod);
  if (!rodMap) {
    if (debug) {
      log << MSG::VERBOSE << "ROD with id: 0x"
          << MSG::hex << static_cast<unsigned> (map_data.mrod)  << MSG::dec << " not found, create it" << endmsg;
    }
    rodMap = new MdtRODMap(map_data.mrod);
    subdetectorMap->setRODMap(map_data.mrod,rodMap, log);
  }
  else {
    if (debug) {
      log << MSG::VERBOSE << "Found the RODid: 0x"
          << MSG::hex <<  static_cast<unsigned> (map_data.mrod)  << MSG::dec << endmsg;
    }
  }
  
  /** look for the correct CSM in the ROD */
  MdtCsmMap* csmMap = rodMap->getCsmMap(map_data.csm);
  if (!csmMap) {
    if (debug) {
      log << MSG::VERBOSE << "CSM with id: 0x"
          << MSG::hex << static_cast<unsigned> (map_data.csm) << MSG::dec << " not found, create it" << endmsg;
   }
    csmMap = new MdtCsmMap(map_data.csm);
    rodMap->setCsmMap(map_data.csm, csmMap, log);

  }
  else {
    if (debug){
      log << MSG::VERBOSE << "Found the CSMid: 0x" << MSG::hex << (int) map_data.csm << MSG::dec << endmsg;
    }
  }

  // if it's a new CSM (here), update the chamber to ROD map at the same time
  bool added = addChamberToRODMap(map_data.stationIndex,map_data.eta,map_data.phi, map_data.subdetectorId, map_data.mrod, log);
  if (!added) {
    if (debug) {
      log << MSG::VERBOSE << "Station already in the map !" << endmsg;
    }
  }
  
  /** check that the tdc does not exist yet */
  MdtAmtMap* amtMap = csmMap->getTdcMap(map_data.tdcId);
  if (!amtMap) {
    // find the corresponding mezzanine type
    const MdtMezzanineType* mezzaType{nullptr}; 
    MezzanineTypes::const_iterator it = m_listOfMezzanineTypes.find(map_data.mezzanine_type);
    if ( it != m_listOfMezzanineTypes.end() ) {
      mezzaType = (*it).second.get();
    }
    else {
      log << MSG::ERROR << "Mezzanine Type: " << map_data.mezzanine_type << " not found in the list !" << endmsg;
      return false;
    }

    // create the mezzanine !
    amtMap = new MdtAmtMap(mezzaType, map_data.tdcId, map_data.channelId, map_data.stationIndex, map_data.eta, 
                          map_data.phi, map_data.multilayer,map_data.layer, map_data.tube, m_mdtIdHelper, log);
    // store it in the csm
    csmMap->setAmtMap(map_data.tdcId,amtMap, log);

  }
  else {
    log << MSG::ERROR << "Tdc with Id: 0x"
        << MSG::hex << static_cast<unsigned> (map_data.tdcId) << MSG::dec  << " already found, cannot be added" << endmsg;
    return false;
  }
  return true;
}



MdtSubdetectorMap* MuonMDT_CablingMap::getSubdetectorMap(uint8_t subdetectorId) const{
  return getItem(subdetectorId);
}


bool MuonMDT_CablingMap::addChamberToRODMap(int station, int eta, int phi,
					    int subdetectorId, int rodId, MsgStream &log)
{
  int sub = subdetectorId;
  int rod = rodId;
  bool debug = (log.level() <= MSG::VERBOSE);
  IdentifierHash chamberId;
  bool hashFound = getStationCode(station, eta, phi,chamberId, log);
  if (!hashFound) {
    log << MSG::ERROR << "Could not found hashId for station: " << station 
	   << " eta: " << eta << " phi: " << phi << endmsg;
    return false;
  }

  uint32_t hardId = (sub<<16) | rod;
  if (debug) {
    log << MSG::VERBOSE << "Adding the chamber with Id: " << chamberId
	   << " and subdetector+rod ID: 0x" << MSG::hex << hardId << MSG::dec << endmsg;
  }

  // check if the chamber has already been put in the map
  ChamberToRODMap::const_iterator it = m_chamberToROD.find(chamberId);
  if ( it != m_chamberToROD.end() ) {
    //    *m_log << MSG::ERROR << "Station with Id: " << chamberId << " already in the map !" << endmsg;
    return false;
  }

  m_chamberToROD.insert(std::pair<IdentifierHash,uint32_t>(chamberId,hardId));

  //new function to do the opposite of the above
  RODToChamberMap::iterator Rod_it = m_RODToChamber.find(hardId);
  if ( Rod_it == m_RODToChamber.end() ) {
    std::vector<IdentifierHash> Idvec;    
    Idvec.push_back(chamberId);
    m_RODToChamber.insert(std::make_pair(hardId, Idvec));
  }
  else{
    Rod_it->second.push_back(chamberId);
  }

  // now check if the ROD is already in the list of ROD vector
  bool rodInitialized = false;
  for (unsigned int i=0 ; i<m_listOfROD.size() ; ++i) {
    if (hardId==m_listOfROD[i]) {
      rodInitialized=true;
    }
  }
  if (!rodInitialized) {
    if (debug) {
      log << MSG::VERBOSE << "Adding the ROD 0x" << MSG::hex << hardId << MSG::dec
	     << " to the list" << endmsg;
    }
    m_listOfROD.push_back(hardId);
  }

  return true;
}

/*
 * get an integer station code for each chamber
 *
 */
bool MuonMDT_CablingMap::getStationCode(int station, int eta, int phi, 
					IdentifierHash& mdtHashId, MsgStream &log) const
{
  // create the station identifier
  Identifier elementId = m_mdtIdHelper->elementID(station,eta,phi);

  if(m_mdtIdHelper->get_module_hash(elementId, mdtHashId) ) {
    log << MSG::ERROR << "Could not find HashId for module: " << endmsg;
    elementId.show();
    return false;
  }
  
  return true;
}



/*
 * return the ROBid of a given station, identified through station, eta, phi
 *
 */
uint32_t MuonMDT_CablingMap::getROBId(int station, int eta, int phi, MsgStream &log) const
{

  int rodId = 0;

  // convert to hash id
  IdentifierHash mdtHashId;
  bool hashFound = getStationCode(station,eta,phi,mdtHashId, log);
  if (!hashFound) {
    log << MSG::ERROR << "Could not find HashId for station: " << station 
	   << " eta: " << eta << " phi: " << phi << endmsg;
    return rodId;
  }

  rodId = getROBId(mdtHashId, log);

  return rodId;
}

// get the ROBid given the identifier hash

uint32_t MuonMDT_CablingMap::getROBId(const IdentifierHash stationCode, MsgStream &log) const
{
  int rodId = 0;

  ChamberToRODMap::const_iterator it = m_chamberToROD.find(stationCode);
  if (it != m_chamberToROD.end()) {
    rodId = it->second;
  }
  else {
    log << MSG::WARNING << "Rod ID not found !" << endmsg;
    return 0;
  }

  return rodId;
}

//get the robs corresponding to a vector of hashIds, copied from Svc before the readCdo migration

std::vector<uint32_t> MuonMDT_CablingMap::getROBId(const std::vector<IdentifierHash>& mdtHashVector, MsgStream &log) const
{
  std::vector<uint32_t> robVector;
  bool debug = (log.level() <= MSG::VERBOSE);
  for ( unsigned int i = 0 ; i<mdtHashVector.size() ; ++i ) {

    int robId = getROBId(mdtHashVector[i], log);
    if (robId==0) {
      log << MSG::ERROR << "ROB id not found for Hash Id: " << mdtHashVector[i] << endmsg;
    } else {
      if(debug){
        log << MSG::VERBOSE << "Found ROB id 0x" << MSG::hex << robId << MSG::dec << " for hashId " << mdtHashVector[i] << endmsg;
      }
    }
    robVector.push_back(robId);
  }
  {
    log << MSG::VERBOSE << "Size of ROB vector is: " << robVector.size() << endmsg;
  }
  return robVector;
}

const std::vector<IdentifierHash>& MuonMDT_CablingMap::getChamberHashVec(const uint32_t ROBId, MsgStream &log) const
{
  RODToChamberMap::const_iterator Rob_it = m_RODToChamber.find(ROBId);
  if(Rob_it != m_RODToChamber.end()) {
    return Rob_it->second;
  }
  else {
    log << MSG::WARNING << "Rod ID not found !" << endmsg;
    return m_emptyIdHashVec;
  }
}

const std::vector<IdentifierHash> MuonMDT_CablingMap::getChamberHashVec(const std::vector< uint32_t> &ROBId_list, MsgStream &log) const
{
   std::vector<IdentifierHash> HashVec;
  
  for(unsigned int i = 0; i< ROBId_list.size(); ++i)
    {
      RODToChamberMap::const_iterator Rob_it = m_RODToChamber.find(ROBId_list[i]);
      if(Rob_it != m_RODToChamber.end()) {
	HashVec.insert(HashVec.end(), Rob_it->second.begin(), Rob_it->second.end());
      }
      else {
	      log << MSG::WARNING << "Rod ID "<<ROBId_list[i]<<" not found, continuing with the rest of the ROBId" << endmsg;
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
  return m_listOfROD;
}



/*
 *
 * translate an online Id into an offline Id
 *
 */

bool MuonMDT_CablingMap::getOfflineId(
            CablingData& cabling_map,
            MsgStream &log) const {

  // get the subdetector
  MdtSubdetectorMap* subdetectorMap = getSubdetectorMap(cabling_map.subdetectorId);
  if (!subdetectorMap) {
    log << MSG::WARNING << "Subdetector: 0x"
        << MSG::hex << static_cast<unsigned> (cabling_map.subdetectorId) << MSG::dec << " not found in the map" << endmsg;
    return false;
  }
  // get the rod
  MdtRODMap* rodMap = subdetectorMap->getRODMap(cabling_map.mrod);
  if (!rodMap) {
    log << MSG::WARNING << "MROD: 0x"
        << MSG::hex << static_cast<unsigned> (cabling_map.mrod) << MSG::dec 
        << " not found in the map of subdetector: 0x"
        << MSG::hex << static_cast<unsigned> (cabling_map.subdetectorId)
	   << MSG::dec << endmsg;
    return false;
  }
  // retrieve the csm
  MdtCsmMap* csmMap = rodMap->getCsmMap(cabling_map.csm);
  if (!csmMap) {
    log << MSG::WARNING << "CSM: 0x"
        << MSG::hex << static_cast<unsigned> (cabling_map.csm) << MSG::dec 
        << " not found in MROD: 0x"
        << MSG::hex << static_cast<unsigned> (cabling_map.mrod) << MSG::dec
        << " of subdetector: 0x"
        << MSG::hex << static_cast<unsigned> (cabling_map.subdetectorId) << MSG::dec << endmsg;
    return false;
  }  
  // retrieve the tdc
  MdtAmtMap* amtMap = nullptr;
  // if it's the dummy TDC (i.e. the 0xff used to convert the full station)
  
 if (cabling_map.tdcId==0xff && cabling_map.channelId==0xff) {

   const std::map<uint8_t, std::unique_ptr<MdtAmtMap>, std::less<uint8_t> >* listOfAmt = csmMap->getListOfElements();
   std::map<uint8_t, std::unique_ptr<MdtAmtMap>, std::less<uint8_t> >::const_iterator it_amt = listOfAmt->begin();
   for ( ; it_amt != listOfAmt->end() ; ++it_amt) {
     
     // get the second tdc of that chamber
     amtMap = (*it_amt).second.get();     
     if (!amtMap) {
       log << MSG::WARNING << "CSM: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.csm )
           << MSG::dec << " of MROD: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.mrod) << MSG::dec 
           << " subdetector: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.subdetectorId) << MSG::dec  
	      << " not identified !!" <<   endmsg;
       return false;
     }
     
     cabling_map.channelId=0;
     if (amtMap->offlineId(cabling_map.channelId, cabling_map.stationIndex, cabling_map.eta, cabling_map.phi, 
				       cabling_map.multilayer, cabling_map.layer, cabling_map.tube, log)) return true;
   }

   // if no channel 0 has been found, there must be an error !!
   return false;

 } else {
   
   amtMap = csmMap->getTdcMap(cabling_map.tdcId);
   if (!amtMap) {
     log << MSG::WARNING << "Tdc: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.tdcId) << MSG::dec 
         << " not found in CSM: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.csm)
         << MSG::dec << " of MROD: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.mrod) << MSG::dec 
         << " subdetector: 0x" << MSG::hex <<  static_cast<unsigned> (cabling_map.subdetectorId) << MSG::dec << endmsg;
      return false;
    }

  }
  
  // retrieve the full information
  if (!amtMap->offlineId(cabling_map.channelId, cabling_map.stationIndex, cabling_map.eta, cabling_map.phi, 
				       cabling_map.multilayer, cabling_map.layer, cabling_map.tube, log)) {    
    log << MSG::WARNING << "Channel: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.channelId) << MSG::dec  
        << " Tdc: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.tdcId) << MSG::dec 
        << " not found in CSM: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.csm)
        << MSG::dec << " of MROD: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.mrod) << MSG::dec 
        << " subdetector: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.subdetectorId) << MSG::dec << endmsg;
    return false;
  }
  if (log.level() <= MSG::VERBOSE) {
    log << MSG::VERBOSE << "Channel: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.channelId) << MSG::dec  
        << " Tdc: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.tdcId) << MSG::dec 
        << " CSM: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.csm)
        << MSG::dec << " of MROD: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.mrod) << MSG::dec 
        << " subdetector: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.subdetectorId)
        << MSG::dec << endmsg;

    log << MSG::VERBOSE << "Mapped to: Station: " << cabling_map.stationIndex << " eta: " << cabling_map.eta
	   << " phi: " << cabling_map.phi << " multiLayer: " << cabling_map.multilayer 
	   << " layer: " << cabling_map.layer << " tube: " << cabling_map.tube << endmsg;
  }

  return true;
}


/** get the online id from the offline id */
bool MuonMDT_CablingMap::getOnlineId(CablingData& cabling_map, MsgStream &log) const {

  bool debug = (log.level() <= MSG::VERBOSE);
  // get the station ROD from the ROD map
  int module_id = getROBId(cabling_map.stationIndex,cabling_map.eta, cabling_map.phi, log);

  cabling_map.mrod = (module_id & 0xff);
  cabling_map.subdetectorId = (module_id>>16) & 0xff;

  if (debug) {
    log << MSG::VERBOSE << "Station: " << cabling_map.stationIndex << " eta: " << cabling_map.eta 
        << " phi: " << cabling_map.phi << " mapped to subdet: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.subdetectorId)
        << MSG::dec << " rodId: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.mrod) << MSG::dec << endmsg;
  }
  
  // get correct subdetector and rod

  // get the subdetector
  MdtSubdetectorMap* subdetectorMap = getSubdetectorMap(cabling_map.subdetectorId);
  if (!subdetectorMap) {
    log << MSG::WARNING << "Subdetector: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.subdetectorId) 
	   << MSG::dec << " not found in the map" << endmsg;
    return false;
  }
  // get the rod
  MdtRODMap* rodMap = subdetectorMap->getRODMap(cabling_map.mrod );
  if (!rodMap) {
    log << MSG::WARNING << "MROD: 0x" << MSG::hex << static_cast<unsigned> (cabling_map.mrod)  << MSG::dec 
        << " not found in the map of subdetector: 0x" << MSG::hex 
        << static_cast<unsigned> (cabling_map.subdetectorId)
        << MSG::dec << endmsg;
    return false;
  }
  

  // loop on the CSMs of this ROD
  const std::map<uint8_t, std::unique_ptr<MdtCsmMap>, std::less<uint8_t> >* listOfCsm= rodMap->getListOfElements();
  
  std::map<uint8_t, std::unique_ptr<MdtCsmMap>, std::less<uint8_t> >::const_iterator it_csm;
 
  const std::map<uint8_t, std::unique_ptr<MdtAmtMap>, std::less<uint8_t> >* listOfAmt;
  std::map<uint8_t, std::unique_ptr<MdtAmtMap>, std::less<uint8_t> >::const_iterator it_amt;

  if (debug) {
    log << MSG::VERBOSE << "number of csm to loop: " << listOfCsm->size()
	   << endmsg;
  }

  for ( it_csm = listOfCsm->begin(); it_csm != listOfCsm->end() ; ++it_csm) {
    
    cabling_map.csm = ((*it_csm).second)->moduleId();
    
    if (debug) {
      log << MSG::VERBOSE << "Csm number: " << MSG::hex << static_cast<unsigned> (cabling_map.csm) << MSG::dec << endmsg;
    }

    listOfAmt = ((*it_csm).second)->getListOfElements();
    for (it_amt=listOfAmt->begin() ; it_amt !=listOfAmt->end() ; ++it_amt) {

      int test_station{0}, test_eta{0}, test_phi{0}, test_multi{0},
          test_layer{0}, test_tube{0};
      
      MdtAmtMap* amtMap = (*it_amt).second.get();

      cabling_map.tdcId = amtMap->moduleId();
      // convert channel 0
      bool found = amtMap->offlineId(0,test_station,test_eta,test_phi,
				     test_multi,test_layer,test_tube, log);
      if (!found) {
          found = amtMap->offlineId(23,test_station,test_eta,test_phi,
				                              test_multi,test_layer,test_tube, log);
	    if (!found) {
	      if (debug) {
	         log << MSG::VERBOSE << "Error converting offline->online" << endmsg;
	     }
	      return false;
	    } 
    }

      // if it's not the right station, exit from the loop 
      if (test_station == cabling_map.stationIndex && test_eta == cabling_map.eta &&
	  test_phi == cabling_map.phi && test_multi==cabling_map.multilayer ) {
	
      	for (int i=0 ; i<24 ; ++i) {
	         found = amtMap->offlineId(i,test_station,test_eta,test_phi,
				        test_multi,test_layer,test_tube, log);
        	  if (found && test_layer==cabling_map.layer && test_tube==cabling_map.tube) {
	             cabling_map.channelId = i;
	            return true;
	          }
	      }
      }  // if to check that it's the right station

    }
  }

  return false;
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/TGCTriggerDbAlg.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"

TGCTriggerDbAlg::TGCTriggerDbAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_readKey_bw("/TGC/TRIGGER/CW_BW"),
  m_readKey_eifi("/TGC/TRIGGER/CW_EIFI"),
  m_readKey_tile("/TGC/TRIGGER/CW_TILE"),
  m_writeKey("TGCTriggerData"),
  m_condSvc("CondSvc", name)
{
  declareProperty("ReadKeyBw", m_readKey_bw);
  declareProperty("ReadKeyEifi", m_readKey_eifi);
  declareProperty("ReadKeyTile", m_readKey_tile);
  declareProperty("WriteKey", m_writeKey);
}

StatusCode TGCTriggerDbAlg::initialize(){

  ATH_MSG_INFO( "initialize " << name() );                

  ATH_CHECK(m_condSvc.retrieve());
  
  ATH_CHECK(m_readKey_bw.initialize());
  ATH_CHECK(m_readKey_eifi.initialize());
  ATH_CHECK(m_readKey_tile.initialize());

  ATH_CHECK(m_writeKey.initialize());

  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TGCTriggerDbAlg::execute(){
  
  ATH_MSG_DEBUG( "start execute " << name() ); 
    
  SG::WriteCondHandle<TGCTriggerData> writeHandle{m_writeKey};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  auto writeCdo = std::make_unique<TGCTriggerData>();


  // Big wheel
  SG::ReadCondHandle<CondAttrListCollection> readHandle_bw(m_readKey_bw);
  const CondAttrListCollection* readCdo_bw(*readHandle_bw);
  
  if (readCdo_bw == 0) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE; 
  } 

  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle_bw.fullKey() << " readCdo->size()= " << readCdo_bw->size());
 
  EventIDRange rangeW_bw;
  if ( !readHandle_bw.range(rangeW_bw) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle_bw.key());
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO("Range of input is " << rangeW_bw);

  loadParameters(writeCdo.get(), readCdo_bw, TGCTriggerData::CW_BW);

  // EIFI
  SG::ReadCondHandle<CondAttrListCollection> readHandle_eifi(m_readKey_eifi);
  const CondAttrListCollection* readCdo_eifi(*readHandle_eifi);
  
  if (readCdo_eifi == 0) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE; 
  } 

  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle_eifi.fullKey() << " readCdo->size()= " << readCdo_eifi->size());
 
  EventIDRange rangeW_eifi;
  if ( !readHandle_eifi.range(rangeW_eifi) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle_eifi.key());
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO("Range of input is " << rangeW_eifi);

  loadParameters(writeCdo.get(), readCdo_eifi, TGCTriggerData::CW_EIFI);
 
  // Tile
  SG::ReadCondHandle<CondAttrListCollection> readHandle_tile(m_readKey_tile);
  const CondAttrListCollection* readCdo_tile(*readHandle_tile);
  
  if (readCdo_tile == 0) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE; 
  } 

  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle_tile.fullKey() << " readCdo->size()= " << readCdo_tile->size());
 
  EventIDRange rangeW_tile;
  if ( !readHandle_tile.range(rangeW_tile) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle_tile.key());
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO("Range of input is " << rangeW_tile);

  loadParameters(writeCdo.get(), readCdo_tile, TGCTriggerData::CW_TILE);

  // fill maps 
  fillReadMapBw(writeCdo.get());
  fillTrigBitEifi(writeCdo.get());
  fillTrigBitTile(writeCdo.get());


  // write condition object
  EventIDRange rangeIntersection = EventIDRange::intersect(rangeW_bw,rangeW_eifi,rangeW_tile);
  if(rangeIntersection.start()>rangeIntersection.stop()) {
    ATH_MSG_ERROR("Invalid intersection range: " << rangeIntersection);
    return StatusCode::FAILURE;
  }

  if (writeHandle.record(rangeIntersection, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record TGCTriggerData " << writeHandle.key() 
		  << " with EventRange " << rangeIntersection
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }		  
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeIntersection << " into Conditions Store");

   
  return StatusCode::SUCCESS;
}


StatusCode TGCTriggerDbAlg::finalize(){
  
  ATH_MSG_INFO( "finalize " << name() );
  return StatusCode::SUCCESS;
}


void TGCTriggerDbAlg::loadParameters(TGCTriggerData* writeCdo,
                                     const CondAttrListCollection* readCdo,
                                     int cw_type) {

  CondAttrListCollection::const_iterator itr = readCdo->begin();
  CondAttrListCollection::const_iterator itr_e = readCdo->end();

  for(; itr!=itr_e; ++itr) {
    const unsigned int channel = (*itr).first;
    const coral::AttributeList& atr = (*itr).second;

    ATH_MSG_DEBUG("channel: " << channel);
   
    std::string file    = *(static_cast<const std::string*>((atr["file"]).addressOfData()));
    std::string data    = *(static_cast<const std::string*>((atr["data"]).addressOfData()));
    std::string version = *(static_cast<const std::string*>((atr["version"]).addressOfData()));
    std::string type    = *(static_cast<const std::string*>((atr["type"]).addressOfData()));
    bool active         = *(static_cast<const bool*>((atr["active"]).addressOfData()));
 
    writeCdo->m_datamap[cw_type][file] = data;
    writeCdo->m_data[cw_type].push_back(data);
    writeCdo->m_file[cw_type].push_back(file);
    writeCdo->m_version[cw_type].push_back(version);
    writeCdo->m_type[cw_type].push_back(type);
    writeCdo->m_active[cw_type].push_back(active);

    ATH_MSG_INFO("active: " << active << " file: " << file);
    ATH_MSG_DEBUG("data: " << data);
  }
}


void TGCTriggerDbAlg::fillReadMapBw(TGCTriggerData* writeCdo)
{
  for(auto peroctant : writeCdo->m_ptmap_bw) {
    for(auto persubsec : peroctant.second) persubsec.second.clear();
  }

  if (!writeCdo->isActive(TGCTriggerData::CW_BW)) {
    return;
  }

  bool fullCW = (writeCdo->getType(TGCTriggerData::CW_BW) == "full");
  std::string vername = writeCdo->getVersion(TGCTriggerData::CW_BW);

  const int kNMODULETYPE = 12;
  const unsigned short modulenumber[kNMODULETYPE]    = {0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8};
  const std::string modulename[kNMODULETYPE]         = {"0","1","2a","2b","3","4","5a","5b","6","7","8a","8b"};
  const std::string sidename[TGCTriggerData::N_SIDE] = {"A","C"};

  for(size_t iSide = 0; iSide < TGCTriggerData::N_SIDE; iSide++) {
    for(size_t iOctant = 0; iOctant < TGCTriggerData::N_OCTANT; iOctant++) {

      std::map<unsigned short, std::map<unsigned short, unsigned char>> per_octant;
      //   SUBSECADDR | 16 bits | unsigned int  |

      for(int iModule = 0; iModule < kNMODULETYPE; iModule++) {

        std::ostringstream dbname;
        dbname << "RPhiCoincidenceMap.";
        if (fullCW) dbname << sidename[iSide] << iOctant << ".";
        dbname << "mod" << modulename[iModule] + "." + vername +"._12.db";

        std::string data = writeCdo->getData(TGCTriggerData::CW_BW, dbname.str());
        std::istringstream stream(data);

        char delimiter = '\n';
        std::string field, tag;
        unsigned char phimod2 = modulename[iModule].find("b") != std::string::npos ? 1 : 0;

        std::map<unsigned short, unsigned char> pt_by_delta;

        while (std::getline(stream, field, delimiter)) {

          std::istringstream header(field);
          header>>tag;

          if (tag == "#") { // read header part.
            unsigned short ssId, mod;
            short lDR, hDR, lDPhi, hDPhi;
            unsigned short ptLevel;
            header>>ptLevel>>ssId>>mod>>lDR>>hDR>>lDPhi>>hDPhi;

            char type = getTYPE( lDR, hDR, lDPhi, hDPhi );

            // check moduleNumber and ptLevel
            if (mod != modulenumber[iModule] || ptLevel > TGCTriggerData::N_PT_THRESH || type < 0 ) {
              ATH_MSG_WARNING("Invalid configuration of DB file.");
              break;
            }

            unsigned short roiaddr = getRoIAddr(type, phimod2, mod, ssId);

            // get window data
            std::getline(stream, field, delimiter);
            std::istringstream cont(field);

            for (unsigned short ir = 0; ir < 31; ir++) {
              unsigned int bit;
              cont>>bit;
              if (bit == 0) continue; // none of window is opened in this dR

              for(unsigned short iphi=0; iphi<15; iphi++) {
                unsigned short delta = (ir<<4) + iphi;
                if(bit>>iphi & 0x1) pt_by_delta[delta] = (unsigned char)(ptLevel & 0x7);
              }
            }
            if((ptLevel&0x7) == 0x6) {
              per_octant[roiaddr] = pt_by_delta;
              pt_by_delta.clear();
            }

          }   // end of if(tag)...
        }   // end of while(getline...)
      }   // end of for(iModule)

      unsigned char octantbit = iSide<<3 & iOctant;
      writeCdo->m_ptmap_bw[octantbit] = per_octant;

      if(!fullCW) break;
    }   // end of for(iOctant)
    if(!fullCW) break;
  }   // end of for(iSide)
}

char TGCTriggerDbAlg::getTYPE(const short lDR, const short hDR, const short lDPhi, const short hDPhi) const
{
  char type = -1;
  if ( (lDR==-15) && (hDR==15) && (lDPhi==-7) && (hDPhi==7))      type = TGCTriggerData::COIN_HH;
  else if ( (lDR==-15) && (hDR==15) && (lDPhi==-3) && (hDPhi==3)) type = TGCTriggerData::COIN_HL;
  else if ( (lDR==-7) && (hDR==7) && (lDPhi==-7) && (hDPhi==7))   type = TGCTriggerData::COIN_LH;
  else if ( (lDR==-7) && (hDR==7) && (lDPhi==-3) && (hDPhi==3))   type = TGCTriggerData::COIN_LL;
  return type;
}

unsigned short TGCTriggerDbAlg::getRoIAddr(const char type, const unsigned char phimod2,
                                           const unsigned short module, const unsigned short roi) const
{
  return (type<<13) + ((phimod2&0x1)<<12) + ((module&0xf)<<8) + roi;
}

void TGCTriggerDbAlg::fillTrigBitEifi(TGCTriggerData* writeCdo)
{
  // remove all entries
  for(size_t sector=0; sector<TGCTriggerData::N_ENDCAP_SECTOR; sector++) {
    for(size_t ssc=0; ssc<TGCTriggerData::N_ENDCAP_SSC; ssc++) {
      for(size_t pos=0; pos<TGCTriggerData::N_EIFI_INPUT; pos++) writeCdo->m_trigbit_eifi[sector][ssc][pos].clear();
      writeCdo->m_flagpt_eifi[sector][ssc].clear();
      writeCdo->m_flagroi_eifi[sector][ssc].clear();
    }
  }

  if (!writeCdo->isActive(TGCTriggerData::CW_EIFI)) {
    return;
  }

  bool fullCW = (writeCdo->getType(TGCTriggerData::CW_EIFI) == "full");
  std::string vername = writeCdo->getVersion(TGCTriggerData::CW_EIFI);
  const std::string sidename[TGCTriggerData::N_SIDE] = {"A","C"};

  for (int iSide = 0; iSide < TGCTriggerData::N_SIDE; iSide++) {

    std::string dbname="";
    if (!fullCW) {
      dbname = "InnerCoincidenceMap." + vername + "._12.db";
    } else {
      dbname = "InnerCoincidenceMap." + sidename[iSide]
               + "." + vername + "._12.db";
    }

    std::string data = writeCdo->getData(TGCTriggerData::CW_EIFI, dbname);
    std::istringstream stream(data);

    char delimiter = '\n';
    std::string field;
    std::string tag;

    while (std::getline(stream, field, delimiter)) {
      int sectorId = -1;
      int sscId    = -1;

      std::istringstream header(field);
      header >> tag;
      if(tag=="#"){ // read header part.
        header >> sectorId >> sscId;
      }

      if (sectorId < 0 || sectorId >= TGCTriggerData::N_ENDCAP_SECTOR ||
          sscId < 0    || sscId >= TGCTriggerData::N_ENDCAP_SSC ) {
        ATH_MSG_WARNING("Invalid configuration of DB file.");
        return;
      }

      unsigned char flag_pt = 0x0;
      for (size_t pt = 0; pt < TGCTriggerData::N_PT_THRESH; pt++){
        unsigned char use;
        header  >> use;
        flag_pt |= (use&0x1)<<pt;
      }
      writeCdo->m_flagpt_eifi[sectorId][sscId].push_back(flag_pt);

      unsigned char flag_roi = 0x0;
      for (size_t pos = 0; pos < TGCTriggerData::N_ROI_IN_SSC; pos++){
        unsigned char use;
        header >> use;
        flag_roi |= (use&0x1)<<pos;
      }
      writeCdo->m_flagroi_eifi[sectorId][sscId].push_back(flag_roi);

      std::getline(stream, field, delimiter);
      std::istringstream cont(field);
      for(size_t pos=0; pos < TGCTriggerData::N_EIFI_INPUT; pos++){
        unsigned int word;
        cont >> word;
        writeCdo->m_trigbit_eifi[sectorId][sscId][pos].push_back(word);
      }
    }  // end of while(std::geline(...))

    if (!fullCW) break;

  }  // end of for(iSide)
}

void TGCTriggerDbAlg::fillTrigBitTile(TGCTriggerData* writeCdo)
{
  if (!writeCdo->isActive(TGCTriggerData::CW_TILE)) {
    return;
  }

  std::string vername = writeCdo->getVersion(TGCTriggerData::CW_TILE);

  std::string dbname = "TileMuCoincidenceMap." + vername + "._12.db";

  std::string data = writeCdo->getData(TGCTriggerData::CW_TILE, dbname);
  std::istringstream stream(data);

  char delimiter = '\n';
  std::string field;
  std::string tag;

  while (std::getline(stream, field, delimiter)) {
    int sideId = -1;
    int sectorId = -1;
    int sscId    = -1;

    std::istringstream header(field);
    header >> tag;
    if (tag=="#"){ // read header part.
      header >> sideId >> sectorId >> sscId;
    }

    if(sideId < 0   || sideId >= TGCTriggerData::N_SIDE ||
       sectorId < 0 || sectorId >= TGCTriggerData::N_ENDCAP_SECTOR ||
       sscId < 0    || sscId >= TGCTriggerData::N_ENDCAP_SSC ) {
      ATH_MSG_WARNING("Invalid configuration of DB file.");
      return;
    }

    writeCdo->m_flagpt_tile[sscId][sectorId][sideId] = 0x0;
    for (size_t pt = 0; pt < TGCTriggerData::N_PT_THRESH; pt++){
      unsigned char use;
      header >> use;
      writeCdo->m_flagpt_tile[sscId][sectorId][sideId] |= (use&0x1)<<pt;
    }

    writeCdo->m_flagroi_tile[sscId][sectorId][sideId] = 0x0;
    for (size_t pos=0; pos< TGCTriggerData::N_ROI_IN_SSC; pos++){
      unsigned char use;
      header >> use;
      writeCdo->m_flagroi_tile[sscId][sectorId][sideId] |= (use&0x1)<<pos;
    }

    std::getline(stream, field, delimiter);
    std::istringstream cont(field);
    writeCdo->m_trigbit_tile[sscId][sectorId][sideId] = 0x0;
    for(size_t pos=0; pos < TGCTriggerData::N_TILE_INPUT; pos++){
      unsigned short word;
      cont >> word;
      writeCdo->m_trigbit_tile[sscId][sectorId][sideId] |= (word & 0xf)<<(pos*4);
    }
  }

}

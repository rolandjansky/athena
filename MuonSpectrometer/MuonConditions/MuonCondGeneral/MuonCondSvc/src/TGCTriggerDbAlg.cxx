/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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


void TGCTriggerDbAlg::fillReadMapBw(TGCTriggerData* writeCdo) {

  if (!writeCdo->isActive(TGCTriggerData::CW_BW)) {
    return;
  }

  const int NMODULETYPE = 12;
  const int NSIDE = 2;
  const int NOCTANT = 8;
  const int DROFFSET= -15;
 
  const int ModuleNumber[NMODULETYPE]       = {0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8};
  const std::string ModuleName[NMODULETYPE] = {"0","1","2a","2b","3","4","5a","5b","6","7","8a","8b"};
  const std::string SideName[NSIDE]         = {"A","C"};
  const std::string OctantName[NOCTANT]     = {"0", "1", "2", "3", "4", "5", "6", "7"};

  std::string fn, fullName, tag;
  int ssId, ptLevel, bit, mod;

  bool fullCW = (writeCdo->getType(TGCTriggerData::CW_BW) == "full" );
  std::string verName = writeCdo->getVersion(TGCTriggerData::CW_BW);

  for  (int iSide = 0; iSide < NSIDE; iSide++) {
    for  (int iOctant = 0; iOctant < NOCTANT; iOctant++) {
      for (int iModule = 0; iModule < NMODULETYPE; iModule++) {

        int phimod2 = ModuleName[iModule].find("b") != std::string::npos ? 1 : 0;
        std::ostringstream modName;
        std::string fn = "RPhiCoincidenceMap.mod"
                         + ModuleName[iModule] + "." + verName +"._12.db";
    
        if (fullCW) {
          if ((iSide >= 0) && (iSide < NSIDE) && (iOctant>=0) && (iOctant < NOCTANT)) {
            fn = "RPhiCoincidenceMap."
                      + SideName[iSide] + OctantName[iOctant]
                      + ".mod" + ModuleName[iModule]
                      + "." + verName +"._12.db";
          }
        }

        int type = -1;
        int lDR, hDR, lDPhi, hDPhi;

        std::string data = writeCdo->getData(TGCTriggerData::CW_BW, fn);
        std::istringstream stream(data);

        char delimiter = '\n';
        std::string field;

        while (std::getline(stream, field, delimiter)) {

          std::istringstream header(field);
          header>>tag;
          if (tag == "#") { // read header part.     
            header>>ptLevel>>ssId>>mod>>lDR>>hDR>>lDPhi>>hDPhi;
            type = getTYPE( lDR, hDR, lDPhi, hDPhi );

            // check moduleNumber and ptLevel
            if (mod != ModuleNumber[iModule] || ptLevel > TGCTriggerData::N_PT_THRESH || type < 0 ) {
              ATH_MSG_WARNING("Invalid configuration of DB file.");
              break;
            }

            // get window data
            std::getline(stream, field, delimiter);
            std::istringstream cont(field);
            std::map<int, int> aWindow;

            for (int ir = 0; ir <= hDR-DROFFSET; ir++) {
              cont>>bit;
              if (bit == 0) continue; // none of window is opened in this dR
              aWindow[ir+DROFFSET] = bit;
            }

            int addr = SUBSECTORADD(ssId,mod,phimod2,type);
            if (writeCdo->m_readmap_bw[iSide][iOctant][ptLevel-1].find(addr)!=writeCdo->m_readmap_bw[iSide][iOctant][ptLevel-1].end()) {
              ATH_MSG_WARNING("This subsector was already reserved.");

            } else {
              writeCdo->m_readmap_bw[iSide][iOctant][ptLevel-1][addr] = aWindow; 
            }
          } 
        }
      }
    }
  }
}

int TGCTriggerDbAlg::getTYPE(int lDR, int hDR, int lDPhi, int hDPhi ) const {
  int type = -1;
  if ( (lDR==-15) && (hDR==15) && (lDPhi==-7) && (hDPhi==7))      type = TGCTriggerDbAlg::TMap_HH;
  else if ( (lDR==-15) && (hDR==15) && (lDPhi==-3) && (hDPhi==3)) type = TGCTriggerDbAlg::TMap_HL;
  else if ( (lDR==-7) && (hDR==7) && (lDPhi==-7) && (hDPhi==7))   type = TGCTriggerDbAlg::TMap_LH;
  else if ( (lDR==-7) && (hDR==7) && (lDPhi==-3) && (hDPhi==3))   type = TGCTriggerDbAlg::TMap_LL;
  return type;
}

int TGCTriggerDbAlg::SUBSECTORADD(int ssid, int modid, int phimod2, int type) const { 
  return (ssid+(modid<<8)+(phimod2<<12) + (type<<16) ); 
}

void TGCTriggerDbAlg::fillTrigBitEifi(TGCTriggerData* writeCdo) {

  if (!writeCdo->isActive(TGCTriggerData::CW_EIFI)) {
    return;
  }

  const int NSIDE = 2;
  const std::string SideName[NSIDE] = {"A","C"};
  
  bool fullCW = (writeCdo->getType(TGCTriggerData::CW_EIFI) == "full");
  std::string verName = writeCdo->getVersion(TGCTriggerData::CW_EIFI);

  for (int iSide = 0; iSide < NSIDE; iSide++) {

    std::string dbname="";
    if (!fullCW) {
      dbname = "InnerCoincidenceMap." + verName + "._12.db";
    } else {
      dbname = "InnerCoincidenceMap." + SideName[iSide]
               + "." + verName + "._12.db";
    }

    std::string data = writeCdo->getData(TGCTriggerData::CW_EIFI, dbname);
    std::istringstream stream(data);

    char delimiter = '\n';
    std::string field;
    std::string tag;

    while (std::getline(stream, field, delimiter)) {
      int sectorId = -1;
      int sscId    = -1;
      int use[TGCTriggerData::N_PT_THRESH]  = {0, 0, 0, 0, 0, 0};
      int roi[TGCTriggerData::N_ROI_IN_SSC] = {1, 1, 1, 1, 1, 1, 1, 1};
      std::istringstream header(field);
      header >> tag;
      if(tag=="#"){ // read header part.
        header >> sectorId >> sscId
               >> use[0] >> use[1] >> use[2]
               >> use[3] >> use[4] >> use[5]
               >> roi[0] >> roi[1] >> roi[2] >> roi[3]
               >> roi[4] >> roi[5] >> roi[6] >> roi[7];
      }

      if (sectorId < 0 || sectorId >= TGCTriggerData::N_ENDCAP_SECTOR ||
          sscId < 0    || sscId >= TGCTriggerData::N_ENDCAP_SSC ) {
        ATH_MSG_WARNING("Invalid configuration of DB file.");
        return;
      }

      for (size_t pt = 0; pt < TGCTriggerData::N_PT_THRESH; pt++){
        writeCdo->m_flagpt_eifi[iSide][pt][sscId][sectorId] = use[pt];
      }
      for (size_t pos = 0; pos < TGCTriggerData::N_ROI_IN_SSC; pos++){
        writeCdo->m_flagroi_eifi[iSide][pos][sscId][sectorId] = roi[pos];
      }

      std::getline(stream, field, delimiter);
      std::istringstream cont(field);
      unsigned int word;
      for(size_t pos=0; pos < TGCTriggerData::N_EIFI_INPUT; pos++){
        cont >> word;

        unsigned int tstBit =1;
        for (unsigned int region = 0; region < TGCTriggerData::N_EIFI_REGION; region++) {
          for (unsigned int readout = 0; readout < TGCTriggerData::N_EIFI_READOUT; readout++) {
            for (unsigned int iBit = 0; iBit < TGCTriggerData::N_EIFI_TRIGBIT; iBit++) {
              writeCdo->m_trigbit_eifi[iSide][pos][sscId][sectorId][region][readout][iBit] = ((tstBit & word) !=0 );
              tstBit = tstBit*2;
            }
          }
        }
      }
    }
  } 
}

void TGCTriggerDbAlg::fillTrigBitTile(TGCTriggerData* writeCdo) {

  if (!writeCdo->isActive(TGCTriggerData::CW_TILE)) {
    return;
  }

  std::string verName = writeCdo->getVersion(TGCTriggerData::CW_TILE);

  std::string dbname="";
  dbname = "TileMuCoincidenceMap." + verName + "._12.db";

  std::string data = writeCdo->getData(TGCTriggerData::CW_TILE, dbname);
  std::istringstream stream(data);

  char delimiter = '\n';
  std::string field;
  std::string tag;

  while (std::getline(stream, field, delimiter)) {
    int sideId = -1;
    int sectorId = -1;
    int sscId    = -1;
    int use[TGCTriggerData::N_PT_THRESH] = {0, 0, 0, 0, 0, 0};
    int roi[TGCTriggerData::N_ROI_IN_SSC] = {1, 1, 1, 1, 1, 1, 1, 1};

    std::istringstream header(field);
    header >> tag;
    if (tag=="#"){ // read header part.
      header >> sideId >> sectorId >> sscId
             >> use[0] >> use[1] >> use[2]
             >> use[3] >> use[4] >> use[5]
             >> roi[0] >> roi[1] >> roi[2] >> roi[3]
             >> roi[4] >> roi[5] >> roi[6] >> roi[7];
    }

    if (sideId < 0   || sideId >= TGCTriggerData::N_SIDE ||
      sectorId < 0 || sectorId >= TGCTriggerData::N_ENDCAP_SECTOR ||
      sscId < 0    || sscId >= TGCTriggerData::N_ENDCAP_SSC ) {
        ATH_MSG_WARNING("Invalid configuration of DB file.");
        return;
    }

    for (size_t pt = 0; pt < TGCTriggerData::N_PT_THRESH; pt++){
      writeCdo->m_flagpt_tile[pt][sscId][sectorId][sideId]   = use[pt];
    }
    for (size_t pos=0; pos< TGCTriggerData::N_ROI_IN_SSC; pos++){
      writeCdo->m_flagroi_tile[pos][sscId][sectorId][sideId] = roi[pos];
    }

    std::getline(stream, field, delimiter);
    std::istringstream cont(field);
    unsigned int word;
    for(size_t pos=0; pos < TGCTriggerData::N_TILE_INPUT; pos++){
      cont >> word;
      writeCdo->m_trigbit_tile[pos][sscId][sectorId][sideId] = word;
    }
  }
}

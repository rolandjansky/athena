/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

#include "RPC_CondCabling/RpcCablingCondAlg.h"

RpcCablingCondAlg::RpcCablingCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_ConfMapPString(nullptr),
  m_MaxType(0),
  m_SectorMap(),
  m_Version(""),
  m_condSvc("CondSvc",name)
{
  declareProperty( "CosmicConfiguration", m_cosmic_configuration=false );
  declareProperty("ApplyFeetPadThresholds", m_ApplyFeetPadThresholds=true, "map 3 low pt thresholds from special feet pads on standard 6 (3low+3high)");
  declareProperty("ForceFeetPadThresholdsFromJO", m_ForceFeetPadThresholdsFromJO=false, "JO override db setting"); 
}

StatusCode RpcCablingCondAlg::initialize() {

  ATH_MSG_DEBUG( "initializing" << name() );
  ATH_CHECK( m_condSvc.retrieve() );
  ATH_CHECK( m_readKey_map_schema.initialize() );
  ATH_CHECK( m_readKey_map_schema_corr.initialize() );
  ATH_CHECK( m_readKey_cm_thr_eta.initialize() );
  ATH_CHECK( m_readKey_cm_thr_phi.initialize() );
  ATH_CHECK( m_writeKey.initialize() );
  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_idHelperSvc.retrieve());
  RDOindex::setRpcIdHelper(&m_idHelperSvc->rpcIdHelper());

  return StatusCode::SUCCESS;
}


StatusCode RpcCablingCondAlg::execute() 
{
  ATH_MSG_DEBUG( "executing" << name() );

  // Write CondHandle
  SG::WriteCondHandle<RpcCablingCondData> writeHandle{m_writeKey};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
        << ". In theory this should not be called, but may happen"
        << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }  
  // define the write condDataObj. Will only  read in the 4 different cond objects
  std::unique_ptr<RpcCablingCondData> writeCdo{std::make_unique<RpcCablingCondData>()};

  // Read CondHandle


  // read in the 4 different cond objects

  // map
  SG::ReadCondHandle<CondAttrListCollection> readHandle_map_schema{m_readKey_map_schema};
  const CondAttrListCollection* readCdoMap{*readHandle_map_schema}; 
  if(readCdoMap==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection readCdoMap->size()= " << readCdoMap->size()); 

  // corr
  SG::ReadCondHandle<CondAttrListCollection> readHandle_map_schema_corr{m_readKey_map_schema_corr};
  const CondAttrListCollection* readCdoCorr{*readHandle_map_schema_corr}; 
  if(readCdoCorr==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection readCdoMap_corr->size()= " << readCdoCorr->size()); 

  // EtaTable
  SG::ReadCondHandle<CondAttrListCollection> readHandle_cm_thr_eta{m_readKey_cm_thr_eta};
  const CondAttrListCollection* readCdoEta{*readHandle_cm_thr_eta}; 
  if(readCdoEta==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection readCdo_cm_thr_eta->size()= " << readCdoEta->size()); 

  // phiTable
  SG::ReadCondHandle<CondAttrListCollection> readHandle_cm_thr_phi{m_readKey_cm_thr_phi};
  const CondAttrListCollection* readCdoPhi{*readHandle_cm_thr_phi}; 
  if(readCdoPhi==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection readCdo_cm_thr_phi->size()= " << readCdoPhi->size()); 


  // set the range of each of the read handles

  EventIDRange range_map_schema;
  if(!readHandle_map_schema.range(range_map_schema)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle_map_schema.key());
    return StatusCode::FAILURE;
  }

  EventIDRange range_map_schema_corr;
  if(!readHandle_map_schema_corr.range(range_map_schema_corr)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle_map_schema_corr.key());
    return StatusCode::FAILURE;
  }

  EventIDRange range_cm_thr_eta;
  if(!readHandle_cm_thr_eta.range(range_cm_thr_eta)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle_cm_thr_eta.key());
    return StatusCode::FAILURE;
  }

  EventIDRange range_cm_thr_phi;
  if(!readHandle_cm_thr_phi.range(range_cm_thr_phi)) {
    ATH_MSG_FATAL( "Failed to retrieve validity range for " << readHandle_cm_thr_phi.key() );
    return StatusCode::FAILURE;
  }

  // Create an intersection of input IOVs
  EventIDRange rangeIntersection = EventIDRange::intersect(range_map_schema, range_map_schema_corr, range_cm_thr_eta, range_cm_thr_phi);
  if(rangeIntersection.start()>rangeIntersection.stop()) {
    ATH_MSG_ERROR("Invalid intersection range: " << rangeIntersection);
    return StatusCode::FAILURE;
  }

  ATH_CHECK(loadParameters(readCdoMap,readCdoCorr,readCdoEta,readCdoPhi) );
  ATH_CHECK(ReadConf());
  ATH_CHECK(buildRDOmap(writeCdo.get()));

  if (writeHandle.record( range_map_schema, std::move(writeCdo)).isFailure())
  { 
    ATH_MSG_FATAL("Could not record RpcCondCablingData " << writeHandle.key() << " with EventRange " <<  range_map_schema << " into Conditions Store");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " <<  range_map_schema );

  return StatusCode::SUCCESS;
}


StatusCode RpcCablingCondAlg::loadParameters(const CondAttrListCollection* readCdoMap,
                                             const CondAttrListCollection* readCdoCorr,
                                             const CondAttrListCollection* readCdoEta,
                                             const CondAttrListCollection* readCdoPhi)
{

  ATH_MSG_DEBUG( "RpcCablingCondAlg::loadParameters()" << name() );



  // begin like RPCCablingDbTool::loadRPCMap() -----------

  ATH_MSG_DEBUG( "--- excute --- loadParameters --- Read Map from DB" ); 

  CondAttrListCollection::const_iterator itrMap;
  int ic=0;
  for (itrMap = readCdoMap->begin(); itrMap != readCdoMap->end(); ++itrMap) 
  {
    ic++;
    const coral::AttributeList& atr=itrMap->second;
    m_ConfMapPString = static_cast<const std::string*>((atr["Map"]).addressOfData());    
  }

  // begin like RPCCablingDbTool::loadRPCCorr() -----------

  ATH_MSG_DEBUG( "excute --- Read Corrections from DB" ); 

  CondAttrListCollection::const_iterator itrCorr;
  for (itrCorr = readCdoCorr->begin(); itrCorr != readCdoCorr->end(); ++itrCorr) 
  {
    const coral::AttributeList& atr=itrCorr->second;
    m_corr = static_cast<const std::string*>((atr["Map"]).addressOfData());
    ATH_MSG_VERBOSE( "Sequence read is \n" << m_corr ); 
    ATH_MSG_VERBOSE( "End of Sequence read and write" );
  }

  // begin like RPCCablingDbTool::loadRPCEtaTable() -----------

  ATH_MSG_DEBUG( "excute --- Read RPCEtaTable from DB" ); 

  CondAttrListCollection::const_iterator itrEta;
  ic=0;
  for (itrEta = readCdoEta->begin(); itrEta != readCdoEta->end(); ++itrEta) {
    ic++;
    ATH_MSG_VERBOSE( "Loop over CondAttrListCollection ic = " << ic ); 
    const coral::AttributeList& atr=itrEta->second;

    std::string etaCM_File = *(static_cast<const std::string*>((atr["CM_File"]).addressOfData()));    
    m_vecetaCM_File.push_back(etaCM_File);
    std::string etaTh0 = *(static_cast<const std::string*>((atr["Th0"]).addressOfData()));      
    m_vecetaTh0.push_back(etaTh0);
    m_trigroads[etaCM_File]=etaTh0;
    std::string etaSequence_Th = *(static_cast<const std::string*>((atr["Sequence_Th"]).addressOfData()));  
    m_vecetaSequence_Th.push_back(etaSequence_Th);
    std::string etaInfo = *(static_cast<const std::string*>((atr["Additional_Info"]).addressOfData()));    
    m_vecetaInfo.push_back(etaInfo);

    ATH_MSG_VERBOSE( "column eta CM_File is \n" << etaCM_File ); 
    ATH_MSG_VERBOSE( "column eta Th0 is \n" << etaTh0 ); 
    ATH_MSG_VERBOSE( "column eta Sequence_Th is \n" << etaSequence_Th ); 
    ATH_MSG_VERBOSE( "column eta Additional_Info is \n" << etaInfo ); 
    ATH_MSG_VERBOSE( "End of Sequence read and write" );

  }

  // begin like RPCCablingDbTool::loadRPCEPhiTable() -----------

  ATH_MSG_DEBUG( "excute --- Read RPCPhiTable from DB" ); 

  CondAttrListCollection::const_iterator itrPhi;
  ic=0;
  for (itrPhi = readCdoPhi->begin(); itrPhi != readCdoPhi->end(); ++itrPhi) {
    ic++;
    ATH_MSG_VERBOSE( "Loop over CondAttrListCollection ic = " << ic ); 
    const coral::AttributeList& atr=itrPhi->second;

    std::string phiCM_File;
    std::string phiTh0;    
    std::string phiInfo;   

    phiCM_File = *(static_cast<const std::string*>((atr["CM_File"]).addressOfData()));    
    m_vecphiCM_File.push_back(phiCM_File);   
    phiTh0 = *(static_cast<const std::string*>((atr["Th0"]).addressOfData()));      
    m_vecphiTh0.push_back(phiTh0);
    m_trigroads[phiCM_File]=phiTh0;
    phiInfo = *(static_cast<const std::string*>((atr["Additional_Info"]).addressOfData()));    
    m_vecphiInfo.push_back(phiInfo);

    ATH_MSG_VERBOSE( "column phi CM_File is \n" << phiCM_File );
    ATH_MSG_VERBOSE( "column phi Th0 is \n" << phiTh0 );
    ATH_MSG_VERBOSE( "column phi Additional_Info is \n" << phiInfo);
    ATH_MSG_VERBOSE( "End of Sequence read and write" );

  }

  return StatusCode::SUCCESS;

}

// begin like CablingRPC::ReadConf() -----------  

StatusCode RpcCablingCondAlg::ReadConf()
{

  ATH_MSG_DEBUG("--->> RPC cabling map from COOL <<"); 
  ATH_MSG_DEBUG("--- ReadConf: map is " << *m_ConfMapPString);
  ATH_MSG_DEBUG("--- ReadConf: map is at    " << (uintptr_t)(m_ConfMapPString) );
  ATH_MSG_DEBUG("--- ReadConf: map has size " << m_ConfMapPString->size() );
  ATH_MSG_DEBUG("--- ReadConf: m_MaxType of types is " << m_MaxType );
  
  if(!m_ConfMapPString) {
    ATH_MSG_ERROR("Pointer to cabling map not set");
    return StatusCode::FAILURE;
  }

  std::stringstream MAP;
  MAP.str(*m_ConfMapPString);
  // Create the reader for the database
  DBline data(MAP);

  // store the setup environment    
  std::string version,setup,layout;

  // Start reading routine
  unsigned int nlines=0;

  // Start reading routine
  ATH_MSG_DEBUG( "--- ReadConf: m_MaxType of types is " << m_MaxType );
  while (++data)
  {	
    ATH_MSG_DEBUG("--- ReadConf: Reading routine. nlines = " << nlines );
    ++nlines;
    data("Version") >> version >> setup >> layout;
    ATH_MSG_DEBUG("--- ReadConf:"  << version << setup << layout);

    if (setup == "Testbeam") m_TestbeamFlag = true;

    // Read the Sector Type enumeration
    int start = 0;
    int stop  = 0;
    if(data("SECTOR TYPES (") >> start >> "-" >> stop >> "):")
      for(int i=start;i<=stop;++i) data >> m_SectorMap[i];
      


    // Set the m_MaxType variable and the type of SectorMap objects
    if(stop == 63 || stop == 8)
    {   
      for(int i=0;i<64;++i) if(m_SectorMap[i] > m_MaxType) m_MaxType = m_SectorMap[i];
      m_SectorType.resize(m_MaxType);
      ATH_MSG_DEBUG( "--- ReadConf: m_MaxType  of types is " << m_MaxType );

      ATH_MSG_DEBUG( "--- ReadConf: Loop over sector-types");
      for(int i=1;i<=m_MaxType;++i)
      { 
        m_SectorType[i-1] = RPC_CondCabling::SectorLogicSetup(i,m_DataName,layout,m_cosmic_configuration);
        RPC_CondCabling::SectorLogicSetup* sec = &(m_SectorType[i-1]);
        for(int j=0;j<64;++j) 
        {
          if(m_SectorMap[j] == i) { *sec << j; m_SectorLogic.insert(SLmap::value_type(j,sec)); }
          ATH_MSG_DEBUG( " filling sectorLogicSetup Map for type "<< i << " sector  "<< j );
        }
      }
    }        

    // Loop on GEOMETRY TYPES
    for(int i=1;i<=m_MaxType;++i)
    {	    
      // Read the RPC geometry
      if(data("RPC GEOM  # :",i))
      {
        RPC_CondCabling::RPCchamberdata RPCdata(data,i);
        if(!(m_SectorType[i-1] += RPCdata)) return StatusCode::FAILURE;
      }
      // Read the Wired OR geometry
      if(data("WIRED OR  # :",i))
      {
        RPC_CondCabling::WiredORdata WORdata(data,i);
        if(!(m_SectorType[i-1] += WORdata)) return StatusCode::FAILURE;
      }
      // Read the CMAs segmentation
      if(data("CMAs  # : pivot segmentation",i))
      {
        RPC_CondCabling::CMApivotdata CMAdata(data,i,layout);
        if(!(m_SectorType[i-1] += CMAdata)) return StatusCode::FAILURE;
      }
      // Read the CMAs cabling
      if(data("CMAs  # : eta cabling",i))
      {
        RPC_CondCabling::CMAcablingdata CMAdata(data,i);
        if(!(m_SectorType[i-1] += CMAdata)) return StatusCode::FAILURE;
      }
    }
  }
  ATH_MSG_DEBUG("--- ReadConf: map n. of lines read is "<< nlines);

  m_Version += version + " " + setup + " " + layout;
  ATH_MSG_DEBUG("RpcCablingCondAlg--- ReadConf: version is " << m_Version);


  // Setup of the LVL-1 configuration
  for(int i=1;i<=m_MaxType;++i) if(!m_SectorType[i-1].setup()) return StatusCode::FAILURE;
  for(int i=1;i<=m_MaxType;++i) if(!m_SectorType[i-1].check()) return StatusCode::FAILURE; 

  ATH_MSG_INFO("maps configuration have been parsed");

  return StatusCode::SUCCESS;

}


// build and init the map for the PAD RDO
StatusCode RpcCablingCondAlg::buildRDOmap(RpcCablingCondData* writeCdo)
{
  ATH_MSG_DEBUG("RpcCablingCondAlg--- buildRDOmap"); 

  for (int sector = 0; sector < 64; ++sector)
  {
    unsigned int nRDOentries=0;
    int side = (sector<32)? 0 : 1;
    int logic_sector = sector % 32;

    if (m_SectorMap[sector])
    {
      // get the Sector Logic Setup
      RPC_CondCabling::SectorLogicSetup Sector = m_SectorType[m_SectorMap[sector] - 1];

      // get the Eta CMA map from the Sector Logic Setup
      const RPC_CondCabling::SectorLogicSetup::EtaCMAmap CMAs = Sector.giveEtaCMA();
      RPC_CondCabling::SectorLogicSetup::EtaCMAmap::const_iterator it = CMAs.begin();

      bool isFirst = false;

      // loop over the whole set of Eta CMAs
      while(it != CMAs.end())
      {
        // get the set of parameters for idenfying the first RPC strip
        unsigned int ID = (*it).second.id().Ixx_index();
        bool inversion  = (*it).second.inversion();
        if ((ID==1 && inversion) || (ID==0 && !inversion)) isFirst = true;
        else isFirst = false;
        unsigned int PADid = (*it).first.PAD_index();
        unsigned int cabling;
        if( (*it).second.get_cabling(Pivot,0,0,0,cabling) ) 
        {
          unsigned int RPC_strip = cabling%100;
          unsigned int RPC_chamber = (cabling/100)%100;
          unsigned int RPC_layer = 0;
          unsigned int RPC_station = (*it).second.whichCMAstation(Pivot);
          unsigned int lvl1_sector = sector;

          RPC_CondCabling::RPCchamber* rpc = Sector.find_chamber(RPC_station, RPC_chamber);
          std::string name = rpc->stationName();
          int sEta = (side)? rpc->stationEta() : -rpc->stationEta();
          int sPhi = (logic_sector==31)? 1 : (logic_sector+1)/4 +1;
          int dR   = rpc->doubletR();
          int dZ   = rpc->doubletZ();
          int dP   = (rpc->phiReadoutPannels()==2)? (logic_sector+1)%2+1: 1;

          // build the Level-1 code index
          RPCdecoder decode(Eta,lvl1_sector,RPC_station,RPC_layer, RPC_chamber,RPC_strip);

          // instanciate the corresponding RDO index
          RDOindex rdo(PADid,decode.code(),name,sEta,sPhi,dR,dZ,dP);

          // compute the key for retrieving RDO into the map
          int key = side*10000 + logic_sector*100 + PADid;
          // insert the RDO into the map
          std::pair < RpcCablingCondData::RDOmap::iterator, bool> ins = writeCdo->m_RDOs.insert(RpcCablingCondData::RDOmap::value_type(key,rdo));

          ATH_CHECK(ins.second);

          ++nRDOentries;
        }

        else if(!(*it).second.get_cabling(Pivot,0,0,0,cabling) && isFirst)
        {
          // try to catch configrm cabling
          bool existLow  = false;
          bool existHigh = false;

          if( !(existLow=(*it).second.get_cabling(LowPt,0,0,0,cabling)) )
            existHigh = (*it).second.get_cabling(HighPt,0,0,0,cabling);

          if (!existLow && !existHigh)
          {
            ATH_MSG_ERROR("Error while configuring the RDO map");
            return StatusCode::FAILURE;
          }

          unsigned int RPC_strip = cabling%100;
          unsigned int RPC_chamber = (cabling/100)%100;
          unsigned int RPC_layer = 0;
          unsigned int RPC_station = 0;
          if(existLow) RPC_station = (*it).second.whichCMAstation(LowPt);
          else         RPC_station = (*it).second.whichCMAstation(HighPt);
          unsigned int lvl1_sector = sector;

          RPC_CondCabling::RPCchamber* rpc = Sector.find_chamber(RPC_station, RPC_chamber);
          std::string name = rpc->stationName();
          int sEta = (side)? rpc->stationEta() : -rpc->stationEta();
          int sPhi = (logic_sector==31)? 1 : (logic_sector+1)/4 +1;
          int dR   = rpc->doubletR();
          int dZ   = rpc->doubletZ();
          int dP   = (rpc->phiReadoutPannels()==2)? (logic_sector+1)%2+1: 1;

          // build the Level-1 code index
          RPCdecoder decode(Eta,lvl1_sector,RPC_station,RPC_layer,
              RPC_chamber,RPC_strip);

          // instanciate the corresponding RDO index
          RDOindex rdo(PADid,decode.code(),name,sEta,sPhi,dR,dZ,dP);

          // compute the key for retrieving RDO into the map
          int key = side*10000 + logic_sector*100 + PADid;

          // insert the RDO into the map
          std::pair < RpcCablingCondData::RDOmap::iterator, bool> ins = writeCdo->m_RDOs.insert(RpcCablingCondData::RDOmap::value_type(key,rdo));

          ATH_CHECK (ins.second);

          ++nRDOentries;
        }

        // increase the iterator            
        while( it != CMAs.end() && (unsigned int)(*it).first.PAD_index() == PADid ) {++it;}	    
      }
    }
    ATH_MSG_DEBUG("RpcCablingConAlg--- buildRDOmap: sector "<<sector<<" n of associated RDO (i.e. Pads) = " << nRDOentries);

  }

  for(int side=0;side<2;++side) for(int rod=0;rod<16;++rod) (writeCdo->m_rod2hash[side][rod]).clear();

  // ------ begin like  RPC_CondCablingSvc::buildOfflineOnlineMap()

  int hashID=0;
  std::set< uint32_t > ROBid;
  IdContext rpcModuleContext = m_idHelperSvc->rpcIdHelper().module_context();

  // reserve enough space in the hash-vector
  writeCdo->m_HashVec.reserve(writeCdo->m_RDOs.size());

  RpcCablingCondData::RDOmap::iterator pad_beg = writeCdo->m_RDOs.begin();
  RpcCablingCondData::RDOmap::iterator pad_end = writeCdo->m_RDOs.end();
  for (; pad_beg!=pad_end; ++pad_beg)
  {
    const RDOindex* pRDOindex = &((*pad_beg).second);
    (*pad_beg).second.set_hash(hashID);

    ++hashID;
    // get pointer to RDOindex class
    writeCdo->m_HashVec.push_back(pRDOindex);
    if( writeCdo->m_HashVec.size() != pRDOindex->hash()+1 ) {
      ATH_MSG_ERROR("Size of hash vector and RDO hash does not match");
      return StatusCode::FAILURE;
    }

    // calculate  m_fullListOfRobIds
    const unsigned short int rob_id = pRDOindex->ROBid();
    const unsigned short int rod_id = pRDOindex->RODid();
    const unsigned short int sub_id = pRDOindex->side();
    const unsigned short int sec_id = pRDOindex->SLid();
    const unsigned short int pad_id = pRDOindex->PADid();

    uint32_t ROD_ID = (sub_id << 16) | rod_id;
    uint32_t ROB_ID = (sub_id << 16) | rob_id;

    ROBid.insert(ROB_ID); 


    //// -------------

    unsigned short int sub_id_index = ((sub_id==0x65)? 1:0); // convert 0x65 -> 1 (side A) and 0x66 -> 0 (side C)

    Identifier id;
    pRDOindex->pad_identifier( id );

    //build the offline_id vector
    writeCdo->m_offline_id[sub_id_index][sec_id][pad_id] = id;

    // build the map
    std::pair < RpcCablingCondData::OfflineOnlineMap::iterator, bool> ins = writeCdo->m_RDOmap.insert( RpcCablingCondData::OfflineOnlineMap::value_type(id,pRDOindex));
    ATH_MSG_DEBUG("OfflineOnlineMap new entry: value  "<< m_idHelperSvc ->rpcIdHelper().show_to_string(id) << 
                  "hash of the RDOindex (key) = " << pRDOindex->hash());
    if(!ins.second) {
      ATH_MSG_ERROR("RpcCablingCondData::OfflineOnlineMap is false for value "<< m_idHelperSvc->rpcIdHelper().show_to_string(id) << " and hash of the RDOindex (key) = " << pRDOindex->hash());
      return StatusCode::FAILURE;
    }

    //build the ROB->RDO map
    std::pair<std::set<IdentifierHash>::iterator, bool> insert_ROB_RDO_returnVal = writeCdo->m_ROB_RDO_map[ROB_ID].insert(IdentifierHash(pRDOindex->hash()));
    if (insert_ROB_RDO_returnVal.second)
      ATH_MSG_DEBUG("A new RDO HashId = " << pRDOindex->hash() << " registered for ROB Id = " << ROB_ID);
    else
      ATH_MSG_VERBOSE("The RDO HashId = " << pRDOindex->hash() << " was already registered for ROB Id = " << ROB_ID);

    //build the PRD->RDO and PRD->ROB maps
    ATH_MSG_VERBOSE("Looking for PRDs corresponding to this RDO");
    std::list<Identifier> strip_id_list;
    IdentifierHash rdoHashId( (IdentifierHash::value_type)pRDOindex->hash() );
    ATH_MSG_VERBOSE("RDO HashId = " << (int)rdoHashId << " RDO Id = " << id.getString() 
                    << " ROB Id = " << MSG::hex << ROB_ID << MSG::dec
                    << " ROD Id = " << MSG::hex << ROD_ID << MSG::dec);

    for (unsigned short int CMAId : {2, 3, 6 ,7}) { //loop over phi CMA IDs
     for (unsigned short int ijk : {1, 2, 3}) { //loop over IJK identifiers
       strip_id_list.clear();
       for (unsigned short int channel : {0, 31}) {//check for the first and the last channel
         strip_id_list.splice( strip_id_list.end(), give_strip_id(sub_id_index, sec_id, pad_id, CMAId, ijk, channel) );
       }
       for (Identifier strip_id : strip_id_list) {
         Identifier idp = m_idHelperSvc->rpcIdHelper().parentID(strip_id);
         IdentifierHash prdHashId;
         int gethash_code = m_idHelperSvc->rpcIdHelper().get_hash(idp, prdHashId, &rpcModuleContext);
         if (gethash_code != 0) {
           ATH_MSG_DEBUG("Unable to get the PRD HashId! parentID(strip_id)=" << idp.getString());
           continue;
         }

         //fill the PRD->RDO map
         std::pair<std::set<IdentifierHash>::iterator, bool> insertRDO_returnVal = writeCdo->m_PRD_RDO_map[prdHashId].insert(rdoHashId);
         if (insertRDO_returnVal.second) {
           ATH_MSG_DEBUG("A new RDO HashId = " << (int)rdoHashId << " registered for PRD HashId = " << (int)prdHashId);
         } else {
           ATH_MSG_VERBOSE("The RDO HashId = " << (int)rdoHashId << " was already registered for PRD HashId = " << (int)prdHashId);
         }

         //fill the PRD->ROB map
         std::pair<std::set<uint32_t>::iterator, bool> insertROB_returnVal = writeCdo->m_PRD_ROB_map[prdHashId].insert(ROB_ID);
         if (insertROB_returnVal.second) {
           ATH_MSG_DEBUG("A new ROB Id = " << MSG::hex << ROB_ID << MSG::dec << " registered for PRD HashId = " << (int)prdHashId);
         } else {
           ATH_MSG_VERBOSE("The ROB Id = " << MSG::hex << ROB_ID << MSG::dec << " was already registered for PRD HashId = " << (int)prdHashId);
         }

       }
     }
    }

    // ----   begin like MuonRPC_CablingSvc::initTrigRoadsModel()

    // Trigger Roads Header 
    std::map<std::string, std::string>::const_iterator it;
    it=m_trigroads.find("infos.txt");
    if (it==m_trigroads.end()){
      ATH_MSG_WARNING("Missing HEADER FILE infos.txt");
    } else {
      ATH_MSG_VERBOSE("======== RPC Trigger Roads from COOL - Header infos ========");
      ATH_MSG_VERBOSE("\n"+it->second+"\n");
      // Read FeetPadThresholds from infos.txt
      if (!m_ForceFeetPadThresholdsFromJO){             
        std::stringstream ss;
        ss << it->second;
        std::string word;
        while (ss >> word){
          if (word=="FeetPadThresholds"){
            m_FeetPadThresholds.assign(3,0);
            ss >> m_FeetPadThresholds.at(0);
            ss >> m_FeetPadThresholds.at(1);
            ss >> m_FeetPadThresholds.at(2);
            ATH_MSG_VERBOSE("FeetPadThresholds set from COOL to: "
                            <<m_FeetPadThresholds.at(0)<<","
                            <<m_FeetPadThresholds.at(1)<<","
                            <<m_FeetPadThresholds.at(2));
          }
        }
      }
    }
           
    // this must be done both in case of source = COOL or ASCII
    // -----  Initialization of Pad configuration ------ //
    if (m_ApplyFeetPadThresholds) {
      // if using COOL check the existence of a PAD not existing in run-1 cabling
      // if (!giveOffflineID(0,21,7,offline_id)&&m_RPCTriggerRoadsfromCool) {
      //   ATH_MSG_INFO("RUN-1 like cabling, not applying FeetPadThresholds");
      // }else{
        if (m_FeetPadThresholds.size()!=3){
          // if thresholds vector empty, set it to default
          m_FeetPadThresholds.assign(3,0);
          m_FeetPadThresholds.at(0)=0;
          m_FeetPadThresholds.at(1)=2;
          m_FeetPadThresholds.at(2)=5;
        }
        ATH_MSG_VERBOSE("Applying FeetPadThresholds : "
          <<  m_FeetPadThresholds.at(0) << ","
          <<  m_FeetPadThresholds.at(1) << ","
          <<  m_FeetPadThresholds.at(2));

        const unsigned int NumFeetSectors = 8;
        unsigned int FeetSectors[NumFeetSectors]={21,22,25,26,53,54,57,58};
        const unsigned int NumSpecialFeetPads = 4;
        unsigned int SpecialFeetPads[NumSpecialFeetPads]={2,4,5,7};

        for (unsigned int is=0; is<NumFeetSectors; is++) 
        {
          for (unsigned int it=0; it<NumSpecialFeetPads; it++) 
          {
            writeCdo->m_RPCPadParameters_array[FeetSectors[is]][SpecialFeetPads[it]].set_feet_on(true);
            for (unsigned int th=0; th<3; th++) 
              writeCdo->m_RPCPadParameters_array[FeetSectors[is]][SpecialFeetPads[it]].set_feet_threshold(th,m_FeetPadThresholds.at(th));
          }
        }

      // }
    }   

    // ------ begin like  PCcablingInterface::RpcPadIdHash::RpcPadIdHash()
    RDOindex index = (*pad_beg).second;
    index.pad_identifier(id);

    writeCdo->m_int2id.push_back(id);

    if( writeCdo->m_int2id.size() != index.hash()+1 )
    {
      ATH_MSG_ERROR("Inconsistence between PAD hash and RpcPadIdHash");
      ATH_MSG_ERROR(index);
      ATH_MSG_ERROR("Position into RpcPadIdHash map is " << writeCdo->m_int2id.size() - 1);
      return StatusCode::FAILURE;
    }

    unsigned short int side = index.side();
    unsigned short int rod  = index.RODid();

    if( rod > 15 )
    {
      ATH_MSG_ERROR("RPC ROD greater than 15");
      return StatusCode::FAILURE;
    }

    IdentifierHash HashID = index.hash();

    (writeCdo->m_rod2hash[(side==0x66)? 0 : 1][rod]).push_back(HashID);

    writeCdo->m_lookup[id]=index.hash(); 
  }

  // reserve enough space
  writeCdo->m_fullListOfRobIds.reserve(ROBid.size());
  for(uint32_t robid : ROBid) writeCdo->m_fullListOfRobIds.push_back(robid);  

  ATH_MSG_DEBUG("Number of valid RPC Pad IDs " << writeCdo->m_int2id.size());

  for(int i = 0; i < 64; i ++) writeCdo->m_SectorMap[i] = m_SectorMap[i]; 
  writeCdo->m_SectorType.assign(m_SectorType.begin(),m_SectorType.end());
  writeCdo->m_MaxType = m_MaxType;

  // record
  if (writeCdo->m_RDOs.empty()) {
    ATH_MSG_ERROR("Could not read any map configuration");
    return StatusCode::FAILURE;
  }
  if (writeCdo->m_HashVec.empty()) {
    ATH_MSG_ERROR("Could not read any HashID");
    return StatusCode::FAILURE;
  }
  if (writeCdo->m_SectorType.empty()) {
    ATH_MSG_ERROR("Could not read any m_SectorMap");
    return StatusCode::FAILURE;
  }
  if (writeCdo->m_int2id.empty()) {
    ATH_MSG_ERROR("Could not read any HashID");
    return StatusCode::FAILURE;
  }
  if (writeCdo->m_lookup.empty()) {
    ATH_MSG_ERROR("Could not read any HashID");
    return StatusCode::FAILURE;
  }
  if (writeCdo->m_fullListOfRobIds.empty()) {
    ATH_MSG_ERROR("Could not read any HashID");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;

}


std::list<Identifier> RpcCablingCondAlg::give_strip_id(unsigned short int SubsystemId,
                                                       unsigned short int SectorId,
                                                       unsigned short int PADId,
                                                       unsigned short int CMAId,
                                                       unsigned short ijk,
                                                       unsigned short int Channel) const
{
  std::list<unsigned int> CodeList;

  int logic_sector = SectorId + SubsystemId*32;
  unsigned short int Ixx = CMAId & 1;
  unsigned short int ep  = (CMAId >> 1) & 1;
  unsigned short int lh  = (CMAId >> 2) & 1;

  ep = (ep==1)? 0 : 1;

  // retrieve the Sector Logic setup
  const RPC_CondCabling::SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];  

  //retrieve the CMAparameters associated to the identifiers
  if(ep)
  {   
    CMAcoverage PhiCov = (logic_sector%2)? OddSectors : EvenSectors;
    CMAidentity PHI(Phi,PhiCov,PADId,Ixx);
    CodeList = s.give_strip_code(PHI,logic_sector,lh,ijk,Channel);
  }
  else
  {
    CMAidentity ETA(Eta,AllSectors,PADId,Ixx);
    CodeList = s.give_strip_code(ETA,logic_sector,lh,ijk,Channel);
  }


  std::list <RPCofflineId> offlineIdList;
  std::list<unsigned int>::const_iterator it = CodeList.begin();
  while(it != CodeList.end())
  {
    RPCdecoder decode(*it);
    RPCofflineId rpcId;

    int RPC_strip    = decode.strip_number();
    int RPC_chamber  = decode.rpc_z_index();
    int RPC_layer    = decode.rpc_layer();
    int RPC_station  = decode.lvl1_station();
    int sector       = (decode.logic_sector())%32;

    const RPC_CondCabling::RPCchamber* rpc = s.find_chamber(RPC_station, RPC_chamber);

    rpcId.stationName = rpc->stationName();
    rpcId.stationEta  = (decode.half_barrel() == Positive)?  rpc->stationEta() : 
      -rpc->stationEta();
    rpcId.stationPhi  = (sector==31)? 1 : (sector+1)/4 +1;
    rpcId.doubletR    = rpc->doubletR();
    rpcId.doubletZ    = rpc->doubletZ();
    rpcId.doubletPhi  = (rpc->phiReadoutPannels()==2)? (sector+1)%2+1: 1;
    rpcId.gasGap      = RPC_layer + 1;
    rpcId.measuresPhi = static_cast<int>(decode.view());
    rpcId.strip       = RPC_strip + 1;

    offlineIdList.push_back(rpcId);

    ++it;
  }

  std::list<Identifier> id;
  std::list<RPCofflineId>::const_iterator iterator = offlineIdList.begin();
  while(iterator != offlineIdList.end())
  { 
    Identifier rpcId = m_idHelperSvc->rpcIdHelper().channelID((*iterator).stationName,
                                                              (*iterator).stationEta,
                                                              (*iterator).stationPhi,
                                                              (*iterator).doubletR,
                                                              (*iterator).doubletZ,
                                                              (*iterator).doubletPhi,
                                                              (*iterator).gasGap,
                                                              (*iterator).measuresPhi,
                                                              (*iterator).strip);
    id.push_back(rpcId);
    ++iterator;
  }

  return id;

}

/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

#include "RPC_CondCabling/RpcCablingCondData.h"


RpcCablingCondData::RpcCablingCondData() {}

RpcCablingCondData::~RpcCablingCondData() {}

const RpcCablingCondData::RDOmap& RpcCablingCondData::give_RDOs(void) const {return m_RDOs;}

bool RpcCablingCondData::giveOnlineID(const unsigned int hashID,
    unsigned short int& ROBid,
    unsigned short int& RODid,
    unsigned short int& side,
    unsigned short int& SLid,
    unsigned short int& RXid,
    unsigned short int& PADid) const
{
  if ( hashID >= m_HashVec.size() ) return false;

  const RDOindex* index = m_HashVec[hashID];

  ROBid = index->ROBid();
  RODid = index->RODid();
  side  = index->side();
  SLid  = index->SLid();
  RXid  = index->RXid();
  PADid = index->PADid();

  return true;
}


const std::vector<uint32_t>& RpcCablingCondData::giveFullListOfRobIds() const {return m_fullListOfRobIds;}



RpcCablingCondData::ID RpcCablingCondData::identifier(int index) const 
{
  int size = m_int2id.size();
  if(index>=0 && index < size) { return m_int2id[index] ; }

  Identifier id;
  return id; 
}

int RpcCablingCondData::operator() (const  ID& id) const 
{
  std::map<ID,int>::const_iterator it = m_lookup.find(id); 
  if(it!=m_lookup.end()){
    return (*it).second; 
  }

  // FIXME  -1 for invalid ID?
  return -1; 

}

int RpcCablingCondData::max() const { return m_int2id.size(); }

int RpcCablingCondData::offset() const { return 0; }

std::vector<IdentifierHash> RpcCablingCondData::rod2hash(uint16_t subsystem_id, uint16_t rod_id) const 
{
  if( (subsystem_id!=0x66 && subsystem_id!=0x65) || (rod_id>=16) ) return std::vector<IdentifierHash>();
  return m_rod2hash[(subsystem_id==0x66)? 0 : 1][rod_id];
}

uint32_t RpcCablingCondData::hash2source(unsigned int h) const
{
  unsigned short int ROBid;
  unsigned short int RODid;
  unsigned short int side;
  unsigned short int SLid;
  unsigned short int RXid;
  unsigned short int PADid;

  if( RpcCablingCondData::giveOnlineID(h,ROBid,RODid,side,SLid,RXid,PADid) )
  {
    uint32_t sub = side;
    uint32_t rod = RODid;
    return (sub << 16) | rod;
  }
  return 0;
}


StatusCode RpcCablingCondData::giveROB_fromRDO(const IdentifierHash rdoHashId,
    uint32_t& robId) const
{
  StatusCode sc = StatusCode::SUCCESS;
  unsigned int hash = (unsigned int)rdoHashId;

  if (hash>=m_HashVec.size()) {
    return StatusCode::FAILURE;
  }

  unsigned short int rob_id = m_HashVec[hash]->ROBid();
  unsigned short int side  = m_HashVec[hash]->side();

  robId = (side << 16) | rob_id;

  return sc;
}


StatusCode RpcCablingCondData::giveROB_fromRDO(const std::vector<IdentifierHash>& rdoHashVec,
                                               std::vector<uint32_t>& robIdVec) const
{

  StatusCode sc = StatusCode::SUCCESS;

  robIdVec.clear();

  for (IdentifierHash rdoHashId : rdoHashVec) {
    uint32_t robid = 0xffffffff;
    if(!giveROB_fromRDO(rdoHashId,robid)) sc = StatusCode::FAILURE;
    robIdVec.push_back(robid);
  }

  //sort and remove duplicates
  std::sort( robIdVec.begin(), robIdVec.end() );
  robIdVec.erase( unique( robIdVec.begin(), robIdVec.end() ), robIdVec.end() );

  return sc;
}


const CMAparameters::CMAlist RpcCablingCondData::give_CMAs(const int logic_sector,const ViewType side,
                                                          const int station,const int cabling_code) const
{
  const RPC_CondCabling::SectorLogicSetup& s = m_SectorType[m_SectorMap[logic_sector] - 1];
  CMAparameters::CMAlist 
    result = s.give_CMAs(logic_sector,side,station,cabling_code);
  return result;
}


unsigned long int RpcCablingCondData::strip_code_fromOffId ( const std::string& stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, int gasGap, int measuresPhi, int strip) const 
{
    unsigned long int code = 0;
      
    unsigned long int etaPhiView=2;
    unsigned long int phi1_16 = 2.*stationPhi;
    if (largeSector(stationName)) phi1_16 = phi1_16-1;
    int logicSector= (phi1_16-1)*2 + doubletPhi - 2;
    if (logicSector<0)   logicSector=31;
    if (measuresPhi >0)  etaPhiView = 1;
    if (stationEta >=0 ) logicSector +=32;

    unsigned long int cabStat = doubletR;
    if (stationName.substr(0,2)=="BO") {
      cabStat = 2+cabStat;
      if (cabStat>3) cabStat=2; // special case of BOF and BOG with dbr=2
    }
    else if (stationName.substr(0,3)=="BML") {
      if (abs(stationEta)==7 || (abs(stationEta)==6 && stationPhi==7)) 
      {
        cabStat=2; // special case of single RPC Chambers at the same R or dbr2
      }
    }

    if (logicSector<0 || logicSector>63) 
    {
      return 999999;
    }

    int zIndexInCablingStation = computeZIndexInCablingStation(stationName, logicSector, stationEta, doubletR, doubletZ, cabStat); 
    if (zIndexInCablingStation > 99) return 0;
    int cablingStation         = cabStat;


    unsigned long int gg = gasGap-1;
    code = strip_code_fromOffline (etaPhiView, logicSector, cablingStation, gg, zIndexInCablingStation, strip);

    return code;
}

unsigned long int RpcCablingCondData::strip_code_fromOffline (int etaPhiView, int logicSector, int cablingStation, int gasGap, int zIndexInCablingStation, int strip) const
{
  unsigned long int code = 0;
  code = 
    etaPhiView     * 100000000 +
    logicSector    * 1000000   +
    cablingStation * 100000    +
    gasGap         * 10000     +
    zIndexInCablingStation  * 100       +
    (strip-1);
  return code;
} 

bool RpcCablingCondData::largeSector(const std::string& stName) const 
{
  bool ls = false;
  if (
      stName == "BML"   || //2 
      stName == "BOL"   || //4 
      stName == "BME"   || //53 
      stName == "BIR"      //52 
     ) 
    ls = true;
  return ls;
}

unsigned int RpcCablingCondData::computeZIndexInCablingStation(const std::string& stationName, int logicSector, 
         int stationEta,  int doubletR, int doubletZ, int cabStat) const 
{

 unsigned int zIndexInCablingStation = 999;

 int cablingStation = -1;
 int sectType = m_SectorMap[logicSector];
 if (sectType < 1 || sectType > m_MaxType+1)
 {
   return 99999;
 }
 const RPC_CondCabling::SectorLogicSetup& sec = m_SectorType[sectType - 1];

 for (unsigned int jStat=1; jStat<4; ++jStat)
 {
   if (cablingStation != -1) break;
   for (unsigned int jCham=0; jCham<20; ++jCham)
   {
     const RPC_CondCabling::RPCchamber* rpcC = sec.find_chamber(jStat,jCham);
     if (rpcC==nullptr) continue;

     if ((rpcC->chamber_name()).substr(0,3)!=stationName ) {
       continue;
     }
     if (rpcC->stationEta()  != abs(stationEta)  ) {
       continue;
     }
     if (rpcC->doubletR()    !=doubletR    ) {
       continue;
     }
     if (rpcC->doubletZ()    !=doubletZ    ) {
       continue;
     }
     cablingStation = jStat;
     zIndexInCablingStation = jCham;
     break;
   }
 }
 if ( cablingStation<1 || cablingStation>3 ||  (cabStat!=cablingStation) ) 
 {
   return 99999;      
 }
 if( zIndexInCablingStation>19 ) 
 {
   return 99999;      
 }

  return zIndexInCablingStation;

}



std::list<Identifier> RpcCablingCondData::give_strip_id(unsigned short int SubsystemId,
                                                        unsigned short int SectorId,
                                                        unsigned short int PADId,
                                                        unsigned short int CMAId,
                                                        unsigned short ijk,
                                                        unsigned short int Channel,
                                                        ServiceHandle<Muon::IMuonIdHelperSvc> idHelperSvc ) const
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
    Identifier rpcId = idHelperSvc->rpcIdHelper().channelID((*iterator).stationName,
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

StatusCode RpcCablingCondData::giveRDO_fromPRD(const IdentifierHash prdHashId, std::vector<IdentifierHash>& rdoHashVec) const
{
  StatusCode sc = StatusCode::SUCCESS;

  rdoHashVec.clear();

  PRD_RDO_Map::const_iterator it = m_PRD_RDO_map.find(prdHashId);

  if (it == m_PRD_RDO_map.cend()) return sc;

  for (IdentifierHash rdoId : (*it).second)
    rdoHashVec.push_back(rdoId);

  return sc;
}

StatusCode RpcCablingCondData::giveROB_fromPRD(const IdentifierHash prdHashId, std::vector<uint32_t>& robIdVec) const
{
  StatusCode sc = StatusCode::SUCCESS;

  robIdVec.clear();

  PRD_ROB_Map::const_iterator it = m_PRD_ROB_map.find(prdHashId);

  if (it == m_PRD_ROB_map.cend()) return sc;

  for (uint32_t robId : (*it).second)
    robIdVec.push_back(robId);

  return sc;
}

bool RpcCablingCondData::give_Pad_Parameters(unsigned short int logic_sector,
                                             unsigned short int PADId,
                                             bool & feet,
                                             bool & eta_and_phi,
                                             unsigned short int  & cma_mask,
                                             unsigned short int  & feet_th0,
                                             unsigned short int  & feet_th1,
                                             unsigned short int  & feet_th2 )const {

  if ( logic_sector>=64 || PADId >= 8) return false;

  feet = m_RPCPadParameters_array[logic_sector][PADId].feet_on();
  eta_and_phi = m_RPCPadParameters_array[logic_sector][PADId].eta_and_phi();
  cma_mask    = m_RPCPadParameters_array[logic_sector][PADId].cma_mask();
  feet_th0    = m_RPCPadParameters_array[logic_sector][PADId].feet_threshold(0);
  feet_th1    = m_RPCPadParameters_array[logic_sector][PADId].feet_threshold(1);
  feet_th2    = m_RPCPadParameters_array[logic_sector][PADId].feet_threshold(2);    

  return true;
}



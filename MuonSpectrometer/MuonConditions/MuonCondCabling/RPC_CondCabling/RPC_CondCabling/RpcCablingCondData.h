/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

#ifndef RPCCABLINGCONDDATA_H
#define RPCCABLINGCONDDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"

#include <map>
#include <string>
#include <vector>

#include "RPC_CondCabling/RDOindex.h"
#include "RPC_CondCabling/SectorLogicSetup.h"
#include "RPC_CondCabling/RPCofflineId.h"
#include "RPC_CondCabling/RPCPadParameters.h"

#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/RpcIdHelper.h"

class RpcCablingCondData{

  friend class RpcCablingCondAlg;

  public:

  typedef std::map <int,RDOindex,std::less < int > > RDOmap;
  typedef std::vector<const RDOindex*> OfflineOnlineHashMap;
  typedef std::map <Identifier,const RDOindex*,std::less < Identifier > > OfflineOnlineMap;
  typedef std::vector < RPC_CondCabling::SectorLogicSetup > STvec;
  typedef std::map <IdentifierHash, std::set<IdentifierHash> > PRD_RDO_Map;
  typedef std::map <IdentifierHash, std::set<uint32_t> > PRD_ROB_Map;
  typedef std::map <uint32_t, std::set<IdentifierHash> > ROB_RDO_Map;

  RpcCablingCondData()=default;
  virtual ~RpcCablingCondData()=default;

  const RDOmap& give_RDOs(void) const;
  
  bool give_ROBid(const Identifier& compactID, unsigned short int& id) const;
  bool give_ROBid(const unsigned int hashID, unsigned short int& id) const;

  bool give_RODid(const Identifier& compactID, unsigned short int& id) const;
  bool give_RODid(const unsigned int hashID, unsigned short int& id) const;

  bool give_side(const Identifier& compactID, unsigned short int& id) const;
  bool give_side(const unsigned int hashID, unsigned short int& id) const;

  bool give_SLid(const Identifier& compactID, unsigned short int& id) const;
  bool give_SLid(const unsigned int hashID, unsigned short int& id) const;

  bool give_RXid(const Identifier& compactID, unsigned short int& id) const;
  bool give_RXid(const unsigned int hashID, unsigned short int& id) const;

  bool give_PADid(const Identifier& compactID, unsigned short int& id) const;
  bool give_PADid(const unsigned int hashID, unsigned short int& id) const;


  const std::vector<uint32_t>& giveFullListOfRobIds() const;

  StatusCode giveROB_fromRDO(const IdentifierHash rdoHashId, uint32_t& robId) const;

  StatusCode giveROB_fromRDO(const std::vector<IdentifierHash>& rdoHashVec,
                             std::vector<uint32_t>& robIdVec) const;

  std::list<Identifier> give_strip_id (unsigned short int SubsystemId,
                                       unsigned short int SectorId,
                                       unsigned short int PADId,
                                       unsigned short int CMAId,
                                       unsigned short ijk,
                                       unsigned short int Channel,
                                       const RpcIdHelper* rpcId ) const;

  const CMAparameters::CMAlist give_CMAs(const int logic_sector,const ViewType side, const int station,const int cabling_code) const;
  unsigned long int strip_code_fromOffline (int etaPhiView, int logicSector, int cablingStation, int gasGap, int zIndexInCablingStation, int strip) const;
  unsigned long int strip_code_fromOffId (const std::string& stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, int gasGap, int measuresPhi, int strip) const;
  bool largeSector(const std::string& stName) const;
  unsigned int computeZIndexInCablingStation(const std::string& stationName, int logicSector, 
                                             int stationEta, int doubletR, int doubletZ, int cabStat) const;

  StatusCode giveRDO_fromPRD(const IdentifierHash prdHashId, std::vector<IdentifierHash>& rdoHashVec) const;
  StatusCode giveRDO_fromPRD(const std::vector<IdentifierHash>& prdHashVec, std::vector<IdentifierHash>& rdoHashVec) const;
  StatusCode giveRDO_fromROB(const std::vector<uint32_t>& robIdVec, std::vector<IdentifierHash>& rdoHashVec) const;
  StatusCode giveROB_fromPRD(const IdentifierHash prdHashId, std::vector<uint32_t>& robIdVec) const;

  virtual bool give_Pad_Parameters( unsigned short int logic_sector,
                                    unsigned short int PADId,
                                    bool & feet,
                                    bool & eta_and_phi,
                                    unsigned short int  & cma_mask,
                                    unsigned short int  & feet_th0,
                                    unsigned short int  & feet_th1,
                                    unsigned short int  & feet_th2 ) const;

  bool give_PAD_address (unsigned short int SubsystemId,
                         unsigned short int SectorId,
                         unsigned short int RoIId,
                         unsigned int& padIdHash) const;

bool give_RoI_borders_id (unsigned short int SubsystemId,
                          unsigned short int SectorId,
                          unsigned short int RoIId,
                          Identifier& EtaLowBorder_id,
                          Identifier& EtaHighBorder_id,
                          Identifier& PhiLowBorder_id,
                          Identifier& PhiHighBorder_id,
                          const RpcIdHelper* rpcId) const;

bool give_RoI_borders (unsigned short int SubsystemId,
                       unsigned short int SectorId,
                       unsigned short int RoIId,
                       unsigned int& EtaLowBorder,
                       unsigned int& EtaHighBorder,
                       unsigned int& PhiLowBorder,
                       unsigned int& PhiHighBorder) const;

Identifier protected_strip_OffId_fromCode (unsigned long int strip_code, const RpcIdHelper* rpcId) const;
RPCofflineId strip_id_fromCode(unsigned long int strip_code) const;

bool give_LowPt_borders (unsigned short int SubsystemId,
                         unsigned short int SectorId,
                         unsigned short int RoIId,
                         unsigned int& EtaLowBorder,
                         unsigned int& EtaHighBorder,
                         unsigned int& PhiLowBorder,
                         unsigned int& PhiHighBorder) const;

bool give_LowPt_borders_id (unsigned short int SubsystemId,
                            unsigned short int SectorId,
                            unsigned short int RoIId,
                            Identifier& EtaLowBorder_id,
                            Identifier& EtaHighBorder_id,
                            Identifier& PhiLowBorder_id,
                            Identifier& PhiHighBorder_id,
                            const RpcIdHelper* rpcId) const;

bool give_HighPt_borders (unsigned short int SubsystemId,
                          unsigned short int SectorId,
                          unsigned short int RoIId,
                          unsigned int& EtaLowBorder,
                          unsigned int& EtaHighBorder,
                          unsigned int& PhiLowBorder,
                          unsigned int& PhiHighBorder) const;

bool give_HighPt_borders_id(unsigned short int SubsystemId,
                            unsigned short int SectorId,
                            unsigned short int RoIId,
                            Identifier& EtaLowBorder_id,
                            Identifier& EtaHighBorder_id,
                            Identifier& PhiLowBorder_id,
                            Identifier& PhiHighBorder_id,
                            const RpcIdHelper* rpcId) const;

Identifier strip_OffId_fromCode (unsigned long int strip_code, const RpcIdHelper* rpcId) const;

bool giveOfflineId(const unsigned short int side, const unsigned short int sector, const unsigned short int padId, Identifier& id) const;

  // migrate from RpcPadIDHash
  // reverse conversion 
  Identifier identifier(int i) const;

  int operator() (const Identifier& id) const ;
  int max() const; 
  int offset() const ;
  std::vector<IdentifierHash> rod2hash(uint16_t subsystem, uint16_t rod) const;
  uint32_t hash2source(unsigned int h) const;



  private:


  RDOmap m_RDOs;
  OfflineOnlineMap m_RDOmap;
  OfflineOnlineHashMap m_HashVec;

  std::string m_map;
  std::string m_corr;    

  std::vector<std::string> m_vecetaCM_File;
  std::vector<std::string> m_vecetaTh0;
  std::vector<std::string> m_vecetaSequence_Th;
  std::vector<std::string> m_vecetaInfo;

  std::map<std::string, std::string> m_trigroads;

  std::vector<std::string> m_vecphiCM_File;
  std::vector<std::string> m_vecphiTh0;
  std::vector<std::string> m_vecphiInfo;

  // migrate from RpcPadIDHash
  std::vector<Identifier> m_int2id;
  std::vector<IdentifierHash> m_rod2hash[2][16];
  std::map<Identifier,int> m_lookup;

  std::vector<uint32_t> m_fullListOfRobIds;
  int m_SectorMap[64]; 
  // array; for each sectorlogic type returns the SectorLogicSetup
  STvec m_SectorType;
  PRD_RDO_Map m_PRD_RDO_map;
  PRD_ROB_Map m_PRD_ROB_map;
  ROB_RDO_Map m_ROB_RDO_map;

  int m_MaxType;

  // list of RPCPadParameters
  RPCPadParameters  m_RPCPadParameters_array[64][8];

  Identifier m_offline_id[2][32][10];

};



CLASS_DEF( RpcCablingCondData , 251594023 , 1 ) //newly generated using clid -m "RpcCablingCondData"
CLASS_DEF( CondCont<RpcCablingCondData> , 124926001 , 0 )


#endif

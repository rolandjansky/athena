/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventAthenaPool/test/RpcPadContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/RpcPadContainerCnv_p1.h"
#include "AthenaBaseComps/AthService.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"
#include "RPCcablingInterface/RpcPadIdHash.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TestTools/initGaudi.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>
#include "MuonEventAthenaPool/CreateTransientTemplates.h"
#include "make_dd.icc"


class TestRPC_CablingSvc
  : public extends<AthService, IRPCcablingSvc>
{
public:
  TestRPC_CablingSvc(const std::string& name,ISvcLocator* sl);
  virtual StatusCode initialize();

  virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
  
  virtual const CablingRPCBase* getRPCCabling() const { std::abort(); }
  virtual StatusCode initMappingModel(IOVSVC_CALLBACK_ARGS) { std::abort(); }
  virtual const CablingRPCBase::RDOmap& give_RDOs() const;
  virtual const CMAparameters::CMAlist give_CMAs(const int,const ViewType,
                                                 const int,const int) const
  { std::abort(); }
    
  virtual void printType(int,int,std::string,bool)const
  { std::abort(); }
    
  virtual void printSector(int,int,std::string,bool)const
  { std::abort(); }

  virtual bool give_global_address(unsigned int,int&,int&,int&,int&) const
  { std::abort(); }
				    
  virtual bool give_RoI_borders (unsigned short int /*SubsystemId*/,
                                 unsigned short int /*SectorId*/,
                                 unsigned short int /*RoIId*/,
                                 unsigned int& /*EtaLowBorder*/,
                                 unsigned int& /*EtaHighBorder*/,
                                 unsigned int& /*PhiLowBorder*/,
                                 unsigned int& /*PhiHighBorder*/) const
  { std::abort(); }

  virtual bool give_RoI_borders_id (unsigned short int /*SubsystemId*/,
                                    unsigned short int /*SectorId*/,
                                    unsigned short int /*RoIId*/,
                                    Identifier& /*EtaLowBorder_id*/,
                                    Identifier& /*EtaHighBorder_id*/,
                                    Identifier& /*PhiLowBorder_id*/,
                                    Identifier& /*PhiHighBorder_id*/) const
  { std::abort(); }

  virtual bool give_LowPt_borders (unsigned short int /*SubsystemId*/,
                                   unsigned short int /*SectorId*/,
                                   unsigned short int /*RoIId*/,
                                   unsigned int& /*EtaLowBorder*/,
                                   unsigned int& /*EtaHighBorder*/,
                                   unsigned int& /*PhiLowBorder*/,
                                   unsigned int& /*PhiHighBorder*/) const
  { std::abort(); }

  virtual bool give_LowPt_borders_id (unsigned short int /*SubsystemId*/,
                                      unsigned short int /*SectorId*/,
                                      unsigned short int /*RoIId*/,
                                      Identifier& /*EtaLowBorder_id*/,
                                      Identifier& /*EtaHighBorder_id*/,
                                      Identifier& /*PhiLowBorder_id*/,
                                      Identifier& /*PhiHighBorder_id*/) const
  { std::abort(); }


  virtual bool give_HighPt_borders (unsigned short int /*SubsystemId*/,
                                    unsigned short int /*SectorId*/,
                                    unsigned short int /*RoIId*/,
                                    unsigned int& /*EtaLowBorder*/,
                                    unsigned int& /*EtaHighBorder*/,
                                    unsigned int& /*PhiLowBorder*/,
                                    unsigned int& /*PhiHighBorder*/) const
  { std::abort(); }

  virtual bool give_HighPt_borders_id (unsigned short int /*SubsystemId*/,
                                       unsigned short int /*SectorId*/,
                                       unsigned short int /*RoIId*/,
                                       Identifier& /*EtaLowBorder_id*/,
                                       Identifier& /*EtaHighBorder_id*/,
                                       Identifier& /*PhiLowBorder_id*/,
                                       Identifier& /*PhiHighBorder_id*/) const
  { std::abort(); }
   
  virtual bool give_Pad_Parameters(unsigned short int /*logic_sector*/,
                                   unsigned short int /*PADId*/,
                                   bool & /*feet*/,
                                   bool & /*eta_and_phi*/,
                                   unsigned short int  & /*cma_mask*/,
                                   unsigned short int  & /*feet_th0*/,
                                   unsigned short int  & /*feet_th1*/,
                                   unsigned short int  & /*feet_th2*/ ) const
  { std::abort(); }
    
    
  virtual unsigned long int strip_code_fromOffId (std::string /*stationName*/, 
                                                  int /*stationEta*/, 
                                                  int /*stationPhi*/, 
                                                  int /*doubletR*/, int /*doubletZ*/, int /*doubletPhi*/, 
                                                  int /*gasGap*/, int /*measuresPhi*/, int /*strip*/) const
  { std::abort(); }

  virtual Identifier strip_OffId_fromCode(unsigned long int /*strip_code_cab*/) const
  { std::abort(); }
   
  virtual std::list<unsigned int> give_strip_code
  (unsigned short int /*SubsystemId*/,
   unsigned short int /*SectorId*/,
   unsigned short int /*PADId*/,
   unsigned short int /*CMAId*/,
   unsigned short /*ijk*/,
   unsigned short int /*Channel*/) const
  { std::abort(); }

  virtual std::list<Identifier> give_strip_id
  (unsigned short int /*SubsystemId*/,
   unsigned short int /*SectorId*/,
   unsigned short int /*PADId*/,
   unsigned short int /*CMAId*/,
   unsigned short /*ijk*/,
   unsigned short int /*Channel*/) const
  { std::abort(); }
    
  //give access to onlineID from compact OfflineID (return == true if OK)
  virtual bool giveOnlineID(const Identifier /*compactID*/,
                            unsigned short int& /*ROBid*/,
                            unsigned short int& /*RODid*/,
                            unsigned short int& /*side*/,
                            unsigned short int& /*SLid*/,
                            unsigned short int& /*RXid*/,
                            unsigned short int& /*PADid*/) const
  { std::abort(); }
   
  virtual bool giveOnlineID(const unsigned int /*hashID*/,
                            unsigned short int& /*ROBid*/,
                            unsigned short int& /*RODid*/,
                            unsigned short int& /*side*/,
                            unsigned short int& /*SLid*/,
                            unsigned short int& /*RXid*/,
                            unsigned short int& /*PADid*/) const
  { std::abort(); }
   
  virtual bool giveOffflineID(unsigned short int /*Side*/,
                              unsigned short int /*Sector*/,
                              unsigned short int /*PADID*/,
                              Identifier& /*ID*/) const
  { std::abort(); }

    
  //map a single PRD HashId into a vector of RDO HashIds
  virtual StatusCode giveRDO_fromPRD(const IdentifierHash /*prdHashId*/,  
                                     std::vector<IdentifierHash>& /*rdoHashVec*/) const
  { std::abort(); }
                                  
  //map a vector of PRD HashIds into a vector of RDO HashIds
  virtual StatusCode giveRDO_fromPRD(const std::vector<IdentifierHash>& /*prdHashVec*/,  
                                     std::vector<IdentifierHash>& /*rdoHashVec*/) const
  { std::abort(); }
                                     
  //map a single ROB Id into a vector of RDO HashIds
  virtual StatusCode giveRDO_fromROB(const uint32_t /*robId*/,  
                                     std::vector<IdentifierHash>& /*rdoHashVec*/) const
  { std::abort(); }
                                  
  //map a vector of ROB Ids into a vector of RDO HashIds
  virtual StatusCode giveRDO_fromROB(const std::vector<uint32_t>& /*robIdVec*/,  
                                     std::vector<IdentifierHash>& /*rdoHashVec*/) const
  { std::abort(); }
                                  
  //map a single PRD HashId into a vector of corresponding ROB Ids
  virtual StatusCode giveROB_fromPRD(const IdentifierHash /*prdHashId*/,
                                     std::vector<uint32_t>& /*robIdVec*/) const
  { std::abort(); }
                                     
  //map a vector of PRD HashIds into a vector of corresponding ROB Ids
  virtual StatusCode giveROB_fromPRD(const std::vector<IdentifierHash>& /*prdHashVec*/,
                                     std::vector<uint32_t>& /*robIdVec*/) const
  { std::abort(); }
                                  
  //map a single RDO HashId into a single ROB Id
  virtual StatusCode giveROB_fromRDO(const IdentifierHash /*rdoHashId*/,
                                     uint32_t& /*robId*/) const
  { std::abort(); }
                                     
  //map a vector of RDO HashIds into a vector of corresponding ROB Ids
  virtual StatusCode giveROB_fromRDO(const std::vector<IdentifierHash>& /*rdoHashVec*/,
                                     std::vector<uint32_t>& /*robIdVec*/) const
  { std::abort(); }
   
  //provide the full list of ROB id
  virtual std::vector<uint32_t> giveFullListOfRobIds() const
  { std::abort(); }
    
  // method used in RPCgeometry
  virtual std::vector<const RDOindex*> GiveHashVec() const
  { std::abort(); }

  // hash function 
  virtual RpcPadIdHash* padHashFunction() const
  { return m_hash.get(); }
    
  // access to service name 
  virtual std::string rpcCabSvcType() const
  { std::abort(); }

private:
  std::unique_ptr<RpcPadIdHash> m_hash;
  CablingRPCBase::RDOmap m_rdomap;  // map<int, RDOindex>
};


TestRPC_CablingSvc::TestRPC_CablingSvc(const std::string& name,ISvcLocator* sl)
  : extends<AthService, IRPCcablingSvc> (name, sl)
{
  // lvl1 code:
  //    strip_number + rpc_z_index*100 + rpc_layer*10*1000 +
  //    lvl1_station*100*1000 + logic_sector*1000*1000 +
  //    strip_type*100*1000*1000
  //      1 <= strip_type <= 2
  //      0 <= logic_sector <= 63
  //      1 <= station_value <= 3
  //      0 <= rpc_layer <= 1
  int code0= 0 + 1*100 + 0*10*1000 + 1*100*1000 + 1*1000*1000 + 1*100*1000*1000;
  int code1= 1 + 2*100 + 0*10*1000 + 2*100*1000 + 2*1000*1000 + 1*100*1000*1000;
  int code2= 2 + 3*100 + 0*10*1000 + 3*100*1000 + 3*1000*1000 + 1*100*1000*1000;
  // pad code name station_eta[-8:8] station_phi[1:8] doubletR[1:2]
  //   doubletZ[1:3] doubletPhi[1:2]
  m_rdomap.emplace (0, RDOindex (0, code0, "BIL", 1, 2, 1, 1, 2));
  m_rdomap.emplace (1, RDOindex (1, code1, "BIL", 6, 7, 2, 2, 1));
  m_rdomap.emplace (2, RDOindex (2, code2, "BIL", 7, 5, 1, 3, 2));
  m_rdomap.find(0)->second.set_hash(0);
  m_rdomap.find(1)->second.set_hash(1);
  m_rdomap.find(2)->second.set_hash(2);

  //IDs: 0x6024c40000000000  0x603b680000000000  0x603e540000000000
}


StatusCode TestRPC_CablingSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{
    msg(MSG::VERBOSE) << "queryInterface Start" << endmsg;
    if(IRPCcablingSvc::interfaceID().versionMatch(riid) ) 
    {
        msg(MSG::VERBOSE) << "versionMatch=true" << endmsg;
        *ppvIF = (IRPCcablingSvc*)(this);
    } else {
        msg(MSG::VERBOSE) << "cannot find the interface!" << endmsg;
        return AthService::queryInterface(riid, ppvIF); 
      }
  msg(MSG::VERBOSE) << "queryInterface succesfull" << endmsg;
  addRef();  // is this needed ?? yes it is ! 
  return StatusCode::SUCCESS;
}


StatusCode TestRPC_CablingSvc::initialize()
{
  m_hash = std::make_unique<RpcPadIdHash> (this);
  return StatusCode::SUCCESS;
}


const CablingRPCBase::RDOmap& TestRPC_CablingSvc::give_RDOs() const
{
  return m_rdomap;
}


DECLARE_SERVICE_FACTORY(  TestRPC_CablingSvc )


//****************************************************************************


void compare (const RpcFiredChannel& p1,
              const RpcFiredChannel& p2)
{
  assert (p1.bcid() == p2.bcid());
  assert (p1.time() == p2.time());
  assert (p1.ijk() == p2.ijk());
  assert (p1.channel() == p2.channel());
  assert (p1.ovl() == p2.ovl());
  assert (p1.thr() == p2.thr());
}


void compare (const RpcCoinMatrix& p1,
              const RpcCoinMatrix& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.onlineId() == p2.onlineId());
  assert (p1.crc() == p2.crc());
  assert (p1.fel1Id() == p2.fel1Id());
  assert (p1.febcId() == p2.febcId());

  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void compare (const RpcPad& p1,
              const RpcPad& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.identifyHash() == p2.identifyHash());
  assert (p1.onlineId() == p2.onlineId());
  //assert (p1.lvl1Id() == p2.lvl1Id());
  //assert (p1.bcId() == p2.bcId());
  assert (p1.status() == p2.status());
  assert (p1.errorCode() == p2.errorCode());
  assert (p1.sector() == p2.sector());

  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void compare (const RpcPadContainer& p1,
              const RpcPadContainer& p2)
{
  RpcPadContainer::const_iterator it1 = p1.begin();
  RpcPadContainer::const_iterator end1 = p1.end();
  RpcPadContainer::const_iterator it2 = p2.begin();
  RpcPadContainer::const_iterator end2 = p2.end();
  for (; it1 != end1; ++it1, ++it2) {
    assert (it2 != end2);
    assert (it1.hashId() == it2.hashId());
    compare (**it1, **it2);
  }
  assert (it2 == end2);
}


void testit (const RpcPadContainer& trans1, IRPCcablingSvc* cabling)
{
  MsgStream log (0, "test");
  RpcPadContainerCnv_p1 cnv;
  cnv.initialize (log, cabling);
  RpcPadContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  std::unique_ptr<RpcPadContainer> trans2 (cnv.createTransient (&pers, log));
  compare (trans1, *trans2);
}


void test1 (IRPCcablingSvc* cabling)
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;
  RpcPadContainer trans1 (3);
  unsigned int ids[3] = {0x6024c400,  0x603b6800,  0x603e5400};
  for (int k=0; k < 3; k++) {
    int koffs = k*50;
    auto pad = std::make_unique<RpcPad> (Identifier(ids[k]),
                                         k,
                                         985+koffs,
                                         984+koffs,
                                         983+koffs,
                                         982+koffs,
                                         981+koffs,
                                         980+koffs);

    for (int j=0; j < 5; j++) {
      int joffs = j*100;
      auto cm = std::make_unique<RpcCoinMatrix> (Identifier (123+joffs),
                                                 234+joffs, 345+joffs,
                                                 456+joffs, 576+joffs);

      for (int i=0; i < 10; i++) {
        int ioffs = 1000*i;
        if (i < 7)
          cm->push_back (new RpcFiredChannel (321+ioffs, 432+ioffs,
                                              i, 534+ioffs));
        else
          cm->push_back (new RpcFiredChannel (213+ioffs, 324+ioffs, 7,
                                              435+ioffs, 546+ioffs));
      }

      pad->push_back (std::move (cm));
    }
    trans1.addCollection (pad.release(), k);
  }

  testit (trans1, cabling);
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("MuonEventAthenaPool_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  make_dd();

  ServiceHandle<IRPCcablingSvc> cabling ("TestRPC_CablingSvc", "test");
  if (cabling.retrieve().isFailure()) {
    std::cerr << "Cannot get cabling\n";
    return 0;
  }

  test1 (cabling.get());
  return 0;
}

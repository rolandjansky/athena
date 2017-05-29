/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRPC_CABLING_MUONRPC_CABLINGSVC_H
#define MUONRPC_CABLING_MUONRPC_CABLINGSVC_H

//#include "GaudiKernel/Service.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "MuonRPC_Cabling/CablingRPC.h"
// added 
#include "RPCcablingInterface/IRPCcablingSvc.h"
//#include "MuonCondInterface/IRPCCablingDbTool.h"
#include "MuonRPC_Cabling/ICallBackMuonRPC_Cabling.h"

#include "AthenaKernel/IOVSvcDefs.h"
#include "MuonRPC_Cabling/RPCPadParameters.h"

class MuonRPC_CablingMap;
class RpcIdHelper;
class IRPCCablingDbTool;
class IRPCTriggerDbTool;
class RpcPadIdHash;




//static const InterfaceID IID_IMuonRPC_CablingSvc("MuonRPC_CablingSvc", 1, 0);

class MuonRPC_CablingSvc : public AthService,  virtual public IRPCcablingSvc,
                           virtual public ICallBackMuonRPC_Cabling
{

    
public:

  MuonRPC_CablingSvc(const std::string& name,ISvcLocator* sl);
  virtual ~MuonRPC_CablingSvc();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();

    // NOT NEEDED ?
    //  static const InterfaceID& interfaceID() { return IID_IMuonRPC_CablingSvc; }

    virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );

    virtual std::string rpcCabSvcType() const {if (!m_RPCMapfromCool) return "simLike_MapsFromFiles"; return "dataLike";}
    
    // Interface implementation
    StatusCode initMappingModel(IOVSVC_CALLBACK_ARGS_P(I,keys));
    StatusCode initTrigRoadsModel(IOVSVC_CALLBACK_ARGS_P(I,keys));
    
    // hash functon for the RPC pad (RDO)
    virtual RpcPadIdHash* padHashFunction() const {return m_padHashIdHelper;}

    virtual const CablingRPCBase::RDOmap& give_RDOs() const;
    virtual const CMAparameters::CMAlist 
          give_CMAs(const int,const ViewType,const int,const int) const;
    virtual void printType(int,int,std::string,bool) const;
    virtual void printSector(int,int,std::string,bool) const;

    virtual bool give_global_address(unsigned int,int&,int&,int&,int&) const;
				    
    virtual bool give_RoI_borders (unsigned short int SubsystemId,
                                   unsigned short int SectorId,
			           unsigned short int RoIId,
			           unsigned int& EtaLowBorder,
				   unsigned int& EtaHighBorder,
				   unsigned int& PhiLowBorder,
				   unsigned int& PhiHighBorder) const;

    virtual bool give_RoI_borders_id (unsigned short int SubsystemId,
                                   unsigned short int SectorId,
			           unsigned short int RoIId,
			           Identifier& EtaLowBorder_id,
				   Identifier& EtaHighBorder_id,
				   Identifier& PhiLowBorder_id,
				   Identifier& PhiHighBorder_id) const;
    

    virtual bool give_LowPt_borders (unsigned short int SubsystemId,
                                     unsigned short int SectorId,
			             unsigned short int RoIId,
			             unsigned int& EtaLowBorder,
				     unsigned int& EtaHighBorder,
				     unsigned int& PhiLowBorder,
				     unsigned int& PhiHighBorder) const;

    virtual bool give_LowPt_borders_id (unsigned short int SubsystemId,
                                   unsigned short int SectorId,
			           unsigned short int RoIId,
			           Identifier& EtaLowBorder_id,
				   Identifier& EtaHighBorder_id,
				   Identifier& PhiLowBorder_id,
				   Identifier& PhiHighBorder_id) const;
 

    virtual bool give_HighPt_borders (unsigned short int SubsystemId,
                                      unsigned short int SectorId,
			              unsigned short int RoIId,
			              unsigned int& EtaLowBorder,
				      unsigned int& EtaHighBorder,
				      unsigned int& PhiLowBorder,
				      unsigned int& PhiHighBorder) const;

    virtual bool give_HighPt_borders_id (unsigned short int SubsystemId,
                                   unsigned short int SectorId,
			           unsigned short int RoIId,
			           Identifier& EtaLowBorder_id,
				   Identifier& EtaHighBorder_id,
				   Identifier& PhiLowBorder_id,
				   Identifier& PhiHighBorder_id) const;


    virtual bool give_Pad_Parameters(unsigned short int logic_sector,
                             unsigned short int PADId,
                             bool & feet,
                             bool & eta_and_phi,
                             unsigned short int  & cma_mask,
                             unsigned short int  & feet_th0,
                             unsigned short int  & feet_th1,
                             unsigned short int  & feet_th2 ) const;
    
    virtual unsigned long int strip_code_fromOffId (std::string stationName, 
						    int stationEta, 
						    int stationPhi, 
						    int doubletR, int doubletZ, int doubletPhi, 
						    int gasGap, int measuresPhi, int strip) const;


    virtual Identifier  strip_OffId_fromCode (unsigned long int strip_code_cab) const; 
    Identifier  protected_strip_OffId_fromCode (unsigned long int strip_code_cab) const; 

    virtual std::list<unsigned int> give_strip_code
                                           (unsigned short int SubsystemId,
                                            unsigned short int SectorId,
			                    unsigned short int PADId,
				            unsigned short int CMAId,
				            unsigned short ijk,
				            unsigned short int Channel) const;

    

    virtual std::list<Identifier> give_strip_id
                                           (unsigned short int SubsystemId,
                                            unsigned short int SectorId,
			                    unsigned short int PADId,
				            unsigned short int CMAId,
				            unsigned short ijk,
				            unsigned short int Channel) const;

    virtual const CablingRPCBase* getRPCCabling() const;
    
    
    //give access to onlineID from compact OfflineID (return == true if OK)
    virtual bool giveOnlineID(const Identifier compactID,
                             unsigned short int& ROBid,
                             unsigned short int& RODid,
                             unsigned short int& side,
                             unsigned short int& SLid,
                             unsigned short int& RXid,
                             unsigned short int& PADid) const;
    
    virtual bool giveOnlineID(const unsigned int hashID,
                             unsigned short int& ROBid,
                             unsigned short int& RODid,
                             unsigned short int& side,
                             unsigned short int& SLid,
                             unsigned short int& RXid,
                             unsigned short int& PADid) const;
    
    virtual bool giveOffflineID(unsigned short int Side,
                               unsigned short int Sector,
			       unsigned short int PADID,
			       Identifier& ID) const;
    
    StatusCode giveRDO_fromPRD(const IdentifierHash prdHashId,
                               std::vector<IdentifierHash>& rdoHashVec) const;
                                    
    StatusCode giveRDO_fromPRD(const std::vector<IdentifierHash>& prdHashVec,  
                               std::vector<IdentifierHash>& rdoHashVec) const;
                                     
    StatusCode giveRDO_fromROB(const uint32_t robId,  
                               std::vector<IdentifierHash>& rdoHashVec) const;
                                  
    StatusCode giveRDO_fromROB(const std::vector<uint32_t>& robIdVec,  
                               std::vector<IdentifierHash>& rdoHashVec) const;
                                    
    StatusCode giveROB_fromPRD(const IdentifierHash prdHashId,
                               std::vector<uint32_t>& robIdVec) const;
                                       
    StatusCode giveROB_fromPRD(const std::vector<IdentifierHash>& prdHashVec,
                               std::vector<uint32_t>& robIdVec) const;
                                  
    StatusCode giveROB_fromRDO(const IdentifierHash rdoHashId,
                               uint32_t& robId) const;
                                     
    StatusCode giveROB_fromRDO(const std::vector<IdentifierHash>& rdoHashVec,
                               std::vector<uint32_t>& robIdVec) const;
    
    std::vector<uint32_t> giveFullListOfRobIds() const;

    // METHOD USED BY RPCgeometry
    std::vector<const RDOindex*> GiveHashVec() const;

 private:

    RpcPadIdHash* m_padHashIdHelper;
    typedef std::map <Identifier,const RDOindex*,std::less < Identifier > > OfflineOnlineMap;
    typedef std::vector<const RDOindex*> OfflineOnlineHashMap;
    typedef std::map <IdentifierHash, std::set<IdentifierHash> > PRD_RDO_Map;
    typedef std::map <IdentifierHash, std::set<uint32_t> > PRD_ROB_Map;
    typedef std::map <uint32_t, std::set<IdentifierHash> > ROB_RDO_Map;
    
    //    const std::string* RPCCorrMap;
    //    const std::string* RPCConfMap;

    bool buildOfflineOnlineMap(void);
    
    const RpcIdHelper*    m_pRpcIdHelper;
    
    OfflineOnlineMap        m_RDOmap;
    OfflineOnlineHashMap    m_HashVec;
    PRD_RDO_Map             m_PRD_RDO_map;
    PRD_ROB_Map             m_PRD_ROB_map;
    ROB_RDO_Map             m_ROB_RDO_map;
    std::vector<uint32_t>   m_fullListOfRobIds;
    Identifier              m_offline_id[2][32][10];
    const Identifier m_uninitialized_identifier;
    
    StringProperty  m_conf_filepath;
    StringProperty  m_conf_filename;
    StringProperty  m_corr_filename;
    BooleanProperty m_cosmic_configuration;
    StringProperty  m_database_repository;

    // 04/08/2009 L. Bellagamba: Cabling maps from Cool 
    BooleanProperty m_RPCMapfromCool;
    ToolHandle<IRPCCablingDbTool> m_condDataTool;

    // 15/03/2010 L. Bellagamba: Trigger Roads from Cool 
    BooleanProperty m_RPCTriggerRoadsfromCool;
    ToolHandle<IRPCTriggerDbTool> m_condTriggerTool;

    // 8/1/2015  M. Corradi: Pad parameters
    BooleanProperty m_ApplyFeetPadThresholds;
    BooleanProperty m_ForceFeetPadThresholdsFromJO;
    std::vector<unsigned short int> m_FeetPadThresholds;
    
    // list of RPCPadParameters
    RPCPadParameters  m_RPCPadParameters_array[64][8];
};


#endif    //  MUONRPC_CABLING_MUONRPC_CABLINGSVC_H 

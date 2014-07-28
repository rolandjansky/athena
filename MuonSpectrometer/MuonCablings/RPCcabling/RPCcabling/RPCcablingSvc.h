/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCcablingSvc_H
#define RPCcablingSvc_H


#ifndef LVL1_STANDALONE

#include <string>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"

#include "MuonIdHelpers/RpcIdHelper.h"

#include "RPCcablingInterface/IRPCcablingSvc.h"
#include "RPCcabling/CablingRPC.h"

#include <math.h>

class RpcPadIdHash;

class IRPCgeometrySvc;

class RPCcablingSvc : public Service,
                      virtual public IRPCcablingSvc
{
    private:
    typedef std::map <Identifier,const RDOindex*,std::less < Identifier > > OfflineOnlineMap;
    typedef std::vector<const RDOindex*> OfflineOnlineHashMap;
    
    RpcPadIdHash* m_padHashIdHelper;

    virtual std::string rpcCabSvcType() const {return "simulationLikeInitialization";}
    bool buildOfflineOnlineMap(void);
    
    unsigned int checkLowCode(unsigned int) const;
    unsigned int checkHighCode(unsigned int) const;

    const RpcIdHelper*     m_pRpcIdHelper;
    const IRPCgeometrySvc* m_pRpcGeo;
    
    OfflineOnlineMap        m_RDOmap;
    OfflineOnlineHashMap    m_HashVec;
    std::vector<uint32_t>   m_fullListOfRobIds;
    Identifier              m_offline_id[2][32][10];

    StringProperty  m_conf_filename;
    StringProperty  m_corr_filename;
    StringProperty  m_database_repository;
    BooleanProperty m_cosmic_configuration;

   
   
//     bool etaPhiDim(unsigned short int SubsystemId,
//                    unsigned short int SectorId,
//                    unsigned short int PADId,
// 		   PADdim& dim) const;
    
    
//     bool giveEtaPhi(unsigned long int EtaLowBorder,
//                     unsigned long int EtaHighBorder,
//                     unsigned long int PhiLowBorder,
// 		    unsigned long int PhiHighBorder,
//                     PADdim& etaphi) const;

//     void computeEtaPhi(float etaLow[3], float etaHigh[3], float etaRad,
//                        float phiLow[3], float phiHigh[3],
// 		       float& etaMin, float& etaMax, 
//                        float& phiMin, float& phiMax) const;


//     std::vector<uint32_t> getHash(float etaMin, float etaMax,
//                                   float phiMin,float phiMax) const;


    
    public:
    // Constructor and other Service methods
    RPCcablingSvc(const std::string& name,ISvcLocator* sl);

    virtual ~RPCcablingSvc() {}

    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

    virtual StatusCode initialize(void);

    virtual StatusCode finalize(void);



    // Interface implementation
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

    virtual bool give_LowPt_borders (unsigned short int SubsystemId,
                                     unsigned short int SectorId,
			             unsigned short int RoIId,
			             unsigned int& EtaLowBorder,
				     unsigned int& EtaHighBorder,
				     unsigned int& PhiLowBorder,
				     unsigned int& PhiHighBorder) const;

    virtual bool give_HighPt_borders (unsigned short int SubsystemId,
                                      unsigned short int SectorId,
			              unsigned short int RoIId,
			              unsigned int& EtaLowBorder,
				      unsigned int& EtaHighBorder,
				      unsigned int& PhiLowBorder,
				      unsigned int& PhiHighBorder) const;

    // dummy implementation of a new interface 
    
    
    bool give_RoI_borders_id (unsigned short int /*SubsystemId*/,
				 unsigned short int /*SectorId*/,
				 unsigned short int /*RoIId*/,
				 Identifier& /*EtaLowBorder_id*/,
				 Identifier& /*EtaHighBorder_id*/,
				 Identifier& /*PhiLowBorder_id*/,
				 Identifier& /*PhiHighBorder_id*/) const 
    {return false;};
  
    bool give_LowPt_borders_id (unsigned short int /*SubsystemId*/,
				unsigned short int /*SectorId*/,
				unsigned short int /*RoIId*/,
				Identifier& /*EtaLowBorder_id*/,
				Identifier& /*EtaHighBorder_id*/,
				Identifier& /*PhiLowBorder_id*/,
				Identifier& /*PhiHighBorder_id*/) const 
    {return false;};
  

    bool give_HighPt_borders_id (unsigned short int /*SubsystemId*/,
				 unsigned short int /*SectorId*/,
				 unsigned short int /*RoIId*/,
				 Identifier& /*EtaLowBorder_id*/,
				 Identifier& /*EtaHighBorder_id*/,
				 Identifier& /*PhiLowBorder_id*/,
				 Identifier& /*PhiHighBorder_id*/) const 
    {return false;};
    


    virtual unsigned long int strip_code_fromOffId (std::string /*stationName*/, 
						    int /*stationEta*/, 
						    int /*stationPhi*/, 
						    int /*doubletR*/, int /*doubletZ*/, int /*doubletPhi*/, 
						    int /*gasGap*/, int /*measuresPhi*/, int /*strip*/) const {return 0;}

    Identifier strip_OffId_fromCode (unsigned long int /*strip_code*/) const
    {Identifier empty; return empty;}
    
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
    
    // map initialization method 
    virtual StatusCode initMappingModel(IOVSVC_CALLBACK_ARGS) {return StatusCode::SUCCESS;}
    
    
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
    
    std::vector<uint32_t> giveFullListOfRobIds() const;

    // hash function 
    virtual RpcPadIdHash* padHashFunction() const {return m_padHashIdHelper;}


    // METHOD USED BY RPCgeometry
    std::vector<const RDOindex*> GiveHashVec() const;

};

#endif



#endif 

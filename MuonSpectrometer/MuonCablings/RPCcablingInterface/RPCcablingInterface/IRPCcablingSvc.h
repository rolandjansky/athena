/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IRPCCABLINGSVC_H
#define IRPCCABLINGSVC_H


#ifndef LVL1_STANDALONE

#include "GaudiKernel/IInterface.h"
#include <string>
#include <vector>
#include <algorithm>


#include "RPCcablingInterface/CablingRPCBase.h"
#include "RPCcablingInterface/CMAparameters.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "RPCcablingInterface/RDOindex.h"

// forward declaration
class Identifier;
class RpcPadIdHash;


// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_RPCcablingSvc("RPCcablingSvc", 1 , 0);

class IRPCcablingSvc : virtual public IInterface 
{
  public:

  // / Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_RPCcablingSvc; }


  // Retrieve the cabling implementation
  virtual const CablingRPCBase* getRPCCabling() const=0;  


  // map initialization method 
  virtual StatusCode initMappingModel(IOVSVC_CALLBACK_ARGS) = 0;

  // RPCcablingSvc Interface
   virtual const CablingRPCBase::RDOmap& give_RDOs() const=0;
    
   virtual const CMAparameters::CMAlist give_CMAs(const int,const ViewType,
                                                const int,const int) const=0;
    
   virtual void printType(int,int,std::string,bool)const=0;
    
   virtual void printSector(int,int,std::string,bool)const=0;

   virtual bool give_global_address(unsigned int,int&,int&,int&,int&) const=0;
				    
   virtual bool give_RoI_borders (unsigned short int SubsystemId,
                                  unsigned short int SectorId,
		                  unsigned short int RoIId,
		                  unsigned int& EtaLowBorder,
			          unsigned int& EtaHighBorder,
			          unsigned int& PhiLowBorder,
			          unsigned int& PhiHighBorder) const=0;

   virtual bool give_RoI_borders_id (unsigned short int SubsystemId,
                                  unsigned short int SectorId,
		                  unsigned short int RoIId,
		                  Identifier& EtaLowBorder_id,
			          Identifier& EtaHighBorder_id,
			          Identifier& PhiLowBorder_id,
			          Identifier& PhiHighBorder_id) const=0;

   virtual bool give_LowPt_borders (unsigned short int SubsystemId,
                                     unsigned short int SectorId,
			             unsigned short int RoIId,
			             unsigned int& EtaLowBorder,
				     unsigned int& EtaHighBorder,
				     unsigned int& PhiLowBorder,
				     unsigned int& PhiHighBorder) const=0;

   virtual bool give_LowPt_borders_id (unsigned short int SubsystemId,
				       unsigned short int SectorId,
				       unsigned short int RoIId,
				       Identifier& EtaLowBorder_id,
				       Identifier& EtaHighBorder_id,
				       Identifier& PhiLowBorder_id,
				       Identifier& PhiHighBorder_id) const=0;


   virtual bool give_HighPt_borders (unsigned short int SubsystemId,
                                      unsigned short int SectorId,
			              unsigned short int RoIId,
			              unsigned int& EtaLowBorder,
				      unsigned int& EtaHighBorder,
				      unsigned int& PhiLowBorder,
				      unsigned int& PhiHighBorder) const=0;

   virtual bool give_HighPt_borders_id (unsigned short int SubsystemId,
					unsigned short int SectorId,
					unsigned short int RoIId,
					Identifier& EtaLowBorder_id,
					Identifier& EtaHighBorder_id,
					Identifier& PhiLowBorder_id,
					Identifier& PhiHighBorder_id) const=0;
   
    virtual bool give_Pad_Parameters(unsigned short int logic_sector,
                                     unsigned short int PADId,
                                     bool & feet,
                                     bool & eta_and_phi,
                                     unsigned short int  & cma_mask,
                                     unsigned short int  & feet_th0,
                                     unsigned short int  & feet_th1,
                                     unsigned short int  & feet_th2 ) const=0;
    
    
   virtual unsigned long int strip_code_fromOffId (std::string stationName, 
						    int stationEta, 
						    int stationPhi, 
						    int doubletR, int doubletZ, int doubletPhi, 
						    int gasGap, int measuresPhi, int strip) const=0;

   virtual Identifier strip_OffId_fromCode(unsigned long int strip_code_cab) const=0;   
   
   virtual std::list<unsigned int> give_strip_code
                                           (unsigned short int SubsystemId,
                                            unsigned short int SectorId,
			                    unsigned short int PADId,
				            unsigned short int CMAId,
				            unsigned short ijk,
				            unsigned short int Channel) const=0;

   virtual std::list<Identifier> give_strip_id
                                           (unsigned short int SubsystemId,
                                            unsigned short int SectorId,
			                    unsigned short int PADId,
				            unsigned short int CMAId,
				            unsigned short ijk,
				            unsigned short int Channel) const=0;
    
   //give access to onlineID from compact OfflineID (return == true if OK)
   virtual bool giveOnlineID(const Identifier compactID,
                             unsigned short int& ROBid,
                             unsigned short int& m_RODid,
                             unsigned short int& m_side,
                             unsigned short int& m_SLid,
                             unsigned short int& m_RXid,
                             unsigned short int& m_PADid) const=0;
   
   virtual bool giveOnlineID(const unsigned int hashID,
                             unsigned short int& ROBid,
                             unsigned short int& m_RODid,
                             unsigned short int& m_side,
                             unsigned short int& m_SLid,
                             unsigned short int& m_RXid,
                             unsigned short int& m_PADid) const=0;
   
   virtual bool giveOffflineID(unsigned short int Side,
                               unsigned short int Sector,
			       unsigned short int PADID,
			       Identifier& ID) const=0;

    
  //map a single PRD HashId into a vector of RDO HashIds
  virtual StatusCode giveRDO_fromPRD(const IdentifierHash prdHashId,  
                                     std::vector<IdentifierHash>& rdoHashVec) const=0;
                                  
  //map a vector of PRD HashIds into a vector of RDO HashIds
  virtual StatusCode giveRDO_fromPRD(const std::vector<IdentifierHash>& prdHashVec,  
                                     std::vector<IdentifierHash>& rdoHashVec) const=0;
                                     
  //map a single ROB Id into a vector of RDO HashIds
  virtual StatusCode giveRDO_fromROB(const uint32_t robId,  
                                     std::vector<IdentifierHash>& rdoHashVec) const=0;
                                  
  //map a vector of ROB Ids into a vector of RDO HashIds
  virtual StatusCode giveRDO_fromROB(const std::vector<uint32_t>& robIdVec,  
                                     std::vector<IdentifierHash>& rdoHashVec) const=0;
                                  
  //map a single PRD HashId into a vector of corresponding ROB Ids
  virtual StatusCode giveROB_fromPRD(const IdentifierHash prdHashId,
                                     std::vector<uint32_t>& robIdVec) const=0;
                                     
  //map a vector of PRD HashIds into a vector of corresponding ROB Ids
  virtual StatusCode giveROB_fromPRD(const std::vector<IdentifierHash>& prdHashVec,
                                     std::vector<uint32_t>& robIdVec) const=0;
                                  
  //map a single RDO HashId into a single ROB Id
  virtual StatusCode giveROB_fromRDO(const IdentifierHash rdoHashId,
                                     uint32_t& robId) const=0;
                                     
  //map a vector of RDO HashIds into a vector of corresponding ROB Ids
  virtual StatusCode giveROB_fromRDO(const std::vector<IdentifierHash>& rdoHashVec,
                                     std::vector<uint32_t>& robIdVec) const=0;
   
    //provide the full list of ROB id
    virtual std::vector<uint32_t> giveFullListOfRobIds() const=0;
    
    // method used in RPCgeometry
    virtual std::vector<const RDOindex*> GiveHashVec() const=0;

   // hash function 
    virtual RpcPadIdHash* padHashFunction() const=0;
    
   // access to service name 
    virtual std::string rpcCabSvcType() const = 0;


};

#endif  //LVL1_STANDALONE
 
#endif  //IRPCCABLINGSVC_H

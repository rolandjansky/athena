/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "TrigL2MuonSA/RpcDataPreparator.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"

#include "Identifier/IdentifierHash.h"
#include "RPCcablingInterface/RpcPadIdHash.h" 

#include "TrigL2MuonSA/RpcData.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_RpcDataPreparator("IID_RpcDataPreparator", 1, 0);

const InterfaceID& TrigL2MuonSA::RpcDataPreparator::interfaceID() { return IID_RpcDataPreparator; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcDataPreparator::RpcDataPreparator(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
   AlgTool(type,name,parent),
   m_msg(0),
   m_storeGateSvc( "StoreGateSvc", name ),
   m_rpcRawDataProvider("Muon__RPC_RawDataProviderTool"),
   m_rpcGeometrySvc("RPCgeometrySvc",""),
   m_iRpcCablingSvc(0),
   m_rpcCabling(0),
   m_iMdtCablingSvc("MDTcablingSvc",""),
   m_robDataProvider(0),
   m_recMuonRoIUtils(),
   m_maskUncCMAch(false)
{
   declareInterface<TrigL2MuonSA::RpcDataPreparator>(this);

   declareProperty("RPC_RawDataProvider", m_rpcRawDataProvider);
   declareProperty("RPC_GeometrySvc",     m_rpcGeometrySvc);
   declareProperty("MDT_Cabbling",        m_iMdtCablingSvc);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcDataPreparator::~RpcDataPreparator() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcDataPreparator::initialize()
{
   // Get a message stream instance
   m_msg = new MsgStream( msgSvc(), name() );
   msg() << MSG::DEBUG << "Initializing RpcDataPreparator - package version " << PACKAGE_VERSION << endreq;
   
   StatusCode sc;
   sc = AlgTool::initialize();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not initialize the AlgTool base class." << endreq;
      return sc;
   }
   
   // Locate the StoreGateSvc
   sc =  m_storeGateSvc.retrieve();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      return sc;
   }

   // Locate RPC RawDataProvider
   sc = m_rpcRawDataProvider.retrieve();
   if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Could not retrieve " << m_rpcRawDataProvider << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved tool " << m_rpcRawDataProvider << endreq;

   // Locate RPC GeometrySvc
   sc = m_rpcGeometrySvc.retrieve();
   if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Could not retrieve " << m_rpcGeometrySvc << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved service " << m_rpcGeometrySvc << endreq;

   // RPC cabling
   const IRPCcablingServerSvc* pRpcCabGet = 0;
   sc = service("RPCcablingServerSvc",pRpcCabGet,true);
   if( sc.isFailure() ) {
      msg() << MSG::ERROR << "Unable to retrieve the RPC cabling Server Service" << endreq;
      return sc;
   }
   sc = pRpcCabGet->giveCabling(m_iRpcCablingSvc);
   if( sc.isFailure() ) {
      msg() << MSG::ERROR << "Unable to retrieve the RPC cabling Service from Server" << endreq;
      return sc;
    }
   m_rpcCabling = m_iRpcCablingSvc->getRPCCabling();

   // MDT cabling
   sc = m_iMdtCablingSvc.retrieve();
   if ( sc.isFailure() ) {
     msg() << MSG::ERROR << "Could not retrieve "
	   << m_iMdtCablingSvc << endreq;
     return sc;
   } 

   // Locate ROBDataProvider
   std::string serviceName = "ROBDataProvider";
   IService* svc = 0;
   sc = service("ROBDataProviderSvc", svc);
   if(sc.isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve " << serviceName << endreq;
      return sc;
   }
   m_robDataProvider = dynamic_cast<ROBDataProviderSvc*> (svc);
   if( m_robDataProvider == 0 ) {
      msg() << MSG::ERROR << "Could not cast to ROBDataProviderSvc " << endreq;
      return StatusCode::FAILURE;
   }
   msg() << MSG::DEBUG << "Retrieved service " << serviceName << endreq;

   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcDataPreparator::prepareData(const LVL1::RecMuonRoI* p_roi,
							TrigL2MuonSA::RpcHits& rpcHits,
							RpcFitResult& rpcFitResult)
{
   rpcHits.clear();

   unsigned short int subsystemID =  p_roi->subsysID();
   unsigned short int sectorID    =  p_roi->sectorID();
   unsigned short int roiNumber =    p_roi->getRoINumber();

   const RpcPad* p_rpcPad = 0;
   unsigned int subsys_id = (subsystemID==1)? 0x65 : 0x66;
   unsigned int robId     = (subsys_id << 16) | (sectorID/2);
   StatusCode sc = getRpcPad(robId,subsystemID,sectorID,roiNumber,p_rpcPad);
   if( sc != StatusCode::SUCCESS ) {
      msg() << MSG::DEBUG << "getRpcPad failed: sc=" << sc << endreq;
      return sc;
   }

   sc = decodeRpcPad(p_rpcPad,rpcHits,rpcFitResult);
   if( sc != StatusCode::SUCCESS ) {
      msg() << MSG::DEBUG << "decodeRpcPad failed: sc=" << sc << endreq;
      return sc;
   }

   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcDataPreparator::decodeRpcPad(const RpcPad* p_rdo,
							 TrigL2MuonSA::RpcHits& /*rpcHits*/,
							 TrigL2MuonSA::RpcFitResult& rpcFitResult)
{
  RPCrawData  rpc_digits;
  CMAdata     cma_patterns((unsigned long int)0);

  if (p_rdo==0 ) {
    msg() << MSG::ERROR << "no Sector Logic and the PAD Id is given" << endreq;
    return StatusCode::FAILURE;
  }

  unsigned long int sector = (long) p_rdo->sector();
  unsigned short int SubsystemId = (sector < 32)? 0 : 1;
  unsigned short int SectorId    = sector%32;
  unsigned short int PADId       = p_rdo->onlineId();
  
  rpcFitResult.SectorId = SectorId;

  RpcPad::const_iterator itPad = p_rdo->begin(); 
  
  for (; itPad!=p_rdo->end(); ++itPad) {
    
    const RpcCoinMatrix * cma = (*itPad);
    
    unsigned long int cmaId = (long) cma->onlineId();
    unsigned short int CMAId =  cmaId & 3;
    unsigned short int lh    = (cmaId >> 2) & 1;
    
    const CMAparameters* CMA = 
      m_rpcCabling->give_CMA(SubsystemId,SectorId,PADId,CMAId);
    
    std::list <unsigned int> CodeList;
    
    //for each cma for over fired channels
    RpcCoinMatrix::const_iterator itCh = (*itPad)->begin();
    
    for (; itCh!=(*itPad)->end(); ++itCh) {
      CodeList.clear();
      const RpcFiredChannel * fChannel = (*itCh);
      
      unsigned short int ijk     = fChannel->ijk();
      unsigned short int channel = fChannel->channel();
      
      if (m_rpcGeometrySvc->rpcGeometry()->isFull() ) {
	float time =  (fChannel->bcid()-2)*25 + fChannel->time()*3.125;
	
	// load Low-pt data into Level-1 logic
	bool trigPatt = (ijk != 6);
	if(lh == 0 && ijk != 7 && CMA !=0 ) {
	  CMA->give_strip_code(sector,lh,ijk,channel,CodeList);
	  
	  if( (CodeList.size() || m_maskUncCMAch) && trigPatt) {
	    RPCrawDigit Digit(0,CodeList,time,m_rpcGeometrySvc->rpcGeometry());
	    const RPCdigit* digit = rpc_digits << Digit;
	    cma_patterns.create_patterns(CMA,digit);
	  }
	}

	// load High-pt data into Level-1 logic
	if(lh == 1 && (ijk == 2 || ijk == 3 || ijk == 4 || ijk == 5) && CMA!=0) {
	  CMA->give_strip_code(sector,lh,ijk,channel,CodeList);
	  if(CodeList.size() || m_maskUncCMAch) {
	    RPCrawDigit Digit(0,CodeList,time,m_rpcGeometrySvc->rpcGeometry());
	    const RPCdigit* digit = rpc_digits << Digit;
	    cma_patterns.create_patterns(CMA,digit);
	  }
	}
	
      } else {

	float time = fChannel->time();
	
      	// load Low-pt data into Level-1 logic
	if(lh == 0 && ijk!=6 && ijk != 7 ) {
	  
	  CMA->give_strip_code(sector,lh,ijk,channel,CodeList);
	  
	  if(CodeList.size() || m_maskUncCMAch) {
	    RPCrawDigit Digit(0,CodeList,time,m_rpcGeometrySvc->rpcGeometry());
	    const RPCdigit* digit = rpc_digits << Digit;
	    cma_patterns.create_patterns(CMA,digit);
	  }
	} 
	
	// load High-pt data into Level-1 logic
	if(lh == 1 && (ijk == 2 || ijk == 3 || ijk == 4 || ijk == 5)) {
	  CMA->give_strip_code(sector,lh,ijk,channel,CodeList);
	  
	  if(CodeList.size() || m_maskUncCMAch) {
	    RPCrawDigit Digit(0,CodeList,time,m_rpcGeometrySvc->rpcGeometry());
	    const RPCdigit* digit = rpc_digits << Digit;
	    cma_patterns.create_patterns(CMA,digit);
	  }
	}
      }
    }
  }

  // get the trigger from the CMA
  CMAdata::PATTERNSlist Eta = cma_patterns.eta_cma_patterns();
  CMAdata::PATTERNSlist::const_iterator EtaPat = Eta.begin();
  CMAtrigger eta(LogicEmulation,&(*EtaPat));
  while(EtaPat != Eta.end()) {
    eta += (*EtaPat).give_trigger(LogicEmulation);
    ++EtaPat;
  }
  
  CMAdata::PATTERNSlist Phi = cma_patterns.phi_cma_patterns();
  CMAdata::PATTERNSlist::const_iterator PhiPat = Phi.begin();
  CMAtrigger phi(LogicEmulation,&(*PhiPat));
  while(PhiPat != Phi.end()) {
    phi += (*PhiPat).give_trigger(LogicEmulation);
    ++PhiPat;
  }
 
  int etaTrigType = eta.trigger().type();
  int phiTrigType = phi.trigger().type();
       
  if( etaTrigType && phiTrigType) {
    //    eta.Print(std::cout,true);
    //    phi.Print(std::cout,true);
    
    int etaThreshold = eta.trigger().threshold();
    int phiThreshold = phi.trigger().threshold();
    
    if(etaTrigType == phiTrigType) {
      rpcFitResult.type = etaTrigType;
      if(etaThreshold<=phiThreshold) rpcFitResult.threshold=etaThreshold;
      else rpcFitResult.threshold = phiThreshold;
    } else if (etaTrigType < phiTrigType) {
      rpcFitResult.type      = etaTrigType;
      rpcFitResult.threshold = etaThreshold;
    }
    else {
      rpcFitResult.type      = phiTrigType;
      rpcFitResult.threshold = phiThreshold;
    }
    
    RPCdigit eta1(*eta.lowPt_hit());
    RPCdigit eta2(*eta.pivot_hit());
    
    RPCdigit phi1(*phi.lowPt_hit());
    RPCdigit phi2(*phi.pivot_hit());
    
    // TOF measurement
    float f_phi1_beta = stationBeta(phi1, eta1, SubsystemId, SectorId); //beta from the first station
    float f_phi2_beta = stationBeta(phi2, eta2, SubsystemId, SectorId); //beta from the first station

    if(!phi1.set_to_chamber(eta1.decoding().rpc_z_index())) return StatusCode::FAILURE;
    if(!phi2.set_to_chamber(eta2.decoding().rpc_z_index())) return StatusCode::FAILURE;
    
    rpcFitResult.rpc1[0] = phi1.x();
    rpcFitResult.rpc1[1] = phi1.y();
    rpcFitResult.rpc1[2] = eta1.z();
    eta1.station_radius(rpcFitResult.rpc1[3]);
    
    rpcFitResult.rpc2[0] = phi2.x();
    rpcFitResult.rpc2[1] = phi2.y();
    rpcFitResult.rpc2[2] = eta2.z();
    eta2.station_radius(rpcFitResult.rpc2[3]);

    float f_phi3_beta = -1.0;
        
    if(rpcFitResult.type == 2) {
      RPCdigit eta3(*eta.highPt_hit());
      RPCdigit phi3(*phi.highPt_hit());
      
      f_phi3_beta = stationBeta(phi3, eta3, SubsystemId, SectorId); //beta from the first station
      
      if(!phi3.set_to_chamber(eta3.decoding().rpc_z_index())) return StatusCode::FAILURE;
      
      rpcFitResult.rpc3[0] = phi3.x();
      rpcFitResult.rpc3[1] = phi3.y();
      rpcFitResult.rpc3[2] = eta3.z();
      eta3.station_radius(rpcFitResult.rpc3[3]);

    }
    float f_beta=-1;
    if (f_phi3_beta!=-1) {
      f_beta = (f_phi1_beta + f_phi2_beta + f_phi3_beta )/3;
    } else {
      f_beta = (f_phi1_beta + f_phi2_beta )/2;
    }

    rpcFitResult.beta = f_beta;

    rpcFitResult.isSuccess = true;

  } else {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcDataPreparator::getRpcPad(unsigned int robId, unsigned short int subsystemID,
						      unsigned short int sectorID, unsigned short int roiNumber,
						      const RpcPad*& p_pad)
{
   std::vector<uint32_t> v_robIds;
   v_robIds.push_back(robId);
   
   m_robDataProvider->addROBData(v_robIds);

   p_pad = 0;

   std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> v_robFragments;
   
   const RpcPadContainer* pRpcPadContainer = Muon::MuonRdoContainerAccess::retrieveRpcPad("RPCPAD");
   if(pRpcPadContainer==0) {
      msg() << MSG::DEBUG << "Rpc PAD container not registered by MuonRdoContainerManager" << endreq;
      msg() << MSG::DEBUG << "-> Retrieving it from the StoreGate" << endreq;
      StatusCode sc = m_storeGateSvc->retrieve(pRpcPadContainer, "RPCPAD");
      if( sc.isFailure() ) {
	 msg() << MSG::ERROR << "Retrieval of RpcPadContainer failed" << endreq;
	 return sc;
      }
   }

   m_robDataProvider->getROBData(std::vector<uint32_t>(1,robId),v_robFragments);

   Identifier rpc_pad_id;       // identifier for accessing the RPC PAD
   unsigned int logic_sector = 0;
   unsigned short int PADId  = 0;
   bool success = m_rpcCabling->give_PAD_address(subsystemID,sectorID,roiNumber,logic_sector,PADId,rpc_pad_id);
   if(!success) {
      msg() << MSG::DEBUG << "error in give_PAD_address" << endreq;
      return StatusCode::FAILURE;
   }

   RpcPadIdHash padHashFunction = *(m_iRpcCablingSvc->padHashFunction());
   IdentifierHash pad_id = padHashFunction(rpc_pad_id);

   if( m_rpcRawDataProvider->convert(v_robFragments,std::vector<IdentifierHash>(1,pad_id)).isFailure()) {
      msg() << MSG::DEBUG << "Failed to convert RPC PAD ID=" << pad_id << endreq;
      return StatusCode::FAILURE;
   }

   RpcPadContainer::const_iterator itPad = pRpcPadContainer->indexFind(pad_id);
   if( itPad==pRpcPadContainer->end() ) {
      msg() << MSG::DEBUG << "Failed to retrieve PAD hash Id " << pad_id << endreq;
      return StatusCode::FAILURE;
   }

   p_pad = *itPad;
   
   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcDataPreparator::finalize()
{
   msg() << MSG::DEBUG << "Finalizing RpcDataPreparator - package version " << PACKAGE_VERSION << endreq;
   
   // delete message stream
   if ( m_msg ) delete m_msg;
   
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

float TrigL2MuonSA::RpcDataPreparator::stationBeta(RPCdigit& phi,
						 RPCdigit& eta,
						 unsigned short int SubsystemId,
						 unsigned short int SectorId) const
{
  const float INVSIGVEL = 4.8; // [nsec/m], signal velocity

  //===================================================================
  //========first station==============================================
  //===================================================================
  
  // Station related parameters
  int i_phi_RPCChamber = eta.decoding().rpc_z_index();
  int i_phi_RPCStation = phi.decoding().lvl1_station();
  int i_phi_RPCLogicSector = phi.decoding().logic_sector();
  
  int i_phi_doubletZ = m_rpcCabling->give_doubletZ(SubsystemId,SectorId,i_phi_RPCStation,i_phi_RPCChamber);
  
  // propagation length along the z direction
  float f_phi_zmin=0., f_phi_zmax=0.;
  m_rpcGeometrySvc->rpcGeometry()->give_rpc_boundaries(i_phi_RPCLogicSector,i_phi_RPCStation,i_phi_RPCChamber,f_phi_zmin,f_phi_zmax);
  
  float f_phi_propLength=0.;
  if(eta.z()>=0)	f_phi_propLength = ((1==i_phi_doubletZ)?(eta.z()-f_phi_zmin):(f_phi_zmax-eta.z()));
  else	f_phi_propLength = ((1==i_phi_doubletZ)?((-1)*eta.z()-f_phi_zmin):(f_phi_zmax-(-1)*eta.z()));
  f_phi_propLength += (f_phi_zmax-f_phi_zmin); // it seems to be more aacurate this way
  
  // TOF
  float f_dist = sqrt(phi.x()*phi.x()+phi.y()*phi.y()+eta.z()*eta.z());
  float f_muTOF = f_dist/100/CLHEP::c_light;
  float f_phi_TOF = f_muTOF + phi.time() - f_phi_propLength/100*INVSIGVEL + 3.125/2.; //[ns]
  
  // beta
  float f_phi_beta = f_dist/100/f_phi_TOF/CLHEP::c_light;

  return f_phi_beta;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------


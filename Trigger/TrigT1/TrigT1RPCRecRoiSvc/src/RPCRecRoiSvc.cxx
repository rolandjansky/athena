/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <fstream>
#include <iomanip>

#include "TrigT1RPCRecRoiSvc/RPCRecRoiSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"

using namespace LVL1RPC;

StatusCode RPCRecRoiSvc::initialize (void) 
{

  MsgStream log( messageService(), name() );
  m_MuonMgr  = 0;
  StoreGateSvc* detStore;
  StatusCode sc = service("DetectorStore",detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << "DetectorStore service not found !" << endreq; 
  } else {
    sc = detStore->retrieve(m_MuonMgr);
    if ( sc.isFailure() ) {
      log << MSG::ERROR << " Cannot retrieve MuonReadoutGeometry " << endreq;
      return sc;
    } else {
      log << MSG::DEBUG << "Found the MuonDetDescrMgr " << endreq;
    }
  }

  m_rPCcablingSvc = 0;
  const IRPCcablingServerSvc* RpcCabGet = 0;
  sc = service("RPCcablingServerSvc",RpcCabGet,1);
  if(sc.isFailure())
   {
      log << MSG::WARNING
	  << "Unable to retrieve the RPC cabling Server Service"
	  << endreq;
      return StatusCode::FAILURE;
    }
 
  sc = RpcCabGet->giveCabling(m_rPCcablingSvc);
  if(sc.isFailure())
    {
      log << MSG::WARNING
	  << "Unable to retrieve the RPC cabling Service from the Server"
	  << endreq;
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS; 
}


void RPCRecRoiSvc::reconstruct (const unsigned int & roIWord) const 
{
    
  //  first extract the parts holding the sector address and 
  //  and the RoI/Overlap from the 32 bit word
  unsigned int sectorAddress = (roIWord & 0x003FC000) >> 14;
  unsigned int sectorRoIOvl  = (roIWord & 0x000007FC) >> 2;

  // subsystem ID and  sector ID
  m_side =  sectorAddress & 0x00000001;
  m_sector = (sectorAddress & 0x0000003e) >> 1;
  // RoI
  m_roi = sectorRoIOvl & 0x0000001F;

  m_eta = 0.;
  m_phi = 0.;
  m_etaMin = 0.;
  m_phiMin = 0.;
  m_etaMax = 0.;
  m_phiMax = 0.;

  
  // Get the strips delimiting the RoIs from rPCcablingSvc
  Identifier EtaLowBorder_id;
  Identifier EtaHighBorder_id;
  Identifier PhiLowBorder_id;
  Identifier PhiHighBorder_id;
  
  Amg::Vector3D EtaLowBorder_pos(0.,0.,0.);
  Amg::Vector3D EtaHighBorder_pos(0.,0.,0.);
  Amg::Vector3D PhiLowBorder_pos(0.,0.,0.);
  Amg::Vector3D PhiHighBorder_pos(0.,0.,0.);

  
  if(m_rPCcablingSvc->
     give_RoI_borders_id(m_side,
			 m_sector,
			 m_roi,
			 EtaLowBorder_id,
			 EtaHighBorder_id,
			 PhiLowBorder_id,
			 PhiHighBorder_id)) {
    
    const MuonGM::RpcReadoutElement* EtaLowBorder_descriptor =
      m_MuonMgr->getRpcReadoutElement(EtaLowBorder_id);
    EtaLowBorder_pos = EtaLowBorder_descriptor->stripPos(EtaLowBorder_id);

    const MuonGM::RpcReadoutElement* EtaHighBorder_descriptor =
      m_MuonMgr->getRpcReadoutElement(EtaHighBorder_id);
    EtaHighBorder_pos = EtaHighBorder_descriptor->stripPos(EtaHighBorder_id);

    const MuonGM::RpcReadoutElement* PhiLowBorder_descriptor =
      m_MuonMgr->getRpcReadoutElement(PhiLowBorder_id);
    PhiLowBorder_pos = PhiLowBorder_descriptor->stripPos(PhiLowBorder_id);

    const MuonGM::RpcReadoutElement* PhiHighBorder_descriptor =
      m_MuonMgr->getRpcReadoutElement(PhiHighBorder_id);
    PhiHighBorder_pos =   PhiHighBorder_descriptor->stripPos(PhiHighBorder_id);

    m_etaMin=EtaLowBorder_pos.eta();
    m_etaMax=EtaHighBorder_pos.eta();
    m_eta=(m_etaMin+m_etaMax)/2.;

    // ensures the correct order
    if (m_etaMin>m_etaMax) {
        double tmp = m_etaMax;
        m_etaMax = m_etaMin;
        m_etaMin  = tmp;
    }
    
    m_phiMin = PhiLowBorder_pos.phi();
    m_phiMax = PhiHighBorder_pos.phi();
    
    m_phi = (m_phiMin+m_phiMax)/2.;
    if(m_phi < -M_PI) {
      m_phi += 2*M_PI;
    }else if (m_phi > M_PI) {
      m_phi -= 2*M_PI;
    }
    
    // ensures the correct order
    if (m_phiMin>m_phiMax&&m_phiMin-m_phiMax<M_PI) {
        double tmp = m_phiMax;
        m_phiMax = m_phiMin;
        m_phiMin  = tmp;
    }
    
  }
}

bool
RPCRecRoiSvc::dumpRoiMap(const std::string& filename)
{  
    MsgStream msg( messageService(), name() );    //get MSG service

    const unsigned int maxSubsystem = 2;
    const unsigned int maxLogicSector = 32;
    const unsigned int maxRoI = 32;

    // ADDED PART FOR ETA-PHI DUMP
    // MC July 2014: add dump the ROI Eta-Phi Map 
    
    std::ofstream roi_map;                          //file storing the RoI/ROB map
    //    roi_map.open("ROI_Mapping.txt", std::ios::out ); //try to open the file
    roi_map.open(filename.c_str(), std::ios::out ); //try to open the file
    if(!roi_map){
        msg << MSG::WARNING << "Unable to open ROI_Mapping file!"<< endreq;
    } else {
        roi_map <<"# side     sector   roi      etaMin       etaMax       phiMin       phiMax     etaMinLow    etaMaxLow    etaMinHigh   etaMaxHigh "<< std::endl;
        roi_map <<"# ------------------------------------------------------------------------------------------------------------------------------ "<< std::endl;
        for(unsigned int side=0;side < maxSubsystem; side++){
            for(unsigned int sector=0;sector < maxLogicSector; sector++){
                for (unsigned int roi=0; roi<maxRoI; roi++){
                    unsigned long int roIWord = (roi<<2)+(side<<14)+(sector<<15);
                    reconstruct(roIWord);
                    double etaMinLow(0),etaMaxLow(0),etaMinHigh(0),etaMaxHigh(0);
                    etaDimLow (etaMinLow, etaMaxLow);
                    etaDimHigh(etaMinHigh,etaMaxHigh);
                    roi_map << std::setw(8)  << side     << " "
                            << std::setw(8)  << sector   << " "
                            << std::setw(8)  << roi      << " "
                            << std::setw(12) << m_etaMin << " "
                            << std::setw(12) << m_etaMax << " "
                            << std::setw(12) << m_phiMin << " "
                            << std::setw(12) << m_phiMax << " "
                            << std::setw(8) << etaMinLow << " "
                            << std::setw(8) << etaMaxLow << " "
                            << std::setw(8) << etaMinHigh << " "
                            << std::setw(8) << etaMaxHigh <<  std::endl;
                } 
            }
        }    
        roi_map.close();
    }
    
    return true;
}

void  RPCRecRoiSvc::RoIsize(const unsigned int & roIWord,
             double & etaMin_LowHigh, double & etaMax_LowHigh, double & phiMin_LowHigh, double & phiMax_LowHigh) const
{
    double etaMin_Low=0;
    double etaMin_High=0;
    double etaMax_Low=0;
    double etaMax_High=0;

    reconstruct(roIWord);
    phiMin_LowHigh=m_phiMin;
    phiMax_LowHigh=m_phiMax;
    
    bool low  = etaDimLow(etaMin_Low,etaMax_Low);
    bool high = etaDimHigh(etaMin_High,etaMax_High);

    if (low&&high) {
        etaMin_LowHigh=std::min(etaMin_Low,etaMin_High);
        etaMax_LowHigh=std::max(etaMax_Low,etaMax_High);
    } else if (low) {
        etaMin_LowHigh =std::min(etaMin_Low,m_etaMin);
        etaMax_LowHigh =std::max(etaMax_Low,m_etaMax);
    } else if (high) {
        etaMin_LowHigh =std::min(etaMin_High,m_etaMin);
        etaMax_LowHigh =std::max(etaMax_High,m_etaMax);
    } else  {
        etaMin_LowHigh = m_etaMin;
        etaMax_LowHigh = m_etaMax;
    }
    
    return;
}



bool  RPCRecRoiSvc::etaDimLow (double& etaMin, double& etaMax)  const 
{
  // Get the strips delimiting the RoIs from rPCcablingSvc
  Identifier EtaLowBorder_id;
  Identifier EtaHighBorder_id;
  Identifier PhiLowBorder_id;
  Identifier PhiHighBorder_id;
  Amg::Vector3D EtaLowBorder_pos(0.,0.,0.);
  Amg::Vector3D EtaHighBorder_pos(0.,0.,0.);

  if( !m_rPCcablingSvc-> give_LowPt_borders_id(m_side,
					       m_sector,
					       m_roi,
					       EtaLowBorder_id,
					       EtaHighBorder_id,
					       PhiLowBorder_id,
					       PhiHighBorder_id)) return false;
  
  const MuonGM::RpcReadoutElement* EtaLowBorder_descriptor =
    m_MuonMgr->getRpcReadoutElement(EtaLowBorder_id);
  EtaLowBorder_pos = EtaLowBorder_descriptor->stripPos(EtaLowBorder_id);
  
  const MuonGM::RpcReadoutElement* EtaHighBorder_descriptor =
    m_MuonMgr->getRpcReadoutElement(EtaHighBorder_id);
  EtaHighBorder_pos = EtaHighBorder_descriptor->stripPos(EtaHighBorder_id);
    
  etaMin=EtaLowBorder_pos.eta();
  etaMax=EtaHighBorder_pos.eta();
  if (etaMin>etaMax){
    double tmp=etaMin;
    etaMin=etaMax;
    etaMax=tmp;
  }
  return true;
}

bool RPCRecRoiSvc::etaDimHigh (double& etaMin, double& etaMax)  const 
{
  // Get the strips delimiting the RoIs from rPCcablingSvc
  Identifier EtaLowBorder_id;
  Identifier EtaHighBorder_id;
  Identifier PhiLowBorder_id;
  Identifier PhiHighBorder_id;
  Amg::Vector3D EtaLowBorder_pos(0.,0.,0.);
  Amg::Vector3D EtaHighBorder_pos(0.,0.,0.);

  if(!m_rPCcablingSvc->give_HighPt_borders_id(m_side,
					      m_sector,
					      m_roi,
					      EtaLowBorder_id,
					      EtaHighBorder_id,
					      PhiLowBorder_id,
					      PhiHighBorder_id)) return false;
    
  const MuonGM::RpcReadoutElement* EtaLowBorder_descriptor =
    m_MuonMgr->getRpcReadoutElement(EtaLowBorder_id);
  EtaLowBorder_pos = EtaLowBorder_descriptor->stripPos(EtaLowBorder_id);
  
  const MuonGM::RpcReadoutElement* EtaHighBorder_descriptor =
    m_MuonMgr->getRpcReadoutElement(EtaHighBorder_id);
  EtaHighBorder_pos = EtaHighBorder_descriptor->stripPos(EtaHighBorder_id);
  
  etaMin=EtaLowBorder_pos.eta();
  etaMax=EtaHighBorder_pos.eta();
  if (etaMin>etaMax){
    double tmp=etaMin;
    etaMin=etaMax;
    etaMax=tmp;
  }
  return true;
}

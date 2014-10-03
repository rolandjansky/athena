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
#include "MDTcabling/IMDTcablingSvc.h"

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

  m_mDTcablingSvc = 0;
  sc = service("MDTcablingSvc", m_mDTcablingSvc,1);
  if(sc.isFailure())
    {
      log << MSG::WARNING
	  << "Unable to retrieve the MDT cabling Service"
	  << endreq;
      return StatusCode::FAILURE;
    }


  return StatusCode::SUCCESS; 
}

void RPCRecRoiSvc::reconstruct (const unsigned int & roIWord) const 
{
  m_eta = 0.;
  m_phi = 0.;

  //  first extract the parts holding the sector address and 
  //  and the RoI/Overlap from the 32 bit word
  unsigned int sectorAddress = (roIWord & 0x003FC000) >> 14;
  unsigned int sectorRoIOvl  = (roIWord & 0x000007FC) >> 2;

  // subsystem ID and  sector ID
  unsigned int subSysID =  sectorAddress & 0x00000001;
  unsigned int sectorID = (sectorAddress & 0x0000003e) >> 1;

  // RoI
  unsigned int roiNumber = sectorRoIOvl & 0x0000001F;


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
     give_RoI_borders_id(subSysID,
			 sectorID,
			 roiNumber,
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

    m_phiMin = PhiLowBorder_pos.phi();
    m_phiMax = PhiHighBorder_pos.phi();
    m_phi = (m_phiMin+m_phiMax)/2.;
    
    if(m_phi < -M_PI) {
      m_phi += 2*M_PI;
    }else if (m_phi > M_PI) {
      m_phi -= 2*M_PI;
    }
  }
}

bool
RPCRecRoiSvc::writeRoiRobMap(const std::string& filename)
{  
    MsgStream msg( messageService(), name() );    //get MSG service

    const int maxSubsystem = 2;
    const int maxLogicSector = 32;
    int       maxRoI = 0;
        
    std::ofstream table;                          //file storing the RoI/ROB map
    table.open(filename.c_str(), std::ios::out ); //try to open the file
    if(!table)                                    //check if file is open
    {
        msg << MSG::ERROR
	    << "Unable to open " << filename << " file!"
	    << endreq;
        return false;
    }

    table << "# Roi/ROB map file for the RPC system" << std::endl << std::endl;
    
    
    CablingRPCBase::RDOmap rdo = m_rPCcablingSvc->give_RDOs();
    
    
    // compute the maximum number of RoI into a logic sector
    for(int side=0;side < maxSubsystem; ++side)
    {
        for(int sector=0;sector < maxLogicSector; ++sector)
	{
	    int kPAD_lw = side * 10000 + sector * 100;
	    int kPAD_hg = side * 10000 + sector * 100 + 99;
	    
	    CablingRPCBase::RDOmap::const_iterator PAD_l = rdo.lower_bound(kPAD_lw);
	    CablingRPCBase::RDOmap::const_iterator PAD_h = rdo.upper_bound(kPAD_hg);
	    
	    int RoIs = distance(PAD_l,PAD_h)*4;
	    
	    if( maxRoI < RoIs) maxRoI = RoIs;
	}
    }
    
    table << "maxSubsystem:   " << maxSubsystem << std::endl;
    table << "maxLogicSector: " << maxLogicSector << std::endl;
    table << "maxRoI:         " << maxRoI << std::endl;
    
    float PhiStep  = 0.3926;  // correspond to pi/8
    float PhiWidth = 0.1320;  // correspond to 7.6 degrees
    
    // write the map
    for(unsigned short int side=0;side < maxSubsystem; ++side)
    {
        for(unsigned short int sector=0;sector < maxLogicSector; ++sector)
	{
	    table << std::endl;
	    table << "#----------+--------+-----+--------+----------" << std::endl;
	    table << "#Subsystem | Sector | RoI | ROBnum | ROBids" << std::endl;
	    table << "#----------+--------+-----+--------+----------" << std::endl;
	    
	    for(unsigned short int roi=1;roi <= maxRoI; ++roi)
	    {
	        float PhiMean = (sector!=31)? ((sector+1)/2)*PhiStep : 0.;
	        float phiMin = PhiMean - PhiWidth;
	        float phiMax = PhiMean + PhiWidth;
		float etaMin = 0.;
		float etaMax = 0.;
	    
	        float etaMin_Low;
	        float etaMax_Low;
	        float etaMin_High;
	        float etaMax_High;
	      
                bool low  = etaDimLow(side,sector,roi,etaMin_Low,etaMax_Low);
		bool high = etaDimHigh(side,sector,roi,etaMin_High,etaMax_High);
		
		if(low || high)
		{
	            if(low && high)
		    {
		        etaMin=(etaMin_Low<=etaMin_High)? etaMin_Low :
			                                  etaMin_High;
			etaMax=(etaMax_Low>=etaMax_High)? etaMax_Low :
			                                  etaMax_High;  
		    } else if (low)
		    {
		        etaMin = etaMin_Low;
			etaMax = etaMax_Low;
		    } else if (high)
		    {
		        etaMin = etaMin_High;
			etaMax = etaMax_High;
		    }
		    
		    MDTGeometry::ROBid robs = m_mDTcablingSvc->getROBid
		                                  (phiMin,phiMax,etaMin,etaMax);
		       
		    int nrob = robs.size();
		    if(nrob)
		    {
		        table << "     " << std::setw(2) << side   << "    |"
			      << "   "   << std::setw(2) << sector << "   |"
			      << "  "    << std::setw(2) << roi    << " |"
			      << "   "   << std::setw(2) << nrob   << "   |";
			
			MDTGeometry::ROBid::const_iterator it = robs.begin();
			while (it != robs.end())
			{
			    table << " " << std::setw(3) << (*it)%48; 
			    ++it;
			}
			table << std::endl;
		    }
		    
		}
	    }
        }
    }
    
    table.close();
    
    /*
    // MC July 2014: add dump the ROI Eta-Phi Map 
    
    std::ofstream roi_map;                          //file storing the RoI/ROB map
    roi_map.open("ROI_Mapping.txt", std::ios::out ); //try to open the file
    if(!roi_map){
      msg << MSG::WARNING << "Unable to open ROI_Mapping file!"<< endreq;
    } else {                   
      maxRoI=10; // try very large value
      for(int side=0;side < maxSubsystem; side++){
	for(int sector=0;sector < maxLogicSector; sector++){
	  for (int roi=0; roi<maxRoI; roi++){
	    // Get the strips delimiting the RoIs from rPCcablingSvc
	    Identifier EtaLowBorder_id;
	    Identifier EtaHighBorder_id;
	    Identifier PhiLowBorder_id;
	    Identifier PhiHighBorder_id;
	    if(m_rPCcablingSvc->
	       give_RoI_borders_id(side,
				   sector,
				   roi,
				   EtaLowBorder_id,
				   EtaHighBorder_id,
				   PhiLowBorder_id,
				   PhiHighBorder_id)) {
	      Amg::Vector3D EtaLowBorder_pos(0.,0.,0.);
	      Amg::Vector3D EtaHighBorder_pos(0.,0.,0.);
	      Amg::Vector3D PhiLowBorder_pos(0.,0.,0.);
	      Amg::Vector3D PhiHighBorder_pos(0.,0.,0.);
    
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
	      
	      roi_map << side << " "
		      << sector << " "
		      << roi << " "
		      << EtaLowBorder_pos.eta()  << " "
		      << EtaHighBorder_pos.eta() << " "
		      << PhiLowBorder_pos.phi()  << " "
		      << PhiHighBorder_pos.phi() << endreq;
	    }//if
	  } 
	}
      }
      roi_map.close();
    }
    */
    return true;
}

bool  RPCRecRoiSvc::etaDimLow (unsigned short int side,
			       unsigned short int sector,
			       unsigned short int roi,
			       float& etaMin, float& etaMax)  const 
{
  // Get the strips delimiting the RoIs from rPCcablingSvc
  Identifier EtaLowBorder_id;
  Identifier EtaHighBorder_id;
  Identifier PhiLowBorder_id;
  Identifier PhiHighBorder_id;
  Amg::Vector3D EtaLowBorder_pos(0.,0.,0.);
  Amg::Vector3D EtaHighBorder_pos(0.,0.,0.);

  if( !m_rPCcablingSvc-> give_LowPt_borders_id(side,
					       sector,
					       roi,
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
    float tmp=etaMin;
    etaMin=etaMax;
    etaMax=tmp;
  }
  return true;
}

bool RPCRecRoiSvc::etaDimHigh (unsigned short int side,
			       unsigned short int sector,
			       unsigned short int roi,
			       float& etaMin, float& etaMax)  const 
{
  // Get the strips delimiting the RoIs from rPCcablingSvc
  Identifier EtaLowBorder_id;
  Identifier EtaHighBorder_id;
  Identifier PhiLowBorder_id;
  Identifier PhiHighBorder_id;
  Amg::Vector3D EtaLowBorder_pos(0.,0.,0.);
  Amg::Vector3D EtaHighBorder_pos(0.,0.,0.);

  if(!m_rPCcablingSvc->give_HighPt_borders_id(side,
					      sector,
					      roi,
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
    float tmp=etaMin;
    etaMin=etaMax;
    etaMax=tmp;
  }
  return true;
}

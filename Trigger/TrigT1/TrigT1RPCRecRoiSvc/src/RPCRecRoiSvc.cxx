/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <fstream>
#include <iomanip>

#include "TrigT1RPCRecRoiSvc/RPCRecRoiSvc.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "RPCgeometry/IRPCgeometrySvc.h"
#include "MDTcabling/IMDTcablingSvc.h"

using namespace LVL1RPC;

StatusCode RPCRecRoiSvc::initialize (void) 
{ 
  MsgStream log( messageService(), name() );

  m_rPCgeometrySvc = 0;
  StatusCode sc = service("RPCgeometrySvc", m_rPCgeometrySvc,1);
  if(sc.isFailure())
    {
      log << MSG::WARNING
	  << "Unable to retrieve the RPC geometry Service"
	  << endreq;
      return StatusCode::FAILURE;
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

  /** set the eta and phi values derived from the RPC cabling service */
  unsigned int EtaLowBorder;
  unsigned int EtaHighBorder;
  unsigned int PhiLowBorder;
  unsigned int PhiHighBorder;
  
  if(m_rPCcablingSvc->
     give_RoI_borders(subSysID,
		      sectorID,
		      roiNumber,
		      EtaLowBorder,
		      EtaHighBorder,
		      PhiLowBorder,
		      PhiHighBorder))
    {
      if(m_rPCgeometrySvc)
	{
	  float firstEta[3] = {0.,0.,0.};
	  float lastEta[3]  = {0.,0.,0.};
	  float firstPhi[3] = {0.,0.,0.};
	  float lastPhi[3]  = {0.,0.,0.};
	  
	  float EtaRad = 0.;
	  float PhiRad = 0.;
	  
	  if(!m_rPCgeometrySvc->
	     give_strip_coordinates(EtaLowBorder,firstEta))
	    {
	      return;
	    }
	  
	  if(!m_rPCgeometrySvc->
	     give_strip_coordinates(EtaHighBorder,lastEta))
	    {
	      return;
	    }
	  
	  if(!m_rPCgeometrySvc->
	     give_strip_coordinates(PhiLowBorder,firstPhi))
	    {
	      return;
	    }
	  
	  if(!m_rPCgeometrySvc->
	     give_strip_coordinates(PhiHighBorder,lastPhi))
	    {
	      return;
	    }
	  
	  if(!m_rPCgeometrySvc->
	     give_strip_radius(EtaLowBorder,EtaRad))
	    {
	      return;
	    }
	  
	  if(!m_rPCgeometrySvc->
	     give_strip_radius(PhiHighBorder,PhiRad))
	    {
	      return;
	    }
	  
	  float DeltaZ = (lastEta[2] - firstEta[2])/2.;
	  float Zmid = firstEta[2] + DeltaZ;
	  
	  float Theta = (Zmid)? atan(EtaRad/fabsf(Zmid)): asin(1);
	  m_eta = (Zmid>0.)?  -log(tan(Theta/2.)) : log(tan(Theta/2.));
	  
	  
	  m_phi = ( atan2(firstPhi[1],firstPhi[0]) +
		    atan2(lastPhi[1],lastPhi[0]) )/2.;
	  if(m_phi < 0.) m_phi += 2*3.141593;
	  
	  // use the new coordinate system of Atlas
	  if(m_phi > 3.141593) m_phi -= 2*3.141593;
	}
    }
}

bool
RPCRecRoiSvc::writeRoiRobMap(const std::string& filename)
{  
    MsgStream msg( messageService(), name() );    //get MSG service
    
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
    
    
    const int maxSubsystem = 2;
    const int maxLogicSector = 32;
    int       maxRoI = 0;
    
    
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
    
    return true;
}

bool
RPCRecRoiSvc::etaDimLow(unsigned short int side,unsigned short int sector,
                        unsigned short int roi,
			float& etaMin, float& etaMax) const
{
    unsigned int EtaLowBorder;
    unsigned int EtaHighBorder;
    unsigned int PhiLowBorder;
    unsigned int PhiHighBorder;

    if(!m_rPCcablingSvc->give_LowPt_borders(side,sector,roi,EtaLowBorder,
                        EtaHighBorder,PhiLowBorder,PhiHighBorder)) return false;

    float firstEta[3] = {0.,0.,0.};
    float lastEta[3]  = {0.,0.,0.};
	  
    float EtaRad = 0.;
	
    if(!m_rPCgeometrySvc->give_strip_coordinates(EtaLowBorder,firstEta))
	                                                       return false;
	  
    if(!m_rPCgeometrySvc->give_strip_coordinates(EtaHighBorder,lastEta))
	                                                       return false;
							       
    if(!m_rPCgeometrySvc->give_strip_radius(EtaLowBorder,EtaRad))
	                                                       return false;
    float Zt = firstEta[2];
	  
    float Theta = (Zt)? atan(EtaRad/fabsf(Zt)): asin(1);
    etaMin = (Zt>0.)?  -log(tan(Theta/2.)) : log(tan(Theta/2.));

    Zt = lastEta[2];
	  
    Theta = (Zt)? atan(EtaRad/fabsf(Zt)): asin(1);
    etaMax = (Zt>0.)?  -log(tan(Theta/2.)) : log(tan(Theta/2.));
    
    if (etaMin > etaMax)
    {
        float tmp = etaMax;
	etaMax = etaMin;
	etaMin = tmp;
    }
    
    return true;
}


bool
RPCRecRoiSvc::etaDimHigh(unsigned short int side, unsigned short int sector,
                         unsigned short int roi,
			 float& etaMin, float& etaMax) const
{
    unsigned int EtaLowBorder;
    unsigned int EtaHighBorder;
    unsigned int PhiLowBorder;
    unsigned int PhiHighBorder;

    if(!m_rPCcablingSvc->give_HighPt_borders(side,sector,roi,EtaLowBorder,
                        EtaHighBorder,PhiLowBorder,PhiHighBorder)) return false;

    float firstEta[3] = {0.,0.,0.};
    float lastEta[3]  = {0.,0.,0.};
	  
    float EtaRad = 0.;
	
    if(!m_rPCgeometrySvc->give_strip_coordinates(EtaLowBorder,firstEta))
	                                                       return false;
	  
    if(!m_rPCgeometrySvc->give_strip_coordinates(EtaHighBorder,lastEta))
	                                                       return false;
							       
    if(!m_rPCgeometrySvc->give_strip_radius(EtaLowBorder,EtaRad))
	                                                       return false;
    float Zt = firstEta[2];
	  
    float Theta = (Zt)? atan(EtaRad/fabsf(Zt)): asin(1);
    etaMin = (Zt>0.)?  -log(tan(Theta/2.)) : log(tan(Theta/2.));

    Zt = lastEta[2];
	  
    Theta = (Zt)? atan(EtaRad/fabsf(Zt)): asin(1);
    etaMax = (Zt>0.)?  -log(tan(Theta/2.)) : log(tan(Theta/2.));
    
    if (etaMin > etaMax)
    {
        float tmp = etaMax;
	etaMax = etaMin;
	etaMin = tmp;
    }
     
    return true;
}

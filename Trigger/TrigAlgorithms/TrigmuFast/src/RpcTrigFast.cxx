/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/RpcTrigFast.h"
#include "TrigmuFast/Datatypes.h"

#include "RPCgeometry/RPCGeometry.h"
#include "RPCcablingInterface/CablingRPCBase.h"
#include "MDTcabling/MDTGeometry.h"

#include "TrigT1RPClogic/RPCrawData.h"
#include "TrigT1RPClogic/CMAdata.h"

#include "MuonCablingTools/ShowRequest.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"

/////////////////////////////////////////////////////////////////////////////////
//<S>//////////////////////// TOF measurement ///////////////////////////////////
/*<S>*///////////////////////////////////////////////////////////////////////////
/*<S>*/
/*<S>*/
/*<S>*/const float SPEEDOFLIGHT = 0.29979; //[m/ns]
/*<S>*/const float INVSIGVEL = 4.8; //[ns/m]
/*<S>*/
/*<S>*///////////////////////////////////////////////////////////////////////////
//<S>//////////////////////// TOF measurement ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

using namespace std;

const string RpcTrigFast::s_name = "Level-1 fast emulation (A. Di Mattia algo.)";

RpcTrigFast::RpcTrigFast(MuMessageInterface& msg,
                         const RpcPad* rpc_pad,TriggerData& out,
			 RpcTrigPatt& patterns,
			 RPCrawData& rpc_digits,CMAdata& cma_patterns,
		         const RPCGeometry* rpcgeo,const CablingRPCBase* rpccab,
			 const MDTGeometry* mdtgeo,const bool maskUncCMAch,
			 const bool trigOnPhiOnly,const bool useRpcTrigPatt,
			 const float RpcTimeDelay)
    : RpcTrigStd(msg,rpc_pad,rpc_digits,cma_patterns,out,patterns,rpcgeo,
                 rpccab,mdtgeo,maskUncCMAch,trigOnPhiOnly,useRpcTrigPatt,
		 RpcTimeDelay)
{
}

RpcTrigFast::~RpcTrigFast(void)
{
}

bool
RpcTrigFast::run(MuonFeatureDetails* det)
{
    // initialization of the trigger data
    memset(&m_rpc_data, 0, sizeof(m_rpc_data));
    memset(&m_rpc_patterns, 0, sizeof(m_rpc_patterns));
    
    m_rpc_data.isEndcap = false;
    m_rpc_data.MDT_Sector_for_trigger = 99;
    m_rpc_data.MDT_Sector_for_overlap = 99;
	
    // initialization of the onlineId var
    uint32_t pad_subSys      = 0x0;
    uint32_t pad_sector      = 0x0;
    uint32_t pad_id          = 0x0;
    //uint32_t pad_hit_bcid    = 0xf;
    //uint32_t pad_hit_time    = 0xf;
    uint32_t pad_hit_cmaid   = 0x0;
    uint32_t pad_hit_ijk     = 0x0;
    uint32_t pad_hit_channel = 0x0;


    // get the Sector Logic and the PAD Id
    if(m_rpc_pad==0) return false;
    
    pad_sector = m_rpc_pad->sector();
    pad_subSys = (pad_sector<32)? 0x0 : 0x1;;
    pad_id     = m_rpc_pad->onlineId();
    
    
    unsigned long int sector = (long) m_rpc_pad->sector();
    
    unsigned short int SubsystemId = (sector < 32)? 0 : 1;
    unsigned short int SectorId    = sector%32;
    unsigned short int PADId       = m_rpc_pad->onlineId();

    m_rpc_data.PhysicsSector = ((SectorId + 1)/4 )%8 + 1;
    m_rpc_data.LargeSmall    = ((SectorId + 1)/2 )%2;
    
    //std::cout << "SubsystemId=" << SubsystemId << ",  SectorId=" << SectorId
    //     << ",  PADid=" << PADId << std::endl;
    
    // loop over CMAs into the PAD to load data into Level-1 logic
    RpcPad::const_iterator it3 = m_rpc_pad->begin(); 
    RpcPad::const_iterator it4 = m_rpc_pad->end();
    for (; it3!=it4; ++it3) 
    {
        const RpcCoinMatrix * cma = (*it3);

        unsigned long int cmaId = (long) cma->onlineId();
       
        unsigned short int CMAId = cmaId & 3;
        unsigned short int lh    = (cmaId >> 2) & 1;

        const CMAparameters* CMA = 
	                  m_rpccab->give_CMA(SubsystemId,SectorId,PADId,CMAId);
        
	
	pad_hit_cmaid  = cma->onlineId();
        
	
	//cout << "SubsystemId = " << SubsystemId
        //     << ", SectorId = " << SectorId
        //     << ", PADId = " << PADId
	//     << ", CMAId = " << CMAId
	//     << ", lh = " << lh << endl;
	//cout << ShowRequest<CMAparameters>(*CMA,false) << endl;
       
        std::list <unsigned int> CodeList;

       
        //for each cma for over fired channels
        RpcCoinMatrix::const_iterator it5 = (*it3)->begin(); 
        RpcCoinMatrix::const_iterator it6 = (*it3)->end();
        for (; it5!=it6; ++it5) 
        {
            CodeList.clear();
	    const RpcFiredChannel * fChannel = (*it5);

            unsigned short int ijk     = fChannel->ijk();
            unsigned short int channel = fChannel->channel();
	    float time                 = (fChannel->bcid()-2)*25 +
	                                  fChannel->time()*3.125 +
                                          m_RpcTimeDelay;
					  //- 45;// shift added to compensate the 
					       // timing calibration offset
	    //std::cout << "m_RpcTimeDelay=" << m_RpcTimeDelay << std::endl;
	    
	    pad_hit_ijk     = fChannel->ijk() & 0x7;
            pad_hit_channel = fChannel->channel() & 0x1f;
	    
	    uint32_t pad_onlineId = pad_hit_channel      |
	                           (pad_hit_ijk << 5)   |
				   (pad_hit_cmaid << 8) |
		 		   (pad_id << 11)       |
				   (pad_subSys << 15)   |
				   (pad_sector << 16);
	    
	    //cout << "sector = " << sector
	    //     << ", lh = " << lh << ", ijk = " << ijk
	    //     << ", channel = " << channel << endl;
	    
	    // load Low-pt data into Level-1 logic
	    
	    
	    bool trigPatt = (m_useRpcTrigPatt)? true : (ijk != 6);
	    
	    if( ijk==7 && det!=0) {
	        det->setExtension0(fChannel->thr());
	    }
	    
	    if(lh == 0 && ijk != 7 && CMA !=0 )
	    {
	        CMA->give_strip_code(sector,lh,ijk,channel,CodeList);
		
		//std::list<unsigned int>::const_iterator it = CodeList.begin();
		//while (it != CodeList.end())
		//{
		//    cout << "code = " << *it << endl; 
		//    ++it;
		//}
                //RPCrawDigit Digit(0,CodeList,time,m_rpcgeo);
		//cout << Digit << endl;
		
		if(det!=0) {
		  uint32_t pad_hit_code = 0xffffffff;
		  if(CodeList.size()!=0) {
	            std::list <unsigned int>::const_iterator it7 = 
		                                       CodeList.begin();
                    pad_hit_code = *it7;
		  }
                  float x = 0.;
		  float y = 0.;
		  float z = 0.;
		  float r = 0.;
		  float p = 0.;
		  fillGeoDataForESD(pad_hit_code,x,y,z,r,p);
		  	
		  det->setPadHit(pad_onlineId,pad_hit_code,x,y,z,r,p);
		  det->setExtension1(fChannel->bcid());
		  det->setExtension2(fChannel->time());
		}
		
		if( (CodeList.size() || m_maskUncCMAch) && trigPatt) {
		    RPCrawDigit Digit(0,CodeList,time,m_rpcgeo);
		    const RPCdigit* digit = m_rpc_digits << Digit;
		    m_cma_patterns.create_patterns(CMA,digit);
		}
            }
	    
	    // load High-pt data into Level-1 logic
	    if(lh == 1 && (ijk == 2 || ijk == 3 || ijk == 4 || ijk == 5) &&
	       CMA!=0)
	    {
	        CMA->give_strip_code(sector,lh,ijk,channel,CodeList);
		
		//std::list<unsigned int>::const_iterator it = CodeList.begin();
		
		//while (it != CodeList.end())
		//{
		//    cout << "code = " << *it << endl; 
		//    ++it;
		//}
		
		//RPCrawDigit Digit(0,CodeList,time,m_rpcgeo);
		//cout << Digit << endl;
		
		if(det!=0) {
		  uint32_t pad_hit_code = 0xffffffff;
		  if(CodeList.size()!=0) {
	            std::list <unsigned int>::const_iterator it7 = 
		                                       CodeList.begin();
                    pad_hit_code = *it7;
		  }
                  float x = 0.;
		  float y = 0.;
		  float z = 0.;
		  float r = 0.;
		  float p = 0.;
		  fillGeoDataForESD(pad_hit_code,x,y,z,r,p);
		  	
		  det->setPadHit(pad_onlineId,pad_hit_code,x,y,z,r,p);
		}
		
		if(CodeList.size() || m_maskUncCMAch) {
		    RPCrawDigit Digit(0,CodeList,time,m_rpcgeo);
		    const RPCdigit* digit = m_rpc_digits << Digit;
		    m_cma_patterns.create_patterns(CMA,digit);
                }
	    }	    
        }
    }
        
    // get the trigger from the CMA
    CMAdata::PATTERNSlist Eta = m_cma_patterns.eta_cma_patterns();
    CMAdata::PATTERNSlist::const_iterator EtaPat = Eta.begin();
    CMAtrigger eta(LogicEmulation,&(*EtaPat));
    while(EtaPat != Eta.end())
    {
	eta += (*EtaPat).give_trigger(LogicEmulation);
	++EtaPat;
    }
	
    CMAdata::PATTERNSlist Phi = m_cma_patterns.phi_cma_patterns();
    CMAdata::PATTERNSlist::const_iterator PhiPat = Phi.begin();
    CMAtrigger phi(LogicEmulation,&(*PhiPat));
    while(PhiPat != Phi.end())
    {
        //(*PhiPat).Print(cout,false);
	//cout << "trigger trovato da CMApattern:" << endl;
	//(*PhiPat).give_trigger(LogicEmulation).Print(cout,true);
	phi += (*PhiPat).give_trigger(LogicEmulation);
	//cout << "trigger += :" << endl;
	//phi.Print(cout,true);
	++PhiPat;
    }
    

    int etaTrigType = eta.trigger().type();
    int phiTrigType = phi.trigger().type();  // <========
    //cout << endl << endl;
    
    uint32_t dummy = 0x0;
    
    eta.patterns()->pivot0().dump(m_rpc_patterns.eta_pivot_lay0,dummy);
    eta.patterns()->pivot1().dump(m_rpc_patterns.eta_pivot_lay1,dummy);
    eta.patterns()->lowPt0().dump(m_rpc_patterns.eta_low_0_lay0,
                                  m_rpc_patterns.eta_low_1_lay0);
    eta.patterns()->lowPt1().dump(m_rpc_patterns.eta_low_0_lay1,
                                  m_rpc_patterns.eta_low_1_lay1);
    eta.patterns()->highPt0().dump(m_rpc_patterns.eta_high_0_lay0,
                                   m_rpc_patterns.eta_high_1_lay0);
    eta.patterns()->highPt1().dump(m_rpc_patterns.eta_high_0_lay1,
                                   m_rpc_patterns.eta_high_1_lay1);
    phi.patterns()->pivot0().dump(m_rpc_patterns.phi_pivot_lay0,dummy);
    phi.patterns()->pivot1().dump(m_rpc_patterns.phi_pivot_lay1,dummy);
    phi.patterns()->lowPt0().dump(m_rpc_patterns.phi_low_0_lay0,
                                  m_rpc_patterns.phi_low_1_lay0);
    phi.patterns()->lowPt1().dump(m_rpc_patterns.phi_low_0_lay1,
                                  m_rpc_patterns.phi_low_1_lay1);
    phi.patterns()->highPt0().dump(m_rpc_patterns.phi_high_0_lay0,
                                   m_rpc_patterns.phi_high_1_lay0);
    phi.patterns()->highPt1().dump(m_rpc_patterns.phi_high_0_lay1,
                                   m_rpc_patterns.phi_high_1_lay1);
    
   if(det!=0) {
        det->setEtaPivot(m_rpc_patterns.eta_pivot_lay0, m_rpc_patterns.eta_pivot_lay1);
        det->setEtaLow (m_rpc_patterns.eta_low_0_lay0, m_rpc_patterns.eta_low_1_lay0, 
	                m_rpc_patterns.eta_low_0_lay1, m_rpc_patterns.eta_low_1_lay1);
        det->setEtaHigh(m_rpc_patterns.eta_high_0_lay0, m_rpc_patterns.eta_high_1_lay0, 
	                m_rpc_patterns.eta_high_0_lay1, m_rpc_patterns.eta_high_1_lay1);
        det->setPhiPivot(m_rpc_patterns.phi_pivot_lay0, m_rpc_patterns.phi_pivot_lay1);
        det->setPhiLow (m_rpc_patterns.phi_low_0_lay0, m_rpc_patterns.phi_low_1_lay0,
	                m_rpc_patterns.phi_low_0_lay1, m_rpc_patterns.phi_low_1_lay1);
        det->setPhiHigh(m_rpc_patterns.phi_high_0_lay0, m_rpc_patterns.phi_high_1_lay0,
	                m_rpc_patterns.phi_high_0_lay1, m_rpc_patterns.phi_high_1_lay1);
    }
    
    if( etaTrigType && phiTrigType)
    {
        //eta.Print(cout,true);
        //phi.Print(cout,true);
		
	int etaThreshold = eta.trigger().threshold();
	int phiThreshold = phi.trigger().threshold();
	
	if(etaTrigType == phiTrigType) 
	{
	    m_rpc_data.type = etaTrigType;
	    if(etaThreshold<=phiThreshold) m_rpc_data.threshold=etaThreshold;
	    else m_rpc_data.threshold = phiThreshold;
	}
        else if (etaTrigType < phiTrigType)
	{
	    m_rpc_data.type      = etaTrigType;
	    m_rpc_data.threshold = etaThreshold;
	}
	else
	{
	    m_rpc_data.type      = phiTrigType;
	    m_rpc_data.threshold = phiThreshold;
	}
  	
        RPCdigit eta1(*eta.lowPt_hit());
        RPCdigit eta2(*eta.pivot_hit());
	
	RPCdigit phi1(*phi.lowPt_hit());
	RPCdigit phi2(*phi.pivot_hit());

/*<S>*//////////////////////////////////////////////////////////////////////////////////
/*<S>*//////////////////////// TOF measurement /////////////////////////////////////////
/*<S>*//////////////////////////////////////////////////////////////////////////////////
/*<S>*/float f_phi1_beta = stationBeta(phi1, eta1, SubsystemId, SectorId); //beta from the first station
/*<S>*/float f_phi2_beta = stationBeta(phi2, eta2, SubsystemId, SectorId); //beta from the first station
/*<S>*//////////////////////////////////////////////////////////////////////////////////
/*<S>*//////////////////////// TOF measurement /////////////////////////////////////////
/*<S>*//////////////////////////////////////////////////////////////////////////////////

	if(!phi1.set_to_chamber(eta1.decoding().rpc_z_index())) return false;
	if(!phi2.set_to_chamber(eta2.decoding().rpc_z_index())) return false;

	m_rpc_data.rpc1[0] = phi1.x();
	m_rpc_data.rpc1[1] = phi1.y();
	m_rpc_data.rpc1[2] = eta1.z();
	eta1.station_radius(m_rpc_data.rpc1[3]);

	m_rpc_data.rpc2[0] = phi2.x();
	m_rpc_data.rpc2[1] = phi2.y();
	m_rpc_data.rpc2[2] = eta2.z();
	eta2.station_radius(m_rpc_data.rpc2[3]);

	/*<S>*///float f_eta3_beta = -1.0;
	/*<S>*/float f_phi3_beta = -1.0;
	
	if(m_rpc_data.type == 2)
	{
            RPCdigit eta3(*eta.highPt_hit());
	    RPCdigit phi3(*phi.highPt_hit());

	/*<S>*//////////////////////////////////////////////////////////////////////////////////
	/*<S>*//////////////////////// TOF measurement /////////////////////////////////////////
	/*<S>*//////////////////////////////////////////////////////////////////////////////////
	/*<S>*/f_phi3_beta = stationBeta(phi3, eta3, SubsystemId, SectorId); //beta from the first station
	/*<S>*//////////////////////////////////////////////////////////////////////////////////
	/*<S>*//////////////////////// TOF measurement /////////////////////////////////////////
	/*<S>*//////////////////////////////////////////////////////////////////////////////////

	

	    if(!phi3.set_to_chamber(eta3.decoding().rpc_z_index())) return false;

	    m_rpc_data.rpc3[0] = phi3.x();
	    m_rpc_data.rpc3[1] = phi3.y();
	    m_rpc_data.rpc3[2] = eta3.z();
	    eta3.station_radius(m_rpc_data.rpc3[3]);
	}
	
	/*<S>*/float f_beta=-1;
	/*<S>*/if(f_phi3_beta!=-1)
	/*<S>*/{
	/*<S>*/	f_beta = (f_phi1_beta + f_phi2_beta + f_phi3_beta )/3;
	/*<S>*/}else
	/*<S>*/{
	/*<S>*/	f_beta = (f_phi1_beta + f_phi2_beta )/2;
	/*<S>*/}
	
	m_rpc_data.beta = f_beta;
	if (det!=0) det->setSpeed_over_c(m_rpc_data.beta);
	
	
    } else if ( m_trigOnPhiOnly && phiTrigType)
    {
        int phiThreshold = phi.trigger().threshold();
	
	m_rpc_data.type      = phiTrigType;
	m_rpc_data.threshold = phiThreshold;
  		
	RPCdigit phi1(*phi.lowPt_hit());
	RPCdigit phi2(*phi.pivot_hit());
	
	m_rpc_data.rpc1[0] = phi1.x();
	m_rpc_data.rpc1[1] = phi1.y();
	m_rpc_data.rpc1[2] = phi1.z();
	phi1.station_radius(m_rpc_data.rpc1[3]);

	m_rpc_data.rpc2[0] = phi2.x();
	m_rpc_data.rpc2[1] = phi2.y();
	m_rpc_data.rpc2[2] = phi2.z();
	phi2.station_radius(m_rpc_data.rpc2[3]);
	if(m_rpc_data.type == 2)
	{
	    RPCdigit phi3(*phi.highPt_hit());
            
	    m_rpc_data.rpc3[0] = phi3.x();
	    m_rpc_data.rpc3[1] = phi3.y();
	    m_rpc_data.rpc3[2] = phi3.z();
	    phi3.station_radius(m_rpc_data.rpc3[3]);
	}

    } else return false;

    // compute the MDT sector for trigger and overlap
    float phiMiddle = atan2(m_rpc_data.rpc2[1],m_rpc_data.rpc2[0]);
    int barrel_tr, barrel_ov;
    m_mdtgeo->getBsects(1,phiMiddle,barrel_tr, barrel_ov);
    int MDT_tr = (m_rpc_data.PhysicsSector - 1)*2 + m_rpc_data.LargeSmall;
    
    if(MDT_tr == barrel_tr)
    {
        m_rpc_data.MDT_Sector_for_trigger = barrel_tr;
	m_rpc_data.MDT_Sector_for_overlap = barrel_ov;
    }
    else if (MDT_tr == barrel_ov)
    {
        m_rpc_data.MDT_Sector_for_trigger = barrel_ov;
	m_rpc_data.MDT_Sector_for_overlap = barrel_tr;
    }
    
    m_rpc_data.side = (m_rpc_data.rpc2[2]<0.)? 0 : 1;
    
    return true;
}
  

float RpcTrigFast::stationBeta(RPCdigit& phi, RPCdigit& eta, unsigned short int
    SubsystemId, unsigned short int SectorId) const
{
/*<S>*/
/*<S>*///float f_timeShift = 5.; //this number should be read from a database
   // comment out to remove compilation warning
   // float f_timeShift = m_RpcTimeDelay;
/*<S>*/
/*<S>*///===================================================================
/*<S>*///========first station==============================================
/*<S>*///===================================================================
/*<S>*/
/*<S>*/// Station related parameters
/*<S>*/int i_phi_RPCChamber = eta.decoding().rpc_z_index();
/*<S>*/int i_phi_RPCStation = phi.decoding().lvl1_station();
/*<S>*/int i_phi_RPCLogicSector = phi.decoding().logic_sector();
/*<S>*/int i_eta_RPCChamber = eta.decoding().rpc_z_index();
/*<S>*/int i_eta_RPCStation = eta.decoding().lvl1_station();
/*<S>*/int i_eta_RPCLogicSector = eta.decoding().logic_sector();
/*<S>*/
/*<S>*/int i_phi_doubletZ = m_rpccab->give_doubletZ(SubsystemId,SectorId,i_phi_RPCStation,i_phi_RPCChamber);
/*<S>*/int i_eta_doubletPhi = m_rpccab->give_doubletPhi(i_eta_RPCLogicSector);
/*<S>*/
/*<S>*/// propagation length along the z direction
/*<S>*/float f_phi_zmin=0., f_phi_zmax=0.;
/*<S>*/m_rpcgeo->give_rpc_boundaries(i_phi_RPCLogicSector,i_phi_RPCStation,i_phi_RPCChamber,f_phi_zmin,f_phi_zmax);
/*<S>*/
/*<S>*/float f_phi_propLength=0.;
/*<S>*/if(eta.z()>=0)	f_phi_propLength = ((1==i_phi_doubletZ)?(eta.z()-f_phi_zmin):(f_phi_zmax-eta.z()));
/*<S>*/else	f_phi_propLength = ((1==i_phi_doubletZ)?((-1)*eta.z()-f_phi_zmin):(f_phi_zmax-(-1)*eta.z()));
/*<S>*/f_phi_propLength += (f_phi_zmax-f_phi_zmin); // it seems to be more aacurate this way
/*<S>*/
/*<S>*/// propagation length along the XY plane
/*<S>*/float f_eta_XYdim = 0.;
/*<S>*/m_rpcgeo->give_rpc_XYdim( i_eta_RPCLogicSector,i_eta_RPCStation,i_eta_RPCChamber,f_eta_XYdim);
/*<S>*/float f_eta_distFromMiddle = sqrt((phi.x()-eta.x())*(phi.x()-eta.x())+(phi.y()-eta.y())*(phi.y()-eta.y())); //[cm]
/*<S>*/
/*<S>*/float f_eta_propLength=0.;
/*<S>*/float f_eta_phi=0., f_center_phi=0.;
/*<S>*/phi.phi(f_eta_phi);
/*<S>*/eta.phi(f_center_phi);
/*<S>*/if(f_eta_phi>f_center_phi)
/*<S>*/{
/*<S>*/		if(2==i_eta_doubletPhi) f_eta_propLength = 0.5*f_eta_XYdim-f_eta_distFromMiddle;
/*<S>*/		else f_eta_propLength = f_eta_XYdim+f_eta_distFromMiddle;
/*<S>*/}else
/*<S>*/{
/*<S>*/		if(2==i_eta_doubletPhi) f_eta_propLength = 0.5*f_eta_XYdim+f_eta_distFromMiddle;
/*<S>*/		else f_eta_propLength = f_eta_XYdim-f_eta_distFromMiddle;
/*<S>*/}
/*<S>*///f_eta_propLength+=f_eta_XYdim;
/*<S>*/// TOF
/*<S>*/float f_dist = sqrt(phi.x()*phi.x()+phi.y()*phi.y()+eta.z()*eta.z());
/*<S>*/float f_muTOF = f_dist/100/SPEEDOFLIGHT;
// fix on 13.3.2011, so as not to subtract timeShift (as it is already added at line 130)
/*<S>*/float f_phi_TOF = f_muTOF + phi.time() - f_phi_propLength/100*INVSIGVEL + 3.125/2.; //[ns]
/*<S>*///float f_phi_TOF = f_muTOF + phi.time() - f_phi_propLength/100*INVSIGVEL - f_timeShift + 3.125/2.; //[ns]
//
/*<S>*///float f_eta_TOF = f_muTOF + eta.time() - f_eta_propLength/100*INVSIGVEL - f_timeShift + 3.125/2.; //[ns]
/*<S>*/
/*<S>*/// beta
/*<S>*/float f_phi_beta = f_dist/100/f_phi_TOF/SPEEDOFLIGHT;
/*<S>*///float f_eta_beta = f_dist/100/f_eta_TOF/SPEEDOFLIGHT;
/*<S>*/
/*<S>*/// Debug prints
/*<S>*///std::cout << "##########################################################" << std::endl;
/*<S>*///std::cout << "#" << std::endl;
/*<S>*///std::cout << "#      f_dist = " << f_dist << std::endl;
/*<S>*///std::cout << "# phi:" << std::endl;
/*<S>*///std::cout << "#      eta.z() = " << eta.z() <<std::endl;
/*<S>*///std::cout << "#      f_phi_zmin = " << f_phi_zmin << std::endl;
/*<S>*///std::cout << "#      f_phi_zmax = " << f_phi_zmax << std::endl;
/*<S>*///std::cout << "#      i_phi_doubletZ = " << i_phi_doubletZ << std::endl;
/*<S>*///std::cout << "#      phi.time() = " << phi.time() << std::endl;
/*<S>*///std::cout << "#      propagation time = " << f_phi_propLength/100*INVSIGVEL << std::endl;
/*<S>*///std::cout << "#      f_phi_TOF = " << f_phi_TOF << std::endl;
/*<S>*///std::cout << "#      f_phi_beta = " << f_phi_beta << std::endl;
/*<S>*///std::cout << "# eta:" << std::endl;
/*<S>*///std::cout << "#      f_eta_XYdim = " << f_eta_XYdim << std::endl;
/*<S>*///std::cout << "#      f_eta_phi = " << f_eta_phi << std::endl;
/*<S>*///std::cout << "#      f_center_phi = " << f_center_phi << std::endl;
/*<S>*///std::cout << "#      f_eta_distFromMiddle = " << f_eta_distFromMiddle << std::endl;
/*<S>*///std::cout << "#      i_eta_doubletPhi = " << i_eta_doubletPhi << std::endl;
/*<S>*///std::cout << "#      eta.time() = " << eta.time() << std::endl;
/*<S>*///std::cout << "#      propagation time = " << f_eta_propLength/100*INVSIGVEL << std::endl;
/*<S>*///std::cout << "#      f_eta_TOF = " << f_eta_TOF << std::endl;
/*<S>*///std::cout << "#      f_eta_beta = " << f_eta_beta << std::endl;
/*<S>*///std::cout << "#" << std::endl;
/*<S>*///std::cout << "##########################################################" << std::endl;

//float avgBeta = (f_phi_beta+f_eta_beta)/2.;
//std::cout << "shikma::?? " << f_phi_beta << " " << f_eta_beta << " " << avgBeta << std::endl;
return f_phi_beta;
}

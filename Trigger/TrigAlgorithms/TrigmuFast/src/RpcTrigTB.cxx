/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/RpcTrigTB.h"
#include "TrigmuFast/Datatypes.h"

#include "RPCgeometry/RPCGeometry.h"
#include "RPCcablingInterface/CablingRPCBase.h"
#include "MDTcabling/MDTGeometry.h"

#include "TrigT1RPClogic/RPCrawData.h"
#include "TrigT1RPClogic/CMAdata.h"

#include "MuonCablingTools/ShowRequest.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"

using namespace std;

const string RpcTrigTB::s_name = "Level-1 fast emulation (A. Di Mattia algo.)";

RpcTrigTB::RpcTrigTB(MuMessageInterface& msg,
                     const RpcPad* rpc_pad,TriggerData& out,
		     RpcTrigPatt& patterns,
		     RPCrawData& rpc_digits,CMAdata& cma_patterns,
		     const RPCGeometry* rpcgeo,const CablingRPCBase* rpccab,
		     const MDTGeometry* mdtgeo,const bool maskUncCMAch,
		     const bool trigOnPhiOnly,const bool useRpcTrigPatt,
		     const float RpcTrigFast)
    : RpcTrigStd(msg,rpc_pad,rpc_digits,cma_patterns,out,patterns,rpcgeo,rpccab,mdtgeo,
                 maskUncCMAch,trigOnPhiOnly,useRpcTrigPatt,RpcTrigFast)
{
}

RpcTrigTB::~RpcTrigTB(void)
{
}

bool
RpcTrigTB::run(MuonFeatureDetails*)
{
    // initialization of the trigger data
    m_rpc_data.type       = 0;
    m_rpc_data.threshold  = 0;
    m_rpc_data.triggerNum = 0;
    for(int i=0;i<4;++i)
    {
        m_rpc_data.rpc1[i] = 0;
        m_rpc_data.rpc2[i] = 0;
        m_rpc_data.rpc3[i] = 0;
    }
    
    memset(&m_rpc_patterns, 0, sizeof(RpcTrigPatt));
    
    //cout << "Sono entrato in TrigTB" << endl;
    // get the Sector Logic and the PAD Id
    unsigned long int sector = (long) m_rpc_pad->sector();
    
    unsigned short int SubsystemId = (sector < 32)? 0 : 1;
    unsigned short int SectorId    = sector%32;
    unsigned short int PADId       = m_rpc_pad->onlineId();

    m_rpc_data.PhysicsSector = ((SectorId + 1)/4 )%8 + 1;
    m_rpc_data.LargeSmall    = ((SectorId + 1)/2 )%2;
    
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
       
        //cout << "SubsystemId = " << SubsystemId
        //     << ", SectorId = " << SectorId
        //     << ", PADId = " << PADId
	//     << ", CMAId = " << CMAId
	//     << ", lh = " << lh << endl;
	//cout << ShowRequest<CMAparameters>(*CMA,true) << endl;
       
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
	    float time                 = fChannel->time();
	    
	    //cout << "ijk = " << ijk
	    //     << ", channel = " << channel << endl;
	    
	    // load Low-pt data into Level-1 logic
	    if(lh == 0 && ijk != 6 && ijk != 7)
	    {
	        CMA->give_strip_code(sector,lh,ijk,channel,CodeList);
		//std::list<unsigned int>::const_iterator it = CodeList.begin();
		//while (it != CodeList.end())
		//{
		//    cout << "code = " << *it << endl; 
		//    ++it;
		//}
                RPCrawDigit Digit(0,CodeList,time,m_rpcgeo);
		//cout << Digit << endl;
		
		const RPCdigit* digit = m_rpc_digits << Digit;
		m_cma_patterns.create_patterns(CMA,digit);
            }
	    
	    // load High-pt data into Level-1 logic
	    if(lh == 1 && (ijk == 2 || ijk == 3 || ijk == 4 || ijk == 5) )
	    {
	        CMA->give_strip_code(sector,lh,ijk,channel,CodeList);
		//std::list<unsigned int>::const_iterator it = CodeList.begin();
		
		//while (it != CodeList.end())
		//{
		//    cout << "code = " << *it << endl; 
		//    ++it;
		//}
		
		RPCrawDigit Digit(0,CodeList,time,m_rpcgeo);
		const RPCdigit* digit = m_rpc_digits << Digit;
		m_cma_patterns.create_patterns(CMA,digit);
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
	//cout << "Ho il Trigger in Eta" << endl;
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
    //int phiTrigType = High;
    
    //cout << endl << endl;
    
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
	
	if(!phi1.set_to_chamber(eta1.decoding().rpc_z_index())) 
	{
	    cout << "Phi1" <<endl;
	    return false;
	}
		
	if(!phi2.set_to_chamber(eta2.decoding().rpc_z_index())) 
	{
	    cout << "Phi2" <<endl;
	    return false;
	}
	
	m_rpc_data.rpc1[0] = eta1.x();
	m_rpc_data.rpc1[1] = phi1.y();
	m_rpc_data.rpc1[2] = eta1.z();
	eta1.station_radius(m_rpc_data.rpc1[3]);
	
	m_rpc_data.rpc2[0] = eta2.x();
	m_rpc_data.rpc2[1] = phi2.y();
	m_rpc_data.rpc2[2] = eta2.z();
	eta2.station_radius(m_rpc_data.rpc2[3]);
		
	
	if(m_rpc_data.type == 2)
	{
            RPCdigit eta3(*eta.highPt_hit());
	    RPCdigit phi3(*phi.highPt_hit());
	    if(!phi3.set_to_chamber(eta3.decoding().rpc_z_index())) 
	    {
	        cout << "Phi3" <<endl;
	        return false;
	    }
	    
	    m_rpc_data.rpc3[0] = eta3.x();
	    m_rpc_data.rpc3[1] = phi3.y();
	    m_rpc_data.rpc3[2] = eta3.z();
	    eta3.station_radius(m_rpc_data.rpc3[3]);
	}
    }
    else if( etaTrigType)
    {
        //eta.Print(cout,true);
        //phi.Print(cout,true);
	
	int etaThreshold = eta.trigger().threshold();
        m_rpc_data.type      = etaTrigType;
        m_rpc_data.threshold = etaThreshold;	
	
  	
        RPCdigit eta1(*eta.lowPt_hit());
        RPCdigit eta2(*eta.pivot_hit());
	
	
	m_rpc_data.rpc1[0] = eta1.x();
	m_rpc_data.rpc1[1] = eta1.y();
	m_rpc_data.rpc1[2] = eta1.z();
	eta1.station_radius(m_rpc_data.rpc1[3]);
	
	m_rpc_data.rpc2[0] = eta2.x();
	m_rpc_data.rpc2[1] = eta2.y();
	m_rpc_data.rpc2[2] = eta2.z();
	eta2.station_radius(m_rpc_data.rpc2[3]);
	
	
	// set as low Pt to eliminate problems due to noisy RPc channel in
	// high-Pt confirm plane
	if(m_rpc_data.type == 2) m_rpc_data.type = 1;	
	/*
	if(m_rpc_data.type == 2)
	{
            RPCdigit eta3(*eta.highPt_hit());
	    
	    m_rpc_data.rpc3[0] = eta3.x();
	    m_rpc_data.rpc3[1] = eta3.y();
	    m_rpc_data.rpc3[2] = eta3.z();
	    eta3.station_radius(m_rpc_data.rpc3[3]);
	}
	*/
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
  

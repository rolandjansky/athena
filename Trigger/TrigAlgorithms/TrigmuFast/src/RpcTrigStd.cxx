/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/ShowData.h"

#include "TrigmuFast/RpcTrigStd.h"
#include "TrigmuFast/MuData.h"
#include "TrigmuFast/Datatypes.h"

#include "RPCgeometry/RPCGeometry.h"
#include "RPCcablingInterface/CablingRPCBase.h"
#include "MDTcabling/MDTGeometry.h"

#include "TrigT1RPClogic/RPCrawData.h"
#include "TrigT1RPClogic/CMAdata.h"

#include <iomanip>

using namespace std;

const string RpcTrigStd::s_type = "RPC Trigger";

RpcTrigStd::RpcTrigStd(MuMessageInterface& msg,
                       const RpcPad* rpc_pad,RPCrawData& rpc_digits,
		       CMAdata& cma_patterns,TriggerData& out,
		       RpcTrigPatt& patterns,
		       const RPCGeometry* rpcgeo,const CablingRPCBase* rpccab,
		       const MDTGeometry* mdtgeo,const bool maskUncCMAch,
		       const bool trigOnPhiOnly,const bool useRpcTrigPatt,
		       const float RpcTimeDelay)
    : m_msg(&msg),m_rpcgeo(rpcgeo),m_rpccab(rpccab),m_mdtgeo(mdtgeo),
      m_maskUncCMAch(maskUncCMAch),m_trigOnPhiOnly(trigOnPhiOnly),
      m_useRpcTrigPatt(useRpcTrigPatt),m_RpcTimeDelay(RpcTimeDelay),
      m_rpc_pad(rpc_pad),m_rpc_digits(rpc_digits),
      m_cma_patterns(cma_patterns),m_rpc_data(out),m_rpc_patterns(patterns)
{
}

RpcTrigStd::~RpcTrigStd(void)
{
}

void
RpcTrigStd::printout(void) const
{
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    __osstream display;
#else                                // put your gcc 2.95 specific code here
    char buffer[50000];
    for (int i=0;i<50000;++i) buffer[i] = '\0';
    __osstream display(buffer,50000);
#endif

    display << name() << " started!" << endl;

    
    display << "RPC data loaded from Raw Data:" << endl;
    display << ShowData<RPCrawData>(m_rpc_digits,"",false) << endl;
    
    
    display << "RPC digits into station 1 ==> "
            << m_rpc_digits.how_many(-1,-1,1,-1,-1,-1) << endl;

    display << "RPC digits into station 2 ==> "
            << m_rpc_digits.how_many(-1,-1,2,-1,-1,-1) << endl;

    display << "RPC digits into station 3 ==> "
            << m_rpc_digits.how_many(-1,-1,3,-1,-1,-1) << endl;


    display << "CMApatterns created from RPC digits:" << endl;
    display << ShowData<CMAdata>(m_cma_patterns,"",false) << endl;
    
    
    std::string test = m_msg->testString();
    
    if(m_rpc_data.type==1) display << test << "  Trigger n. " << m_rpc_data.triggerNum
				   << ", Low Pt with threshold " 
                                   << m_rpc_data.threshold << endl;
    else if(m_rpc_data.type==2) display << test << "  Trigger n. " 
                 << m_rpc_data.triggerNum 
		 << ", High Pt with threshold "  << m_rpc_data.threshold 
                 << endl;
    else display << test << "No RPC trigger found!" << endl;

    display << test << "RPC trigger hits:" << endl;

    display.setf(ios::fixed);

    display << test << "   First station:    X=" << setw(9) << setprecision(3)
	    << m_rpc_data.rpc1[0] << "  Y=" << setw(9) << setprecision(3)
	    << m_rpc_data.rpc1[1] << "  Z=" << setw(9) << setprecision(3)
            << m_rpc_data.rpc1[2] << "  Rstation=" 
            << setw(9) << setprecision(3) <<  m_rpc_data.rpc1[3] << endl;

    display << test << "   Second station:   X=" << setw(9) << setprecision(3)
	    << m_rpc_data.rpc2[0] << "  Y=" << setw(9) << setprecision(3)
	    << m_rpc_data.rpc2[1] << "  Z=" << setw(9) << setprecision(3)
            << m_rpc_data.rpc2[2] << "  Rstation=" 
            << setw(9) << setprecision(3) <<  m_rpc_data.rpc2[3] << endl;

    display << test << "   Third station:    X=" << setw(9) << setprecision(3)
	    << m_rpc_data.rpc3[0] << "  Y=" << setw(9) << setprecision(3)
	    << m_rpc_data.rpc3[1] << "  Z=" << setw(9) << setprecision(3)
            << m_rpc_data.rpc3[2] << "  Rstation=" 
            << setw(9) << setprecision(3) <<  m_rpc_data.rpc3[3] << endl;

#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif

    m_msg->displayInfo(display.str());
}

void
RpcTrigStd::fillGeoDataForESD(uint32_t code,
                               float& x,float& y,float& z,float& r,float& p){

    if(m_rpcgeo->existCode(code)) {
        float xyz[3];
        float radius;
        float pitch;
        bool  res = true;
        res &= m_rpcgeo->give_strip_coordinates(code, xyz);
        res &= m_rpcgeo->give_strip_radius(code,radius);
    
        if( (code)/100000000) res |= m_rpcgeo->give_phi_strip_size(code,pitch);
        else                  res |= m_rpcgeo->give_eta_strip_size(code,pitch);
        if (res) {
            x = xyz[0];
            y = xyz[1];
            z = xyz[2];
            r = radius;
            p = pitch;
        } 
    }   
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/MuMessageInterface.h"
#include "TrigmuFast/MuServiceInterface.h"
#include "TrigmuFast/LVL1emu.h"
#include "TrigmuFast/RpcTrigFast.h"
#include "TrigmuFast/RpcTrigTB.h"
#include "TrigmuFast/TgcTrigDecoder.h"

#include "TrigMuonEvent/MuonFeatureDetails.h"

using namespace std;

const string LVL1emu::s_type = "Level-1 emulation";
const string LVL1emu::s_name = "Level-1 emulation - Roma 1";

LVL1emu::LVL1emu(MuMessageInterface& msg,
                 MuServiceInterface& svc,
		 bool maskUncCMAch,
		 bool trigOnPhiOnly,
		 bool useRpcTrigPatt,
		 float RpcTimeDelay,
                 const MuData& mu_data)
    : m_msg(&msg),m_svc(&svc),m_maskUncCMAch(maskUncCMAch),
      m_trigOnPhiOnly(trigOnPhiOnly),m_useRpcTrigPatt(useRpcTrigPatt),
      m_RpcTimeDelay(RpcTimeDelay),
      m_data(mu_data),m_cma_patterns((unsigned long int)0)
{
    m_tgcLutDigits.clear();
}

LVL1emu::~LVL1emu(void)
{
}

MUFAST::ErrorCode
LVL1emu::start(MuonFeatureDetails* det)
{

    if(m_data.isBarrelTrigger())       //BARREL
    {
        RpcTrigStd* emulation;

        // Level-1 emulation ////////////////////////////////////////////////
        if( m_svc->rpcgeo()->isFull() )                                    //
        {                                                                  //
            emulation = new RpcTrigFast(*m_msg,m_data.rpc_pad(),           //
	                      m_trigger_data,m_rpc_patterns,m_rpc_digits,  //
			                                  m_cma_patterns,  //
			                 m_svc->rpcgeo(),m_svc->rpccab(),  //
					 m_svc->mdtgeo(),m_maskUncCMAch,   //
					 m_trigOnPhiOnly,m_useRpcTrigPatt, //
					 m_RpcTimeDelay);                  //
        }                                                                  //
        else                                                               //
        {                                                                  //
            emulation = new RpcTrigTB(*m_msg,m_data.rpc_pad(),             //
	                      m_trigger_data,m_rpc_patterns,m_rpc_digits,  //
			                                  m_cma_patterns,  //
			           m_svc->rpcgeo(),m_svc->rpccab(),        //
				   m_svc->mdtgeo(),true,false,false,0.);   //
        }                                                                  //
                                                                           //
        TrigTimer* emulation_time = m_svc->give_timer(emulation->number());//
                                                                           //
        if(emulation_time) emulation_time->start();                        //
        if(!emulation->run(det))                                           //
	{                                                                  //
	    delete emulation;                                              //
	    //if (det!=0) det->setError(MUFAST::RPC_TRIGGER_ERROR);        //
	    return MUFAST::RPC_TRIGGER_ERROR;                              //
	}                                                                  //
        if(emulation_time) emulation_time->stop();                         //
        if(emulation_time) emulation_time->propVal(m_data.rpc_digits());   //
                                                                           //
                                                                           //
        if (m_msg->info_printout() >> emulation->number() & 1)             //
        {                                                                  //
            m_msg->lock();                                                 //
            emulation->printout();                                         //
	    m_msg->unlock();                                               //
        }                                                                  //
        /////////////////////////////////////////////////////////////////////

        delete emulation;
    }

    if(m_data.isEndcapTrigger())      //ENDCAP
    {
        // TGC processing ///////////////////////////////////////////////////
        __osstream* display = 0;                                           //
	if(m_msg->info_printout() >> 15 & 1)                               //
        {                                                                  //
                                                                           //
                                                                           //
#if (__GNUC__) && (__GNUC__ > 2) // put your gcc 3.2 specific code here    //
            display = new __osstream;                                      //
#else                            // put your gcc 2.95 specific code here   //
            char buffer[50000];                                            //
            for (int i=0;i<50000;++i) buffer[i] = '\0';                    //
            display = new __osstream(buffer,50000);                        //
#endif                                                                     //
	}                                                                  //
                                                                           //
        TgcTrigStd* tgcDeco = new TgcTrigDecoder(*m_msg,display,           //
	                          m_data.tgc_rdo(),                        //
	                          m_data.muon_roi(),                       //
				  m_trigger_data,                          //
				  m_TgcRawVector,                          //
                                  m_tgcLutDigits,                          //
				  m_svc->tgcLutSvc(), m_svc);              //
                                                                           //
	TrigTimer* tgc_deco_time = m_svc->give_timer(tgcDeco->number());   //
        if(tgc_deco_time) tgc_deco_time->start();                          //
	if(!tgcDeco->run(det))                                             //
	{                                                                  //
	    delete tgcDeco;                                                //
	    if(display) delete display;                                    //
	    //if (det!=0) det->setError(MUFAST::TGC_DECODER_ERROR);        //
	    return MUFAST::TGC_DECODER_ERROR;                              //
	}                                                                  //
        if(tgc_deco_time) tgc_deco_time->stop();                           //
        if(tgc_deco_time) tgc_deco_time->propVal(m_data.tgc_digits());     //
                                                                           //
	if (m_msg->info_printout() >> tgcDeco->number() & 1)               //
        {                                                                  //
            m_msg->lock();                                                 //
            tgcDeco->printout();                                           //
	    m_msg->unlock();                                               //
        }                                                                  //
	delete tgcDeco;                                                    //
	if(display) delete display;                                        //
        /////////////////////////////////////////////////////////////////////
    }

    // add RoI eta/phi
    m_trigger_data.roiEta = m_data.muon_roi()->eta();
    m_trigger_data.roiPhi = m_data.muon_roi()->phi();

    // return
    return MUFAST::NO_ERROR;
}

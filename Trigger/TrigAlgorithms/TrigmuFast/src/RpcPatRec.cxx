/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/MuMessageInterface.h"
#include "TrigmuFast/MuServiceInterface.h"
#include "TrigmuFast/RpcPatRec.h"
#include "TrigmuFast/TrackLZN.h"
#include "TrigmuFast/TrackTB.h"
#include "TrigmuFast/TrackTgc.h"
#include "TrigmuFast/RoadsDefADM.h"
#include "TrigmuFast/RoadsDefTB.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"

using namespace std;

const string RpcPatRec::s_type = "RPC Pattern recognition";
const string RpcPatRec::s_name = "RPC Pattern recognition - Roma 1";

RpcPatRec::RpcPatRec(MuMessageInterface& msg,
                     MuServiceInterface& svc,
		     bool tilt_field, bool usePol2ForEPt,
	             const TriggerData& trigger_data)
   : m_msg(&msg),m_svc(&svc),m_tilt_field(tilt_field),m_usePol2ForEPt(usePol2ForEPt),m_trigger_data(trigger_data)
{   
    memset(&m_trig_track, 0, sizeof(m_trig_track));
    memset(&m_muon_roads, 0, sizeof(m_muon_roads));
}

RpcPatRec::~RpcPatRec(void)
{   
}

MUFAST::ErrorCode
RpcPatRec::start(MuonFeatureDetails* det)
{
    TrackStd* tracking;
    RoadsDefStd* muonRoads;
    
    // trigger data fit algorithms //////////////////////////////////////////
    if( m_svc->isCosmic() && !m_trigger_data.isEndcap)                     //
    {                                                                      //
        tracking = new TrackTB(*m_msg,m_trigger_data, m_trig_track,        //
	                         m_muon_roads);                            //
    }                                                                      //
    else if (!m_trigger_data.isEndcap)                                     //
    {                                                                      //
        tracking = new TrackLZN(*m_msg,m_trigger_data, m_trig_track,       //
	                         m_muon_roads);                            //
    }                                                                      //
    else                                                                   //
    {                                                                      //
        tracking = new TrackTgc(*m_msg,m_svc,m_tilt_field,m_usePol2ForEPt, //
	 		         m_trigger_data,                           //
			         m_trig_track,m_muon_roads);               //
    }                                                                      //
                                                                           //
    TrigTimer* track_time = m_svc->give_timer(tracking->number());         //
                                                                           //
    if(track_time) track_time->start();                                    //
    if(!tracking->run(det))                                                //
    {                                                                      //
        delete tracking;                                                   //
	//if (det!=0) det->setError(MUFAST::TRIG_TRACKS_ERROR);            //
	return MUFAST::TRIG_TRACKS_ERROR;                                  //
    }                                                                      //
    if(track_time) track_time->stop();                                     //
                                                                           //
    if ((m_msg->info_printout() >> tracking->number() & 1))                //
    {                                                                      //
        m_msg->lock();                                                     //
        tracking->printout();                                              //
	m_msg->unlock();                                                   //
    }                                                                      //
    delete tracking;                                                       //
    /////////////////////////////////////////////////////////////////////////


    // Muon Roads definition algorithm //////////////////////////////////////
    if( m_svc->isCosmic() )                                                //
    {                                                                      //
        muonRoads = new RoadsDefTB(*m_msg,m_trigger_data,m_trig_track,     //
	                    m_muon_roads,m_svc->mdtgeo(),m_svc->roads());  //
    }                                                                      //
    else                                                                   //
    {                                                                      //
        muonRoads = new RoadsDefADM(*m_msg,m_trigger_data,m_trig_track,    //
	                    m_muon_roads,m_svc->mdtgeo(),m_svc->roads());  //
    }                                                                      //
                                                                           //
    TrigTimer* roads_time = m_svc->give_timer(muonRoads->number());        //
                                                                           //
    if(roads_time) roads_time->start();                                    //
    if(!muonRoads->run(det))                                               //
    {                                                                      //
        delete muonRoads;                                                  //
	//if (det!=0) det->setError(MUFAST::MUON_ROADS_ERROR);             //
	return MUFAST::MUON_ROADS_ERROR;                                   //
    }                                                                      //
    if(roads_time) roads_time->stop();                                     //
                                                                           //
    if (m_msg->info_printout() >> muonRoads->number() & 1)                 //
    {                                                                      //
        m_msg->lock();                                                     //
        muonRoads->printout();                                             //
	m_msg->unlock();                                                   //
    }                                                                      //
    delete muonRoads;                                                      //
    /////////////////////////////////////////////////////////////////////////


    return MUFAST::NO_ERROR;

}

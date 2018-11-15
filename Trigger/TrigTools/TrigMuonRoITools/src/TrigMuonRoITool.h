/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONROITOOL_H
#define TRIGMUONROITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h" 
#include "GaudiKernel/ServiceHandle.h"
#include "EventInfo/EventID.h"
#include "TrigMuonRoITools/ITrigMuonRoITool.h"

/* class StoreGateSvc; */
class IROBDataProviderSvc;

class TrigMuonRoITool: public AthAlgTool, 
		       virtual public ITrigMuonRoITool
{
    public:

    TrigMuonRoITool(const std::string& type, 
		    const std::string& name,
		    const IInterface*  parent);
    
    ~TrigMuonRoITool();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();
    
    /// iterator over muon RoIs in time with event in MuCTPi format
    virtual std::vector<ROIB::MuCTPIRoI>::const_iterator begin_InTimeRoIs() ;
    virtual std::vector<ROIB::MuCTPIRoI>::const_iterator end_InTimeRoIs() ;

    /// iterator over muon RoIs out of time with event in MuCTPi format
    /// pair < Muon RoI in RoIB format, difference: RoI(BCID)-event(BCID) >
    virtual std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator begin_OutOfTimeRoIs() ;
    virtual std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator end_OutOfTimeRoIs() ;

    private:

    /// run number - 32 bit unsigned
    EventID::number_type      m_run_no ;
    /// event number - 32 bit unsigned
    EventID::number_type      m_event_number ;
    /// bunch crossing ID,  32 bit unsigned
    EventID::number_type      m_bunch_crossing_id ;
    /// time stamp - posix time in seconds from 1970, 32 bit unsigned
    EventID::number_type      m_time_stamp ;
    /// time stamp ns - ns time offset for time_stamp, 32 bit unsigned
    EventID::number_type      m_time_stamp_ns_offset ;
    /// luminosity block identifier, 32 bit unsigned
    EventID::number_type      m_lumi_block ;

    typedef ServiceHandle<IROBDataProviderSvc> IIROBDataProviderSvc_t;
    /// Reference to the ROBDataProviderSvc service
    IIROBDataProviderSvc_t           m_robDataProviderSvc;

    /// Source identifiers for ROB fragments
    IntegerProperty                  m_daqMuCTPiROBid ;

    /// vectors with muCTPi ROB Ids to retrieve
    std::vector<uint32_t>            m_muCTPiRobIds;     

    /// Number of pt Thresholds
    static const uint32_t NUMBER_OF_PT_THRESHOLDS = 6;   // Number of pT thresholds

    /// trigger muCTPi RoIs from DAQ ROB in RoIB format
    std::vector<ROIB::MuCTPIRoI>                  m_inTime_muCTPIRoIs;     // RoIs from DAQ muCTPi ROB (in time with event BCID)
    std::vector< std::pair<ROIB::MuCTPIRoI,int> > m_outOfTime_muCTPIRoIs;  // RoIs from DAQ muCTPi ROB (out of time with event BCID)

    /// Helper for decoding the muCTPi RoIB and DAQ ROB
    void decodeMuCTPi();

    /// Helper for converting a mirod DAQ data word to a muCTPi RoIB data word
    uint32_t mirodToRoIBDataWord( uint32_t data_word );

    /// Helper to print contents of a muCTPi RoIB data word
    void dumpRoIBDataWord(uint32_t data_word );

    /// Configurable to decide how to decode muCTPi
    bool m_decodeMuCTPiFromROB;
};

#endif  // TRIGMUONROITOOL_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTEXECTOOL_H
#define TRIG_TRIGNTEXECTOOL_H

//
// Tool collects chain and sequence execution flow data.
//

// Framework
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Local
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigCostMonitor/ITrigNtTool.h"

class TrigOperationalInfoCollection;

namespace Trig
{
  static const InterfaceID IID_TrigNtExecTool("Trig::TrigNtExecTool", 1, 0); 
  
  class TrigNtExecTool : virtual public Trig::ITrigNtTool, public AthAlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtExecTool; }
    
    TrigNtExecTool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtExecTool() {}
    
    StatusCode initialize();
    StatusCode finalize();

    void SetSteer(const HLT::TrigSteer *ptr);

    bool Fill(TrigMonConfig *confg);
    bool Fill(TrigMonEvent &event);
    
  private:

    enum OpiState {
      kBad, kAlg, kChn, kSig, kSeq
    };
    enum AlgState { 
      kBadAlg, kCall, kCached, kRoiId, kStartSec, kStartUSec, kStopSec, kStopUSec, kTotalTime
    };
    enum SeqState { 
      kBadSeq, kFirst, kStart, kElapsedTime, kPrevious
    };

    struct ReadEntry {
      
      ReadEntry() 
  :  m_call(-1), m_palg(-1), m_value(0.0), 
     m_opiState(kBad), m_algState(kBadAlg), m_seqState(kBadSeq) {}
      
      int       m_call;
      int       m_palg;
      float     m_value;
      OpiState  m_opiState;
      AlgState  m_algState;
      SeqState  m_seqState;
      
      std::string m_key;
      std::vector<TrigConfSeq>::const_iterator m_seqIter;
    };

    typedef std::vector<ReadEntry>::iterator ReadIter;

  private:
    
    void ReadOPI(TrigMonEvent &event, const TrigOperationalInfoCollection &rvec);

    void ReadSeq(TrigMonEvent &event, const TrigConfChain &chain, 
     std::vector<ReadEntry> &read_vec, int icurr);
    void SaveSeq(TrigMonEvent &event, const TrigMonSeq &seq);
    void ReadAlg(TrigMonSeq &entry, 
     std::vector<ReadEntry> &read_vec, int icurr, int istop);
    
    TrigMonAlg MakeAlg(unsigned int alg_position,
           std::vector<ReadEntry> &read_vec, int icurr, int istop, int icall);
    
    void ParseKey(std::vector<std::string>& ls, const std::string &key, int ibeg);

  private:

    // Tool properties
    bool             m_printOPI;         // Print TrigOperationalInfo content
    bool             m_printSEQ;         // Print TrigMonSeq list
    bool             m_saveSeqTimer;     // Save sequence timer

    // Athena tool and service handles
    const HLT::TrigSteer        *m_parentAlg;

    // Tool variables
    std::string                  m_keyExten;         // Key for extended TrigOperationalInfo
    std::string                  m_fallBackKey;      // Old Key for extended TrigOperationalInfo
    double                       m_timeAlg;          // Trigger processing time from algorithms 
    double                       m_timeSeq;          // Trigger processing time from sequences
 
    TrigMonConfig               *m_config;           // Current trigger configuration

    std::vector<TrigConfChain>   m_TrigConfChain;    // Current chain configurations
    std::vector<TrigConfSeq>     m_TrigConfSeq;      // Current sequence configurations
  };
}

#endif

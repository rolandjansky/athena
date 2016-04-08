/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "eformat/eformat.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc.h"
#include <stdint.h>
#include <set>

/////////////////////////////////////////////////////////////////////////////
class IROBDataProviderSvc;
class StreamTag;

class MTCalibPeb:public AthAlgorithm {
public:
  MTCalibPeb(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  static std::string s_dataScoutingResultName;
  
  typedef ServiceHandle<IROBDataProviderSvc> IIROBDataProviderSvc_t;
  /// Reference to the ROBDataProviderSvc service
  IIROBDataProviderSvc_t  m_robDataProviderSvc;

  // Reference to a ROBDataProviderSvc which implements also the Hlt additions
  SmartIF<ITrigROBDataProviderSvc> m_hltROBDataProviderSvc;

  // L1 info retrieval
  BooleanProperty         m_retrieveLvl1;

  // ROB retrieval
  BooleanProperty                                  m_retrieveROBs;
  typedef std::vector<uint32_t>                    Uint32Array_t;
  typedef std::map<std::string, Uint32Array_t >    MapStringUint32Array_t;
  typedef std::map<std::string, std::vector<int> > MapStringVector_t;
  typedef SimpleProperty< Uint32Array_t >          Uint32ArrayProperty_t;
  typedef SimpleProperty< MapStringVector_t >      MapStringVectorProperty_t;
  MapStringVectorProperty_t                        m_mapRobIdsProperty;
  MapStringUint32Array_t                           m_mapRobIds;
  IntegerProperty                                  m_timeBetweenRobRetMicroSec;

  // string identifier for the HLT instance (L2,EF)
  StringProperty          m_hltInstance;

  // base name of the HLT result object in Storegate
  StringProperty          m_hltResultName;

  // StoreGate Keys = m_hltResultName + "_" + m_hltInstance
  std::string             m_hltResultSGKey;

  // base name of the ROBDataMonitorCollection object in Storegate
  StringProperty          m_ROBDataMonitorCollection_SG_Name;

  // StreamTags
  typedef SimpleProperty< std::vector< std::vector<std::string> > >  StreamTagProperty_t;
  StreamTagProperty_t                                                m_listStreamTags;

  typedef std::vector< TriggerInfo::StreamTag > StreamTagVector_t;
  StreamTagVector_t                             m_configuredStreamTags;

  // Chains
  Uint32ArrayProperty_t                         m_configuredChainNumbers;
  uint32_t                                      m_max_chain_counter;
  void setTriggerTypeBit(unsigned int bit, std::vector<uint32_t>& bitarray);

  // random accept
  DoubleProperty                                m_acceptRate;
  bool randomAccept(double rate);

  // Cpu burn time
  IntegerProperty                               m_burnTimeMicroSec;
  IntegerProperty                               m_burnTimeCycles;
  unsigned int randomCPUBurn(int burntime);

  // Helper to fill Rob and SubDet IDs into StreamTags
  void streamTagRobDetHelper(std::string input_str, std::set<TriggerInfo::number_type>& output_set); 

  // Helper to print StreamTags
  std::string printStreamTags(const std::vector<TriggerInfo::StreamTag>& rhs);
};

std::ostream& operator<<(std::ostream& os, const std::set<TriggerInfo::number_type>& rhs);

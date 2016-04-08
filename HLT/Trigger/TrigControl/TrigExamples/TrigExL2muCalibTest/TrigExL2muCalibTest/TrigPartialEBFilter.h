/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGPARTIALEBFILTER
#define TRIGPARTIALEBFILTER

#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventInfo/TriggerInfo.h"
#include <string>
#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////

class TrigPartialEBFilter : public AthAlgorithm {
public:
  TrigPartialEBFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  bool randomAccept(double rate);
  
  typedef std::vector< TriggerInfo::StreamTag > StreamTagVector_t;
  
  /// Reference to StoreGateSvc;
  std::string                      m_hltResultSGKey;
  
  
  //Algorithms properties
  DoubleProperty  m_acceptRate;        //fraction of physics evt to be accepted
                                       //  negative value: all physics rejected
			               //  0. - < 1.0    : randome selection
			               //  >= 1.0        : all physics accepted

  StringProperty   m_hltInstance;      // name of the HLT level: L2 or EF
  StringProperty   m_hltResultName;    // tag name for the HLT result
  			     
  StringProperty   m_calStreamName;    //calibration stream to be accepted;
                                       // null string: all calibration streams 
				       //              accepted
};

#endif

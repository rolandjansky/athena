/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_WRITETRIGCONFIG_H
#define ANP_WRITETRIGCONFIG_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : AlgEvent
 * @Author : Rustem Ospanov
 *
 * @Brief  : Algorithm for writing trigger configuration
 * 
 **********************************************************************************/

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"

// Local
#include "TrigCostBase/AlgEvent.h"

namespace Anp
{
  class WriteTrigConfig {
  public:
    
    WriteTrigConfig();
    virtual ~WriteTrigConfig();
    
    void Config(const Registry &reg);
    
    void Write(const std::vector<TrigConfChain> &cvec);

    void WriteLV1(std::ostream &os, const std::vector<TrigConfChain> &cvec);
    void WriteHLT(std::ostream &os, const std::vector<TrigConfChain> &cvec);

  private:

    std::string                 fXmlPathLV1; // Write LV1 XML file to this path
    std::string                 fXmlPathHLT; // Write HLT XML file to this path
    bool                        fDebug;      // Print debugging info
  };
}

#endif

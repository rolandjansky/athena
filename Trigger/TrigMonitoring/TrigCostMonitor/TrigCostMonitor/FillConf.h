/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_FILLCONF_H
#define TRIG_FILLCONF_H

//
// Helper function to fill TrigMonConfig from trigger configuration
//

// C/C++
#include <sstream>

#include "TrigConfJobOptData/JobOption.h"
#include "TrigConfJobOptData/JobOptionTable.h"


namespace TrigConf
{
  class CTPConfig;
  class HLTSequenceList;
  class HLTChainList;
  class HLTPrescaleSet;
}

class TrigMonConfig;

namespace Trig
{
  class FillConf {
  public:

    bool FillLV1(TrigMonConfig &confg, const TrigConf::CTPConfig       &ctp_config);
    bool FillSeq(TrigMonConfig &confg, const TrigConf::HLTSequenceList &seq_list);
    bool FillHLT(TrigMonConfig &confg, const TrigConf::HLTChainList    &chn_list);
    bool FillStreams(TrigMonConfig &confg, const TrigConf::HLTPrescaleSet &pss);
    bool FillEBHypo(TrigMonConfig &confg,  const TrigConf::JobOptionTable &jot);

    bool FillVar(TrigMonConfig &confg, 
		 std::string &menu, std::string &lv1_ps_name, std::string &hlt_ps_name);

    void UpdateLV1(TrigMonConfig &confg, const TrigConf::CTPConfig &ctp_confg);
    void UpdateHLT(TrigMonConfig &confg, const TrigConf::HLTChainList &chnl);

    std::string error() const { return m_error.str(); }
    std::string debug() const { return m_debug.str(); }

  private:
    
    std::stringstream m_error;
    std::stringstream m_debug;
  };
}

#endif

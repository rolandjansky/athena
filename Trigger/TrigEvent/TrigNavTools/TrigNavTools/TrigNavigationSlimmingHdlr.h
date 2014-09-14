/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_NAVIGATION_SLIMMING_HLDR
#define TRIG_NAVIGATION_SLIMMING_HLDR

#include "AthenaKernel/ISlimmingHdlr.h"

#include <string>
#include <vector>

#include <stdint.h>



namespace HLT {

  class TrigNavigationSlimmingTool;
  class HLTResult;

  class TrigNavigationSlimmingHdlr : public ::Athena::ISlimmingHdlr
  {

    public:

      TrigNavigationSlimmingHdlr( const TrigNavigationSlimmingHdlr& rhs );

      TrigNavigationSlimmingHdlr( HLT::HLTResult *result, INamedInterface *requestor );

      ~TrigNavigationSlimmingHdlr();

      void commit();

      void rollback();

      void *object();

      std::type_info& type_id();

      const ::INamedInterface* requester();

      void setSlimmer(HLT::TrigNavigationSlimmingTool *slimmer) { m_slimmer = slimmer; };
      HLT::TrigNavigationSlimmingTool *slimmer() { return m_slimmer; };

      void setName(const std::string &name) { m_name = name; };
      std::string name() { return m_name; };

    private:

      HLT::HLTResult *m_result;

      const INamedInterface *m_requester;

      // nav and cuts data so that you can undo a slim
      std::vector<uint32_t> m_unslimmedNavData;
      std::vector<unsigned int> m_unslimmedCuts;

      HLT::TrigNavigationSlimmingTool *m_slimmer;

      std::string m_name;

  };

}

#endif

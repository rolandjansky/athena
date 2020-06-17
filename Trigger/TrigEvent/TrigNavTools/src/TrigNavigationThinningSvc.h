// -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigNavTools_TrigNavigationThinningSvc_h
#define TrigNavTools_TrigNavigationThinningSvc_h

#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/ITrigNavigationThinningSvc.h"
#include "TrigNavTools/TrigNavigationSlimmingTool.h"




class TrigNavigationThinningSvc : public extends<AthService, ITrigNavigationThinningSvc>
{
public:
  TrigNavigationThinningSvc( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;
  
  // specifics of ITrigNavigationThinning

  virtual StatusCode doSlimming( const EventContext& /*ctx*/,
                                 std::vector<uint32_t>& slimmed_and_serialized ) const override
  { return m_slimmingTool->doSlimming(slimmed_and_serialized);  }
  


private:
  ToolHandle<HLT::TrigNavigationSlimmingTool> m_slimmingTool;
};


#endif

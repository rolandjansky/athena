/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: KLFitterRun.h 688037 2015-08-07 01:24:06Z morrisj $
#ifndef ANALYSISTOP_TOPEVENTRECONSTRCUTIONTOOLS_KLFITTERRUN_H
#define ANALYSISTOP_TOPEVENTRECONSTRCUTIONTOOLS_KLFITTERRUN_H

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopEventReconstructionTools/KLFitterTool.h"

namespace top{
  class Event;
  class TopConfig;
  
  class KLFitterRun : public EventSelectorBase {
    public:
      KLFitterRun(const std::string& kLeptonType,std::shared_ptr<top::TopConfig> config);
      virtual ~KLFitterRun(){}
      
      virtual bool apply(const top::Event&) const override;
      std::string name() const;
      
    private:
      std::string m_name;

      std::unique_ptr<top::KLFitterTool> m_myFitter;
      
  };
}
#endif

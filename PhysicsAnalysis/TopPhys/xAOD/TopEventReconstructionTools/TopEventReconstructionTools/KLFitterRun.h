/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: KLFitterRun.h 688037 2015-08-07 01:24:06Z morrisj $
#ifndef ANALYSISTOP_TOPEVENTRECONSTRCUTIONTOOLS_KLFITTERRUN_H
#define ANALYSISTOP_TOPEVENTRECONSTRCUTIONTOOLS_KLFITTERRUN_H

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopEventReconstructionTools/KLFitterTool.h"

namespace top {
  class Event;
  class TopConfig;

  class KLFitterRun: public EventSelectorBase {
  public:
    KLFitterRun(const std::string& kSelectionName, const std::string& kParameters,
                std::shared_ptr<top::TopConfig> config);
    virtual ~KLFitterRun() {}

    virtual bool apply(const top::Event&) const override;
    std::string name() const override;

  private:
      enum jetPos { CANBEB=0, CANBELF , JETTYPESIZE};

      bool hasAutoSetOption(const std::string curtom_parameters);
      std::vector<std::vector<unsigned int> > setJetsToRun(const top::Event&) const;

      std::string m_name;

      std::unique_ptr<top::KLFitterTool> m_myFitter;

      bool m_useJetAutoSet;
      int m_Njcut;
      int m_nb;
      int m_delta;

  };
}
#endif

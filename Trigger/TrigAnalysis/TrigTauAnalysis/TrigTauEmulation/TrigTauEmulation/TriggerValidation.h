/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef TRIGTAUEMULATION_TRIGGERVALIDATION_H
#define TRIGTAUEMULATION_TRIGGERVALIDATION_H

#include <map>

#include "TH1F.h"
#include "TH2F.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"

class TriggerValidation

{
  public: 
    TriggerValidation (const std::string & name);
    virtual ~ TriggerValidation() {};
    void fill_histograms(const xAOD::EventInfo * ei,
        const xAOD::EmTauRoIContainer * l1taus,
        const std::string & decor = "");

    void fill_l1tau(const xAOD::EmTauRoI * l1tau);

    std::map<std::string, TH1F*> Histograms() {return m_h1d;}
    std::map<std::string, TH2F*> Maps() {return m_h2d;}

  private:
    std::map<std::string, TH1F*> m_h1d;
    std::map<std::string, TH2F*> m_h2d;


};

#endif

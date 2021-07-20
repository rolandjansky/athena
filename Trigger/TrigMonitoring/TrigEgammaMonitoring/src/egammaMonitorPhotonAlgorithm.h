/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef egammaMonitorPhotonAlgorithm_H
#define egammaMonitorPhotonAlgorithm_H

#include "egammaMonitorBaseAlgorithm.h"

#include "StoreGate/ReadDecorHandleKeyArray.h"

class egammaMonitorPhotonAlgorithm: public egammaMonitorBaseAlgorithm
{

  public:

    egammaMonitorPhotonAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  protected:

    void filltopoPhotonShowerShapes(const EventContext& ctx) const;
    void filltopoPhotonIsolation(const EventContext& ctx) const;

  private:

    TString m_baseName;
    SG::ReadHandleKey<xAOD::PhotonContainer> m_photonsKey{this, "PhotonKey", "Photons", ""};
    SG::ReadDecorHandleKeyArray<xAOD::PhotonContainer> m_isoKey{this, "IsoVarKeys", { }, "The list of isolation variables to be monitored"};
    ToolHandle<GenericMonitoringTool> m_monTool{this, "MonTool", "", "Monitoring tool"};
};
 
#endif

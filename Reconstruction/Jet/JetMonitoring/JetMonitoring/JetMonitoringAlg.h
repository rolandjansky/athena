/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORALGORITHM_H
#define JETMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "xAODJet/JetContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetMonitoring/IJetHistoFiller.h"


////////////////////////////////////////////////////
/// \class JetMonitoringAlg
///
/// A monitoring algorithm in charge of filling histogram for a
/// JetContainer.
///
/// The jet container is specified as the JetContainerName property
/// The histograms to be filled are specified by a list of IJetHistoFiller tools
///  set to the HistoFillers property.

class JetMonitoringAlg : public AthMonitorAlgorithm {
public:
    JetMonitoringAlg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~JetMonitoringAlg();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

    bool isPassed(const std::string &c) const ;
    unsigned int isPassedBits(const std::string &c) const ;
private:

  SG::ReadHandleKey<xAOD::JetContainer> m_jetContainerKey;
  
  ToolHandleArray<IJetHistoFiller> m_jetFillerTools;

  bool m_failureOnMissingContainer;
  
};
#endif

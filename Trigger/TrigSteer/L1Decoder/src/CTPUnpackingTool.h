/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1DECODER_CTPUNPACKINGTOOL_H
#define L1DECODER_CTPUNPACKINGTOOL_H 1

#include "CTPUnpackingToolBase.h"

#include "DecisionHandling/HLTIdentifier.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "GaudiKernel/IIncidentListener.h"

class CTPUnpackingTool : public CTPUnpackingToolBase, public IIncidentListener {
public:
  
  CTPUnpackingTool( const std::string& type,
                    const std::string& name, 
                    const IInterface* parent );

  virtual StatusCode decode(const ROIB::RoIBResult& roib, HLT::IDVec& enabledChains) const override;

  virtual StatusCode initialize() override;
  virtual void handle(const Incident& incident) override;

private:
  StatusCode decodeCTPToChainMapping();
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
}; 


#endif

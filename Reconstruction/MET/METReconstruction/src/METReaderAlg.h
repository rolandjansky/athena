/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METReaderAlg.h

#ifndef METReaderAlg_H
#define METReaderAlg_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/DataHandle.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComponentMap.h"


namespace met {
  class METReaderAlg : public AthReentrantAlgorithm { 

  public: 

    /// Constructor with parameters:
    METReaderAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~METReaderAlg(); 

    /// Athena algorithm's Hooks
    virtual StatusCode  initialize() override;
    virtual StatusCode  execute_r (const EventContext& ctx) const override;
    virtual StatusCode  finalize() override;

  private: 

    /// Default constructor:
    METReaderAlg();
    
    SG::ReadHandleKey<xAOD::MissingETContainer> m_met_RefFinal;
    SG::ReadHandleKey<xAOD::MissingETContainer> m_met_LocHadTopo;
    SG::ReadHandleKey<xAOD::MissingETContainer> m_met_Track;
    SG::ReadHandleKey<xAOD::MissingETContainer> m_met_PFlow;
    SG::ReadHandleKey<xAOD::MissingETContainer> m_met_Truth;
    SG::ReadHandleKey<xAOD::MissingETContainer> m_met_TruthReg;
    SG::ReadHandleKey<xAOD::MissingETComponentMap> m_metMap_RefFinal;
    SG::ReadHandleKey<xAOD::MissingETComponentMap> m_metMap_LocHadTopo;


  }; 

}

#endif

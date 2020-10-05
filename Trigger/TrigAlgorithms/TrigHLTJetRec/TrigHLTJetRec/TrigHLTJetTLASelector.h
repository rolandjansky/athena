/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration  
 */

/**
 * @class TrigHLTJetTLASelector
 * @brief Algorithm to select jets from the TLA/scouting stream
 *        and store them in a vector of jets to be passed on
 * @author Antonio Boveia
 * @author Caterina Doglioni
**/ 

#ifndef TRIGHLTJETTLASELECTOR_H
#define TRIGHLTJETTLASELECTOR_H

#include "Gaudi/Property.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/Jet.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class TrigHLTJetTLASelector
: public AthReentrantAlgorithm
{
  public:
    TrigHLTJetTLASelector (const std::string& name, ISvcLocator* svcLocator);
    virtual StatusCode initialize() override;
    virtual StatusCode execute (const EventContext& ctx) const override;

  private:
    // Declare the keys used to access the data: one for reading and one
    // for writing. This initializer list syntax also informs the scheduler of the
    // data dependency, obviating the need for a separate declareProperty(...)

    SG::ReadHandleKey< xAOD::JetContainer > m_inputJetsKey {
      this, "TLAInputJetCollection", "HLT_xAOD__JetContainer_a4tcemsubjesISFS", "input jet container key"};  
    SG::WriteHandleKey< xAOD::JetContainer > m_outputJetsKey {
      this, "TLAOutputJetCollection", "HLT_TrigHLTJetTLASelectorCollection", "output jet container key"};  

    Gaudi::Property<float> m_jetPtThreshold {this, "jetPtThreshold", 0. , "Threshold for saving jets in TLA container"};
    Gaudi::Property<int> m_maxNJets {this, "maxNJets", -1 , "Maximum number of jets saved in TLA container"};
    
};

#endif


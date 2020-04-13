/*
  Filter to deselect chains for muon trigger algorithms
  
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEF_MUONCHAINFILTERALG_H
#define TRIGMUONEF_MUONCHAINFILTERALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"

class MuonChainFilterAlg : public AthAlgorithm
{
  public :

    /** Constructor **/
    MuonChainFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );
  
    /** initialize */
    StatusCode initialize();
  
    /** finalize */
    StatusCode finalize();
  
    /** execute the filter alg */
    StatusCode execute();


  private :

    Gaudi::Property< std::vector<std::string> > m_filterChains {this, "ChainsToFilter", { }, "Vector of chains to filter out" };
    Gaudi::Property< bool > m_writeL2muComb {this, "WriteMuComb", true, "Flag to record muComb muons" };
    SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer>  m_inputDecisionKeys{ this, "InputDecisions", {}, "Inputs to the filter" };
    SG::WriteHandleKey<xAOD::L2CombinedMuonContainer> m_muCombKey {this, "L2MuCombContainer", "MuonL2CBInfo", "Output container for muComb"};

};

#endif

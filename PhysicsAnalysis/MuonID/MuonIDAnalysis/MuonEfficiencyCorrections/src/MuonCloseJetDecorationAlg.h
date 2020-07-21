/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef DERIVATIONFRAMEWORKMUONS_MUONCLOSEJETDECORATIONALG_H
#define DERIVATIONFRAMEWORKMUONS_MUONCLOSEJETDECORATIONALG_H
// Core include(s):
#include "AthContainers/AuxElement.h"
// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include <memory>
namespace CP {

    /// Small algorithm to quickly write the jetDR decoration needed for isolation
    /// scale-factors to the muon
    class MuonCloseJetDecorationAlg: public AthAlgorithm {

        public:
            /// Regular Algorithm constructor
            MuonCloseJetDecorationAlg(const std::string& name, ISvcLocator* svcLoc);

            /// Function initialising the algorithm
             StatusCode initialize() override;
            /// Function executing the algorithm
             StatusCode execute() override;
             ~MuonCloseJetDecorationAlg()= default;

        private:
            /// muon container
            std::string m_muon_container;
            /// jet container
            std::string m_jet_container;
            /// name of the decoration
            std::string m_jet_dr_decoration;
            /// value to be assigned if no jet is in the event
            float m_dummy_value;
            /// Minimum pt on the jet
            float m_jet_pt_cut;
            std::unique_ptr< SG::AuxElement::Decorator<float>> m_decorator;
            
    };

} 

#endif

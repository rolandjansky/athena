/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronInJetSubtractionAlgorithm.h 654856 2015-03-17 19:00:30Z dferreir $
#ifndef ANALYSISTOP_TOPJETSUBSTRUCTURE_ELECTRONINJETSUBTRACTIONALGORITHM_H
#define ANALYSISTOP_TOPJETSUBSTRUCTURE_ELECTRONINJETSUBTRACTIONALGORITHM_H

#include "xAODEgamma/ElectronContainer.h"
#include "xAODJet/JetContainer.h"
#include "TopJetSubstructure/TTBarElectronJetOverlap.h"

namespace top{
  class ElectronInJetSubtractionAlgorithm final {
    public:
      ElectronInJetSubtractionAlgorithm();
      virtual ~ElectronInJetSubtractionAlgorithm(){}
      
      ElectronInJetSubtractionAlgorithm(const ElectronInJetSubtractionAlgorithm& rhs) = delete;
      ElectronInJetSubtractionAlgorithm(ElectronInJetSubtractionAlgorithm&& rhs) = delete;
      ElectronInJetSubtractionAlgorithm& operator=(const ElectronInJetSubtractionAlgorithm& rhs) = delete; 
      
      void apply( xAOD::ElectronContainer* electrons , xAOD::JetContainer* jets , const bool useLooseElectrons=false );
      
    private:
      bool m_doLooseCuts;
      // Pass selection strings
      const std::string m_passPreORSelection;
      const std::string m_passPreORSelectionLoose;      

      TTBarElectronJetOverlap m_o;
  };
}

#endif

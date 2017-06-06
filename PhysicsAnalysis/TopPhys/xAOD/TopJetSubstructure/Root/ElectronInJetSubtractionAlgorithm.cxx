/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronInJetSubtractionAlgorithm.cxx 654856 2015-03-17 19:00:30Z dferreir $
#include "TopJetSubstructure/ElectronInJetSubtractionAlgorithm.h"

#include <iostream>

namespace top{
  
  ElectronInJetSubtractionAlgorithm::ElectronInJetSubtractionAlgorithm() :
    m_doLooseCuts(false),
    m_passPreORSelection("passPreORSelection"),
    m_passPreORSelectionLoose("passPreORSelectionLoose")
  {
    
  }
  
  void ElectronInJetSubtractionAlgorithm::apply( xAOD::ElectronContainer* electrons , xAOD::JetContainer* jets , const bool useLooseElectrons )
  {
    std::string leptonDef;
    if (!useLooseElectrons){
      leptonDef = m_passPreORSelection;
    }
    if (useLooseElectrons) {
      leptonDef = m_passPreORSelectionLoose;
    }

    unsigned int passedElectronsBefore(0);
    for (auto e : *electrons) {
      if (e->auxdataConst<char>(leptonDef) == 1) {
        ++passedElectronsBefore;
      }
    }
    
    m_o.Load(jets, electrons, leptonDef);
    m_o.AnalyzeEvent(leptonDef);

    unsigned int passedElectrons(0);
    for (auto e : *electrons) {
      if (e->auxdataConst<char>(leptonDef) == 1) {
        ++passedElectrons;
      }
    }
    
    bool debug(false);
    if (debug) {
      std::cout<<"Total number of electrons = "<<electrons->size()<<" that pass cuts = "<<passedElectrons<<"/" << passedElectronsBefore << " :: Total number of jets = "<<jets->size()<<std::endl;
    }
  }
  
}

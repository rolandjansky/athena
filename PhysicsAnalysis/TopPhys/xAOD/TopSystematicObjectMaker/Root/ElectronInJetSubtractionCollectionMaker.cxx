/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronInJetSubtractionCollectionMaker.cxx 680332 2015-07-04 01:25:47Z morrisj $
#include "TopSystematicObjectMaker/ElectronInJetSubtractionCollectionMaker.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCore/ShallowCopy.h"

namespace top{
  
  ElectronInJetSubtractionCollectionMaker::ElectronInJetSubtractionCollectionMaker( const std::string& name ) :
    asg::AsgTool( name ),
    m_config(nullptr),
    m_doLooseCuts(false)
  {
    declareProperty( "config" , m_config );    
  }
  
  StatusCode ElectronInJetSubtractionCollectionMaker::initialize()
  {
    if (!m_config->isMC())
        m_doLooseCuts = true;
    
    if (m_config->doOverlapRemovalOnLooseLeptonDef())
        m_doLooseCuts = true;
    
    if (m_config->doLooseEvents())
        m_doLooseCuts = true;    
    
    return StatusCode::SUCCESS;    
  }
  
  StatusCode ElectronInJetSubtractionCollectionMaker::execute()
  {
    // Loop over nominal, all electron syst and all jet syst
    // For each:
    //  Create a shallow electron and a shallow jet collection 
    //  Run the subtraction algorithm:
    //    - Modify some jet 4-momentum
    //    - Veto some electrons
    //  Place modified shallow collections into TStore
    
    for (auto currentSystematic : *m_config->systHashElectronInJetSubtraction()) {
      
      // Get the Electrons
      const xAOD::ElectronContainer* electrons(nullptr);
      top::check(evtStore()->retrieve(electrons, m_config->sgKeyElectronsStandAlone( currentSystematic ) ) , "ElectronInJetSubtractionCollectionMaker::execute() Failed to retrieve electrons" );
      
      // Get the Jets 
      const xAOD::JetContainer* jets(nullptr);
      top::check(evtStore()->retrieve(jets, m_config->sgKeyJetsStandAlone( currentSystematic ) ) , "ElectronInJetSubtractionCollectionMaker::execute() Failed to retrieve jets" );
      
      // Shallow copy the electrons
      std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > shallow_electrons = xAOD::shallowCopyContainer( *electrons );
      // Shallow copy the jets
      std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_jets = xAOD::shallowCopyContainer( *jets );
      
      // Apply the Electron In Jet Subtraction
      // The actual calculation is performed in another package
      m_subtractionAlgorithm.apply( shallow_electrons.first , shallow_jets.first );
      
      // Save the modified electrons to TStore
      std::string outputElectronsSGKey = m_config->sgKeyElectrons( currentSystematic );
      std::string outputElectronsSGKeyAux = outputElectronsSGKey + "Aux.";
      
      xAOD::TReturnCode saveElectrons = evtStore()->tds()->record( shallow_electrons.first , outputElectronsSGKey );
      xAOD::TReturnCode saveElectronsAux = evtStore()->tds()->record( shallow_electrons.second , outputElectronsSGKeyAux );
      if( !saveElectrons || !saveElectronsAux ){
        return StatusCode::FAILURE;
      }      
           
      // Save the modified jets to TStore
      std::string outputJetsSGKey = m_config->sgKeyJets( currentSystematic , false );
      std::string outputJetsSGKeyAux = outputJetsSGKey + "Aux.";
      
      xAOD::TReturnCode saveJets = evtStore()->tds()->record( shallow_jets.first , outputJetsSGKey );
      xAOD::TReturnCode saveJetsAux = evtStore()->tds()->record( shallow_jets.second , outputJetsSGKeyAux );
      if( !saveJets || !saveJetsAux ){
        return StatusCode::FAILURE;
      }
      
      
      // If we are using loose lepton definitions anywhere, we'd better make more jet colelctions
      // We subtract loose leptons from these jets
      // We now have loose jets! fun fun fun, we all love combinatorics 
      
      if (m_doLooseCuts) {
        // Shallow copy the jets
        std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_jetsLoose = xAOD::shallowCopyContainer( *jets ); 
        
        // Apply the Electron In Jet Subtraction
        // The actual calculation is performed in another package
        m_subtractionAlgorithm.apply( shallow_electrons.first , shallow_jetsLoose.first , m_doLooseCuts );

        // Save the modified jets to TStore
        std::string outputJetsLooseSGKey = m_config->sgKeyJets( currentSystematic , m_doLooseCuts );
        std::string outputJetsLooseSGKeyAux = outputJetsLooseSGKey + "Aux.";
        
        xAOD::TReturnCode saveJetsLoose = evtStore()->tds()->record( shallow_jetsLoose.first , outputJetsLooseSGKey );
        xAOD::TReturnCode saveJetsLooseAux = evtStore()->tds()->record( shallow_jetsLoose.second , outputJetsLooseSGKeyAux );
        if( !saveJetsLoose || !saveJetsLooseAux ){
          return StatusCode::FAILURE;
        }        
      }
      
      
    }
   
    return StatusCode::SUCCESS;      
  }
  
}

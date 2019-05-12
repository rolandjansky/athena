/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgAnalysisAlgorithms/AsgViewFromSelectionAlg.h>

#include <CxxUtils/fpcompare.h>
#include <xAODCore/AuxContainerBase.h>
#include <xAODEgamma/PhotonContainer.h>
#include <xAODEgamma/ElectronContainer.h>
#include <xAODJet/JetContainer.h>
#include <xAODMuon/MuonContainer.h>
#include <xAODTau/TauJetContainer.h>
#include <xAODTau/DiTauJetContainer.h>

//
// method implementations
//

namespace CP
{
  template<typename Type> StatusCode AsgViewFromSelectionAlg ::
  executeTemplate (const CP::SystematicSet& sys)
  {
    const Type *input = nullptr;
    ANA_CHECK (evtStore()->retrieve (input, m_inputHandle.getName (sys)));
    auto viewCopy = std::make_unique<Type> (SG::VIEW_ELEMENTS);
    for (const auto particle : *input)
    {
      bool keep = true;
      for (const auto& accessor : m_accessors)
      {
        if ((accessor.first->getBits (*particle) | accessor.second) != selectionAccept())
        {
          keep = false;
          break;
        }
      }
      if (keep)
      {
        typename Type::value_type particleNC =
          const_cast<typename Type::value_type>(particle);
        viewCopy->push_back (particleNC);
      }
    }

    if (m_sortPt)
    {
      std::sort (viewCopy->begin(), viewCopy->end(), [] (const xAOD::IParticle *a, const xAOD::IParticle *b) {return CxxUtils::fpcompare::greater (a->pt(), b->pt());});
    }

    // If anyone might be concerned about efficiency here, this will
    // add/sort a couple more entries than needed only to remove them
    // from the vector afterwards, so there is a slight efficiency
    // loss.  However, this option is not expected to be used very
    // often and the algorithm is still expected to run quickly, so I
    // decided to keep the code above simpler and just do this as a
    // separate step, instead of trying to optimize this by
    // integrating it with the code above.
    if (viewCopy->size() > m_sizeLimit)
      viewCopy->resize (m_sizeLimit);

    // In case we want to output a view copy, do that here.
    if (!m_deepCopy)
    {
      ANA_CHECK (evtStore()->record (viewCopy.release(),
                                     m_outputHandle.getName (sys)));
      return StatusCode::SUCCESS;
    }

    // Apparently we want to make a deep copy. So set that one up.
    auto deepCopy = std::make_unique<Type> ();
    auto aux = std::make_unique<xAOD::AuxContainerBase> ();
    deepCopy->setStore (aux.get());
    deepCopy->reserve (viewCopy->size());
    for (auto particle : *viewCopy)
    {
      typename Type::value_type pcopy = new typename Type::base_value_type();
      deepCopy->push_back (pcopy);
      *pcopy = *particle;
    }

    // Record the deep copy into the event store.
    ANA_CHECK (evtStore()->record (deepCopy.release(),
                                   m_outputHandle.getName (sys)));
    ANA_CHECK (evtStore()->record (aux.release(),
                                   m_outputHandle.getName (sys) + "Aux."));

    return StatusCode::SUCCESS;
  }




  StatusCode AsgViewFromSelectionAlg ::
  executeFindType (const CP::SystematicSet& sys)
  {
    if( m_allowMissing ) {
      const std::string& name = m_inputHandle.getName( sys );
      if( ! evtStore()->contains< xAOD::IParticleContainer >( name ) ) {
        m_function = &AsgViewFromSelectionAlg::executeMissing;
        return StatusCode::SUCCESS;
      }
    }

    const xAOD::IParticleContainer *input = nullptr;
    ANA_CHECK (m_inputHandle.retrieve (input, sys));

    if (dynamic_cast<const xAOD::ElectronContainer*> (input))
    {
      m_function =
        &AsgViewFromSelectionAlg::executeTemplate<xAOD::ElectronContainer>;
    }
    else if (dynamic_cast<const xAOD::PhotonContainer*> (input))
    {
      m_function =
        &AsgViewFromSelectionAlg::executeTemplate<xAOD::PhotonContainer>;
    }
    else if (dynamic_cast<const xAOD::JetContainer*> (input))
    {
      m_function =
        &AsgViewFromSelectionAlg::executeTemplate<xAOD::JetContainer>;
    }
    else if (dynamic_cast<const xAOD::MuonContainer*> (input)) {
      m_function =
        &AsgViewFromSelectionAlg::executeTemplate<xAOD::MuonContainer>;
    }
    else if (dynamic_cast<const xAOD::TauJetContainer*> (input))
    {
      m_function =
        &AsgViewFromSelectionAlg::executeTemplate<xAOD::TauJetContainer>;
    }
    else if (dynamic_cast<const xAOD::DiTauJetContainer*> (input))
    {
      m_function =
        &AsgViewFromSelectionAlg::executeTemplate<xAOD::DiTauJetContainer>;
    }
    else
    {
      ANA_MSG_ERROR ("unknown type contained in AsgViewFromSelectionAlg, please extend it");
      return StatusCode::FAILURE;
    }

    return (this->*m_function) (sys);
  }



  StatusCode AsgViewFromSelectionAlg ::
  executeMissing (const CP::SystematicSet&)
  {
    return StatusCode::SUCCESS;
  }



  AsgViewFromSelectionAlg ::
  AsgViewFromSelectionAlg (const std::string& name, 
                           ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    /// \todo this would probably better be an std::map, but this
    /// isn't supported as a property type for AnaAlgorithm right now
    declareProperty ("selection", m_selection, "the list of selection decorations");
    declareProperty ("ignore", m_ignore, "the list of cuts to *ignore* for each selection");
    declareProperty ("sortPt", m_sortPt, "whether to sort objects in pt");
    declareProperty ("allowMissing", m_allowMissing,
                     "Allow the input container to be missing");
    declareProperty ("sizeLimit", m_sizeLimit, "the limit on the size of the output container");
    declareProperty ("deepCopy", m_deepCopy, "perform a deep copy");
  }



  StatusCode AsgViewFromSelectionAlg ::
  initialize ()
  {
    m_systematicsList.addHandle (m_inputHandle);
    m_systematicsList.addHandle (m_outputHandle);
    ANA_CHECK (m_systematicsList.initialize());

    if (m_ignore.size() > m_selection.size())
    {
      ANA_MSG_ERROR ("ignore property can't have more properties than selection property");
      return StatusCode::FAILURE;
    }
    for (std::size_t iter = 0, end = m_selection.size(); iter != end; ++ iter)
    {
      SelectionType ignore = 0;
      if (iter < m_ignore.size())
        ignore = m_ignore[iter];
      std::unique_ptr<ISelectionAccessor> accessor;
      ANA_CHECK (makeSelectionAccessor (m_selection[iter], accessor));
      m_accessors.push_back (std::make_pair (std::move (accessor), ignore));
    }

    return StatusCode::SUCCESS;
  }



  StatusCode AsgViewFromSelectionAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        return (this->*m_function) (sys);});
  }
}

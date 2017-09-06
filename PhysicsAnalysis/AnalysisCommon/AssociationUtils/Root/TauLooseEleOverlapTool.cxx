// System includes
#include <typeinfo>

// Framework includes
#include "CxxUtils/make_unique.h"
#include "AthContainers/ConstDataVector.h"

// Local includes
#include "AssociationUtils/TauLooseEleOverlapTool.h"

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  TauLooseEleOverlapTool::TauLooseEleOverlapTool(const std::string& name)
    : BaseOverlapTool(name)
  {
    declareProperty("DR", m_maxDR = 0.2,
                    "Delta-R cone for flagging overlaps");
    declareProperty("UseRapidity", m_useRapidity = true,
                    "Calculate delta-R using rapidity");
    declareProperty("ElectronID", m_eleID = "DFCommonElectronsLHLoose",
                    "Loose electron ID selection string");
    declareProperty("AltElectronID", m_altEleID = "",
                    "Optional alternate ID in case primary one unavailable");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode TauLooseEleOverlapTool::initializeDerived()
  {
    using CxxUtils::make_unique;

    // Initialize the dR matcher
    m_dRMatcher = make_unique<DeltaRMatcher>(m_maxDR, m_useRapidity);

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode TauLooseEleOverlapTool::
  findOverlaps(const xAOD::IParticleContainer& cont1,
               const xAOD::IParticleContainer& cont2) const
  {
    // Check the container types
    if(typeid(cont1) != typeid(xAOD::TauJetContainer) &&
       typeid(cont1) != typeid(ConstDataVector<xAOD::TauJetContainer>)) {
      ATH_MSG_ERROR("First container arg is not of type TauJetContainer!");
      return StatusCode::FAILURE;
    }
    if(typeid(cont2) != typeid(xAOD::ElectronContainer) &&
       typeid(cont2) != typeid(ConstDataVector<xAOD::ElectronContainer>)) {
      ATH_MSG_ERROR("Second container arg is not of type ElectronContainer!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( findOverlaps(static_cast<const xAOD::TauJetContainer&>(cont1),
                            static_cast<const xAOD::ElectronContainer&>(cont2)) );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode TauLooseEleOverlapTool::
  findOverlaps(const xAOD::TauJetContainer& taus,
               const xAOD::ElectronContainer& electrons) const
  {
    ATH_MSG_DEBUG("Removing taus from loose electrons");

    // Initialize output decorations if necessary
    m_decHelper->initializeDecorations(taus);
    m_decHelper->initializeDecorations(electrons);

    // Loop over loose surviving electrons
    for(auto electron : electrons){
      if(m_decHelper->isRejectedObject(*electron)) continue;

      // Check the electron ID
      bool passID = false;
      ATH_CHECK( checkElectronID(*electron, passID) );
      if(!passID) continue;

      // Loop over surviving taus
      for(auto tau : taus){
        if(!m_decHelper->isSurvivingObject(*tau)) continue;

        // Test for overlap
        if(m_dRMatcher->objectsMatch(*electron, *tau)){
          ATH_CHECK( handleOverlap(tau, electron) );
        }
      }
    }  

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Loose electron criteria
  //---------------------------------------------------------------------------
  StatusCode TauLooseEleOverlapTool::
  checkElectronID(const xAOD::Electron& electron, bool& pass) const
  {
    try {
      // Try the configured ID string.
      if(!electron.passSelection(pass, m_eleID)) {
        // If the ID wasn't found, try the fallback ID if provided.
        if(!m_altEleID.empty()) {
          if(!electron.passSelection(pass, m_altEleID)) {
            ATH_MSG_ERROR("Electron IDs unavailable: " <<
                          m_eleID << ", " << m_altEleID);
            return StatusCode::FAILURE;
          }
        }
        else {
          ATH_MSG_ERROR("Electron ID unavailable: " << m_eleID);
          return StatusCode::FAILURE;
        }
      }
    }
    // Workaround for derivations with "int" type ID flags.
    catch(const SG::ExcAuxTypeMismatch& e) {
      if(electron.isAvailable<int>(m_eleID)) {
        pass = electron.auxdata<int>(m_eleID);
      }
      else {
        throw;
      }
    }

    return StatusCode::SUCCESS;
  }

} // namespace ORUtils

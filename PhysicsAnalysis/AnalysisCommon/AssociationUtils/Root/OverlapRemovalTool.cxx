/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework includes
#include "CxxUtils/make_unique.h"

// Local includes
#include "AssociationUtils/OverlapRemovalTool.h"
#include "AssociationUtils/MacroChecks.h"


namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Standard constructor
  //---------------------------------------------------------------------------
  OverlapRemovalTool::OverlapRemovalTool(const std::string& name)
    : asg::AsgTool(name),
      m_eleEleORT("", this),
      m_eleMuORT("", this), m_eleJetORT("", this), m_muJetORT("", this),
      m_tauEleORT("", this), m_tauMuORT("", this), m_tauJetORT("", this),
      m_phoEleORT("", this), m_phoMuORT("", this), m_phoJetORT("", this),
      m_eleFatJetORT("", this), m_jetFatJetORT("", this)
  {
    declareProperty("EleEleORT", m_eleEleORT, "Electron-electron overlap tool");
    declareProperty("EleMuORT", m_eleMuORT, "Electron-muon overlap tool");
    declareProperty("EleJetORT", m_eleJetORT, "Electron-jet overlap tool");
    declareProperty("MuJetORT", m_muJetORT, "Muon-jet overlap tool");
    declareProperty("TauEleORT", m_tauEleORT, "Tau-electron overlap tool");
    declareProperty("TauMuORT", m_tauMuORT, "Tau-muon overlap tool");
    declareProperty("TauJetORT", m_tauJetORT, "Tau-jet overlap tool");
    declareProperty("PhoEleORT", m_phoEleORT, "Photon-electron overlap tool");
    declareProperty("PhoMuORT", m_phoMuORT, "Photon-muon overlap tool");
    declareProperty("PhoJetORT", m_phoJetORT, "Photon-jet overlap tool");
    declareProperty("EleFatJetORT", m_eleFatJetORT, "electron-fatjet overlap tool");
    declareProperty("JetFatJetORT", m_jetFatJetORT, "jet-fatjet overlap tool");

    // Input/output labels for reset functionality.
    declareProperty("InputLabel", m_inputLabel = "selected",
                    "Decoration which specifies input objects");
    declareProperty("OutputLabel", m_outputLabel = "overlaps",
                    "Decoration given to objects that fail OR");
    declareProperty("OutputPassValue", m_outputPassValue = false,
                    "Set the result assigned to objects that pass");
    declareProperty("RequireExpectedPointers", m_requireExpectedPointers = true,
                    "Require non-null container pointers when expected by config");
  }

  //---------------------------------------------------------------------------
  // Initialize the tool
  //---------------------------------------------------------------------------
  StatusCode OverlapRemovalTool::initialize()
  {
    ATH_MSG_DEBUG("Initializing master tool " << name());
    ATH_MSG_DEBUG("Master tool config: InputLabel " << m_inputLabel <<
                  " OutputLabel " << m_outputLabel <<
                  " OutputPassValue " << m_outputPassValue);

    // Initialize the decoration helper
    m_decHelper =
      CxxUtils::make_unique<OverlapDecorationHelper>
        (m_inputLabel, m_outputLabel, m_outputPassValue);

    // Retrieve the configured tools
    if(!m_eleEleORT.empty()) ATH_CHECK( m_eleEleORT.retrieve() );
    if(!m_eleMuORT.empty())  ATH_CHECK( m_eleMuORT.retrieve() );
    if(!m_eleJetORT.empty()) ATH_CHECK( m_eleJetORT.retrieve() );
    if(!m_muJetORT.empty())  ATH_CHECK( m_muJetORT.retrieve() );
    if(!m_tauEleORT.empty()) ATH_CHECK( m_tauEleORT.retrieve() );
    if(!m_tauMuORT.empty())  ATH_CHECK( m_tauMuORT.retrieve() );
    if(!m_tauJetORT.empty()) ATH_CHECK( m_tauJetORT.retrieve() );
    if(!m_phoEleORT.empty()) ATH_CHECK( m_phoEleORT.retrieve() );
    if(!m_phoMuORT.empty())  ATH_CHECK( m_phoMuORT.retrieve() );
    if(!m_phoJetORT.empty()) ATH_CHECK( m_phoJetORT.retrieve() );
    if(!m_eleFatJetORT.empty()) ATH_CHECK( m_eleFatJetORT.retrieve() );
    if(!m_jetFatJetORT.empty()) ATH_CHECK( m_jetFatJetORT.retrieve() );

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Remove all overlapping objects according to the configured tools
  //---------------------------------------------------------------------------
  StatusCode OverlapRemovalTool::
  removeOverlaps(const xAOD::ElectronContainer* electrons,
                 const xAOD::MuonContainer* muons,
                 const xAOD::JetContainer* jets,
                 const xAOD::TauJetContainer* taus,
                 const xAOD::PhotonContainer* photons,
                 const xAOD::JetContainer* fatJets) const
  {
    // Reset all decorations to passing
    if(electrons) m_decHelper->resetDecorations(*electrons);
    if(muons)     m_decHelper->resetDecorations(*muons);
    if(jets)      m_decHelper->resetDecorations(*jets);
    if(taus)      m_decHelper->resetDecorations(*taus);
    if(photons)   m_decHelper->resetDecorations(*photons);
    if(fatJets)   m_decHelper->resetDecorations(*fatJets);

    // Following recommended removal sequence from harmonization note.
    // NOTE: some details are still unspecific in the recommendations.
    // Talk to me if this does not support your needs.

    // Electron-electron OR
    ATH_CHECK( removeOverlap(m_eleEleORT, electrons, electrons) );
    // Tau-electron OR
    ATH_CHECK( removeOverlap(m_tauEleORT, taus, electrons));
    // Tau-muon OR
    ATH_CHECK( removeOverlap(m_tauMuORT, taus, muons) );
    // Electron-muon OR
    ATH_CHECK( removeOverlap(m_eleMuORT, electrons, muons) );
    // Pho-electron OR
    ATH_CHECK( removeOverlap(m_phoEleORT, photons, electrons) );
    // Pho-muon OR
    ATH_CHECK( removeOverlap(m_phoMuORT, photons, muons) );
    // Electron-jet OR
    ATH_CHECK( removeOverlap(m_eleJetORT, electrons, jets) );
    // Muon-jet OR
    ATH_CHECK( removeOverlap(m_muJetORT, muons, jets) );
    // Tau-jet OR
    ATH_CHECK( removeOverlap(m_tauJetORT, jets, taus) );
    // Pho-jet OR
    ATH_CHECK( removeOverlap(m_phoJetORT, jets, photons) );
    // Electron-fatjet OR
    ATH_CHECK( removeOverlap(m_eleFatJetORT, fatJets, electrons) );
    // jet-fatjet OR
    ATH_CHECK( removeOverlap(m_jetFatJetORT, jets, fatJets) );

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Perform overlap removal with one tool
  //---------------------------------------------------------------------------
  StatusCode OverlapRemovalTool::
  removeOverlap(const ToolHandle<IOverlapTool>& tool,
                const xAOD::IParticleContainer* cont1,
                const xAOD::IParticleContainer* cont2) const
  {
    if(!tool.empty()) {
      if(cont1 && cont2) {
        ATH_CHECK( tool->findOverlaps(*cont1, *cont2) );
      }
      else if(!cont1 && m_requireExpectedPointers) {
        ATH_MSG_ERROR("First container pointer for " << tool.name() << " is NULL!");
        return StatusCode::FAILURE;
      }
      else if(!cont2 && m_requireExpectedPointers) {
        ATH_MSG_ERROR("Second container pointer for " << tool.name() << " is NULL!");
        return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }

} // namespace ORUtils

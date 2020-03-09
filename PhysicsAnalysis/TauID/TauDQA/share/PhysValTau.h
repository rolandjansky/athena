///////////////////////// -*- C++ -*- /////////////////////////////
// PhysValTau.h 
// Header file for class PhysValTau
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TAUDQA_PHYSVALTAU_H
#define TAUDQA_PHYSVALTAU_H

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/ToolHandle.h"

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TauValidationPlots.h"
#include "TauAnalysisTools/ITauTruthMatchingTool.h"
#include "TauAnalysisTools/ITauSelectionTool.h"
#include <TLorentzVector.h>

//Additional includes
#include "RecoTypes.h"

// Forward declaration

//namespace PhysVal {

class PhysValTau
  : public ManagedMonitorToolBase
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  PhysValTau( const std::string& type,
		  const std::string& name, 
		  const IInterface* parent );

  /// Destructor: 
  virtual ~PhysValTau(); 

  // Athena algtool's Hooks
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();


  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 


  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  PhysValTau();

  // Containers
  //std::string m_TruthParticleContainerName; 
  std::string m_TauJetContainerName; 
  std::string m_TauDetailsContainerName; 
  std::string m_TruthParticleContainerName; 

  bool m_isMC;           //!< Are we running over MC data?
    
   //Variable Definitions
    
    /*keeps track of matched tau jets*/    
    std::vector<size_t> m_matched_itr;

   // Tool used for truth-matching
   ToolHandle<TauAnalysisTools::ITauTruthMatchingTool> m_truthTool;
   // Tool used to select "primitive" and "nominal" taus
   ToolHandle<TauAnalysisTools::ITauSelectionTool> m_primTauSel;
   ToolHandle<TauAnalysisTools::ITauSelectionTool> m_nomiTauSel;

  //Histograms
  // general tau all prongs plots
  std::unique_ptr<TauValidationPlots> m_oTauValidationPlots;
  // TauValidationPlots m_oTauValidationPlots;
    bool matchTrueAndRecoTau     (const xAOD::TauJetContainer *&taus, const xAOD::TruthParticle* trueTau, const xAOD::TauJet* &matchedRecoTau );
  
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//}
#endif //> !TAUDQA_PHYSVALTAU_H

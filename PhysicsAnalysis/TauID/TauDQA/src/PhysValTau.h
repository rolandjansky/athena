///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhysValTau.h 
// Header file for class PhysValTau
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TAUDQA_PHYSVALTAU_H
#define TAUDQA_PHYSVALTAU_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TauValidationPlots.h"
//#include "TauTools/TruthHandler.h"
//#include "TauTools/MatchingTools_depreciated.h"
#include <TLorentzVector.h>
//Additional includes

#include "TruthHandler.h"
//#include "TauTools/MatchingTools.h"


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
    
    //Variable Definitiona
    /** provides tools to match tau candidates to truth taus (from TauTools). */
    // MatchingTools matchtool;
    /** is a cut applied to the Delta R variable. */
    float m_DeltaRMatchCut;
    
    /** is a threshold (cut) applied the pT of the leading charged pion. */
    float m_LeadingChPiPt_Threshold;
    /** cuts used for truth matching to had taus*/
    float m_visETcut;
    float m_visEtacut;
    
    
    /** counts the number of truth taus. */
    int m_truth_all;
    
    /** counts the number of truth taus geometrically matched. */
    int m_truth_all_matched;
    
    /** counts the number of truth 1p taus. */
    int m_truth_1p;

    /** counts the number of truth taus geometrically matched and 1 track. */
    int m_truth_1p_matched;

    /** counts the number of truth 3p taus. */
    int m_truth_3p;
    
    /** counts the number of truth taus geometrically matched and 3 track. */
    int m_truth_3p_matched;
    
    
    /** Object to define Truth Handler Tool*/
    TruthHandler truthHandler;

  /** is a pointer to the truth particle container accessed. */              
  //const xAOD::TruthParticleContainer* mcpart;

  //Histograms
  // general tau all prongs plots
  std::unique_ptr<TauValidationPlots> m_oTauValidationPlots;
  // TauValidationPlots m_oTauValidationPlots;

  
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//}
Double_t DiffPhi(float dPhi);
Bool_t Cone(  Double_t eta_t,Double_t phi_t,Double_t eta_mc,Double_t phi_mc,Double_t Distance) ;
Double_t Cone(  Double_t eta_t,Double_t phi_t,Double_t eta_mc,Double_t phi_mc) ;
#endif //> !TAUDQA_PHYSVALTAU_H

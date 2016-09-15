/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Select_Bmumu.cxx, (c) ATLAS Detector software
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
// 
// Based on Select_onia2mumu.h.
// Original author: Daniel Scheirich <daniel.scheirich@cern.ch>
//
// Select B candidates for the B(s)mumu analysis including for
// the reference channels used.
//
// For an example see BPHY8.py .
//
// Job options provided by this class:
// - V0Tools                -- ToolHandle for V0Tools (default: Trk::V0Tools) 
// - HypothesisName         -- name given to the hypothesis (passed flag)
// - InputVtxContainerName  -- name of the input vertex container
// - TrkMasses"             -- list of masses to be assigned to the tracks
//                             used for lifetime calculation
//                             (Make sure to give them in correct order!)
//  - VtxMassHypo           -- mass used in the calculation of lifetime
//  - MassMin               -- minimum of mass range
//  - MassMax               -- maximum of mass range
//  - Chi2Max               -- maximum chi2 cut
//  - DoVertexType          -- bits defining vertex association types
//                             to be used
//  - BlindMassMin          -- minimum of blinded mass range
//  - BlindMassMax          -- maximum blinded mass range
//  - DoBlinding            -- switch to enable blinding (default: false)
//  - UseMuCalcMass         -- use MUCALC mass in mass cuts (default: false)
//                           
//============================================================================
//
#include "DerivationFrameworkBPhys/Select_Bmumu.h"

#include <vector>
#include <string>
#include "TVector3.h"

#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODBPhys/BPhysHypoHelper.h"

namespace DerivationFramework {

  Select_Bmumu::Select_Bmumu(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    CfAthAlgTool(t,n,p),
    m_v0Tools("Trk::V0Tools") {
    
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    // Declare tools    
    declareProperty("V0Tools", m_v0Tools);

    // Declare user-defined properties
    
    declareProperty("HypothesisName"       , m_hypoName              = "A");
    declareProperty("InputVtxContainerName", m_inputVtxContainerName = "JpsiCandidates");
    declareProperty("TrkMasses"            , m_trkMasses             = std::vector<double>(2, 105.658) );    
    declareProperty("VtxMassHypo"          , m_massHypo              = 3096.916 );                  
    declareProperty("MassMax"              , m_massMax               = 6000);                   
    declareProperty("MassMin"              , m_massMin               = 2000);                   
    declareProperty("Chi2Max"              , m_chi2Max               = 200);
    declareProperty("DoVertexType"         , m_DoVertexType          = 7);

    declareProperty("BlindMassMin"         , m_blindMassMin          = 0.);
    declareProperty("BlindMassMax"         , m_blindMassMax          = 0.);
    declareProperty("DoBlinding"           , m_doBlinding            = false);
    declareProperty("UseMuCalcMass"        , m_useMuCalcMass         = false);
  }
  //----------------------------------------------------------------------------
  StatusCode Select_Bmumu::initialize() {
  
    ATH_MSG_DEBUG("in initialize()");
    
    // retrieve V0 tools
    CHECK( m_v0Tools.retrieve() );
    
    return StatusCode::SUCCESS;
  }
  //----------------------------------------------------------------------------
  StatusCode Select_Bmumu::finalize() {

    // everything all right
    return StatusCode::SUCCESS;
  }
  //----------------------------------------------------------------------------
  void Select_Bmumu::ProcessVertex(xAOD::BPhysHypoHelper &bcand,
				   xAOD::BPhysHelper::pv_type pv_t) const {

      const xAOD::Vertex* pv = bcand.pv(pv_t); 
      if (pv) {
        // decorate the vertex. 
        // Proper decay time assuming constant mass hypothesis m_massHypo
        BPHYS_CHECK( bcand.setTau( m_v0Tools->tau(bcand.vtx(), pv,  m_massHypo), 
                                  pv_t,
                                  xAOD::BPhysHypoHelper::TAU_CONST_MASS) );
        // Proper decay time assuming error constant mass hypothesis m_massHypo
        BPHYS_CHECK( bcand.setTauErr( m_v0Tools->tauError(bcand.vtx(), pv,
							 m_massHypo), 
                                     pv_t,
                                     xAOD::BPhysHypoHelper::TAU_CONST_MASS) );
        
        BPHYS_CHECK( bcand.setTau( m_v0Tools->tau(bcand.vtx(), pv,  m_trkMasses), 
                                  pv_t,
                                  xAOD::BPhysHypoHelper::TAU_INV_MASS) );

        BPHYS_CHECK( bcand.setTauErr( m_v0Tools->tauError(bcand.vtx(), pv,
							 m_trkMasses), 
                                     pv_t,
                                     xAOD::BPhysHypoHelper::TAU_INV_MASS) );       
        
        //enum pv_type {PV_MAX_SUM_PT2, PV_MIN_A0, PV_MIN_Z0};
      } else {
      
        const float errConst = -9999999;
        BPHYS_CHECK( bcand.setTau(errConst, pv_t,
                                  xAOD::BPhysHypoHelper::TAU_CONST_MASS) );
        // Proper decay time assuming error constant mass hypothesis m_massHypo
        BPHYS_CHECK( bcand.setTauErr( errConst, 
                                     pv_t,
                                     xAOD::BPhysHypoHelper::TAU_CONST_MASS) );
        
        BPHYS_CHECK( bcand.setTau( errConst, 
                                  pv_t,
                                  xAOD::BPhysHypoHelper::TAU_INV_MASS) );

        BPHYS_CHECK( bcand.setTauErr( errConst, 
                                     pv_t,
                                     xAOD::BPhysHypoHelper::TAU_INV_MASS) );
    }
  } 
  //----------------------------------------------------------------------------
  StatusCode Select_Bmumu::addBranches() const {

    // Jpsi container and its auxilliary store
    xAOD::VertexContainer*    bcandContainer = NULL;
    xAOD::VertexAuxContainer* bcandAuxContainer = NULL;
    
    // retrieve from the StoreGate
    CHECK(evtStore()->retrieve(bcandContainer, m_inputVtxContainerName));
    CHECK(evtStore()->retrieve(bcandAuxContainer,
			       m_inputVtxContainerName+"Aux."));
    
    bool doPt = (m_DoVertexType & 1) != 0;
    bool doA0 = (m_DoVertexType & 2) != 0;
    bool doZ0 = (m_DoVertexType & 4) != 0;

    // loop over B candidates and perform selection and augmentation
    // counters
    int nPassMassCuts = 0;
    int nPassChi2Cut  = 0;
    xAOD::VertexContainer::iterator bcandItr = bcandContainer->begin();
    for (; bcandItr!=bcandContainer->end(); ++bcandItr) {
      // create BPhysHypoHelper
      xAOD::BPhysHypoHelper bcand(m_hypoName, *bcandItr);
      
      //----------------------------------------------------
      // decorate the vertex
      //----------------------------------------------------
      // a) invariant mass and error
      if ( !bcand.setMass(m_trkMasses) )
	ATH_MSG_WARNING("Decoration bcand.setMass failed");
      
      double massErr = m_v0Tools->invariantMassError(bcand.vtx(), m_trkMasses);
      if ( !bcand.setMassErr(massErr) )
	ATH_MSG_WARNING("Decoration bcand.setMassErr failed");
      
      // b) proper decay time and error: 
      // retrieve the refitted PV (or the original one,
      // if the PV refitting was turned off)
      if (doPt) ProcessVertex(bcand, xAOD::BPhysHelper::PV_MAX_SUM_PT2);
      if (doA0) ProcessVertex(bcand, xAOD::BPhysHelper::PV_MIN_A0);
      if (doZ0) ProcessVertex(bcand, xAOD::BPhysHelper::PV_MIN_Z0);
      
      //----------------------------------------------------
      // perform the selection (i.e. flag the vertex)
      //----------------------------------------------------
      // flag the vertex indicating that it is selected by this selector
      bcand.setPass(true);
      
      // now we check other cuts. if one of them didn't pass, set the flag to 0
      // and continue to the next candidate:
      
      // 1) invariant mass cuts
      bool passedMuCalcMassCut(m_useMuCalcMass);
      if ( m_useMuCalcMass ) {
	std::string bname = m_hypoName+"_MUCALC_mass";
	static SG::AuxElement::Accessor<float> mucalcAcc(bname);
	if ( mucalcAcc.isAvailable(**bcandItr) ) {
	  passedMuCalcMassCut = massCuts(mucalcAcc(**bcandItr));
	} else {
	  passedMuCalcMassCut = false;
	  ATH_MSG_WARNING("MUCALC mass not available: " << bname << " !");
	}
      }
      bool passedMassCut = massCuts(bcand.mass());
      if ( !(passedMassCut || passedMuCalcMassCut) ) {
        bcand.setPass(false); // flag as failed
        continue;
      }
      nPassMassCuts++;
      
      // 2) chi2 cut
      if ( bcand.vtx()->chiSquared() > m_chi2Max) {
        bcand.setPass(false);; // flag as failed
        continue;
      }
      nPassChi2Cut++;
      
    } // end of loop over bcand candidates

    // counters
    // event level
    addEvent("allEvents");
    if ( bcandContainer->size() > 0 ) addEvent("eventsWithCands");
    if ( nPassMassCuts > 0 )          addEvent("massCutEvents");
    if ( nPassChi2Cut  > 0 )          addEvent("chi2CutEvents");
    // candidate level
    addToCounter("allCandidates"    , bcandContainer->size());
    addToCounter("massCutCandidates", nPassMassCuts);
    addToCounter("chi2CutCandidates", nPassChi2Cut);
    
    // all OK
    return StatusCode::SUCCESS;
  }  
  //----------------------------------------------------------------------------
  // Check whether mass cuts (including a possibly blinding region cut)
  // are passed.
  //----------------------------------------------------------------------------
  bool Select_Bmumu::massCuts(float mass) const {
 
    return (mass > m_massMin && mass < m_massMax)
      && !(m_doBlinding && mass > m_blindMassMin && mass < m_blindMassMax ); 
  }
  //----------------------------------------------------------------------------

} // namespace DerivationFramework

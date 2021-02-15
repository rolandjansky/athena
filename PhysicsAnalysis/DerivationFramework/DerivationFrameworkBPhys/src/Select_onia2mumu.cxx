/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// Select_onia2mumu.cxx
///////////////////////////////////////////////////////////////////
// Author: Daniel Scheirich <daniel.scheirich@cern.ch>
// Based on the Integrated Simulation Framework
//
// Basic Jpsi->mu mu derivation example

#include "DerivationFrameworkBPhys/Select_onia2mumu.h"

#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "xAODBPhys/BPhysHypoHelper.h"

#include <vector>
#include <string>

namespace DerivationFramework {

  Select_onia2mumu::Select_onia2mumu(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_v0Tools("Trk::V0Tools")
  {
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
    declareProperty("LxyMin"               , m_lxyMin                = std::numeric_limits<double>::lowest());
    declareProperty("Do3d"                 , m_do3d = false);
    
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode Select_onia2mumu::initialize()
  {
  
    ATH_MSG_DEBUG("in initialize()");
    
    // retrieve V0 tools
    CHECK( m_v0Tools.retrieve() );
    ATH_CHECK(m_inputVtxContainerName.initialize());

    return StatusCode::SUCCESS;
    
  }
  
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  void Select_onia2mumu::ProcessVertex(xAOD::BPhysHypoHelper &onia, xAOD::BPhysHelper::pv_type pv_t) const{
      constexpr float errConst = -9999999;
      const xAOD::Vertex* pv = onia.pv(pv_t); 
      if(pv) {
        // decorate the vertex. 
        // Proper decay time assuming constant mass hypothesis m_massHypo
        BPHYS_CHECK( onia.setTau( m_v0Tools->tau(onia.vtx(), pv,  m_massHypo), 
                                  pv_t,
                                  xAOD::BPhysHypoHelper::TAU_CONST_MASS) );
        // Proper decay time assuming error constant mass hypothesis m_massHypo
        BPHYS_CHECK( onia.setTauErr( m_v0Tools->tauError(onia.vtx(), pv,  m_massHypo), 
                                     pv_t,
                                     xAOD::BPhysHypoHelper::TAU_CONST_MASS) );
        
        BPHYS_CHECK( onia.setTau( m_v0Tools->tau(onia.vtx(), pv,  m_trkMasses), 
                                  pv_t,
                                  xAOD::BPhysHypoHelper::TAU_INV_MASS) );

        BPHYS_CHECK( onia.setTauErr( m_v0Tools->tauError(onia.vtx(), pv,  m_trkMasses), 
                                     pv_t,
                                     xAOD::BPhysHypoHelper::TAU_INV_MASS) );       
        
        //enum pv_type {PV_MAX_SUM_PT2, PV_MIN_A0, PV_MIN_Z0, PV_MIN_Z0_BA};
      }else{
      

        BPHYS_CHECK( onia.setTau(errConst, pv_t,
                                  xAOD::BPhysHypoHelper::TAU_CONST_MASS) );
        // Proper decay time assuming error constant mass hypothesis m_massHypo
        BPHYS_CHECK( onia.setTauErr( errConst, 
                                     pv_t,
                                     xAOD::BPhysHypoHelper::TAU_CONST_MASS) );
        
        BPHYS_CHECK( onia.setTau( errConst, 
                                  pv_t,
                                  xAOD::BPhysHypoHelper::TAU_INV_MASS) );

        BPHYS_CHECK( onia.setTauErr( errConst, 
                                     pv_t,
                                     xAOD::BPhysHypoHelper::TAU_INV_MASS) );        
    }

    if(m_do3d){
        BPHYS_CHECK( onia.setTau3d( pv ? m_v0Tools->tau3D(onia.vtx(), pv,  m_massHypo) : errConst, 
                                  pv_t,
                                  xAOD::BPhysHypoHelper::TAU_CONST_MASS) );
        // Proper decay time assuming error constant mass hypothesis m_massHypo
        BPHYS_CHECK( onia.setTau3dErr( pv ? m_v0Tools->tau3DError(onia.vtx(), pv,  m_massHypo) : errConst,
                                     pv_t,
                                     xAOD::BPhysHypoHelper::TAU_CONST_MASS) );
        
        BPHYS_CHECK( onia.setTau3d( pv ? m_v0Tools->tau3D(onia.vtx(), pv,  m_trkMasses) : errConst,
                                  pv_t,
                                  xAOD::BPhysHypoHelper::TAU_INV_MASS) );

        BPHYS_CHECK( onia.setTau3dErr( pv ? m_v0Tools->tau3DError(onia.vtx(), pv,  m_trkMasses) : errConst,
                                     pv_t,
                                     xAOD::BPhysHypoHelper::TAU_INV_MASS) );    

    }

  }
  
  
  StatusCode Select_onia2mumu::addBranches() const
  {
    
    SG::ReadHandle<xAOD::VertexContainer> oniaContainer(m_inputVtxContainerName);

    bool doPt   = (m_DoVertexType & 1) != 0;
    bool doA0   = (m_DoVertexType & 2) != 0;
    bool doZ0   = (m_DoVertexType & 4) != 0;
    bool doZ0BA = (m_DoVertexType & 8) != 0;
    // loop over onia candidates and perform selection and augmentation
    xAOD::VertexContainer::const_iterator oniaItr = oniaContainer->begin();
    for(; oniaItr!=oniaContainer->end(); ++oniaItr) {
      // create BPhysHypoHelper
      xAOD::BPhysHypoHelper onia(m_hypoName, *oniaItr);
      if((*oniaItr)->nTrackParticles() != m_trkMasses.size())
          ATH_MSG_WARNING("Vertex has " << (*oniaItr)->nTrackParticles() << " while provided masses " << m_trkMasses.size());
      //----------------------------------------------------
      // decorate the vertex
      //----------------------------------------------------
      // a) invariant mass and error
      if( !onia.setMass(m_trkMasses) ) ATH_MSG_WARNING("Decoration onia.setMass failed");
      
      double massErr = m_v0Tools->invariantMassError(onia.vtx(), m_trkMasses);
      if( !onia.setMassErr(massErr) ) ATH_MSG_WARNING("Decoration onia.setMassErr failed");
      
      // b) proper decay time and error: 
      // retrieve the refitted PV (or the original one, if the PV refitting was turned off)
      if(doPt)   ProcessVertex(onia, xAOD::BPhysHelper::PV_MAX_SUM_PT2);
      if(doA0)   ProcessVertex(onia, xAOD::BPhysHelper::PV_MIN_A0);
      if(doZ0)   ProcessVertex(onia, xAOD::BPhysHelper::PV_MIN_Z0);
      if(doZ0BA) ProcessVertex(onia, xAOD::BPhysHelper::PV_MIN_Z0_BA);
      
      //----------------------------------------------------
      // perform the selection (i.e. flag the vertex)
      //----------------------------------------------------
      // flag the vertex indicating that it is selected by this selector
      onia.setPass(true);
      
      // now we check othe cuts. if one of them didn't pass, set the flag to 0
      // and continue to the next candidate:
      
      // 1) invariant mass cut
      if( onia.mass() < m_massMin || onia.mass() > m_massMax) {
        onia.setPass(false); // flag as failed
        continue;
      }

      // 2) chi2 cut
      if( onia.vtx()->chiSquared() > m_chi2Max) {
        onia.setPass(false);; // flag as failed
        continue;
      }
      // 3) lxy cut
      if( onia.lxy(xAOD::BPhysHelper::PV_MAX_SUM_PT2) < m_lxyMin) {
        onia.setPass(false);; // flag as failed
        continue;
      }

    } // end of loop over onia candidates
    
    // all OK
    return StatusCode::SUCCESS;
  }  
  
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  
}

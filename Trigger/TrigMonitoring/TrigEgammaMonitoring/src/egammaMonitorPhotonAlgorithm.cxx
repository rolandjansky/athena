#include "egammaMonitorPhotonAlgorithm.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "xAODPrimitives/IsolationType.h"
#include "StoreGate/ReadCondHandleKey.h"


egammaMonitorPhotonAlgorithm::egammaMonitorPhotonAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ):
  AthAlgorithm( name, pSvcLocator )

{}

StatusCode egammaMonitorPhotonAlgorithm::initialize() 
{
  
  ATH_CHECK(m_photonsKey.initialize());
  if (!m_monTool.empty()) CHECK(m_monTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode egammaMonitorPhotonAlgorithm::execute_r(const EventContext& ctx) const
{
  
  filltopoPhotonShowerShapes(ctx);
  filltopoPhotonIsolation(ctx);
  return StatusCode::SUCCESS;
}


void egammaMonitorPhotonAlgorithm::filltopoPhotonShowerShapes( const EventContext& ctx) const{

    SG::ReadHandle<xAOD::PhotonContainer> photons(m_photonsKey, ctx);
    ATH_MSG_DEBUG("Fill SS Reco Photon distributions: ");
    
    std::vector<float> e237_vec, e277_vec, ethad_vec, ethad1_vec, Rhad_vec, Rhad1_vec, Reta_vec, Rphi_vec, weta1_vec, weta2_vec, wtots1_vec,
      f1_vec, f3_vec,f3core_vec, fracs1_vec, eratio_vec, DeltaE_vec, et_vec, highet_vec , eta_vec, phi_vec;
    float val{-99};
    
    auto e237_col               = Monitored::Collection("e237"    , e237_vec );
    auto e277_col               = Monitored::Collection("e277"    , e277_vec );
    auto ethad_col              = Monitored::Collection("ethad"    , ethad_vec );
    auto ethad1_col             = Monitored::Collection("ethad1"   , ethad1_vec );
    auto Rhad_col               = Monitored::Collection("Rhad"     , Rhad_vec    );
    auto Rhad1_col              = Monitored::Collection("Rhad1"    , Rhad1_vec   );
    auto Reta_col               = Monitored::Collection("Reta"     , Reta_vec    );
    auto Rphi_col               = Monitored::Collection("Rphi"     , Rphi_vec    );
    auto weta1_col              = Monitored::Collection("weta1"    , weta1_vec   );
    auto weta2_col              = Monitored::Collection("weta2"    , weta2_vec   );
    auto wtots1_col             = Monitored::Collection("wtots1"   , wtots1_vec   );
    auto f1_col                 = Monitored::Collection("f1"       , f1_vec      );
    auto f3_col                 = Monitored::Collection("f3"       , f3_vec      );
    auto f3core_col             = Monitored::Collection("f3core"   , f3core_vec  );
    auto fracs1_col             = Monitored::Collection("fracs1"   , fracs1_vec  );
    auto eratio_col             = Monitored::Collection("eratio"   , eratio_vec  );
    auto DeltaE_col             = Monitored::Collection("deltaE"   , DeltaE_vec  );
    
    for (const auto& photon : *photons) {
    
        e237_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::e237));
        e277_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::e237));
        ethad_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::ethad));
        ethad1_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::ethad1));
        Rhad_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::Rhad));
        Rhad1_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::Rhad1));
        Reta_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::Reta));
        Rphi_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::Rphi));
        weta1_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::weta1));
        weta2_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::weta2));
        wtots1_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::wtots1));
        f1_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::f1));
        f3_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::f3));
        f3core_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::f3core));
        fracs1_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::fracs1));
        eratio_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::Eratio));
        DeltaE_vec.push_back(photon->showerShapeValue(val,xAOD::EgammaParameters::DeltaE));
        

    }
    auto mon = Monitored::Group(m_monTool, e237_col, e277_col,ethad_col, ethad1_col, Rhad_col, Rhad1_col,Reta_col,Rphi_col,weta1_col,weta2_col,wtots1_col,f1_col,f3_col,f3core_col,fracs1_col,eratio_col,DeltaE_col);
    ATH_MSG_DEBUG("Photon - ShowerShapes Online Monitoring in Reconstruction ..."); 
    
  }

void egammaMonitorPhotonAlgorithm::filltopoPhotonIsolation( const EventContext& ctx) const{


    SG::ReadHandle<xAOD::PhotonContainer> photons(m_photonsKey, ctx);
    std::vector<float> topoetcone20_vec, topoetcone40_shift_vec, topoetcone20_rel_vec, topoetcone40_shift_rel_vec, ptcone20_vec, ptcone30_vec, ptcone40_vec;
    float val{-99};

    auto topoetcone20_col       = Monitored::Collection("topoetcone20", topoetcone20_vec);   
    auto ptcone20_col = Monitored::Collection("ptcone20", ptcone20_vec);
    auto ptcone30_col = Monitored::Collection("ptcone30", ptcone30_vec);
    auto ptcone40_col = Monitored::Collection("ptcone40", ptcone40_vec);

    
    for (const auto& photon : *photons) {

        topoetcone20_vec.push_back( photon->isolationValue(val,xAOD::Iso::topoetcone20)/Gaudi::Units::GeV);
        ptcone20_vec.push_back(photon->isolationValue(val,xAOD::Iso::ptcone20));
        ptcone30_vec.push_back(photon->isolationValue(val,xAOD::Iso::ptcone30));
        ptcone40_vec.push_back(photon->isolationValue(val,xAOD::Iso::ptcone40));


    }

    auto mon = Monitored::Group(m_monTool,ptcone20_col, ptcone30_col, ptcone40_col, topoetcone20_col);
    
    ATH_MSG_DEBUG("Photon - Isolation Online Monitoring in Reconstruction ..."); 
  
  
  }

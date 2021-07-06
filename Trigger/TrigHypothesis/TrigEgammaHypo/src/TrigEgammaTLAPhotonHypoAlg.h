#ifndef TrigEgammaTLAPhotonHypoAlg_H
#define TrigEgammaTLAPhotonHypoAlg_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "DecisionHandling/HypoBase.h"

#include "TrigEgammaTLAPhotonHypoTool.h"

#include "xAODEgamma/PhotonContainer.h"



class TrigEgammaTLAPhotonHypoAlg : public HypoBase {

  public:
    TrigEgammaTLAPhotonHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );
    
    

    virtual StatusCode initialize() override;
    virtual StatusCode execute( const EventContext& context ) const override;


  private:
    ToolHandleArray<TrigEgammaTLAPhotonHypoTool> m_hypoTools {
      this, "HypoTools", {}, "Hypo Tools"
    };

    
    SG::ReadHandleKey< xAOD::PhotonContainer > m_TLAPhotonsKey {
      this, "Photons", "Photons", "TLA Photon container key"
    };

    SG::ReadHandleKey< xAOD::PhotonContainer > m_OriginalPhotonsKey {
      this, "OriginalPhotons", "OriginalPhotons", "Original Photon container key"
    };


};

#endif

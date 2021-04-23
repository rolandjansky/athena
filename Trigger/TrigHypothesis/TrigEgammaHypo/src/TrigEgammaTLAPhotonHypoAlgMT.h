#ifndef TrigEgammaTLAPhotonHypoAlgMT_H
#define TrigEgammaTLAPhotonHypoAlgMT_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "DecisionHandling/HypoBase.h"

#include "TrigEgammaTLAPhotonHypoTool.h"

#include "xAODEgamma/PhotonContainer.h"



class TrigEgammaTLAPhotonHypoAlgMT : public HypoBase {

  public:
    TrigEgammaTLAPhotonHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );
    
    

    virtual StatusCode initialize() override;
    virtual StatusCode execute( const EventContext& context ) const override;


  private:
    ToolHandleArray<TrigEgammaTLAPhotonHypoTool> m_hypoTools {
      this, "HypoTools", {}, "Hypo Tools"
    };

    // for the moment use TrigPhoton, but I need to understand if this should actually be just PhotonContainer
    SG::ReadHandleKey< xAOD::PhotonContainer > m_TLAPhotonsKey {
      this, "Photons", "Photons", "TLA Photon container key"
    };


};

#endif

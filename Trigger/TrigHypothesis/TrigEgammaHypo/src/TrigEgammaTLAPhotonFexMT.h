/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIG_TrigEgammaTLAPhoton_FexMT_H
#define TRIG_TrigEgammaTLAPhoton_FexMT_H


#include "Gaudi/Property.h"

// athena stuff
#include "GaudiKernel/ToolHandle.h"

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// trigger EDM
#include "xAODTrigEgamma/TrigPhoton.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"


class TrigEgammaTLAPhotonFexMT : public AthAlgorithm {

  public:
    TrigEgammaTLAPhotonFexMT(const std::string & name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;


  private:
    // declaration of the object keys necessary for accessing data
    // This initializer list syntax also informs the scheduler of the
    // data dependency, obviating the need for a separate declareProperty(...)

    SG::ReadHandleKey < xAOD::TrigPhotonContainer > m_fastPhotonsKeys{
      this,
      "TLAInputPhotonContainer",
      "Photons",
      "input Photon container name"
    };

    SG::WriteHandleKey < xAOD::TrigPhotonContainer > m_TLAOutPhotonsKey{
      this,
      "TLAOutputPhotonContainer",
      "TLAPhotons",
      "output Photon container name"
    };

    Gaudi::Property<float> m_photonPtThreshold {
      this,
      "photonPtThreshold",                                 // name
      0. ,                                                 // default value
      "Threshold for saving Photons in TLA container"
    };

    Gaudi::Property<int> m_maxNPhotons {
      this,
      "maxNPhotons",
      -1 ,
      "Maximum number of Photons saved in TLA container"
    };


};

#endif

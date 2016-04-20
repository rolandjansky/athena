/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_BKGELECTRONCLASSIFICATION_H
#define DERIVATIONFRAMEWORK_BKGELECTRONCLASSIFICATION_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

class IMCTruthClassifier;

namespace DerivationFramework {

  class BkgElectronClassification : public AthAlgTool, public IAugmentationTool {
    public: 
      BkgElectronClassification(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:

      /** @brief Name of the input electron container **/
      std::string m_electronContainerName;
      /** @brief Name of the truth particle container **/
      std::string m_truthParticleContainerName;
      /** @brief MCTruthClassifier **/
      ToolHandle<IMCTruthClassifier>   m_mcTruthClassifier;
      /** @brief barcode cut for egamma helpers **/
      int m_barcodecut;

  }; 
}

#endif // DERIVATIONFRAMEWORK_BKGELECTRONCLASSIFICATION_H

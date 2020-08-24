/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TRUTHCALOSHOWERDECORATOR_H
#define DERIVATIONFRAMEWORK_TRUTHCALOSHOWERDECORATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloCalibHitRec/CalibHitToCaloCellTool.h"
#include <vector>


namespace DerivationFramework {

  class TruthCaloShowerDecorator : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthCaloShowerDecorator(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:

      /** @brief Name of the input electron container **/
      std::string m_electronContainerName;
      /** @brief Name of the truth particle container **/
      std::string m_truthParticleContainerName;
      /** @brief barcode cut for egamma helpers **/
      int m_singleParticleBarcode;

      ToolHandle<CalibHitToCaloCellTool> m_calibhitToCaloCellTool; 
      //std::vector<std::string> m_truthClusterContainerNames;

  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHCALOSHOWERDECORATOR_H

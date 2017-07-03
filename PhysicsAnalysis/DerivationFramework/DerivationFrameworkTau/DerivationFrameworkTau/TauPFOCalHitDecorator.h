/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TauPFOCalHitDecorator.h
 * @author W. Davey will.davey@cern.ch
 * @date September 2016
 */

#ifndef DERIVATIONFRAMEWORK_TAUPFOCALHITDECORATOR_H
#define DERIVATIONFRAMEWORK_TAUPFOCALHITDECORATOR_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
//#include "GaudiKernel/ToolHandle.h"

/**
 * @class TauPFOCalHitDecorator
 * @brief decorate calhit summary information onto neutral pfos 
 */

class CaloCalibrationHitContainer;

namespace DerivationFramework {

  class TauPFOCalHitDecorator : public AthAlgTool, public IAugmentationTool {

    public: 
      TauPFOCalHitDecorator(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_tauContainerName;

      void addCalHitContainer(
        const char* name, 
        std::vector<const CaloCalibrationHitContainer*>& vec) const;

  }; 
}

#endif

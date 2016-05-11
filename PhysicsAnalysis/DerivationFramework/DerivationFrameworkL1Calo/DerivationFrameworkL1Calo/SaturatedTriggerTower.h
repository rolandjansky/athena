/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SaturatedTriggerTower.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_SATURATEDTRIGGERTOWER_H
#define DERIVATIONFRAMEWORK_SATURATEDTRIGGERTOWER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

namespace DerivationFramework {

  class SaturatedTriggerTower : public AthAlgTool, public ISkimmingTool {
    public: 
      SaturatedTriggerTower(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual bool eventPassesFilter() const;

    private:
      int m_adcThreshold;
      std::string m_collName;
  }; 
}

#endif // DERIVATIONFRAMEWORK_SATURATEDTRIGGERTOWER_H

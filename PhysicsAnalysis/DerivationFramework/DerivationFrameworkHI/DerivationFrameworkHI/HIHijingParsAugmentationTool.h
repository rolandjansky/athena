/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//// HIHijingParsAugmentationTool.h
/////////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_HIHijingParsAugmentationTool_H
#define DERIVATIONFRAMEWORK_HIHijingParsAugmentationTool_H
 
#include <string>
 
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace DerivationFramework {
 
  class HIHijingParsAugmentationTool : public AthAlgTool, public IAugmentationTool {
    public:
      HIHijingParsAugmentationTool(const std::string& t, const std::string& n, const IInterface* p);
      virtual StatusCode addBranches() const;
    private:
      int m_nhar;
      std::string m_HijingEventParamsKey;
      std::string m_EventInfo_key;
  };
}
 
#endif // DERIVATIONFRAMEWORK_HIHijingParsAugmentationTool_H

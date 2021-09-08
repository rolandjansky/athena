/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthHSDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRUTHHSDECORATOR_H
#define DERIVATIONFRAMEWORK_TRUTHHSDECORATOR_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  /** This decorator is applied to all truth particles, indicating
   *    whether or not they come from the hard scatter in the event.
   *    This is useful for cases where MC has full pileup truth.
   */
  class TruthHSDecorator : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthHSDecorator(const std::string& t, const std::string& n, const IInterface* p);
      virtual StatusCode addBranches() const;

    private:
      //configurables
      std::string m_truthEventKey; 
      std::string m_truthParticleKey; 
      std::string m_decorationName;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TruthHSDecorator_H

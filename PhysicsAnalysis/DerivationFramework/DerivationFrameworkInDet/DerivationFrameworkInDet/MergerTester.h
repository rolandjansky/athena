/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MergerTester.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_MergerTester_H
#define DERIVATIONFRAMEWORK_MergerTester_H

#include <string>
#include <map>
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "ExpressionEvaluation/ExpressionParserUser.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"

namespace DerivationFramework {

  /** @brief Class-algorithm for track particle collection merging*/
  class MergerTester : public AthAlgTool, public IAugmentationTool 
    {
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      /** @brief Standard Algotithm methods:                           */
      ///////////////////////////////////////////////////////////////////

      MergerTester(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~MergerTester() {}
      StatusCode initialize();
      static StatusCode execute();
      StatusCode finalize();
      virtual StatusCode addBranches() const;
 
    protected:

      ///////////////////////////////////////////////////////////////////
      /** @brief Protected data:                                       */
      ///////////////////////////////////////////////////////////////////
      SG::ReadHandleKey<xAOD::TrackParticleContainer>      m_inputParticleLocation; /** Vector of track collections to be merged. */

      ///////////////////////////////////////////////////////////////////
      /** @brief Protected methods:                                    */
      ///////////////////////////////////////////////////////////////////

    private:
    };
    
}
#endif // DERIVATIONFRAMEWORK_MergerTester_H

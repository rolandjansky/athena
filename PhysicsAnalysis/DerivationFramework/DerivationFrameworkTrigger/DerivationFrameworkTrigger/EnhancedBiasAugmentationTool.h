/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnhancedBiasAugmentationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_ENHANCEDBIASAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_ENHANCEDBIASAUGMENTATIONTOOL_H

#include <string>
#include <map>
#include <cstdint>

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "EnhancedBiasWeighter/ReadLumiBlock.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

namespace DerivationFramework {

  class EnhancedBiasAugmentationTool : public AthAlgTool, public IAugmentationTool {
    public: 
      EnhancedBiasAugmentationTool(const std::string& t, const std::string& n, const IInterface* p);
      virtual StatusCode addBranches() const;
      virtual StatusCode initialize();


    private:
      void loadWeights();
      void loadLumi();
      template <class T> T stringToNum(const std::string& i, T typeOfClass) const;

      ToolHandle<Trig::IBunchCrossingTool> m_bcTool;

      int32_t                        m_runNumber; 
      std::map<int32_t, uint32_t>    m_eventNumberToIdMap;    //!< Used to cache & map EventNumber to weight-ID
      std::map<uint32_t, float>      m_idToWeightMap;         //!< Cache & map weight-ID to weight. TODO, move to 46bit, but also need to update some XMLs
      std::map<uint32_t, char>       m_idToUnbiasedMap;       //!< Cache & map weight-ID to unbiased flag
      mutable ReadLumiBlock          m_readLumiBlock;         //!< Cache lumi block lengths TODO - not actually modified, but without a const accessor
      std::map<uint32_t, uint32_t>   m_eventsPerLB;           //!< Cache & map enhanced bias events per LB
      std::map<uint32_t, char>       m_goodLB;                //!< Cache & map "good run list" like mechanism for rates
  }; 
}

#endif // DERIVATIONFRAMEWORK_ENHANCEDBIASAUGMENTATIONTOOL_H

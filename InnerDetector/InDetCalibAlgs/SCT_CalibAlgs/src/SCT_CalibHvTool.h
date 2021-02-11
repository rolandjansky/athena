/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibHvTool.h
 * Header file for the SCT_CalibHvTool class
 * @author Shaun Roe
 **/

#ifndef SCT_CalibHvTool_h
#define SCT_CalibHvTool_h

//Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

//Inner detector includes
#include "InDetIdentifier/SCT_ID.h"

//local includes
#include "SCT_CalibAlgs/ISCT_CalibHistoTool.h"
#include "SCT_ConditionsTools/ISCT_DCSConditionsTool.h" //template parameter

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

//STL includes
#include <string>
#include <vector>
#include <queue>
#include <utility> //for 'pair'

//fwd declarations
class StatusCode;
class ISvcLocator;
class IdentifierHash;
class EventInfo;

class SCT_CalibHvTool : public extends<AthAlgTool, ISCT_CalibHistoTool>
{

   public:
      //@name Tool methods, reimplemented
      //@{
      SCT_CalibHvTool(const std::string&, const std::string&, const IInterface*);
      virtual ~SCT_CalibHvTool() = default;
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      //@}
      //@name ISCT_CalibHistoTool methods, reimplemented
      //@{
      virtual bool book();
      virtual bool fill(const bool fromData=false);
      virtual bool fillFromData();
      //@}

   private:
      enum {n_chipPerSide = 6, n_chipPerModule = 12, n_stripPerChip = 128, n_etaInBarrel = 12,
            n_phiBinsB0 = 32, n_phiBinsB1 = 40, n_phiBinsB2 = 48, n_phiBinsB3 = 56,
            n_phiBinsECShort = 40, n_phiBinsECMiddle = 40, n_phiBinsECOuter = 52, n_elements=8176,
            n_BSErrorType = 15, firstBSErrorType = 0, lastBSErrorType = 14
           };

      ToolHandle<ISCT_DCSConditionsTool> m_DCSConditionsTool{this, "SCT_DCSConditionsTool", "SCT_DCSConditionsTool", "Tool to retrieve SCT DCS information"};

      const SCT_ID* m_pSCTHelper{nullptr};
      SCT_ID::const_id_iterator m_waferItrBegin;
      SCT_ID::const_id_iterator m_waferItrEnd;
      typedef std::vector<int> VecInt;
      VecInt* m_sct_waferHash{nullptr};
      VecInt* m_sct_numHitsInWafer{nullptr};

      //private use in this class
      int m_maxq{100};
      // For HV trips
      std::vector<std::queue<int>> m_phvtripQueue;
      std::queue<int> m_prevLBN;
      VecInt m_phvtripRunningTotalInt;
      VecInt m_phvtripProcessedEventsInt;
      VecInt m_phvtripHasItTripped;
      VecInt m_phvtripHasItTripped_prev;
      //
      int m_phvtripPrevTime{0};
      int m_phvtripFirstTime{0};
      double m_absolutetriplimit{0};
      double m_relativetriplimit{0};
      int m_tq[100] {0};

      std::vector<std::vector<std::pair<int, int>>> m_summarytrips;
      std::vector<std::vector<std::pair<int, int>>> m_summarytripslb;
      const EventInfo* m_evt{nullptr};
      bool m_outputLowHits{false};
      int m_lowHitCut{100};
      ///retrieve a tool and report if it failed
      template<class T>
      bool retrievedTool(T& tool, const std::string& toolName) const {
         if (tool.retrieve().isFailure()) {
            ATH_MSG_ERROR("Unable to retrieve " << toolName);
            return false;
         }
         return true;
      }

};
#endif

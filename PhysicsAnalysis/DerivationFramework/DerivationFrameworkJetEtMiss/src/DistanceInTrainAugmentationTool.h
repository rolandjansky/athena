/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DistanceInTrainAugmentationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Chris Young (christopher.young@cern.ch)
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_BCDISTANCEAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_BCDISTANCEAUGMENTATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"

#include "xAODEventInfo/EventInfo.h"

#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

namespace DerivationFramework {

  class DistanceInTrainAugmentationTool : public AthAlgTool, public IAugmentationTool {

    public: 
      DistanceInTrainAugmentationTool( const std::string& t, const std::string& n, const IInterface* p );

      ~DistanceInTrainAugmentationTool();

      StatusCode  initialize();
      StatusCode  finalize();

      virtual StatusCode addBranches() const;

    private:

      ToolHandle<Trig::IBunchCrossingTool> m_bcTool;

      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo_key{this, "EventInfo", "EventInfo", "Input event information"};
      
      SG::WriteDecorHandleKey<xAOD::EventInfo> m_BCIDDistanceFrontKey {this, "BCIDDistanceFrontKey", "EventInfo.DFCommonJets_BCIDDistanceFromFront", "Decoration for BCID distance from front"};
      SG::WriteDecorHandleKey<xAOD::EventInfo> m_BCIDDistanceTailKey {this, "BCIDDistanceTailKey", "EventInfo.DFCommonJets_BCIDDistanceTail" , "Decoration for BCID distance from tail"};
      SG::WriteDecorHandleKey<xAOD::EventInfo> m_BCIDGapBeforeTrainKey {this, "BCIDGapBeforeTrainKey", "EventInfo.DFCommonJets_BCIDGapBeforeTrain", "Decoration for BCID gap before train"};
      SG::WriteDecorHandleKey<xAOD::EventInfo> m_BCIDGapAfterTrainKey {this, "BCIDGapAfterTrainKey", "EventInfo.DFCommonJets_BCIDGapAfterTrain", "Decoration for BCID gap after train"};
      SG::WriteDecorHandleKey<xAOD::EventInfo> m_BCIDTypeKey {this, "BCIDTypeKey", "EventInfo.DFCommonJets_BCIDType", "Decoration for BCID type"};
      SG::WriteDecorHandleKey<xAOD::EventInfo> m_BCIDGapBeforeTrainMinus12Key {this, "BCIDGapBeforeTrainMinus12Key", "EventInfo.DFCommonJets_BCIDGapBeforeTrainMinus12", "Decoration for BCID gap before train minus 12 BCIDs"};
      SG::WriteDecorHandleKey<xAOD::EventInfo> m_BCIDGapAfterTrainMinus12Key {this, "BCIDGapAfterTrainMinus12Key", "EventInfo.DFCommonJets_BCIDGapAfterTrainMinus12", "Decoration for BCID gap after train minus 12 BCIDs"};
      SG::WriteDecorHandleKey<xAOD::EventInfo> m_BCIDTypeMinus12Key {this, "BCIDTypeMinus12Key", "EventInfo.DFCommonJets_BCIDTypeMinus12", "Decoration for BCID type minus 12 BCIDs"};

  }; 

}

#endif


// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSALGORITHMS_SIDETECTORELEMENTINFOCONDALG_H
#define PIXELCONDITIONSALGORITHMS_SIDETECTORELEMENTINFOCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/WriteHandleKey.h"

#include "InDetConditionsSummaryService/IDetectorElementStatusTool.h"
#include "InDetReadoutGeometry/SiDetectorElementStatus.h"


namespace InDet {
   class SiDetectorElementStatusCondAlg : public AthReentrantAlgorithm
   {
   public:
      SiDetectorElementStatusCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
      virtual ~SiDetectorElementStatusCondAlg() override = default;

      virtual StatusCode initialize() override;
      virtual StatusCode execute(const EventContext& ctx) const override;
      virtual StatusCode finalize() override;

      // @TODO should a reentrant algorithm be clonable ??
      // virtual bool isClonable() const override { return true; };

   private:
      ToolHandle <IDetectorElementStatusTool> m_condSummaryTool
         {this, "ConditionsSummaryTool", "", "Tool to retrieve e.g. Pixel or SCT Conditions summary"};
      SG::ReadHandleKey<InDet::SiDetectorElementStatus> m_readKey
         {this, "ReadKey", "", "Optional key of an input SiDetectorElementStatus to be anded with the summary information from the tool "};

      SG::WriteCondHandleKey<InDet::SiDetectorElementStatus> m_writeKey
         {this, "WriteKey", "", "Key of output SiDetectorElementStatus data"};
      Gaudi::Property< bool> m_activeOnly
         {this, "ActiveOnly", false, "Module and chip status will only reflect whether the modules or chips are active not necessarily whether the signals are good."};
      mutable std::atomic<unsigned int> m_maxSize{};
   };
}
#endif // PIXELCONDITIONSALGORITHMS_SIDETECTORELEMENTINFOALG_H

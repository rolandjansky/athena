// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSALGORITHMS_SIDETECTORELEMENTINFOALG_H
#define PIXELCONDITIONSALGORITHMS_SIDETECTORELEMENTINFOALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/WriteHandleKey.h"

#include "InDetConditionsSummaryService/IDetectorElementStatusTool.h"
#include "InDetReadoutGeometry/SiDetectorElementStatus.h"


namespace InDet {
   class SiDetectorElementStatusAlg : public AthReentrantAlgorithm
   {
   public:
      SiDetectorElementStatusAlg(const std::string& name, ISvcLocator* pSvcLocator);
      virtual ~SiDetectorElementStatusAlg() override = default;

      virtual StatusCode initialize() override;
      virtual StatusCode execute(const EventContext& ctx) const override;

      // @TODO should a reentrant algorithm be clonable ??
      // virtual bool isClonable() const override { return true; };

   private:
      ToolHandle <IDetectorElementStatusTool> m_condSummaryTool
         {this, "ConditionsSummaryTool", "", "Tool to retrieve e.g. Pixel or SCT Conditions summary"};
      SG::WriteHandleKey<InDet::SiDetectorElementStatus> m_writeKey
         {this, "WriteKey", "", "Key of output SiDetectorElementStatus data"};
   };
}
#endif // PIXELCONDITIONSALGORITHMS_SIDETECTORELEMENTINFOALG_H

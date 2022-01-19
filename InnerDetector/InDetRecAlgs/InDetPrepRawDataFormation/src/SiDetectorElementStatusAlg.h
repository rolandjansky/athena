// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSALGORITHMS_SIDETECTORELEMENTINFOALG_H
#define PIXELCONDITIONSALGORITHMS_SIDETECTORELEMENTINFOALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/WriteHandleKey.h"

#include "InDetConditionsSummaryService/IExtendedInDetConditionsTool.h"
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
      ToolHandle <IExtendedInDetConditionsTool> m_condSummaryTool
         {this, "ConditionsSummaryTool", "", "Tool to retrieve e.g. Pixel or SCT Conditions summary"};
      SG::ReadHandleKey<InDet::SiDetectorElementStatus> m_readKey
         {this, "ReadKey", "", "Optional key of an input SiDetectorElementStatus to be anded with the summary information from the tool "};

      SG::WriteHandleKey<InDet::SiDetectorElementStatus> m_writeKey
         {this, "WriteKey", "", "Key of output SiDetectorElementStatus data"};
      Gaudi::Property< bool> m_activeOnly
         {this, "ActiveOnly", false, "Module and chip status will only reflect whether the modules or chips are active not necessarily whether the signals are good."};
   };
}
#endif // PIXELCONDITIONSALGORITHMS_SIDETECTORELEMENTINFOALG_H

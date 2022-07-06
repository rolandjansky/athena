/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGVSIHYPOALG_H
#define TRIGVSIHYPOALG_H

// Framework includes
#include "TrigVSIHypoTool.h"

#include "DecisionHandling/HypoBase.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTracking/VertexContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "Gaudi/Property.h"
#include "AthViews/View.h"
#include <map>

// STL includes
#include <string>
class TrigVSIHypoAlg : public ::HypoBase  {
   public:
      TrigVSIHypoAlg(const std::string& name, ISvcLocator* pSvcLocator);

      virtual ~TrigVSIHypoAlg() = default;
      virtual StatusCode initialize() override;
      virtual StatusCode execute(const EventContext& context) const override;

   private:
      TrigVSIHypoAlg();
      ToolHandleArray<TrigVSIHypoTool>                   m_hypoTools    {this, "HypoTools", {},"Tools that perform actual selection"};
      SG::ReadHandleKey<xAOD::VertexContainer>           m_verticesKey  {this, "verticesKey", "Undefined",""};
      SG::WriteHandleKey<xAOD::TrigCompositeContainer>   m_vtxCountKey  {this, "vtxCountKey", "Undefined",""};
      Gaudi::Property<bool>  m_isViewBased   {this, "isViewBased",   true,    "Flag to toggle the hypo to run in a view or in whole-event level"};
      Gaudi::Property<float> m_minPt         {this, "minPt",         1000.,   "Preselection. Accept vertices with momentum higher than this limit"};
      Gaudi::Property<float> m_minR          {this, "minR" ,         20.,     "Preselection. Accept vertices with decay position R higher this limit"};
      Gaudi::Property<float> m_requiredNTrks {this, "requiredNTrks", 2, "Preselection. Accept vertices with tracks more than this limit"};
      ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
};
#endif

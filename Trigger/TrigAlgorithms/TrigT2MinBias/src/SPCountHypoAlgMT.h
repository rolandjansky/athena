/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT2MINBIAS_SPCOUNTHYPOALGMT_H
#define TRIGT2MINBIAS_SPCOUNTHYPOALGMT_H

// Framework includes
#include "DecisionHandling/HypoBase.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "SPCountHypoTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include <map>
#include "Gaudi/Property.h"
#include "AthViews/View.h"

// STL includes
#include <string>
class SPCountHypoAlgMT : public ::HypoBase  {

public:
  SPCountHypoAlgMT(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;


private:
  SPCountHypoAlgMT();
  ToolHandleArray< SPCountHypoTool > m_hypoTools {this, "HypoTools", {},"Tools that perform actual selection"};
  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_spacePointsKey{this,"SpacePointsKey","Undefined",""};
};
#endif // TRIGT2MINBIAS_SPCOUNTHYPOALGMT_H

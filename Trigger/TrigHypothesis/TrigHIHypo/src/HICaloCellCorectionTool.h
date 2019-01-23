/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HICALOCELLCORECTIONTOOL_H
#define HICALOCELLCORECTIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "HIGlobal/IHIEventShapeFiller.h"
#include "StoreGate/ReadHandleKey.h"

#include <string>

//using namespace xAOD;
class HICaloCellCorectionTool
  : public extends<AthAlgTool, ICaloCellMakerTool>
{
 public:
  HICaloCellCorectionTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~HICaloCellCorectionTool(){};
  
  virtual StatusCode initialize() override;
  virtual StatusCode process(CaloCellContainer* cellContainer) override;
 private:

  SG::ReadHandleKey<xAOD::HIEventShapeContainer> m_eventShapeKey;
  double getAvgEnergy (const xAOD::HIEventShapeContainer& eventShape,
                       const int layer,
                       const double eta) const;
};



#endif

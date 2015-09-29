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

#include <string>

using namespace xAOD;
class HICaloCellCorectionTool : public  ICaloCellMakerTool, public AthAlgTool{
 public:
  HICaloCellCorectionTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~HICaloCellCorectionTool(){};
  
  virtual StatusCode initialize();
  virtual StatusCode process(CaloCellContainer* cellContainer);
 private:

  std::string m_CaloCellContainerName;
  std::string m_HIEventShapeContainerName;
  const xAOD::HIEventShapeContainer* m_eventShape;
  //  ToolHandle<IHIEventShapeFiller> m_eventShapeTool;
  double m_avgEnergy;
  double getAvgEnergy(const int layer, const double eta);
};



#endif

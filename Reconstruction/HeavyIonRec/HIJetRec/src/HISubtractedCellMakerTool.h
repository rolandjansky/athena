/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HISUBTRACTEDCELLMAKERTOOL_H__
#define __HISUBTRACTEDCELLMAKERTOOL_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"

#include <string>
#include <vector>


class HISubtractedCellMakerTool : public AthAlgTool, virtual ICaloCellMakerTool
{

public:

  HISubtractedCellMakerTool(const std::string& type, const std::string& name, const IInterface* parent);
  StatusCode initialize();
  StatusCode process(CaloCellContainer* theCellContainer);
  

private:
  std::string m_event_shape_key;
  std::vector<unsigned int> m_flow_harmonics;


};

#endif

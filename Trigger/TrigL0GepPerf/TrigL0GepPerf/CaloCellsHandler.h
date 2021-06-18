/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGL0GEPPERF_CaloCellsHandler_H
#define TRIGL0GEPPERF_CaloCellsHandler_H

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "TrigL0GepPerf/CustomCaloCell.h"
#include <vector>


class CaloCellsHandler{

 public:

  CaloCellsHandler(){}

  void fillCellsInformation(const CaloCellContainer* cells);
  void updateCellsInformation(const CaloCellContainer* cells);
  void passNoiseTool(ToolHandle<ICalorimeterNoiseTool> m_noiseTool) { noiseTool_cells = m_noiseTool; }
  void passCaloCellIDTool( const CaloCell_ID* ccIdHelper) { m_ccIdHelper = ccIdHelper; }  

  Gep::CustomCaloCell getCaloCell(unsigned int id) { return m_caloCells[id]; }
  std::map<unsigned int,Gep::CustomCaloCell> getCaloCellsMap(){ return m_caloCells; }



 private:

  std::map<unsigned int,Gep::CustomCaloCell> m_caloCells; 
  ToolHandle<ICalorimeterNoiseTool> noiseTool_cells;
  const mutable CaloCell_ID* m_ccIdHelper;
  std::vector<unsigned int> getNeighbours(const CaloCellContainer* allcells, const CaloCell* acell);

};

#endif

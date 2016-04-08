/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMPTY_CELLBUILDERTOOL_H
#define EMPTY_CELLBUILDERTOOL_H
// 
// CellBuilderTool.cxx
//     Building Cells objects from Atlfast
//
// Michael Duehrssen

#include "FastCaloSim/BasicCellBuilderTool.h"

class EmptyCellBuilderTool: public BasicCellBuilderTool
{
public:    
  EmptyCellBuilderTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent);
  ~EmptyCellBuilderTool();


  virtual StatusCode initialize();

  // update theCellContainer
  virtual StatusCode process( CaloCellContainer * theCellContainer) ;
private:
  void create_empty_calo(CaloCellContainer * theCellContainer);

};

#endif






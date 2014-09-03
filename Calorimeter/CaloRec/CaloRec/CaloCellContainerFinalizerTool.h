// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALOREC_CALOCELLCONTAINERFINALIZERTOOL_H
#define CALOREC_CALOCELLCONTAINERFINALIZERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloInterface/ICaloConstCellMakerTool.h"

class CaloCellContainerFinalizerTool
  : public AthAlgTool,
    virtual public ICaloCellMakerTool,
    virtual public ICaloConstCellMakerTool
{
public:    
  CaloCellContainerFinalizerTool(const std::string& type, 
                                 const std::string& name, 
				 const IInterface* parent) ;


  virtual StatusCode initialize() ; 

  // update theCellContainer
  virtual StatusCode process(CaloCellContainer     * theCellContainer) override;
  virtual StatusCode process(CaloConstCellContainer* theCellContainer) override;

private:
  template <class CONTAINER>
  StatusCode doProcess (CONTAINER* theCellContainer);
};

#endif


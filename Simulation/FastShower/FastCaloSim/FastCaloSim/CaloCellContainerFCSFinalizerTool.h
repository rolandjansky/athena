// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALOCELLCONTAINERFCSFINALIZERTOOL_H
#define CALOCELLCONTAINERFCSFINALIZERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloInterface/ICaloConstCellMakerTool.h"

class CaloCellContainerFCSFinalizerTool
  : public AthAlgTool,
    virtual public ICaloCellMakerTool,
    virtual public ICaloConstCellMakerTool
{
public:    
  CaloCellContainerFCSFinalizerTool(const std::string& type, 
                                 const std::string& name, 
				 const IInterface* parent) ;


  // update theCellContainer
  virtual StatusCode process(CaloCellContainer     * theCellContainer) override;
  virtual StatusCode process(CaloConstCellContainer* theCellContainer) override;

private:
  template <class CONTAINER>
  StatusCode doProcess (CONTAINER* theCellContainer);
};

#endif


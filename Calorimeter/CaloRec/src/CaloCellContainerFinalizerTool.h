// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALOREC_CALOCELLCONTAINERFINALIZERTOOL_H
#define CALOREC_CALOCELLCONTAINERFINALIZERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloInterface/ICaloConstCellMakerTool.h"

class CaloCell_ID;

class CaloCellContainerFinalizerTool
  : public extends<AthAlgTool, ICaloCellMakerTool, ICaloConstCellMakerTool>
{
public:    
  CaloCellContainerFinalizerTool(const std::string& type, 
                                 const std::string& name, 
				 const IInterface* parent) ;


  virtual StatusCode initialize() override; 

  // update theCellContainer
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;
  virtual StatusCode process (CaloConstCellContainer* theCellContainer,
                              const EventContext& ctx) const  override;

private:
  template <class CONTAINER>
  StatusCode doProcess (CONTAINER* theCellContainer) const;

  const CaloCell_ID* m_theCaloCCIDM;
};

#endif


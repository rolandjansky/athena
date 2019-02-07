/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOREC_CALOCELLCONTAINERCHECKERTOOL_H
#define CALOREC_CALOCELLCONTAINERCHECKERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloInterface/ICaloConstCellMakerTool.h"

class CaloCellContainerCheckerTool
  : public extends<AthAlgTool, ICaloCellMakerTool, ICaloConstCellMakerTool>
{
 
public:    
  
  CaloCellContainerCheckerTool(const std::string& type, 
			       const std::string& name, 
			       const IInterface* parent) ;


  virtual StatusCode initialize() override; 

  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;
  virtual StatusCode process (CaloConstCellContainer* theCellContainer,
                              const EventContext& ctx) const override;

 private:
  StatusCode doProcess (const CaloCellContainer* theCellContainer,
                        const EventContext& ctx) const;

  size_t m_eventsToCheck;
};

#endif


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOCELLCONTAINERMERGERTOOL_H
#define CALOREC_CALOCELLCONTAINERMERGERTOOL_H

class StoreGateSvc; 
#include "AthenaBaseComps/AthAlgTool.h"


#include "CaloInterface/ICaloCellMakerTool.h"

class CaloCellContainerMergerTool
  : public extends<AthAlgTool, ICaloCellMakerTool>
{
public:    
  
  CaloCellContainerMergerTool(const std::string& type, 
                              const std::string& name, 
                              const IInterface* parent) ;


  virtual StatusCode initialize() override;

  // update theCellContainer
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;


 private:
  // Name of input container
  SG::ReadHandleKey<CaloCellContainer> m_caloCellsKey;
  std::vector<int> m_caloNums ; // list of calo to which this container 
                               // correspond. Only used to specify 
                               // bool hasCalo of CaloCellContainer
};

#endif


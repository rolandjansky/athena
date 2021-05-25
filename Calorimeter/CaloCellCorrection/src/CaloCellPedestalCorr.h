//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCELLCORRECTION_CALOCELLPEDESTALCORR_H
#define CALOCELLCORRECTION_CALOCELLPEDESTALCORR_H

#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloConditions/CaloCellPedShift.h"
#include "StoreGate/ReadHandle.h"
#include "CaloEvent/CaloBCIDAverage.h"

class CaloCondBlobFlt;
class CaloCell;
class CaloCell_ID;

class CaloCellPedestalCorr : public AthAlgTool, 
			     virtual public ICaloCellMakerTool

{

public:

  CaloCellPedestalCorr(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~CaloCellPedestalCorr() {};

  virtual StatusCode initialize() override;

  StatusCode process( CaloCellContainer * theCellContainer, 
		      const EventContext& ctx) const override;

private:

  SG::ReadCondHandleKey<CaloCellPedShift> m_pedShiftKey{this,"CaloPedShiftKey","CaloPedShift"};
  SG::ReadHandleKey<CaloBCIDAverage> m_caloBCIDAvg{this,"CaloBCIDAverageKey","","SG Key of CaloBCIDAverage object"};

  Gaudi::Property<bool> m_isMC{this,"isMC",false,"Data/MC flag"};
  const CaloCell_ID* m_cellId=nullptr;

};

#endif

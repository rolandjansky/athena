//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCELLCORRECTION_CALOCELLPEDESTALCORR_H
#define CALOCELLCORRECTION_CALOCELLPEDESTALCORR_H

#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloCondBlobObjs/ICaloCoolIdTool.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
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
  // const DataHandle<CondAttrListCollection> m_noiseAttrListColl;


  ToolHandle<ICaloCoolIdTool> m_caloCoolIdTool;
  float m_lumi0;
  
  SG::ReadCondHandleKey<CondAttrListCollection> m_pedShiftFolder{this,"PedestalShiftFolder","/CALO/Pedestal/CellPedestal","SG Key of Attr list containing pedestal shifts"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_lumiFolderName{this,"LumiFolderName","/TRIGGER/LUMI/LBLESTONL","SG Key of Attr list for Luminosity estimate"};
  const CaloCell_ID* m_cellId;

  SG::ReadHandleKey<CaloBCIDAverage> m_caloBCIDAvg{this,"CaloBCIDAverageKey","","SG Key of CaloBCIDAverage object"};

  bool m_isMC;
};

#endif

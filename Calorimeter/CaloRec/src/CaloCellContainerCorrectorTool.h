/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOCELLCONTAINERCORRECTORTOOL_H
#define CALOREC_CALOCELLCONTAINERCORRECTORTOOL_H

class StoreGateSvc; 
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloUtils/CaloCellCorrection.h"

#include "GaudiKernel/ToolHandle.h"

class CaloCellContainerCorrectorTool
  : public extends<AthAlgTool, ICaloCellMakerTool>
{
public:    
  CaloCellContainerCorrectorTool(const std::string& type, 
				 const std::string& name, 
				 const IInterface* parent) ;

  typedef ToolHandleArray<CaloCellCorrection> ::iterator 
    CellCorrectionToolIterator;   

  virtual StatusCode initialize() override;
  // update theCellContainer
  virtual StatusCode process ( CaloCellContainer* theCellContainer,
                               const EventContext& ctx ) const override;


 private:
// properties

  std::vector<int> m_caloNums ; // which calo to correct
  //reminder  enum SUBCALO { LAREM = 0, LARHEC = 1, LARFCAL = 2, TILE = 3, NSUBCALO = 4, NOT_VALID=999999 };

  ToolHandleArray<CaloCellCorrection> m_cellCorrectionTools;

  bool m_caloSelection ;

  StatusCode processOnCellIterators(const CaloCellContainer::iterator  &  itrCellBeg,
                                    const CaloCellContainer::iterator & itrCellEnd,
                                    const EventContext& ctx) const;
  

};

#endif


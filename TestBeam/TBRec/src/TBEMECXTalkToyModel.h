/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBEMECXTALKTOYMODEL_H
#define TBREC_TBEMECXTALKTOYMODEL_H

class StoreGateSvc; 
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloEvent/CaloCellContainer.h"

#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloCell_ID.h"

class CaloDetDescrManager;


class TBEMECXTalkToyModel
  : public extends<AthAlgTool, ICaloCellMakerTool>
{
public:    
  TBEMECXTalkToyModel(const std::string& type, 
                      const std::string& name, 
                      const IInterface* parent) ;

  virtual StatusCode initialize() override;
  // update theCellContainer
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;


 private:
// properties

  std::vector<int> m_caloNums ; // which calo to correct
  //reminder  enum SUBCALO { LAREM = 0, LARHEC = 1, LARFCAL = 2, TILE = 3, NSUBCALO = 4, NOT_VALID=999999 };

  bool m_caloSelection ;

  const CaloDetDescrManager * m_calo_dd_man;
  const CaloCell_ID * m_calo_id;


  float m_xtalkScaleLong;
  float m_xtalkScaleEta;
  float m_xtalkScaleEMEC2Eta;

  StatusCode processOnCellIterators(const CaloCellContainer::iterator  &  itrCellBeg, const CaloCellContainer::iterator & itrCellEnd ) const;
  

};

#endif


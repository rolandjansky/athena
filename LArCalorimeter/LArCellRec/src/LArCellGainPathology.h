/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARCELLGAINPATHOLOGY_H
#define LARCELLREC_LARCELLGAINPATHOLOGY_H

/** 
@class LArCellGainPathology
@brief Apply to MC the pathology on gain corruption between cha0-63, 64-127, which affected 7 samples data April-July 2010


 Created July 29, 2010   G.Unal
*/



#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"


class CaloCell_ID;
class LArOnlineID;
class HWIdentifier;
class CaloCell;

class LArCellGainPathology
  : public extends<AthAlgTool, ICaloCellMakerTool>
{
public:    
  LArCellGainPathology(const std::string& type, 
                       const std::string& name, 
                       const IInterface* parent) ;


  /** initialize the tool
  */
  virtual StatusCode initialize() override;

  /** finalize   the tool
  */
  virtual StatusCode finalize() override;

  /** update theCellContainer, masking Feb with errors
  */
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;

 private:
  /** method to apply pathology between a couple of cells
  */
  void ApplyPathology(CaloCellContainer* theCont, HWIdentifier id1, HWIdentifier id,
		      const LArOnOffIdMapping* cabling) const;

  /** method to find cell from hardware id
  */
  CaloCell* GetCell(CaloCellContainer* theCont, HWIdentifier id, 
		    const LArOnOffIdMapping* cabling) const;

  /** handle to LAr cabling service
  */
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  /** pointers to storegateSvc and identifier helpers
  */
  const CaloCell_ID* m_calo_id;
  const LArOnlineID* m_onlineID;

};

#endif


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LArCellGainPathology_H
#define LARCELLREC_LArCellGainPathology_H

/** 
@class LArCellGainPathology
@brief Apply to MC the pathology on gain corruption between cha0-63, 64-127, which affected 7 samples data April-July 2010


 Created July 29, 2010   G.Unal
*/



#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

class LArCablingService;
class StoreGateSvc;
class CaloCell_ID;
class LArOnlineID;
class ILArBadChanTool;
class HWIdentifier;
class CaloCell;

class LArCellGainPathology: public AthAlgTool,
  virtual public ICaloCellMakerTool 
{
public:    
  LArCellGainPathology(const std::string& type, 
                       const std::string& name, 
                       const IInterface* parent) ;


  /** initialize the tool
  */
  virtual StatusCode initialize() ; 

  /** finalize   the tool
  */
  virtual StatusCode finalize() ; 

  /** update theCellContainer, masking Feb with errors
  */
  virtual StatusCode process( CaloCellContainer * theCellContainer) ;

  /** Callback added to handle Data-driven GeoModel initialisation
   */
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);



 private:
  /** method to apply pathology between a couple of cells
  */
  void ApplyPathology(CaloCellContainer* theCont, HWIdentifier id1, HWIdentifier id2);

  /** method to find cell from hardware id
  */
  CaloCell* GetCell(CaloCellContainer* theCont, HWIdentifier id);

  /** handle to LAr cabling service
  */
  ToolHandle<LArCablingService> m_cablingService;

  /** pointers to storegateSvc and identifier helpers
  */
  const CaloCell_ID* m_calo_id;
  const LArOnlineID* m_onlineID;

};

#endif


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloFillCellPositionShift.h
// 
//

#ifndef _CaloCondPhysAlgs_CaloFillCellPositionShift_H
#define _CaloCondPhysAlgs_CaloFillCellPositionShift_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"

class CaloIdManager;
class CaloDetDescrmanager;
class CaloCell_ID;
namespace CaloRec {
  class CaloCellPositionShift;
}

class CaloFillCellPositionShift : public AthAlgorithm {

  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    CaloFillCellPositionShift(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~CaloFillCellPositionShift();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();
    /** standard Athena-Algorithm method */
    StatusCode          stop();
    
  private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  const DataHandle<CaloIdManager> m_caloIdMgr;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  const CaloCell_ID*       m_calo_id;

  CaloRec::CaloCellPositionShift* m_cellPos;

  std::string m_key;

};
#endif

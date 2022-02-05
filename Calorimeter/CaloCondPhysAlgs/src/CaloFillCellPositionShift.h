/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// CaloFillCellPositionShift.h
// 
//

#ifndef CALOCONDPHYSALGS_CALOFILLCELLPOSITIONSHIFT_H
#define CALOCONDPHYSALGS_CALOFILLCELLPOSITIONSHIFT_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"

class CaloIdManager;
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
    virtual StatusCode          initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          execute() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          finalize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          stop() override;
    
  private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  const CaloCell_ID*       m_calo_id;

  CaloRec::CaloCellPositionShift* m_cellPos;

  std::string m_key;

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey
    {this,"CaloDetDescrManager","CaloDetDescrManager","SG Key for CaloDetDescrManager in the Condition Store" };
};
#endif

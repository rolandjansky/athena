/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 *
 * @file   CalibHitToCaloCell.h
 * @author Gia, gia@mail.cern.ch
 * @date   March, 2005
 */

#ifndef CALOCALIBHITREC_CALIBHITTOCALOCELL_H
#define CALOCALIBHITREC_CALIBHITTOCALOCELL_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <string>
#include <vector>

typedef std::vector<double> Energy ;

class CaloCell_ID;
class CaloDM_ID;

class LArCell;

class CalibHitToCaloCell : public AthAlgorithm {
public:

    CalibHitToCaloCell(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~CalibHitToCaloCell();                         
    
    StatusCode initialize() override;
    StatusCode execute() override;
    StatusCode finalize() override;

    void test_energy(Energy*);

private:

    std::string m_tileActiveHitCnt;
    std::string m_tileInactiveHitCnt;
    std::string m_tileDMHitCnt;
    std::string m_larInactHitCnt;
    std::string m_larActHitCnt;
    std::string m_larDMHitCnt;
    
    bool m_store_Tot;
    bool m_store_Vis;
    bool m_store_Em;
    bool m_store_NonEm;
    bool m_storeUnknown;

    std::string m_caloCell_Tot;
    std::string m_caloCell_Vis;
    std::string m_caloCell_Em;
    std::string m_caloCell_NonEm;

    const CaloCell_ID*  m_caloCell_ID{nullptr};
    const CaloDM_ID*    m_caloDM_ID{nullptr};

    std::vector<LArCell*> m_Cells_Tot;
    std::vector<LArCell*> m_Cells_Vis;
    std::vector<LArCell*> m_Cells_Em;
    std::vector<LArCell*> m_Cells_NonEm;

    int m_nchan;

    SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
	, "CaloDetDescrManager"
	, "CaloDetDescrManager"
	, "SG Key for CaloDetDescrManager in the Condition Store" };
};

#endif

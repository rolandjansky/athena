/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : CalibHitToCaloCell.h
//
// Author   : Gia	gia@mail.cern.ch
// Created  : March. 2005
//
//  
//****************************************************************************
#ifndef CalibHitToCaloCell_H
#define CalibHitToCaloCell_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"

#include <string>
#include <vector>

typedef std::vector<double> Energy ;

class CaloCell_ID;
class CaloDM_ID;
class TileID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;

class LArCell;
class CaloDetDescrManager;

class CalibHitToCaloCell : public AthAlgorithm {
public:

    CalibHitToCaloCell(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~CalibHitToCaloCell();                         
    
    StatusCode initialize();    
    StatusCode execute();
    StatusCode finalize();

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

    const CaloCell_ID*  m_caloCell_ID;
    const CaloDM_ID*    m_caloDM_ID;
    //const TileID*       m_tile_ID;
    //const LArEM_ID*     m_larEm_ID;
    //const LArHEC_ID*    m_larHec_ID;
    //const LArFCAL_ID*   m_larFcal_ID;

    std::vector<LArCell*> m_Cells_Tot;
    std::vector<LArCell*> m_Cells_Vis;
    std::vector<LArCell*> m_Cells_Em;
    std::vector<LArCell*> m_Cells_NonEm;

    int m_nchan;
//    int m_dm_nchan;
};

#endif

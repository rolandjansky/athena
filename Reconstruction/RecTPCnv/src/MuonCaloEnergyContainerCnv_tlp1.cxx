/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        MuonCaloEnergyContainerCnv_tlp1.cxx
// package:     Reconstruction/RecTPCnv
// authors:     Ketevi A. Assamagan
// date:        2008-04-17
//-----------------------------------------------------------------------------

#include "RecTPCnv/MuonCaloEnergyContainerCnv_tlp1.h"

MuonCaloEnergyContainerCnv_tlp1 :: MuonCaloEnergyContainerCnv_tlp1( )
{
    addMainTPConverter();
    addTPConverter( &m_caloEnergyCnv );
}

void MuonCaloEnergyContainerCnv_tlp1 :: setPStorage( MuonCaloEnergyContainer_tlp1 *storage )
{
    setMainCnvPStorage( &storage->m_muonCaloEnergyContainer );
    m_caloEnergyCnv.setPStorage( &storage->m_caloEnergies );
}


T_TPCnv<MuonCaloEnergyContainer, MuonCaloEnergyContainer_tlp1 >::T_TPCnv()
{
}


void T_TPCnv<MuonCaloEnergyContainer, MuonCaloEnergyContainer_tlp1 >::
persToTrans (const MuonCaloEnergyContainer_tlp1* pers,
             MuonCaloEnergyContainer* trans,
             MsgStream& msg)
{
  setPStorage (const_cast<MuonCaloEnergyContainer_tlp1*> (pers));
  m_mainConverter.pstoreToTrans (0, trans, msg);
}


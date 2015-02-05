/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        MuonCaloEnergyContainerCnv_tlp1.h
// package:     Reconstruction/RecTPCnv
// authors:     Ketevi A. Assamagan
// date:        2008-04-17
//-----------------------------------------------------------------------------

#ifndef RECTPCNV_MUONCALOENERGY_CONTAINER_CNV_TLP1_H
#define RECTPCNV_MUONCALOENERGY_CONTAINER_CNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "RecTPCnv/MuonCaloEnergyContainer_tlp1.h"
#include "RecTPCnv/MuonCaloEnergyContainerCnv_p1.h"

#include "RecTPCnv/CaloEnergyCnv_p2.h"

#include <iostream>

class MuonCaloEnergyContainerCnv_tlp1:
    public AthenaPoolTopLevelTPConverter<MuonCaloEnergyContainerCnv_p1, MuonCaloEnergyContainer_tlp1>
{
    public:
        MuonCaloEnergyContainerCnv_tlp1();
        virtual ~MuonCaloEnergyContainerCnv_tlp1() {}
        virtual void setPStorage( MuonCaloEnergyContainer_tlp1 *storage );

        unsigned short        getConverterID() { return 3; } // This is the ID of this extending converter (i.e. InDet is 1)
  
    protected:
        CaloEnergyCnv_p2 m_caloEnergyCnv;
};


#endif

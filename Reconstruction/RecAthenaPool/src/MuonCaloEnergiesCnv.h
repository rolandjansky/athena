/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// removal candidate - together will all MuonCaloEnergies* things.

#ifndef RECATHENAPOOL_MUONCALOENERGIES_CNV_H
#define RECATHENAPOOL_MUONCALOENERGIES_CNV_H


#include "AthenaPoolCnvSvc/T_AthenaPoolExtendingCnv.h"

#include "RecTPCnv/MuonCaloEnergyContainerCnv_tlp1.h"

#include "MuonCaloEnergies.h"


//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef MuonCaloEnergyContainer_tlp1       MuonCaloEnergies_PERS;

typedef T_AthenaPoolExtendingCnv<TPCnv::MuonCaloEnergies, MuonCaloEnergies_PERS>       MuonCaloEnergiesCnvBase;

//-----------------------------------------------------------------------------
// Converter for TrackCollection object
//-----------------------------------------------------------------------------
class MuonCaloEnergiesCnv : public MuonCaloEnergiesCnvBase
{
  friend class CnvFactory<MuonCaloEnergiesCnv>;

protected:
  MuonCaloEnergiesCnv( ISvcLocator *svcloc );

  virtual StatusCode initialize();

  virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_TPConverter; }

  virtual void          readObjectFromPool( const std::string& );

  virtual AthenaPoolCnvTPExtension*  clone() { return new MuonCaloEnergiesCnv(0); }
  
private:  
  MuonCaloEnergyContainerCnv_tlp1            m_TPConverter;
};

#endif



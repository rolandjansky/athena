/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TPCNV_MUONMEASUREMENTSCNV_H
#define TPCNV_MUONMEASUREMENTSCNV_H


#include "AthenaPoolCnvSvc/T_AthenaPoolExtendingCnv.h"

#include "MuonEventTPCnv/MuonMeasurementsCnv_tlp2.h"
#include "MuonEventTPCnv/MuonMeasurementsCnv_tlp1.h"

#include "MuonMeasurements.h"

//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef TPCnv::MuonMeasurements_tlp2       MuonMeasurements_PERS;

typedef T_AthenaPoolExtendingCnv<TPCnv::MuonMeasurements, MuonMeasurements_PERS>       MuonMeasurementsCnvBase;

//-----------------------------------------------------------------------------
// Converter for TrackCollection object
//-----------------------------------------------------------------------------
class MuonMeasurementsCnv : public MuonMeasurementsCnvBase
{
  friend class CnvFactory<MuonMeasurementsCnv>;

protected:
  MuonMeasurementsCnv( ISvcLocator *svcloc );

  virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_TPConverter_p2; }

  virtual void          readObjectFromPool( const std::string& );

  virtual AthenaPoolCnvTPExtension*  clone() { return new MuonMeasurementsCnv(0); }
  
private:  
  MuonMeasurementsCnv_tlp2            m_TPConverter_p2;
  MuonMeasurementsCnv_tlp1            m_TPConverter_p1;
};

#endif


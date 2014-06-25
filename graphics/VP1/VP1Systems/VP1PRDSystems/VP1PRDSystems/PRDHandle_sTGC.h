/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDHANDLE_STGC_H
#define PRDHANDLE_STGC_H

#include "VP1PRDSystems/PRDHandleBase.h"
#include "VP1PRDSystems/PRDCollHandle_sTGC.h"
#include "MuonPrepRawData/sTgcPrepData.h"

class PRDHandle_sTGC : public PRDHandleBase {
public:

  PRDHandle_sTGC(PRDCollHandle_sTGC*,const Muon::sTgcPrepData*);
  virtual ~PRDHandle_sTGC() {};

  SoTransform * createTransform() const ; // FIXME! 
  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed) ;

  int regionIndex();

  const Muon::sTgcPrepData * stgc() const { return m_stgc; }
  const Trk::PrepRawData * getPRD() const { return m_stgc; }

  bool inMuonChamber() const { return true; }
  GeoPVConstLink parentMuonChamberPV() const { return m_stgc->detectorElement()->parentStationPV(); }

private:
  const Muon::sTgcPrepData* m_stgc;

};

#endif

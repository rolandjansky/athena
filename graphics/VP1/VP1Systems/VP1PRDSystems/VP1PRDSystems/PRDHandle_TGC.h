/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDHANDLE_TGC_H
#define PRDHANDLE_TGC_H

#include "VP1PRDSystems/PRDHandleBase.h"
#include "VP1PRDSystems/PRDCollHandle_TGC.h"
#include "MuonPrepRawData/TgcPrepData.h"

class PRDHandle_TGC : public PRDHandleBase {
public:

  PRDHandle_TGC(PRDCollHandle_TGC*,const Muon::TgcPrepData*);
  virtual ~PRDHandle_TGC() {};

  SoTransform * createTransform() const;

  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed);
  int regionIndex();

  const Muon::TgcPrepData * tgc() const { return m_tgc; }
  const Trk::PrepRawData * getPRD() const { return m_tgc; }

  bool inMuonChamber() const { return true; }
  GeoPVConstLink parentMuonChamberPV() const { return m_tgc->detectorElement()->parentStationPV(); }

private:
  const Muon::TgcPrepData* m_tgc;

};

#endif

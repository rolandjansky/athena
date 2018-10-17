/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDHANDLE_CSC_H
#define PRDHANDLE_CSC_H

#include "VP1PRDSystems/PRDHandleBase.h"
#include "VP1PRDSystems/PRDCollHandle_CSC.h"
#include "MuonPrepRawData/CscPrepData.h"
#include <QStringList>

class PRDHandle_CSC : public PRDHandleBase {
public:

  PRDHandle_CSC(PRDCollHandle_CSC*,const Muon::CscPrepData*);
  virtual ~PRDHandle_CSC() {};

  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed);
  int regionIndex();

  const Muon::CscPrepData * csc() const { return m_csc; }
  const Trk::PrepRawData * getPRD() const { return m_csc; }


  bool inMuonChamber() const { return true; }
  GeoPVConstLink parentMuonChamberPV() const { return m_csc->detectorElement()->parentStationPV(); }

  virtual QStringList clicked() const;//Called when user selects the node. The returned strings will be displayed in the textbox.


private:
  const Muon::CscPrepData* m_csc;

};

#endif

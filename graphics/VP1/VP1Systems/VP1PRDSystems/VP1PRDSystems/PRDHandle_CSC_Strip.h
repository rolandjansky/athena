/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1PRDSYSTEMS_PRDHANDLE_CSC_STRIP_H
#define VP1PRDSYSTEMS_PRDHANDLE_CSC_STRIP_H

#include "VP1PRDSystems/PRDHandleBase.h"
#include "VP1PRDSystems/PRDCollHandle_CSC_Strip.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include <QStringList>

class PRDHandle_CSC_Strip : public PRDHandleBase {
public:

  PRDHandle_CSC_Strip(PRDCollHandle_CSC_Strip*,const Muon::CscStripPrepData*);
  virtual ~PRDHandle_CSC_Strip() {};

  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed);
  int regionIndex();

  const Muon::CscStripPrepData * csc() const { return m_csc; }
  const Trk::PrepRawData * getPRD() const { return m_csc; }


  bool inMuonChamber() const { return true; }
  GeoPVConstLink parentMuonChamberPV() const { return m_csc->detectorElement()->parentStationPV(); }

  virtual QStringList clicked() const;//Called when user selects the node. The returned strings will be displayed in the textbox.


private:
  const Muon::CscStripPrepData* m_csc;

};

#endif

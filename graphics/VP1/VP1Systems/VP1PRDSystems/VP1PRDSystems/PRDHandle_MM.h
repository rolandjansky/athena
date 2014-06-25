/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDHANDLE_MM_H
#define PRDHANDLE_MM_H

#include "VP1PRDSystems/PRDHandleBase.h"
#include "VP1PRDSystems/PRDCollHandle_MM.h"
#include "MuonPrepRawData/MMPrepData.h"

class PRDHandle_MM : public PRDHandleBase {
public:

  PRDHandle_MM(PRDCollHandle_MM*,const Muon::MMPrepData*);
  virtual ~PRDHandle_MM() {};

  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed);
  int regionIndex();
  QStringList clicked() const;

  // inline bool highLight() const;

  const Muon::MMPrepData * mm() const { return m_prd; }
  const Trk::PrepRawData * getPRD() const { return m_prd; }

  // inline bool masked() const;
  // QString driftCircleStatus() const;
  // inline int ADC() const;

  bool inMuonChamber() const { return true; }
  GeoPVConstLink parentMuonChamberPV() const { return m_prd->detectorElement()->parentStationPV(); }


// protected:
//   void visibleStateChanged(bool newstate);

private:
  const Muon::MMPrepData* m_prd;

};


#endif

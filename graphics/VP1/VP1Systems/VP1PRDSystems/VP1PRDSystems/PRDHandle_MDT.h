/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDHANDLE_MDT_H
#define PRDHANDLE_MDT_H

#include "VP1PRDSystems/PRDHandleBase.h"
#include "VP1PRDSystems/PRDCollHandle_MDT.h"
#include "MuonPrepRawData/MdtPrepData.h"

class PRDHandle_MDT : public PRDHandleBase {
public:

  PRDHandle_MDT(PRDCollHandle_MDT*,const Muon::MdtPrepData*);
  virtual ~PRDHandle_MDT() {};

  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed);
  int regionIndex();
  QStringList clicked() const;

  inline bool highLight() const;

  bool transformUsesSurfacePositionOnly() const { return true; }
  const Muon::MdtPrepData * driftCircle() const { return m_driftcircle; }
  const Trk::PrepRawData * getPRD() const { return m_driftcircle; }

  inline bool masked() const;
  QString driftCircleStatus() const;
  inline int ADC() const;

  bool inMuonChamber() const { return true; }
  GeoPVConstLink parentMuonChamberPV() const { return m_driftcircle->detectorElement()->parentStationPV(); }


// protected:
//   void visibleStateChanged(bool newstate);

private:
  const Muon::MdtPrepData* m_driftcircle;

};

inline bool PRDHandle_MDT::masked() const
{
  return m_driftcircle->status()==Muon::MdtStatusMasked;
}

inline int PRDHandle_MDT::ADC() const
{
  return m_driftcircle->adc();
}

inline bool PRDHandle_MDT::highLight() const
{
  PRDCollHandle_MDT* ch = static_cast<PRDCollHandle_MDT*>(collHandle());
  return ( masked() && ch->highLightMasked() ) || ( ADC() <= ch->highLightADCBelow() );
}

#endif

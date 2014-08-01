/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EFLOWREC_EFLOW_TRACK_CALO_POINTS_H
#define EFLOWREC_EFLOW_TRACK_CALO_POINTS_H

/********************************************************************

NAME:     eflowTrackCaloPoints.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  R. Duxfield
CREATED:  4th January, 2006

********************************************************************/

#include <map>
#include "eflowRec/eflowCaloRegions.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "eflowRec/eflowUtil.h"
#include "xAODTracking/TrackParticle.h"

class MsgStream;
class eflowTrackToCaloTrackExtrapolatorTool;

class eflowTrackCaloPoints {
 public:

  eflowTrackCaloPoints(const eflowTrackCaloPoints& other);
  eflowTrackCaloPoints(const ToolHandle<eflowTrackToCaloTrackExtrapolatorTool>& extrapolator, const xAOD::TrackParticle* track);
  eflowTrackCaloPoints() : m_isInBarrel(false)  {}
  ~eflowTrackCaloPoints() {}

  const std::pair<float, float> operator[] (eflowCalo::LAYER lay) const;
  const eflowEtaPhiPosition& getEtaPhiPos(eflowCalo::LAYER lay) const;

  double getEta(eflowCalo::LAYER lay) const {return getEtaPhiPos(lay).getEta();}
  double getPhi(eflowCalo::LAYER lay) const {return getEtaPhiPos(lay).getPhiD();}

  const std::pair<float, float> getEM2etaPhi() const  {return (*this)[getEM2Layer()]; }
  const eflowEtaPhiPosition& getEM2etaPhiPos() const  {return getEtaPhiPos(getEM2Layer()); }
  double getEM2eta() const {return getEM2etaPhiPos().getEta(); }
  double getEM1eta() const {return getEtaPhiPos(getEM1Layer()).getEta(); }

  static double defaultEta();
  static double defaultPhi();

  inline bool haveLayer(eflowCalo::LAYER lay) const { return getEta(lay) != m_defaultEtaPhiPair.first;  }

 private:

  std::pair<Amg::Vector3D, Amg::Vector3D >
  setLastPointAndDirection(const std::vector<Amg::Vector3D >& vecEM);
  void fixFailedEmExtrapolations(const eflowCalo::LAYER firstLayer);
  void doHcalBarrelLinearExtrapolations(const Amg::Vector3D& lastPoint, const Amg::Vector3D& direction);
  std::vector<Amg::Vector3D > getAllExtrapolations(const std::vector<CaloCell_ID::CaloSample>& allSamples,
      const ToolHandle<eflowTrackToCaloTrackExtrapolatorTool>& extrapolator, const xAOD::TrackParticle* track, MsgStream& log);

  void setEtaPhi(eflowCalo::LAYER lay, const Amg::Vector3D& vec);
  void copyEtaPhi(eflowCalo::LAYER fromLay, eflowCalo::LAYER toLay);
  inline eflowCalo::LAYER getEM2Layer() const { return m_isInBarrel ? eflowCalo::EMB2 : eflowCalo::EME2; }
  inline eflowCalo::LAYER getEM1Layer() const { return m_isInBarrel ? eflowCalo::EMB1 : eflowCalo::EME1; }

  static const std::pair<float, float>  m_defaultEtaPhiPair;
  static const Amg::Vector3D m_nullVector;
  static const eflowEtaPhiPosition m_defaultEtaPhiPosition;

  bool m_isInBarrel;
  std::map<eflowCalo::LAYER, eflowEtaPhiPosition>  m_etaPhiPositions;
};


inline void eflowTrackCaloPoints::setEtaPhi(eflowCalo::LAYER lay, const Amg::Vector3D& vec) {
  m_etaPhiPositions[lay] = (vec != m_nullVector) ? eflowEtaPhiPosition(vec.eta(), vec.phi())
                                                 : m_defaultEtaPhiPosition;
}

inline void eflowTrackCaloPoints::copyEtaPhi(eflowCalo::LAYER fromLay, eflowCalo::LAYER toLay) {
  std::map<eflowCalo::LAYER, eflowEtaPhiPosition>::const_iterator it = m_etaPhiPositions.find(fromLay);
  if (it != m_etaPhiPositions.end()) {
    m_etaPhiPositions[toLay] = it->second;
  }
}


#endif

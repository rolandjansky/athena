/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "GeoPrimitives/GeoPrimitives.h" // compiles without, throws down warning: CHECKREQ---> The following packages are apparently not needed. (severity 2)
#include "TrkParameters/TrackParameters.h"  // typedef
#include "eflowRec/eflowUtil.h"

class MsgStream;

/**
This class stores a map of calorimeter layers and track parameters (the result of the track extrapolation to the calorimeter layers). It also stores maps from calorimeter layers to positions, directions and eflowEtaPhiPositions. 
*/
class eflowTrackCaloPoints {
 public:

  eflowTrackCaloPoints(const std::map<eflowCalo::LAYER, const Trk::TrackParameters*> & trackParameters);
 eflowTrackCaloPoints() : m_isEM1Barrel(false), m_isEM2Barrel(false)  {}
  ~eflowTrackCaloPoints();

  const std::pair<float, float> operator[] (eflowCalo::LAYER layer) const;
  const eflowEtaPhiPosition& getEtaPhiPos(eflowCalo::LAYER layer) const;

  double getEta(eflowCalo::LAYER layer) const {return getEtaPhiPos(layer).getEta();}
  double getPhi(eflowCalo::LAYER layer) const {return getEtaPhiPos(layer).getPhiD();}

  const std::pair<float, float> getEM2etaPhi() const  {return (*this)[getEM2Layer()]; }
  const eflowEtaPhiPosition& getEM2etaPhiPos() const  {return getEtaPhiPos(getEM2Layer()); }
  double getEM2eta() const {return getEM2etaPhiPos().getEta(); }
  double getEM1eta() const {return getEtaPhiPos(getEM1Layer()).getEta(); }
  double getFCAL0eta() const {return getEtaPhiPos(eflowCalo::FCAL0).getEta(); }
  
  Amg::Vector3D getPosition(eflowCalo::LAYER layer);
  Amg::Vector3D getDirection(eflowCalo::LAYER layer);

  static double defaultEta();
  static double defaultPhi();

  inline bool haveLayer(eflowCalo::LAYER layer) const { return getEta(layer) != m_defaultEtaPhiPair.first;  }

  void setEtaPhi(eflowCaloENUM secondLayer, double eta, double phi);
  void setEtaPhi(eflowCalo::LAYER lay, const Amg::Vector3D& vec);
  void copyEtaPhi(eflowCalo::LAYER fromLay, eflowCalo::LAYER toLay);

  static Amg::Vector3D parToPosition(const Trk::TrackParameters* extrapolatedParameters);
  static Amg::Vector3D parToDirection(const Trk::TrackParameters* extrapolatedParameters);

  bool m_isEM1Barrel;
  bool m_isEM2Barrel;

 private:

  static const Amg::Vector3D m_nullVector;

  inline eflowCalo::LAYER getEM2Layer() const { return m_isEM2Barrel ? eflowCalo::EMB2 : eflowCalo::EME2; }
  inline eflowCalo::LAYER getEM1Layer() const { return m_isEM1Barrel ? eflowCalo::EMB1 : eflowCalo::EME1; }


  static const std::pair<float, float>  m_defaultEtaPhiPair;
  static const eflowEtaPhiPosition m_defaultEtaPhiPosition;

  std::map<eflowCalo::LAYER, Amg::Vector3D > m_positions;
  std::map<eflowCalo::LAYER, Amg::Vector3D > m_directions;
  std::map<eflowCalo::LAYER, eflowEtaPhiPosition>  m_etaPhiPositions;
};

inline void eflowTrackCaloPoints::copyEtaPhi(eflowCalo::LAYER fromLay, eflowCalo::LAYER toLay) {
  std::map<eflowCalo::LAYER, eflowEtaPhiPosition>::const_iterator it = m_etaPhiPositions.find(fromLay);
  if (it != m_etaPhiPositions.end()) {
    m_etaPhiPositions[toLay] = it->second;
  }
}


#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFMatchPositions.h
 *
 *  Created on: 25.03.2014
 *      Author: tlodd
 */

#ifndef PFMATCHPOSITION_H_
#define PFMATCHPOSITION_H_

#include <vector>

#include "eflowRec/eflowUtil.h"
#include "eflowRec/PFMatchInterfaces.h"

namespace PFMatch {
//
///* Cache position  */
//
//template <class PositionType>
//class PositionObjectCache {
//protected:
//  PositionObjectCache() { m_position = new PositionType(); }
//  virtual ~PositionObjectCache() { delete m_position; }
//  PositionType* m_position;
//};


/* Position classes */

class EtaPhi: public eflowEtaPhiPosition {
public:
  EtaPhi(const eflowEtaPhiPosition& etaphi): eflowEtaPhiPosition(etaphi) { }
  EtaPhi() { }
  virtual ~EtaPhi() { }
};

class AllLayersEtaPhi {
public:
  AllLayersEtaPhi() { }
  virtual ~AllLayersEtaPhi() {
    unsigned int nLay = _etaphiInLayer.size();
    for(unsigned int iLay = 0; iLay < nLay; ++iLay) { delete _etaphiInLayer[iLay]; }
  }

  std::vector<EtaPhi*> _etaphiInLayer;
};

class EtaPhiWithVariance: public EtaPhi {
public:
  EtaPhiWithVariance(eflowEtaPhiPosition etaphi, double etaVar, double phiVar):
    EtaPhi(etaphi), _etaVariance(etaVar), _phiVariance(phiVar) { }
  EtaPhiWithVariance(): _etaVariance(NAN), _phiVariance(NAN) { }
  virtual ~EtaPhiWithVariance() { }

  double _etaVariance;
  double _phiVariance;
};


/* Cluster position provider classes */

class ClusterPlainEtaPhiProvider: public ClusterPositionProvider<EtaPhi> {
public:
  ClusterPlainEtaPhiProvider() { }
  virtual ~ClusterPlainEtaPhiProvider() { }

  EtaPhi* getPosition(const ICluster* cluster);
};

class ClusterGeometricalCenterProvider: public ClusterPositionProvider<EtaPhiWithVariance> {
public:
  ClusterGeometricalCenterProvider() { }
  virtual ~ClusterGeometricalCenterProvider() { }

private:
  EtaPhiWithVariance* getPosition(const ICluster* cluster);

  static const double m_etaPhiLowerLimit;
};

/* Track position provider classes */

class TrackEtaPhiInFixedLayersProvider: public TrackPositionProvider<EtaPhi> {
public:
  TrackEtaPhiInFixedLayersProvider(LayerType barrelLayer, LayerType endcapLayer):
    _barrelLayer(barrelLayer), _endcapLayer(endcapLayer) { }
  virtual ~TrackEtaPhiInFixedLayersProvider() { }

  EtaPhi* getPosition(const ITrack* track);

private:
  LayerType _barrelLayer;
  LayerType _endcapLayer;
};


/* The position provider factories */

class TrackPositionFactory {
public:
  static IPositionProvider* Get(std::string positionType) {
    if (positionType == "EM1EtaPhi") {
      return new TrackEtaPhiInFixedLayersProvider(TrackLayer::EMB1, TrackLayer::EME1);
    } else if (positionType == "EM2EtaPhi") {
      return new TrackEtaPhiInFixedLayersProvider(TrackLayer::EMB2, TrackLayer::EME2);
    } else {
      std::cerr << "TrackPositionFactory\tERROR\tInvalid track position type: \"" << positionType << "\"" << std::endl;
      assert(false);
      return 0;
    }
  }
};

class ClusterPositionFactory {
public:
  static IPositionProvider* Get(std::string positionType) {
    if (positionType == "PlainEtaPhi") {
      return new ClusterPlainEtaPhiProvider();
    } else if (positionType == "GeomCenterEtaPhi") {
      return new ClusterGeometricalCenterProvider();
    } else {
      std::cerr << "ClusterPositionFactory\tERROR\tInvalid cluster position type: \"" << positionType << "\"" << std::endl;
      assert(false);
      return 0;
    }
  }
};

}

#endif /* PFMATCHPOSITION_H_ */

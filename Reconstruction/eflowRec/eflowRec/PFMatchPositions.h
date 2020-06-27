/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    unsigned int nLay = m_etaphiInLayer.size();
    for(unsigned int iLay = 0; iLay < nLay; ++iLay) { delete m_etaphiInLayer[iLay]; }
  }

  std::vector<EtaPhi*> m_etaphiInLayer;
};

class EtaPhiWithVariance: public EtaPhi {
public:
  EtaPhiWithVariance(eflowEtaPhiPosition etaphi, double etaVar, double phiVar):
    EtaPhi(etaphi), m_etaVariance(etaVar), m_phiVariance(phiVar) { }
  EtaPhiWithVariance(): m_etaVariance(NAN), m_phiVariance(NAN) { }
  virtual ~EtaPhiWithVariance() { }

  double m_etaVariance;
  double m_phiVariance;
};


/* Cluster position provider classes */

class ClusterPlainEtaPhiProvider: public ClusterPositionProvider<EtaPhi> {
public:
  ClusterPlainEtaPhiProvider() { }
  virtual ~ClusterPlainEtaPhiProvider() { }

  EtaPhi getPosition(const ICluster* cluster) const;
};

class ClusterGeometricalCenterProvider: public ClusterPositionProvider<EtaPhiWithVariance> {
public:
  ClusterGeometricalCenterProvider() { }
  virtual ~ClusterGeometricalCenterProvider() { }

private:
  EtaPhiWithVariance getPosition(const ICluster* cluster) const;

  static const double m_etaPhiLowerLimit;
};

/* Track position provider classes */

class TrackEtaPhiInFixedLayersProvider: public TrackPositionProvider<EtaPhi> {
public:
  TrackEtaPhiInFixedLayersProvider(LayerType barrelLayer, LayerType endcapLayer, LayerType fcalLayer):
    m_barrelLayer(barrelLayer), m_endcapLayer(endcapLayer), m_fcalLayer(fcalLayer) { }
  virtual ~TrackEtaPhiInFixedLayersProvider() { }

  EtaPhi getPosition(const ITrack* track) const;

private:
  LayerType m_barrelLayer;
  LayerType m_endcapLayer;
  LayerType m_fcalLayer;
};


/* The position provider factories */

class TrackPositionFactory {
public:
  static std::unique_ptr<IPositionProvider> Get(std::string positionType) {
    if (positionType == "EM1EtaPhi") {
      return std::make_unique<TrackEtaPhiInFixedLayersProvider>(TrackLayer::EMB1, TrackLayer::EME1, TrackLayer::FCAL0);
    } else if (positionType == "EM2EtaPhi") {
      return std::make_unique<TrackEtaPhiInFixedLayersProvider>(TrackLayer::EMB2, TrackLayer::EME2, TrackLayer::FCAL0);
    } else {
      std::cerr << "TrackPositionFactory\tERROR\tInvalid track position type: \"" << positionType << "\"" << std::endl;
      assert(false);
      return 0;
    }
  }
};

class ClusterPositionFactory {
public:
  static std::unique_ptr<IPositionProvider> Get(std::string positionType) {
    if (positionType == "PlainEtaPhi") {
      return std::make_unique<ClusterPlainEtaPhiProvider>();
    } else if (positionType == "GeomCenterEtaPhi") {
      return std::make_unique<ClusterGeometricalCenterProvider>();
    } else {
      std::cerr << "ClusterPositionFactory\tERROR\tInvalid cluster position type: \"" << positionType << "\"" << std::endl;
      assert(false);
      return 0;
    }
  }
};

}

#endif /* PFMATCHPOSITION_H_ */

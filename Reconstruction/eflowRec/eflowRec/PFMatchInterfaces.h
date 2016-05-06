/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFMatchInterfaces.h
 *
 *  Created on: 03.04.2014
 *      Author: tlodd
 */

#ifndef PFMATCHINTERFACES_H_
#define PFMATCHINTERFACES_H_

#include <vector>
#include <cassert>

#include "eflowRec/eflowUtil.h"
#include "eflowRec/eflowCaloRegions.h"

namespace PFMatch {

typedef eflowCalo::LAYER LayerType;
#define TrackLayer eflowCalo

/* The track and cluster abstract interface classes */

class ITrack {
protected:
  ITrack() { }
public:
  virtual ~ITrack() { }

  virtual eflowEtaPhiPosition etaPhiInLayer(LayerType layer) const = 0;
};

class ICluster {
protected:
  ICluster() { }
public:
  virtual ~ICluster () { }

  virtual double eta() const = 0;
  virtual double phi() const = 0;
  virtual double etaMean() const = 0;
  virtual double phiMean() const = 0;
  virtual double etaVariance() const = 0;
  virtual double phiVariance() const = 0;
  virtual bool calVarianceStatus() const = 0;
  virtual void etaMean(double) const = 0;
  virtual void phiMean(double) const = 0;
  virtual void etaVariance(double) const = 0;
  virtual void phiVariance(double) const = 0;
  virtual void setCalVarianceStatus() const = 0;

  virtual unsigned int nCells() const = 0;
  virtual const std::vector<double>& cellPhi() const = 0;
  virtual const std::vector<double>& cellEta() const = 0;
};


/* Position base class */

class IPosition {
protected:
  IPosition() { }
public:
  virtual ~IPosition() { }
};


/* PositionProvider dummy base class */

class IPositionProvider {
protected:
  IPositionProvider() { }
public:
  virtual ~IPositionProvider() { }
};

/* PositionProviderInterface */

template <class ObjectType, class PositionType>
class PositionProvider: public IPositionProvider {
protected:
  PositionProvider() { m_position = new PositionType; }
  PositionProvider(const PositionProvider& originalPositionProvider) { m_position = new PositionType(*originalPositionProvider.getPosition()); }

public:
  virtual ~PositionProvider() { delete m_position; }
  virtual PositionType* getPosition(const ObjectType* cluster) = 0;

protected:
  PositionType* m_position;
};

template <class PositionType>
class ClusterPositionProvider: public PositionProvider<ICluster, PositionType> { };

template <class PositionType>
class TrackPositionProvider: public PositionProvider<ITrack, PositionType> { };

/* Distance calculator interface */

template<class TrackPositionType, class ClusterPositionType>
class DistanceCalculator {
protected:
  DistanceCalculator() { }
public:
  virtual ~DistanceCalculator() { }

  virtual double distanceBetween(TrackPositionType* position1, ClusterPositionType* position2) = 0;
};


/* The distance provider */

class IDistanceProvider {
protected:
  IDistanceProvider() { }
public:
  virtual ~IDistanceProvider() { }

  virtual double distanceBetween(const ITrack* track, const ICluster* cluster) = 0;
};

template<class TrackPositionType, class ClusterPositionType>
class DistanceProvider: public IDistanceProvider {
public:
  DistanceProvider(IPositionProvider*   trackPosition,
                   IPositionProvider* clusterPosition,
                   DistanceCalculator<TrackPositionType, ClusterPositionType>* distanceCalculator):
        m_distanceCalculator(distanceCalculator) {
    m_trackPosition   = dynamic_cast<TrackPositionProvider<TrackPositionType>*>(trackPosition);
    m_clusterPosition = dynamic_cast<ClusterPositionProvider<ClusterPositionType>*>(clusterPosition);
    assert(m_trackPosition);
    assert(m_clusterPosition);
    assert(m_distanceCalculator);
  }
  virtual ~DistanceProvider() {
    delete m_trackPosition;
    delete m_clusterPosition;
    delete m_distanceCalculator;
  }

  double distanceBetween(const ITrack* track, const ICluster* cluster) {
    return m_distanceCalculator->distanceBetween(m_trackPosition->getPosition(track),
                                                m_clusterPosition->getPosition(cluster));
  }

private:
  TrackPositionProvider<TrackPositionType>* m_trackPosition;
  ClusterPositionProvider<ClusterPositionType>* m_clusterPosition;
  DistanceCalculator<TrackPositionType, ClusterPositionType>* m_distanceCalculator;
};


}

#endif /* PFMATCHINTERFACES_H_ */

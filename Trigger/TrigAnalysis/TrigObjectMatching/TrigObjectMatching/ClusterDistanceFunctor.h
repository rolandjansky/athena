/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CLUSTER_DISTANCE_FUNCTOR_H
#define CLUSTER_DISTANCE_FUNCTOR_H

#include "TrigObjectMatching/DistanceFunctor.h"

class TrigEMCluster;
class TrigElectron;
class CaloCluster;
class EmTau_ROI;

/**
  * @brief helper namespace for calculating deltaR for unknown
  *        object types
  */
// What is the type of cluster returned by a given object?
namespace TrigMatch {
    template<class T> struct clusterTraits { typedef CaloCluster type; };
    template<> struct clusterTraits<TrigElectron> { typedef TrigEMCluster type; };
    template<> struct clusterTraits<EmTau_ROI> { typedef EmTau_ROI type; };
}
namespace ClusterDistanceHelper {
        // what the deltaR is between two clusters
        template<class T, class U>
            float deltaR(const T* t, const U* u) {
                return DeltaRDistanceFunctor<T,U>::deltaR(t, u);
            }
        // how to get the cluster for various types
        template <class T>
            const typename TrigMatch::clusterTraits<T>::type *getCluster(const T* t) { return t->cluster(); }
        inline const EmTau_ROI *getCluster(const EmTau_ROI* t) { return t; }
}

/**
  * @brief The ClusterDistanceFunctor is a templated distance functor
  *        used for matching two objects based upon their cluster rather
  *        than their eta and phi directly.
  */
template<class T, class U>
class ClusterDistanceFunctor : public DistanceFunctor<T,U> {

    public:
        ClusterDistanceFunctor() { };
        virtual ~ClusterDistanceFunctor() { };
    private:
        virtual float calculateDistance(const T *t, const U *u) const {
            if(!t || !u)
                return -1.;
            return ClusterDistanceHelper::deltaR(ClusterDistanceHelper::getCluster(t), 
                                                 ClusterDistanceHelper::getCluster(u));
        }

};

#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OBJECT_MATCHING_H
#define OBJECT_MATCHING_H

#include "TrigObjectMatching/DistanceFunctor.h"

#include "AthContainers/DataVector.h"
#include "CxxUtils/fpcompare.h"

#include <vector>

/**
 * @brief ObjectMatching is a base class that performs generic
 *        matching of objects using the distance functor framework
 */

class ObjectMatching {

  public:

    ObjectMatching() { ; };
    virtual ~ObjectMatching() { ; };

    /**
     * @brief distance returns the distance between the two supplied objects
     *        using metric, if available, or the default metric, if not.
     */
    template <typename T, typename U>
      float distance(
          const T *t, 
          const U *u,
          const DistanceFunctor<T, U> *metric) const;

    // default metric version - uses deltaR matching and so no metric
    // need be supplied
    template <typename T, typename U>
      float distance(
          const T *t, 
          const U *u) const;
    /**
     *
     * @brief matchToObjects returns the objects of type T from the supplied
     *        vector that match to matchObject, sorted by distance from
     *        the matchObject according to metric.
     */
    template <typename T, typename U>
      std::vector<const T*> matchToObjects(
          const U* matchObject,
          const std::vector<const T*> &targetObjects,
          float maxDistance,
          const DistanceFunctor<T, U> *metric) const;

    // default metric version - uses deltaR matching and so no metric
    // need be supplied
    template <typename T, typename U>
      std::vector<const T*> matchToObjects(
          const U* matchObject,
          const std::vector<const T*> &targetObjects,
          float maxDistance = 0.1) const;

    // similar matching functions for data vectors instead of std vectors
    template <typename T, typename U>
      std::vector<const T*> matchToObjects(
          const U* matchObject,
          const DataVector<T> &targetObjects,
          float maxDistance,
          const DistanceFunctor<T, U> *metric) const;

    template <typename T, typename U>
      std::vector<const T*> matchToObjects(
          const U* matchObject,
          const DataVector<T> &targetObjects,
          float maxDistance = 0.1) const;

    template <typename T, typename U>
      bool anyMatch(
          const U* matchObject,
          const std::vector<const T*> &targetObjects,
          float maxDistance,
          const DistanceFunctor<T, U> *metric) const;

    template <typename T, typename U>
      bool anyMatch(
          const U* matchObject,
          const std::vector<const T*> &targetObjects,
          float maxDistance) const;

    /**
     *
     * @brief matchToObject returns the object of type T from the suppled
     *        vector that best matches matchObject according to the distance
     *        metric, if available, or if not, according the the default
     *        metric.
     */
    template <typename T, typename U>
      const T* matchToObject(
          const U *matchObject,
          const std::vector<const T*> &targetObjects,
          float maxDistance,
          const DistanceFunctor<T, U> *metric) const;

    // default metric version - uses deltaR matching and so no metric
    // need be supplied
    template <typename T, typename U>
      const T* matchToObject(
          const U *matchObject,
          const std::vector<const T*> &targetObjects,
          float maxDistance = 0.1) const;

    // similar functions for data vectors rather than std::vectors
    template <typename T, typename U>
      const T* matchToObject(
          const U *matchObject,
          const DataVector<T> &targetObjects,
          float maxDistance,
          const DistanceFunctor<T, U> *metric) const;

    template <typename T, typename U>
      const T* matchToObject(
          const U *matchObject,
          const DataVector<T> &targetObjects,
          float maxDistance = 0.1) const;

  protected:

    // returns factory held default metric (deltaR)
    template <typename T, typename U>
      const DistanceFunctor<T,U> *prepareMetric() const;

  private:

    // functor for sorting by distance relative to a base object
    // with a given metric
    template <typename T, typename U>
      class DistanceCompare {
        public:
          DistanceCompare(const U *baseObject, const DistanceFunctor<T, U> *m) {
            m_metric = m;
            m_baseObject = baseObject;
          }
          virtual bool operator()(const T *x, const T *y) {
            // see Control/CxxUtils/fpcompare.h for explanation of
            // why this is required
            return CxxUtils::fpcompare::less((*m_metric)(x,m_baseObject),
                    (*m_metric)(y,m_baseObject));
          };
        private:
          const U *m_baseObject;
          const DistanceFunctor<T, U> *m_metric;
      };


}; // end ObjectMatching declaration

#include "TrigObjectMatching/ObjectMatching.icc"

#endif

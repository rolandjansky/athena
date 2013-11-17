/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DISTANCE_FUNCTOR_H
#define DISTANCE_FUNCTOR_H

#include <math.h>

template <typename T, typename U>
class DistanceFunctor {

  public:
    float operator()(const T *t, const U *u) const {
      return calculateDistance(t, u);
    }

    float distance(const T *t, const U *u) const {
      return (*this)(t, u);
    }

    virtual ~DistanceFunctor() {;}

  private:
    virtual float calculateDistance(const T *t, const U *u) const = 0;

}; // end class DistanceFunctor

template <typename T, typename U>
class DeltaRDistanceFunctor : virtual public DistanceFunctor<T, U> {
 
  public:

    virtual ~DeltaRDistanceFunctor() {;}

    static float deltaR(const T *t, const U *u) {

      if(!t || !u)
        return -1.;

      float phiDiff = t->phi() - u->phi();
      while(phiDiff < -M_PI) phiDiff += 2. * M_PI;
      while(phiDiff >= M_PI) phiDiff -= 2. * M_PI;
      float etaDiff = t->eta() - u->eta();
      return sqrt(phiDiff * phiDiff + etaDiff * etaDiff);

    }

  private:

    virtual float calculateDistance(const T *t, const U *u) const {
      return DeltaRDistanceFunctor<T,U>::deltaR(t, u);
    }

}; // end class DeltaRDistanceFunctor

#endif

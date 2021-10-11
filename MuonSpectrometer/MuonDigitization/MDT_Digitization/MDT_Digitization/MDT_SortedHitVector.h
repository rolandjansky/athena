/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DIGITIZATION_MDT_SORTEDHITVECTOR_H
#define MDT_DIGITIZATION_MDT_SORTEDHITVECTOR_H
#include "MuonSimEvent/MDTSimHit.h"
class mdt_hit_info {
public:
    mdt_hit_info(Identifier i, double t, double a, double r, const TimedHitPtr<MDTSimHit>* aHit) :
        id(i), time(t), adc(a), radius(r), simhit(aHit) {}
    mdt_hit_info(Identifier i, double t, double a) : id(i), time(t), adc(a) {}
    mdt_hit_info() : time(0.), adc(0.) {}
    Identifier id;
    double time;
    double adc;
    double radius;
    const TimedHitPtr<MDTSimHit>* simhit;
    bool operator<(const mdt_hit_info& aInfo) const {
        if (id < aInfo.id)
            return true;
        else if (id == aInfo.id)
            return time < aInfo.time;
        return false;
    }
};

// instead of using a map we use a sorted vector to temporary store the hits.

typedef std::vector<mdt_hit_info> HitVector;
typedef HitVector::iterator HitIt;

class MDT_SortedHitVector : public HitVector {
public:
    MDT_SortedHitVector() : m_isSorted(false){};
    void insert(const mdt_hit_info& hit);
    void sort();
    bool isSorted() { return m_isSorted; }

private:
    bool m_isSorted;
};
inline void MDT_SortedHitVector::sort() {
    std::stable_sort(HitVector::begin(), HitVector::end());
    m_isSorted = true;
}
inline void MDT_SortedHitVector::insert(const mdt_hit_info& hit) {
    push_back(hit);
    m_isSorted = false;
}

#endif

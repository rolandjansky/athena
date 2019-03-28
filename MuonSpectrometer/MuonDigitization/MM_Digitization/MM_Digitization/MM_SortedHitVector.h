/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MMSORTEDHITVECTOR_H
#define MM_DIGITIZATION_MMSORTEDHITVECTOR_H
/*******************************************************************************/
class micromegas_hit_info {
 public:
  micromegas_hit_info(Identifier i,double t,int ch,double strP,const TimedHitPtr<MMSimHit>* aHit) :
    id(i),time(t),charge(ch),stripPos(strP),simhit(aHit) {}
    micromegas_hit_info(Identifier i,double t,int ch) : id(i),time(t),charge(ch),stripPos(0),simhit(0) {}
      micromegas_hit_info() : time(0.),charge(0),stripPos(0),simhit(0) {}
	Identifier id;
	double time;
	int charge;
	double stripPos;
	const TimedHitPtr<MMSimHit>* simhit;
	bool operator < (const micromegas_hit_info& aInfo) const
	{
	  if(id <  aInfo.id)
	    return true;
	  else if(id == aInfo.id)
	    return time < aInfo.time;
	  return false;
	}
};
/*******************************************************************************/
// instead of using a map we use a sorted vector to temporary store the hits.
typedef std::vector<micromegas_hit_info> HitVector;
typedef HitVector::iterator HitIt;
/*******************************************************************************/
class MmSortedHitVector : public HitVector {
 public:
  MmSortedHitVector() : m_isSorted(false) {};
  void insert(const micromegas_hit_info& hit);
  void sort();
  bool isSorted() { return m_isSorted; }
 private:
  bool m_isSorted;
};
/*******************************************************************************/
inline void MmSortedHitVector::sort() {
  std::stable_sort(HitVector::begin(),HitVector::end());
  m_isSorted = true;
}
/*******************************************************************************/
inline void MmSortedHitVector::insert(const micromegas_hit_info& hit) {
  push_back(hit);
  m_isSorted = false;
}
/*******************************************************************************/
#endif

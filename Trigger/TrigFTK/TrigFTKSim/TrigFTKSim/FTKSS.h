/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKSS_H
#define FTKSS_H

#include "FTKHit.h"
#include "FTKRoad.h"
#include "MultiTruth.h"

#include <Rtypes.h>

#include <vector>

/** This class describes the hits in a single SS. 
    A SS is described at different levels, in HW this correspond
    to a different bit number to describe the SSs, i.e. AM SS and TSP SS.
    Because this, to be general the list of hits should be built using
    the builtHitsList() method, that is supposed to navigate trough 
    the different levels to accumulate all the hits.
    The m_hits vector should be empty in all the splitted levels.
*/
class FTKSS {
private:
  
  std::vector<FTKHit> m_hits; // actual hits
  
  std::vector<FTKSS> m_subSS; // vector of sub-superstrips

public:
  FTKSS();
  virtual ~FTKSS();

  void addSubStrips(const std::vector<FTKSS> &substrips) 
  { m_subSS = substrips; }
  int getNSubStrips() const { return m_subSS.size(); }

  void addHit(const FTKHit& v) { m_hits.push_back(v); }
  void addHit(int iss, const FTKHit &v) { m_subSS[iss].addHit(v); }

  bool hasHits() const;
  bool hasHits(int iss) const { return m_subSS[iss].hasHits(); }

  int getNHits() const { return m_hits.size(); }
  int getNHits(int iss) const { return m_subSS[iss].getNHits(); }

  const std::vector<FTKHit>& getHits() const { return m_hits; }
  const std::vector<FTKHit>& getHits(int iss) const { return m_subSS[iss].getHits(); }
  
  void buildList(std::vector<FTKHit>& list,int mask=-1) const;

  void copyHitsToRoad(FTKRoad *,int ipl);

  const FTKHit& getHit(int id) const { return m_hits[id]; }
  const FTKHit& getHit(int iss, int id) const { return m_subSS[iss].getHit(id); }
  
  const MultiTruth& getTruth(int id) const { return m_hits[id].getTruth(); }
  const MultiTruth& getTruth(int iss, int id) const { return m_subSS[iss].m_hits[id].getTruth(); }

  const FTKSS& getSubSS(int iss) const { return m_subSS[iss]; }

  ClassDef(FTKSS,5)
};

#endif // FTKSS_H

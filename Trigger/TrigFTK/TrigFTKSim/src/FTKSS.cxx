/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSS.h"
#include "TrigFTKSim/MultiTruth.h"

#include <cassert>
using namespace std;

FTKSS::FTKSS()
  : m_hits(), m_subSS() 
{}

FTKSS::~FTKSS()
{
  m_subSS.clear();
  m_hits.clear();
}



/** */
bool FTKSS::hasHits() const
{
  // if list of hits in this SS has hits is ok
  if (!m_hits.empty()) return true;
  
  // if the list is empty maybe there are hits in the sub-SS
  vector<FTKSS>::const_iterator ss_i = m_subSS.begin();
  for ( ;ss_i!=m_subSS.end();++ss_i) {
    if ((*ss_i).hasHits()) return true;
  }

  // this could happen in the sub-SS not in a main SS
  return false;
}

/** This method copy the hits in this SS in a given road.
    The plane argument specifies in which layer the SS is 
    related to this pattern */
void FTKSS::copyHitsToRoad(FTKRoad *road, int plane)
{
  // check if the SS has sub-SSs
  if (m_subSS.empty()) {
    assert(road->getNSubRoads()==0); // verify both the SS and the road don't use TSP
    // if the TSP is not used just pass the hits to the road
    road->addHits(plane,m_hits);
    return; // done
  }

  // the mask with 1 bit per subSS is stored in the road
  const int &fld_mask = road->getSubSSMask(plane);  
  // the current list of hits is update according the mask
  vector<FTKHit> tmplst;
  buildList(tmplst,fld_mask);

  // add the list of hits to this planes
  road->addHits(plane,tmplst);

  for (int isr=0;isr!=road->getNSubRoads();++isr) { // subroad loop
    FTKRoad *subroad = road->getSubRoad(isr);
    int pos = subroad->getSSID(plane);

    if (m_subSS[pos].m_hits.empty()) continue; // skip if empty

    m_subSS[pos].copyHitsToRoad(subroad,plane);
  } // end subroad loop
}


void FTKSS::buildList(vector<FTKHit> &hitlist, int mask) const
{
  if (m_subSS.empty()) {
    // this is the final unsplitted SS, append the hits in the global list
    hitlist.insert(hitlist.end(),m_hits.begin(),m_hits.end());
    return;
  }

  /* check with of the sub SS match the mask */
  for (unsigned is=0;is!=m_subSS.size();++is) {
    if (mask&(1<<is)) m_subSS[is].buildList(hitlist);
  }
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKRoadFileInput.h"

using namespace std;

int FTKRoadFileInput::nextEvent()
{
  int res(0);

  if (m_entry>=m_max_entry || !m_current_tree) {
    if (nextFile()<0) {
      // end of file list
      return -1;
    }
    else {
      // will return that a new file is open
      res = 1;
    }
  }
  
  // get this entry and move the counter
  m_current_tree->GetEntry(m_entry++);
  // m_data[0]->Print(true); // hardcode cy debug
 
  // reset road and bank position for the following call within the
  // TrackFitter loop over the roads
  // Also need to rebuild the map for FTKRoadStream::findRoad
  for (int ib=0;ib!=m_nbanks;++ib) {
    // check that this bank is enabled, not 0x0 pointer, or skip the bank
    if (!m_data[ib]) continue;

    if (m_cur_object[ib]) {
      m_data[ib]->detachHits(m_cur_iobject[ib]);
    }
    m_cur_iobject[ib] = -1;
    m_cur_object[ib] = 0;
    m_data[ib]->buildRoadMap();
    // expand the roads to prepare the list of the road to fit
    m_data[ib]->prepareFitList();
  }

  return res;
}


/** this function go to the next road, moving to the next bank if needed
    and return the pointer to the current road. return 0 if all the roads
    were used */
const FTKRoad* FTKRoadFileInput::nextRoad(int ibank)
{
  // if the bank isn't active a NULL is returned
  if (!m_data[ibank]) return 0x0;

  // get the road
  m_cur_object[ibank] = m_data[ibank]->fetchRoad();

  // check if the bank has enough roads
  if (!m_cur_object[ibank]) {
    m_cur_object[ibank] = 0;
  }
  else {
    // increment road counter
    m_cur_iobject[ibank] += 1; 
  }

  return m_cur_object[ibank];
}

/** this function go to the first road and return the pointer to the
    current road. return 0 if all the roads were used */
const FTKRoad* FTKRoadFileInput::firstRoad(int ibank)
{
  // if the bank isn't active a NULL is returned
  if (!m_data[ibank]) return 0x0;

  // get the road
  m_cur_object[ibank] = m_data[ibank]->fetchFirstRoad();
  m_cur_iobject[ibank] = 0; 

  // check if the bank has enough roads
  if (!m_cur_object[ibank]) {
    m_cur_object[ibank] = 0;
  }
  else {
    // increment road counter
    m_cur_iobject[ibank] += 1; 
  }

  return m_cur_object[ibank];
}

// retrieve superstrip by (bank,plane,ssid)
const FTKSS&
FTKRoadFileInput::getSS(const unsigned int& ibank,const unsigned int& iplane,const unsigned int& ssid) const
{
  return( m_data[ibank]->getSS(iplane,ssid) );
}


// retrieve superstrip by (bank,plane,ssid) in the list of unused hits
const FTKSS&
FTKRoadFileInput::getUnusedSS(const unsigned int& ibank,const unsigned int& iplane,const unsigned int& ssid) const
{
  return( m_data[ibank]->getUnusedSS(iplane,ssid) );
}


FTKRoad* FTKRoadFileInput::getRoad(int region,int roadid,int bankid) {
  int roadidx = m_data[region]->findRoad(roadid,bankid);
  m_data[region]->attachHits(roadidx);
  return m_data[region]->getRoad(roadidx);
}

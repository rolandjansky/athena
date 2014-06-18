/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKTrackInput.h"

using namespace std;

int FTKTrackInput::nextEvent()
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
  // m_data[0]->Print();  // printing track cy debug hardcoded region

  // reset track and bank position
  // Also need to rebuild the map for FTKTrackStream::findTrack
  for (int ib=0;ib!=m_nbanks;++ib) {
    //  if(cur_object[ib]) m_data[ib]->Print(); // cy experiment
    cur_iobject[ib] = -1;
    cur_object[ib] = 0;
    m_data[ib]->buildTrackMap();
  }

  return res;
}


/** this function go to the next track, moving to the next bank if needed
    and return the pointer to the current track. return 0 if all the tracks
    were used.
    *This function is used to read in SCT-only tracks in Constantinos case*
 */
const FTKTrack* FTKTrackInput::nextTrack(int ibank)
{
  cur_iobject[ibank] += 1; // increment track counter
  // check if the bank has enough tracks
  if (cur_iobject[ibank]==m_data[ibank]->getNTracks()) {
    cur_iobject[ibank] -= 1; // return to the previous element
    cur_object[ibank] = 0;
  }
  else {
    // return the track
    cur_object[ibank] = m_data[ibank]->getTrack(cur_iobject[ibank]);
  }

  return cur_object[ibank];
}

const FTKTrack* FTKTrackInput::getTrack(int region,int trackid,int bankid) const {
  int trackidx = m_data[region]->findTrack(trackid,bankid);
  return m_data[region]->getTrack(trackidx);
}

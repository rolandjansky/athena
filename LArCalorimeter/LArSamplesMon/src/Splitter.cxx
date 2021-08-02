/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/Splitter.h"

#include "LArSamplesMon/Interface.h"
#include "LArSamplesMon/FilterParams.h"
#include "LArSamplesMon/DataTweaker.h"

#include <vector>
#include <map>
#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


bool Splitter::splitEvents(TString name, unsigned int max) const
{
  std::map<unsigned int, bool> events;
  
  cout << "Scanning ntuple..." << endl;
  for (unsigned int i = 0; i < m_interface->nChannels(); i++) {
    const History* history = m_interface->cellHistory(i);
    if (!history) continue;
    for (unsigned int k = 0; k < history->nData(); k++)
      events[history->data(k)->event()] = true;
  }
   
  cout << "Found " << events.size() << " events";
  if (events.size() > max) {
    cout << ", greater than specified maximum (" << max << "), aborting..." << endl;
    return false;
  }
  cout << ", will now split the ntuple" << endl;
  
  unsigned int i = 1;
  for (std::map<unsigned int, bool>::const_iterator event = events.begin();
       event != events.end(); ++event, ++i) {
    cout << "Processing event " << event->first << " (" << i << " of " << events.size() << ")" << endl;
    FilterParams f;
    f.addEvent(event->first);
    m_interface->filter(f, DataTweaker(), Form("%s_%d.root", name.Data(), event->first));
  }

  return true;
}


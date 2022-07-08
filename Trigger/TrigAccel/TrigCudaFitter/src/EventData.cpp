#include <stdlib.h>
#include "McTrack.h"
#include "RecTrack.h"
#include "EventData.h"

EventData::EventData() : m_eventNumber(0)
{
  m_mcTrack=NULL;m_tracks.clear();
}


EventData::~EventData()
{
  delete m_mcTrack;

  for(std::vector<const RecTrack*>::iterator it=m_tracks.begin(); it!=m_tracks.end();++it)
    delete (*it);
}

void EventData::setEventNumber(int n)
{
  m_eventNumber=n;
}

void EventData::setMcTrack(const McTrack* ptrack)
{
  m_mcTrack=ptrack;
}

void EventData::addRecTrack(const RecTrack* ptrack)
{
  m_tracks.push_back(ptrack);
}

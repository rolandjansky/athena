#ifndef __EVENTDATA_H__
#define __EVENTDATA_H__

#include <vector>

class RecTrack;
class McTrack;

class EventData
{
  public:
    EventData();
    ~EventData();

    void setEventNumber(int n);
    void setMcTrack(const McTrack* ptrack);
    void addRecTrack(const RecTrack* ptrack);

    std::vector<const RecTrack*> m_tracks;
    const McTrack* m_mcTrack;
    int m_eventNumber;
};

#endif

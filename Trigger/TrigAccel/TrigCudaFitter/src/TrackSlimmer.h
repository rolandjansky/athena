#pragma once
#include<vector>

class RecTrack;
struct TrackInfoStruct;

class TrackSlimmer
{
  public:
    TrackSlimmer(void);
    ~TrackSlimmer(void);

    void processTracks(std::vector<const RecTrack*>&);

  private:
    std::vector<struct TrackInfoStruct*> m_vpTIS;
};

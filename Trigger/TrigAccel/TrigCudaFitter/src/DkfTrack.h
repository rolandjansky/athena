#ifndef __DKFTRACK_H__
#define __DKFTRACK_H__

#include <vector>

class TrkTrackState;
class TrkBaseNode;
class TrkPlanarSurface;
class RecTrack;

class DkfTrack
{
  public:
    DkfTrack(TrkTrackState*,const RecTrack*);
    virtual ~DkfTrack(void);

    TrkTrackState* m_pTrackState;
    std::vector<TrkBaseNode*> m_vpNodes;
    const RecTrack* m_pRecTrack;
    std::vector<const TrkPlanarSurface*> m_vpSurfaces;
    std::vector<TrkTrackState*> m_vpTrackStates;

  private:
    double m_dChi2;
    int m_nNDOF;

  public:
    double getChi2(void);
    void addChi2(double,int);
    int getNDOF(void);
};

#endif

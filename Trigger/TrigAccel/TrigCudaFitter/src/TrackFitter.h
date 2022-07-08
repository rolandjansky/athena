#ifndef __TRACK_FITTER_H__
#define __TRACK_FITTER_H__

class RecTrack;
class DkfTrack;
class TrkPlanarSurface;
class TrkTrackState;

class TrackFitter
{
  public:
    TrackFitter(void);
    virtual ~TrackFitter(void);

    virtual DkfTrack* fit(const RecTrack*);

  protected:
    virtual TrkTrackState* extrapolate(TrkTrackState* pTS, TrkPlanarSurface* pSB, TrkPlanarSurface* pSE,double J[5][5],double& path);
    void matrixInversion5x5(double a[5][5]);
    virtual DkfTrack* createDkfTrack(const RecTrack* inputTrack);

    double m_startB[3];
    double m_endB[3];
    std::vector<int> m_reMapVector;
};

#endif

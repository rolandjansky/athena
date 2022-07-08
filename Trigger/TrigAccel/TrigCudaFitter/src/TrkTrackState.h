//////////////////////////////////////////////////////////////////
// TrkTrackState.h
//   Header file for class TrkTrackState
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////

#ifndef __TRK_TRACK_STATE_H__
#define __TRK_TRACK_STATE_H__

class TrkBaseNode;
class TrkPlanarSurface;
class TrkTrackState
{
  public:
    TrkTrackState();
    TrkTrackState(const double[5]);
    TrkTrackState(const TrkTrackState*);
    ~TrkTrackState(){};

    void report();
    void resetCovariance();
    void serialize(char fileName[]);
    TrkPlanarSurface* getSurface();
    void attachToSurface(TrkPlanarSurface*);
    void setScatteringMode(int);
    int getScatteringMode();
    void applyMaterialEffects();
    void applyMultipleScattering();
    void updateTrackState(double*);
    void updateTrackCovariance(double*);
    void setTrackCovariance(double A[5][5]);
    void setTrackState(double A[5]);
    void setSmootherGain(double A[5][5]);
    void setPreviousState(TrkTrackState*);
    void runSmoother();
    inline double getTrackState(int i)
    {
      return m_Rk[i];
    }
    inline double getTrackCovariance(int i, int j)
    {
      return m_Gk[i][j];
    }
    inline int getStateSize()
    {
      return 5;
    }

  protected:
    double m_Rk[5],m_Re[5];
    double m_Gk[5][5],m_Ge[5][5];
    int m_scattMode;
    bool m_isScattered;
    TrkPlanarSurface* m_pSurface;
    TrkTrackState* m_pPrevState;
    double m_A[5][5];

  private:
    void applyEnergyLoss(int);
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkTrackState.h
//   Header file for class TrkTrackState
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////

#ifndef __TRK_TRACKSTATE_H__
#define __TRK_TRACKSTATE_H__


namespace Trk {	
  class TrkBaseNode;
  class TrkPlanarSurface;
  class MeasuredPerigee;
  class TrkTrackState
    {
    public:
      TrkTrackState();
      TrkTrackState(double[5]);
      TrkTrackState(const TrkTrackState*);
      ~TrkTrackState(){};
      void m_report();
      void m_resetCovariance();
      void m_serialize(char fileName[]);
      TrkPlanarSurface* m_getSurface();
      void m_attachToSurface(TrkPlanarSurface*);
      void m_setScatteringMode(int);
      int m_getScatteringMode();
      void m_applyMaterialEffects();
      void m_updateTrackState(double*);
      void m_updateTrackCovariance(double*);
      void m_setTrackCovariance(double A[5][5]);
      void m_setTrackState(double A[5]);
      void m_setSmootherGain(double A[5][5]);
      void m_setPreviousState(TrkTrackState*);
      void m_runSmoother();
      void m_applyMultipleScattering();
      void m_applyEnergyLoss(int);
      inline double m_getTrackState(int i)
	{
	  return m_Rk[i];
	}
      inline double m_getTrackCovariance(int i, int j)
	{
	  return m_Gk[i][j];
	}
      inline int m_getStateSize()
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
    };
}

#endif

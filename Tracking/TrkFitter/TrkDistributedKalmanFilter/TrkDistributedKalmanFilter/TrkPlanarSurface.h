/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkPlanarSurface.h
//   Header file for class TrkPlanarSurface
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////

#ifndef TRKDISTRIBUTEDKALMANFILTER_TRK_PLANAR_SURFACE_H
#define TRKDISTRIBUTEDKALMANFILTER_TRK_PLANAR_SURFACE_H

namespace Trk {	
  class Surface;
}

namespace Trk
{
  class TrkPlanarSurface
    {
    public:
      TrkPlanarSurface(double Center[3],
		       double Normal[3], double Transform[3][3], double, const Surface* pS = 0);
      ~TrkPlanarSurface();
      const double* getParameters();
      double getPar(int);
      double radiusVector();
      const double* getCenter();
      const double* getNormal();
      double getRotMatrix(int,int);
      double getInvRotMatrix(int,int);
      double getRadLength();
      double calculateCombinedMatrix(int,int,TrkPlanarSurface*);
      void rotateVectorToLocal(double*,double*);
      void rotateVectorToGlobal(double*,double*);
      void transformPointToLocal(double*,double*);
      void transformPointToGlobal(double*,double*);
      void setBreakPoint(double);
      void unsetBreakPoint(){m_breakPoint=false;}
      bool isBreakPoint();
      double getInput();
      void report();
      const Surface* getTrkSurface();
    private:
      void calculateInverseTransform();
      double m_Params[4];
      double m_Center[3];
      double m_Normal[3];
      double m_M[3][3];
      double m_L[3][3];
      double m_Rc;
      double m_radLength;
      double m_input;
      bool m_breakPoint;
      const Surface* m_trkSurface;
    };
}


#endif

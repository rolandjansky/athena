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

#ifndef __TRK_PLANAR_SURFACE_H__
#define __TRK_PLANAR_SURFACE_H__

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
      const double* m_getParameters();
      double m_getPar(int);
      double m_radiusVector();
      const double* m_getCenter();
      const double* m_getNormal();
      double m_getRotMatrix(int,int);
      double m_getInvRotMatrix(int,int);
      double m_getRadLength();
      double m_calculateCombinedMatrix(int,int,TrkPlanarSurface*);
      void m_rotateVectorToLocal(double*,double*);
      void m_rotateVectorToGlobal(double*,double*);
      void m_transformPointToLocal(double*,double*);
      void m_transformPointToGlobal(double*,double*);
      void m_setBreakPoint(double);
      void m_unsetBreakPoint(){m_breakPoint=false;}
      bool m_isBreakPoint();
      double m_getInput();
      void m_report();
      const Surface* m_getTrkSurface();
    private:
      void m_calculateInverseTransform();
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

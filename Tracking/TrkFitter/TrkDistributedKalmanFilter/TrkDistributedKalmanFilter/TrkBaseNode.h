/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkBaseNode.h old
//   Header file for class TrkBaseNode
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////

#ifndef __TRK_BASENODE_H__
#define __TRK_BASENODE_H__

namespace Trk {	
  class TrkTrackState;
  class TrkPlanarSurface;
  class PrepRawData;
  class RIO_OnTrack;
  class TrkBaseNode {
  public:
    TrkBaseNode();
    virtual ~TrkBaseNode();
    virtual void m_validateMeasurement(TrkTrackState*) = 0;
    virtual void m_updateTrackState(TrkTrackState*) = 0;
    virtual void m_report() = 0;
    virtual bool m_isValidated();
    virtual void m_setNodeState(int);
    virtual int m_getNodeState();
    virtual void m_setNodeType(char);
    virtual char m_getNodeType();
    virtual void m_updateInternal();
    virtual void m_updateWithRIO(const RIO_OnTrack*) = 0;

    TrkTrackState* m_getTrackState();
    virtual TrkPlanarSurface* m_getSurface();
    virtual const PrepRawData* m_getPrepRawData();
    virtual void m_serialize(char fileName[]) = 0;
    double m_getChi2();
    virtual double m_getChi2Distance(TrkTrackState*)=0;
    int m_getNdof();
    virtual int m_getKalmanGain(double[5][2]) = 0;
    virtual int m_getResiduals(double[2]) = 0;
    virtual int m_getInverseResidualVariance(double[2][2]) = 0;
    virtual int m_getMeasurementMatrix(double[2][5]) = 0;

  protected:
    int m_nodeState;
    double m_chi2Cut;
    char m_nodeType;
    TrkPlanarSurface* m_pSurface;
    const PrepRawData* m_pPRD;
    double m_dChi2;
    int m_ndof;
    TrkTrackState* m_pTrackState;
  };
}
#endif

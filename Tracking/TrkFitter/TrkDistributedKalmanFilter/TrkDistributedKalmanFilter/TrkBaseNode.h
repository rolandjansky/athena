/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#ifndef TRKDISTRIBUTEDKALMANFILTER_TRK_BASENODE_H
#define TRKDISTRIBUTEDKALMANFILTER_TRK_BASENODE_H

namespace Trk {	
  class TrkTrackState;
  class TrkPlanarSurface;
  class PrepRawData;
  class RIO_OnTrack;
  class TrkBaseNode {
  public:
    TrkBaseNode();
    virtual ~TrkBaseNode();
    virtual void validateMeasurement(TrkTrackState*) = 0;
    virtual void updateTrackState(TrkTrackState*) = 0;
    virtual void report() = 0;
    virtual bool isValidated();
    virtual void setNodeState(int);
    virtual int getNodeState();
    virtual void setNodeType(char);
    virtual char getNodeType();
    virtual void updateInternal();
    virtual void updateWithRIO(const RIO_OnTrack*) = 0;

    TrkTrackState* getTrackState();
    virtual TrkPlanarSurface* getSurface();
    virtual const PrepRawData* getPrepRawData();
    virtual void serialize(char fileName[]) = 0;
    double getChi2() const;
    virtual double getChi2Distance(TrkTrackState*)=0;
    int getNdof() const;
    virtual int getKalmanGain(double[5][2]) = 0;
    virtual int getResiduals(double[2]) = 0;
    virtual int getInverseResidualVariance(double[2][2]) = 0;
    virtual int getMeasurementMatrix(double[2][5]) = 0;

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

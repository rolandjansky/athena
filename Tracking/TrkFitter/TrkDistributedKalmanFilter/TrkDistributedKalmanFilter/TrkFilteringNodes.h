/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkFilteringNodes.h
//   Header file for classes representing filtering nodes
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////

#ifndef TRKDISTRIBUTEDKALMANFILTER_TRK_FILTERING_NODES_H
#define TRKDISTRIBUTEDKALMANFILTER_TRK_FILTERING_NODES_H

#include "TrkDistributedKalmanFilter/TrkBaseNode.h"



namespace Trk {	
/** inheriting from this, should not fwd declare
	class TrkBaseNode;
**/
	class TrkTrackState;
	class TrkPlanarSurface;

	class PrepRawData;

  class TrkFilteringNode1D : public TrkBaseNode
    {
    public:
      TrkFilteringNode1D():m_B{},m_D{},m_H{},m_K{},m_resid{},m_m{},m_V{}{/** nop **/};
      virtual ~TrkFilteringNode1D(){};
      virtual int getKalmanGain(double[5][2]);
      virtual int getResiduals(double[2]);
      virtual int getInverseResidualVariance(double[2][2]);
      virtual int getMeasurementMatrix(double[2][5]);
      virtual void runKalmanFilter(TrkTrackState*);
      virtual double calculateChi2();
    protected:
      double m_B[5],m_D,m_H[5],m_K[5];
      double m_resid;
      double m_m;
      double m_V;
    };

  class TrkFilteringNode2D : public TrkBaseNode 
    {
    public:
      TrkFilteringNode2D():m_B{},m_D{},m_H{},m_K{},m_resid{},m_m{},m_V{}{/** nop **/};
      virtual ~TrkFilteringNode2D(){};
      virtual int getKalmanGain(double[5][2]);
      virtual int getResiduals(double[2]);
      virtual int getInverseResidualVariance(double[2][2]);
      virtual int getMeasurementMatrix(double[2][5]);
      virtual void runKalmanFilter(TrkTrackState*);
      virtual double calculateChi2();
    protected:
      double m_B[5][2],m_D[2][2],m_H[2][5],m_K[5][2];
      double m_resid[2];
      double m_m[2];
      double m_V[2][2];
    };

  class TrkClusterNode : public TrkFilteringNode1D
    {
    public:
      TrkClusterNode(TrkPlanarSurface*,double,const PrepRawData*);
      TrkClusterNode(TrkPlanarSurface*,double,double,double);
      virtual ~TrkClusterNode(){};
      virtual void validateMeasurement(TrkTrackState*);
      virtual void updateTrackState(TrkTrackState*);
      virtual double getChi2Distance(TrkTrackState*);
      virtual void serialize(char fileName[]);
      virtual void report(){};
      virtual void updateWithRIO(const RIO_OnTrack*);
    };

  class TrkEndCapClusterNode : public TrkFilteringNode1D
    {
    public:
      TrkEndCapClusterNode(TrkPlanarSurface*,double,const PrepRawData*);
      TrkEndCapClusterNode(TrkPlanarSurface*,double,const PrepRawData*,double);
      TrkEndCapClusterNode(TrkPlanarSurface*,double,double,double,double);
      virtual ~TrkEndCapClusterNode(){};
      virtual void validateMeasurement(TrkTrackState*);
      virtual void updateTrackState(TrkTrackState*);
      virtual double getChi2Distance(TrkTrackState*);
      virtual void serialize(char fileName[]);
      virtual void report(){};
      virtual void updateWithRIO(const RIO_OnTrack*);
    protected:
      double m_Rc;
    };

  class TrkPixelNode : public TrkFilteringNode2D
    {
    public:
      TrkPixelNode(TrkPlanarSurface*,double,const PrepRawData*);
      TrkPixelNode(TrkPlanarSurface*,double,const double[2],const double[4]);
      virtual ~TrkPixelNode(){};
      virtual void validateMeasurement(TrkTrackState*);
      virtual void updateTrackState(TrkTrackState*);
      virtual double getChi2Distance(TrkTrackState*);
      virtual void serialize(char fileName[]);
      virtual void report();
      virtual void updateWithRIO(const RIO_OnTrack*);
    };
  
  class TrkTrtNode : public TrkFilteringNode1D
    {
    public:
      TrkTrtNode(TrkPlanarSurface*,double,double,double,const PrepRawData*);
      virtual ~TrkTrtNode(){};
      virtual void validateMeasurement(TrkTrackState*);
      virtual void updateTrackState(TrkTrackState*);
      virtual double getChi2Distance(TrkTrackState*);
      virtual void serialize(char fileName[]);
      virtual void updateInternal();
      virtual void report(){};
      virtual void updateWithRIO(const RIO_OnTrack*);
    protected:
      double m_minBound,m_maxBound;
      char m_lrSign;
      bool m_freezeLR;
    };

}

#endif

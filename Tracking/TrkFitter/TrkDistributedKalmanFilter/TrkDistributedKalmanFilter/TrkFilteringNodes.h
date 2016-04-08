/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#ifndef __TRK_FILTERING_NODES_H__
#define __TRK_FILTERING_NODES_H__

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
      virtual int m_getKalmanGain(double[5][2]);
      virtual int m_getResiduals(double[2]);
      virtual int m_getInverseResidualVariance(double[2][2]);
      virtual int m_getMeasurementMatrix(double[2][5]);
      virtual void m_runKalmanFilter(TrkTrackState*);
      virtual double m_calculateChi2();
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
      virtual int m_getKalmanGain(double[5][2]);
      virtual int m_getResiduals(double[2]);
      virtual int m_getInverseResidualVariance(double[2][2]);
      virtual int m_getMeasurementMatrix(double[2][5]);
      virtual void m_runKalmanFilter(TrkTrackState*);
      virtual double m_calculateChi2();
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
      virtual void m_validateMeasurement(TrkTrackState*);
      virtual void m_updateTrackState(TrkTrackState*);
      virtual double m_getChi2Distance(TrkTrackState*);
      virtual void m_serialize(char fileName[]);
      virtual void m_report(){};
      virtual void m_updateWithRIO(const RIO_OnTrack*);
    };

  class TrkEndCapClusterNode : public TrkFilteringNode1D
    {
    public:
      TrkEndCapClusterNode(TrkPlanarSurface*,double,const PrepRawData*);
      TrkEndCapClusterNode(TrkPlanarSurface*,double,const PrepRawData*,double);
      TrkEndCapClusterNode(TrkPlanarSurface*,double,double,double,double);
      virtual ~TrkEndCapClusterNode(){};
      virtual void m_validateMeasurement(TrkTrackState*);
      virtual void m_updateTrackState(TrkTrackState*);
      virtual double m_getChi2Distance(TrkTrackState*);
      virtual void m_serialize(char fileName[]);
      virtual void m_report(){};
      virtual void m_updateWithRIO(const RIO_OnTrack*);
    protected:
      double m_Rc;
    };

  class TrkPixelNode : public TrkFilteringNode2D
    {
    public:
      TrkPixelNode(TrkPlanarSurface*,double,const PrepRawData*);
      TrkPixelNode(TrkPlanarSurface*,double,double[2],double[4]);
      virtual ~TrkPixelNode(){};
      virtual void m_validateMeasurement(TrkTrackState*);
      virtual void m_updateTrackState(TrkTrackState*);
      virtual double m_getChi2Distance(TrkTrackState*);
      virtual void m_serialize(char fileName[]);
      virtual void m_report();
      virtual void m_updateWithRIO(const RIO_OnTrack*);
    };
  
  class TrkTrtNode : public TrkFilteringNode1D
    {
    public:
      TrkTrtNode(TrkPlanarSurface*,double,double,double,const PrepRawData*);
      virtual ~TrkTrtNode(){};
      virtual void m_validateMeasurement(TrkTrackState*);
      virtual void m_updateTrackState(TrkTrackState*);
      virtual double m_getChi2Distance(TrkTrackState*);
      virtual void m_serialize(char fileName[]);
      virtual void m_updateInternal();
      virtual void m_report(){};
      virtual void m_updateWithRIO(const RIO_OnTrack*);
    protected:
      double m_minBound,m_maxBound;
      char m_lrSign;
      bool m_freezeLR;
    };

}

#endif

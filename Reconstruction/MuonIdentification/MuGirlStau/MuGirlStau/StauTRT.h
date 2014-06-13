/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGRILNSSTAUTRT_H_
#define MUGRILNSSTAUTRT_H_

#include "GaudiKernel/MsgStream.h"
#include "MuGirlStau/StauToolDefinitions.h"
#include "MuGirlStau/StauTool.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "InDetCosmicsEventPhase/IInDetCosmicsEventPhaseTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <vector>
#include "TVector3.h"
namespace MuGirlNS
{



struct TRTHit
{
	double driftRadius; 	// distance from track to the wire
	double driftTime;   	// time for driftRadius
	double rawTime;     	// measured time
	double t0; 	 	// calibrated delay (electronics + tof from db)
	double error;		// error (time resultion)
	TVector3 globalPosition;// hit position
	TVector3 detectorCenter;	
	TRTHit():
	driftRadius(0.),
	driftTime(0.),
	rawTime(0.),
	t0(0.),
	error(0.),
	globalPosition(0.),
	detectorCenter(0.)
	{
	}
	
	TRTHit(	double in_driftRadius, 
		double in_driftTime, 
		double in_rawTime,
		double in_t0,
		double in_error,
		double in_x, 
		double in_y, 
		double in_z,
		double in_xd,
		double in_yd,
		double in_zd):
	driftRadius(in_driftRadius),
	driftTime(in_driftTime),
	rawTime(in_rawTime),
	t0(in_t0),
	error(in_error),
	globalPosition(in_x,in_y,in_z),
	detectorCenter(in_xd,in_yd,in_zd)
	{
	}

	double timeResidual(double beta, double tTrack = 0.)
	{
            double mToF = rawTime - t0 + (globalPosition.Mag() / CLHEP::c_light) - driftTime;
            return mToF - globalPosition.Mag() / CLHEP::c_light / beta - tTrack;
	}
};



class StauTRT
{
public:
	StauTRT(StauTool* pStau, MsgStream& log , const Trk::Track* track, double beta = 1.0);
	

       /** default destructor */
	virtual ~StauTRT();

    void initStepData(TrtStepData* pTrtStepData, double beta, double tTrack = 0);
    void clearStepData(TrtStepData* pTrtStepData);
    void printStepData(TrtStepData* pTrtStepData);
    void clear();
    void processTrtWithBeta(double currentBeta, TrtStepData* pTrtStepData);
    void processTrtWithTTrack(double tTrack, TrtStepData* pTrtStepData,double beta = 1.0);
    bool hasHits();
    void setAverageTTrack(double tTrack );
    double averageTTrack();
    void setBeta(double beta);
    double beta(); 
    void setDofAtMinChi2(double);
    double dofAtMinChi2();
    void setMinChi2(double chi2);
    double minChi2();	 
    inline StauHits getHits() { return m_hits; }
protected:
	
        void preProcess(bool cosmics = false);
	StauTool*                  m_stauTool;
        MsgStream&                 m_log;
      
	const Trk::Track* m_track;
	ToolHandle<ITRT_DriftFunctionTool> m_drifttool;
	
	ServiceHandle<ITRT_CalDbSvc> m_trtconddbsvc;
	ServiceHandle<StoreGateSvc> m_sg;
	std::string m_tracksName;
	const TrackCollection* m_tracksCollection;
	


	//ToolHandle<ITrackInCaloTools>   m_pToCalo;
	//std::vector<const InDet::TRT_DriftCircle*>  m_driftCircles;
	//std::vector<const InDet::TRT_DriftCircleOnTrack*> m_driftCirclesOnTrack;
	//ToolHandle<InDet::IInDetCosmicsEventPhaseTool> m_epTool;

	std::vector<TRTHit> m_trthits;
	double m_residual;
	double m_error;
	double m_ePhase;
	double m_nHits;
	double m_beta;
	double m_minChi2;
	double m_dofAtMinChi2;
        StauHits m_hits;
};
}
#endif /*MUGRILNSSTAUTRT_H_*/

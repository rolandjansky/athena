/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDAlignMonNtuple_H
#define IDAlignMonNtuple_H

// **********************************************************************
// IDAlignMonNtuple.h
// AUTHORS: Ben Cooper
// **********************************************************************

#include <vector>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/NTuple.h"	
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "TrkParameters/TrackParameters.h"

class AtlasDetectorID;
class PixelID;
class SCT_ID;

namespace InDetDD{
  class SCT_DetectorManager;
}

namespace Trk {
  class IUpdator;
  class IPropagator;
  class RIO_OnTrack;
  class Track;
  class TrackStateOnSurface;
  class ITruthToTrack;
  class IResidualPullCalculator; 
}

class IDAlignMonNtuple : public ManagedMonitorToolBase
{

public:

	IDAlignMonNtuple( const std::string & type, const std::string & name, const IInterface* parent ); 

	virtual ~IDAlignMonNtuple();

	virtual StatusCode initialize();
	virtual StatusCode bookHistograms();
	virtual StatusCode fillHistograms();
	virtual StatusCode procHistograms();


protected:


 private:

	
	StatusCode setupTools();
	void setTrackErrorValues(int);
	
	const Trk::TrackParameters* getUnbiasedTrackParameters(const Trk::Track*, const Trk::TrackStateOnSurface*);
	StatusCode getSiResiduals(const Trk::Track*, const Trk::TrackStateOnSurface*, bool, double*);

	std::string m_stream;
	std::string m_tracksName;
	std::string m_tracksTruthName;
	int m_checkrate;
	bool m_unbiasedSCT;
	bool m_usePRD;	//if desired we can use PrepRawData hits information i.e. before insitu calibration of hits
	bool m_doPulls;
	std::string m_VxPrimContainerName;

	//tools
	const AtlasDetectorID*                m_idHelper;
	const InDetDD::SCT_DetectorManager*   m_SCT_Mgr;
	const PixelID*                        m_pixelID;
	const SCT_ID*                         m_sctID; 
	ToolHandle<Trk::IUpdator>             m_iUpdator;
	ToolHandle<Trk::IPropagator>          m_propagator;
	ToolHandle<Trk::ITruthToTrack>        m_truthToTrack;
	ToolHandle<Trk::IResidualPullCalculator>    m_residualPullCalculator;   //!< The residual and pull calculator tool handle
	INTupleSvc*                           m_ntupleSvc;


	//Ntuple related
	NTuple::Tuple* ntuple;  
	NTuple::Item<long> m_nt_ntrks;
	NTuple::Item<long> m_nt_nhits;
	NTuple::Item<long> m_nt_nvtx;
	NTuple::Item<long> m_nt_vtxntrks;
	NTuple::Item<long> m_nt_goodvtx;
	NTuple::Item<float> m_nt_vtxX;
	NTuple::Item<float> m_nt_vtxY;
	NTuple::Item<float> m_nt_vtxZ;

	NTuple::Array<long> m_nt_trknhits;
	NTuple::Array<float> m_nt_trkqoverpt;
	NTuple::Array<float> m_nt_trketa;
	NTuple::Array<float> m_nt_trktheta;
	NTuple::Array<float> m_nt_trkphi;
	NTuple::Array<float> m_nt_trkd0;
	NTuple::Array<float> m_nt_trkz0;
	NTuple::Array<float> m_nt_trkcharge;
	NTuple::Array<float> m_nt_trkchi2;
	NTuple::Array<long> m_nt_trkdof;
	NTuple::Array<float> m_nt_trkvtxd0;

	NTuple::Array<long> m_nt_trkistruth;
	NTuple::Array<long> m_nt_trktruthpdg;
	NTuple::Array<float> m_nt_trktruthprob;
	NTuple::Array<float> m_nt_trktruthpt;
	NTuple::Array<float> m_nt_trktruthphi;
	NTuple::Array<float> m_nt_trktrutheta;
	NTuple::Array<float> m_nt_trktruthphi0;
	NTuple::Array<float> m_nt_trktruthd0;
	NTuple::Array<float> m_nt_trktruthz0;
	NTuple::Array<float> m_nt_trktrutheta0;
	NTuple::Array<float> m_nt_trktruthqoverpt;
	NTuple::Array<float> m_nt_trktruthpt0;
	NTuple::Array<float> m_nt_trktruthcharge;
	NTuple::Array<float> m_nt_trktruthvtxX;
	NTuple::Array<float> m_nt_trktruthvtxY;
	NTuple::Array<float> m_nt_trktruthvtxZ;

	NTuple::Matrix<long> m_nt_dettype;
	NTuple::Matrix<long> m_nt_isbarrel;
	NTuple::Matrix<long> m_nt_layer;
	NTuple::Matrix<long> m_nt_hitmodphi;
	NTuple::Matrix<long> m_nt_hitmodeta;
	NTuple::Matrix<long> m_nt_hittype;
	NTuple::Matrix<float> m_nt_residualx;
	NTuple::Matrix<float> m_nt_residualy;
	NTuple::Matrix<float> m_nt_hitx;
	NTuple::Matrix<float> m_nt_hity;
	NTuple::Matrix<float> m_nt_biasedresidualx;
	NTuple::Matrix<float> m_nt_biasedresidualy;
	NTuple::Matrix<float> m_nt_errorx;
	NTuple::Matrix<float> m_nt_errory;
	NTuple::Matrix<long> m_nt_hitxwidth;
	NTuple::Matrix<long> m_nt_hitywidth;
	NTuple::Matrix<long> m_nt_hitolegwidth;
	NTuple::Matrix<float> m_nt_hitincidangle;

};

#endif

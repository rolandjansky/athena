/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************************
// $Id: InDetAlignMonBeamSpot.h,v 1.5 2009-02-05 20:55:07 beringer Exp $
//
// InDetAlignMonBeamSpot is a module to monitor the primary vertex and beam spot in
// the context of package InnerDetector/InDetMonitoring/InDetAlignmentMonitoring.
//
// Written in March 2008 by Juerg Beringer (LBNL)
//
// ********************************************************************************

#ifndef InDetAlignMonBeamSpot_H
#define InDetAlignMonBeamSpot_H

//#include <vector>
#include <string>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

class TH1F;
class TH2F;

class IBeamCondSvc;

namespace Trk {
  class IExtrapolator;
}


class InDetAlignMonBeamSpot : public ManagedMonitorToolBase {

public:
	InDetAlignMonBeamSpot( const std::string & type, const std::string & name, const IInterface* parent ); 

	virtual ~InDetAlignMonBeamSpot();

	virtual StatusCode initialize();
	virtual StatusCode bookHistograms();
	virtual StatusCode fillHistograms();
	//virtual StatusCode procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );
	virtual StatusCode procHistograms();
protected:
        ToolHandle<Trk::IExtrapolator> m_extrapolator;
        bool m_hasExtrapolator;

        ServiceHandle<IBeamCondSvc> m_beamCondSvc;
        bool m_hasBeamCondSvc;

        TH1F* m_hTrNPt;
        TH1F* m_hTrPt;
        TH2F* m_hTrDPhi;
        TH2F* m_hTrDPhiCorr;

	TH1F* m_hBsX;
	TH1F* m_hBsY;
	TH1F* m_hBsZ;
	TH1F* m_hBsTiltX;
	TH1F* m_hBsTiltY;

        TH1F* m_hPvN;
        TH1F* m_hPvNPriVtx;
        TH1F* m_hPvNPileupVtx;

        TH1F* m_hPvX;
        TH1F* m_hPvY;
        TH1F* m_hPvZ;
        TH1F* m_hPvErrX;
        TH1F* m_hPvErrY;
        TH1F* m_hPvErrZ;
        TH1F* m_hPvChiSqDoF;

        TH2F* m_hPvXZ;
        TH2F* m_hPvYZ;
        TH2F* m_hPvYX;

        TH1F* m_hPvNTracksAll;
        TH1F* m_hPvNTracks;
        TH1F* m_hPvTrackPt;
        TH1F* m_hPvTrackEta;

private:
        TH1F* makeAndRegisterTH1F(MonGroup& mon, const char* name, const std::string& title, int nBins, float minX, float maxX);
        TH2F* makeAndRegisterTH2F(MonGroup& mon, const char* hName, const std::string& hTitle,
				  int nBinsX, float minX, float maxX,
				  int nBinsY, float minY, float maxY);

	std::string m_stream;
        bool m_useBeamspot;
	std::string m_vxContainerName;
        bool m_vxContainerWithBeamConstraint;
        std::string m_trackContainerName;
        std::string m_histFolder;
	std::string m_triggerChainName;
        unsigned int m_minTracksPerVtx;
        float m_minTrackPt;
};

#endif

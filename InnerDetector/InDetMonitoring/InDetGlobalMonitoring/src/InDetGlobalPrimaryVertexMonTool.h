/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGlobalPrimaryVertexMonTool_H
#define InDetGlobalPrimaryVertexMonTool_H

#include <string>
#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class TH1F_LW;
class TH2F_LW;
class TProfile_LW;

namespace Trk {
  class VxCandidate;
}

namespace xAOD {
  class Vertex_v1;
}

class InDetGlobalPrimaryVertexMonTool : public ManagedMonitorToolBase {

public:
	InDetGlobalPrimaryVertexMonTool( const std::string & type, const std::string & name, const IInterface* parent );

	virtual ~InDetGlobalPrimaryVertexMonTool();

        virtual StatusCode initialize();
        virtual StatusCode bookHistogramsRecurrent();
	virtual StatusCode fillHistograms();

protected:
        // basic monitoring
        TH1F_LW* m_hPvN;
        TH1F_LW* m_hPvNPriVtx;
        TH1F_LW* m_hPvNPileupVtx;

        TProfile_LW* m_hPvN_LB;
    
        TH2F_LW* m_hPvNaveMu;
    
        TH1F_LW* m_hPvX;
        TH1F_LW* m_hPvY;
        TH1F_LW* m_hPvZ;
        TH1F_LW* m_hPvErrX;
        TH1F_LW* m_hPvErrY;
        TH1F_LW* m_hPvErrZ;
        TH1F_LW* m_hPvChiSqDoF;

        TH1F_LW* m_hPvNTracks;
        TH1F_LW* m_hPvTrackPt;
        TH1F_LW* m_hPvTrackEta;

        // enhnaced monitoring
        TH2F_LW* m_hVrt_XpullVsNtrkAverage_split;
        TH2F_LW* m_hVrt_YpullVsNtrkAverage_split;
        TH2F_LW* m_hVrt_ZpullVsNtrkAverage_split;

        TH2F_LW* m_hVrt_XpullVsPt2Average_split;
        TH2F_LW* m_hVrt_YpullVsPt2Average_split;
        TH2F_LW* m_hVrt_ZpullVsPt2Average_split;

        TH2F_LW* m_hVrt_Xerr_vs_ntrk;
        TH2F_LW* m_hVrt_Yerr_vs_ntrk;
        TH2F_LW* m_hVrt_Zerr_vs_ntrk;

        TH2F_LW* m_hVrt_Xerr_vs_pt2;
        TH2F_LW* m_hVrt_Yerr_vs_pt2;
        TH2F_LW* m_hVrt_Zerr_vs_pt2;

	// reconstruction efficiency plots
	TH1F_LW* m_hVrt_split_tag_ntrk; ///< Number of tag split vertices as fx of the number of tracks at probe (denominator)
	TH1F_LW* m_hVrt_split_probe_ntrk; ///< Number of probe and tag split vertices as fx of the number of tracks at probe (numerator)
	TH1F_LW* m_hVrt_split_matched_tag_ntrk; ///< Number of tag split vertices matched with original vertex as fx of the number of tracks at probe (denominator)
	TH1F_LW* m_hVrt_split_matched_probe_ntrk; ///< Number of probe and tag split vertices matched with original vertex as fx of the number of tracks at probe (numerator)
	TH1F_LW* m_hVrt_split_dist_tag; ///< Distance between tag-split and original non-BC vertex (used for selection efficiency)
	TH1F_LW* m_hVrt_split_dist_probe; ///< Distance between probe-split and original non-BC vertex (used for selection efficiency)

private:
        TH1F_LW* makeAndRegisterTH1F(MonGroup& mon, const char* name, std::string title, int nBins, float minX, float maxX);
        TH2F_LW* makeAndRegisterTH2F(MonGroup& mon, const char* hName, std::string hTitle,
                                     int nBinsX, float minX, float maxX,
                                     int nBinsY, float minY, float maxY);
        TH2F_LW* makeAndRegisterTH2FVariableXBins(MonGroup& mon, const char* hName, std::string hTitle,
                                     int nBinsX, double* xRange,
                                     int nBinsY, float minY, float maxY);
        // athena monitoring needs dummy registration of histos nad graphs which are created during post processing by a macro
        void makeAndRegisterDummyTGraph(MonGroup& mon, const char* hName);

	// Properties
        std::string m_histFolder;
        std::string m_vxContainerName;
        std::string m_vxContainerNameWithoutBeamConstraint;
        std::string m_vxContainerNameSplit;
	int         m_splitVertexTrkInvFraction; ///< store inverse of the fraction of input tracks used for probe vertex (1:N)
	float       m_distanceSplitVxMatch; ///< store maximum distance for matching split vertices to original non-BC vertex
	/** store metric to be used for split vertex matching in selection efficiency
	 * Values currently implemented:
	 * 0: dummy metric; returns zero distance
	 * 1: simple delta-z metric
	 * 2: 3-D distance divided by the error (dominated by Delta z)
	 * 3: quadratic sum of distances divided by their errors in the 3 directions. Expected RMS = 1. Default.
	 */
	int         m_splitMatchingMetric; 
        bool m_doEnhancedMonitoring; // trigger the enhanced monitoring (to be activated by InDetFlags.doMonitoringPrimaryVertexingEnhanced because split vertexing and vertexing with no beam constraint need to be run as well)
	// Helper functions
	/** Returns matching distance between split and original vertex.
	 * @param splitVx pointer to split vertex 
	 * @param originalVx pointer to original non-BC vertex
	 * @return distance for matching split to original Vertex
	 */
    double GetSplitMatchDistance(const xAOD::Vertex_v1* splitVx, const xAOD::Vertex_v1* originalVx);
	
};

#endif

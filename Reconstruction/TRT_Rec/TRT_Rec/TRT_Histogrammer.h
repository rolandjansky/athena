/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header:
//  Author: Jim Loken
//
#ifndef TRTREC_TRTHISTOGRAMMER_H
#define TRTREC_TRTHISTOGRAMMER_H

#include <list>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatInterfaces/ITRT_Histogrammer.h"
#include "TRT_Rec/TRT_Fit.h"
#include "TRT_Rec/TRT_RoadData.h"

class MsgStream;
namespace Trk
{
    class IIntersector;
    class IRIO_OnTrackCreator;
}

class TRT_Histogrammer: public AthAlgTool,
			virtual public ITRT_Histogrammer
{
public:
    TRT_Histogrammer	       	(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    ~TRT_Histogrammer		(void);
   
    StatusCode			initialize();
    StatusCode			finalize();

    // current track has significant curvature increase
    bool			bremLike (double rMax, double rPhiMax) const;

    // select and confirm the RoadData objects forming a TRT track segment 
    bool 			confirmSegment (const Trk::TrackParameters&		trkParameters,
						const std::vector<TRT_RoadData*>&	roadData);

    // number of trt layers (high threshold hits) associated to current segment 
    int				numLayers (void) const;
    int				numTrHits (void) const;

    // residual for a particular RoadData object
    double			residual (const TRT_RoadData& hit) const;

    // current segment has a significant proportion of high threshold hits
    bool			trRich (void) const;
  
private:
    void			fitPeak (void);
    void			flagAssociations (void);
    void			makeHistograms (void);
    void			outlierRemoval (std::list<TRT_RoadData*>& driftHits);

    // configurable
    double					m_muonTrThreshold;

    // tools
    ToolHandle<Trk::IRIO_OnTrackCreator>	m_driftCircleOnTrackMaker;
    ToolHandle<Trk::IIntersector>              	m_intersector;
    ToolHandle<Trk::IRIO_OnTrackCreator>	m_strawOnTrackMaker;
    
    // parameters controlling the histogram algorithm:
    const double				m_dCorr;
    const int					m_maxBin;
    const unsigned				m_maxHit;
    std::vector<double>				m_maxResidual;
    const unsigned				m_minPeak;
    const int					m_n1Corr;
    const int					m_n2Corr;
    const double				m_wbin0;
    const double				m_wbinPt;

    // for internal communication:
    TRT_Fit*					m_fit;
    unsigned					m_hMaxPt;
    std::vector<TRT_RoadData*>::const_iterator	m_hitPt[200];  // 2*m_maxHit
    int						m_iCorrMax;
    double					m_inversePt;
    int						m_numLayers;
    int						m_numTrHits;
    bool					m_pBelowMuonTrThreshold;
    const std::vector<TRT_RoadData*>*		m_roadData;
    double					m_scaleFactor;
};

#endif // TRTHISTOGRAMMER_H

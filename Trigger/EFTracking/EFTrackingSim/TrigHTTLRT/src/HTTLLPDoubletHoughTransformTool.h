// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTLLPDOUBLETHOUGHTRANSFORMTOOL_H
#define HTTLLPDOUBLETHOUGHTRANSFORMTOOL_H


#include "TFile.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"


#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTObjects/HTTVectors.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTHough/HTTRoadFinderToolI.h"


/*
Implments HoughT for d0, q/pt using hit doublets
Hough space x -> d0 , y -> q/pt
*/

class HTTLLPDoubletHoughTransformTool : public AthAlgTool, virtual public IHTTRoadFinderTool {

public:
    HTTLLPDoubletHoughTransformTool(const std::string&, const std::string&, const IInterface*);

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual StatusCode getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) override;


private:
    BooleanProperty m_traceHits = true; // Trace each hit that goes in a bin

    IntegerProperty m_threshold = 8; // munimum number of hits required for the road
    IntegerProperty m_threshold50 = 8; // munimum number of hits required for the road

    typedef vector2D<std::pair<int, std::unordered_set<const HTTHit*>>> Image;
    std::vector<HTTRoad_Hough> m_roads;

    const double m_acceptedDistanceBetweenLayersMin = 200; // min R disstance for hits pair filtering
    const double m_acceptedDistanceBetweenLayersMax = 600;
    FloatProperty m_d0_range = 120;
    FloatProperty m_qOverPt_range = 0.002;
    IntegerProperty m_imageSize_x = 216; // i.e. number of bins in d0
    IntegerProperty m_imageSize_y = 216; // i.e. number of bins in q/pT
    double m_step_x = 0; // helpers (accumulator granularity)
    double m_step_y = 0;
    BooleanProperty m_continuous = true; // assure that there is continuity of the line (i.e. middle bins in d0 are filled when one q/pT step would result in a hole)

    StatusCode fillImage(const HTTHit *hit1, const HTTHit *hit2, Image& image) const;
    bool passThreshold(Image const & image, int x, int y) const;
    bool isLocalMaxima(Image const & image, int x, int y) const;

    HTTRoad_Hough createRoad(std::unordered_set<const HTTHit*> const & hits, unsigned x, unsigned y) const;
    unsigned m_event = 0;
    void drawImage(Image const & image) const;

    float xtod0(int x) const { return x*m_step_x - m_d0_range; }
    float ytoqoverpt(int y) const { return y*m_step_y - m_qOverPt_range; }
    int m_eventsProcessed = 0;
    int m_roadsGenerated = 0;

};

#endif // 



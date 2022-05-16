// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "TH2.h"
#include "HTTLLPDoubletHoughTransformTool.h"

// simple 2D vector with basic arithmetics
typedef double fp_t; //  floating point type alias, can change it to float from double

typedef std::array<fp_t, 2> pvec;
pvec operator+(const pvec& a, const pvec& b);
pvec operator-(const pvec& a, const pvec& b);
pvec operator*(const pvec& a, double scale);


pvec operator+(const pvec& a, const pvec& b) {
    return { {a[0]+b[0], a[1]+b[1]} };
}
pvec operator-(const pvec& a, const pvec& b) {
    return { {a[0]-b[0], a[1]-b[1]} };

}
pvec operator*(const pvec& a, double scale) {
    return { {a[0]*scale, a[1]*scale} };
}

double length(const pvec& v) {
    return std::hypot(v[0], v[1]);
}

pvec rotate90( const pvec& v) {
    return { -v[1], v[0]};
}

double crossProduct( const pvec& a, const pvec& b ) {
    return a[0]*b[1] - a[1]*b[0];
}


HTTLLPDoubletHoughTransformTool::HTTLLPDoubletHoughTransformTool(const std::string& algname, const std::string& name , const IInterface* ifc) 
  : base_class(algname, name, ifc)
{
    declareInterface<IHTTRoadFinderTool>(this);
}


StatusCode HTTLLPDoubletHoughTransformTool::initialize() { 
    if (m_imageSize_y %2 == 1) {
        ATH_MSG_ERROR("Can not have odd number  " <<  m_imageSize_y << " of bins in q/pT - will result in division by 0");
        return StatusCode::FAILURE;
    }
    m_step_x = (2*m_d0_range) / m_imageSize_x;
    m_step_y = (2*m_qOverPt_range) / m_imageSize_y;

    return StatusCode::SUCCESS;
 }

StatusCode HTTLLPDoubletHoughTransformTool::finalize() { 
    ATH_MSG_INFO("Number of events procesed by the HTTLLPDoubletHoughTransformTool " << m_eventsProcessed <<  " roads " << m_roadsGenerated);
    return StatusCode::SUCCESS;
}

StatusCode HTTLLPDoubletHoughTransformTool::getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) {
    roads.clear();
    m_roads.clear();
    m_eventsProcessed++;
    Image image(m_imageSize_x, m_imageSize_y); // not quite optimal to allocate this memory in evey event, but following nominal HT
    for (unsigned ihit1 = 0; ihit1 < hits.size(); ihit1++) {
      const HTTHit *hit1 = hits[ihit1];
      for (unsigned ihit2 = ihit1+1; ihit2 < hits.size(); ihit2++) {
	const HTTHit *hit2 = hits[ihit2];
            ATH_MSG_DEBUG("Hits pair R: " << hit1->getR() << " " << hit2->getR());
            //TODO: replace with qualification by layer IDs
            const double radiusDifference =  hit2->getR() - hit1->getR();

            if (  not (m_acceptedDistanceBetweenLayersMin < radiusDifference && radiusDifference < m_acceptedDistanceBetweenLayersMax) ) 
                continue;
            if ( hit1->getLayer() == hit2->getLayer() )
                continue;
            ATH_CHECK(fillImage( hit1, hit2, image ));
        }
    } 
    if ( m_event < 20 )
        drawImage(image);

    for (unsigned y = 0; y < m_imageSize_y; y++) {
        for (unsigned x = 0; x < m_imageSize_x; x++) {
            if (passThreshold(image, x, y) && isLocalMaxima( image, x, y ) ) {
                ATH_MSG_DEBUG("Creating road with threshold " << image(x,y).first << " hits " << image(x,y).second.size() 
                                << " at x: " << x  << " d0 " << xtod0(x) <<  " y: "<< y << " q/pt " << ytoqoverpt(y));
                m_roadsGenerated++;
                m_roads.push_back(createRoad(image(x, y).second, x, y));
            }
        }
    }
    roads.reserve(m_roads.size());
    for (HTTRoad_Hough & r : m_roads) roads.push_back(&r);
    m_event++;
    return StatusCode::SUCCESS;   
}


StatusCode HTTLLPDoubletHoughTransformTool::fillImage(const HTTHit *hit1, const HTTHit *hit2, Image& image) const { 
    const pvec p1 {{hit1->getX(), hit1->getY()}};
    const pvec p2 {{hit2->getX(), hit2->getY()}};
    const pvec halfDiff = (p2 - p1)*0.5;
    const fp_t halfLen = length(halfDiff);

    int xbefore = -1;

    for ( unsigned y = 0; y < m_imageSize_y; y++ ) {
        const fp_t qoverpt = (y * m_step_y) + m_step_y*0.5 - m_qOverPt_range; // use middle f the bin
        const fp_t radius = 1.0/(0.6*qoverpt); // magic transform constants
        const fp_t scale = std::copysign( std::sqrt( std::pow(radius/halfLen, 2) - 1), radius );
        const pvec rprime = rotate90(halfDiff) * scale;
        const pvec center = p1 + halfDiff + rprime;
        const fp_t d0 =  (std::signbit(radius) ? -1.0 : 1.0)*(length(center) - abs(radius));

        // find the bin x position in the image
        int x = (d0 + m_d0_range) / m_step_x;

        if ( 0 <= x && x < (int)m_imageSize_x) {
            if (xbefore == -1) xbefore = x;
            if ( m_continuous ) { // fill the bins along x starintg from the last one filled 
                const int xmin =  (xbefore < x)? xbefore: x;
                const int xmax =  (xbefore < x)? x: xbefore;
                for ( int xinterpolated = xmin; xinterpolated <= xmax; ++xinterpolated) {
                    image(xinterpolated, y).first++;
                    image(xinterpolated, y).second.insert( hit1 );
                    image(xinterpolated, y).second.insert( hit2 );
                }
            } else {
                image(x, y).first++;
                image(x, y).second.insert( hit1 );
                image(x, y).second.insert( hit2 );
            }
            xbefore = x;
        }    
    }
    return StatusCode::SUCCESS;
}

HTTRoad_Hough HTTLLPDoubletHoughTransformTool::createRoad(std::unordered_set<const HTTHit*> const & hits, unsigned /*x*/, unsigned /*y*/) const {
    // Get the road hits
    std::vector<const HTTHit*> road_hits;
    layer_bitmask_t hitLayers = 0;
    for (const HTTHit * hit : hits)
    {
        road_hits.push_back(hit);
        hitLayers |= 1 << hit->getLayer();
    }

    auto sorted_hits = ::sortByLayer(road_hits);
    sorted_hits.resize(8); // If no hits in last layer, return from sortByLayer will be too short

    HTTRoad_Hough r;
    r.setHitLayers(hitLayers);
    r.setHits(sorted_hits);
    return r;
}


bool HTTLLPDoubletHoughTransformTool::passThreshold(Image const & image, int x, int y) const {
    const int count = image(x,y).first;
    const float d0 = xtod0(x);
    if ( std::abs(d0) < 50.0 && count >= m_threshold50 ) return true;
    if ( std::abs(d0) >= 50.0 && count >= m_threshold ) return true;
    
    return false;
}

bool HTTLLPDoubletHoughTransformTool::isLocalMaxima(Image const & image, int x, int y) const { 
    const auto centerValue =  image(x,y).first;
    for ( int xaround = std::min(x-1, 0); xaround <= std::max((int)m_imageSize_x-1, x+1); xaround++  ) {
      for ( int yaround = std::min(y-1, 0); yaround <= std::max((int)m_imageSize_y-1, y+1); yaround++  ) {
            if ( image(xaround,yaround).first > centerValue )
                return false;
        }
    }
    return true; 
}

void HTTLLPDoubletHoughTransformTool::drawImage(Image const & image) const { 

    TH2I h(("event"+std::to_string( m_event )).c_str(), "LLP Doublet Hough space;d_{0}[mm];q/pT [e/GeV]",
            m_imageSize_x, -m_d0_range, m_d0_range,
            m_imageSize_y, -m_qOverPt_range, m_qOverPt_range );

    for (unsigned x = 0; x < m_imageSize_x; x++)
        for (unsigned y = 0; y < m_imageSize_y; y++)
            h.SetBinContent(x+1, y+1, image(x, y).first); // +1 since root bins are 1-indexed

    h.SaveAs((name()+"_event_"+std::to_string( m_event )+".root").c_str());
}


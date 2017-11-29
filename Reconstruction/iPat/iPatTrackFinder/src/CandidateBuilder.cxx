/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CandidateBuilder.cxx, (c) ATLAS iPatRec
//
// build a Track out of the SpacePoint's forming a TrackCandidate
//
///////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/SystemOfUnits.h"
#include "InDetPrepRawData/SiCluster.h"
#include "VxVertex/RecVertex.h"
#include "iPatInterfaces/ISiClusterProperties.h"
#include "iPatInterfaces/ISiDetectorLocator.h"
#include "iPatInterfaces/ITrackQuality.h"
#include "iPatInterfaces/IiPatFitter.h"
#include "iPatTrack/TrackEnvelope.h"
#include "iPatTrackFinder/CandidateBuilder.h"
#include "iPatTrackFinder/Point.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

CandidateBuilder::CandidateBuilder (const std::string&	type,
				    const std::string&	name,
				    const IInterface*	parent)
    :   AthAlgTool		(type, name, parent),
	m_clusterProperties	("SiClusterProperties/SiClusterProperties"),
	m_detectorLocator	("SiDetectorLocator/SiDetectorLocator"),
	m_trackFitter		("TrackFitter/TrackFitter"),
	m_trackQuality		("TrackQuality/TrackQuality"),
 	m_envelopePP		(nullptr),
	m_envelopePPP		(nullptr),
	m_envelopePPS		(nullptr),
	m_envelopePS		(nullptr),
	m_envelopePSI		(nullptr),
	m_envelopePSS		(nullptr),
	m_envelopeSS		(nullptr),
	m_envelopeSSS		(nullptr),
	m_hits			(new hit_list),
	m_hitsPP		(new hit_list),
	m_hitsPPP		(new hit_list),
	m_hitsPPS		(new hit_list),
	m_hitsPS		(new hit_list),
	m_hitsPSI		(new hit_list),
	m_hitsPSS		(new hit_list),
	m_hitsSS		(new hit_list),
	m_hitsSSS		(new hit_list)
{
    declareInterface<ICandidateBuilder>(this);
    declareProperty("SiClusterProperties",	m_clusterProperties);
    declareProperty("SiDetectorLocator",	m_detectorLocator);
    declareProperty("TrackFitter",		m_trackFitter);
    declareProperty("TrackQuality",		m_trackQuality);
}

CandidateBuilder::~CandidateBuilder (void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
CandidateBuilder::initialize()
{
    ATH_MSG_INFO( "initialize with 8 track envelopes" );

    // get the Tools
    if (m_clusterProperties.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_clusterProperties );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_clusterProperties );
    }
    if (m_detectorLocator.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_detectorLocator );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_detectorLocator );
    }
    if (m_trackFitter.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackFitter );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackFitter );
    }
    if (m_trackQuality.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackQuality );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackQuality );
    }
     
    // create a maximal hit_list (6 hits for 3 sct layers)
    // - these hits are deleted by the CandidateBuilder finalize
    //
    // plus TrackEnvelope's and hit_list's for each 2 and 3 pt pixel/sct combination
    // - these contain pointers to the above hits
    // - each list is owned by the appropriate envelope 
    // - hit ownership stays with the CandidateBuilder

    HitOnTrack* hot 	= new HitOnTrack;   // inner pix/sct
    m_hits->push_back(hot);
    m_hitsPPP->push_back(hot);
    m_hitsPPS->push_back(hot);
    m_hitsPSI->push_back(hot);
    m_hitsPSS->push_back(hot);
    m_hitsSSS->push_back(hot);
    hot 		= new HitOnTrack;   // inner sct (2nd wafer)
    m_hits->push_back(hot);
    m_hitsSSS->push_back(hot);
    hot 		= new HitOnTrack;   // middle pix/sct
    m_hits->push_back(hot);
    m_hitsPP->push_back(hot);
    m_hitsPPP->push_back(hot);
    m_hitsPPS->push_back(hot);
    m_hitsPS->push_back(hot);
    m_hitsPSS->push_back(hot);
    m_hitsSS->push_back(hot);
    m_hitsSSS->push_back(hot);
    hot 		= new HitOnTrack;   // middle sct (2nd wafer)
    m_hits->push_back(hot);
    m_hitsPSS->push_back(hot);
    m_hitsSS->push_back(hot);
    m_hitsSSS->push_back(hot);
    hot 		= new HitOnTrack;   // outer pix/sct
    m_hits->push_back(hot);
    m_hitsPP->push_back(hot);
    m_hitsPPP->push_back(hot);
    m_hitsPPS->push_back(hot);
    m_hitsPS->push_back(hot);
    m_hitsPSI->push_back(hot);
    m_hitsPSS->push_back(hot);
    m_hitsSS->push_back(hot);
    m_hitsSSS->push_back(hot);
    hot 		= new HitOnTrack;   // outer sct (2nd wafer)
    m_hits->push_back(hot);
    m_hitsPPS->push_back(hot);
    m_hitsPS->push_back(hot);
    m_hitsPSI->push_back(hot);
    m_hitsPSS->push_back(hot);
    m_hitsSS->push_back(hot);
    m_hitsSSS->push_back(hot);

    // now can create the envelopes
    m_envelopePP      	= new TrackEnvelope(two_point,m_hitsPP);
    m_envelopePPP     	= new TrackEnvelope(three_point,m_hitsPPP);
    m_envelopePPS     	= new TrackEnvelope(three_point,m_hitsPPS);
    m_envelopePS      	= new TrackEnvelope(two_point,m_hitsPS);
    m_envelopePSI     	= new TrackEnvelope(two_point,m_hitsPSI);
    m_envelopePSS     	= new TrackEnvelope(three_point,m_hitsPSS);
    m_envelopeSS	= new TrackEnvelope(two_point,m_hitsSS);
    m_envelopeSSS     	= new TrackEnvelope(three_point,m_hitsSSS);
    
    return StatusCode::SUCCESS;
}

StatusCode
CandidateBuilder::finalize()
{
    ATH_MSG_DEBUG( "CandidateBuilder::finalize()" );
    
    m_hitsPP->clear();
    m_hitsPPP->clear();
    m_hitsPPS->clear();
    m_hitsPS->clear();
    m_hitsPSI->clear();
    m_hitsPSS->clear();
    m_hitsSS->clear();
    m_hitsSSS->clear();
    delete m_envelopePP;
    delete m_envelopePPP;
    delete m_envelopePPS;
    delete m_envelopePS;
    delete m_envelopePSI;
    delete m_envelopePSS;
    delete m_envelopeSS;
    delete m_envelopeSSS;
    
    for (hit_iterator h = m_hits->begin();
	 h != m_hits->end();
	 ++h)
	delete *h;

    m_hits->clear();
    delete m_hits;
    delete m_hitsPP;
    delete m_hitsPPP;
    delete m_hitsPPS;
    delete m_hitsPS;
    delete m_hitsPSI;
    delete m_hitsPSS;
    delete m_hitsSS;
    delete m_hitsSSS;
    
    return StatusCode::SUCCESS; 
}

const Track&
CandidateBuilder::twoPointTrack (PerigeeParameters*	perigee,
				 const Point&		point1,
				 const Point&		point2,
				 bool			pixelInterchange,
				 const Trk::RecVertex&	vertex)
{
//     std::cout << " twoPointTrack " << std::endl;
    TrackEnvelope*	envelope;
    if (pixelInterchange)
    {
	envelope = m_envelopePSI;
	m_hitIterator = envelope->hits().begin();
	hitsFromPixel(point1);
	hitsFromPoint(point2);
    }
    else if (point2.isPixel())
    {
	envelope = m_envelopePP;
	m_hitIterator = envelope->hits().begin();
	hitsFromPixel(point1);
	hitsFromPixel(point2);
    }
    else if (point1.isPixel())
    {
	envelope = m_envelopePS;
	m_hitIterator = envelope->hits().begin();
	hitsFromPixel(point1);
	hitsFromPoint(point2);
    }
    else
    {
	envelope = m_envelopeSS;
	m_hitIterator = envelope->hits().begin();
	hitsFromPoint(point1);
	hitsFromPoint(point2);
    }
    envelope->status(two_point);
    envelope->perigeeParameters(perigee);
    m_trackFitter->fitWithVertex(two_point,
				 envelope->fitQuality(),
				 envelope->perigeeParameters(),
				 vertex,
				 envelope->track().hit_list_begin(),
				 envelope->track().hit_list_end());
    if (m_trackQuality->goodCandidate(*envelope->fitQuality(),
				      envelope->perigeeParameters()))
    {
	envelope->hitQuality().fill(envelope->track().hit_list_begin(),
				    envelope->track().hit_list_end());
    }
    else
    {
	envelope->status(undefined);
    }
    return envelope->track();
}

const Track&
CandidateBuilder::threePointTrack (const Point& point1,
				   const Point& point2,
				   const Point& point3)
{
//     std::cout << " threePointTrack " << std::endl;
    Amg::Vector3D	separation		= point3.globalPosition() - point1.globalPosition();
    PerigeeParameters*	perigeeParameters	= new PerigeeParameters(separation.x()/separation.perp(),
									separation.z()/separation.perp(),
									0.005,
									separation.y()/separation.perp(),
									0.,
									point1.globalPosition(),
									point1.globalPosition().z());
    TrackEnvelope* envelope = m_envelopeSSS;
    m_hitIterator = envelope->hits().begin();
    if (point1.isPixel())
    {
	hitsFromPixel(point1);
	(*m_hitIterator++)->status(strip_reject);
    }
    else
    {
	hitsFromPoint(point1);
    }
    if (point2.isPixel())
    {
	hitsFromPixel(point2);
	(*m_hitIterator++)->status(strip_reject);
    }
    else
    {
	hitsFromPoint(point2);
    }
    if (point3.isPixel())
    {
	hitsFromPixel(point3);
	(*m_hitIterator++)->status(strip_reject);
    }
    else
    {
	hitsFromPoint(point3);
    }

    envelope->status(three_point);
    envelope->perigeeParameters(perigeeParameters);
    m_trackFitter->fit(segment,
		       envelope->fitQuality(),
		       envelope->perigeeParameters(),
		       envelope->track().hit_list_begin(),
		       envelope->track().hit_list_end());
    envelope->hitQuality().fill(envelope->track().hit_list_begin(),
				envelope->track().hit_list_end());

    return envelope->track();
}

const Track&
CandidateBuilder::threePointTrack (PerigeeParameters*	perigee,
				   const Point&		point1,
				   const Point&		point2,
				   const Point&		point3)
{
    // cout << " threePointTrack " << endl;

    TrackEnvelope*	envelope;
    if (point2.isPixel())
    {
	if (point3.isPixel())
	{
	    envelope = m_envelopePPP;
	    m_hitIterator = envelope->hits().begin();
	    hitsFromPixel(point1);
	    hitsFromPixel(point2);
	    hitsFromPixel(point3);
	}
	else
	{
	    envelope = m_envelopePPS;
	    m_hitIterator = envelope->hits().begin();
	    hitsFromPixel(point1);
	    hitsFromPixel(point2);
	    hitsFromPoint(point3);
	}
    }
    else if (point1.isPixel())
    {
	envelope = m_envelopePSS;
	m_hitIterator = envelope->hits().begin();
	hitsFromPixel(point1);
	hitsFromPoint(point2);
	hitsFromPoint(point3);
    }
    else
    {
	envelope = m_envelopeSSS;
	m_hitIterator = envelope->hits().begin();
	hitsFromPoint(point1);
	hitsFromPoint(point2);
	hitsFromPoint(point3);
    }
    envelope->status(three_point);
    envelope->perigeeParameters(perigee);
    m_trackFitter->fit(three_point,
		       envelope->fitQuality(),
		       envelope->perigeeParameters(),
		       envelope->track().hit_list_begin(),
		       envelope->track().hit_list_end());
    if (m_trackQuality->goodCandidate(*envelope->fitQuality(),
				      envelope->perigeeParameters()))
    {
	envelope->hitQuality().fill(envelope->track().hit_list_begin(),
				    envelope->track().hit_list_end());
    }
    else
    {
	envelope->status(undefined);
    }
    return envelope->track();
}

const Track&
CandidateBuilder::threePointTrack (const PerigeeParameters&	perigee,
				   const Point&			point1,
				   const Point&			point2,
				   const Point&			point3,
				   bool				pixelInterchange)
{
    // cout << " threePointTrack " << endl;
    TrackEnvelope*	envelope;
    if (pixelInterchange)
    {
	 envelope = m_envelopePPS;
	 m_hitIterator = envelope->hits().begin();
	 ++m_hitIterator;
	 hitsFromPixel(point1);
    }
    else if (point3.isPixel())
    {
	envelope = m_envelopePPP;
	m_hitIterator = envelope->hits().begin();
	hitsFromPixel(point1);
    }
    else if (point2.isPixel())
    {
	envelope = m_envelopePPS;
	m_hitIterator = envelope->hits().begin();
	hitsFromPixel(point1);
    }
    else if (point1.isPixel())
    {
	envelope = m_envelopePSS;
	m_hitIterator = envelope->hits().begin();
	hitsFromPixel(point1);
    }
    else
    {
	envelope = m_envelopeSSS;
	m_hitIterator = envelope->hits().begin();
	hitsFromPoint(point1);
    }
    
    // should maybe insert some material effects !
    envelope->status(three_point);
    envelope->perigeeParameters().fillPerigee(perigee);
    m_trackFitter->fit(three_point,
		       envelope->fitQuality(),
		       envelope->perigeeParameters(),
		       envelope->track().hit_list_begin(),
		       envelope->track().hit_list_end());
    if (m_trackQuality->goodCandidate(*envelope->fitQuality(),
				      envelope->perigeeParameters()))
    {
	envelope->hitQuality().fill(envelope->track().hit_list_begin(),
				    envelope->track().hit_list_end());
    }
    else
    {
	envelope->status(undefined);
    }
    return envelope->track();
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
CandidateBuilder::hitsFromPixel (const Point& point)
{
    const InDet::SiCluster*	cluster		= point.cluster1();
    const SiliconDetector*	detector	=
	&m_detectorLocator->pixelDetector(point.detectorId1());
    std::pair<double,double> broadErrors = m_clusterProperties->broadErrors(cluster,
									    detector,
									    point.globalPosition());
    //SiClusterProperties		properties(cluster,detector,point.globalPosition());
    // waiting for DetectorElement migration
    const HepGeom::Point3D<double> position = HepGeom::Point3D<double>(cluster->globalPosition().x(),
								       cluster->globalPosition().y(),
								       cluster->globalPosition().z());
    double sinStereo	= cluster->detectorElement()->sinStereo(position);
    
    (*m_hitIterator++)->fill(cluster->globalPosition(),
			     broadErrors.first,
			     0.,
			     broadErrors.second,
			     0.,
			     // cluster->detectorElement()->sinStereo(cluster->globalPosition()),
			     sinStereo,
			     0.,
			     point.inBarrel(),
			     cluster->detectorElement(),
			     Identifier(),
			     broad_pixel);
}

void
CandidateBuilder::hitsFromPoint (const Point& point)
{
    const InDet::SiCluster*	cluster1  = point.cluster1();
    const SiliconDetector*	detector1 = &m_detectorLocator->SCT_Detector(point.detectorId1());
    std::pair<double,double>	broadErrors1 = m_clusterProperties->broadErrors(cluster1,
									    detector1,
									    point.globalPosition());
    // waiting for DetectorElement migration
    const HepGeom::Point3D<double> position1 = HepGeom::Point3D<double>(cluster1->globalPosition().x(),
									cluster1->globalPosition().y(),
									cluster1->globalPosition().z());
    double sinStereo1	= cluster1->detectorElement()->sinStereo(position1);
    
    (*m_hitIterator++)->fill(cluster1->globalPosition(),
			     broadErrors1.first,
			     0.,
			     0.,
			     0.,
			     // cluster1->detectorElement()->sinStereo(cluster1->globalPosition()),
			     sinStereo1,
			     0.,
			     point.inBarrel(),
			     cluster1->detectorElement(),
			     Identifier(),
			     broad_strip);
    const InDet::SiCluster* 	cluster2  = point.cluster2();
    const SiliconDetector*  	detector2 = &m_detectorLocator->SCT_Detector(point.detectorId2());
    std::pair<double,double>	broadErrors2 = m_clusterProperties->broadErrors(cluster2,
									    detector2,
									    point.globalPosition());
    // waiting for DetectorElement migration
    const HepGeom::Point3D<double> position2 = HepGeom::Point3D<double>(cluster2->globalPosition().x(),
									cluster2->globalPosition().y(),
									cluster2->globalPosition().z());
    double sinStereo2	= cluster2->detectorElement()->sinStereo(position2);
    
    (*m_hitIterator++)->fill(cluster2->globalPosition(),
			     broadErrors2.first,
			     0.,
			     0.,
			     0.,
			     // cluster2->detectorElement()->sinStereo(cluster2->globalPosition()),
			     sinStereo2,
			     0.,
			     point.inBarrel(),
			     cluster2->detectorElement(),
			     Identifier(),
			     broad_strip);
}

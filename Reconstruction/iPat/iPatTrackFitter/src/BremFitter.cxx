/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 simple brem recovery procedure - written in a hurry to meet DC1 deadline
 
 ATLAS Collaboration
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <iostream>
#include <iomanip>
#include "GaudiKernel/MsgStream.h"
#include "iPatTrack/Track.h"
#include "iPatTrackFitter/BremFitter.h"
#include "iPatTrackFitter/TrackFitter.h"
#include "iPatTrackParameters/PerigeeParameters.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

//------------------------------------------------------------------------

BremFitter::BremFitter (const std::string&	type, 
			const std::string&	name,
			const IInterface*	parent)
    :	AlgTool			(type, name, parent),
    	m_fitQuality		(0),
	m_hits			(new hit_list),
	m_perigeeParameters	(0)
{}

//--------------------------------------------------------------------------
BremFitter::~BremFitter(void)
{
    for (HitList::hit_iterator h = m_hits->begin();
	 h != m_hits->end();
	 ++h)
	delete *h;
    m_hits->clear();

    delete m_fitQuality;
    delete m_hits;
    delete m_perigeeParameters;
}

//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

StatusCode
BremFitter::initialize()
{
    // print name and package version
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "BremFitter::initialize()"
	<< " - package version " << PACKAGE_VERSION << endreq;

    // get the ToolSvc 
    IToolSvc* toolSvc;
    if (StatusCode::SUCCESS != service("ToolSvc",toolSvc))
    {
	log << MSG::ERROR << " Can't get ToolSvc " << endreq;
	return StatusCode::FAILURE;
    }

    // track fitter tool
    IAlgTool* algtool;
    if (StatusCode::SUCCESS != toolSvc->retrieveTool("TrackFitter",algtool))
    {
	log << MSG::ERROR << " Can't get TrackFitter " << endreq;
	return StatusCode::FAILURE;
    }
    m_fitter = dynamic_cast<TrackFitter*>(algtool);

    return StatusCode::SUCCESS;
}

StatusCode
BremFitter::finalize()
{
    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------    
void
BremFitter::bremRecovery(PerigeeParameters*&   	perigeeParameters,
			 HitList::hit_citerator	begin,
			 HitList::hit_citerator	end)
{
    // tidy up any existing hits
    for (HitList::hit_iterator h = m_hits->begin();
	 h != m_hits->end();
	 ++h)
	delete *h;
    m_hits->clear();
    
    // keep original momentum 
    double		oldInversePt = perigeeParameters->qOverPt();

    // first attempt = discard TRT hits
    int		       	layers	= 0;
    int		       	straws	= 0;
    for (HitList::hit_citerator h = begin; h != end; ++h)
    {
	if ((**h).isDrift())
	{
	    ++straws;
	    continue;
	}
	else if (straws == 0)
	{
	    m_hits->push_back(new HitOnTrack(**h));

	    // count layers (with a maximum of 3 pixel layers)
	    if ((**h).isPixel() && layers > 2)	layers = 2;
	    if ((**h).status() == scatterer)	++layers;
	}
    }

    // check for sufficient Silicon layers to allow cutting back 
    if (layers < 4) return;
    
    if (straws > 0)
    {
	delete m_fitQuality;
	if (m_perigeeParameters	== 0) m_perigeeParameters = new PerigeeParameters;
	m_perigeeParameters->fillPerigee(*perigeeParameters);
	m_fitter->fit(truncated,
		      m_fitQuality,
		      *m_perigeeParameters,
		      m_hits->begin(),
		      m_hits->end());

	// check fit OK
	if (m_fitQuality->fit_code()) return;
	
	// check for an acceptably precise fit (10%) with significant pt improvement:
	double 	inversePt			= m_perigeeParameters->qOverPt();
	const AmgSymMatrix(5)& covariance	= m_perigeeParameters->covariance();
	double			ptError		= sqrt(covariance(4,4))/std::abs(inversePt);
	if (ptError > 0.10)	return;

	// save result
     	delete perigeeParameters;
	perigeeParameters	= m_perigeeParameters;
	m_perigeeParameters	= 0;
// 	perigeeParameters->print();
// 	std::cout << " bremFitter - no TRT " << std::endl;
    }
    
    // now try with first 5, then 4, layers (at least to first SCT layer)
    HitList::hit_iterator last = m_hits->end();
    for (int maxLayers = 5; maxLayers != 2; --maxLayers)
    {	
	if (layers <= maxLayers) continue;
	--last;
	while (layers > maxLayers)
	{
	    --layers;
	    do
	    { 
		--last;
	    } while ((**last).status() != scatterer);
	}
	if (m_perigeeParameters	== 0) m_perigeeParameters = new PerigeeParameters;
	m_perigeeParameters->fillPerigee(*perigeeParameters);
	m_fitter->fit(truncated,
		      m_fitQuality,
		      *m_perigeeParameters,
		      m_hits->begin(),
		      ++last);

	// check fit OK
	if (m_fitQuality->fit_code()) return;

// 	m_perigeeParameters->print();
// 	--last;
// 	std::cout << " bremFitter " << (**last).position().perp() << std::endl;
// 	++last;
	
	// check for an acceptably precise fit (15%) with significant pt improvement:
	double 			inversePt	= m_perigeeParameters->qOverPt();
	double 			ratio 		= oldInversePt/inversePt;
	const AmgSymMatrix(5)& covariance	= m_perigeeParameters->covariance();
	double	       		ptError		= sqrt(covariance(4,4))/std::abs(inversePt);
	if (ptError > 0.15 || ratio < (1. + ptError))	return;
	delete perigeeParameters;
	perigeeParameters	= m_perigeeParameters;
	m_perigeeParameters	= 0;
    }
}

//---------------------------------------------------------------------------

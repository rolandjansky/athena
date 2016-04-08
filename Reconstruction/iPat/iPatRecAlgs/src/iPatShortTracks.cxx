/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 iPatShortTracks Algorithm - produce a ShortTrackCollection containing
 track refits to a subset of hits according to the jobOptions configuration.
 Only high quality (zero holes) tracks produced.
 
 ATLAS Collaboration
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iostream>
#include <iomanip>
#include "GaudiKernel/SystemOfUnits.h"
#include "iPatInterfaces/IiPatFitter.h"
#include "iPatRecAlgs/iPatShortTracks.h"
#include "iPatTrack/HitList.h"
#include "iPatTrack/Track.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

iPatShortTracks::iPatShortTracks(const std::string& name, ISvcLocator* pSvcLocator)
    :	AthAlgorithm		(name, pSvcLocator),
	m_trackFitter		("TrackFitter/TrackFitter"),
	m_detectorOption	("pixel"),
	m_minPt			(3.0*Gaudi::Units::GeV),
	m_shortTracksName    	("iPatShortTracks"),
	m_tracksName		("iPatTracks"),
	m_tracks		(0),
	m_trackCount		(0)
{
    declareProperty("TrackFitter",	m_trackFitter);
    declareProperty("DetectorOption",	m_detectorOption);
    declareProperty("ShortTracksName",	m_shortTracksName);
    declareProperty("TracksName",	m_tracksName);
    declareProperty("minPt",		m_minPt);
}

//--------------------------------------------------------------------------
iPatShortTracks::~iPatShortTracks(void)
{}

//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>

StatusCode
iPatShortTracks::initialize()
{
    // print where you are
    ATH_MSG_INFO( "iPatShortTracks::initialize()" );

    // get the Tools
    if (m_trackFitter.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackFitter );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackFitter );
    }
    
    // check tracks have a name
    if (m_shortTracksName == "")
    {
	ATH_MSG_FATAL( "No name set for iPat ShortTracks" );
	return StatusCode::FAILURE;
    }

    if (m_tracksName == "")
    {
	ATH_MSG_FATAL( "No name set for iPat Tracks" );
	return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
    
StatusCode
iPatShortTracks::execute()
{
    ATH_MSG_DEBUG( "entered execution" );

    // Get hold of Tracks from TDS
    StatusCode sc	= evtStore()->retrieve(m_tracks, m_tracksName);
    if (sc.isFailure())
    {
	ATH_MSG_ERROR( "iPatShortTracks::Could not retrieve TrackContainer" );
	return StatusCode::SUCCESS;
    }

    // create a ShortTrackCollection
    iPatTrackContainer* shortTracks = new iPatTrackContainer;

    for (std::vector<Track*>::const_iterator t = m_tracks->begin();
	 t != m_tracks->end();
	 ++t)
    {
	// select primary tracks with sufficient pT
	if ((**t).status() == secondary
	    || (**t).perigee_parameters().abs_pt() < m_minPt) 
	{
	    // would like to keep track container vectors in phase,
	    // but the following segv's
	    // 	    shortTracks->push_back(0);
	    continue;
	}
	
	// copy hits up to selected layer
	bool		quit		= false;
	int		layers		= 0;
	int		pixClusters	= 0;
	int		sctClusters	= 0;
	// value to essentially remove influence of hit on fit
	double		largeSigma	= 10.*Gaudi::Units::mm;
	HitList::hit_citerator	h	= (**t).hit_list_begin();
	hit_list* 	hits   		= new hit_list;
	bool		haveTrt		= false;
	while (! quit && h != (**t).hit_list_end()) 
	{
	    // bool	havePixel	= false;
	    // bool	haveSct		= false;
	    HitOnTrack* hot = new HitOnTrack(**h);
	    hits->push_back(hot);

	    // count clusters (need min 4)
	    if ((**h).isCluster())
	    {
		if ((**h).isPixel())
		{
		    // havePixel	= true;
		    ++pixClusters;
		    // set r-phi essentially unmeasured if pixelOption not selected
		    // next few lines need updating to set rot's !
		    if (m_detectorOption != "pixel")
		    {
			if (m_detectorOption == "trt")
			{
			    hot->sigma(largeSigma);
			    hot->rot(0);
			}
			else
			{
			    hits->pop_back();
			}
		    }
		}
		else
		{
		    // haveSct	= true;
		    ++sctClusters;
		    if (m_detectorOption == "trt")
		    {
			hot->sigma(largeSigma);
			hot->rot(0);
		    }
		}
	    }
	    
 	    // (**h).print();
	    
	    // count maximum of 2 pixel layers
	    if ((**h).isPixel() && layers > 1)
	    {
		layers = 1;
	    }
	    else if ((**h).status() == scatterer)
	    {
		++layers;
		if (m_detectorOption == "pixel" && sctClusters) quit = true;
	    }
	    else if ((**h).isDrift())
	    {
		haveTrt	= true;
		if (m_detectorOption == "sct")
		{
		    quit = true;
		    hits->pop_back();
		}
	    }
	    ++h;
	}

	// apply quality cuts:
	//  pixel option: NO holes allowed, 2 pix layers and 2 sct clusters
	//  sct   option: 8 sct clusters
	//  trt   option: no cuts - just deweighting
	HitQuality*	 hitQuality = new HitQuality(hits->begin(),hits->end());
	if ((m_detectorOption == "pixel" && hitQuality->number_planar_holes())
	    || layers < 3
	    || (m_detectorOption == "pixel" && (pixClusters < 2 || sctClusters < 2))
	    || (m_detectorOption == "sct" && sctClusters < 8)
	    || (m_detectorOption == "trt" && sctClusters < 6)
	    || (m_detectorOption == "trt" && ! haveTrt))
	{
	    for (HitList::hit_citerator h = hits->begin(); h != hits->end(); ++h) delete *h;
	    delete hits;
	    delete hitQuality;
	    continue;
	}
	
	// build short track
	FitQuality*		fitQuality		= 0;
	parameter_vector*	scattererParameters	= 0;
	PerigeeParameters*	perigeeParameters	= new PerigeeParameters;
	perigeeParameters->fillPerigee((**t).perigee_parameters());
	m_trackFitter->fitWithResiduals(truncated,
					fitQuality,
					*perigeeParameters,
					scattererParameters,
					hits->begin(),
					hits->end());

	// check fit succeeded
	if (fitQuality->fit_code())
	{
	    for (HitList::hit_citerator h = hits->begin(); h != hits->end(); ++h) delete *h;
	    delete hits;
	    delete hitQuality;
	    delete fitQuality;
	    delete perigeeParameters;
	    for (parameter_iterator s = scattererParameters->begin();
		 s != scattererParameters->end();
		 ++s)
		delete *s;
	    delete scattererParameters;
	    continue;
	}    

	// store shortTrack
	// 	std::cout << "                ";
	// 	perigeeParameters->print();
	// 	std::cout << " short: layers " << layers
	// 		  << "  pix,sctClusters " << pixClusters << " " << sctClusters << std::endl;
	// 	std::cout << std::setiosflags(std::ios::fixed)
	// 		  << "       diagonal errors"
	// 		  << std::setw(8) << std::setprecision(2)
	// 		  << std::sqrt(perigeeParameters->covariance()[0][0])
	// 		  << std::setw(8) << std::setprecision(2)
	// 		  << std::sqrt(perigeeParameters->covariance()[1][1])
	// 		  << std::setw(18) << std::setprecision(4)
	// 		  << std::sqrt(perigeeParameters->covariance()[2][2])
	// 		  << std::setw(8) << std::setprecision(4)
	// 		  << std::sqrt(perigeeParameters->covariance()[3][3])
	// 		  << std::setw(12) << std::setprecision(2)
	// 		  << (std::sqrt(perigeeParameters->covariance()[4][4]) *
	// 		      perigeeParameters->abs_pt() *
	// 		      perigeeParameters->abs_pt() / Gaudi::Units::GeV)
	// 		  << std::endl;
	Track* shortTrack = new Track(truncated,
				      hits,
				      scattererParameters,
				      perigeeParameters,
				      fitQuality,
				      hitQuality);
	shortTrack->truthAssociation(new TruthAssociation((**t).truthAssociation()));
 	shortTracks->push_back(shortTrack);
	++m_trackCount;
    }

    // record and lock the ShortTrackCollection
    //     shortTracks->print();
    sc = evtStore()->record(shortTracks, m_shortTracksName, false);
    if (sc.isFailure())
    {
	ATH_MSG_WARNING( "iPatRec::Could not record ShortTracks" );
	return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG( "exit execution" );

    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode
iPatShortTracks::finalize()
{
    ATH_MSG_INFO( m_trackCount << " short tracks produced" );
    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------



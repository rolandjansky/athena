/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 tool defining which of the Pixel,SCT and TRT detectors are to be processed
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "iPatGeometryTools/DetectorSelection.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

DetectorSelection::DetectorSelection (const std::string&	type,
				      const std::string&	name, 
				      const IInterface*		parent)
    :	AthAlgTool		(type, name, parent),
	m_processPixels		(true),
	m_processSCT		(true),
	m_processTRT		(true)
{
    declareInterface<IDetectorSelection>(this);
    declareProperty("processPixels",	m_processPixels);
    declareProperty("processSCT",	m_processSCT);
    declareProperty("processTRT",	m_processTRT);
}

DetectorSelection::~DetectorSelection (void) 
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
DetectorSelection::initialize()
{
    // print where you are
    ATH_MSG_DEBUG( "DetectorSelection::initialize()"
		   << "-package version " << PACKAGE_VERSION );

    return StatusCode::SUCCESS;
}

StatusCode
DetectorSelection::finalize()
{
    return StatusCode::SUCCESS;
}


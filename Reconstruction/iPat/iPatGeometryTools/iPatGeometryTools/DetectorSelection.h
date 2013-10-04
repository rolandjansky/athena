/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 tool defining which of the Pixel,SCT and TRT detectors are to be processed
 ***************************************************************************/

#ifndef IPATGEOMETRYTOOLS_DETECTORSELECTION_H
# define IPATGEOMETRYTOOLS_DETECTORSELECTION_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "iPatInterfaces/IDetectorSelection.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class DetectorSelection: public AthAlgTool,
			 virtual public IDetectorSelection
{

public:
    DetectorSelection		(const std::string&	type, 
				 const std::string&	name,
				 const IInterface*	parent);
    ~DetectorSelection		(void); 	// destructor

    StatusCode			initialize();
    StatusCode			finalize();

    bool			processPixels (void) const;
    bool			processSCT (void) const;
    bool			processTRT (void) const;

private:
    
    // configuration
    bool		m_processPixels;
    bool		m_processSCT;
    bool		m_processTRT;
};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline bool
DetectorSelection::processPixels (void) const
{ return m_processPixels; }

inline bool
DetectorSelection::processSCT (void) const
{ return m_processSCT; }

inline bool
DetectorSelection::processTRT (void) const
{ return m_processTRT; }

#endif // IPATGEOMETRYTOOLS_DETECTORSELECTION_H


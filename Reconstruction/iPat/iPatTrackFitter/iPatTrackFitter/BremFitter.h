/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 simple brem recovery procedure - written in a hurry to meet DC1 deadline
 
 ATLAS Collaboration
 ***************************************************************************/

#ifndef IPATTRACKFITTER_BREMFITTER_H
# define IPATTRACKFITTER_BREMFITTER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/AlgTool.h"
#include "iPatTrack/HitList.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class FitQuality;
class PerigeeParameters;
class TrackFitter;

class BremFitter: public AlgTool
{

public:

    BremFitter			(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    ~BremFitter			(void); 	// destructor

    StatusCode			initialize();
    StatusCode			finalize();

    // replace PerigeeParameters if brem recovery successful
    void	bremRecovery(PerigeeParameters*&	perigeeParameters,
			     HitList::hit_citerator	begin,
			     HitList::hit_citerator	end);
    
private:

    FitQuality*				m_fitQuality;
    TrackFitter*			m_fitter;
    HitList::hit_list*			m_hits;
    PerigeeParameters*			m_perigeeParameters;
};

#endif // IPATTRACKFITTER_BREMFITTER_H

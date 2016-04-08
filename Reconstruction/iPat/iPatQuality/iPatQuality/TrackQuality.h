/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 apply centralised track quality cuts and definitions
 ***************************************************************************/

#ifndef IPATQUALITY_TRACKQUALITY_H
#define IPATQUALITY_TRACKQUALITY_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "iPatInterfaces/ITrackQuality.h"
#include "iPatTrack/TrackStatus.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class TrackQuality: public AthAlgTool,
		    virtual public ITrackQuality
{
public:

    TrackQuality	(const std::string& type,
			 const std::string& name,
			 const IInterface* parent);
    ~TrackQuality	(void);

    StatusCode		initialize();
    StatusCode		finalize();

//     bool		continuityCheck (std::list<HitOnTrack*>::const_iterator begin,
// 					 std::list<HitOnTrack*>::const_iterator end) const;
    bool		aboveMinPt	(const PerigeeParameters&) const;
    bool		goodCandidate	(const FitQuality&, const PerigeeParameters&) const;
    bool		goodTrack	(TrackStatus, const FitQuality&, const HitQuality&) const;
    int			maxReject	(const HitQuality&) const;
    void		minPt		(double minPt);
    void		print		(const FitQuality&, const PerigeeParameters&) const;
    void		print		(const HitQuality&) const;
    bool		sufficientHits	(TrackStatus				status,
					 HitQuality&				hitQuality,
					 std::list<HitOnTrack*>::const_iterator	begin,
					 std::list<HitOnTrack*>::const_iterator	end) const;
private:
    void		setStatus (TrackStatus status) const;

    // state (mutable where used for local communication)
    mutable int			m_maxPlanarHoles;
    mutable int			m_minEffectiveLayers;
    mutable double		m_minFitProbability;
    mutable int			m_minPlanarClusters;
    double			m_minPt;
    mutable TrackStatus		m_status;
};

#endif	// IPATQUALITY_TRACKQUALITY_H

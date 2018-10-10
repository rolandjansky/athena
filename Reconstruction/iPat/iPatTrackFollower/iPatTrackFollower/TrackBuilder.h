/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 build a Track from the hit_list formed by TrackFollowing an existing Track
 --------------------------------------------------------------------------
 ***************************************************************************/

#ifndef IPATTRACKFOLLOWER_TRACKBUILDER_H
# define IPATTRACKFOLLOWER_TRACKBUILDER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatInterfaces/ITrackBuilder.h"
#include "iPatTrack/HitList.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class ITrackQuality;
class IiPatFitter;
class MsgStream;

class TrackBuilder: public AthAlgTool,
		    virtual public ITrackBuilder
{

public:
    // constructor
    TrackBuilder 	(const std::string& type,
			 const std::string& name,
			 const IInterface* parent);
    ~TrackBuilder	(void);
 
    StatusCode		initialize();
    StatusCode		finalize();

    Track*		trackFromHits (TrackStatus,
				       std::list<HitOnTrack*>*,
				       const Track&) const;
    Track*		trackRefit (const Track&) const;
    
private:

    // helpers, managers, tools
    ToolHandle<IiPatFitter>	m_trackFitter;
    ToolHandle<ITrackQuality>  	m_trackQuality;

    // counters
    mutable int			m_buildCount;

};

#endif // IPATTRACKFOLLOWER_TRACKBUILDER_H

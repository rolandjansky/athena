/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 iPatShortTracks Algorithm - produce a ShortTrackCollection containing
 track refits to a subset of hits according to the jobOptions configuration.
 
 ATLAS Collaboration
 ***************************************************************************/

#ifndef IPATRECALGS_IPATSHORTTRACKS_H
# define IPATRECALGS_IPATSHORTTRACKS_H

//<<<<<< INCLUDES >>>>>>

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatRecEvent/iPatTrackContainer.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IiPatFitter;
class StoreGateSvc;

class iPatShortTracks:public AthAlgorithm
{
   
public:

    iPatShortTracks(const std::string& name, ISvcLocator* pSvcLocator);
    ~iPatShortTracks(void);

    StatusCode	initialize(void);    
    StatusCode	execute(void);
    StatusCode	finalize(void);  
    
private:
    
    // helpers, managers, tools
    ToolHandle<IiPatFitter>			m_trackFitter;
    
    // configuration
    std::string					m_detectorOption;
    double				       	m_minPt;
    std::string					m_shortTracksName;
    std::string					m_tracksName;

    // data access
    const iPatTrackContainer*		      	m_tracks;
    
    // counters
    int					       	m_trackCount;
};

#endif // IPATRECALGS_IPATSHORTTRACKS_H





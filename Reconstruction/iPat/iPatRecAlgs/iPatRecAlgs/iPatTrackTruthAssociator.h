/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 iPatTrackTruthAssociator - get truth (barcode) for each found track 
 
 ATLAS Collaboration
 ***************************************************************************/

#ifndef IPATRECALGS_IPATTRACKTRUTHASSOCIATOR_H
# define IPATRECALGS_IPATTRACKTRUTHASSOCIATOR_H

//<<<<<< INCLUDES >>>>>>

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class HitOnTrack;
class IDetectorSelection;
class ITruthSelector;
class iPatTrackContainer;

class iPatTrackTruthAssociator:public AthAlgorithm
{

public:

    iPatTrackTruthAssociator(const std::string& name, ISvcLocator* pSvcLocator);
    ~iPatTrackTruthAssociator(void);

    StatusCode	initialize(void);    
    StatusCode	execute(void);
    StatusCode	finalize(void);  
    
private:
    
    bool					inB_Layer(const HitOnTrack&);

    ToolHandle<IDetectorSelection>		m_detectorSelection;
    ToolHandle<ITruthSelector>			m_truthSelector;
    std::string					m_tracksLocation;
    int						m_backgroundCount;
    int						m_eventCount;
    int						m_fakeCount;
    bool					m_haveTruth;
    const iPatTrackContainer*			m_tracks;
    int						m_wrongCount;
};

#endif // IPATRECALGS_IPATTRACKTRUTHASSOCIATOR_H





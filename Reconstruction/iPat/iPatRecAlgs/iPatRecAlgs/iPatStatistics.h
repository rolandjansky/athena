/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 iPatStatistics Algorithm - produce SpacePoint and track statistics.
 Summary printed at finalize step.
 
 ATLAS Collaboration
 ***************************************************************************/

#ifndef IPATRECALGS_IPATSTATISTICS_H
# define IPATRECALGS_IPATSTATISTICS_H

//<<<<<< INCLUDES >>>>>>

#include <string>
#include <vector>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatRecEvent/iPatTrackContainer.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IDetectorSelection;
class ILayerNumberAllocator;
class ITruthParameters;
class ITruthSelector;

class iPatStatistics:public AthAlgorithm
{
   
public:

    iPatStatistics(const std::string& name, ISvcLocator* pSvcLocator);
    ~iPatStatistics(void);

    StatusCode	initialize(void);    
    StatusCode	execute(void);
    StatusCode	finalize(void);
    
private:
      
    // helpers, managers, tools
    ToolHandle<IDetectorSelection>			m_detectorSelection;
    ToolHandle<ILayerNumberAllocator>			m_layerNumberAllocator;
    ToolHandle<ITruthParameters>		       	m_truthParameters;
    ToolHandle<ITruthSelector>			       	m_truthSelector;

    // configuration
    bool						m_haveTruth;
    double						m_minPt;
    std::string						m_overlapSpacePointsLocation;
    std::string						m_pixelSpacePointsLocation;
    std::string						m_sctSpacePointsLocation;
    std::string						m_tracksLocation;

    // internal communication
    const iPatTrackContainer*				m_tracks;
    
    //counters
    unsigned						m_countPixelBroad;
    unsigned						m_countPixelHits;
    unsigned						m_countPixelXXX;
    unsigned						m_countPixelXXO;
    unsigned						m_countPixelXOX;
    unsigned						m_countPixelOXX;
    unsigned						m_countPixelOOX;
    unsigned						m_countPrimaryXXX;
    unsigned						m_countPrimaryXXO;
    unsigned						m_countPrimaryXOX;
    unsigned						m_countPrimaryOXX;
    unsigned						m_countPrimaryOOX;
    unsigned						m_countSctBroad;
    unsigned						m_countSctHits;
    unsigned						m_countTrtAssociated;
    unsigned						m_countTrtBroad;
    unsigned						m_countTrtHits;
    unsigned						m_countTrtMissed;
    unsigned						m_countTrtTruthAssociated;
    unsigned						m_countTrtTruthMissed;
    int							m_eventCount;
    std::vector<int>   				       	m_primaryCount;
    std::vector<int>   				       	m_secondaryCount;
    bool					       	m_selectSpacePoints;
    bool					       	m_selectTracks;
    std::vector<int>					m_spacePointsByLayer;
    int							m_spacePointCount;
    std::vector<int>			       		m_trackClusters;
    std::vector<int>					m_trackCount;
    std::vector<int>					m_trackFakes;
    std::vector<int>					m_trackHoles;
    std::vector<int>					m_trackMissedPrimary;
    std::vector<int>					m_trackMissedSecondary;
    std::vector<int>					m_trackPrimary;
    std::vector<int>					m_trackSecondary;
    std::vector<int>					m_trackStraws;
};

#endif // IPATRECALGS_IPATSTATISTICS_H





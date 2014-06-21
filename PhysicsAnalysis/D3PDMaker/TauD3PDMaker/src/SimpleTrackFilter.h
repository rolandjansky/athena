/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMPLETRACKFILTER_H
#define SIMPLETRACKFILTER_H

// 
//     Algorithm to filter TrackParticleContainer
// Dugan O'Neil July 2010

#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/ToolHandle.h"

class SimpleTrackFilter: public AthAlgorithm
{
public:    

    SimpleTrackFilter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SimpleTrackFilter();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private: 

    std::string     m_tracksName;     //track container
    std::string     m_outputTracksName;     //output Track container
    float           m_ptCut;                //min track pt
    int             m_nSCTPixHitsCut;     //nSCTHits + nPixHits > m_nSCTPixHitsCut
};

#endif






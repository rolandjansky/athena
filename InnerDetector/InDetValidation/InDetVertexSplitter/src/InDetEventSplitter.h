/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_INDETEVENTSPLITTER_H 
#define INDET_INDETEVENTSPLITTER_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : InDetEventSplitter.h
/// Package : offline/InnerDetector/InDetCalibAlgs/InDetBeamSpotFinder
/// Author  : Peter V. Loscutoff
/// Created : February 2009
///
/// DESCRIPTION:
///
/// This class will create track collections from the the input list of vertices.
///  One set of track collections will contain all the tracks that contributed to a vertex.
///  The other sets of track collections (odd and even) will each contain half the tracks.
/// 
/// Peter V.Loscutoff, February 2009
/// 
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ObjectVector.h"
//#include "StoreGate/StoreGateSvc.h"

#include <string>
#include <vector>

namespace InDet {
  
  class InDetEventSplitter : public AthAlgorithm {
    
  public:
    
    InDetEventSplitter(const std::string& name, ISvcLocator* pSvcLocator);
    ~InDetEventSplitter();
    
    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();
    
  private:
    
    std::vector<std::string> m_trackKeys;
    
    bool isOdd;
    int addToVx;

    StatusCode split_vertices();
    
    /// containers to retrieve
    std::string m_tpbContainerName;
    std::string m_trackContainerName;
    int m_maxVtx;
    bool m_priOnly;
    bool m_savetpb;
    int m_rndSeed;

    int eventN;
  };
}

#endif // INDET_INDETEVENTSPLITTER_H


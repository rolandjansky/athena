/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/***************************************************************************
TrkAmbiguityScore Algorithm 
***************************************************************************/


#ifndef TRKAMBIGUITYSCORE_H
#define TRKAMBIGUITYSCORE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
#include <string>

namespace Trk 
{
  class ITrackAmbiguityProcessorTool;
  class ITrackAmbiguityScoreProcessorTool;

  class TrkAmbiguityScore : public AthAlgorithm
  {
    public:
      TrkAmbiguityScore(const std::string& name, ISvcLocator* pSvcLocator);
      ~TrkAmbiguityScore(void);
      
      StatusCode	      initialize(void) override;
      StatusCode	      execute(void) override;
      StatusCode	      finalize(void) override;

    private:
      SG::ReadHandleKeyArray<TrackCollection>       m_originTracksKey;
      SG::WriteHandleKey<std::multimap<const Track*, float>> m_scoredTracksKey;
      ToolHandle<ITrackAmbiguityScoreProcessorTool> m_scoreTool;
    };

}


#endif 





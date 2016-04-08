/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSlimmer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKTRACKSLIMMER_TRK_TRACKSLIMMER_H
#define TRKTRACKSLIMMER_TRK_TRACKSLIMMER_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

class AtlasDetectorID;
class Identifier;

namespace Trk 
{
  class ITrackSlimmingTool;
  
  /** @class TrackSlimmer
      
  Simple algorithm to load tracks and use Trk::ITrackSlimmingTool to e.g. strip them down
  to hits + perigee parameters for use in AOD.
  
  @author  Edward Moyse <Edward.Moyse@cern.ch>
  */  
  
  class TrackSlimmer : public AthAlgorithm
    {
    public:
      
      /** Standard Athena-Algorithm Constructor */
      TrackSlimmer(const std::string& name, ISvcLocator* pSvcLocator);
      /** Default Destructor */
      ~TrackSlimmer();
      
      /** standard Athena-Algorithm method */
      StatusCode          initialize();
      /** standard Athena-Algorithm method */
      StatusCode          execute();
      /** standard Athena-Algorithm method */
      StatusCode          finalize();
      
    private:
      
      /** member variables for algorithm properties: */
      // int/double/bool  m_propertyName;
      ToolHandle<ITrackSlimmingTool>        m_slimTool;
      
      std::vector<std::string>              m_trackLocation;
      
      std::vector<std::string>              m_slimmedTracksLocation;
      
      double                                m_ptCut;
      
      unsigned int m_numSlimmedTracks;
      unsigned int m_numOriginalTracks;      
      
    }; 
} // end of namespace

#endif 

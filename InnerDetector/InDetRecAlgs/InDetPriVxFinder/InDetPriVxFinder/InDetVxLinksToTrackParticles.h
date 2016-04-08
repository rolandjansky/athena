/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetVxLinksToTrackParticles.h  -  Description
                             -------------------
    begin   : 27-01-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :
 ***************************************************************************/

#ifndef InDetVxLinksToTrackParticles_InDetVxLinksToTrackParticles_H
#define InDetVxLinksToTrackParticles_InDetVxLinksToTrackParticles_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

/** 
 *  A non-so-simple algorithm setting the links from 
 *  xAOD::TrackParticles to xAOD::Vertices correctly.
 * 
 *  Kirill Prokofiev, March 2014
 */

namespace Reco {
  class ITrackToVertex;
}

namespace InDet
{
  class IVertexFinder;
  
  class InDetVxLinksToTrackParticles : public AthAlgorithm
  {
  public:
    InDetVxLinksToTrackParticles(const std::string &name, ISvcLocator *pSvcLocator);

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    
  private:
    std::string m_verticesName; //!< Name of the vertex container
    std::string m_tracksName; //!< Name of the track container
    std::string m_tracksOutName; //!< Optional name to create a shallow copy for the linked and modified track particles.
    
    ToolHandle<Reco::ITrackToVertex>  m_trackToVertex;

    bool m_copy;
  };//end of class
}//end of namespace definitions
#endif

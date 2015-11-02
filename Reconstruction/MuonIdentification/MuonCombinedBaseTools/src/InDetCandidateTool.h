/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_INDETCANDIDATETOOL_H
#define MUONCOMBINEDBASETOOLS_INDETCANDIDATETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IInDetCandidateTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"

namespace Trk {
  class ITrackSelectorTool;
}

namespace MuonCombined {

  class InDetCandidateTool: public AthAlgTool, virtual public IInDetCandidateTool
  {

  public:
    InDetCandidateTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~InDetCandidateTool(void); // destructor
  
    StatusCode initialize();
    StatusCode finalize();

    /**IInDetCandidateTool interface: build an InDetCandidateCollection from a InDet TrackParticleContainer */
    void create( const xAOD::TrackParticleContainer& indetTrackParticles, InDetCandidateCollection& outputContainer ) const;

  private:
    ToolHandle <Trk::ITrackSelectorTool>               m_trackSelector; //!< Tool to select tracks
    bool m_flagCandidateAsSiAssociated;

  };

}	// end of namespace

#endif 



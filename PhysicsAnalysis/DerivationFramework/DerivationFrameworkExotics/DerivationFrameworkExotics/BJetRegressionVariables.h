/////////////////////////////////////////////////////////////////
// BJetRegressionVariables.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_BJetRegressionVariables_H
#define DERIVATIONFRAMEWORK_BJetRegressionVariables_H

/** @class BJetRegressionVariables
    @author Alexander Melzer (alexander.melzer@cern.ch)
*/

#include "AsgTools/ToolHandle.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#include "JetEDM/TrackVertexAssociation.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>
#include <string>


namespace DerivationFramework {
 
  class BJetRegressionVariables : public AthAlgTool, public IAugmentationTool {
  public:
    BJetRegressionVariables(const std::string& t, const std::string& n, const IInterface* p);

    ~BJetRegressionVariables();
     
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;
 
  private:
    std::string m_containerName; 
    std::string m_assocTracksName;
    std::vector<float> m_minTrackPt;
    std::string m_vertexContainer;
    bool m_requireTrackPV;
    InDet::InDetTrackSelectionTool* m_TrackSelTool;
    std::string m_tva;

    struct TrackMomentStruct {float vecSumPtTrk; float scalSumPtTrk; float vecSumPtTrkClean; float scalSumPtTrkClean;};

    TrackMomentStruct getSumTrackPt( const float minTrackPt, 
				     const std::vector<const xAOD::TrackParticle*>& tracks, 
				     const xAOD::Vertex* vertex, 
				     const jet::TrackVertexAssociation* tva) const;

    const std::string getMomentBaseName(const float minTrackPt, const bool pv0) const;
  };
}

#endif // DERIVATIONFRAMEWORK_BJetRegressionVariables_H

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexTrackIsolation.h,
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_VertexTrackIsolation_H
#define DERIVATIONFRAMEWORK_VertexTrackIsolation_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
//#include "MuonIdHelpers/MuonIdHelperTool.h"
//#include "IsolationTool/TrackIsolationTool.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
//#include "xAODPrimitives/IsolationType.h"
#include <vector>
/** THE reconstruction tool
 */
namespace DerivationFramework {

  class VertexTrackIsolation : public AthAlgTool, public IAugmentationTool {
    public: 
      VertexTrackIsolation(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      
      virtual StatusCode addBranches() const;

      bool isSame(const xAOD::Vertex* theVtx1, const xAOD::Vertex* theVtx2) const;
      bool isContainedIn(const xAOD::Vertex* theVtx, const std::vector<const xAOD::Vertex*> &theColl) const;
      
    private:

	ToolHandle<xAOD::ITrackIsolationTool> m_trackIsoTool;
	std::string m_trackContainerName;
	std::string m_vertexContainerName;
	std::vector<unsigned int> m_cones;
	std::vector<std::string> m_passFlags;
	int m_vertexType; 	//Which type of primary vertices should be used? (7 = 0b111 are all at the moment)

        bool m_doIsoPerTrk;
        int m_removeDuplicate;
  }; 
}

#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Reco_mumu.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_Reco_mumu_H
#define DERIVATIONFRAMEWORK_Reco_mumu_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/JpsiFinder.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"

/** forward declarations
 */
namespace Trk {
  class V0Tools;
}

class IBeamCondSvc;

/** THE reconstruction tool
 */
namespace DerivationFramework {

  class Reco_mumu : public AthAlgTool, public IAugmentationTool {
    public: 
      Reco_mumu(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      
      virtual StatusCode addBranches() const;
      
    private:
      /** tools
       */
      ToolHandle<Trk::V0Tools>                    m_v0Tools;
      ToolHandle<Analysis::ICandidateSearch>      m_jpsiFinder;
      ToolHandle<Analysis::PrimaryVertexRefitter> m_pvRefitter;
      ServiceHandle<IBeamCondSvc>                 m_beamSpotSvc;
      
      /** job options
       */
      std::string m_outputVtxContainerName;
      std::string m_pvContainerName;
      std::string m_refPVContainerName;
      bool        m_refitPV;
      int         m_PV_max;
      int         m_DoVertexType;
      size_t      m_PV_minNTracks;
      bool        m_do3d;
      bool        m_checkCollections;
      std::vector<std::string> m_CollectionsToCheck;
  }; 
}

#endif // DERIVATIONFRAMEWORK_Reco_mumu_H

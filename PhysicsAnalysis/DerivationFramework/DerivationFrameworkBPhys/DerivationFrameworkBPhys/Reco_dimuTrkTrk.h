/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Reco_dimuTrk.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_Reco_dimuTrkTrk_H
#define DERIVATIONFRAMEWORK_Reco_dimuTrkTrk_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/JpsiPlus2Tracks.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"

/** forward declarations
 */
namespace Trk {
  class V0Tools;
}
class IBeamCondSvc;

namespace DerivationFramework {

  class Reco_dimuTrkTrk : public AthAlgTool, public IAugmentationTool {
    public: 
      Reco_dimuTrkTrk(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      
      virtual StatusCode addBranches() const;
      
    private:
      
      ToolHandle<Trk::V0Tools>                    m_v0Tools;
      ToolHandle<Analysis::JpsiPlus2Tracks>        m_jpsi2Track;
      ToolHandle<Analysis::PrimaryVertexRefitter> m_pvRefitter;
      ServiceHandle<IBeamCondSvc>                 m_beamSpotSvc;

      std::string m_outputVtxContainerName;
      std::string m_pvContainerName;
      std::string m_refPVContainerName;
      bool        m_refitPV;
      //This parameter will allow us to optimize the number of PVs under consideration as the probability
      //of a useful primary vertex drops significantly the higher you go
      int         m_PV_max;
      int         m_DoVertexType;
      size_t      m_PV_minNTracks;
  }; 
}

#endif // DERIVATIONFRAMEWORK_Reco_dimuTrkTrk_H


/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Bmumu_reco_mumu.h
///////////////////////////////////////////////////////////////////
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Original author (Reco_mumu):
//          Daniel Scheirich <daniel.scheirich@cern.ch>
// 
// Changes:
// Basic dimuon reconstruction for the derivation framework.
// This class inherits from CfAthAlgTool instead of AthAlgTool in order
// to have access to the CutFlowSvc instance.
//
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_Bmumu_reco_mumu_H
#define DERIVATIONFRAMEWORK_Bmumu_reco_mumu_H

#include <string>

#include "DerivationFrameworkBPhys/CfAthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/JpsiFinder.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

/** forward declarations
 */
namespace Trk {
  class V0Tools;
}

/** THE reconstruction tool
 */
namespace DerivationFramework {

  class Bmumu_reco_mumu : public CfAthAlgTool, public IAugmentationTool {
    public: 
      Bmumu_reco_mumu(const std::string& t, const std::string& n,
		      const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      
      virtual StatusCode addBranches() const;
      
    private:
      /** tools
       */
      ToolHandle<Trk::V0Tools>                    m_v0Tools;
      ToolHandle<Analysis::JpsiFinder>            m_jpsiFinder;
      ToolHandle<Analysis::PrimaryVertexRefitter> m_pvRefitter;
      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
      
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
  }; 
}

#endif // DERIVATIONFRAMEWORK_Bmumu_reco_mumu_H

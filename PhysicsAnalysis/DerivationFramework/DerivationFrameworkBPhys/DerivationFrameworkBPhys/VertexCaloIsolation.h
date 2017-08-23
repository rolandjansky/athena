/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexCaloIsolation.h  by Matteo Bedognetti
///////////////////////////////////////////////////////////////////
//
// This code is based on CaloIsolationTool of IsolationTools package
//
// Etcone is determined as a topoCluster-isolation value minus Energy Density (ED) correction and minus the energy depositions of the muons
// Muon's energy deposition is already stored in side the xAOD::Muon objects, but the muon-clusters are used to correct for the fact that they muons may have overlapping clusters
// The muon-clusters are stored as well in connection with the muons themselves
//
// The idea of comparing topoClusters with muon-clusters to decide what part of the muon's deposition is of 
// importance had to be abandoned because topCluster cells are not present in xAOD
//
// It enforces the fact that for muons no core-surface is removed for the energy-density correction (thus the corrections are independent from each other)
//
// "isReliable" flag reports of each isolation value if all particles crossing the cone have been correctly corrected for.
// In the case of 2mu+ 1 track it mirrors the fact that the track does not extrapolate into the cone (as tracks have no muon-cluster from which to determine the core-correction)
//
#ifndef DERIVATIONFRAMEWORK_VertexCaloIsolation_H
#define DERIVATIONFRAMEWORK_VertexCaloIsolation_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
//#include "MuonIdHelpers/MuonIdHelperTool.h"
//#include "IsolationTool/CaloIsolationTool.h"
//#include "TrkCaloExtension/CaloExtension.h"

#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"

//#include "TrackToCalo/CaloCellCollector.h"
//#include "CaloInterface/ICaloNoiseTool.h"
#include "xAODBPhys/BPhysHelper.h"
//#include "xAODPrimitives/IsolationType.h" //


/** THE reconstruction tool
 */
namespace DerivationFramework {

  class VertexCaloIsolation : public AthAlgTool, public IAugmentationTool {
    public: 
      VertexCaloIsolation(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      
      StatusCode addBranches() const;
      bool extrapolateTrack(TLorentzVector& extr_tp, const xAOD::IParticle& tp) const;
      bool extrapolateMuon(TLorentzVector& extr_tp, const xAOD::CaloCluster* cluster) const;
      xAOD::TrackParticle&  makeSlyTrack(xAOD::TrackParticle&, const TLorentzVector& candidate, const xAOD::Vertex* vertex, xAOD::BPhysHelper::pv_type vertexType) const;


    private:

	//ToolHandle<xAOD::ICaloCellIsolationTool> m_caloIsoTool;
      ToolHandle<xAOD::ICaloTopoClusterIsolationTool> m_caloIsoTool;
      std::string m_trackContainerName;
      std::string m_vertexContainerName;
      std::string m_caloClusterContainerName;
      std::string m_cellContainerName;
      std::string m_muonContainerName;
      ToolHandle<Trk::IParticleCaloExtensionTool>              m_caloExtTool;
      std::vector<unsigned int> m_cones;  //I cannot use xAOD::Iso::IsolationType as a type here, as it clashes with setProperty()
      std::vector<std::string> m_passFlags;


    //  ToolHandle <ICaloNoiseTool>                   m_caloNoiseTool;  //Removed to reduce requirements
      //Rec::CaloCellCollector   m_cellCollector;		//Seems to be a plain class, so no need for handles

      bool m_applyCaloNoiseCut;
      /// Number of sigma for calo cell noise cut
      float m_sigmaCaloNoiseCut;

  	  int m_vertexType;


	      
  }; 
}

#endif


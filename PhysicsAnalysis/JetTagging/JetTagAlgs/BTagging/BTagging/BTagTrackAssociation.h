/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_BTAGTRACKASSOCIATION_H
#define BTAGGING_BTAGTRACKASSOCIATION_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "BTagging/IBTagTrackAssociation.h"

#include <string>

/** The namespace of all packages in PhysicsAnalysis/JetTagging */
namespace Analysis
{
  class IBTagTrackAssociation;
  class ParticleToJetAssociator;

  class BTagTrackAssociation : public AthAlgTool, virtual public IBTagTrackAssociation
  {
      public:
          /** Constructors and destructors */
          BTagTrackAssociation(const std::string&,const std::string&,const IInterface*); //NameType& name);
          virtual ~BTagTrackAssociation();

          virtual StatusCode initialize() override;
          virtual StatusCode finalize() override;
	  virtual StatusCode BTagTrackAssociation_exec(jetcollection_t* theJets, const xAOD::TrackParticleContainer* tracks = 0) const override;

      private:
          bool m_BTagAssociation;

          /** Jet Track Merger, softLep Jet Electron/Muon Merger*/
          /** There are 3 lists of associator tools: to associate tracks, electrons/photons and muons
          */

          ToolHandleArray< Analysis::ParticleToJetAssociator > m_TrackToJetAssociatorList;
          ToolHandleArray< Analysis::ParticleToJetAssociator > m_ElectronToJetAssociatorList;
          ToolHandleArray< Analysis::ParticleToJetAssociator > m_MuonToJetAssociatorList;

          std::vector<std::string> m_TrackToJetAssocNameList;
          std::vector<std::string> m_ElectronToJetAssocNameList;
          std::vector<std::string> m_PhotonToJetAssocNameList;
          std::vector<std::string> m_MuonToJetAssocNameList;

          std::vector<std::string> m_TrackContainerNameList;
          std::vector<std::string> m_ElectronContainerNameList;
          std::vector<std::string> m_PhotonContainerNameList;
          std::vector<std::string> m_MuonContainerNameList;

          std::vector<std::string> m_ParticlesToAssociateList;
          std::vector<std::string> m_MuonsToAssociateList;

          std::string m_collectionAppendix ;

  }; // End class

} // End namespace

#endif // BTAGTRACKASSOCIATION_H

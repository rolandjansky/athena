/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OverlapRemovalIndices.h 643484 2015-02-02 11:23:34Z simon $
#ifndef ANALYSISTOP_TOPOBJECTSELECTONTOOLS_OVERLAPREMOVALASG_H
#define ANALYSISTOP_TOPOBJECTSELECTONTOOLS_OVERLAPREMOVALASG_H

// system include(s):
#include <vector>

// Framework includes:
#include "AsgTools/AnaToolHandle.h"

#include "TopObjectSelectionTools/OverlapRemovalBase.h"
#include "AssociationUtils/IOverlapRemovalTool.h"

namespace top {

/**
 * @brief The Harmonization Group recommended object overlap removal
 */
  class OverlapRemovalASG : public top::OverlapRemovalBase {
    public:
      /**
       * @brief Sets up ASG Overlap Removal Tool
       * 
       * @param dolargeJet Enables OR also with large-R jets
       * This is false by default becaause large-R jets are a bit special.
       *
       */
      OverlapRemovalASG(bool dolargeJet = false);

    ///does nothing
      virtual ~OverlapRemovalASG();

      /**
       * @brief Perform the "ASG standard" overlap removal.
       *
       * This runs for every event.
       *
       * @param photon All the photons in the event
       * @param el All the electrons (even bad ones).  Good ones are decorated
       * with passPreORSelection = 1.
       * @param mu All the muons in the event
       * @param tau All the taus in the event
       * @param jet All the jets in the event
       * @param ljet All the large-R jets in the event
       *
       * @param goodPhotons The indices of the photons to keep
       * @param goodElectrons The indices of the electrons to keep (e.g. the 0th, 2nd)
       * @param goodMuons The indices of the muons to keep
       * @param goodTaus The indices of the taus to keep
       * @param goodJets The indices of the jets to keep
       * @param goodLargeRJets The indices of the large-R jets to keep
       *
       */
      virtual void overlapremoval(const xAOD::PhotonContainer*   photon ,
                                  const xAOD::ElectronContainer* el  ,
                                  const xAOD::MuonContainer*     mu  ,
                                  const xAOD::TauJetContainer*   tau ,
                                  const xAOD::JetContainer*      jet ,
                                  const xAOD::JetContainer*      ljet,
                                  std::vector<unsigned int>& goodPhotons,
                                  std::vector<unsigned int>& goodElectrons,
                                  std::vector<unsigned int>& goodMuons,
                                  std::vector<unsigned int>& goodTaus,
                                  std::vector<unsigned int>& goodJets,
                                  std::vector<unsigned int>& goodLargeRJets,
				  const bool isLooseEvent );

      ///Print something useful to the screen
      virtual void print(std::ostream&) const;

  protected:

      /// For a generic container of type DataVector, fill a vector of
      /// ints for objects we want to keep
      virtual void removeObjectOverlap( const xAOD::IParticleContainer* xaod ,
                                        std::vector<unsigned int>& OUT_vec,
                                        const std::string passTopCuts);


      /// For a generic container of type DataVector, fill a vector of
      /// ints for objects we want to keep
      /// This doesn't check for overlap removal (Large R jets)
      virtual void removeObject( const xAOD::IParticleContainer* xaod ,
                                 std::vector<unsigned int>& OUT_vec,
                                 const std::string passTopCuts);

      // ASG overlap removal tool
      asg::AnaToolHandle<ORUtils::IOverlapRemovalTool> m_overlapRemovalTool;
      asg::AnaToolHandle<ORUtils::IOverlapRemovalTool> m_overlapRemovalToolLoose;

      const std::string m_passPreORSelection;
      const std::string m_passPreORSelectionLoose;
      const std::string m_overlaps; // true if object overlaps with another
      
      bool m_doLargeJet;

  };

}
#endif

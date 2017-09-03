/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_IOVERLAPREMOVALTOOL_H
#define ASSOCIATIONUTILS_IOVERLAPREMOVALTOOL_H

// Framework includes
#include "AsgTools/IAsgTool.h"

// EDM includes
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"


namespace ORUtils
{

  /// @class IOverlapRemovalTool
  /// @brief Interface for the overlap removal tool
  ///
  /// @author Steve Farrell <steven.farrell@cern.ch>
  ///
  class IOverlapRemovalTool : public virtual asg::IAsgTool
  {

      /// Declare the interface
      ASG_TOOL_INTERFACE(IOverlapRemovalTool)

    public:

      /// Top-level method for performing full overlap-removal.
      /// The individual OR methods will be called in the recommended order,
      /// and the considered objects will be decorated with the output result.
      virtual StatusCode
      removeOverlaps(const xAOD::ElectronContainer* electrons,
                     const xAOD::MuonContainer* muons,
                     const xAOD::JetContainer* jets,
                     const xAOD::TauJetContainer* taus = 0,
                     const xAOD::PhotonContainer* photons = 0,
                     const xAOD::JetContainer* fatJets = 0) const = 0;

  }; // class IOverlapRemovalTool

} // namespace ORUtils

#endif

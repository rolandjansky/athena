/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FSRUTILS_IFsrPhotonTool_H
#define FSRUTILS_IFsrPhotonTool_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODMuon/Muon.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"

namespace FSR {

   /// Simple interface for searching the FSR candidate
   ///
   /// @author Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>
   ///
   /// $Date: 2014-07-18 $
   ///
   class FsrCandidate {
   public:
      enum FsrType { FsrNear, FsrFar, FsrUnknown };

       FsrCandidate() : particle(0), deltaR(-1), Et(-1), f1(-1), eta(-1), phi(-1), phIso(-1),
                       type(FsrUnknown) {}
      std::string container;
      const xAOD::IParticle* particle;
      double  deltaR;
      double  Et;
      double  f1;
      double  eta;
      double  phi;
      double  phIso; // Only available for far photons
      FsrType type;
      bool operator<(const FsrCandidate& c) const {
          return deltaR < c.deltaR;
      }
   };

   class IFsrPhotonTool : public virtual asg::IAsgTool {

      /// Declare the interface that the class provides
      ASG_TOOL_INTERFACE( FSR::IFsrPhotonTool )

   public:
      /// Get the Ok/Error as a return value
      
      virtual CP::CorrectionCode getFsrPhoton(const xAOD::IParticle* part, FsrCandidate& candidate,
                                              const xAOD::PhotonContainer* photons,
                                              const xAOD::ElectronContainer* electrons) = 0;

      virtual std::vector<FsrCandidate>* getFsrCandidateList(const xAOD::IParticle* part,
                                                             const xAOD::PhotonContainer* photons,
                                                             const xAOD::ElectronContainer* electrons) = 0;

      virtual std::vector<FsrCandidate>* getFarFsrCandidateList(const xAOD::IParticle* part,
                                                                const xAOD::PhotonContainer* photons ) = 0;

      virtual std::vector<FsrCandidate>* getNearFsrCandidateList(const xAOD::Muon* part,
                                                                const xAOD::PhotonContainer* photons,
                                                                const xAOD::ElectronContainer* electrons ) = 0;

   }; // class IFsrPhotonTool

} // namespace FSR

#endif // FSRUTILS_IFsrPhotonTool_H

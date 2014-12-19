// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonCnvAlg.h 297747 2013-10-28 15:14:24Z krasznaa $
#ifndef XAODCREATORALGS_MUONCREATOR_H
#define XAODCREATORALGS_MUONCREATOR_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
// xAOD include
#include "xAODPrimitives/IsolationType.h"
#include "xAODMuon/MuonContainer.h"
#include "muonEvent/MuonParamDefs.h"
#include "Particle/TrackParticleContainer.h"
#include "xAODMuonCnv/IMuonDressingTool.h"

namespace Analysis {
  class Muon;
}


namespace xAODMaker {

   /**
    *  @short Algorithm creating xAOD::Muons from Muons
    *
    *         This algorithm can be used to translate the Muons coming
    *         from an AOD, and create xAOD::Muon objects out of them
    *         for an output xAOD.
    *
    * @author Edward Moyse <Edward.Moyse@cern.ch>
    *
    */
   class MuonCnvAlg : public AthAlgorithm {

   public:
      /// Regular algorithm constructor
      MuonCnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// The key of the input CaloClusterContainer
      std::string m_aodContainerName;
      /// The key for the output xAOD::CaloClusterContainer
      std::string m_xaodContainerName;

      void setParameters(const Analysis::Muon& aodmuon, xAOD::Muon& xaodmuon);
      void checkAndSetParameter(MuonParameters::ParamDef aodParameter,xAOD::Muon::ParamDef xaodParameter, const Analysis::Muon& aodmuon, xAOD::Muon& xaodmuon);
      void setIsolations(const Analysis::Muon& aodmuon, xAOD::Muon& xaodmuon);
      void checkAndSetIsolation(MuonParameters::ParamDef aodParameter,xAOD::Iso::IsolationType xaodParameter, const Analysis::Muon& aodmuon, xAOD::Muon& xaodmuon);
      void setLinks(const Analysis::Muon& aodmuon, xAOD::Muon& xaodmuon) const;
      void setMuonHitContent(const Analysis::Muon& aodmuon, xAOD::Muon& xaodmuon);
      ElementLink<xAOD::TrackParticleContainer> getNewLink(const ElementLink<Rec::TrackParticleContainer>& oldLink, const std::string& name) const;
      xAOD::Muon::Author convertAuthor(MuonParameters::Author aodAuthor); // conversion of author (release 17 to release 19) 
      
      std::string   m_inDetTrackParticles; //!< Location/Key for xAOD::TrackParticles from ID. 
      std::string   m_saTrackParticles ;   //!< Location/Key for MS SA (extrap to beamline) xAOD::TrackParticles. 
      std::string   m_cbTrackParticles ;   //!< Location/Key for combined fit xAOD::TrackParticles.
      std::string   m_mgTrackParticles ;   //!< Location/Key for MuGirl combined fit xAOD::TrackParticles; needed in conversion from release 17
      // bool m_doConversionFromRel17; un-initilized variable that is actually never used, commenting out - coverity defect 28732

      ToolHandle<xAOD::IMuonDressingTool>  m_muonDressingTool;
   }; // class MuonCnvAlg

} // namespace xAODMaker

#endif // XAODCREATORALGS_MUONCREATOR_H

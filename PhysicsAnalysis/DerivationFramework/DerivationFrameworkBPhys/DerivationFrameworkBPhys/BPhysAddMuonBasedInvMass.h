/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BPhysAddMuonBasedInvMass.h, (c) ATLAS Detector software
//============================================================================
// 
// Author : Wolfgang Walkwiaak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Add muon-information based invarient mass to secondary vertices using
// a four vector sum.
//
// For an example see BPHY8.py .
//
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_BPhysAddMuonBasedInvMass_H
#define DERIVATIONFRAMEWORK_BPhysAddMuonBasedInvMass_H

#include <string>
#include <vector>
#include <set>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODBPhys/BPhysHelper.h"
#include "EventPrimitives/EventPrimitives.h"
#include "ITrackToVertex/ITrackToVertex.h"

namespace DerivationFramework {
  //
  // typedefs -- to abbreviate long lines
  //
  typedef std::vector<const xAOD::TrackParticle*> TrackBag;
  typedef std::vector<const xAOD::Muon*>          MuonBag;

  class BPhysAddMuonBasedInvMass : virtual public AthAlgTool,
    virtual public IAugmentationTool {

  public: 
      BPhysAddMuonBasedInvMass(const std::string& t, const std::string& n,
			       const IInterface* p);

      virtual StatusCode initialize();
      virtual StatusCode finalize();
      
      virtual StatusCode addBranches() const;

  protected:
      std::pair<double, double> getMuCalcMass(xAOD::BPhysHelper& vtx,
					      std::vector<double>
					      trkMasses,
					      int nMuRequested) const;
      std::pair<TrackBag, int> getTracksWithMuons(xAOD::BPhysHelper& vtx) const;
      std::pair<double,double>
	getInvariantMassWithError(TrackBag trksIn,
				  std::vector<double> massHypotheses,
				  const Amg::Vector3D& pos) const;
      AmgSymMatrix(3) getMomentumCov(const xAOD::TrackParticle* track) const;
      AmgSymMatrix(3) getMomentumCov(const Trk::Perigee* perigee) const;
      AmgSymMatrix(3) getMomentumCov(const AmgVector(5)& pars,
				     const AmgSymMatrix(5)& cov) const;
      MuonBag findAllMuonsInDecay(xAOD::BPhysHelper& vtx) const;

  private:      
      // job options
      std::string                      m_branchPrefix;
      std::string                      m_vertexContainerName;
      std::string                      m_trackParticleContainerName;

      std::string                      m_muonContainerName;
      std::vector<double>              m_trkMasses;
      ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
      
  }; // class
} // namespace

#endif // DERIVATIONFRAMEWORK_BPhysAddMuonBasedInvMass_H

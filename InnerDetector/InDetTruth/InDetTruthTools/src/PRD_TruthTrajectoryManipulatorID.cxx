/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////
// PRD_TruthTrajectoryManipulatorID.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetTruthTools/PRD_TruthTrajectoryManipulatorID.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetPrepRawData/PixelCluster.h"
// DetectorDescription
#include "AtlasDetDescr/AtlasDetectorID.h"
// HepMC
#include "HepMC/GenParticle.h"
// CLHEP
#include "CLHEP/Random/RandFlat.h"

InDet::PRD_TruthTrajectoryManipulatorID::PRD_TruthTrajectoryManipulatorID(const std::string& t, const std::string& n, const IInterface* p) :
AthAlgTool(t,n,p),
m_randSvc("AtDSFMTGenSvc", n),
m_randEngineName("PRD_TruthTrajectoryManipulatorID"),
m_randEngine(nullptr)
{
    declareInterface<Trk::IPRD_TruthTrajectoryManipulator>(this);

    declareProperty("RandomNumberService", m_randSvc, "ATLAS Random number AthService");
    declareProperty("RandomStreamName", m_randEngineName, "Name of the random number stream");
}

StatusCode InDet::PRD_TruthTrajectoryManipulatorID::initialize() {
  ATH_MSG_VERBOSE("Initializing ...");
  StatusCode sc = detStore()->retrieve(m_atlasId, "AtlasID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get AtlasID helper !" << endreq;
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_randSvc.retrieve());

  // Get own engine with own seeds
  m_randEngine = m_randSvc->GetEngine(m_randEngineName);
  if (!m_randEngine) {
      ATH_MSG_ERROR("Failed to retrieve RandomEngine instance");
      return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode InDet::PRD_TruthTrajectoryManipulatorID::finalize() {
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}

bool InDet::PRD_TruthTrajectoryManipulatorID::manipulateTruthTrajectory( Trk::PRD_TruthTrajectory &prdvec) const {

  if( (*prdvec.genParticle).barcode() < 100000){
      const int pdg_id = (*prdvec.genParticle).pdg_id();
      const double prob_pix = pdg_id == 2212 ? 0.04 : 0.;
      const double prob_sct = 0.04375;
      std::vector<const Trk::PrepRawData* >::iterator prdIter  = prdvec.prds.begin();
      std::vector<const Trk::PrepRawData* >::iterator prdIterE = prdvec.prds.end();

      while( prdIter != prdIterE ){
            if( m_atlasId->is_pixel((*prdIter)->identify()) ){
                if( prob_pix > 0. && CLHEP::RandFlat::shoot(m_randEngine) <= prob_pix ){
                  prdIter = prdvec.prds.erase(prdIter);
                  prdIterE  = prdvec.prds.end();
                }
                else ++ prdIter;
            }
            else if( m_atlasId->is_sct((*prdIter)->identify()) ){
                double rand = CLHEP::RandFlat::shoot(m_randEngine);
                if( rand <= prob_sct ){
                  prdIter = prdvec.prds.erase(prdIter);
                  prdIterE  = prdvec.prds.end();
                }
                else ++ prdIter;
            }
            else ++ prdIter;
      }
  }

  return true;
}

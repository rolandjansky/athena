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
#include "InDetIdentifier/PixelID.h"
// HepMC
#include "HepMC/GenParticle.h"
// CLHEP
#include "CLHEP/Random/RandFlat.h"

InDet::PRD_TruthTrajectoryManipulatorID::PRD_TruthTrajectoryManipulatorID(const std::string& t, const std::string& n, const IInterface* p) :
AthAlgTool(t,n,p),
m_randSvc("AtDSFMTGenSvc", n),
m_randEngineName("PRD_TruthTrajectoryManipulatorID"),
m_randEngine(nullptr),
m_pixelIblEff(0.995),
m_pixelEff(0.98),
m_sctEff(0.98),
m_trtEff(0.99)
{
    declareInterface<Trk::IPRD_TruthTrajectoryManipulator>(this);

    declareProperty("PixelIBLEfficiency",m_pixelIblEff);
    declareProperty("PixelEfficiency",m_pixelEff);
    declareProperty("SCT_Efficiency",m_sctEff);
    declareProperty("TRT_Efficiency",m_trtEff);
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

  sc = detStore()->retrieve(m_pixelId, "PixelID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get PixelID helper !" << endreq;
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

      std::vector<const Trk::PrepRawData* >::iterator prdIter  = prdvec.prds.begin();
      std::vector<const Trk::PrepRawData* >::iterator prdIterE = prdvec.prds.end();

      while( prdIter != prdIterE ){
            if( m_atlasId->is_pixel((*prdIter)->identify()) ){
              double prob = m_pixelEff;
	      if( m_pixelId->is_barrel((*prdIter)->identify()) && 
                  m_pixelId->layer_disk((*prdIter)->identify()) == 0) prob=m_pixelIblEff;
              if(CLHEP::RandFlat::shoot(m_randEngine) > prob ){
                prdIter = prdvec.prds.erase(prdIter);
                prdIterE  = prdvec.prds.end();
              }
              else ++ prdIter;
            }
            else if( m_atlasId->is_sct((*prdIter)->identify()) ){
                double rand = CLHEP::RandFlat::shoot(m_randEngine);
                if( rand > m_sctEff ){
                  prdIter = prdvec.prds.erase(prdIter);
                  prdIterE  = prdvec.prds.end();
                }
                else ++ prdIter;
            }
            else if( m_atlasId->is_trt((*prdIter)->identify()) ){
                double rand = CLHEP::RandFlat::shoot(m_randEngine);
                if( rand > m_trtEff ){
                  prdIter = prdvec.prds.erase(prdIter);
                  prdIterE  = prdvec.prds.end();
                }
                else ++ prdIter;
            }
            else ++ prdIter;
      }
  

  return true;
}

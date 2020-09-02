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
#include "AtlasHepMC/GenParticle.h"

InDet::PRD_TruthTrajectoryManipulatorID::PRD_TruthTrajectoryManipulatorID(const std::string& t, const std::string& n, const IInterface* p) :
AthAlgTool(t,n,p)
{
    declareInterface<Trk::IPRD_TruthTrajectoryManipulator>(this);
}

StatusCode InDet::PRD_TruthTrajectoryManipulatorID::initialize() {
  ATH_MSG_VERBOSE("Initializing ...");
  StatusCode sc = detStore()->retrieve(m_atlasId, "AtlasID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get AtlasID helper !" << endmsg;
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
      srand( static_cast< unsigned int >( time( 0 ) ) );
      const int pdg_id = (*prdvec.genParticle).pdg_id();
      const double prob_pix = pdg_id == 2212 ? 4. : 0;
      const double prob_sct = 4.375;
      std::vector<const Trk::PrepRawData* >::iterator prdIter  = prdvec.prds.begin();
      std::vector<const Trk::PrepRawData* >::iterator prdIterE = prdvec.prds.end();

      while( prdIter != prdIterE ){
            if( m_atlasId->is_pixel((*prdIter)->identify()) ){
                if( prob_pix > 0 ? rand()%100 <= prob_pix : false ){
                  prdIter = prdvec.prds.erase(prdIter);
                  prdIterE  = prdvec.prds.end();
                }
                else ++ prdIter;
            }
            else if( m_atlasId->is_sct((*prdIter)->identify()) ){
                if( rand()%100 <= prob_sct ){
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

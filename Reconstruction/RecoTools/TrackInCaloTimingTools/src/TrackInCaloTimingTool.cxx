/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackInCaloTimingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrackInCaloTimingTool.h"
#include "CaloEvent/CaloCell.h"

//================ Constructor =================================================

Rec::TrackInCaloTimingTool::TrackInCaloTimingTool(const std::string& t,
                                                  const std::string& n,
                                                  const IInterface*  p ):
  AthAlgTool(t,n,p), m_trackInCaloTool("TrackInCaloTools", this) {

  declareInterface<ITrackInCaloTimingTool>(this);
  declareInterface<Trk::ITrackTimingTool>(this);
  
  // retrieve properties from job options
  declareProperty("TrackInCaloTool", m_trackInCaloTool);
  declareProperty("MinCellEnergy", m_minCellEnergy = 300); // if > 0, this is used as a cut for what cells to consider
  declareProperty("doEtaAndPhiCompensation", m_doEtaAndPhiCompensation = false); // needs MC study now, high-stats muon data sample later
  declareProperty("doBadCellCleaning", m_doBadCellCleaning = false); // needs some db access implemented
  int minSampling = 0, maxSampling = 0; // seems declareProperty can't handle CaloCell_ID::CaloSample..
  declareProperty("MinSampling", minSampling = CaloCell_ID::TileBar0);
  declareProperty("MaxSampling", maxSampling = CaloCell_ID::TileExt2);
  m_minSampling = (CaloCell_ID::CaloSample) minSampling; m_maxSampling = (CaloCell_ID::CaloSample) maxSampling;
}

//================ Destructor ==================================================

Rec::TrackInCaloTimingTool::~TrackInCaloTimingTool() {}


//================ Initialisation ==============================================

StatusCode Rec::TrackInCaloTimingTool::initialize() {
  
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;
  
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Rec::TrackInCaloTimingTool::finalize() {
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//================ Calo ToF functions ===========================================

std::vector<Trk::SpaceTimePoint*> Rec::TrackInCaloTimingTool::timeMeasurements(const Trk::Track& trk, const Trk::SegmentCollection* /* unused */) const {
  ATH_MSG_DEBUG("In Rec::TrackInCaloTimingTool::timeMeasurements()");
  
  int currentTrackCaloCells_N = 0;
  float currentTrackCaloCells_totalE = 0;
  
  ATH_MSG_DEBUG("Will now retrieve the calo cells crossed by the current track");  
  
  std::vector<const CaloCell*> cellsOnTrack = m_trackInCaloTool->getCellsCrossedByTrack(&trk, TICT::ps, TICT::hec3, false);
  
  ATH_MSG_DEBUG(cellsOnTrack.size() << " cell(s) along the extrapolated track were found");
  
  std::vector<Trk::SpaceTimePoint*> stp;
  
  // Loop over the crossed cells and create SpaceTimePoint objects
  if (cellsOnTrack.size()) {
    for (std::vector<const CaloCell*>::const_iterator cellItr = cellsOnTrack.begin(); 
         cellItr != cellsOnTrack.end(); ++cellItr) {
      // ignore cells from unwanted samplings
      CaloCell_ID::CaloSample sampling = CaloCell_ID::Unknown;
      const CaloDetDescrElement *caloDDE = (*cellItr)->caloDDE();
      if (caloDDE) {
         sampling = caloDDE->getSampling();
      }
      ATH_MSG_DEBUG("CaloCell: sampling: " << sampling << ", (eta,phi) = (" << (*cellItr)->eta() << "," << (*cellItr)->phi() 
                   << "), (x,y,z) = (" << (*cellItr)->x() << "," << (*cellItr)->y() << "," << (*cellItr)->z() << ")"
                   << ", E=" << (*cellItr)->e() << ", t=" << (*cellItr)->time());
      // if cell has too low energy, go on to the next one
      if ((*cellItr)->e() < m_minCellEnergy) {
        ATH_MSG_DEBUG("Skipping cell - energy too low (" << (*cellItr)->e() << " MeV)");
        continue;
      }
      if (sampling < m_minSampling || sampling > m_maxSampling) {
	ATH_MSG_DEBUG("Skipping cell with sampling = " << sampling);
        continue;
      }

      ATH_MSG_DEBUG("\tCaloCell passed the cell energy cut (" << (*cellItr)->e() << " > " << m_minCellEnergy 
                    << " MeV) and sampling cut (" << m_minSampling << " <= " << sampling << " <= " << m_maxSampling << ")");

      // use parameterized cell timing resolution to get estimated error and weight
      float weight = 0;
      float error = 0;
      Trk::TrackState::MeasurementType id = Trk::TrackState::unidentified;

      // for Tile:
      if (sampling >= CaloCell_ID::TileBar0 && sampling <= CaloCell_ID::TileExt2) {
        error = tileCellTimeError((*cellItr)->e());
        id = Trk::TrackState::TileCal;
        ATH_MSG_DEBUG("Tile cell: estimated error on cell time to be " << error << " ns");
      }
      // for LAr
      else { // TODO: get the timing resolution functions for the other samplings!
        error = 3.0/((*cellItr)->e()/1000.0); // only for now!
        id = Trk::TrackState::LArCal;
      }
      weight = cellTimeWeight(error);
      stp.push_back(new Trk::SpaceTimePoint(Amg::Vector3D((*cellItr)->x(), (*cellItr)->y(), (*cellItr)->z()), (*cellItr)->time(), error, weight, id));
      ++currentTrackCaloCells_N;
      currentTrackCaloCells_totalE += (*cellItr)->e();
    }
  }
  ATH_MSG_DEBUG("TrackInCaloTimingTool processed " << currentTrackCaloCells_N
               << " CaloCells with a total of " << currentTrackCaloCells_totalE/1000 
               << " GeV for the current track");

  return stp;
}

float Rec::TrackInCaloTimingTool::calorimeterBeta(const Trk::Track& trk) const {
  // TODO: should be refined! For now, uses cell-energy weighted mean and 
  ATH_MSG_DEBUG("In Rec::TrackInCaloTimingTool::timeMeasurements()");

  std::vector<Trk::SpaceTimePoint*> cellStpVector = timeMeasurements(trk);

  // weight the individual beta measurements together by their weights
  float weightedBetaSum = 0, weightSum = 0, beta = -1;
  for (std::vector<Trk::SpaceTimePoint*>::const_iterator stpIt = cellStpVector.begin(); 
       stpIt != cellStpVector.end(); ++stpIt) {
    float dist = sqrt(pow((*stpIt)->globalPosition().x(),2)+pow((*stpIt)->globalPosition().y(),2)+pow((*stpIt)->globalPosition().z(),2));
    float beta = dist/((*stpIt)->time()*2.99792458e2+dist);
    ATH_MSG_DEBUG("..calculating beta, this cell: beta = " << beta << ", weight = " << (*stpIt)->weight() 
                  << " (cumulative: weighted beta = " << weightedBetaSum << ", weight sum = " << weightSum << ")");
    weightedBetaSum += (*stpIt)->weight()*beta;
    weightSum += (*stpIt)->weight();
  }

  if (weightSum)  
    beta = weightedBetaSum/weightSum;

  return beta;
}

// The same functions but for TrackParticles
std::vector<Trk::SpaceTimePoint*> Rec::TrackInCaloTimingTool::timeMeasurements(const Trk::TrackParticleBase& tpb) const {
  return timeMeasurements(*tpb.originalTrack());
}

// a mock implementation for the segments, must be implemented for gaudi factories to work => returns empty vector
std::vector<Trk::SpaceTimePoint*> Rec::TrackInCaloTimingTool::timeMeasurements(std::vector<const Trk::Segment*> /*segments*/) const {
  std::vector<Trk::SpaceTimePoint*> v;
  return v;
}

float Rec::TrackInCaloTimingTool::calorimeterBeta(const Trk::TrackParticleBase& tpb) const {
  return calorimeterBeta(*tpb.originalTrack());
}

float Rec::TrackInCaloTimingTool::tileCellTimeError(float cellEnergy) const {
  float p0 = 0, p1 = 0, p2 = 0;
  cellEnergy = cellEnergy/1000; // MeV => GeV (used in resolution function)
  if (cellEnergy < 20) { // high gain
    p0 = 0.61;
    p1 = 1.1;
    p2 = 1.5;
  }
  else { // low gain
    p0 = 0;
    p1 = 2.5;      
    p2 = 14;
  }
  return sqrt(pow(p0,2) + pow(p1,2)/cellEnergy + pow(p2/cellEnergy,2));
}
    
float Rec::TrackInCaloTimingTool::cellTimeWeight(float error) const {
  return 1.0/pow(error,2);
}


//===============================================================================

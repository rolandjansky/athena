/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigFTKTrackBuilder tool
// -------------------------------
// ATLAS Collaboration
//
// 13.06.2013 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <map>
#include <vector>

#include "GaudiKernel/IPartPropSvc.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepPDT/ParticleDataTable.hh"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"

#include "TrigFTKFastSimTruth/TrigFTKTrackBuilderTool.h"
#include "TrigFTKFastSimTruth/FTKTruthTrajectory.h"

#include "TrigFTKSim/FTKHit.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKRawHit.h"
#include "TrigFTKSim/FTKTrack.h"


#include "TrkParameters/TrackParameters.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/LocalParameters.h"


using Gaudi::Units::micrometer;

// local namespace
namespace {
  const TString tp_name[] = {"Ipt","eta","phi","d0","z0"};
  enum tp_index { tp_Ipt, tp_eta, tp_phi, tp_d0, tp_z0 };
}

TrigFTKTrackBuilderTool::TrigFTKTrackBuilderTool(const std::string& type, const std::string& name, const IInterface*  parent)
: base_class(type, name, parent),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_beamSpotSvc("BeamCondSvc", name),
  m_rndmSvc("AtRndmGenSvc", name),
  m_randomStreamName("TrigFTKFastSimTruthRandom"),
  m_smearingFilePath("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/FTK/FastSim/Truth"),
  m_smearingFileName("truth_smearing_param_gauss2_v2.root")
{
  declareProperty("PMapPath", m_pmap_path, "Path to the (12L) plane map");

  declareProperty("RndmSvc", m_rndmSvc);
  declareProperty("RndmStream", m_randomStreamName);
  
  declareProperty("smearingRootFilePath", m_smearingFilePath);
  declareProperty("smearingRootFileName", m_smearingFileName);
}

StatusCode TrigFTKTrackBuilderTool::initialize()
{
  if (m_pmap_path.empty()) {
    ATH_MSG_FATAL("Main 12L plane map definition missing");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Loading 12L plane map from " << m_pmap_path);
  m_pmap.reset(new FTKPlaneMap(m_pmap_path.c_str()));
  if (!m_pmap->getIsOK()) {
      ATH_MSG_FATAL("Error using plane map: " << m_pmap_path);
      return StatusCode::FAILURE;
  }

  m_nplanes = m_pmap->getNPlanes();
  m_ncoords = m_pmap->getTotalDim();

  IPartPropSvc* partPropSvc = nullptr;
  ATH_CHECK(service("PartPropSvc", partPropSvc, true));
  m_particleDataTable = partPropSvc->PDT();

  ATH_CHECK(m_extrapolator.retrieve());
  ATH_CHECK(m_beamSpotSvc.retrieve());

  ATH_CHECK(m_rndmSvc.retrieve());
  m_randomEngine = m_rndmSvc->GetEngine(m_randomStreamName);
  if (!m_randomEngine) {
    ATH_MSG_FATAL("Failed to get random engine: " << m_randomStreamName);
    return StatusCode::FAILURE;
  }

  TString filename;
  if( m_smearingFilePath.back() == '/' ) {
    filename = m_smearingFilePath + m_smearingFileName;
  } else {
    filename = m_smearingFilePath + "/" + m_smearingFileName;
  }
  TFile *smearing = new TFile(filename);
  if (smearing==nullptr) {
    ATH_MSG_FATAL("Failed to open smearing parameter root file: " << filename);
  } else {
    ATH_MSG_INFO("Using smearing parameter root file: " << filename);
  }
  
  TTreeReader reader("truth_smearing_param",smearing);
  std::unique_ptr< TTreeReaderArray<double> > tp_reader[FTKSmearingConstants::ntp];
  
  for( int itp=0; itp<FTKSmearingConstants::ntp; itp++ ) {
    tp_reader[itp].reset( new TTreeReaderArray<double>(reader,tp_name[itp]) );
  }
  reader.Next();
  
  for( int itp=0; itp<FTKSmearingConstants::ntp; itp++ ) {
    int iRegion = 0;
    for( int iIBL=0; iIBL<FTKSmearingConstants::nIBL; iIBL++ ) {
      for( int ieta=0; ieta<FTKSmearingConstants::neta; ieta++ ) {
        m_smearing_parameters[iIBL][ieta][itp].n1 = (*tp_reader[itp])[iRegion+0];
        m_smearing_parameters[iIBL][ieta][itp].m1 = (*tp_reader[itp])[iRegion+1];
        m_smearing_parameters[iIBL][ieta][itp].a1 = (*tp_reader[itp])[iRegion+2];
        m_smearing_parameters[iIBL][ieta][itp].b1 = (*tp_reader[itp])[iRegion+3];
        m_smearing_parameters[iIBL][ieta][itp].n2 = (*tp_reader[itp])[iRegion+4];
        m_smearing_parameters[iIBL][ieta][itp].m2 = (*tp_reader[itp])[iRegion+5];
        m_smearing_parameters[iIBL][ieta][itp].a2 = (*tp_reader[itp])[iRegion+6];
        m_smearing_parameters[iIBL][ieta][itp].b2 = (*tp_reader[itp])[iRegion+7];

        //ATH_MSG_DEBUG("itp: " << itp << ", iIBL: " << iIBL << ", ieta: " << ieta);
        iRegion+=FTKSmearingConstants::nparam;
      }
    }
  }

  delete smearing;

  return StatusCode::SUCCESS;
}

bool TrigFTKTrackBuilderTool::smearTrack(bool hasIBLhit, FTKTrack *track, const HepMC::GenParticle* particle) const
{

  const int pdgcode = particle->pdg_id();
  // reject neutral or unstable particles
  const HepPDT::ParticleData* pd = m_particleDataTable->particle(abs(pdgcode));
  if( !pd ) { ATH_MSG_ERROR("Invalid particle data"); return false; }
  float charge = pd->charge();
  if (pdgcode < 0) charge *= -1.; // since we took absolute value above

  const Amg::Vector3D momentum(particle->momentum().px(), particle->momentum().py(), particle->momentum().pz());
  if(!particle->production_vertex()){
    ATH_MSG_ERROR("production vertex is null");
    return false;
  }
  HepMC::ThreeVector pos_vtx = particle->production_vertex()->point3d();

  const Amg::Vector3D position(pos_vtx.x(), pos_vtx.y(), pos_vtx.z());

  const Trk::CurvilinearParameters cParameters(position, momentum, charge);
  Trk::PerigeeSurface persf( m_beamSpotSvc->beamPos() );
  const Trk::TrackParameters* tP = m_extrapolator->extrapolate(cParameters,persf, Trk::anyDirection, false);

  double truth_pT = particle->momentum().perp();
  double truth_eta = particle->momentum().pseudoRapidity();
  double truth_ctheta = 1./std::tan(particle->momentum().theta());
  double truth_qOver2Pt = charge/(2*truth_pT);

  double truth_phi = 999.;
  double truth_d0  = 999.;
  double truth_z0  = 999.;
  if(tP){
    truth_phi = tP->parameters()[Trk::phi];
    truth_d0  = tP->parameters()[Trk::d0];
    truth_z0  = tP->parameters()[Trk::z0];
  }
  else{
    ATH_MSG_WARNING("Could not get track parameter for truth.truth phi,d0,z0=999");
    return false;
  }

  int eta_bin = 999;

  if (std::abs(truth_eta)>=0.0 and std::abs(truth_eta)<0.5) { eta_bin = 0; }
  else if (std::abs(truth_eta)>=0.5 and std::abs(truth_eta)<1.0) { eta_bin = 1; }
  else if (std::abs(truth_eta)>=1.0 and std::abs(truth_eta)<1.5) { eta_bin = 2; }
  else if (std::abs(truth_eta)>=1.5 and std::abs(truth_eta)<2.0) { eta_bin = 3; }
  else if (std::abs(truth_eta)>=2.0 and std::abs(truth_eta)<2.5) { eta_bin = 4; }
  else { eta_bin = 5; }

  //std::cout << "This is eta: " << truth_eta << std::endl;
  //std::cout << "This is the eta bin: " << eta_bin << std::endl;

  // a bit of trigonometry to propagate the sigma on ctheta
  double theta = 2. * std::atan(std::exp(-1.0*truth_eta));
  double factor = (2. * std::exp(truth_eta)) / (std::exp(2.*truth_eta)+1);
  double sigma_ctheta = factor / std::pow(std::sin(theta),2);
  // sigma_ctheta *= sigma_eta;

  double smeared_tp[FTKSmearingConstants::ntp];
  for( int itp=0; itp<FTKSmearingConstants::ntp; itp++ ) {
    double random_frac = m_randomEngine->flat();
    dgparam par = m_smearing_parameters[hasIBLhit][eta_bin][itp];
    double core_ratio = par.n1 / (par.n1+par.n2);
    
    double sigma = 1.;
    if( itp == tp_eta ) sigma = sigma_ctheta;
    
    if( random_frac < core_ratio ) { // use core gaussian
      sigma *= std::sqrt( par.a1 + par.b1*std::pow(truth_qOver2Pt,2) );
      smeared_tp[itp] = CLHEP::RandGauss::shoot(m_randomEngine,par.m1,sigma);
    } else { // use tail gaussian
      sigma *= std::sqrt( par.a2 + par.b2*std::pow(truth_qOver2Pt,2) );
      smeared_tp[itp] = CLHEP::RandGauss::shoot(m_randomEngine,par.m2,sigma);
    }
  }

  track->setHalfInvPt((charge * 0.5/(truth_pT)) + smeared_tp[tp_Ipt]);
  track->setCotTheta(truth_ctheta + smeared_tp[tp_eta]); // tp_eta is the correct index for ctheta
  track->setPhi(truth_phi + smeared_tp[tp_phi]);
  track->setIP(truth_d0 + smeared_tp[tp_d0]);
  track->setZ0(truth_z0 + smeared_tp[tp_z0]);

  return true;
}


std::unique_ptr<FTKTrack> TrigFTKTrackBuilderTool::createFTKTrackForSector(
  const FTKTruthTrajectory& traj,
  const std::unordered_map<int, int>& planeHits) const
{
  // Create an FTK track and fill all hits that
  // belong to the sector. The planeHits give the mapping
  // of hit index to plane in this sector.
  auto track = std::make_unique<FTKTrack>(m_ncoords, m_nplanes);

  // Store number of missing hits and a bitmask of assigned
  // hits.
  int nmissing = m_nplanes;
  unsigned int bitmask = 0;

  for (size_t ihit = 0; ihit < traj.hits.size(); ++ihit) {
    const auto& hit = traj.hits[ihit];

    // If no entryis present for a hit, it does not belong to the sector.
    if (planeHits.find(ihit) == planeHits.end())
      continue;

    // Hit belongs to this sector, now store the hit coordinates
    // and the FTKHit object into the track.
    const auto plane = planeHits.at(ihit);

    FTKHit ftkhit = hit->getFTKHit(m_pmap.get());
    // Keep truth information
    assert(hit->getTruth());
    ftkhit.setTruth(*hit->getTruth());

    // Set track hit information
    if (hit->getIsPixel()) {
      const int ix = m_pmap->getDim(plane, 0);
      const int iy = m_pmap->getDim(plane, 1);

      bitmask |= ( (1<<ix) | (1<<iy));
      nmissing -= 2;

      track->setCoord(ix, ftkhit[0]);
      track->setCoord(iy, ftkhit[1]);
      track->setFTKHit(plane, ftkhit);
    }
    else if (hit->getIsSCT()) {
      const int ix = m_pmap->getDim(plane, 0);

      bitmask |= 1 << ix;
      nmissing -= 1;

      track->setCoord(ix, ftkhit[0]);
      track->setFTKHit(plane, ftkhit);
    }
  }

  track->setNMissing(nmissing);
  track->setBitmask(bitmask);

  return track;
}

std::unique_ptr<FTKTrack> TrigFTKTrackBuilderTool::createFTKTrack(const FTKTruthTrajectory& traj,
    const std::vector<std::unordered_map<int, int> >& sectorHitToPlaneMap,
    const std::vector<int>& regions,
    const std::vector<int>& sectors) const
{
  std::unique_ptr<FTKTrack> bestTrack;

  // Loop through all sectors that the track fits in and select the
  // best track (the one with most hits) from all candidates.
  for (size_t sectorIndex = 0; sectorIndex < sectorHitToPlaneMap.size(); ++sectorIndex) {
    auto track = createFTKTrackForSector(traj, sectorHitToPlaneMap[sectorIndex]);

    track->setBankID(regions.at(sectorIndex));
    track->setSectorID(sectors.at(sectorIndex));

    const bool hasIBLhit = track->getFTKHit(0).getDim() > 0;
    smearTrack(hasIBLhit, track.get(), traj.genParticle);

    if (!bestTrack || (track->getNMissing() < bestTrack->getNMissing())) {
      bestTrack.reset(track.release());
    }
  }

  return bestTrack;
}

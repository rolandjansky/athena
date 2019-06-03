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

TrigFTKTrackBuilderTool::TrigFTKTrackBuilderTool(const std::string& type, const std::string& name, const IInterface*  parent)
: base_class(type, name, parent),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_beamSpotSvc("BeamCondSvc", name),
  m_rndmSvc("AtRndmGenSvc", name),
  m_randomStreamName("TrigFTKFastSimTruthRandom")
{
  declareProperty("PMapPath", m_pmap_path, "Path to the (12L) plane map");

  declareProperty("RndmSvc", m_rndmSvc);
  declareProperty("RndmStream", m_randomStreamName);
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

  return StatusCode::SUCCESS;
}

bool TrigFTKTrackBuilderTool::smearTrack(bool hasIBLhit, FTKTrack *track, const HepMC::GenParticle* particle) const
{

  bool linpar = false;

  // -----------------------
  // linear parameterisation
  // -----------------------

  // with IBL hit
  std::map<int,std::vector<double>> withIBL_lin_d0;
  withIBL_lin_d0[0] = {2.165000e-02,  9.746521e+01} ;
  withIBL_lin_d0[1] = {2.101431e-02,  1.160074e+02} ;
  withIBL_lin_d0[2] = {2.394603e-02,  1.361484e+02} ;
  withIBL_lin_d0[3] = {2.809658e-02,  1.742900e+02} ;
  withIBL_lin_d0[4] = {3.455036e-02,  2.320326e+02} ;
  withIBL_lin_d0[5] = {3.455036e-02,  2.320326e+02} ;

  std::map<int,std::vector<double>> withIBL_lin_z0;
  withIBL_lin_z0[0] = {8.291734e-02,  1.155988e+02 };
  withIBL_lin_z0[1] = {7.341500e-02,  1.515614e+02 };
  withIBL_lin_z0[2] = {1.054898e-01,  2.652691e+02 };
  withIBL_lin_z0[3] = {1.516307e-01,  4.907506e+02 };
  withIBL_lin_z0[4] = {2.813610e-01,  9.213986e+02 };
  withIBL_lin_z0[5] = {2.813610e-01,  9.213986e+02 };

  std::map<int,std::vector<double>> withIBL_lin_eta;
  withIBL_lin_eta[0] = {9.444928e-04,  3.986158e+00 };
  withIBL_lin_eta[1] = {7.551402e-04,  4.239754e+00 };
  withIBL_lin_eta[2] = {7.807419e-04,  4.982420e+00 };
  withIBL_lin_eta[3] = {8.810903e-04,  5.835897e+00 };
  withIBL_lin_eta[4] = {1.167114e-03,  6.637692e+00 };
  withIBL_lin_eta[5] = {1.167114e-03,  6.637692e+00 };

  std::map<int,std::vector<double>> withIBL_lin_phi;
  withIBL_lin_phi[0] = {4.638598e-04,  3.417675e+00 };
  withIBL_lin_phi[1] = {4.750186e-04,  3.876165e+00 };
  withIBL_lin_phi[2] = {5.519338e-04,  4.640639e+00 };
  withIBL_lin_phi[3] = {7.017347e-04,  5.796550e+00 };
  withIBL_lin_phi[4] = {8.980032e-04,  7.489104e+00 };
  withIBL_lin_phi[5] = {8.980032e-04,  7.489104e+00 };

  std::map<int,std::vector<double>> withIBL_lin_hlfIpt;
  withIBL_lin_hlfIpt[0] = {1.903324e-06,  1.633974e-02 };
  withIBL_lin_hlfIpt[1] = {1.861172e-06,  1.947333e-02 };
  withIBL_lin_hlfIpt[2] = {2.418678e-06,  2.525643e-02 };
  withIBL_lin_hlfIpt[3] = {4.411741e-06,  3.037951e-02 };
  withIBL_lin_hlfIpt[4] = {4.854594e-06,  3.993603e-02 };
  withIBL_lin_hlfIpt[5] = {4.854594e-06,  3.993603e-02 };


  // no IBL hit
  std::map<int,std::vector<double>> noIBL_lin_d0;
  noIBL_lin_d0[0] = {2.912790e-02,  1.924919e+02 };
  noIBL_lin_d0[1] = {2.860355e-02,  2.299675e+02 };
  noIBL_lin_d0[2] = {2.798439e-02,  2.922039e+02 };
  noIBL_lin_d0[3] = {3.997899e-02,  3.538939e+02 };
  noIBL_lin_d0[4] = {5.354769e-02,  4.717690e+02 };
  noIBL_lin_d0[5] = {5.354769e-02,  4.717690e+02 };

  std::map<int,std::vector<double>> noIBL_lin_z0;
  noIBL_lin_z0[0] = {1.470882e-01,  2.459152e+02 };
  noIBL_lin_z0[1] = {1.400019e-01,  3.123315e+02 };
  noIBL_lin_z0[2] = {1.559520e-01,  4.842909e+02 };
  noIBL_lin_z0[3] = {2.353992e-01,  9.313151e+02 };
  noIBL_lin_z0[4] = {4.052754e-01,  2.007789e+03 };
  noIBL_lin_z0[5] = {4.052754e-01,  2.007789e+03 };

  std::map<int,std::vector<double>> noIBL_lin_eta;
  noIBL_lin_eta[0] = {1.161208e-03,  4.964795e+00 };
  noIBL_lin_eta[1] = {9.397004e-04,  5.581838e+00 };
  noIBL_lin_eta[2] = {8.315023e-04,  6.417038e+00 };
  noIBL_lin_eta[3] = {8.612335e-04,  8.373559e+00 };
  noIBL_lin_eta[4] = {1.165942e-03,  1.116492e+01 };
  noIBL_lin_eta[5] = {1.165942e-03,  1.116492e+01 };

  std::map<int,std::vector<double>> noIBL_lin_phi;
  noIBL_lin_phi[0] = {3.968176e-04,  5.126463e+00 };
  noIBL_lin_phi[1] = {4.198626e-04,  5.995350e+00 };
  noIBL_lin_phi[2] = {4.472895e-04,  7.388593e+00 };
  noIBL_lin_phi[3] = {7.468634e-04,  8.957967e+00 };
  noIBL_lin_phi[4] = {1.013463e-03,  1.195214e+01 };
  noIBL_lin_phi[5] = {1.013463e-03,  1.195214e+01 };

  std::map<int,std::vector<double>> noIBL_lin_hlfIpt;
  noIBL_lin_hlfIpt[0] = {2.114881e-06,  1.591428e-02 };
  noIBL_lin_hlfIpt[1] = {2.225571e-06,  1.814322e-02 };
  noIBL_lin_hlfIpt[2] = {2.695112e-06,  2.459482e-02 };
  noIBL_lin_hlfIpt[3] = {5.146374e-06,  2.830717e-02 };
  noIBL_lin_hlfIpt[4] = {5.777676e-06,  3.808874e-02 };
  noIBL_lin_hlfIpt[5] = {5.777676e-06,  3.808874e-02 };


  // -----------------------
  // sqrt parameterisation
  // -----------------------

  // with IBL hit
  std::map<int,std::vector<double>> withIBL_sq_d0;
  withIBL_sq_d0[0] = { 6.833476e-04,  2.053167e+04 } ;
  withIBL_sq_d0[1] = { 7.110604e-04,  2.629804e+04 } ;
  withIBL_sq_d0[2] = { 8.887155e-04,  3.635433e+04 } ;
  withIBL_sq_d0[3] = { 1.240019e-03,  5.512366e+04 } ;
  withIBL_sq_d0[4] = { 2.010290e-03,  9.366573e+04 } ;
  withIBL_sq_d0[5] = { 2.010290e-03,  9.366573e+04 } ;

  std::map<int,std::vector<double>> withIBL_sq_z0;
  withIBL_sq_z0[0] = { 8.182191e-03,  5.613992e+04 };
  withIBL_sq_z0[1] = { 6.926827e-03,  7.541361e+04 };
  withIBL_sq_z0[2] = { 1.450779e-02,  2.038478e+05 };
  withIBL_sq_z0[3] = { 3.203846e-02,  5.949688e+05 };
  withIBL_sq_z0[4] = { 1.098017e-01,  2.102301e+06 };
  withIBL_sq_z0[5] = { 1.098017e-01,  2.102301e+06 };

  std::map<int,std::vector<double>> withIBL_sq_eta;
  withIBL_sq_eta[0] = { 1.285953e-06,  3.543054e+01 };
  withIBL_sq_eta[1] = { 8.962818e-07,  3.544991e+01 };
  withIBL_sq_eta[2] = { 1.005838e-06,  4.664460e+01 };
  withIBL_sq_eta[3] = { 1.311960e-06,  6.090104e+01 };
  withIBL_sq_eta[4] = { 2.210244e-06,  8.523981e+01 };
  withIBL_sq_eta[5] = { 2.210244e-06,  8.523981e+01 };

  std::map<int,std::vector<double>> withIBL_sq_phi;
  withIBL_sq_phi[0] = { 3.532779e-07,  2.043296e+01 };
  withIBL_sq_phi[1] = { 3.952656e-07,  2.575445e+01 };
  withIBL_sq_phi[2] = { 5.448243e-07,  3.618809e+01 };
  withIBL_sq_phi[3] = { 8.780914e-07,  5.498637e+01 };
  withIBL_sq_phi[4] = { 1.403608e-06,  9.138036e+01 };
  withIBL_sq_phi[5] = { 1.403608e-06,  9.138036e+01 };

  std::map<int,std::vector<double>> withIBL_sq_hlfIpt;
  withIBL_sq_hlfIpt[0] = { 1.836182e-11,  9.804247e-04 };
  withIBL_sq_hlfIpt[1] = { 6.989922e-12,  5.492308e-04 };
  withIBL_sq_hlfIpt[2] = { 1.038204e-11,  9.896642e-04 };
  withIBL_sq_hlfIpt[3] = { 3.414800e-11,  1.627811e-03 };
  withIBL_sq_hlfIpt[4] = { 4.302132e-11,  2.582682e-03 };
  withIBL_sq_hlfIpt[5] = { 4.302132e-11,  2.582682e-03 };


  // no IBL hit
  std::map<int,std::vector<double>> noIBL_sq_d0;
  noIBL_sq_d0[0] = {1.285215e-03,  6.789728e+04 };
  noIBL_sq_d0[1] = {1.318903e-03,  9.059932e+04 };
  noIBL_sq_d0[2] = {1.592501e-03,  1.304213e+05 };
  noIBL_sq_d0[3] = {2.723310e-03,  2.131978e+05 };
  noIBL_sq_d0[4] = {5.110093e-03,  3.781747e+05 };
  noIBL_sq_d0[5] = {5.110093e-03,  3.781747e+05 };

  std::map<int,std::vector<double>> noIBL_sq_z0;
  noIBL_sq_z0[0] = {2.683949e-02,  2.209501e+05 };
  noIBL_sq_z0[1] = {2.498995e-02,  2.968105e+05 };
  noIBL_sq_z0[2] = {3.306697e-02,  5.964677e+05 };
  noIBL_sq_z0[3] = {7.886040e-02,  2.009275e+06 };
  noIBL_sq_z0[4] = {2.583879e-01,  8.139110e+06 };
  noIBL_sq_z0[5] = {2.583879e-01,  8.139110e+06 };

  std::map<int,std::vector<double>> noIBL_sq_eta;
  noIBL_sq_eta[0] = {1.915813e-06,  5.393240e+01 };
  noIBL_sq_eta[1] = {1.411604e-06,  5.869697e+01 };
  noIBL_sq_eta[2] = {1.164994e-06,  7.193975e+01 };
  noIBL_sq_eta[3] = {1.383405e-06,  1.143532e+02 };
  noIBL_sq_eta[4] = {2.680622e-06,  2.015519e+02 };
  noIBL_sq_eta[5] = {2.680622e-06,  2.015519e+02 };

  std::map<int,std::vector<double>> noIBL_sq_phi;
  noIBL_sq_phi[0] = {3.019947e-07,  3.838097e+01 };
  noIBL_sq_phi[1] = {3.352412e-07,  5.019127e+01 };
  noIBL_sq_phi[2] = {4.389225e-07,  7.557339e+01 };
  noIBL_sq_phi[3] = {1.050043e-06,  1.213369e+02 };
  noIBL_sq_phi[4] = {2.187977e-06,  2.114925e+02 };
  noIBL_sq_phi[5] = {2.187977e-06,  2.114925e+02 };

  std::map<int,std::vector<double>> noIBL_sq_hlfIpt;
  noIBL_sq_hlfIpt[0] = {7.004789e-12,  4.130296e-04 };
  noIBL_sq_hlfIpt[1] = {7.521822e-12,  5.390953e-04 };
  noIBL_sq_hlfIpt[2] = {1.274270e-11,  9.718143e-04 };
  noIBL_sq_hlfIpt[3] = {4.524908e-11,  1.547622e-03 };
  noIBL_sq_hlfIpt[4] = {5.795779e-11,  2.592702e-03 };
  noIBL_sq_hlfIpt[5] = {5.795779e-11,  2.592702e-03 };


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

  double sigma_HalfInvPt   = 0;
  double sigma_phi  = 0;
  double sigma_d0   = 0;
  double sigma_z0   = 0;
  double sigma_eta  = 0;

  if (hasIBLhit and linpar) {

    // can this sigma go negative?
    sigma_HalfInvPt = withIBL_lin_hlfIpt[eta_bin][0] +  withIBL_lin_hlfIpt[eta_bin][1] * truth_qOver2Pt;
    sigma_phi       = withIBL_lin_phi[eta_bin][0]    +  withIBL_lin_phi[eta_bin][1]    * truth_qOver2Pt;
    sigma_d0        = withIBL_lin_d0[eta_bin][0]     +  withIBL_lin_d0[eta_bin][1]     * truth_qOver2Pt;
    sigma_z0        = withIBL_lin_z0[eta_bin][0]     +  withIBL_lin_z0[eta_bin][1]     * truth_qOver2Pt;
    sigma_eta       = withIBL_lin_eta[eta_bin][0]    +  withIBL_lin_eta[eta_bin][1]    * truth_qOver2Pt;

  } else if(!hasIBLhit and linpar) {

    sigma_HalfInvPt = noIBL_lin_hlfIpt[eta_bin][0] + noIBL_lin_hlfIpt[eta_bin][1] * truth_qOver2Pt;
    sigma_phi       = noIBL_lin_phi[eta_bin][0]    + noIBL_lin_phi[eta_bin][1]    * truth_qOver2Pt;
    sigma_d0        = noIBL_lin_d0[eta_bin][0]     + noIBL_lin_d0[eta_bin][1]     * truth_qOver2Pt;
    sigma_z0        = noIBL_lin_z0[eta_bin][0]     + noIBL_lin_z0[eta_bin][1]     * truth_qOver2Pt;
    sigma_eta       = noIBL_lin_eta[eta_bin][0]    + noIBL_lin_eta[eta_bin][1]    * truth_qOver2Pt;

  } else if (hasIBLhit and !linpar) {

    sigma_HalfInvPt = std::sqrt(withIBL_sq_hlfIpt[eta_bin][0] + withIBL_sq_hlfIpt[eta_bin][1] * std::pow(truth_qOver2Pt,2));
    sigma_phi       = std::sqrt(withIBL_sq_phi[eta_bin][0]    + withIBL_sq_phi[eta_bin][1]    * std::pow(truth_qOver2Pt,2));
    sigma_d0        = std::sqrt(withIBL_sq_d0[eta_bin][0]     + withIBL_sq_d0[eta_bin][1]     * std::pow(truth_qOver2Pt,2));
    sigma_z0        = std::sqrt(withIBL_sq_z0[eta_bin][0]     + withIBL_sq_z0[eta_bin][1]     * std::pow(truth_qOver2Pt,2));
    sigma_eta       = std::sqrt(withIBL_sq_eta[eta_bin][0]    + withIBL_sq_eta[eta_bin][1]    * std::pow(truth_qOver2Pt,2));

  } else if(!hasIBLhit and !linpar) {

    sigma_HalfInvPt = std::sqrt(noIBL_sq_hlfIpt[eta_bin][0] + noIBL_sq_hlfIpt[eta_bin][1] * std::pow(truth_qOver2Pt,2));
    sigma_phi       = std::sqrt(noIBL_sq_phi[eta_bin][0]    + noIBL_sq_phi[eta_bin][1]    * std::pow(truth_qOver2Pt,2));
    sigma_d0        = std::sqrt(noIBL_sq_d0[eta_bin][0]     + noIBL_sq_d0[eta_bin][1]     * std::pow(truth_qOver2Pt,2));
    sigma_z0        = std::sqrt(noIBL_sq_z0[eta_bin][0]     + noIBL_sq_z0[eta_bin][1]     * std::pow(truth_qOver2Pt,2));
    sigma_eta       = std::sqrt(noIBL_sq_eta[eta_bin][0]    + noIBL_sq_eta[eta_bin][1]    * std::pow(truth_qOver2Pt,2));

  }

  // a bit of trigonometry to propagate the sigma on ctheta
  double theta = 2. * std::atan(std::exp(-1.0*truth_eta));
  double factor = (2. * std::exp(truth_eta)) / (std::exp(2.*truth_eta)+1);
  double sigma_ctheta = factor / std::pow(std::sin(theta),2);

  sigma_ctheta *= sigma_eta;

  track->setHalfInvPt((charge * 0.5/(truth_pT)) + CLHEP::RandGauss::shoot(m_randomEngine, 0, sigma_HalfInvPt));
  track->setCotTheta(truth_ctheta + CLHEP::RandGauss::shoot(m_randomEngine, 0, sigma_ctheta));
  track->setPhi(truth_phi + CLHEP::RandGauss::shoot(m_randomEngine, 0, sigma_phi));
  track->setIP(truth_d0 + CLHEP::RandGauss::shoot(m_randomEngine, 0, sigma_d0));
  track->setZ0(truth_z0 + CLHEP::RandGauss::shoot(m_randomEngine, 0, sigma_z0));

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

/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#include "EnergyDepositionTool.h"

#include "TString.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetIdentifier/PixelID.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"

#include "GeneratorObjects/HepMcParticleLink.h"
#include "SiPropertiesTool/SiliconProperties.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"

#include "PathResolver/PathResolver.h"


#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandExpZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
#include "TLorentzVector.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "PixelDigitizationUtilities.h"
#include <cmath>
#include <fstream>
#include <limits> //for numeric_limits min




namespace{
  //iBetaGamma function returning zero for the error case
  double iBetaGammaFn(const double k){
    double result(0.);
    constexpr double me =0.51099906; //electron mass in MeV, directly from CLHEP file
    if (auto subCalc = 2. * me + k; subCalc>0){
      result = std::sqrt(k*subCalc)/me;
    }
    return result;
  }
  double iBetaGammaFn(const TLorentzVector & vec){
    double result(0.);
    double beta = vec.Beta();
    //if beta >=1, bad things happen because 
    //TLorentzVector::Gamma = 1./sqrt(1. - beta*beta)
    if (beta <1.0) result = beta * vec.Gamma();
    return result;
  }
  //=======================================
  // TRF PDG
  //=======================================
  int 
  trfPDG(int pdgId) {
    if (std::fabs(pdgId) == 2212) return 1;                                              // proton

    if (std::fabs(pdgId) == 211) return 2;                                              // pion

    // alpha is skipped -- 3
    if (std::fabs(pdgId) == 11) return 4;                                              // electron

    if (std::fabs(pdgId) == 321) return 5;                                              // kaon

    if (std::fabs(pdgId) == 13) return 6;                                              // muon

    return -1;   // unsupported particle
  }
  
  //==========================================
  // S E T  F A I L U R E
  //==========================================
  void 
  SetFailureFlag(std::vector<std::pair<double, double> >& rawHitRecord) {
    rawHitRecord.clear();
    std::pair<double, double> specialFlag;
    specialFlag.first = -1.;
    specialFlag.second = -1.;
    rawHitRecord.push_back(specialFlag);
    return;
  }
}

// Constructor with parameters:
EnergyDepositionTool::EnergyDepositionTool(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent) {}

EnergyDepositionTool::~EnergyDepositionTool() {}

//=======================================
// I N I T I A L I Z E
//=======================================
StatusCode EnergyDepositionTool::initialize() {
  ATH_MSG_INFO("You are using EnergyDepositionTool for solid-state silicon detectors.");

  ATH_CHECK(detStore()->retrieve(m_pixelID, "PixelID"));

  //Setup distortions tool
  if (!m_disableDistortions) {
    ATH_MSG_DEBUG("Getting distortions tool");
    ATH_CHECK(m_distortionKey.initialize());
  }

 if (m_doBichsel) {
    // Load Bichsel data
    m_bichselData.clear();
    ATH_MSG_INFO("The number of collision for each sampling is " << m_nCols);
    ATH_MSG_INFO("Loading data file");
    int n_ParticleType = 6;
    for (int iParticleType = 1; iParticleType <= n_ParticleType; iParticleType++) {
      const std::string & inputFileName(PixelDigitization::formBichselDataFileName(iParticleType, m_nCols));
      const std::string & fullFileName = PathResolverFindCalibFile(inputFileName);
      ATH_MSG_INFO("Bichsel Data File "<<fullFileName);
      BichselData iData = PixelDigitization::getBichselDataFromFile(fullFileName);
      m_bichselData.push_back(iData);
    }
    ATH_MSG_INFO("Finish Loading Data File");
  }


  m_doDeltaRay = (m_doBichsel && m_doDeltaRay);    // if we don't do Bichsel model, no re-simulation on delta-ray at
                                                   // all!
  return StatusCode::SUCCESS;
}

//=======================================
// F I N A L I Z E
//=======================================
StatusCode EnergyDepositionTool::finalize() {
  ATH_MSG_DEBUG("EnergyDepositionTool::finalize()");
  return StatusCode::SUCCESS;
}

//=======================================
// D E P O S I T  E N E R G Y
//=======================================
StatusCode EnergyDepositionTool::depositEnergy(const TimedHitPtr<SiHit>& phit, const InDetDD::SiDetectorElement& Module,
                                               std::vector<std::pair<double, double> >& trfHitRecord,
                                               std::vector<double>& initialConditions,
                                               CLHEP::HepRandomEngine* rndmEngine,
                                               const EventContext &ctx) {
  ATH_MSG_DEBUG("Deposit energy in sensor volume.");

  //Check if simulated particle or delta ray
  const EBC_EVCOLL evColl = EBC_MAINEVCOLL;
  const HepMcParticleLink::PositionFlag idxFlag =
    (phit.eventId() == 0) ? HepMcParticleLink::IS_POSITION : HepMcParticleLink::IS_INDEX;
  const HepMcParticleLink McLink {
    HepMcParticleLink(phit->trackNumber(), phit.eventId(), evColl, idxFlag, ctx)
  };
  const HepMC::GenParticle* genPart = McLink.cptr();
  bool delta_hit = true;
  if (genPart) delta_hit = false;
  double sensorThickness = Module.design().thickness();


  //Get path of particle through volume of G4
  double stepsize = sensorThickness / m_numberOfSteps;
  const CLHEP::Hep3Vector startPosition = phit->localStartPosition();
  const CLHEP::Hep3Vector endPosition = phit->localEndPosition();

  //Get entry and exit positions, store for SensorSim tools
  double eta_0 = startPosition[SiHit::xEta];
  double phi_0 = startPosition[SiHit::xPhi];
  const double depth_0 = startPosition[SiHit::xDep];

  double eta_f = endPosition[SiHit::xEta];
  double phi_f = endPosition[SiHit::xPhi];
  const double depth_f = endPosition[SiHit::xDep];

  //Simulate effect of bowing on entry and exit points
  if (!m_disableDistortions && !delta_hit) simulateBow(&Module, phi_0, eta_0, depth_0, phi_f, eta_f, depth_f);

  double dEta = eta_f - eta_0;
  double dPhi = phi_f - phi_0;
  const double dDepth = depth_f - depth_0;
  double pathLength = std::sqrt(dEta * dEta + dPhi * dPhi + dDepth * dDepth);

  //Scale steps and charge chunks
  const int nsteps = int(pathLength / stepsize) + 1;
  const int ncharges = this->m_numberOfCharges * this->m_numberOfSteps / nsteps + 1;

  //Store information
  initialConditions.clear();
  initialConditions = {eta_0, phi_0, depth_0, dEta, dPhi, dDepth, static_cast<double>(ncharges)};

  //////////////////////////////////////////////////////
  // ***                For Bichsel               *** //
  //////////////////////////////////////////////////////
  double iTotalLength = pathLength * 1000.;   // mm -> micrometer
  initialConditions.push_back(iTotalLength);

  // -1 ParticleType means we are unable to run Bichel simulation for this case
  int ParticleType = -1;
  if (m_doBichsel and !(Module.isDBM()) and genPart) {
    ParticleType = delta_hit ? (m_doDeltaRay ? 4 : -1) : trfPDG(genPart->pdg_id());
    if (ParticleType != -1) { // this is a protection in case delta_hit == true (a delta ray)
      TLorentzVector genPart_4V;

      if (genPart) { // non-delta-ray
        genPart_4V.SetPtEtaPhiM(genPart->momentum().perp(), genPart->momentum().eta(),
                                genPart->momentum().phi(), genPart->momentum().m());
        if (iBetaGammaFn(genPart_4V) < m_doBichselBetaGammaCut){
          ParticleType = -1;
        }        
      } else { // delta-ray.
        double k = phit->energyLoss() / CLHEP::MeV;     // unit of MeV
        if (iBetaGammaFn(k) < m_doBichselBetaGammaCut){
          ParticleType = -1;
        } 
      }

      // In-time PU
      if (!m_doPU) {
        if (phit.eventId() != 0) ParticleType = -1;
      }

      // Out-of-time PU
      // We don't cut on the out-of-time PU, since studies show that the fraction is too small
    }
  }

  if (ParticleType != -1) { // yes, good to go with Bichsel
    // I don't know why genPart->momentum() goes crazy ...
    TLorentzVector genPart_4V;
    double iBetaGamma=0.;
    if (genPart) {
      genPart_4V.SetPtEtaPhiM(genPart->momentum().perp(), genPart->momentum().eta(),
                              genPart->momentum().phi(), genPart->momentum().m());
      iBetaGamma = iBetaGammaFn(genPart_4V);
    } else {
      double k = phit->energyLoss() / CLHEP::MeV;     // unit of MeV                            // unit of MeV
      iBetaGamma = iBetaGammaFn(k);
    }

    int iParticleType = ParticleType;
    // begin simulation
    std::vector<std::pair<double, double> > rawHitRecord = BichselSim(iBetaGamma, iParticleType, iTotalLength,
                                                                      genPart ? (genPart->momentum().e() /
                                                                                 CLHEP::MeV) : (phit->energyLoss() /
                                                                                                CLHEP::MeV),
    rndmEngine);

    // check if returned simulation result makes sense
    if (rawHitRecord.empty()) { // deal with rawHitRecord==0 specifically -- no energy deposition
      std::pair<double, double> specialHit;
      specialHit.first = 0.;
      specialHit.second = 0.;
      trfHitRecord.push_back(specialHit);
    } else if ((rawHitRecord.size() == 1) && (rawHitRecord[0].first == -1.) && (rawHitRecord[0].second == -1.)) { // special flag returned from BichselSim meaning it FAILs
      for (int j = 0; j < nsteps; j++) { // do the same thing as old digitization method
        std::pair<double, double> specialHit;
        specialHit.first = 1.0 * iTotalLength / nsteps * (j + 0.5);
        specialHit.second = phit->energyLoss() * 1.E+6 / nsteps;
        trfHitRecord.push_back(specialHit);
      }
    } else { // cluster thousands hits to ~20 groups
      trfHitRecord = ClusterHits(rawHitRecord, nsteps);
    }
  } else { // same as old digitization method
    //////////////////////////////////////////////////////
    // ***         B I C H S E L    O F F           *** //
    //////////////////////////////////////////////////////
    //double iTotalLength = pathLength*1000.;   // mm -> micrometer
    for (int j = 0; j < nsteps; j++) { // do the same thing as old digitization method
      std::pair<double, double> specialHit;
      specialHit.first = 1.0 * iTotalLength / nsteps * (j + 0.5);
      specialHit.second = phit->energyLoss() * 1.E+6 / nsteps;
      trfHitRecord.push_back(specialHit);
    }
  }

  // *** Finsih Bichsel *** //
  return StatusCode::SUCCESS;
}

//======================================
//  S I M U L A T E    B O W
//======================================
void EnergyDepositionTool::simulateBow(const InDetDD::SiDetectorElement* element, double& xi, double& yi,
                                       const double zi, double& xf, double& yf, const double zf) const {
  // If tool is NONE we apply no correction.
  Amg::Vector3D dir(element->hitPhiDirection() * (xf - xi),
                    element->hitEtaDirection() * (yf - yi), element->hitDepthDirection() * (zf - zi));

  Amg::Vector2D locposi = element->hitLocalToLocal(yi, xi);
  Amg::Vector2D locposf = element->hitLocalToLocal(yf, xf);

  Amg::Vector2D newLocposi = SG::ReadCondHandle<PixelDistortionData>(m_distortionKey)->correctSimulation(m_pixelID->wafer_hash(
                                                                                                           element->
                                                                                                            identify()), locposi,
                                                                                                         dir);
  Amg::Vector2D newLocposf = SG::ReadCondHandle<PixelDistortionData>(m_distortionKey)->correctSimulation(m_pixelID->wafer_hash(
                                                                                                           element->
                                                                                                            identify()), locposf,
                                                                                                         dir);

  // Extract new coordinates and convert back to hit frame.
  xi = newLocposi[Trk::x] * element->hitPhiDirection();
  yi = newLocposi[Trk::y] * element->hitEtaDirection();

  xf = newLocposf[Trk::x] * element->hitPhiDirection();
  yf = newLocposf[Trk::y] * element->hitEtaDirection();
}

//=======================================
// B I C H S E L   D E P O S I T I O N
//=======================================
// input total length should be in the unit of micrometer
// InciEnergy should be in MeV
// In case there is any abnormal in runtime, (-1,-1) will be returned indicating old deposition model should be used
// instead
//-----------------------------------------------------------
std::vector<std::pair<double, double> > 
EnergyDepositionTool::BichselSim(double BetaGamma, int ParticleType,double TotalLength, 
     double InciEnergy,CLHEP::HepRandomEngine* rndmEngine) const {
  ATH_MSG_DEBUG("Begin EnergyDepositionTool::BichselSim");

  // prepare hit record (output)
  std::vector<std::pair<double, double> > rawHitRecord;
  double TotalEnergyLoss = 0.;
  double accumLength = 0.;
  
  if (BetaGamma <= std::numeric_limits<double>::min()){
    SetFailureFlag(rawHitRecord);
    return rawHitRecord;
  }

  // load relevant data
  BichselData iData = m_bichselData[ParticleType - 1];
  double BetaGammaLog10 = std::log10(BetaGamma);
  std::pair<int, int> indices_BetaGammaLog10 = iData.getBetaGammaIndices(BetaGammaLog10);

  // upper bound
  double IntXUpperBound = iData.interpolateCrossSection(indices_BetaGammaLog10, BetaGammaLog10);
  if (IntXUpperBound <= 0.) {
    ATH_MSG_WARNING("Negative IntXUpperBound in EnergyDepositionTool::BichselSim! (-1,-1) will be returned");
    SetFailureFlag(rawHitRecord);
    return rawHitRecord;
  }
  
  if(IntXUpperBound<std::numeric_limits<double>::min()){
    SetFailureFlag(rawHitRecord);
    return rawHitRecord;
  }
  
  // mean-free path
  double lambda = (1. / IntXUpperBound) * 1.E4;   // unit of IntX is cm-1. It needs to be converted to micrometer-1


  // direct those hits with potential too many steps into nominal simulation
  int LoopLimit = m_LoopLimit;                         // limit assuming 1 collision per sampling
  if (std::abs(1.0 * TotalLength / lambda) > LoopLimit) {       // m_nCols is cancelled out in the formula
    SetFailureFlag(rawHitRecord);
    return rawHitRecord;
  }

  // begin simulation
  int count = 0;
  while (true) {
    // infinite loop protection
    if (count >= (1.0 * LoopLimit / m_nCols)) {
      ATH_MSG_WARNING(
        "Potential infinite loop in BichselSim. Exit Loop. A special flag will be returned (-1,-1). The total length is " << TotalLength << ". The lambda is " << lambda <<
          ".");
      SetFailureFlag(rawHitRecord);
      break;
    }

    // sample hit position -- exponential distribution
    double HitPosition = 0.;
    for (int iHit = 0; iHit < m_nCols; iHit++) {
      HitPosition += CLHEP::RandExpZiggurat::shoot(rndmEngine, lambda);
    }
    // termination by hit position
    // yes, in case m_nCols > 1, we will loose the last m_nCols collisions. So m_nCols cannot be too big
    if (accumLength + HitPosition >= TotalLength) break;

    // sample single collision
    double TossEnergyLoss = -1.;
    while (TossEnergyLoss <= 0.) { // we have to do this because sometimes TossEnergyLoss will be negative due to too
                                   // small TossIntX
      double TossIntX = CLHEP::RandFlat::shoot(rndmEngine, 0., IntXUpperBound);
      TossEnergyLoss = iData.interpolateCollisionEnergy(indices_BetaGammaLog10, std::log10(TossIntX));
    }

    // check if it is delta-ray -- delta-ray is already taken care of by G4 and treated as an independent hit.
    // Unfortunately, we won't deal with delta-ray using Bichsel's model
    // as long as m_nCols is not very big, the probability of having >= 2 such a big energy loss in a row is very small.
    // In case there is a delta-ray, it would be so dominant that other energy deposition becomes negligible
    if (TossEnergyLoss > (m_DeltaRayCut * 1000.)) {
      TossEnergyLoss = 0.;
    }

    bool fLastStep = false;

    if (((TotalEnergyLoss + TossEnergyLoss) / 1.E+6) > InciEnergy) {
      ATH_MSG_WARNING(
        "Energy loss is larger than incident energy in EnergyDepositionTool::BichselSim! This is usually delta-ray.");
      TossEnergyLoss = InciEnergy * 1.E+6 - TotalEnergyLoss;
      fLastStep = true;
    }

    // update
    accumLength += HitPosition;
    TotalEnergyLoss += TossEnergyLoss;

    // record this hit
    std::pair<double, double> oneHit;
    if (m_nCols == 1) oneHit.first = accumLength;
    else oneHit.first = (accumLength - 1.0 * HitPosition / 2);// as long as m_nCols is small enough (making sure lambda*m_nCols is within resolution of a pixel), then taking middle point might still be reasonable
    oneHit.second = TossEnergyLoss;
    rawHitRecord.push_back(oneHit);

    count++;

    if (fLastStep) break;
  }

  ATH_MSG_DEBUG("Finish EnergyDepositionTool::BichselSim");

  return rawHitRecord;
}

//=======================================
// C L U S T E R   H I T S
//=======================================
std::vector<std::pair<double, double> > EnergyDepositionTool::ClusterHits(std::vector<std::pair<double,
                                                                                                double> >& rawHitRecord,
                                                                          int n_pieces) const {
  ATH_MSG_DEBUG("Begin EnergyDepositionTool::ClusterHits");
  std::vector<std::pair<double, double> > trfHitRecord;

  if ((int) (rawHitRecord.size()) < n_pieces) { // each single collision is the most fundamental unit
    n_pieces = rawHitRecord.size();
  }

  int unitlength = int(1.0 * rawHitRecord.size() / n_pieces);
  int index_start = 0;
  int index_end = unitlength - 1;   // [index_start, index_end] are included
  while (true) {
    // calculate weighted center of each slice
    double position = 0.;
    double energyloss = 0.;

    for (int index = index_start; index <= index_end; index++) {
      position += (rawHitRecord[index].first * rawHitRecord[index].second);
      energyloss += rawHitRecord[index].second;
    }
    position = (energyloss == 0. ? 0. : position / energyloss);

    // store
    std::pair<double, double> oneHit;
    oneHit.first = position;
    oneHit.second = energyloss;
    trfHitRecord.push_back(oneHit);

    // procede to next slice
    index_start = index_end + 1;
    index_end = index_start + unitlength - 1;

    if (index_start > (int) (rawHitRecord.size() - 1)) {
      break;
    }

    if (index_end > (int) (rawHitRecord.size() - 1)) {
      index_end = rawHitRecord.size() - 1;
    }
  }

  ATH_MSG_DEBUG("Finish EnergyDepositionTool::ClusterHits");

  return trfHitRecord;
}


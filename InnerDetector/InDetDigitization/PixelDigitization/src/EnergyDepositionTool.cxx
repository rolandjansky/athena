/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyDepositionTool.cxx
//   Implementation file for class EnergyDepositionTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
// Details in head file
///////////////////////////////////////////////////////////////////

#include "EnergyDepositionTool.h"

#include "TGraph.h"
#include "TString.h"
#include "TMath.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetIdentifier/PixelID.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "SiPropertiesSvc/SiliconProperties.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include "PathResolver/PathResolver.h"
#include <fstream>
#include <cmath>

#include "CLHEP/Random/RandExpZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
#include "TLorentzVector.h"
#include "CLHEP/Units/PhysicalConstants.h"

using namespace std;

// Constructor with parameters:
EnergyDepositionTool::EnergyDepositionTool(const std::string& type, const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_numberOfSteps(50),
  m_numberOfCharges(10),
  m_disableDistortions(false),
  m_doBichsel(false),
  m_doBichselBetaGammaCut(0.1),        // replace momentum cut
  m_doDeltaRay(false),                 // need validation
  m_doPU(true)
{ 

  declareProperty("DeltaRayCut", m_DeltaRayCut = 117.);
  declareProperty("nCols", m_nCols = 1);
  declareProperty("LoopLimit", m_LoopLimit = 100000);
  declareProperty("numberOfSteps",m_numberOfSteps,"Geant4:number of steps for PixelPlanar");
  declareProperty("numberOfCharges",m_numberOfCharges,"Geant4:number of charges for PixelPlanar");
  declareProperty("DisableDistortions",m_disableDistortions, "Disable simulation of module distortions");
  declareProperty("doBichsel", m_doBichsel, "re-do charge deposition following Bichsel model");
  declareProperty("doBichselBetaGammaCut", m_doBichselBetaGammaCut, "minimum beta-gamma for particle to be re-simulated through Bichsel Model");
  declareProperty("doDeltaRay", m_doDeltaRay, "whether we simulate delta-ray using Bichsel model");
  declareProperty("doPU", m_doPU, "Whether we apply Bichsel model on PU");
}

// Destructor:
EnergyDepositionTool::~EnergyDepositionTool(){}

//=======================================
// I N I T I A L I Z E
//=======================================
StatusCode EnergyDepositionTool::initialize() {
  
  ATH_MSG_INFO("You are using EnergyDepositionTool for solid-state silicon detectors.");

  //Setup distortions tool
  if (!m_disableDistortions) {
    ATH_MSG_DEBUG("Getting distortions tool");
    if (!m_pixDistoTool.empty()) {
      CHECK(m_pixDistoTool.retrieve());
      ATH_MSG_DEBUG("Distortions tool retrieved");
    }
    else {
      ATH_MSG_DEBUG("No distortions tool selected");
    }
  }

  if(m_doBichsel){
    // Load Bichsel data
    m_BichselData.clear();
    ATH_MSG_INFO("The number of collision for each sampling is " << m_nCols);
    ATH_MSG_INFO("Loading data file");

    int n_ParticleType = 6;
    for(int iParticleType = 1; iParticleType <= n_ParticleType; iParticleType++){
    
      std::ifstream inputFile;
      TString inputFileName = TString::Format("PixelDigitization/Bichsel_%d%s.dat", iParticleType, m_nCols == 1 ? "" : TString::Format("_%dsteps", m_nCols).Data());

      std::string FullFileName = PathResolverFindCalibFile(std::string(inputFileName.Data()));
      inputFile.open(FullFileName.data());

      ATH_MSG_INFO( "Loading file name : " << inputFileName.Data());
      ATH_MSG_INFO( "-- File full name: " << FullFileName.data());
      ATH_MSG_INFO( "-- Is file open ? " << inputFile.is_open());

      if(!inputFile.is_open()){
        ATH_MSG_FATAL("Fail to load file " << inputFileName.Data() << " !");
	ATH_MSG_FATAL("EnergyDepositionTool::initialize() failed");
	return StatusCode::FAILURE;
      }

      // prepare data
      BichselData iData;

      double BetaGammaLog10 = 0; inputFile >> BetaGammaLog10;
      double ColELog10 = 0;      inputFile >> ColELog10;
      double IntXLog10 = 0;      inputFile >> IntXLog10;

      ATH_MSG_INFO( "-- File eof check : " << inputFile.eof());

      while(!inputFile.eof()){
        // check if this BetaGamma has already been stored
        if( (iData.Array_BetaGammaLog10.size() == 0) || (iData.Array_BetaGammaLog10.back() != BetaGammaLog10) ){ // a new BetaGamma

          if(iData.Array_BetaGammaLog10.size() != 0){
            iData.Array_BetaGammaLog10_UpperBoundIntXLog10.push_back(iData.Array_BetaGammaLog10_IntXLog10.back().back());
	  }

	  iData.Array_BetaGammaLog10.push_back(BetaGammaLog10);
	  std::vector<double> new_ColELog10;  iData.Array_BetaGammaLog10_ColELog10.push_back(new_ColELog10);
	  std::vector<double> new_IntXLog10;  iData.Array_BetaGammaLog10_IntXLog10.push_back(new_IntXLog10);
	}

	iData.Array_BetaGammaLog10_ColELog10.back().push_back(ColELog10);
	iData.Array_BetaGammaLog10_IntXLog10.back().push_back(IntXLog10);

	inputFile >> BetaGammaLog10;
	inputFile >> ColELog10;
	inputFile >> IntXLog10;
      }
      iData.Array_BetaGammaLog10_UpperBoundIntXLog10.push_back(iData.Array_BetaGammaLog10_IntXLog10.back().back());

      ATH_MSG_INFO("-- Array_BetaGammaLog10 size : " << iData.Array_BetaGammaLog10.size());
      ATH_MSG_INFO("-- Array_BetaGammaLog10_ColELog10 size at 0 : " << iData.Array_BetaGammaLog10_ColELog10[0].size());
      ATH_MSG_INFO("-- Array_BetaGammaLog10_IntXLog10 size at 0 : " << iData.Array_BetaGammaLog10_IntXLog10[0].size());
      ATH_MSG_INFO("-- Array_BetaGammaLog10_UpperBoundIntXLog10 : " << iData.Array_BetaGammaLog10_UpperBoundIntXLog10.size());

      m_BichselData.push_back(iData);
      inputFile.close();

      ATH_MSG_INFO("-- Finish loading file " << inputFileName.Data());
    }
  ATH_MSG_INFO("Finish Loading Data File");
  }

  m_doDeltaRay = (m_doBichsel && m_doDeltaRay);    // if we don't do Bichsel model, no re-simulation on delta-ray at all!
  return StatusCode::SUCCESS;
}
//=======================================
// F I N A L I Z E
//=======================================
StatusCode EnergyDepositionTool::finalize() {
  ATH_MSG_DEBUG ( "EnergyDepositionTool::finalize()");
  return StatusCode::SUCCESS;
}


//=======================================
// D E P O S I T  E N E R G Y
//=======================================
StatusCode EnergyDepositionTool::depositEnergy(const TimedHitPtr<SiHit> &phit, const InDetDD::SiDetectorElement &Module, std::vector<std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions, CLHEP::HepRandomEngine *rndmEngine){

  ATH_MSG_DEBUG("Deposit energy in sensor volume.");
  
  //Check if simulated particle or delta ray
  const HepMcParticleLink McLink = HepMcParticleLink(phit->trackNumber(),phit.eventId());
  const HepMC::GenParticle* genPart= McLink.cptr(); 
  bool delta_hit = true;
  if (genPart) delta_hit = false;
  double sensorThickness = Module.design().thickness();


  //Get path of particle through volume of G4
  double stepsize = sensorThickness/m_numberOfSteps;
  const CLHEP::Hep3Vector startPosition=phit->localStartPosition();
  const CLHEP::Hep3Vector endPosition=phit->localEndPosition();

  //Get entry and exit positions, store for SensorSim tools
  double eta_0=startPosition[SiHit::xEta];
  double phi_0=startPosition[SiHit::xPhi];
  const double depth_0=startPosition[SiHit::xDep];

  double eta_f = endPosition[SiHit::xEta];
  double phi_f = endPosition[SiHit::xPhi];
  const double depth_f = endPosition[SiHit::xDep];

  //Simulate effect of bowing on entry and exit points
  if (!m_disableDistortions && !delta_hit) simulateBow(&Module,phi_0,eta_0,depth_0,phi_f,eta_f,depth_f);
  
  double dEta=eta_f-eta_0;
  double dPhi=phi_f-phi_0;
  const double dDepth=depth_f-depth_0;
  double pathLength=sqrt(dEta*dEta+dPhi*dPhi+dDepth*dDepth);

  //Scale steps and charge chunks
  const int nsteps=int(pathLength/stepsize)+1; 
  const int ncharges=this->m_numberOfCharges*this->m_numberOfSteps/nsteps+1; 

  //Store information
  initialConditions.clear();
  initialConditions.push_back( eta_0 );
  initialConditions.push_back( phi_0 );
  initialConditions.push_back( depth_0 );
  initialConditions.push_back( dEta );
  initialConditions.push_back( dPhi );
  initialConditions.push_back( dDepth );
  initialConditions.push_back( ncharges );

  //////////////////////////////////////////////////////
  // ***                For Bichsel               *** //
  //////////////////////////////////////////////////////
  double iTotalLength = pathLength*1000.;   // mm -> micrometer
  initialConditions.push_back( iTotalLength );

  // -1 ParticleType means we are unable to run Bichel simulation for this case
  int ParticleType = -1;
  if(m_doBichsel && !(Module.isDBM())){

    ParticleType = delta_hit ? (m_doDeltaRay ? 4 : -1) : trfPDG(genPart->pdg_id()); 
  

    if(ParticleType != -1){ // this is a protection in case delta_hit == true (a delta ray)
      TLorentzVector genPart_4V;

      if(genPart){ // non-delta-ray
        genPart_4V.SetPtEtaPhiM(genPart->momentum().perp(), genPart->momentum().eta(), genPart->momentum().phi(), genPart->momentum().m());
        double iBetaGamma = genPart_4V.Beta() * genPart_4V.Gamma();
        if(iBetaGamma < m_doBichselBetaGammaCut) ParticleType = -1;

      }
      else{ // delta-ray. 
        double k = phit->energyLoss()/CLHEP::MeV;     // unit of MeV
        double m = 0.511;                             // unit of MeV
        double iBetaGamma = TMath::Sqrt(k*(2*m+k))/m;

        if(iBetaGamma < m_doBichselBetaGammaCut) ParticleType = -1;
      }

      // In-time PU
      if(!m_doPU){
        if(phit.eventId() != 0) ParticleType = -1;
      }

      // Out-of-time PU
      // We don't cut on the out-of-time PU, since studies show that the fraction is too small
    }
  } 

  if(ParticleType != -1){ // yes, good to go with Bichsel
    // I don't know why genPart->momentum() goes crazy ... 
    TLorentzVector genPart_4V;
    double iBetaGamma;

    
    if(genPart){
      genPart_4V.SetPtEtaPhiM(genPart->momentum().perp(), genPart->momentum().eta(), genPart->momentum().phi(), genPart->momentum().m());
      iBetaGamma = genPart_4V.Beta() * genPart_4V.Gamma();
    }
    else{
      double k = phit->energyLoss()/CLHEP::MeV;     // unit of MeV
      double m = 0.511;                             // unit of MeV
      iBetaGamma = TMath::Sqrt(k*(2*m+k))/m;
    }

    int iParticleType = ParticleType;
    //double iTotalLength = pathLength*1000.;   // mm -> micrometer

    // begin simulation
    std::vector<std::pair<double,double> > rawHitRecord = BichselSim(iBetaGamma, iParticleType, iTotalLength, genPart ? (genPart->momentum().e()/CLHEP::MeV) : (phit->energyLoss()/CLHEP::MeV), rndmEngine);

    // check if returned simulation result makes sense
    if(rawHitRecord.size() == 0){ // deal with rawHitRecord==0 specifically -- no energy deposition
      std::pair<double,double> specialHit;
      specialHit.first = 0.; specialHit.second = 0.;
      trfHitRecord.push_back(specialHit);      
    }
    else if( (rawHitRecord.size() == 1) && (rawHitRecord[0].first == -1.) && (rawHitRecord[0].second == -1.) ){ // special flag returned from BichselSim meaning it FAILs
      for(int j = 0; j < nsteps; j++){ // do the same thing as old digitization method
        std::pair<double,double> specialHit;
        specialHit.first = 1.0*iTotalLength/nsteps * (j + 0.5); specialHit.second = phit->energyLoss()*1.E+6/nsteps;
        trfHitRecord.push_back(specialHit);
      }
    }
    else{ // cluster thousands hits to ~20 groups
      trfHitRecord = ClusterHits(rawHitRecord, nsteps);
    }
  }
  else{  // same as old digitization method
  //////////////////////////////////////////////////////
  // ***         B I C H S E L    O F F           *** //
  //////////////////////////////////////////////////////
    //double iTotalLength = pathLength*1000.;   // mm -> micrometer
    for(int j = 0; j < nsteps; j++){ // do the same thing as old digitization method
      std::pair<double,double> specialHit;
      specialHit.first = 1.0*iTotalLength/nsteps * (j + 0.5); specialHit.second = phit->energyLoss()*1.E+6/nsteps;
      trfHitRecord.push_back(specialHit);
    }
  }
  
  // *** Finsih Bichsel *** //
  return StatusCode::SUCCESS;

}

//======================================
//  S I M U L A T E    B O W 
//======================================
void EnergyDepositionTool::simulateBow(const InDetDD::SiDetectorElement * element, double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const {

  // If tool is NONE we apply no correction.
  if (m_pixDistoTool.empty()) return;
  Amg::Vector3D dir(element->hitPhiDirection()*(xf-xi), element->hitEtaDirection()*(yf-yi), element->hitDepthDirection()*(zf-zi));

  Amg::Vector2D locposi = element->hitLocalToLocal(yi, xi);
  Amg::Vector2D locposf = element->hitLocalToLocal(yf, xf);

  Amg::Vector2D newLocposi = m_pixDistoTool->correctSimulation(element->identify(), locposi, dir);
  Amg::Vector2D newLocposf = m_pixDistoTool->correctSimulation(element->identify(), locposf, dir);

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
// In case there is any abnormal in runtime, (-1,-1) will be returned indicating old deposition model should be used instead
//-----------------------------------------------------------
std::vector<std::pair<double,double> > EnergyDepositionTool::BichselSim(double BetaGamma, int ParticleType, double TotalLength, double InciEnergy, CLHEP::HepRandomEngine *rndmEngine) const{
  ATH_MSG_DEBUG("Begin EnergyDepositionTool::BichselSim");

  // prepare hit record (output)
  std::vector<std::pair<double,double> > rawHitRecord;
  double TotalEnergyLoss = 0.;
  double accumLength = 0.;

  // load relevant data
  BichselData iData = m_BichselData[ParticleType-1];
  double BetaGammaLog10 = TMath::Log10(BetaGamma);
  std::pair<int,int> indices_BetaGammaLog10 = GetBetaGammaIndices(BetaGammaLog10, iData);

  // upper bound
  double IntXUpperBound = GetUpperBound(indices_BetaGammaLog10, BetaGammaLog10, iData);
  if(IntXUpperBound <= 0.){
    ATH_MSG_WARNING("Negative IntXUpperBound in EnergyDepositionTool::BichselSim! (-1,-1) will be returned");
    SetFailureFlag(rawHitRecord);
    return rawHitRecord;
  }
 
  // mean-free path
  double lambda = (1./IntXUpperBound) * 1.E4;   // unit of IntX is cm-1. It needs to be converted to micrometer-1

  // check nan lambda
  if(std::isnan(lambda)){
    SetFailureFlag(rawHitRecord);
    return rawHitRecord;
  }

  // direct those hits with potential too many steps into nominal simulation
  int LoopLimit = m_LoopLimit;                         // limit assuming 1 collision per sampling
  if(fabs(1.0*TotalLength/lambda) > LoopLimit){        // m_nCols is cancelled out in the formula
    SetFailureFlag(rawHitRecord);
    return rawHitRecord;
  }

  // begin simulation
  int count = 0;
  while(true){
    // infinite loop protection
    if(count >= (1.0*LoopLimit/m_nCols)){
      ATH_MSG_WARNING("Potential infinite loop in BichselSim. Exit Loop. A special flag will be returned (-1,-1). The total length is " << TotalLength << ". The lambda is " << lambda << ".");
      SetFailureFlag(rawHitRecord);
      break;
    }

    // sample hit position -- exponential distribution
    double HitPosition = 0.;
    for(int iHit = 0; iHit < m_nCols; iHit++){
	   HitPosition += CLHEP::RandExpZiggurat::shoot(rndmEngine, lambda);
    }
    // termination by hit position
    // yes, in case m_nCols > 1, we will loose the last m_nCols collisions. So m_nCols cannot be too big
    if(accumLength + HitPosition >= TotalLength)
      break;

    // sample single collision
    double TossEnergyLoss = -1.;
    while(TossEnergyLoss <= 0.){ // we have to do this because sometimes TossEnergyLoss will be negative due to too small TossIntX
      double TossIntX = CLHEP::RandFlat::shoot(rndmEngine, 0., IntXUpperBound);
      TossEnergyLoss = GetColE(indices_BetaGammaLog10, TMath::Log10(TossIntX), iData);
    }

    // check if it is delta-ray -- delta-ray is already taken care of by G4 and treated as an independent hit. Unfortunately, we won't deal with delta-ray using Bichsel's model
    // as long as m_nCols is not very big, the probability of having >= 2 such a big energy loss in a row is very small. In case there is a delta-ray, it would be so dominant that other energy deposition becomes negligible
    if(TossEnergyLoss > (m_DeltaRayCut*1000.)){
      TossEnergyLoss = 0.;
    }

    bool fLastStep = false;

    if( ((TotalEnergyLoss + TossEnergyLoss)/1.E+6) > InciEnergy ){
      ATH_MSG_WARNING("Energy loss is larger than incident energy in EnergyDepositionTool::BichselSim! This is usually delta-ray.");
      TossEnergyLoss = InciEnergy*1.E+6 - TotalEnergyLoss;
      fLastStep = true;
    }

    // update
    accumLength += HitPosition;
    TotalEnergyLoss += TossEnergyLoss;

    // record this hit
    std::pair<double,double> oneHit;
    if(m_nCols == 1)  oneHit.first = accumLength; 
    else              oneHit.first = (accumLength - 1.0*HitPosition/2);     // as long as m_nCols is small enough (making sure lambda*m_nCols is withint resolution of a pixel), then taking middle point might still be reasonable
    oneHit.second = TossEnergyLoss;
    rawHitRecord.push_back(oneHit);

    count++;

    if(fLastStep)
      break;
  }

  ATH_MSG_DEBUG("Finish EnergyDepositionTool::BichselSim");

  return rawHitRecord;
}

//=======================================
// C L U S T E R   H I T S
//=======================================
std::vector<std::pair<double,double> > EnergyDepositionTool::ClusterHits(std::vector<std::pair<double,double> >& rawHitRecord, int n_pieces) const{
  ATH_MSG_DEBUG("Begin EnergyDepositionTool::ClusterHits");
  std::vector<std::pair<double,double> > trfHitRecord;

  if((int)(rawHitRecord.size()) < n_pieces){ // each single collision is the most fundamental unit
    n_pieces = rawHitRecord.size();
  }

  int unitlength = int(1.0*rawHitRecord.size()/n_pieces);
  int index_start = 0;
  int index_end = unitlength-1;   // [index_start, index_end] are included
  while(true){
    // calculate weighted center of each slice
    double position = 0.;
    double energyloss = 0.;

    for(int index = index_start; index <= index_end; index++){
      position += (rawHitRecord[index].first * rawHitRecord[index].second);
      energyloss += rawHitRecord[index].second;
    }
    position = (energyloss == 0. ? 0. : position/energyloss);

    // store
    std::pair<double,double> oneHit;
    oneHit.first = position; oneHit.second = energyloss;
    trfHitRecord.push_back(oneHit);

    // procede to next slice
    index_start = index_end + 1;
    index_end = index_start + unitlength - 1;

    if(index_start > (int)(rawHitRecord.size()-1)){
      break;
    }

    if(index_end > (int)(rawHitRecord.size()-1)){
      index_end = rawHitRecord.size()-1;
    }
  }

  ATH_MSG_DEBUG("Finsih EnergyDepositionTool::ClusterHits");

  return trfHitRecord;
}


///////////
// Utils //
///////////

//=======================================
// TRF PDG
//=======================================
int EnergyDepositionTool::trfPDG(int pdgId) const{
  if(std::fabs(pdgId) == 2212) return 1;   // proton
  if(std::fabs(pdgId) == 211)  return 2;   // pion
  // alpha is skipped -- 3
  if(std::fabs(pdgId) == 11)   return 4;   // electron
  if(std::fabs(pdgId) == 321)  return 5;   // kaon
  if(std::fabs(pdgId) == 13)   return 6;   // muon

  return -1;   // unsupported particle
}

//TODO: why is this implemented? We shuld use a std lib search
//=======================================
// C L U S T E R   H I T S
//=======================================
std::pair<int,int> EnergyDepositionTool::FastSearch(std::vector<double> vec, double item) const{
  std::pair<int,int> output;

  int index_low = 0;
  int index_up = vec.size()-1;

  if((item < vec[index_low]) || (item > vec[index_up])){
    output.first = -1; output.second = -1;
    return output;
  }
  else if(item == vec[index_low]){
    output.first = index_low; output.second = index_low;
    return output;
  }
  else if(item == vec[index_up]){
    output.first = index_up; output.second = index_up;
    return output;
  }

  while( (index_up - index_low) != 1 ){
    int index_middle = int(1.0*(index_up + index_low)/2.);
    if(item < vec[index_middle])
      index_up = index_middle;
    else if(item > vec[index_middle])
      index_low = index_middle;
    else{ // accurate hit. Though this is nearly impossible ...
      output.first = index_middle; output.second = index_middle;
      return output;
    }
  }

  output.first = index_low; output.second = index_up;
  return output;
}

//=======================================
// B E T A   G A M M A   I N D E X 
//=======================================
std::pair<int,int> EnergyDepositionTool::GetBetaGammaIndices(double BetaGammaLog10, BichselData& iData) const{
  std::pair<int,int> indices_BetaGammaLog10;
  if(BetaGammaLog10 > iData.Array_BetaGammaLog10.back()){ // last one is used because when beta-gamma is very large, energy deposition behavior is very similar
    indices_BetaGammaLog10.first = iData.Array_BetaGammaLog10.size()-1;
    indices_BetaGammaLog10.second = iData.Array_BetaGammaLog10.size()-1;
  }
  else{
    indices_BetaGammaLog10 = FastSearch(iData.Array_BetaGammaLog10, BetaGammaLog10);
  }

  return indices_BetaGammaLog10;
}

//==========================================
// G E T   C O L L I S I O N   E N E R G Y
//==========================================
//Interpolate collision energy
double EnergyDepositionTool::GetColE(std::pair<int,int> indices_BetaGammaLog10, double IntXLog10, BichselData& iData) const{

  if( (indices_BetaGammaLog10.first==-1) && (indices_BetaGammaLog10.second==-1) )
    return -1.;
  
  // BetaGammaLog10_2 then
  std::pair<int,int> indices_IntXLog10_x2 = FastSearch(iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.second], IntXLog10);
  if (indices_IntXLog10_x2.first<0)  { return -1; }
  if (indices_IntXLog10_x2.second<0) { return -1; }
  
  double y21 = iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.second][indices_IntXLog10_x2.first];
  double y22 = iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.second][indices_IntXLog10_x2.second];
  double Est_x2 = ((y22 - IntXLog10)*iData.Array_BetaGammaLog10_ColELog10[indices_BetaGammaLog10.second][indices_IntXLog10_x2.first] + (IntXLog10 - y21)*iData.Array_BetaGammaLog10_ColELog10[indices_BetaGammaLog10.second][indices_IntXLog10_x2.second])/(y22-y21);
  double Est = Est_x2;

  return TMath::Power(10., Est);
}

//===========================================
// Overloaded C O L L I S I O N  E N E R G Y
//===========================================
double EnergyDepositionTool::GetColE(double BetaGammaLog10, double IntXLog10, BichselData& iData) const{
  std::pair<int,int> indices_BetaGammaLog10 = GetBetaGammaIndices(BetaGammaLog10, iData);
  return GetColE(indices_BetaGammaLog10, IntXLog10, iData);
}


//==========================================
// G E T   U P P E R   B O U N D  BETA GAMMA
//==========================================
double EnergyDepositionTool::GetUpperBound(std::pair<int,int> indices_BetaGammaLog10, double BetaGammaLog10, BichselData& iData) const{

  if (indices_BetaGammaLog10.first<0)  { return -1; }
  if (indices_BetaGammaLog10.second<0) { return -1; }
  double BetaGammaLog10_1 = iData.Array_BetaGammaLog10[indices_BetaGammaLog10.first];
  double BetaGammaLog10_2 = iData.Array_BetaGammaLog10[indices_BetaGammaLog10.second];

  // obtain estimation
  double Est_1 = iData.Array_BetaGammaLog10_UpperBoundIntXLog10[indices_BetaGammaLog10.first];
  double Est_2 = iData.Array_BetaGammaLog10_UpperBoundIntXLog10[indices_BetaGammaLog10.second];

  // final estimation
  double Est = ((BetaGammaLog10_2 - BetaGammaLog10)*Est_1 + (BetaGammaLog10 - BetaGammaLog10_1)*Est_2)/(BetaGammaLog10_2 - BetaGammaLog10_1);

  return TMath::Power(10., Est);
}

//==========================================
// overloaded G E T  U P P E R   B O U N D
//==========================================
double EnergyDepositionTool::GetUpperBound(double BetaGammaLog10, BichselData& iData) const{
  std::pair<int,int> indices_BetaGammaLog10 = GetBetaGammaIndices(BetaGammaLog10, iData);
  return GetUpperBound(indices_BetaGammaLog10, BetaGammaLog10, iData);
}

//==========================================
// S E T  F A I L U R E
//==========================================
void EnergyDepositionTool::SetFailureFlag(std::vector<std::pair<double,double> >& rawHitRecord) const{
  rawHitRecord.clear();
  std::pair<double, double> specialFlag;
  specialFlag.first = -1.; specialFlag.second = -1.;
  rawHitRecord.push_back(specialFlag);

  return;
}

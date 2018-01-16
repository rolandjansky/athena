/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BichselSimTool.cxx
//   Implementation file for class BichselSimTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
// Details in head file
///////////////////////////////////////////////////////////////////

#include "BichselSimTool.h"

#include "TGraph.h"
#include "TString.h"
#include "TMath.h"

#include "PathResolver/PathResolver.h"
#include <fstream>
#include <cmath>

#include "AtlasCLHEP_RandomGenerators/RandExpZiggurat.h"
#include "CLHEP/Random/RandFlat.h"

using namespace std;

static const InterfaceID IID_IBichselSimTool("BichselSimTool", 1, 0);
const InterfaceID& BichselSimTool::interfaceID( ){ return IID_IBichselSimTool; }

// Constructor with parameters:
BichselSimTool::BichselSimTool(const std::string& type, const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_rndmSvc("AtDSFMTGenSvc",name),
  m_rndmEngineName("PixelDigitization"),
  m_rndmEngine(0)
{ 
	declareInterface< BichselSimTool >( this );

  declareProperty("DeltaRayCut", m_DeltaRayCut = 117.);
  declareProperty("nCols", m_nCols = 1);
  declareProperty("LoopLimit", m_LoopLimit = 100000);
  declareProperty("RndmSvc", m_rndmSvc, "Random Number Service used in BichselSimTool");
  declareProperty("RndmEngine", m_rndmEngineName, "Random engine name");
}

// Destructor:
BichselSimTool::~BichselSimTool()
{
}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode BichselSimTool::initialize() {
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL("BichselSimTool::initialize() failed");
    return sc;
  } 

  
  //** define your initialize below **//

  // random svc
  if ( m_rndmSvc.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( " Can't get RndmSvc " );
    return StatusCode::FAILURE;
  } else { 
    ATH_MSG_DEBUG ( "Retrieved RndmSvc" );
  }

  // get the random stream
  ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE;
  } else { 
    ATH_MSG_DEBUG ( " Found RndmEngine : " << m_rndmEngineName ); 
  }

  // clear data table
  m_BichselData.clear();

  // a notification on m_nCols
  ATH_MSG_INFO("The number of collision for each sampling is " << m_nCols);

  // load data table
  ATH_MSG_INFO("Loading data file");

  int n_ParticleType = 6;
  for(int iParticleType = 1; iParticleType <= n_ParticleType; iParticleType++){
    // configure file name
    std::ifstream inputFile;
    TString inputFileName = TString::Format("PixelDigitization/Bichsel_%d%s.dat", iParticleType, m_nCols == 1 ? "" : TString::Format("_%dsteps", m_nCols).Data());

    std::string FullFileName = PathResolverFindCalibFile(std::string(inputFileName.Data()));
    inputFile.open(FullFileName.data());

    ATH_MSG_INFO( "Loading file name : " << inputFileName.Data());
    ATH_MSG_INFO( "-- File full name: " << FullFileName.data());
    ATH_MSG_INFO( "-- Is file open ? " << inputFile.is_open());

    if(!inputFile.is_open()){
      ATH_MSG_FATAL("Fail to load file " << inputFileName.Data() << " !");
      sc = StatusCode::FAILURE;
      continue;
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

  return sc; 
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode BichselSimTool::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "BichselSimTool::finalize() failed");
    return sc ;
  }

  //** define your finalize below **//


  ATH_MSG_DEBUG ( "BichselSimTool::finalize()");
  return sc ;
}

//-----------------------------------------------------------
// Implementation below
//-----------------------------------------------------------

/////////////////////////////////////////////
// Main Public Functions Accessed by Users //
/////////////////////////////////////////////

// input total length should be in the unit of micrometer
// InciEnergy should be in MeV
// In case there is any abnormal in runtime, (-1,-1) will be returned indicating old deposition model should be used instead
std::vector<std::pair<double,double> > BichselSimTool::BichselSim(double BetaGamma, int ParticleType, double TotalLength, double InciEnergy) const{
  ATH_MSG_DEBUG("Begin BichselSimTool::BichselSim");

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
    ATH_MSG_WARNING("Negative IntXUpperBound in BichselSimTool::BichselSim! (-1,-1) will be returned");
    // if(IntXUpperBound == -1.){
    //   std::cout << "Cannot find beta-gamma(log10) " << BetaGammaLog10 << " in data table!" << std::endl;
    // }
    SetFailureFlag(rawHitRecord);
    return rawHitRecord;
  }
  
  // mean-free path
  double lambda = (1./IntXUpperBound) * 1.E4;   // unit of IntX is cm-1. It needs to be converted to micrometer-1

  // check nan lambda
  // I dont' know why we get nan lambda this moment ... but anyway ... 
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
    for(int iHit = 0; iHit < m_nCols; iHit++) HitPosition += CLHEP::RandExpZiggurat::shoot(m_rndmEngine, lambda);

    // termination by hit position
    // yes, in case m_nCols > 1, we will loose the last m_nCols collisions. So m_nCols cannot be too big
    if(accumLength + HitPosition >= TotalLength)
      break;

    // sample single collision
    double TossEnergyLoss = -1.;
    // double TossIntX_record;
    while(TossEnergyLoss <= 0.){ // we have to do this because sometimes TossEnergyLoss will be negative due to too small TossIntX
      double TossIntX = CLHEP::RandFlat::shoot(m_rndmEngine, 0., IntXUpperBound);
      TossEnergyLoss = GetColE(indices_BetaGammaLog10, TMath::Log10(TossIntX), iData);

      // TossIntX_record = TossIntX;
    }

    // check if it is delta-ray -- delta-ray is already taken care of by G4 and treated as an independent hit. Unfortunately, we won't deal with delta-ray using Bichsel's model
    // as long as m_nCols is not very big, the probability of having >= 2 such a big energy loss in a row is very small. In case there is a delta-ray, it would be so dominant that other energy deposition becomes negligible
    if(TossEnergyLoss > (m_DeltaRayCut*1000.)){
      // ATH_MSG_WARNING("!!! Energy deposition beyond delta-ray cut !!!");
      // std::cout << "+++++++ " << TossEnergyLoss/1000. << " keV " << "+++++++++++" << std::endl; 
      // std::cout << "+++++++ " << ParticleType << std::endl;
      // std::cout << "+++++++ " << BetaGammaLog10 << std::endl;
      // std::cout << "+++++++ " << TMath::Log10(TossIntX_record) << std::endl;
      TossEnergyLoss = 0.;
    }

    bool fLastStep = false;

    // in case energy loss so far is larger than incident energy -- basically not possible ...
    // This becomes important after taking delta-ray into account!
    if( ((TotalEnergyLoss + TossEnergyLoss)/1.E+6) > InciEnergy ){
      ATH_MSG_WARNING("Energy loss is larger than incident energy in BichselSimTool::BichselSim! This is usually delta-ray.");
      // then this is the last step
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

  ATH_MSG_DEBUG("Finish BichselSimTool::BichselSim");

  return rawHitRecord;
}

std::vector<std::pair<double,double> > BichselSimTool::ClusterHits(std::vector<std::pair<double,double> >& rawHitRecord, int n_pieces) const{
  ATH_MSG_DEBUG("Begin BichselSimTool::ClusterHits");
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

  ATH_MSG_DEBUG("Finsih BichselSimTool::ClusterHits");

  return trfHitRecord;
}


///////////
// Utils //
///////////

int BichselSimTool::trfPDG(int pdgId) const{
  if(std::fabs(pdgId) == 2212) return 1;   // proton
  if(std::fabs(pdgId) == 211)  return 2;   // pion
  // alpha is skipped -- 3
  if(std::fabs(pdgId) == 11)   return 4;   // electron
  if(std::fabs(pdgId) == 321)  return 5;   // kaon
  if(std::fabs(pdgId) == 13)   return 6;   // muon

  return -1;   // unsupported particle
}

// assume vec is already sorted from small to large
std::pair<int,int> BichselSimTool::FastSearch(std::vector<double> vec, double item) const{
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


std::pair<int,int> BichselSimTool::GetBetaGammaIndices(double BetaGammaLog10, BichselData& iData) const{
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



// make sure IntXLog10 is less than the value given by GetUpperBound()
// Please refer to below which can give more realistive result
// But don't delete this fraction. Keep it for record
// double BichselSimTool::GetColE(double BetaGammaLog10, double IntXLog10, BichselData& iData) const{
//   std::pair<int,int> indices_BetaGammaLog10;
//   if(BetaGammaLog10 > iData.Array_BetaGammaLog10.back()){ // last one is used because when beta-gamma is very large, energy deposition behavior is very similar
//     indices_BetaGammaLog10.first = iData.Array_BetaGammaLog10.size()-1;
//     indices_BetaGammaLog10.second = iData.Array_BetaGammaLog10.size()-1;
//   }
//   else{
//     indices_BetaGammaLog10 = FastSearch(iData.Array_BetaGammaLog10, BetaGammaLog10);
//   }

//   if( (indices_BetaGammaLog10.first==-1) && (indices_BetaGammaLog10.second==-1) )
//     return -1.;
//   double BetaGammaLog10_1 = iData.Array_BetaGammaLog10[indices_BetaGammaLog10.first];
//   double BetaGammaLog10_2 = iData.Array_BetaGammaLog10[indices_BetaGammaLog10.second];

//   // BetaGammaLog10_1 first
//   std::pair<int,int> indices_IntXLog10_x1 = FastSearch(iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.first], IntXLog10);
//   if( (indices_IntXLog10_x1.first==-1) && (indices_IntXLog10_x1.second==-1) )
//     return -1.;
//   double y11 = iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.first][indices_IntXLog10_x1.first];
//   double y12 = iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.first][indices_IntXLog10_x1.second];
//   double Est_x1 = ((y12 - IntXLog10)*iData.Array_BetaGammaLog10_ColELog10[indices_BetaGammaLog10.first][indices_IntXLog10_x1.first] + (IntXLog10 - y11)*iData.Array_BetaGammaLog10_ColELog10[indices_BetaGammaLog10.first][indices_IntXLog10_x1.second])/(y12-y11);

//   // BetaGammaLog10_2 then
//   std::pair<int,int> indices_IntXLog10_x2 = FastSearch(iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.second], IntXLog10);
//   if( (indices_IntXLog10_x2.first==-1) && (indices_IntXLog10_x2.second==-1) )
//     return -1;
//   double y21 = iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.second][indices_IntXLog10_x2.first];
//   double y22 = iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.second][indices_IntXLog10_x2.second];
//   double Est_x2 = ((y22 - IntXLog10)*iData.Array_BetaGammaLog10_ColELog10[indices_BetaGammaLog10.second][indices_IntXLog10_x2.first] + (IntXLog10 - y21)*iData.Array_BetaGammaLog10_ColELog10[indices_BetaGammaLog10.second][indices_IntXLog10_x2.second])/(y22-y21);

//   // final estimation
//   double Est = ((BetaGammaLog10_2 - BetaGammaLog10)*Est_x1 + (BetaGammaLog10 - BetaGammaLog10_1)*Est_x2)/(BetaGammaLog10_2 - BetaGammaLog10_1);

//   return TMath::Power(10., Est);
// }

// another way to do interpolation -- Fix beta-gamma
// IMPORTANT!! Use this one. don't use the upper one.
// This one will give correct dEdx curve
double BichselSimTool::GetColE(std::pair<int,int> indices_BetaGammaLog10, double IntXLog10, BichselData& iData) const{
  // std::pair<int,int> indices_BetaGammaLog10;
  // if(BetaGammaLog10 > iData.Array_BetaGammaLog10.back()){ // last one is used because when beta-gamma is very large, energy deposition behavior is very similar
  //   indices_BetaGammaLog10.first = iData.Array_BetaGammaLog10.size()-1;
  //   indices_BetaGammaLog10.second = iData.Array_BetaGammaLog10.size()-1;
  // }
  // else{
  //   indices_BetaGammaLog10 = FastSearch(iData.Array_BetaGammaLog10, BetaGammaLog10);
  // }

  if( (indices_BetaGammaLog10.first==-1) && (indices_BetaGammaLog10.second==-1) )
    return -1.;
  //double BetaGammaLog10_1 = iData.Array_BetaGammaLog10[indices_BetaGammaLog10.first];
  //double BetaGammaLog10_2 = iData.Array_BetaGammaLog10[indices_BetaGammaLog10.second];

  // BetaGammaLog10_1 first
  // std::pair<int,int> indices_IntXLog10_x1 = FastSearch(iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.first], IntXLog10);
  // if( (indices_IntXLog10_x1.first==-1) && (indices_IntXLog10_x1.second==-1) )
  //   return -1.;
  // double y11 = iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.first][indices_IntXLog10_x1.first];
  // double y12 = iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.first][indices_IntXLog10_x1.second];
  // double Est_x1 = ((y12 - IntXLog10)*iData.Array_BetaGammaLog10_ColELog10[indices_BetaGammaLog10.first][indices_IntXLog10_x1.first] + (IntXLog10 - y11)*iData.Array_BetaGammaLog10_ColELog10[indices_BetaGammaLog10.first][indices_IntXLog10_x1.second])/(y12-y11);

  // BetaGammaLog10_2 then
  std::pair<int,int> indices_IntXLog10_x2 = FastSearch(iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.second], IntXLog10);
  if( (indices_IntXLog10_x2.first==-1) && (indices_IntXLog10_x2.second==-1) )
    return -1;
  double y21 = iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.second][indices_IntXLog10_x2.first];
  double y22 = iData.Array_BetaGammaLog10_IntXLog10[indices_BetaGammaLog10.second][indices_IntXLog10_x2.second];
  double Est_x2 = ((y22 - IntXLog10)*iData.Array_BetaGammaLog10_ColELog10[indices_BetaGammaLog10.second][indices_IntXLog10_x2.first] + (IntXLog10 - y21)*iData.Array_BetaGammaLog10_ColELog10[indices_BetaGammaLog10.second][indices_IntXLog10_x2.second])/(y22-y21);

  // final estimation
  //double Est = ((BetaGammaLog10_2 - BetaGammaLog10)*Est_x1 + (BetaGammaLog10 - BetaGammaLog10_1)*Est_x2)/(BetaGammaLog10_2 - BetaGammaLog10_1);
  double Est = Est_x2;

  return TMath::Power(10., Est);
}

double BichselSimTool::GetColE(double BetaGammaLog10, double IntXLog10, BichselData& iData) const{
  std::pair<int,int> indices_BetaGammaLog10 = GetBetaGammaIndices(BetaGammaLog10, iData);
  return GetColE(indices_BetaGammaLog10, IntXLog10, iData);
}

// IMPORTANT!! For this one, one should use interpolation, instead of fixed beta-gamma.
// Otherwise, dE/dx shape will get distorted again.
double BichselSimTool::GetUpperBound(std::pair<int,int> indices_BetaGammaLog10, double BetaGammaLog10, BichselData& iData) const{
  // std::pair<int,int> indices_BetaGammaLog10;
  // if(BetaGammaLog10 > iData.Array_BetaGammaLog10.back()){
  //   indices_BetaGammaLog10.first = iData.Array_BetaGammaLog10.size()-1;
  //   indices_BetaGammaLog10.second = iData.Array_BetaGammaLog10.size()-1;
  // }
  // else{
  //   indices_BetaGammaLog10 = FastSearch(iData.Array_BetaGammaLog10, BetaGammaLog10);
  // }

  if( (indices_BetaGammaLog10.first==-1) && (indices_BetaGammaLog10.second==-1) ){
    // std::cout << "++++++++++++++" << std::endl;
    // std::cout << BetaGammaLog10 << std::endl;
    // std::cout << iData.Array_BetaGammaLog10[0] << " , " << iData.Array_BetaGammaLog10.back() << std::endl;

    return -1.;
  }
  double BetaGammaLog10_1 = iData.Array_BetaGammaLog10[indices_BetaGammaLog10.first];
  double BetaGammaLog10_2 = iData.Array_BetaGammaLog10[indices_BetaGammaLog10.second];

  // obtain estimation
  double Est_1 = iData.Array_BetaGammaLog10_UpperBoundIntXLog10[indices_BetaGammaLog10.first];
  double Est_2 = iData.Array_BetaGammaLog10_UpperBoundIntXLog10[indices_BetaGammaLog10.second];

  // final estimation
  double Est = ((BetaGammaLog10_2 - BetaGammaLog10)*Est_1 + (BetaGammaLog10 - BetaGammaLog10_1)*Est_2)/(BetaGammaLog10_2 - BetaGammaLog10_1);

  return TMath::Power(10., Est);
}

double BichselSimTool::GetUpperBound(double BetaGammaLog10, BichselData& iData) const{
  std::pair<int,int> indices_BetaGammaLog10 = GetBetaGammaIndices(BetaGammaLog10, iData);
  return GetUpperBound(indices_BetaGammaLog10, BetaGammaLog10, iData);
}

// Keep for record
// double BichselSimTool::GetUpperBound(double BetaGammaLog10, BichselData& iData) const{
//   std::pair<int,int> indices_BetaGammaLog10;
//   if(BetaGammaLog10 > iData.Array_BetaGammaLog10.back()){
//     indices_BetaGammaLog10.first = iData.Array_BetaGammaLog10.size()-1;
//     indices_BetaGammaLog10.second = iData.Array_BetaGammaLog10.size()-1;
//   }
//   else{
//     indices_BetaGammaLog10 = FastSearch(iData.Array_BetaGammaLog10, BetaGammaLog10);
//   }

//   if( (indices_BetaGammaLog10.first==-1) && (indices_BetaGammaLog10.second==-1) )
//     return -1.;
//   double BetaGammaLog10_1 = iData.Array_BetaGammaLog10[indices_BetaGammaLog10.first];
//   double BetaGammaLog10_2 = iData.Array_BetaGammaLog10[indices_BetaGammaLog10.second];

//   // obtain estimation
//   double Est_1 = iData.Array_BetaGammaLog10_UpperBoundIntXLog10[indices_BetaGammaLog10.first];
//   double Est_2 = iData.Array_BetaGammaLog10_UpperBoundIntXLog10[indices_BetaGammaLog10.second];

//   // final estimation
//   //double Est = ((BetaGammaLog10_2 - BetaGammaLog10)*Est_1 + (BetaGammaLog10 - BetaGammaLog10_1)*Est_2)/(BetaGammaLog10_2 - BetaGammaLog10_1);
//   double Est = Est_2;

//   return TMath::Power(10., Est);
// }

void BichselSimTool::SetFailureFlag(std::vector<std::pair<double,double> >& rawHitRecord) const{
  rawHitRecord.clear();
  std::pair<double, double> specialFlag;
  specialFlag.first = -1.; specialFlag.second = -1.;
  rawHitRecord.push_back(specialFlag);

  return;
}







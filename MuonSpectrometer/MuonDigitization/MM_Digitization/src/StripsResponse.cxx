/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *  StripsResponse.cxx
 *  MC for micromegas athena integration
 *
 **/

/// PROJECTS
#include "MM_Digitization/StripsResponse.h"
#include "GaudiKernel/MsgStream.h"
#include "MM_Digitization/MM_IonizationCluster.h"
#include "MM_Digitization/MM_StripResponse.h"
#include "MM_Digitization/GarfieldGas.h"
#include "PathResolver/PathResolver.h"

#include<map>
#include<algorithm>

using namespace std;

/*******************************************************************************/
StripsResponse::StripsResponse():
  m_qThreshold(0.001), 
  m_diffusSigma(0.360/10.),
  m_LogitundinalDiffusSigma(0.190/10.),
  m_pitch(0.500), 
  m_stripwidth(m_pitch - 0.0),
  //  gain(1.),  
  m_gain(5.e+3),
  m_polya(0),
  m_conv_gaus(0),
  m_gas(0), 
  driftGap(5.128),
  driftVelocity(0.047),
  nstrip(0),
  temp_polya(0),
  polya_theta(0),
  numberOfElectrons(0),
  dimClusters(0),
  MaxPrimaryIons(300),
  pt(0),
  xx(0),
  xxDiffussion(0),
  yy(0),
  yyDiffussion(0),
  primaryion(0),
  lmean(0),
  totalelectrons(0),
  ll(0),
  gRandom_loc(0),
  randomNum(0)
{
  clearValues();
  initializationFrom();
}
/*******************************************************************************/
void StripsResponse::loadGasFile(std::string fileName){
  Athena::MsgStreamMember log("StripsResponse::loadGasFile"); 
  std::string fileWithPath = PathResolver::find_file (fileName, "CALIBPATH");
  if (fileWithPath == "") {
    log << MSG::FATAL << "StripResponse::loadGasFile(): Could not find file " << fileName << endmsg;
    exit(1);
  }
  bool ok = m_gas->LoadGasFile(fileWithPath);
  if(!ok){
    log << MSG::FATAL << "StripResponse::loadGasFile(): Could not load file " << fileName << endmsg;
    exit(1);
  }
  m_gas->PrintGas();
}
/*******************************************************************************/
void StripsResponse::initHistos()
{
  // For validation purposes only...
}
/*******************************************************************************/
void StripsResponse::writeHistos()
{}
/*******************************************************************************/
void StripsResponse::initFunctions()
{
  m_polya  = new TF1("polya","(1./[1])*(TMath::Power([0]+1,[0]+1)/TMath::Gamma([0]+1))*TMath::Power(x,[0])*TMath::Exp(-([0]+1)*x)", 0., 4.);
  m_conv_gaus  = new TF1("conv_gaus","1.*TMath::Exp(-TMath::Power(x,2.)/(2.*[0]*[0])) + 0.001*TMath::Exp(-TMath::Power(x,2)/(2.*[1]*[1]))", -1., 1.);
  m_LongitudinalDiffusionFunction = new TF1("longdiff","gaus", 0., 5.);
  m_TransverseDiffusionFunction = new TF1("transdiff", "1.*TMath::Exp(-TMath::Power(x,2.)/(2.*[0]*[0])) + 0.001*TMath::Exp(-TMath::Power(x,2)/(2.*[1]*[1]))", -1., 1.);
  Athena::MsgStreamMember log("StripsResponse::initFunctions");
  log << MSG::DEBUG << "StripsResponse::initFunctions DONE" << endmsg;
  // if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "\t \t StripsResponse::initFunctions DONE " << endmsg;
}
/*******************************************************************************/
void StripsResponse::clearValues()
{
  /// Delete Gen:
  if (m_conv_gaus !=0) delete m_conv_gaus;
  if (m_polya !=0) delete m_polya;
  if (gRandom_loc !=0) delete gRandom_loc;
  if (m_gas !=0) {
    delete m_gas;
    m_gas=0;
  }
  //--------------------

}
/*******************************************************************************/
void StripsResponse::initializationFrom()
{
  m_crossTalk1=0.0;
  m_crossTalk2=0.0;
  m_Lorentz_Angle = 0.0;

  initHistos ();
  initFunctions();
  randomNum = new TRandom3(0);

  m_gas = new GarfieldGas(); 

  Athena::MsgStreamMember log("StripsResponse::initializationFrom");
  log << MSG::DEBUG << "StripsResponse::initializationFrom set values" << endmsg;
  m_polya->SetParameter(0, 2.3);
  m_polya->SetParameter(1, 1.);
  //  log << MSG::DEBUG << "StripsResponse::initializationFrom getRandom: " << m_polya->GetRandom() << endmsg;
}
/*******************************************************************************/
MmStripToolOutput StripsResponse::GetResponseFrom(const MmDigitToolInput & digiInput) 
//MmElectronicsToolInput StripsResponse::GetResponseFrom(const MmDigitToolInput & digiInput) 
{
  Athena::MsgStreamMember log("StripsResponse::GetResponseFrom"); 
  log << MSG::DEBUG << "\t \t StripsResponse::GetResponseFrom start " << endmsg;

  m_Lorentz_Angle = 0.0;
  Amg::Vector3D b = digiInput.magneticField()*1e+3;//kT->T
  double ez=600.;//V/cm
  double vx,vy,vz;//X:#strip(radius,increasing to outer) Z:z(positive to IP)
  bool ok = m_gas->ElectronVelocity(0.,0.,ez,b.x(),b.y(),b.z(),vx,vy,vz); 
  if(!ok){
    log << MSG::INFO << "StripsResponse::GetResponseFrom failed to get drift velocity" << endmsg;
  }
  m_Lorentz_Angle = atan2(vx,-vz)*180./TMath::Pi();// positive to outer strips

  m_IonizationClusters.clear();
  finalNumberofStrip.clear();
  finalqStrip.clear();
  finaltStrip.clear();
  m_crossTalk1=0.0; 
  m_crossTalk2=0.0; 
  m_Lorentz_Angle = 0.0; 
  log << MSG::DEBUG << "\t \t StripsResponse::calculateResponseFrom call whichStrips " << endmsg;
  whichStrips(digiInput.positionWithinStrip(), digiInput.stripIDLocal(), digiInput.incomingAngle(), digiInput.stripMaxID(), digiInput);

  log << MSG::DEBUG << "\t \t StripsResponse::GetResponseFrom MmDigitToolOutput create " << endmsg;
 
  MmStripToolOutput tmp(finalNumberofStrip, finalqStrip, finaltStrip); // T.Saito

  //  log << MSG::DEBUG << "StripsResponse::GetResponseFrom -> hitWasEfficient(): " << tmp.hitWasEfficient() << endmsg;
  return tmp;
}
//__________________________________________________________________________________________________
//==================================================================================================
// calculate the strips that got fired, charge and time. Assume any angle thetaD....
//==================================================================================================
void StripsResponse::whichStrips(const float & hitx, const int & stripID, const float & thetaD, const int & stripMaxID, const MmDigitToolInput & digiInput)
{

  Athena::MsgStreamMember msglog("StripsResponse::whichStrips");
  msglog << MSG::DEBUG << "\t \t StripsResponse::whichStrips start " << endmsg;
  float eventTime = digiInput.eventTime();
  float theta = thetaD*TMath::Pi()/180.0;
  
  m_pitch = get_stripWidth();
  dimClusters=7000; //dimClusters=total number of collisions
  pt=0.;

  lmean = 10./26.;  //  24 e/8 mm or 23 e/10mm (as it has been used here) according to Sauli's paper, for Argon  
  
  primaryion = 0;
 
  // Now generate the primary ionization colisions, Poisson distributed
  randomNum->SetSeed((int)fabs(hitx*10000));
  pt = randomNum->Uniform();
  ll = -lmean*log(pt);
  
  while (ll < driftGap/TMath::Cos(theta)){

    MM_IonizationCluster IonizationCluster(hitx, ll*sin(theta), ll*cos(theta));
    IonizationCluster.createElectrons(randomNum);
    //    IonizationCluster.diffuseElectrons(m_LogitundinalDiffusSigma, m_diffusSigma, randomNum);
    //---
    TVector2 initialPosition = IonizationCluster.getIonizationStart();
    for (auto& Electron : IonizationCluster.getElectrons()){
      m_LongitudinalDiffusionFunction->SetParameters(1.0, initialPosition.Y(), initialPosition.Y()*m_LogitundinalDiffusSigma);
      if (m_LogitundinalDiffusSigma == 0 || m_diffusSigma == 0) {
	m_TransverseDiffusionFunction->SetParameters(initialPosition.Y()*m_diffusSigma, 0.0);
      } else {
	m_TransverseDiffusionFunction->SetParameters(initialPosition.Y()*m_diffusSigma, 1.0);
      }
      gRandom = randomNum;
      Electron->setOffsetPosition(m_TransverseDiffusionFunction->GetRandom(), m_LongitudinalDiffusionFunction->GetRandom());
    }
    //---

    Amg::Vector3D b = digiInput.magneticField() * 1000.;
    double ez=600.;
    double vx, vy, vz;
    bool ok = m_gas->ElectronVelocity(0.,0.,ez,b.x(),b.y(),b.z(),vx,vy,vz);    
    if(!ok) {
      msglog << MSG::WARNING << "StripsResponse::GetResponseFrom failed to get drift velocity " << endmsg;
    }

    IonizationCluster.propagateElectrons(vx, -vz, driftVelocity);
    //    IonizationCluster.avalancheElectrons(m_gain, randomNum);
    //---
    for (auto& Electron : IonizationCluster.getElectrons()){
      m_polya->SetParameters(2.3, 1.0);
      gRandom = randomNum;
      Electron->setCharge(m_gain*m_polya->GetRandom());

      // Add eventTime in Electron time
      Electron->setTime(Electron->getTime() + eventTime);
    }
    //---
    m_IonizationClusters.push_back(IonizationCluster);

    pt = randomNum->Uniform();
    ll -= lmean*log(pt);

    primaryion++;
    if (primaryion >= MaxPrimaryIons) break; //don't create more than "MaxPrimaryIons" along a track....
    
  }//end of clusters loop

  float timeresolution = 0.01; //ns
  
  MM_StripResponse StripResponse(m_IonizationClusters, timeresolution, m_pitch, stripID, stripMaxID);
  StripResponse.timeOrderElectrons();
  StripResponse.calculateTimeSeries(thetaD, digiInput.gasgap());
  StripResponse.simulateCrossTalk(m_crossTalk1, m_crossTalk2);
  StripResponse.calculateSummaries(m_qThreshold);
  //Connect the output with the rest of the existing code
  finalNumberofStrip = StripResponse.getStripVec();
  finalqStrip = StripResponse.getTotalChargeVec();
  finaltStrip = StripResponse.getTimeThresholdVec();
  tStripElectronicsAbThr = StripResponse.getTimeMaxChargeVec();
  qStripElectronics = StripResponse.getMaxChargeVec();

} // end of whichStrips()
/*******************************************************************************/
StripsResponse::~StripsResponse()
{
  clearValues();
}
/*******************************************************************************/

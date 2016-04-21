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

#include "MM_Digitization/GarfieldGas.h" // 27/05/2015 T.Saito
#include "PathResolver/PathResolver.h"// 27/05/2015 T.Saito
/*******************************************************************************/
StripsResponse::StripsResponse():
  qThreshold(0.001), 
  diffusSigma(0.360/10.),
  LogitundinalDiffusSigma(0.190/10.),
  pitch(0.500),
  stripwidth(pitch - 0.0),
  polya(0),
  conv_gaus(0),
  gas(0), // 28/05/2015 T.Saito
  driftGap(5.128),
  driftVelocity(0.047),
  nstrip(0),
  temp_polya(0),
  polya_theta(0),
  numberOfElectrons(0),
  dimClusters(0),
  MaxPrimaryIons(0),
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
// 28/05/2015 T.Saito
void StripsResponse::loadGasFile(std::string fileName){
  Athena::MsgStreamMember log("StripsResponse::loadGasFile"); 
  std::string fileWithPath = PathResolver::find_file (fileName, "DATAPATH");
  if (fileWithPath == "") {
    log << MSG::FATAL << "StripResponse::loadGasFile(): Could not find file " << fileName << endreq;
    exit(1);
  }
  bool ok = gas->LoadGasFile(fileWithPath);
  if(!ok){
    log << MSG::FATAL << "StripResponse::loadGasFile(): Could not load file " << fileName << endreq;
    exit(1);
  }
  gas->PrintGas();
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
  polya  = new TF1("polya","(1./[1])*(TMath::Power([0]+1,[0]+1)/TMath::Gamma([0]+1))*TMath::Power(x,[0])*TMath::Exp(-([0]+1)*x)", 0., 4.);
  conv_gaus  = new TF1("conv_gaus","1.*TMath::Exp(-TMath::Power(x,2.)/(2.*[0]*[0])) + 0.001*TMath::Exp(-TMath::Power(x,2)/(2.*[1]*[1]))", -1., 1.);
  Athena::MsgStreamMember log("tripsResponse_uM::initFunctions");
  log << MSG::DEBUG << "tripsResponse_uM::initFunctions DONE" << endreq;
  // if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "\t \t StripsResponse::initFunctions DONE " << endreq;
}
/*******************************************************************************/
void StripsResponse::clearValues()
{
  /// Delete Gen:
  if (conv_gaus !=0) delete conv_gaus;
  if (polya !=0) delete polya;
  if (gRandom_loc !=0) delete gRandom_loc;
  //-- 28/05/2015 T.Saito
  if (gas !=0) {
    delete gas;
    gas=0;
  }
  //--------------------

}
/*******************************************************************************/
void StripsResponse::initializationFrom()
{
  crossTalk1=0.0;
  crossTalk2=0.0;
  Lorentz_Angle = 0.0;

  initHistos ();
  initFunctions();
  randomNum = new TRandom3(1);
  randomNum->SetSeed(1);

  gas = new GarfieldGas(); // 28/05/2015 T.Saito

  Athena::MsgStreamMember log("StripsResponse::initializationFrom");
  log << MSG::DEBUG << "StripsResponse::initializationFrom set values" << endreq;
  polya->SetParameter(0, 2.3);
  polya->SetParameter(1, 1.);
  log << MSG::DEBUG << "StripsResponse::initializationFrom getRandom: " << polya->GetRandom() << endreq;
}
/*******************************************************************************/
void StripsResponse::testRandomNumGens(UInt_t sd)
{
  TRandom3 *randomNum_ = new TRandom3(1);
  TRandom *rsave = gRandom;
  TRandom *r0 = new TRandom(1);
  Athena::MsgStreamMember log("StripsResponse::testRandomNumGens"); 
  rsave->Clear();
  r0->Clear();
  r0->SetSeed(1);
  gRandom->Clear();
  gRandom->SetSeed(sd);//always the same random numbers !!!
  rsave->SetSeed(1);//always the same random numbers !!!
  printf("Seeds a: %d | %d | %d\n", gRandom->GetSeed(), rsave->GetSeed(), r0->GetSeed());
  float pt_ = gRandom->Uniform();
  float pt = rsave->Uniform();
  float pt0 = r0->Uniform();
  for (int i=0; i< 6; i++) {
    pt_ = gRandom->Uniform();
    pt = rsave->Uniform();
    pt0 = r0->Uniform();

    log << MSG::DEBUG <<"\tpt <> " << randomNum_->Uniform() << "  " << pt << "  " << pt_ << "  " << pt0 << " | " << gRandom->GetSeed() << endreq;
    log << MSG::DEBUG << gRandom->GetSeed() << "  " << randomNum_->GetSeed() << endreq;
  }
  log << MSG::DEBUG << gRandom->GetSeed() << "  " << randomNum_->GetSeed() << endreq;
}
/*******************************************************************************/
MmDigitToolOutput StripsResponse::GetResponceFrom(const MmDigitToolInput & digiInput)
{
  Athena::MsgStreamMember log("StripsResponse::GetResponceFrom"); 
  log << MSG::DEBUG << "\t \t StripsResponse::GetResponceFrom start " << endreq;

  //-------------- 28/05/2015 T.Saito
  Lorentz_Angle = 0.0;
  Amg::Vector3D b = digiInput.magneticField()*1e+3;//kT->T
  double ez=600.;//V/cm
  double vx,vy,vz;//X:#strip(radius,increasing to outer) Z:z(positive to IP)
  bool ok = gas->ElectronVelocity(0.,0.,ez,b.x(),b.y(),b.z(),vx,vy,vz); 
  if(!ok){
    log << MSG::INFO << "StripsResponse::GetResponceFrom failed to get drift velocity" << endreq;
  }
  Lorentz_Angle = atan2(vx,-vz)*180./TMath::Pi();// positive to outer strips
  //------------------------

  /// Reduce internal calls of functions
  calculateResponceFrom(digiInput.positionWithinStrip() , digiInput.stripIDLocal() , digiInput.incomingAngle() , digiInput.stripMaxID()); // 3/2/2015 T.Saito
  //  calculateResponceFrom(digiInput.positionWithinStrip() , digiInput.stripIDLocal() , digiInput.incomingAngle() , digiInput.stripMaxID() );
  
  log << MSG::DEBUG << "\t \t StripsResponse::GetResponceFrom MmDigitToolOutput create " << endreq;
  /// ToDo: include loop for calculating Trigger study vars
 
  // MmDigitToolOutput(bool hitWasEff, std::vector <int> strpos, std::vector<float> time, std::vector<int> charge, int strTrig, float strTimeTrig ): 
  MmDigitToolOutput tmp(true, finalNumberofStrip, finaltStrip, finalqStrip, 5, 0.3);
  log << MSG::DEBUG << "StripsResponse::GetResponceFrom -> hitWasEfficient(): " << tmp.hitWasEfficient() << endreq;
  return tmp;
}
/*******************************************************************************/
void StripsResponse::calculateResponceFrom(const float & hitx, const int & stripID, const float & thetaD, const int & stripMaxID) /// <-------- stripID!!! // 3/2/2015 T.Saito
//void StripsResponse::calculateResponceFrom(const float & hitx, const int & stripID, const float & thetaD, const int & stripMaxID) /// <-------- stripID!!!
{
  Athena::MsgStreamMember log("StripsResponse::calculateResponceFrom"); 
  log << MSG::DEBUG << "\t \t StripsResponse::calculateResponceFrom start " << endreq;
  finalNumberofStrip.clear();
  finalqStrip.clear();
  finaltStrip.clear();
  crossTalk1=0.0; 
  crossTalk2=0.0; 
  Lorentz_Angle = 0.0; 
  log << MSG::DEBUG << "\t \t StripsResponse::calculateResponceFrom call whichStrips " << endreq;
  whichStrips(hitx, stripID, thetaD, stripMaxID);
}
//__________________________________________________________________________________________________
//==================================================================================================
// calculate the strips that got fired, charge and time. Assume any angle thetaD....
//==================================================================================================
void StripsResponse::whichStrips(const float & hitx, const int & stripID, const float & thetaD, const int & stripMaxID){


  pitch = get_stripWidth(); // 20/08/2015

  dimClusters=7000; //dimClusters=total number of collisions
  MaxPrimaryIons = 300; 
  pt=0.;
  xx=0.;
  xxDiffussion=0.;
  yy=0.; 
  yyDiffussion=0.;
  
  // NelectronProb has the probability that a primary ionization cluster
  float NelectronProb[23]={65.6,80.6,87.0,90.5,92.75,94.30,95.35,96.16,96.77,97.26,
			   97.65,97.95,98.20,98.40,98.56,98.68,98.775,98.850,98.913,98.967,99.016,99.061,99.105};
  
  lmean = 10./26.;  //  24 e/8 mm or 23 e/10mm (as it has been used here) according to Sauli's paper, for Argon

  float theta = thetaD*TMath::Pi()/180.0, xoff=0.;
  

  //define the polya distribution to get the flactuations of the charges...
  //  Double_t polya_alpha=1., polya_beta=(0.02-1.)/4., temp_polya, polya_theta=0.43;
  polya_theta=2.3;

  //name, name of the above C++ function, min,max, number of parameters (a,b)
  //Initialize parameters
  polya->SetParameter(0, polya_theta);
  polya->SetParameter(1, 1.);

  // zero charge,  time of primary ionization clusters etc
  l.clear(); l.resize(dimClusters, 0.);
  clusterelectrons.clear(); clusterelectrons.resize(dimClusters, 0.);
  qstrip.clear(); qstrip.resize(dimClusters, 0.);
  time.clear(); time.resize(dimClusters, 0.);
  cntTimes.clear(); cntTimes.resize(dimClusters, 0.);
  firstq.clear(); firstq.resize(dimClusters, 0.);
  ll = 0;
  totalelectrons = 0;
  primaryion = 0;
 
  // Now generate the primary ionization colisions, Poisson distributed
  while (ll<driftGap/TMath::Cos(theta)){
    pt = randomNum->Uniform();
    ll = ll-lmean*log(pt);
    if (ll>driftGap/TMath::Cos(theta)) break; // if you exceed the driftGap, go out...
    l[primaryion]=ll;
    pt = 100.0*randomNum->Uniform();
    // Use the Nelectron probability to calculate number of electrons per collision
    
    float limProb1=NelectronProb[22], limProb2;
    
    for (int k = 0; k<dimClusters; k++){
      
      if (k<23) {
        if (pt <= NelectronProb[0]) {
          clusterelectrons[primaryion] = clusterelectrons[primaryion] + k + 1;
          totalelectrons = totalelectrons + clusterelectrons[primaryion];
          break;
        } else
          if (pt <= NelectronProb[k] && pt > NelectronProb[k-1]){
            clusterelectrons[primaryion] = clusterelectrons[primaryion] + k + 1;
            totalelectrons = totalelectrons + clusterelectrons[primaryion];
            break;
          }
      } else {
        // more probabilities according to Fischle paper
        limProb2 = limProb1 + 21.6/((k)*(k));
        if (pt <= limProb2 && pt > limProb1){
          clusterelectrons[primaryion] = clusterelectrons[primaryion] + k + 1;
          totalelectrons = totalelectrons + clusterelectrons[primaryion];
          break;
        }
        
        limProb1 = limProb2;
      }
    }// end of loop for total cluster ionization
    
    primaryion = primaryion + 1;
    if (primaryion >= MaxPrimaryIons) break; //don't create more than "MaxPrimaryIons" along a track....
  }//end of clusters loop
  for (int m=0; m<primaryion; m++){    
    yy = l[m]*cos(theta);  // path of the drifted electrons
    for(int j=1;j<=clusterelectrons[m];j++){
      //
      //===================================================
      // calculate the new charge due to the Polya fluctiation of the avalanche
      // do this for each electron separately rather than the full cluster (by George Iakovidis)
      temp_polya = polya->GetRandom();
      //===================================================
      //
      yyDiffussion = randomNum->Gaus(yy, LogitundinalDiffusSigma*yy); // insert longitundinal diffusion of electron
      if(diffusSigma==0 || LogitundinalDiffusSigma==0) conv_gaus->SetParameters(diffusSigma*yy, 0.0);
      else conv_gaus->SetParameters(diffusSigma*yy, 1.0);

      float temp_gaus_conv = conv_gaus->GetRandom();
      xxDiffussion = hitx + l[m]*sin(theta) + temp_gaus_conv;// insert transverese diffusion of electron

      xx = xxDiffussion + TMath::Tan(Lorentz_Angle * TMath::Pi()/180.)*yyDiffussion;

      nstrip = xx/(stripwidth+(pitch-stripwidth));
      if (xx<0) nstrip = nstrip - 1;
      nstrip = nstrip + stripID;

      if (xx-hitx>l[m]*sin(theta)) {
        xoff = xx-(hitx+l[m]*sin(theta));
      } else {
        xoff = (hitx+l[m]*sin(theta))-xx;
      }
      numberOfElectrons = 1.0*temp_polya;//single electron fluctuation
      numberofStrip.push_back(nstrip);  qStrip.push_back(numberOfElectrons); tStrip.push_back(TMath::Sqrt(yyDiffussion*yyDiffussion+xoff*xoff)/driftVelocity);
    }

    // for each charge, assigne a crosstalk to the two neighbor strips... crossTalk1 for the strip+-1 and crossTalk2 for the strip+-2
    // do strip+-1 crosstalk only if it is greater than 0% (by George Iakovidis)
    if (crossTalk1 > 0.0) {
      if (nstrip-1>-1) {
        numberofStrip.push_back(nstrip-1);  qStrip.push_back(crossTalk1*numberOfElectrons);
        tStrip.push_back(TMath::Sqrt(yyDiffussion*yyDiffussion+xoff*xoff)/driftVelocity);
      }
      if (nstrip+1>-1) {
        numberofStrip.push_back(nstrip+1);  qStrip.push_back(crossTalk1*numberOfElectrons);
        tStrip.push_back(TMath::Sqrt(yyDiffussion*yyDiffussion+xoff*xoff)/driftVelocity);
      }
    }
    // do strip+-2 crosstalk only if it is greater than 0% (by George Iakovidis)
    if (crossTalk2 > 0.0) {
      if (nstrip-2>-1) {
        numberofStrip.push_back(nstrip-2);  qStrip.push_back(crossTalk2*numberOfElectrons);
        tStrip.push_back(TMath::Sqrt(yyDiffussion*yyDiffussion+xoff*xoff)/driftVelocity);
      }
      if (nstrip+2>-1) {
        numberofStrip.push_back(nstrip+2);  qStrip.push_back(crossTalk2*numberOfElectrons);
        tStrip.push_back(TMath::Sqrt(yyDiffussion*yyDiffussion+xoff*xoff)/driftVelocity);
      }
    }
    // end of crosstalk assignemnt
    
  }
  //calculate the time per strip, in case you cross the qThreshold=2e-
  nstrip = 0;
  for (size_t ii = 0; ii<numberofStrip.size(); ii++){
    nstrip = numberofStrip.at(ii);
    if (qstrip[nstrip] == 0) firstq[nstrip] = 0;
    qstrip[nstrip] = qstrip[nstrip] + qStrip.at(ii);

    // find the strip that receives charge above the qThreshold, assign as time the mean time of the arriving e
    if ((qstrip[nstrip] > 0.0 && firstq[nstrip] == 0 && qstrip[nstrip]<qThreshold) ||
	(qstrip[nstrip] > 0.0 && firstq[nstrip] == 0 && qstrip[nstrip]>=qThreshold && cntTimes[nstrip]>=0.)) {
      time[nstrip] = time[nstrip] + tStrip.at(ii);
      cntTimes[nstrip] = cntTimes[nstrip]+1;
    }
    if (qstrip[nstrip]>=qThreshold && firstq[nstrip] == 0) {
      time[nstrip] = time[nstrip]/cntTimes[nstrip];
      firstq[nstrip] = 1;
      cntTimes[nstrip]=0;
      stripNumber.push_back(nstrip);     // record the strip number that got a hit for 1rst time...
    }
  }
  
  int minStripEv=1000000, maxStripEv=-10000000;
  for (size_t ii = 0; ii<numberofStrip.size(); ii++){
    if (qStrip.at(ii)<qThreshold) {continue;}
    if (minStripEv>=numberofStrip.at(ii)) { minStripEv=numberofStrip.at(ii);}
    if (maxStripEv<=numberofStrip.at(ii)) { maxStripEv=numberofStrip.at(ii);}
  }
  for (int ii=minStripEv; ii<=maxStripEv; ii++) {
    if ( qstrip[ii]==0 || qstrip[ii]<qThreshold) {  continue;}   // bad strip... ignore it...
    if ( ii > stripMaxID || ii == 0 ) continue; // do not consider strips outside allowed range, i.e. outside chamber
    
    finalNumberofStrip.push_back(ii); finalqStrip.push_back(qstrip[ii]); finaltStrip.push_back(time[ii]); 
  }

  //
  //****************************************************************************
  // end of debug=10, plot the pulse of the strip... qStrip vs strip number
  //****************************************************************************
  //
  numberofStrip.clear(); qStrip.clear(); tStrip.clear();
} // end of whichStrips()
/*******************************************************************************/
StripsResponse::~StripsResponse()
{
  clearValues();
}
/*******************************************************************************/

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MDT_Response/MDT_Response.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RanluxEngine.h"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"

#include "TH1.h"
#include "TFile.h"

#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

// random generator
CLHEP::RanluxEngine ranEngine;
CLHEP::RandFlat     ranFlat(ranEngine);

// create MDT response simulation object
MDT_Response response;


double tubeMax = 14.7;
double tubeLength = 5000.;

double generateFlat() {
  return ranFlat.fire();
}

double generateRadius() {
  return tubeMax*generateFlat();
}

double generatePositionAlongTube() {
  return tubeLength*generateFlat();
}

void generateEvents( int eventMax ) {
  TH1* adcHist = new TH1F("adc","adc",400,0,200);
  TH1* driftHist = new TH1F("driftTime","driftTime",1000,0,1000);
  TH1* radiusHit = new TH1F("radiusHit","radiusHit",100,0,tubeMax);
  TH1* radiusNoHit = new TH1F("radiusNoHit","radiusNoHit",100,0,tubeMax);
  TH1* posLengthHit = new TH1F("posLengthHit","posLengthHit",100,0,tubeLength);
  TH1* posLengthNoHit = new TH1F("posLengthNoHit","posLengthNoHit",100,0,tubeLength);
  for( int i=0;i<eventMax; ++i ){
    
    // generate drift radius + position along tube
    double radius = generateRadius();
    double positionAlongTube = generatePositionAlongTube();
    response.SetSegment(radius,positionAlongTube);

    // check whether tube fired as we also simulate tube inefficiency
    bool hasHit = response.GetSignal(&ranEngine);
    if( hasHit ){
      // get the drift time (TDC count) and ADC
      double driftTime = response.DriftTime();
      double adc       = response.AdcResponse();
      adcHist->Fill(adc);
      driftHist->Fill(driftTime);
      radiusHit->Fill(radius);
      posLengthHit->Fill(positionAlongTube);
    }else{
      radiusNoHit->Fill(radius);
      posLengthNoHit->Fill(positionAlongTube);
    }
    
  }

}

int main(int argc, char* argv[] ){

  int eventMax = 20000;
  if( argc > 1 ) {
    char *endptr;
    errno = 0;
    long convArg = strtol(argv[1], &endptr, 0);
    if(errno == ERANGE || *endptr != '\0' || argv[1] == endptr) {
      std::cout<<"Invalid parameter! Quit now!"<<std::endl;
      exit(1);
    }
    // Only needed if sizeof(int) < sizeof(long)
    if(convArg < INT_MIN || convArg > INT_MAX) {
      std::cout<<"Invalid parameter! Quit now!"<<std::endl;
      exit(1);
    }
    int tempMax = (int) convArg;
    if (tempMax > 0) eventMax = tempMax; 
  } 
  
  std::cout << " Starting simulation of MDT_response, events " << eventMax << std::endl;

  TFile* outputFile = new TFile("MDT_ResponseTest.root","RECREATE");

  // simulate long chamber
  tubeLength = 5000.;
  TDirectory* dir = outputFile->mkdir("LongTubes");
  dir->cd();
  generateEvents(eventMax);

  // simulate short chamber
  tubeLength = 500.;
  dir = outputFile->mkdir("ShortTubes");
  dir->cd();
  generateEvents(eventMax);

  // simulate short chamber, large incident angle
  double increasedPathLength = 0.25; // here one could use the angle
  response.SetClusterDensity(8.5*increasedPathLength);
  dir = outputFile->mkdir("ShortTubesMediumAngle");
  dir->cd();
  generateEvents(eventMax);

//   // simulate short chamber, large incident angle
//   increasedPathLength = 4; // here one could use the angle
//   response.SetClusterDensity(8.5*increasedPathLength);
//   dir = outputFile->mkdir("ShortTubesLargeAngle");
//   dir->cd();
//   generateEvents(eventMax);

  outputFile->Write();
  outputFile->Close();

  return 0;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <utility>
#include <math.h>

/******************************************************
    @class JetFitterPtEtaCategories
********************************************************/


//using namespace std;

std::pair<int,double> getPtCategory(double Pt)
{

  double correctionfactor=1e3;//PLEASE CHANGE IT TO 1 FOR INTERNAL CODE USE (GeV)
  int actualpT=-1;
  double interval=1;
  
  if (fabs(Pt)<25*correctionfactor){
    actualpT=0;
  } else if (fabs(Pt)>=25*correctionfactor&&fabs(Pt)<35*correctionfactor) {
    actualpT=1;
//    interval=5;
  } else if (fabs(Pt)>=35*correctionfactor&&fabs(Pt)<50*correctionfactor) {
    actualpT=2;
//    interval=15;
  } else if (fabs(Pt)>=50*correctionfactor&&fabs(Pt)<80*correctionfactor) {
    actualpT=3;
//    interval=20;
  } else if (fabs(Pt)>=80*correctionfactor&&fabs(Pt)<120*correctionfactor) {
    actualpT=4;
//    interval=40;
  } else if (fabs(Pt)>=120*correctionfactor&&fabs(Pt)<200*correctionfactor) {
    actualpT=5;
//    interval=65/2;
  } else if (fabs(Pt)>=200*correctionfactor) {
    actualpT=6;
//    interval=200/6;
  }

  return std::pair<int,double>(actualpT,interval);
}

std::pair<int,double> getEtaCategory(double eta)
{

  int actualeta=-1;
  double interval=1;
  
  if (fabs(eta)>1.5){
    actualeta=2;
//    interval=2.;
  } else if (fabs(eta)>=0.7&&fabs(eta)<1.5){
    actualeta=1;
//    interval=1.;
  } else {
    actualeta=0;
//    interval=0.6;
  }
  
  return std::pair<int,double>(actualeta,interval);
}

int getNPtBins()
{
  return 7;
}

int getNEtaBins()
{
  return 3;
}

 

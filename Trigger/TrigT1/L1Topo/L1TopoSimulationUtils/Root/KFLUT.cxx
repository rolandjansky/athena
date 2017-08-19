/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// KFLUT
// contains LUT values for KF corrections to MET
//

#include<cmath>
#include "L1TopoSimulationUtils/KFLUT.h"
#include <iostream>

using namespace std;

TCS::KFLUT::KFLUT(){

   fillLUT();
}

TCS::KFLUT::~KFLUT(){

}


int TCS::KFLUT::getetabin(double eta){

   int bin = 0;
   if (eta >= etalimits.back()) return (etalimits.size()-1);
   if (eta < etalimits[0]) return 0;

   int sizelim = etalimits.size() - 1 ;
   for (int  j=0;j< sizelim ;j++) {
         if (abs(eta) >= etalimits[j] && abs(eta) < etalimits[j+1]) {
           bin = j;
         }
   }

   return bin;


}


int TCS::KFLUT::getetbin(unsigned int et){

    int binet = 0;
    if (et >= etlimits.back()) return (etlimits.size()-1);
    if (et < etlimits[0]) return 0;

    int sizelimet = etlimits.size() - 1 ;
    for (int i=0;i< sizelimet ;i++) {
        if (et >= etlimits[i] && et < etlimits[i+1]) {
           binet = i;
        }
    }
    return binet;


}


double TCS::KFLUT::getcorrKF(int iet,int jeta){

   return LUTKF.at(iet).at(jeta);


}

void TCS::KFLUT::fillLUT(){

    etalimits = {-0.10,0.10,0.30,0.50,0.70,0.90,1.10,1.30,1.50,1.70,1.90,2.10,2.33,2.55,2.80,3.42};
    etlimits = {8,16,32,64,128,1024};

    vector<double> v0 {0.14,0.12,0.12,0.17,0.26,0.3,0.3,0.42,0.38,0.19,0.16,0.15,0.52,0.59,0.62,0.33};
    vector<double> v1 {0.19,0.17,0.18,0.2,0.27,0.3,0.3,0.43,0.36,0.19,0.16,0.14,0.33,0.35,0.38,0.28};
    vector<double> v2 {0.13,0.11,0.11,0.13,0.17,0.19,0.18,0.28,0.21,0.11,0.09,0.08,0.12,0.09,0.12,0.17};
    vector<double> v3 {-0.01,-0.02,-0.02,-0.01,0.02,0.03,0.03,0.12,0.04,-0.04,-0.05,-0.06,-0.03,-0.06,-0.03,0.05};
    vector<double> v4 {-0.12,-0.12,-0.12,-0.11,-0.08,-0.07,-0.08,-0.01,-0.08,-0.14,-0.14,-0.15,-0.13,-0.15,-0.12,-0.04};

    LUTKF.push_back(v0);
    LUTKF.push_back(v1);
    LUTKF.push_back(v2);
    LUTKF.push_back(v3);
    LUTKF.push_back(v4);



}

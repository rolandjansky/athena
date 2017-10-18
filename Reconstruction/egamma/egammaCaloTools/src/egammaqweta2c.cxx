/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #include "egammaqweta2c.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <math.h>

// constants from qweta2c.F 
const float egammaqweta2c::P0A[3] = {0.0045, 0.005375, -0.0562};
const float egammaqweta2c::P1A[3] = {-0.0016, -0.0215, 0.114};
const float egammaqweta2c::P2A[3] = {-0.0866, 0.0215, -0.053};

const float egammaqweta2c::P0B[3] = {0.0039, 0.005075, -0.0324};
const float egammaqweta2c::P1B[3] = {0.00816, -0.0203, 0.0653};
const float egammaqweta2c::P2B[3] = {-0.145, 0.0203, -0.0286};

const float egammaqweta2c::P0C[3] = {0.0047, 0.0035, 0.0};
const float egammaqweta2c::P1C[3] = {-0.0184, -0.0139, 0.0};
const float egammaqweta2c::P2C[3] = {0.0180, 0.0137, 0.0};

// Constructor 
egammaqweta2c::egammaqweta2c(const std::string& type,
			     const std::string& name,
			     const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  // declare Interface
  declareInterface<Iegammaqweta2c>(this);
}


// DESTRUCTOR:

egammaqweta2c::~egammaqweta2c()
{ }

/////////////////////////////////////////////////////////////////

// INITIALIZE METHOD:
       
StatusCode egammaqweta2c::initialize()
{
  ATH_MSG_DEBUG(" Initializing egammaqweta2c");
 
  return StatusCode::SUCCESS;
}

// FINALIZE METHOD:
  
StatusCode egammaqweta2c::finalize()
{
  return StatusCode::SUCCESS;
}

float egammaqweta2c::Correct(float eta, float etacell, float weta2) const
{

//  float eta = cluster->eta(2);      // use second sampling
//  float weta2 = cluster->etasize(2);

  float aeta = fabs(eta);

  //egammaqweta2c relpos;    
  float etarel =  RelPosition(eta,etacell);
  float qweta2c;

  if (aeta < 0.8) {
    if (etarel < 0.1) {
      qweta2c = weta2 - (P0A[0] + P1A[0]*etarel + P2A[0]*etarel*etarel);
    }
    else if (etarel < 0.9) {
      qweta2c = weta2 - (P0A[1] + P1A[1]*etarel + P2A[1]*etarel*etarel);
    }
    else {
      qweta2c = weta2 - (P0A[2] + P1A[2]*etarel + P2A[2]*etarel*etarel);
    }
  }

  else if (aeta < 1.5) {
    if (etarel < 0.1) {
      qweta2c = weta2 - (P0B[0] + P1B[0]*etarel + P2B[0]*etarel*etarel);
    }
    else if (etarel < 0.9) {
      qweta2c = weta2 - (P0B[1] + P1B[1]*etarel + P2B[1]*etarel*etarel);
    }
    else {
      qweta2c = weta2 - (P0B[2] + P1B[2]*etarel + P2B[2]*etarel*etarel);
    }

  }
  else if (aeta < 1.8) {
    if (etarel < 0.1) {
      qweta2c = weta2 - (P0B[0] + P1B[0]*etarel + P2B[0]*etarel*etarel);
    }
    else if (etarel < 0.9) {
      qweta2c = weta2 - (P0B[1] + P1B[1]*etarel + P2B[1]*etarel*etarel);
    }
    else {
      qweta2c = weta2 - (P0B[2] + P1B[2]*etarel + P2B[2]*etarel*etarel);
    }
  }
  else if (aeta < 2.0) {
    qweta2c = weta2 - (P0C[0] + P1C[0]*etarel + P2C[0]*etarel*etarel);
  }

  else if (aeta < 2.5) {
    qweta2c = weta2 - (P0C[1] + P1C[1]*etarel + P2C[1]*etarel*etarel);
  }

  else {
    qweta2c = weta2;
  }

  return qweta2c;
}

// =====================================================================
double egammaqweta2c::RelPosition(float eta, float etacell) const
{

//  float eta = cluster->eta(2);      // use second sampling
//  float weta2 = cluster->etasize(2);


  // need etarel = mod(aeta,0.025) : Relative distance from cell edge
  //  float aeta = fabs(eta);
  if (eta == -999.) return -999;
  //  float aetacell = fabs(etacell);
  //  float etarel = fmod((double)aeta,0.025)/0.025;
  // position within cell (-cellsize/2 - cellsize/2)
  double x = fabs(eta-etacell-0.025/2.);
  double g = 0.025;
  //  float etarel = fmod(aeta-aetacell, 0.025/2.)/0.025;
  double etarel = fmod(x,g)/(double)0.025;
  //  etarel += 0.5;
  return etarel;
}


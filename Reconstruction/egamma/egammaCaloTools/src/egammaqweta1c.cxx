/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaqweta1c.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <math.h>

// Constructor 
egammaqweta1c::egammaqweta1c(const std::string& type,
			     const std::string& name,
			     const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  // declare Interface
  declareInterface<Iegammaqweta1c>(this);
}


// DESTRUCTOR:

egammaqweta1c::~egammaqweta1c()
{ }

/////////////////////////////////////////////////////////////////

// INITIALIZE METHOD:
       
StatusCode egammaqweta1c::initialize()
{
  ATH_MSG_DEBUG(" Initializing egammaqweta1c");
 
  return StatusCode::SUCCESS;
}

// =====================================================================
// FINALIZE METHOD:
StatusCode egammaqweta1c::finalize()
{
  return StatusCode::SUCCESS;
}

// ===================================================================
float egammaqweta1c::Correct(float eta, float etacell, float width) const
{

  //   Make correction to the width of the cluster shower in sampling 1
 
  float aeta = fabs(eta);
  float qweta1c;
  //egammaqweta1c relpos;    
  float etarel =  RelPosition(eta,etacell);

  if (aeta < 1.0) {
    qweta1c = width - 0.76 * pow(etarel,2);
  }

  else if (aeta < 1.45) {
    qweta1c = width - 0.85*pow(etarel,2) + 1.9*pow(etarel,4);
  }

  else if (aeta < 1.5) {
    qweta1c = width;
  }
  else if (aeta < 1.8) {
    qweta1c = width - 0.85*pow(etarel,2) + 1.9*pow(etarel,4);
  }
  else if (aeta < 2.0) {
    qweta1c = width - 0.84*pow(etarel,2);
  }

  else if (aeta < 2.5) {
    qweta1c = width - 0.40*pow(etarel,2) - 2.1*pow(etarel,4);
  }

  else {
    qweta1c = width;
  }

  return qweta1c; 
}

// ===============================================================
double egammaqweta1c::RelPosition(float eta, float etacell) const
{
  //
  //   get relative position within cell in eta of the cluster 
  //   shower in sampling 1
  //

  float aeta = fabs(eta);
  //  float aetacell = fabs(etacell);
  double etarel;

  if (aeta < 1.8) {
    float ngra = 8.;
    double dgra = 0.025/ngra;
    double etapos = fabs(eta-etacell-dgra/2.);
    etarel = fmod(etapos,dgra)/dgra - 0.5;
    //    cout << "etarel " << etapos << " , " << etarel << endl;
  }

  else if (aeta < 2.0) {
    float ngra = 6.;
    float dgra = 0.025/ngra;
    double etapos = fabs(eta-etacell-dgra/2.);
    etarel = fmod(etapos,dgra)/dgra - 0.5;
    //    cout << "etarel " << etapos << " , " << etarel << endl;
  }

  else if (aeta < 2.4) {
    float ngra = 4.;
    float dgra = 0.025/ngra;
    double etapos = fabs(eta-etacell-dgra/2.);
    etarel = fmod(etapos,dgra)/dgra - 0.5;
    //    cout << "etarel " << etapos << " , " << etarel << endl;
  }

  else if (aeta < 2.5) {
    float ngra = 1.;
    float dgra = 0.025/ngra;
    double etapos = fabs(eta-etacell-dgra/2.);
    etarel = fmod(etapos,dgra)/dgra - 0.5;
  }

  else {
    etarel = -999.;
  }

  return etarel; 
}



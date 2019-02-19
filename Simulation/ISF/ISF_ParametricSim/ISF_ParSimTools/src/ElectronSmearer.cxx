/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ElectronSmearer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_ParSimTools/ElectronSmearer.h"
// ISF
#include "ISF_Event/ISFParticle.h"
// Trk

//================ Constructor =================================================
iParSim::ElectronSmearer::ElectronSmearer(const std::string& t, const std::string& n, const IInterface*  p ) :
  base_class(t,n,p)
{
}

//================ Destructor =================================================

iParSim::ElectronSmearer::~ElectronSmearer()
{}

//================ Initialisation =================================================
StatusCode iParSim::ElectronSmearer::initialize()
{    
    ATH_MSG_VERBOSE( "initialize() successful." );
    return StatusCode::SUCCESS;
}


/** Creates a new ISFParticle from a given ParticleState, universal transport tool */
bool iParSim::ElectronSmearer::smear(xAOD::TrackParticle* xaodTP, CLHEP::HepRandomEngine*) const
{
  
  // DEFINING PARAMETERS COVARIANCE MATRIX //
  static const float covMatrix[ 15 ] = {
      11., 0., 11., 0., 0.,
      11., 0., 0., 0., 11.,
      0., 0., 0., 0., 11.
    };
    static const std::vector< float > covMatrixVec( covMatrix, covMatrix + 15 );
    //std::vector<float> covMatrixVec(15,0); for(int i=0; i<15; i++)covMatrixVec[i]=0.0;
  xaodTP->setDefiningParametersCovMatrixVec(covMatrixVec);

  return true;
}

unsigned int iParSim::ElectronSmearer::pdg() const 
{
  return 11;
}

StatusCode iParSim::ElectronSmearer::finalize()
{ 
    ATH_MSG_VERBOSE( "finalize() successful " );
    return StatusCode::SUCCESS;
}





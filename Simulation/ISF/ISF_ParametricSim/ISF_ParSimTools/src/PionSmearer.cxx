/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PionSmearer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_ParSimTools/PionSmearer.h"
// ISF
#include "ISF_Event/ISFParticle.h"
// Trk

//================ Constructor =================================================
iParSim::PionSmearer::PionSmearer(const std::string& t, const std::string& n, const IInterface*  p ) :
  AthAlgTool(t,n,p)
{
    declareInterface<iParSim::IChargedSmearer>(this);
}

//================ Destructor =================================================

iParSim::PionSmearer::~PionSmearer()
{}

//================ Initialisation =================================================
StatusCode iParSim::PionSmearer::initialize()
{    
    ATH_MSG_VERBOSE( "initialize() successful." );
    return StatusCode::SUCCESS;
}


/** Creates a new ISFParticle from a given ParticleState, universal transport tool */
bool iParSim::PionSmearer::smear(xAOD::TrackParticle* xaodTP) const
{
  
  // DEFINING PARAMETERS COVARIANCE MATRIX //
  static const float covMatrix[ 15 ] = {
      211., 0., 211., 0., 0.,
      211., 0., 0., 0., 211.,
      0., 0., 0., 0., 211.
    };
    static const std::vector< float > covMatrixVec( covMatrix, covMatrix + 15 );
    //std::vector<float> covMatrixVec(15,0); for(int i=0; i<15; i++)covMatrixVec[i]=0.0;
  xaodTP->setDefiningParametersCovMatrixVec(covMatrixVec);

  return true;
}

unsigned int iParSim::PionSmearer::pdg() const 
{
  return 211;
}

StatusCode iParSim::PionSmearer::finalize()
{ 
    ATH_MSG_VERBOSE( "finalize() successful " );
    return StatusCode::SUCCESS;
}





/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PileupSimSelector.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "ISF_SimulationSelectors/PileupSimSelector.h"
// HepMC includes
#include "ISF_Event/ISFParticle.h"
// Units includes
#include "GaudiKernel/SystemOfUnits.h"

/** Constructor **/
ISF::PileupSimSelector::PileupSimSelector(const std::string& t, const std::string& n, const IInterface* p) : 
  ISimulationSelector(t,n,p)
{
    declareInterface<ISF::ISimulationSelector>(this);
}

/** Destructor **/
ISF::PileupSimSelector::~PileupSimSelector()
{
}

// Athena algtool's Hooks
StatusCode  ISF::PileupSimSelector::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");
    return StatusCode::SUCCESS;
}

StatusCode  ISF::PileupSimSelector::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}

/** check whether given particle passes all cuts -> will be used for routing decision*/
bool  ISF::PileupSimSelector::passSelectorCuts(const ISFParticle& particle) const
{
  Barcode::ParticleBarcode barcode = particle.barcode();

  int bcid = m_bitcalculator.GetBCID( barcode );

  ATH_MSG_VERBOSE( "[fatras pileupselector] barcode = " << barcode << " bcid = " << bcid 
                   << " extra = " << ( particle.getUserInformation()!=0 ? particle.getUserInformation()->getExtraBC() : -1 ) );

  //if ( bcid!=0 ) { return false; }

  return true;
}


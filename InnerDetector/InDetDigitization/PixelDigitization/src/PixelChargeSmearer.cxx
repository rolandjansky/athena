/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelChargeSmearer.cxx
//   Implementation file for class PixelChargeSmearer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelChargeSmearer.h"
#include "SiDigitization/SiChargedDiodeCollection.h"

#include "CLHEP/Random/RandomEngine.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

static const InterfaceID IID_IPixelChargeSmearer("PixelChargeSmearer", 1, 0);
const InterfaceID& PixelChargeSmearer::interfaceID( ){ return IID_IPixelChargeSmearer; }

// Constructor with parameters:
PixelChargeSmearer::PixelChargeSmearer(const std::string& type, const std::string& name,const IInterface* parent) :
  AthAlgTool(type,name,parent),
  m_amplitude(0),
  m_rndmSvc("AtDSFMTGenSvc",name),
  m_rndmEngineName("PixelDigitization"),
  m_rndmEngine(0)
{  
	declareInterface< PixelChargeSmearer >( this );
	declareProperty("RndmSvc",m_rndmSvc,"Random Number Service used in Pixel digitization");
	declareProperty("RndmEngine",m_rndmEngineName,"Random engine name");
	declareProperty("ThermalNoise",m_amplitude,"Thermal noise amplitude");
}

// Destructor:
PixelChargeSmearer::~PixelChargeSmearer()
{}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------

StatusCode PixelChargeSmearer::initialize() {
  StatusCode sc = AthAlgTool::initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelChargeSmearer::initialize() failed");
    return sc ;
  }
  if (m_rndmSvc.retrieve().isFailure()) {
	ATH_MSG_ERROR("Can't get RndmSvc");
	return StatusCode::FAILURE;
  }
  else {
	ATH_MSG_DEBUG("Retrieved RndmSvc");
  }
 
  ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
	ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
	return StatusCode::FAILURE;
  } else { 
	ATH_MSG_DEBUG ( " Found RndmEngine : " << m_rndmEngineName ); 
  }

  ATH_MSG_DEBUG ( "PixelChargeSmearer::initialize()");
  return sc ;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode PixelChargeSmearer::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelChargeSmearer::finalize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "PixelChargeSmearer::finalize()");
  return sc ;
}

// process the list of charged diodes
void PixelChargeSmearer::process(SiChargedDiodeCollection &collection) const
{
  // if the noise amplitude if 0, do nothing
  if (0==m_amplitude) return;

  // loop on all charged diodes
  for(SiChargedDiodeIterator i_chargedDiode=collection.begin();
      i_chargedDiode!=collection.end();
      ++i_chargedDiode) {

    // create noise charge
    SiCharge charge(m_amplitude*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine),0,SiCharge::noise);

    // add it to the charged diode
    (*i_chargedDiode).second.add(charge);
  }
}

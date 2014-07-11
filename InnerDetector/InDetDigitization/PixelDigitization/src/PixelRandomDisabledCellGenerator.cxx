/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// PixelRandomDisabledCellGenerator.cxx 
//   Implementation file for class PixelRandomDisabledCellGenerator 
/////////////////////////////////////////////////////////////////// 
// (c) ATLAS Detector software 
/////////////////////////////////////////////////////////////////// 

#include "PixelRandomDisabledCellGenerator.h"
#include "SiDigitization/SiHelper.h"

// random number service
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"

#include "CLHEP/Random/RandomEngine.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"

static const InterfaceID IID_IPixelRandomDisabledCellGenerator("PixelRandomDisabledCellGenerator", 1, 0);
const InterfaceID& PixelRandomDisabledCellGenerator::interfaceID( ){ return IID_IPixelRandomDisabledCellGenerator; }

// Constructor with parameters:
PixelRandomDisabledCellGenerator::PixelRandomDisabledCellGenerator(const std::string& type, const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_myfunc(0),
  m_rndmSvc("AtDSFMTGenSvc",name),
  m_rndmEngineName("PixelDigitization"),
  m_rndmEngine(0),
  m_disableProbability(9e-3)
{
  declareInterface< PixelRandomDisabledCellGenerator >( this ); 
  declareProperty("RndmSvc",m_rndmSvc,"Random Number Service used in Pixel digitization");
  declareProperty("RndmEngine",m_rndmEngineName,"Random engine name");
  declareProperty("TotalBadChannels",m_disableProbability);
 
}

// Destructor:
PixelRandomDisabledCellGenerator::~PixelRandomDisabledCellGenerator()
{}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode PixelRandomDisabledCellGenerator::initialize() {
  StatusCode sc = AthAlgTool::initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelRandomDisabledCellGenerator::initialize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "PixelRandomDisabledCellGenerator::initialize()");
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

  m_myfunc = SiHelper::disabled;

  ATH_MSG_INFO ( "\tCreating disabled cell generator with "<<m_disableProbability<<" probability");

  return sc ;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode PixelRandomDisabledCellGenerator::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelRandomDisabledCellGenerator::finalize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "PixelRandomDisabledCellGenerator::finalize()");
  return sc ;
}

// process the collection of charged diodes
void PixelRandomDisabledCellGenerator::process(SiChargedDiodeCollection &collection) const
{   
  // disabling is applied to all cells even unconnected or below threshold
  // ones to be able to use these cells as well

  for(SiChargedDiodeIterator i_chargedDiode=collection.begin() ;
      i_chargedDiode!=collection.end() ; ++i_chargedDiode) {

    // disable the diode randomly
    if(CLHEP::RandFlat::shoot(m_rndmEngine)<m_disableProbability) (*m_myfunc)((*i_chargedDiode).second,true,false);

  }
}

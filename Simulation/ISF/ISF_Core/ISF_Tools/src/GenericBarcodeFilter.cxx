/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericBarcodeFilter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "GenericBarcodeFilter.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

// ISF interfaces
#include "ISF_Interfaces/IParticleFilter.h"

/** Constructor **/
ISF::GenericBarcodeFilter::GenericBarcodeFilter(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_filterOutUnsetBarcodes(true),
  m_onlyLegacyPrimaries(false),
  m_legacyPartGenerationIncrement(1000000),
  m_legacyFirstSecondary(200001)
{
  declareProperty( "AllowOnlyDefinedBarcodes",
                   m_filterOutUnsetBarcodes=true,
                   "Filter out all particles with Barcode::fUndefinedBarcode");
  declareProperty( "AllowOnlyLegacyPrimaries",
                   m_onlyLegacyPrimaries=false,
                   "Turn the filtering of primary particle barcodes on/off (according to MC12 barcode scheme)");
  declareProperty( "LegacyParticleGenerationIncrement",
                   m_legacyPartGenerationIncrement=1000000,
                   "The barcode increments by this number after each non-destructive interaction.");
  declareProperty( "LegacyFirstSecondaryBarcode",
                   m_legacyFirstSecondary=200001,
                   "The first secondary particle produced by the simulation will have this barcode");

}

/** Destructor **/
ISF::GenericBarcodeFilter::~GenericBarcodeFilter()
{
}

/** Athena algtool's Hooks */
StatusCode  ISF::GenericBarcodeFilter::initialize()
{
  ATH_MSG_INFO("initialize() ...");

  ATH_MSG_INFO("initialize() successful");
  return StatusCode::SUCCESS;
}


/** Athena algtool's Hooks */
StatusCode  ISF::GenericBarcodeFilter::finalize()
{
  ATH_MSG_INFO("finalize() ...");

  ATH_MSG_INFO("finalize() successful");
  return StatusCode::SUCCESS;
}


/** does the particle pass the this filter? */
bool ISF::GenericBarcodeFilter::passFilter(const ISFParticle &p) const {
  // the particle barcode
  Barcode::ParticleBarcode bCode = p.barcode();

  // if filtering of unset particle barcodes is turned on -> check barcode
  bool pass = (!m_filterOutUnsetBarcodes) || bCode!=Barcode::fUndefinedBarcode;

  // check whether it's a primary particle
  if (pass && m_onlyLegacyPrimaries) {
    // get the barcode of this particle before the first interaction
    Barcode::ParticleBarcode unincrementedBCode = bCode % m_legacyPartGenerationIncrement;
    // check whether the particle has a BC smaller than the first one created by the simulator
    pass = unincrementedBCode < m_legacyFirstSecondary;
  }

  return pass;
}

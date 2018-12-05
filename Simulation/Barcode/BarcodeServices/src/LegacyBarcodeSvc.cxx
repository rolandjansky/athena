/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LegacyBarcodeSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "BarcodeServices/LegacyBarcodeSvc.h"
// framework include


/** Constructor **/
Barcode::LegacyBarcodeSvc::LegacyBarcodeSvc(const std::string& name,ISvcLocator* svc) :
  base_class(name,svc),
  m_bitcalculator(new Barcode::BitCalculator()),
  m_firstVertex(-200001),
  m_vertexIncrement(-1),
  m_currentVertex(-1),
  m_firstSecondary(200001),
  m_secondaryIncrement(1),
  m_currentSecondary(1),
  m_particleGenerationIncrement(1000000),
  m_doUnderOverflowChecks(true)
{
  // python properties
  declareProperty("FirstSecondaryVertexBarcode",  m_firstVertex=-200001                );
  declareProperty("VertexIncrement"            ,  m_vertexIncrement=-1                 );
  declareProperty("FirstSecondaryBarcode"      ,  m_firstSecondary=200001              );
  declareProperty("SecondaryIncrement"         ,  m_secondaryIncrement=1               );
  declareProperty("ParticleGenerationIncrement",  m_particleGenerationIncrement=1000000);
  declareProperty("DoUnderAndOverflowChecks"   ,  m_doUnderOverflowChecks=true         );
}


Barcode::LegacyBarcodeSvc::~LegacyBarcodeSvc()
{
  delete m_bitcalculator;
}


/** framework methods */
StatusCode Barcode::LegacyBarcodeSvc::initialize()
{
  ATH_MSG_VERBOSE ("initialize() ...");
  ATH_MSG_DEBUG( "LegacyBarcodeSvc start of initialize in thread ID: " << std::this_thread::get_id() );

  ATH_CHECK( this->initializeBarcodes() );

  ATH_MSG_VERBOSE ("initialize() successful");
  return StatusCode::SUCCESS;
}

StatusCode Barcode::LegacyBarcodeSvc::initializeBarcodes() {
    static std::mutex barcodeMutex;
    std::lock_guard<std::mutex> barcodeLock(barcodeMutex);
    ATH_MSG_DEBUG( name() << "::initializeBarcodes()" );

  // look for pair containing barcode info using the thread ID
  // if it doesn't exist, construct one and insert it.
  const auto tid = std::this_thread::get_id();
  auto bcPair = m_bcThreadMap.find(tid);
  if ( bcPair == m_bcThreadMap.end() ) {
      auto result = m_bcThreadMap.insert( std::make_pair( tid, BarcodeInfo(m_currentVertex, m_currentSecondary)) );
      if (result.second) {
          ATH_MSG_DEBUG( "initializeBarcodes: initialized new barcodes for thread ID " << tid );
          ATH_CHECK( this->resetBarcodes() );
          ATH_MSG_DEBUG( "initializeBarcodes: reset new barcodes for thread ID " << tid );
      } else {
          ATH_MSG_ERROR( "initializeBarcodes: failed to initialize new barcode for thread ID " << tid );
      }
  } else {
      ATH_MSG_DEBUG( "initializeBarcodes: barcodes for this thread ID found, did not construct new" );
      ATH_CHECK( this->resetBarcodes() );
      ATH_MSG_DEBUG( "initializeBarcodes: reset existing barcodes for thread ID " << tid );
  }
  return StatusCode::SUCCESS;
}

//FIXME this should return an optional type, since returning the value of the end iterator is undefined behaviour (I think it causes a segfault).
Barcode::LegacyBarcodeSvc::BarcodeInfo& Barcode::LegacyBarcodeSvc::getBarcodeInfo() const {
    const auto tid = std::this_thread::get_id();
    auto bcPair = m_bcThreadMap.find(tid);
    if ( bcPair == m_bcThreadMap.end() ) {
        ATH_MSG_ERROR( "getBarcodeInfo: failed to get BarcodeInfo for thread ID " << tid );
        return bcPair->second;
    } else {
        return bcPair->second;
    }
}

/** Generate a new unique vertex barcode, based on the parent particle barcode and
    the physics process code causing the truth vertex*/
Barcode::VertexBarcode Barcode::LegacyBarcodeSvc::newVertex( Barcode::ParticleBarcode /* parent */,
                                                             Barcode::PhysicsProcessCode /* process */)
{
  BarcodeInfo& bc = getBarcodeInfo();
  bc.currentVertex += m_vertexIncrement;
  // a naive underflog checking based on the fact that vertex
  // barcodes should never be positive
  if ( m_doUnderOverflowChecks && (bc.currentVertex > 0))
    {
      ATH_MSG_ERROR("LegacyBarcodeSvc::newVertex(...)"
                    << " will return a vertex barcode greater than 0: "
                    << bc.currentVertex << ". Possibly Integer Underflow?");
    }

  return bc.currentVertex;
}


/** Generate a new unique barcode for a secondary particle, based on the parent
    particle barcode and the process code of the physics process that created
    the secondary  */
Barcode::ParticleBarcode Barcode::LegacyBarcodeSvc::newSecondary( Barcode::ParticleBarcode /* parentBC */,
                                                                  Barcode::PhysicsProcessCode /* process */)
{
  BarcodeInfo& bc = getBarcodeInfo();
  bc.currentSecondary += m_secondaryIncrement;
  // a naive overflow checking based on the fact that particle
  // barcodes should never be negative
  if ( m_doUnderOverflowChecks && (bc.currentSecondary < 0))
    {
      ATH_MSG_DEBUG("LegacyBarcodeSvc::newSecondary(...)"
                    << " will return a particle barcode of less than 0: "
                    << bc.currentSecondary << ". Reset to zero.");

      bc.currentSecondary = Barcode::fUndefinedBarcode;
    }

  return bc.currentSecondary;
}


/** Generate a common barcode which will be shared by all children
    of the given parent barcode (used for child particles which are
    not stored in the mc truth event) */
Barcode::ParticleBarcode Barcode::LegacyBarcodeSvc::sharedChildBarcode( Barcode::ParticleBarcode /* parentBC */,
                                                                        Barcode::PhysicsProcessCode /* process */)
{
  // concept of shared barcodes not present in MC12 yet
  return Barcode::fUndefinedBarcode;
}


/** Update the given barcode (e.g. after an interaction) */
Barcode::ParticleBarcode Barcode::LegacyBarcodeSvc::incrementBarcode( Barcode::ParticleBarcode old,
                                                                      Barcode::PhysicsProcessCode /* process */)
{
  Barcode::ParticleBarcode newBC = old + m_particleGenerationIncrement;
  // a naive overflow checking based on the fact that particle
  // barcodes should never be negative
  if ( m_doUnderOverflowChecks && (newBC < 0))
    {
      ATH_MSG_DEBUG("LegacyBarcodeSvc::incrementBarcode('" << old << "')"
                    << " will return a particle barcode of less than 0: "
                    << newBC << ". Reset to zero.");
      newBC = Barcode::fUndefinedBarcode;
    }
  return newBC;
}


void Barcode::LegacyBarcodeSvc::registerLargestGenEvtParticleBC( Barcode::ParticleBarcode /* bc */) {
}


void Barcode::LegacyBarcodeSvc::registerLargestGenEvtVtxBC( Barcode::VertexBarcode /* bc */) {
}


/** Return the secondary particle offset */
Barcode::ParticleBarcode Barcode::LegacyBarcodeSvc::secondaryParticleBcOffset() const {
  return m_firstSecondary;
}


/** Return the secondary vertex offset */
Barcode::VertexBarcode Barcode::LegacyBarcodeSvc::secondaryVertexBcOffset() const {
  return m_firstVertex;
}


/** Return the barcode increment for each generation of updated particles */
Barcode::ParticleBarcode Barcode::LegacyBarcodeSvc::particleGenerationIncrement() const
{
  return m_particleGenerationIncrement;
}

StatusCode Barcode::LegacyBarcodeSvc::resetBarcodes()
{
    ATH_MSG_DEBUG( "resetBarcodes: resetting barcodes" );
    BarcodeInfo& bc = getBarcodeInfo();
    bc.currentVertex    = m_firstVertex    - m_vertexIncrement;
    bc.currentSecondary = m_firstSecondary - m_secondaryIncrement;
    return StatusCode::SUCCESS;
}


/** framework methods */
StatusCode Barcode::LegacyBarcodeSvc::finalize()
{
  ATH_MSG_VERBOSE ("finalize() ...");
  ATH_MSG_VERBOSE ("finalize() successful");
  return StatusCode::SUCCESS;
}

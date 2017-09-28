/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ValidationBarcodeSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "BarcodeServices/ValidationBarcodeSvc.h"
// framework include
#include "GaudiKernel/IIncidentSvc.h"


/** Constructor **/
Barcode::ValidationBarcodeSvc::ValidationBarcodeSvc(const std::string& name,ISvcLocator* svc) :
  base_class(name,svc),
  m_incidentSvc("IncidentSvc", name),
  m_firstVertex(-200001),
  m_vertexIncrement(-1),
  m_currentVertex(-1),
  m_firstSecondary(200001),
  m_secondaryIncrement(1),
  m_currentSecondary(1),
  m_particleGenerationIncrement(1000000),
  m_barcodeGenerationOffset(1e8),
  m_doUnderOverflowChecks(true)
{
  // python properties
  declareProperty("FirstSecondaryVertexBarcode",  m_firstVertex=-200001                );
  declareProperty("VertexIncrement"            ,  m_vertexIncrement=-1                 );
  declareProperty("FirstSecondaryBarcode"      ,  m_firstSecondary=200001              );
  declareProperty("SecondaryIncrement"         ,  m_secondaryIncrement=1               );
  declareProperty("ParticleGenerationIncrement",  m_particleGenerationIncrement=1000000);
  declareProperty("BarcodeGenerationOffset"    ,  m_barcodeGenerationOffset=1e8        );
  declareProperty("DoUnderAndOverflowChecks"   ,  m_doUnderOverflowChecks=true         );
}


Barcode::ValidationBarcodeSvc::~ValidationBarcodeSvc()
{}


/** framework methods */
StatusCode Barcode::ValidationBarcodeSvc::initialize()
{
  ATH_MSG_VERBOSE ("initialize() ...");

  CHECK( m_incidentSvc.retrieve() );

  // register to the incident service: BeginEvent needed for refresh of counter
  m_incidentSvc->addListener( this, IncidentType::BeginEvent);

  ATH_MSG_VERBOSE ("initialize() successful");
  return StatusCode::SUCCESS;
}


/** Generate a new unique vertex barcode, based on the parent particle barcode and
    the physics process code causing the truth vertex*/
Barcode::VertexBarcode Barcode::ValidationBarcodeSvc::newVertex( Barcode::ParticleBarcode /* parent */,
                                                                 Barcode::PhysicsProcessCode /* process */)
{
  m_currentVertex += m_vertexIncrement;
  // a naive underflog checking based on the fact that vertex
  // barcodes should never be positive
  if ( m_doUnderOverflowChecks && (m_currentVertex > 0))
    {
      ATH_MSG_ERROR("ValidationBarcodeSvc::newVertex(...)"
                    << " will return a vertex barcode greater than 0: "
                    << m_currentVertex << ". Possibly Integer Underflow?");
    }
  return m_currentVertex;
}


/** Generate a new unique barcode for a secondary particle, based on the parent
    particle barcode and the process code of the physics process that created
    the secondary  */
Barcode::ParticleBarcode Barcode::ValidationBarcodeSvc::newSecondary( Barcode::ParticleBarcode parentBC,
                                                                      Barcode::PhysicsProcessCode /* process */)
{
  m_currentSecondary += m_secondaryIncrement;
  // a naive overflow checking based on the fact that particle
  // barcodes should never be negative
  if ( m_doUnderOverflowChecks && (m_currentSecondary < 0))
    {
      ATH_MSG_ERROR("ValidationBarcodeSvc::newSecondary(...)"
                    << " will return a particle barcode of less than 0: "
                    << m_currentSecondary << ". Possibly Integer Overflow?");
    }
  int primGen = int( (parentBC-m_particleGenerationIncrement) / m_barcodeGenerationOffset);
  Barcode::ParticleBarcode returnBC = m_currentSecondary + (primGen+1)*m_barcodeGenerationOffset;

  return returnBC;
}


/** Generate a common barcode which will be shared by all children
    of the given parent barcode (used for child particles which are
    not stored in the mc truth event) */
Barcode::ParticleBarcode Barcode::ValidationBarcodeSvc::sharedChildBarcode( Barcode::ParticleBarcode parentBC,
                                                                            Barcode::PhysicsProcessCode /* process */)
{
  return parentBC+m_barcodeGenerationOffset;
}


/** Update the given barcode (e.g. after an interaction) */
Barcode::ParticleBarcode Barcode::ValidationBarcodeSvc::incrementBarcode( Barcode::ParticleBarcode old,
                                                                          Barcode::PhysicsProcessCode /* process */)
{
  Barcode::ParticleBarcode newBC = old + m_particleGenerationIncrement;
  // a naive overflow checking based on the fact that particle
  // barcodes should never be negative
  if ( m_doUnderOverflowChecks && (newBC < 0))
    {
      ATH_MSG_ERROR("ValidationBarcodeSvc::incrementBarcode('" << old << "')"
                    << " will return a particle barcode of less than 0: "
                    << newBC << ". Possibly Integer Overflow?");
    }
  return (newBC);
}


void Barcode::ValidationBarcodeSvc::registerLargestGenEvtParticleBC( Barcode::ParticleBarcode /* bc */)
{
}


void Barcode::ValidationBarcodeSvc::registerLargestGenEvtVtxBC( Barcode::VertexBarcode /* bc */)
{
}


/** Return the secondary particle offset */
Barcode::ParticleBarcode Barcode::ValidationBarcodeSvc::secondaryParticleBcOffset() const
{
  return m_firstSecondary;
}


/** Return the secondary vertex offset */
Barcode::VertexBarcode Barcode::ValidationBarcodeSvc::secondaryVertexBcOffset() const
{
  return m_firstVertex;
}


/** Return the barcode increment for each generation of updated particles */
Barcode::ParticleBarcode Barcode::ValidationBarcodeSvc::particleGenerationIncrement() const
{
  return m_particleGenerationIncrement;
}


/** Handle incident */
void Barcode::ValidationBarcodeSvc::handle(const Incident& inc)
{
  if ( inc.type() == IncidentType::BeginEvent )
    {
      ATH_MSG_VERBOSE("'BeginEvent' incident caught. Resetting Vertex and Particle barcode counters.");
      m_currentVertex    = m_firstVertex    - m_vertexIncrement;
      m_currentSecondary = m_firstSecondary - m_secondaryIncrement;
    }
}


/** framework methods */
StatusCode Barcode::ValidationBarcodeSvc::finalize()
{
  ATH_MSG_VERBOSE ("finalize() ...");
  ATH_MSG_VERBOSE ("finalize() successful");
  return StatusCode::SUCCESS;
}

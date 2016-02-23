/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericBarcodeSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "BarcodeServices/GenericBarcodeSvc.h"
// framework include
#include "GaudiKernel/IIncidentSvc.h"


/** Constructor **/
Barcode::GenericBarcodeSvc::GenericBarcodeSvc(const std::string& name,ISvcLocator* svc) :
  AthService(name,svc),
  m_incidentSvc("IncidentSvc", name),
  m_firstVertex(-1000001),
  m_vertexIncrement(-1),
  m_currentVertex(-1),
  m_firstSecondary(1000001),
  m_secondaryIncrement(1),
  m_currentSecondary(1),
  m_particleRegenerationIncrement(10000000),
  m_doUnderOverflowChecks(true),
  m_encodePhysicsProcess(false)
{
  // python properties
  declareProperty("FirstSecondaryVertexBarcode"   ,  m_firstVertex=-1000001                  );
  declareProperty("VertexIncrement"               ,  m_vertexIncrement=-1                    );
  declareProperty("FirstSecondaryBarcode"         ,  m_firstSecondary=1000001                );
  declareProperty("SecondaryIncrement"            ,  m_secondaryIncrement=1                  );
  declareProperty("ParticleRegenerationIncrement" ,  m_particleRegenerationIncrement=10000000);
  declareProperty("DoUnderAndOverflowChecks"      ,  m_doUnderOverflowChecks=true            );
  declareProperty("EncodePhysicsProcessInVertexBC",  m_encodePhysicsProcess=false            );
}


Barcode::GenericBarcodeSvc::~GenericBarcodeSvc()
{}


/** framework methods */
StatusCode Barcode::GenericBarcodeSvc::initialize()
{
  ATH_MSG_VERBOSE ("initialize() ...");

  CHECK( m_incidentSvc.retrieve());

  // register to the incident service: BeginEvent needed for refresh of counter
  m_incidentSvc->addListener( this, IncidentType::BeginEvent);

  ATH_MSG_VERBOSE ("initialize() successful");
  return StatusCode::SUCCESS;
}


/** Generate a new unique vertex barcode, based on the parent particle barcode and
    the physics process code causing the truth vertex*/
Barcode::VertexBarcode Barcode::GenericBarcodeSvc::newVertex( Barcode::ParticleBarcode /* parent */,
                                                              Barcode::PhysicsProcessCode process )
{
  // update the internal vertex BC counter
  m_currentVertex += m_vertexIncrement;

  // the barcode that will be returned
  Barcode::VertexBarcode newBC = m_currentVertex;

  // if enabled, put the physics process code into the vertex barcode
  if (m_encodePhysicsProcess)
    {
      newBC = newBC - process;
      // an example vertex BC would be (8th vtx, process #1234):  -8201234
    }

  // a naive underflog checking based on the fact that vertex
  // barcodes should never be positive
  if ( m_doUnderOverflowChecks && ( newBC > 0))
    {
      ATH_MSG_ERROR("LegacyBarcodeSvc::newVertex(...)"
                    << " will return a vertex barcode greater than 0: '"
                    << m_currentVertex << "'. Possibly Integer Underflow?");
    }

  return newBC;
}


/** Generate a new unique barcode for a secondary particle, based on the parent
    particle barcode and the process code of the physics process that created
    the secondary  */
Barcode::ParticleBarcode Barcode::GenericBarcodeSvc::newSecondary( Barcode::ParticleBarcode /* parentBC */,
                                                                   Barcode::PhysicsProcessCode /* process */)
{
  m_currentSecondary += m_secondaryIncrement;

  // a naive overflow checking based on the fact that particle
  // barcodes should never be negative
  if ( m_doUnderOverflowChecks && (m_currentSecondary < 0))
    {
      ATH_MSG_ERROR("LegacyBarcodeSvc::newSecondary(...)"
                    << " will return a particle barcode of less than 0: '"
                    << m_currentSecondary << "'. Reset to zero.");
      m_currentSecondary = Barcode::fUndefinedBarcode;
    }

  return m_currentSecondary;
}


/** Generate a common barcode which will be shared by all children
    of the given parent barcode (used for child particles which are
    not stored in the mc truth event) */
Barcode::ParticleBarcode Barcode::GenericBarcodeSvc::sharedChildBarcode( Barcode::ParticleBarcode /* parentBC */,
                                                                         Barcode::PhysicsProcessCode /* process */)
{
  // concept of shared barcodes not supported here yet
  return Barcode::fUndefinedBarcode;
}


/** Update the given barcode (e.g. after an interaction) */
Barcode::ParticleBarcode Barcode::GenericBarcodeSvc::incrementBarcode( Barcode::ParticleBarcode old,
                                                                       Barcode::PhysicsProcessCode /* process */)
{
  Barcode::ParticleBarcode newBC = old + m_particleRegenerationIncrement;

  // a naive overflow checking based on the fact that particle
  // barcodes should never be negative
  if ( m_doUnderOverflowChecks && (newBC < 0))
    {
      ATH_MSG_ERROR("LegacyBarcodeSvc::incrementBarcode('" << old << "')"
                    << " will return a particle barcode of less than 0: '"
                    << newBC << "'. Reset to zero.");
      newBC = Barcode::fUndefinedBarcode;
    }

  return newBC;
}


void Barcode::GenericBarcodeSvc::registerLargestGenEvtParticleBC( Barcode::ParticleBarcode /* bc */)
{
}


void Barcode::GenericBarcodeSvc::registerLargestGenEvtVtxBC( Barcode::VertexBarcode /* bc */)
{
}


/** Return the secondary particle offset */
Barcode::ParticleBarcode Barcode::GenericBarcodeSvc::secondaryParticleBcOffset() const
{
  return m_firstSecondary;
}


/** Return the secondary vertex offset */
Barcode::VertexBarcode Barcode::GenericBarcodeSvc::secondaryVertexBcOffset() const
{
  return m_firstVertex;
}


/** Return the barcode increment for each generation of updated particles */
Barcode::ParticleBarcode Barcode::GenericBarcodeSvc::particleGenerationIncrement() const
{
  return m_particleRegenerationIncrement;
}


/** Handle incident */
void Barcode::GenericBarcodeSvc::handle(const Incident& inc)
{
  if ( inc.type() == IncidentType::BeginEvent )
    {
      ATH_MSG_VERBOSE("'BeginEvent' incident caught. Resetting Vertex and Particle barcode counters.");
      m_currentVertex    = m_firstVertex    - m_vertexIncrement;
      m_currentSecondary = m_firstSecondary - m_secondaryIncrement;
    }
}


/** framework methods */
StatusCode Barcode::GenericBarcodeSvc::finalize()
{
  ATH_MSG_VERBOSE ("finalize() ...");
  ATH_MSG_VERBOSE ("finalize() successful");
  return StatusCode::SUCCESS;
}


/** Query the interfaces. */
StatusCode Barcode::GenericBarcodeSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{

  if ( IID_IBarcodeSvc == riid )
    {
      *ppvInterface = (IBarcodeSvc*)this;
    }
  else
    {
      // Interface is not directly available: try out a base class
      return Service::queryInterface(riid, ppvInterface);
    }
  addRef();
  return StatusCode::SUCCESS;
}

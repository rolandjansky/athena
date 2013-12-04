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
  m_firstVertex(-200000),
  m_vertexIncrement(-1000000),
  m_curVertex(m_firstVertex),
  m_firstSecondary(200001),
  m_secondaryIncrement(1),
  m_curParticle(m_firstSecondary),
  m_doUnderOverflowChecks(true),
  m_encodePhysicsProcess(true)
{
  // python properties
  declareProperty("FirstVertexBarcode"            ,  m_firstVertex=-200000        );
  declareProperty("VertexIncrement"               ,  m_vertexIncrement=-1000000   );
  declareProperty("FirstSecondaryBarcode"         ,  m_firstSecondary=200001      );
  declareProperty("SecondaryIncrement"            ,  m_secondaryIncrement=1       );
  declareProperty("DoUnderAndOverflowChecks"      ,  m_doUnderOverflowChecks=true );
  declareProperty("EncodePhysicsProcessInVertexBC",  m_encodePhysicsProcess=true  );
}


Barcode::GenericBarcodeSvc::~GenericBarcodeSvc() 
{}


/** framework methods */
StatusCode Barcode::GenericBarcodeSvc::initialize()
{
  ATH_MSG_INFO ("initialize() ...");

  if ( m_incidentSvc.retrieve().isFailure()){
      ATH_MSG_WARNING("Can not retrieve " << m_incidentSvc << ". Exiting.");
      return StatusCode::FAILURE;
  }
  // register to the incident service: BeginEvent needed for refresh of counter
  m_incidentSvc->addListener( this, IncidentType::BeginEvent);

  ATH_MSG_INFO ("initialize() successful");
  return StatusCode::SUCCESS;
}


/** Generate a new unique vertex barcode, based on the parent particle barcode and
    the physics process code causing the truth vertex*/
Barcode::VertexBarcode Barcode::GenericBarcodeSvc::newVertex( Barcode::ParticleBarcode /* parent */,
                                                              Barcode::PhysicsProcessCode process ) {
  // update the internal vertex BC counter
  m_curVertex += m_vertexIncrement;

  // the barcode that will be returned
  Barcode::VertexBarcode newBC = m_curVertex;

  // if enabled, put the physics process code into the vertex barcode
  if (m_encodePhysicsProcess)
    newBC = newBC - process;
    // an example vertex BC would be (8th vtx, process #1234):  -8201234

  // a naive underflog checking based on the fact that vertex
  // barcodes should never be positive
  if ( m_doUnderOverflowChecks && ( newBC > 0))
    ATH_MSG_ERROR("LegacyBarcodeSvc::newVertex(...)"
                  << " will return a vertex barcode greater than 0: "
                  << m_curVertex << ". Possibly Integer Underflow?");

  return newBC;
}


/** Generate a new unique barcode for a secondary particle, based on the parent
    particle barcode and the process code of the physics process that created
    the secondary  */
Barcode::ParticleBarcode Barcode::GenericBarcodeSvc::newSecondary( Barcode::ParticleBarcode /* parentBC */,
                                                                   Barcode::PhysicsProcessCode /* process */) {
  m_curParticle += m_secondaryIncrement;

  // a naive overflow checking based on the fact that particle
  // barcodes should never be negative
  if ( m_doUnderOverflowChecks && (m_curParticle < 0))
    ATH_MSG_ERROR("LegacyBarcodeSvc::newSecondary(...)"
                  << " will return a particle barcode of less than 0: "
                  << m_curParticle << ". Possibly Integer Overflow?");

  return m_curParticle;
}


/** Generate a common barcode which will be shared by all children
    of the given parent barcode (used for child particles which are 
    not stored in the mc truth event) */
Barcode::ParticleBarcode Barcode::GenericBarcodeSvc::sharedChildBarcode( Barcode::ParticleBarcode /* parentBC */,
                                                                         Barcode::PhysicsProcessCode /* process */) {
  // concept of shared barcodes not supported here yet
  return Barcode::fUndefinedBarcode;
}


/** Update the given barcode (e.g. after an interaction) */
Barcode::ParticleBarcode Barcode::GenericBarcodeSvc::incrementBarcode( Barcode::ParticleBarcode /* old */,
                                                                       Barcode::PhysicsProcessCode /* process */) {
  m_curParticle += m_secondaryIncrement;
  return ( m_curParticle);
}


void Barcode::GenericBarcodeSvc::registerLargestGenEvtParticleBC( Barcode::ParticleBarcode /* bc */) {
}


void Barcode::GenericBarcodeSvc::registerLargestGenEvtVtxBC( Barcode::VertexBarcode /* bc */) {
}


/** Handle incident */
void Barcode::GenericBarcodeSvc::handle(const Incident& inc) {
  if ( inc.type() == IncidentType::BeginEvent ) {
      ATH_MSG_VERBOSE("'BeginEvent' incident caught. Resetting Vertex and Particle barcode counters.");
      m_curParticle = m_firstSecondary - m_secondaryIncrement;
      m_curVertex   = m_firstVertex    - m_vertexIncrement;
  }
}


/** framework methods */
StatusCode Barcode::GenericBarcodeSvc::finalize()
{
  ATH_MSG_INFO ("finalize() ...");
  ATH_MSG_INFO ("finalize() successful");
  return StatusCode::SUCCESS;
}


/** Query the interfaces. */
StatusCode Barcode::GenericBarcodeSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {

 if ( IID_IBarcodeSvc == riid ) 
    *ppvInterface = (IBarcodeSvc*)this;
 else  {
   // Interface is not directly available: try out a base class
   return Service::queryInterface(riid, ppvInterface);
 }
 addRef();
 return StatusCode::SUCCESS;
}


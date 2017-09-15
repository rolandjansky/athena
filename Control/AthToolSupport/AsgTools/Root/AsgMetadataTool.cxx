// $Id: AsgMetadataTool.cxx 799036 2017-03-01 09:56:55Z will $

// System include(s):
#include <stdexcept>

// Local include(s):
#include "AsgTools/AsgMetadataTool.h"

#ifdef ASGTOOL_ATHENA
// Gaudi/Athena include(s):
#   include "GaudiKernel/Incident.h"
#   include "GaudiKernel/IIncidentSvc.h"
#endif // ASGTOOL_ATHENA

#ifdef ASGTOOL_STANDALONE
// xAOD include(s):
#   include "xAODRootAccessInterfaces/TActiveEvent.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ASGTOOL_STANDALONE

#ifdef ASGTOOL_ATHENA
namespace IncidentType {
   /// Incident in Athena, emitted when metadata should be written out
   static const std::string MetaDataStop = "MetaDataStop";
} // namespace IncidentType
#endif // ASGTOOL_ATHENA

namespace asg {

#ifdef ASGTOOL_STANDALONE
   // To be able to refer to xAOD::IncidentType as IncidentType...
   using namespace xAOD;
#endif // ASGTOOL_STANDALONE

   AsgMetadataTool::AsgMetadataTool( const std::string& name )
      : AsgTool( name ),
#ifdef ASGTOOL_STANDALONE
        m_inputMetaStore( SgTEventMeta::InputStore ),
        m_outputMetaStore( SgTEventMeta::OutputStore ),
#elif defined(ASGTOOL_ATHENA)
        m_inputMetaStore( "StoreGateSvc/InputMetaDataStore", name ),
        m_outputMetaStore( "StoreGateSvc/MetaDataStore", name ),
#endif // Environment selection
        m_beginInputFileCalled( false )
   {

#ifdef ASGTOOL_STANDALONE
      // Try to access the current active TEvent:
      xAOD::TVirtualEvent* vevent = xAOD::TActiveEvent::event();
      xAOD::TEvent* event = dynamic_cast< xAOD::TEvent* >( vevent );
      if( ! event ) {
         ATH_MSG_WARNING( "Couldn't find active xAOD::TEvent object" );
         ATH_MSG_WARNING( "Callbacks to the tool will not be available" );
         return;
      }
      // Register the tool for callbacks:
      if( event->addListener( this ).isFailure() ) {
         ATH_MSG_ERROR( "Couldn't register the tool for xAOD callbacks" );
      }
#elif defined(ASGTOOL_ATHENA)
      // Declare the interface implemented by this base class:
      declareInterface< IIncidentListener >( this );
#endif // Environment selection
   }

   AsgMetadataTool::~AsgMetadataTool() {

#ifdef ASGTOOL_STANDALONE
      // Try to access the active TEvent:
      xAOD::TVirtualEvent* vevent = xAOD::TActiveEvent::event();
      xAOD::TEvent* event = dynamic_cast< xAOD::TEvent* >( vevent );
      // If we didn't succeed, fine. No need to complain about it...
      if( event ) {
         // But if there is a TEvent, then let's try to de-register from it
         // properly.
         if( event->removeListener( this ).isFailure() ) {
            ATH_MSG_WARNING( "Active xAOD::TEvent didn't have a callback for "
                             "this tool" );
         }
      }
#else
      //if initialized, then we should remove listeners
      if(FSMState() == Gaudi::StateMachine::INITIALIZED) {
	ServiceHandle< IIncidentSvc > incSvc( "IncidentSvc", name() );
        if( incSvc.retrieve().isSuccess() ) {
	  incSvc->removeListener( this ); //removes entirely
	}
      }
#endif // ASGTOOL_STANDALONE
   }

   AsgMetadataTool::MetaStorePtr_t AsgMetadataTool::inputMetaStore() const {

#ifdef ASGTOOL_STANDALONE
      return &m_inputMetaStore;
#elif defined(ASGTOOL_ATHENA)
      return m_inputMetaStore;
#endif // Environment selection
   }

   AsgMetadataTool::MetaStorePtr_t AsgMetadataTool::outputMetaStore() const {

#ifdef ASGTOOL_STANDALONE
      return &m_outputMetaStore;
#elif defined(ASGTOOL_ATHENA)
      return m_outputMetaStore;
#endif // Environment selection
   }

   /// This function is used to set up the callbacks from IncidentSvc in
   /// Athena at the right time during initialisation, without the user having
   /// to do anything special in his/her code.
   ///
   StatusCode AsgMetadataTool::sysInitialize() {

#ifdef ASGTOOL_ATHENA

      // Connect to the IncidentSvc:
      ServiceHandle< IIncidentSvc > incSvc( "IncidentSvc", name() );
      ATH_CHECK( incSvc.retrieve() );

      // Set up the right callbacks: don't rethrow exceptions, any failure and we should end
      incSvc->addListener( this, IncidentType::BeginEvent, 0, false );
      incSvc->addListener( this, IncidentType::BeginInputFile, 0, false );
      incSvc->addListener( this, IncidentType::EndInputFile, 0, false );
      incSvc->addListener( this, IncidentType::MetaDataStop, 70, false );

      // Let the base class do its thing:
      ATH_CHECK( AlgTool::sysInitialize() );

#endif // ASGTOOL_ATHENA

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   void AsgMetadataTool::handle( const Incident& inc ) {

      // Tell the user what's happening:
      ATH_MSG_VERBOSE( "Callback received with incident: " << inc.type() );

      // Call the appropriate member function:
      if( inc.type() == IncidentType::BeginInputFile ) {
         m_beginInputFileCalled = true;
         if( beginInputFile().isFailure() ) {
            ATH_MSG_FATAL( "Failed to call beginInputFile()" );
            throw std::runtime_error( "Couldn't call beginInputFile()" );
         }
      } else if( inc.type() == IncidentType::EndInputFile ) {
         if( endInputFile().isFailure() ) {
            ATH_MSG_FATAL( "Failed to call endInputFile()" );
            throw std::runtime_error( "Couldn't call endInputFile()" );
         }
      } else if( inc.type() == IncidentType::BeginEvent ) {
         // If the tool didn't catch the begin input file incident for the
         // first input file of the job, then call the appropriate function
         // now.
         if( ! m_beginInputFileCalled ) {
            m_beginInputFileCalled = true;
            if( beginInputFile().isFailure() ) {
               ATH_MSG_FATAL( "Failed to call beginInputFile()" );
               throw std::runtime_error( "Couldn't call beginInputFile()" );
            }
         }
         if( beginEvent().isFailure() ) {
            ATH_MSG_FATAL( "Failed to call beginEvent()" );
            throw std::runtime_error( "Couldn't call beginEvent()" );
         }
      } else if( inc.type() == IncidentType::MetaDataStop ) {
         if( metaDataStop().isFailure() ) {
            ATH_MSG_FATAL( "Failed to call metaDataStop()" );
            throw std::runtime_error( "Couldn't call metaDataStop()" );
         }
      } else {
         ATH_MSG_WARNING( "Unknown incident type received: " << inc.type() );
      }

      return;
   }

   /// Dummy implementation that can be overridden by the derived tool.
   ///
   StatusCode AsgMetadataTool::beginInputFile() {

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /// Dummy implementation that can be overridden by the derived tool.
   ///
   StatusCode AsgMetadataTool::endInputFile() {

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /// Dummy implementation that can be overridden by the derived tool.
   ///
   StatusCode AsgMetadataTool::beginEvent() {

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /// Dummy implementation that can be overridden by the derived tool.
   ///
   StatusCode AsgMetadataTool::metaDataStop() {

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace asg

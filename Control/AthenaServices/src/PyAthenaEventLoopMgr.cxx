/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//===================================================================
//	PyAthenaEventLoopMgr.cxx
//--------------------------------------------------------------------
//
//	Package    : AthenaServices
//
//  Description: implementation of the Application's interactive mode
//               handler
//
//	Author     : Wim Lavrijsen
//  History    :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 01/12/05| Initial version                              | WL
// +---------+----------------------------------------------+---------
//
//====================================================================

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // non-MT EventLoopMgr

// Python
#include "Python.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/DataSvc.h"

// AthenaServices
#include "PyAthenaEventLoopMgr.h"

#include "RootUtils/PyAthenaGILStateEnsure.h"


//- data ------------------------------------------------------------------
namespace {

   char* execalgs = const_cast< char* >( "executeAlgorithms" );

} // unnamed namespace


//=========================================================================
// Outside access to the event loop manager
//=========================================================================
PyAthenaEventLoopMgr* PyAthenaEventLoopMgr::pointer() {
   IEventProcessor* ep = nullptr;

   static const bool CREATEIF( false );
   if ( ( Gaudi::svcLocator()->service( "PyAthenaEventLoopMgr", ep, CREATEIF ) ).isSuccess() ) {
      ep->addRef();
      return dynamic_cast< PyAthenaEventLoopMgr* >( ep ); 
   }

   return nullptr;
}


//=========================================================================
// Standard Constructor
//=========================================================================
PyAthenaEventLoopMgr::PyAthenaEventLoopMgr( const std::string& name, 
                                            ISvcLocator* svcLoc )
   : AthenaEventLoopMgr( name, svcLoc ), m_manager( nullptr )
{
  /// overrides the base-class default: for interactive use, it is mandatory
  /// to leave the store untouched at the end of the event (so people can
  /// interactively browse the store).
  /// Hence, we ask for store clean-up at the beginning of the event.
  AthenaEventLoopMgr::m_clearStorePolicy.set( "BeginEvent" );
}


//=========================================================================
// Python side manager set/get
//=========================================================================
PyObject* PyAthenaEventLoopMgr::setManager( PyObject* mgr )
{
   RootUtils::PyGILStateEnsure gil;
   if ( ! PyObject_HasAttrString( mgr, execalgs ) )
   {
      PyErr_SetString( PyExc_TypeError, "given object is not a manager" );
      return nullptr;
   }

// set on ourselves
   PyObject* old = m_manager;
   Py_INCREF( mgr );
   m_manager = mgr;

// set onto the module as a module global (this may replace the old one, so
// users are best served to only access this through the module)
   PyObject* pyelm = PyImport_AddModule( const_cast< char* >( "AthenaServices.PyAthenaEventLoopMgr" ) );
   if ( pyelm ) {
      Py_INCREF( mgr );
      PyModule_AddObject( pyelm, const_cast< char* >( "EventLoopMgr" ), mgr );
   }

// hand the python side its interfaces
   PyObject* pyself = PyCapsule_New( (void*)static_cast< IEventSeek* >( this ), nullptr, nullptr );
#if PY_MAJOR_VERSION < 3
   PyObject* method = PyString_FromString( "_installServices" );
#else
   PyObject* method = PyUnicode_FromString( "_installServices" );
#endif
   PyObject* result = PyObject_CallMethodObjArgs( mgr, method, pyself, 0 );
   Py_DECREF( method );
   Py_DECREF( pyself );

   if ( ! result )
      return nullptr;

// return old value (with its refcount), if set previously; or None
   if ( old != nullptr )
      return old;

   Py_INCREF( Py_None );
   return Py_None;
}

PyObject* PyAthenaEventLoopMgr::getManager()
{
   RootUtils::PyGILStateEnsure gil;
   if ( ! m_manager )
   {
      Py_INCREF( Py_None );
      return Py_None;
   }
      
   Py_INCREF( m_manager );
   return m_manager;
}


//=========================================================================
// Implementation of IAppMgrUI::initialize
//=========================================================================
StatusCode PyAthenaEventLoopMgr::initialize()    
{
   RootUtils::PyGILStateEnsure gil;
// get the PyAthenaEventLoopMgr python-side class
   PyObject* modpyelm = PyImport_ImportModule( const_cast< char* >( "AthenaServices.PyAthenaEventLoopMgr" ) );
   if ( modpyelm ) {

      PyObject* pyelm = PyObject_GetAttrString( modpyelm, const_cast< char* >( "PyAthenaEventLoopMgr" ) );
      Py_DECREF( modpyelm );

      if ( pyelm ) {
         PyObject* args = PyTuple_New( 0 );
         PyObject* self = PyObject_Call( pyelm, args, nullptr );
         Py_DECREF( args );
         Py_DECREF( pyelm );

         if ( self ) {
         // initialize base class, which then allows queryInterface as appropriate
            StatusCode sc = this->AthenaEventLoopMgr::initialize();

         // set the interfaces on the python-side manager
            PyObject* result = setManager( self );
            Py_DECREF( self );

            if ( ! result ) {
               PyErr_Print();
               sc.ignore();
               return StatusCode::FAILURE;
            }

            return sc;
         }

      }

   }

   PyErr_Print();
   return StatusCode::FAILURE;
}


//=========================================================================
// Run the algorithms for the current event
//=========================================================================
StatusCode PyAthenaEventLoopMgr::executeAlgorithms(const EventContext& ctx)
{
   RootUtils::PyGILStateEnsure gil;
// forward to call to the python-side object
   if ( m_manager != nullptr )
   {
   // forward call, if python side manager available
     PyObject* pycontext = PyCapsule_New ( const_cast<EventContext*>(&ctx), nullptr, nullptr);
      PyObject* result = PyObject_CallMethod( m_manager, execalgs,
                                              (char*)"O", pycontext);
      Py_DECREF (pycontext);

      if ( ! result )
      {
	  PyErr_Print();            // should use MessageSvc instead
	  return StatusCode::FAILURE;
      }

#if PY_MAJOR_VERSION < 3
      if ( PyInt_Check( result ) || PyLong_Check( result ) )
      {
	 StatusCode sc = StatusCode( (int) PyInt_AS_LONG( result ) );
         Py_DECREF( result );
         return sc;
      }
#else
      if ( PyLong_Check( result ) )
      {
	 StatusCode sc = StatusCode( (int) PyLong_AS_LONG( result ) );
         Py_DECREF( result );
         return sc;
      }
#endif

   // FIXME: allow python result to be a statuscode
      MsgStream log( msgSvc(), name() );
      log << MSG::ERROR
	  << "result from python event loop manager has unexpected type."
	  << endmsg;
      Py_DECREF( result );
      return StatusCode::FAILURE;
   }

// otherwise, let base class handle it
   return AthenaEventLoopMgr::executeAlgorithms(ctx);
}


//=========================================================================
// Implementation of IAppMgrUI::finalize
//=========================================================================
StatusCode PyAthenaEventLoopMgr::finalize()
{
   RootUtils::PyGILStateEnsure gil;
   Py_XDECREF( m_manager );

   return this->AthenaEventLoopMgr::finalize();
}

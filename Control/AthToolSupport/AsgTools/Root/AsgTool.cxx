// $Id: AsgTool.cxx 620407 2014-10-07 13:40:28Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolStore.h"

namespace asg {

   AsgTool::AsgTool( const std::string& name )
      : AsgToolBase(
#ifdef ASGTOOL_ATHENA
                    "", name, Gaudi::svcLocator()->service( "ToolSvc" )
#elif defined(ASGTOOL_STANDALONE)
                    this
#else
#   error "What environment are we in?!?"
#endif // Environment selection
                    )
#ifdef ASGTOOL_STANDALONE
      , m_name( name ), m_ppropmgr( new PropertyMgr() ), m_event()
#endif // ASGTOOL_STANDALONE
   {
#ifdef ASGTOOL_STANDALONE
      declareProperty( "OutputLevel", msg().mutable_level() );
      ToolStore::put( this ).ignore(); // Register the tool in the ToolStore
#endif // ASGTOOL_STANDALONE
   }

   AsgTool::~AsgTool() {

#ifdef ASGTOOL_STANDALONE
      ToolStore::remove( this ).ignore(); // Remove the tool from the ToolStore
      delete m_ppropmgr;
#endif // ASGTOOL_STANDALONE
   }

#ifdef ASGTOOL_STANDALONE

   SgTEvent* AsgTool::evtStore() const {

      return &m_event;
   }

   /// See the comments for PropertyMgr::setProperty to see why this
   /// function specialisation is needed, and why it has this exact form.
   ///
   /// @param name The name of the string property to set
   /// @param value The value of the string property to set
   /// @returns <code>StatusCode::SUCCESS</code> if the call was successful,
   ///          <code>StatusCode::FAILURE</code> otherwise
   ///
   StatusCode AsgTool::setProperty( const std::string& name,
                                    const char* value ) {

      // Set the property using the property manager:
      return m_ppropmgr->setProperty( name, value );
   }

   PropertyMgr* AsgTool::getPropertyMgr() {

      return m_ppropmgr;
   }

   const PropertyMgr* AsgTool::getPropertyMgr() const {

      return m_ppropmgr;
   }

   const std::string& AsgTool::name() const {

      return m_name;
   }

   void AsgTool::setName( const std::string& name ) {

      m_name = name;
      return;
   }

#endif // ASGTOOL_STANDALONE

   /// Instead of using this, weirdly named function, user code should get
   /// the string name of the current minimum message level (in case they
   /// really need it...), with:
   ///
   /// <code>
   ///   MSG::name( msg().level() )
   /// </code>
   ///
   /// This function's name doesn't follow the ATLAS coding rules, and as such
   /// will be removed in the not too distant future.
   ///
   /// @returns The string name of the current minimum message level that's
   ///          printed
   ///
   const std::string& AsgTool::msg_level_name() const {

      return MSG::name( msg().level() );
   }

   void AsgTool::print() const {

      ATH_MSG_INFO( "AsgTool " << name() << " @ " << this );
      return;
   }

} // namespace asg

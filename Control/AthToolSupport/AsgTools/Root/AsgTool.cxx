/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <iostream>

// Local include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolStore.h"

#ifndef XAOD_STANDALONE


namespace asg {

// Helpers to pack/unpack the PTR#TYPE/NAME string set up by the macros
// in AsgToolMacros.h.

std::string ptrToString (const void* p)
{
  char buf[80];
  snprintf (buf, 80, "%p", p);
  return buf;
}

std::string getType (const std::string& s)
{
  std::string::size_type pos1 = s.find('#');
  if (pos1 == std::string::npos)
    pos1 = 0;
  else
    ++pos1;
  std::string::size_type pos2 = s.find('/', pos1);
  if (pos2 != std::string::npos)
    return s.substr (pos1, pos2-pos1);
  return "";
}

std::string getName (const std::string& s)
{
  std::string::size_type pos = s.find('/');
  if (pos == std::string::npos)
    return s;
  return s.substr (pos+1, std::string::npos);
}

const IInterface* getParent (const std::string& s)
{
  std::string::size_type pos = s.find('#');
  if (pos == std::string::npos)
    return Gaudi::svcLocator()->service( "ToolSvc" );
  void* p;
  std::string ss = s.substr (0, pos);
  sscanf (ss.c_str(), "%p", &p);
  if (p != nullptr)
    return reinterpret_cast<IInterface*>(p);
  return Gaudi::svcLocator()->service( "ToolSvc" );
}

} // anonymous namespace
#endif // not XAOD_STANDALONE


namespace asg {

   AsgTool::AsgTool( const std::string& name )
      : AsgToolBase(
#ifndef XAOD_STANDALONE
                    getType(name), getName(name), getParent(name)
#else // not XAOD_STANDALONE
                    this
#endif // not XAOD_STANDALONE
                    )
#ifdef XAOD_STANDALONE
      , m_name( name ), m_ppropmgr( new PropertyMgr() ), m_event()
#endif // XAOD_STANDALONE
   {
#ifdef XAOD_STANDALONE
     msg().declarePropertyFor (*this);
      
#endif // XAOD_STANDALONE
      ToolStore::put( this ).ignore(); // Register the tool in the ToolStore
   }

   AsgTool::~AsgTool() {

#ifdef XAOD_STANDALONE
      
      delete m_ppropmgr;
#endif // XAOD_STANDALONE
      ToolStore::remove( this ).ignore(); // Remove the tool from the ToolStore
   }

#ifdef XAOD_STANDALONE

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

#endif // XAOD_STANDALONE

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

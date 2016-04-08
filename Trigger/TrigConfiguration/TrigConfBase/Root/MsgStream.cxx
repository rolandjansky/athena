/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include "TrigConfBase/MsgStream.h"

using namespace TrigConf;

/** Forward declaration of IMessageSvc interface.
 *  We only use it as pointer and don't need the full implementation.
 *  Dereferencing will not work!
 */
class IMessageSvc;

/** Some weak symbol referencing magic...
 *  These are declared in AthenaKernel/getMessageSvc.h and will be non-NULL in case
 *  the GaudiSvc/AthenaKernel shared libraries have been loaded.
 */
namespace Athena {
  IMessageSvc* getMessageSvc(bool) __attribute__((weak));
  void reportMessage(IMessageSvc*, const std::string&, int, const std::string&) __attribute__((weak));
  int outputLevel(IMessageSvc*, const std::string&) __attribute__((weak));
  void setOutputLevel(IMessageSvc*, const std::string&, int) __attribute__((weak));
};

/** Global pointer to MessageSvc and helper to fill it
 */
namespace {
  IMessageSvc* p_msgsvc;
  inline void getMessageSvc() {
    // If we haven't found a MessageSvc try to find one
    if (p_msgsvc==0 && Athena::getMessageSvc!=NULL) {
      p_msgsvc = Athena::getMessageSvc(/*quiet=*/true);
    }
  }
  const char* levelNames[MSGTC::NUM_LEVELS] = {"NIL","VERBOSE","DEBUG","INFO",
                                               "WARNING","ERROR","FATAL","ALWAYS"};
}

MsgStreamTC::MsgStreamTC(const std::string& name) :
      m_active(false),
      m_msgLevel(MSGTC::INFO),
      m_name(name)
{
  getMessageSvc();
  // Initialize our own OutputLevel to the global one from the MessageSvc
  m_level = (Athena::outputLevel!=NULL) ? MSGTC::Level(Athena::outputLevel(p_msgsvc, m_name)) : MSGTC::INFO;
}

// Set message level of stream
void MsgStreamTC::setLevel(MSGTC::Level lvl) {
   lvl = (lvl >= MSGTC::NUM_LEVELS) ? MSGTC::ALWAYS : (lvl<MSGTC::NIL) ? MSGTC::NIL : lvl;
   m_level = lvl;
   getMessageSvc();
   if (p_msgsvc) {
      Athena::setOutputLevel(p_msgsvc, m_name, m_level);
   }
}



void MsgStreamTC::doOutput()
{
  getMessageSvc();
  if (p_msgsvc) {
    Athena::reportMessage(p_msgsvc, m_name, m_msgLevel, str());
  }
  else {
    // later use ERS here (ERS needs to be a third case, we also need simple cout)
    std::ios::fmtflags f( std::cout.flags() );
    std::cout << std::setw(m_width) << std::left << m_name << " " << levelNames[m_msgLevel] << " " << str() << std::endl;
    std::cout.flags(f);
  }
  str(""); // Clear our buffer after message has been printed
}

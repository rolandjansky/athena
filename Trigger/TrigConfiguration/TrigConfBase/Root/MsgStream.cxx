/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
 *  These are declared in AthenaKernel/getMessageSvc.h and will be non-nullptr in case
 *  the GaudiSvc/AthenaKernel shared libraries have been loaded.
 */
namespace Athena {
  IMessageSvc* getMessageSvc(bool) __attribute__((weak));
  void reportMessage(IMessageSvc*, const std::string&, int, const std::string&) __attribute__((weak));
  int outputLevel(const IMessageSvc*, const std::string&) __attribute__((weak));
  void setOutputLevel(IMessageSvc*, const std::string&, int) __attribute__((weak));
}


namespace {

  /// Helper to get MessageSvc if running in athena environment
  static IMessageSvc* getMessageSvc() {
    static IMessageSvc* const msgsvc =
      Athena::getMessageSvc!=nullptr ? Athena::getMessageSvc(/*quiet=*/true) : nullptr;
    return msgsvc;
  }

  static const char* const levelNames[MSGTC::NUM_LEVELS] =
    {"NIL","VERBOSE","DEBUG","INFO","WARNING","ERROR","FATAL","ALWAYS"};
}

MsgStreamTC::MsgStreamTC(const std::string& name) :
      m_active(false),
      m_msgLevel(MSGTC::INFO),
      m_name(name)
{
  // Initialize our own OutputLevel to the global one from the MessageSvc
  m_level = (Athena::outputLevel!=nullptr) ? MSGTC::Level(Athena::outputLevel(getMessageSvc(), m_name)) : MSGTC::INFO;
}

// Set message level of stream
void MsgStreamTC::setLevel(MSGTC::Level lvl) {
   lvl = (lvl >= MSGTC::NUM_LEVELS) ? MSGTC::ALWAYS : (lvl<MSGTC::NIL) ? MSGTC::NIL : lvl;
   m_level = lvl;
   if (getMessageSvc()) {
     Athena::setOutputLevel(getMessageSvc(), m_name, m_level);
   }
}

void MsgStreamTC::doOutput()
{
  if (getMessageSvc()) {
    Athena::reportMessage(getMessageSvc(), m_name, m_msgLevel, str());
  }
  else {
    std::ios::fmtflags f( std::cout.flags() );
    std::cout << std::setw(m_width) << std::left << m_name << " " << levelNames[m_msgLevel] << " " << str() << std::endl;
    std::cout.flags(f);
  }
  str(""); // Clear our buffer after message has been printed
}

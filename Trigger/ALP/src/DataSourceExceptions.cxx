/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/

#include <stdexcept>
#include <string>
#include "ALP/DataSourceExceptions.h"

namespace ALPNS{
  namespace DSErrors{
    EventNotReady::EventNotReady()noexcept :m_msg("Event Not Ready!"){}
    EventNotReady::EventNotReady(const std::string &m)noexcept{
      m_msg=std::string("Event Not Ready: "+m);
    }
    const char * EventNotReady::what() const noexcept {
      return m_msg.c_str();
    }
    EventNotReady::EventNotReady(const EventNotReady &rhs) noexcept {m_msg=rhs.m_msg;}
    EventNotReady& EventNotReady::operator=(const EventNotReady &rhs) noexcept{m_msg=rhs.m_msg;return *this;}
    EventNotReady::~EventNotReady()noexcept {}

    NoMoreEvents::NoMoreEvents() noexcept :m_msg("No More Events"){}
    NoMoreEvents::NoMoreEvents(const std::string &m) noexcept{
      m_msg=std::string("No more events available "+m);
    }
    const char * NoMoreEvents::what() const noexcept {
      return m_msg.c_str();
    }
    NoMoreEvents::NoMoreEvents(const NoMoreEvents &rhs) noexcept {m_msg=rhs.m_msg;}
    NoMoreEvents& NoMoreEvents::operator=(const NoMoreEvents &rhs) noexcept{m_msg=rhs.m_msg;return *this;}
    NoMoreEvents::~NoMoreEvents()noexcept {}

    CommunicationError::CommunicationError() noexcept :m_msg("Communication Error") {}
    CommunicationError::CommunicationError(const std::string &m) noexcept {
      m_msg=std::string("Communication Error: "+m);
    }
    const char * CommunicationError::what() const noexcept {
      return m_msg.c_str();
    }
    CommunicationError::CommunicationError(const CommunicationError &rhs) noexcept {m_msg=rhs.m_msg;}
    CommunicationError& CommunicationError::operator=(const CommunicationError &rhs) noexcept{m_msg=rhs.m_msg;return *this;}
    CommunicationError::~CommunicationError() noexcept {}

  }//namespace DSErrors
  //common errors
  BadConfig::BadConfig() noexcept :m_msg("Bad Configuration") {}
  BadConfig::BadConfig(const std::string &m) noexcept {
    m_msg=std::string("Bad Configuration  "+m);
  }
  const char * BadConfig::what() const noexcept {
    return m_msg.c_str();
  }
  BadConfig::BadConfig(const BadConfig &rhs) noexcept {m_msg=rhs.m_msg;}
  BadConfig& BadConfig::operator=(const BadConfig &rhs) noexcept{m_msg=rhs.m_msg;return *this;}
  BadConfig::~BadConfig() noexcept {}

  NonexistentLib::NonexistentLib() noexcept : m_msg("Nonexistent library!") {}
  NonexistentLib::NonexistentLib(const std::string &m) noexcept {
    m_msg=std::string("Non-Existent Library  "+m);
  }
  const char * NonexistentLib::what() const noexcept {
    return m_msg.c_str();
  }
  NonexistentLib::NonexistentLib(const NonexistentLib &rhs) noexcept {m_msg=rhs.m_msg;}
  NonexistentLib& NonexistentLib::operator=(const NonexistentLib &rhs) noexcept{m_msg=rhs.m_msg;return *this;}

  NonexistentLib::~NonexistentLib() noexcept {}
  UnexpectedException::UnexpectedException() noexcept:m_msg("Unexpected Exception") {}
  UnexpectedException::UnexpectedException(const std::string &m) noexcept {
    m_msg=std::string("Unexpected Exception "+m);
  }
  const char * UnexpectedException::what() const noexcept {
    return m_msg.c_str();
  }
  UnexpectedException::UnexpectedException(const UnexpectedException &rhs) noexcept {m_msg=rhs.m_msg;}
  UnexpectedException& UnexpectedException::operator=(const UnexpectedException &rhs) noexcept{m_msg=rhs.m_msg;return *this;}
  UnexpectedException::~UnexpectedException() noexcept {}
}

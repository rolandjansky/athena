// -*- c++ -*-
/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/
#ifndef ALP_DATASOURCEEXCEPTIONS_H
#define ALP_DATASOURCEEXCEPTIONS_H
#include <stdexcept>
#include <string>

namespace ALPNS{
  namespace DSErrors{
    class EventNotReady:virtual public std::exception{
    public:
      EventNotReady() noexcept;
      EventNotReady(const EventNotReady&) noexcept;
      EventNotReady& operator=(const EventNotReady&) noexcept;
      EventNotReady(const std::string &m) noexcept;
      virtual const char * what() const noexcept;
      virtual ~EventNotReady() noexcept;
    private:
      std::string m_msg;
    };
    class NoMoreEvents:virtual public std::exception{
    public:
      NoMoreEvents() noexcept;
      NoMoreEvents(const std::string &m) noexcept;
      NoMoreEvents(const NoMoreEvents&) noexcept;
      NoMoreEvents& operator=(const NoMoreEvents&) noexcept;
      virtual const char * what() const noexcept;
      virtual ~NoMoreEvents() noexcept;
    private:
      std::string m_msg;
    };
    class CommunicationError:virtual public std::exception{
    public:
      CommunicationError() noexcept;
      CommunicationError(const std::string &m) noexcept;
      CommunicationError(const CommunicationError&) noexcept;
      CommunicationError& operator=(const CommunicationError&) noexcept;
      virtual const char * what() const noexcept;
      virtual ~CommunicationError() noexcept;
    private:
      std::string m_msg;
    };
  }//namespace DSErrors
  //common errors
  class BadConfig:virtual public std::exception{
  public:
    BadConfig() noexcept;
    BadConfig(const std::string &m) noexcept;
    BadConfig(const BadConfig&) noexcept;
    BadConfig& operator=(const BadConfig&) noexcept;
    virtual const char * what() const noexcept;
    virtual ~BadConfig() noexcept;
  private:
    std::string m_msg;
  };
  
  class NonexistentLib:virtual public std::exception{
  public:
    NonexistentLib() noexcept;
    NonexistentLib(const std::string &m) noexcept;
    NonexistentLib(const NonexistentLib&) noexcept;
    NonexistentLib& operator=(const NonexistentLib&) noexcept;
    virtual const char * what() const noexcept;
    virtual ~NonexistentLib() noexcept;
  private:
    std::string m_msg;
  };
  
  class UnexpectedException:virtual public std::exception{
  public:
    UnexpectedException()noexcept;
    UnexpectedException(const std::string &m)noexcept;
    UnexpectedException(const UnexpectedException&) noexcept;
    UnexpectedException& operator=(const UnexpectedException&) noexcept;
    virtual const char * what() const noexcept;
    virtual ~UnexpectedException() noexcept;
    private:
      std::string m_msg;
  };
}
#endif

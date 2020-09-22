/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFIO_TRIGCONFIOEXCEPTIONS_H
#define TRIGCONFIO_TRIGCONFIOEXCEPTIONS_H

#include <exception>
#include <string>

namespace TrigConf {

   class IOException : public std::exception {
   public:
      IOException(std::string msg);
      virtual const char* what() const noexcept;
   private:
      const std::string m_msg;
   };

   class QueryException : public IOException {
   public:
      QueryException(std::string msg) : IOException(std::move(msg)) {}
   };

   class NoQueryException : public IOException {
   public:
      NoQueryException(std::string msg) : IOException(std::move(msg)) {}
   };

   class NoSMKException : public IOException {
   public:
      NoSMKException(std::string msg) : IOException(std::move(msg)) {}
   };

   class NoL1PSKException : public IOException {
   public:
      NoL1PSKException(std::string msg) : IOException(std::move(msg)) {}
   };

   class NoHLTPSKException : public IOException {
   public:
      NoHLTPSKException(std::string msg) : IOException(std::move(msg)) {}
   };

   class NoBGSKException : public IOException {
   public:
      NoBGSKException(std::string msg) : IOException(std::move(msg)) {}
   };

   class ParsingException : public IOException {
   public:
      ParsingException(std::string msg) : IOException(std::move(msg)) {}
   };


}

#endif

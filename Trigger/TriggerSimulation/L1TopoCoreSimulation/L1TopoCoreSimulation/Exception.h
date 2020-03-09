/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//
//  ConfigException.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/18/12.
//

#ifndef L1TOPOCORESIMULATION_EXCEPTION_H
#define L1TOPOCORESIMULATION_EXCEPTION_H

#include <iostream>
#include <sstream>

#define TCS_EXCEPTION(MSG) \
std::stringstream o;\
o << MSG;\
throw TCS::Exception(o.str());

namespace TCS {
   
   class Exception : virtual public std::exception {
   public:
      enum type_t {
         CONFIG,
         RUNTIME
      };
      
      Exception(const std::string& msg) :
         m_msg(msg)
      {}
      
			virtual ~Exception() throw() {};

      virtual char const* what() const throw() { return m_msg.data(); }
      
   private:
      std::string m_msg;
   };
   
}

#endif /* defined(L1TOPOCORESIMULATION_EXCEPTION_H) */

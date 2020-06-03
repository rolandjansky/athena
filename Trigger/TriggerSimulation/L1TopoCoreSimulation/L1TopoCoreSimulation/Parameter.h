/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  Parameter.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/18/12.
//

#ifndef __TopoCore__Parameter__
#define __TopoCore__Parameter__

#include <iostream>
#include <string>
#include <stdint.h>

namespace TCS {

   typedef uint32_t parType_t;
   
   class Parameter {
   public:
      
      Parameter(const std::string & name) :
         m_name(name)
         , m_value(0)
         , m_defaultValue(0)
         , m_isDefault(true)
      {}
      
      Parameter(const std::string & name, TCS::parType_t value) :
         m_name(name)
         , m_value(value)
         , m_defaultValue(value)
         , m_isDefault(true)
      {}
      
      ~Parameter(){};
      
      // accessors
      TCS::parType_t value() const { return m_value; }       // parameter value
      TCS::parType_t operator()() const { return m_value; }  // parameter value
      TCS::parType_t defaultValue() const { return m_defaultValue; }
      const std::string & name() const { return m_name; }  // parameter name
      bool isDefault() const { return m_isDefault; }
      
      // setters
      void setValue(TCS::parType_t value) { m_value = value; }
      void setIsDefault(bool isDefault) { m_isDefault = isDefault; }
      
   private:
      std::string m_name;       // parameter name
      TCS::parType_t m_value;         // parameter value
      TCS::parType_t m_defaultValue;  // default value
      mutable bool m_isDefault; // indicates if the value has been overwritten, the logic of this is defined by the outside
   };
   
} // end of namespace TCS

std::ostream & operator<<(std::ostream &, const TCS::Parameter&);

#endif /* defined(__TopoCore__Parameter__) */

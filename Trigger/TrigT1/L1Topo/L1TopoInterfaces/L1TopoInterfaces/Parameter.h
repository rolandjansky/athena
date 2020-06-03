/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*  Parameter.h
 *  L1TopoInterfaces
 *
 *  This class describes Parameters that an algorithm can carry
 *
 *
 *
 */

#ifndef TopoCore__Parameter
#define TopoCore__Parameter

#include <iostream>
#include <string>
#include <stdint.h>

namespace TCS {

   typedef uint32_t parType_t;
   
   class Parameter {
   public:
      
      Parameter(const std::string & name, TCS::parType_t value);
      Parameter(const std::string & name, TCS::parType_t value, unsigned int selection);

      ~Parameter(){};
      
      // accessors
      const std::string & name() const { return m_name; }
      unsigned int        selection() const { return m_selection; }
      parType_t           value() const { return m_value; }
      unsigned int        position() const { return m_position; }
      parType_t           defaultValue() const { return m_defaultValue; }
      bool                isDefault() const { return m_isDefault; }
      bool                isExtended() const { return m_isExtended; }
      parType_t           operator()() const { return m_value; }
      
      // new accessors
      const std::string & val() const { return m_val; }
      std::string         defaultVal() const { return m_defaultVal; }
      unsigned int        pos() const { return m_pos; }
      const std::string & sel() const { return m_sel; }
      
      // setters
      void setName(const std::string& name) { m_name = name; }
      void setSelection(parType_t selection) { m_selection = selection; }
      void setValue(parType_t value) { m_value = value; m_isDefault = false; }
      void setPosition(parType_t position) { m_position = position; }
      void setIsDefault(bool isDefault) { m_isDefault = isDefault; }
      void setIsExtended(bool isExtended) { m_isExtended = isExtended; }
      
      // new setters
      void setVal(const std::string& val) { m_val = val; }
      void setPos(unsigned int pos) { m_pos = pos; }
      void setSel(const std::string& sel) { m_sel = sel; }
      
   private:

      std::string  m_name;              // parameter name
      parType_t    m_value {0};         // parameter value
      unsigned int m_position {0};      // parameter position
      unsigned int m_selection {0};     // parameter selection
      parType_t    m_defaultValue {0};  // default value
      mutable bool m_isDefault {true};  // indicates if the value has been overwritten, the logic of this is defined by the outside
      mutable bool m_isExtended {false};
      
      // new private vars
      std::string  m_val;
      unsigned int m_pos;
      std::string  m_sel;
      std::string  m_defaultVal;
   };
   
std::ostream & operator<<(std::ostream &, const TCS::Parameter&);

} // end of namespace TCS

#endif /* defined(__TopoCore__Parameter__) */

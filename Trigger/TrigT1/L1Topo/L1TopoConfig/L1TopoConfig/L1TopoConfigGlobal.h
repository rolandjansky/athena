/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TopoCore__L1TopoConfigGlobal__
#define __TopoCore__L1TopoConfigGlobal__

#include <vector>
#include <string>

namespace TXC {
  
   struct TopoConfigElement {
      TopoConfigElement() = default;
      TopoConfigElement(const std::string & name, const std::string & value) : name(name), value(value) {}
      std::string name {""};
      std::string value {""};
   };

   class L1TopoConfigGlobal {
   public:
    
      // default constructor
      L1TopoConfigGlobal() =default;

      //move constructor to allow faster vector filling
      L1TopoConfigGlobal(L1TopoConfigGlobal&&) noexcept = default;
      L1TopoConfigGlobal& operator=(L1TopoConfigGlobal&& ) noexcept = default;

      // destructor
      virtual ~L1TopoConfigGlobal();
    
      // get all TopoConfig elements
      const std::vector<TopoConfigElement> & getTopoConfigs() const { return m_topoConfigElements; }
    
      // setters
      void addTopoConfigElement(const std::string & name, const std::string & value);
      void addTopoConfigElement(const TopoConfigElement & config);
    
   private:
    
      // TopoConfig elements stored here
      std::vector<TopoConfigElement> m_topoConfigElements;
   };

std::ostream & operator<<(std::ostream &, const L1TopoConfigGlobal &);

} // end of namespace TXC

#endif 

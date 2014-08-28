/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPOMENU_HPP
#define L1TOPOMENU_HPP

#include <vector>
#include <map>
#include <string>

#include "L1TopoCommon/StatusCode.h"

#include "L1TopoConfig/L1TopoConfigAlg.h"
#include "L1TopoConfig/L1TopoConfigOutputList.h"
#include "L1TopoConfig/L1TopoConfigGlobal.h"

namespace TXC {
  
   class L1TopoMenu {

   public:

      // default constructor
      L1TopoMenu();
    
      // constructor
      L1TopoMenu(const std::string & name, const std::string & version);

      // assignment constructor
      L1TopoMenu(L1TopoMenu&& o);

      L1TopoMenu(const L1TopoMenu&) = delete;

      // destructor
      ~L1TopoMenu();

      //L1TopoMenu & operator=(const L1TopoMenu& rhs);
    
      // get menu attributes
      const std::string & getName() const {return m_name; }
      const std::string & getVersion() const {return m_version; }


      // accessors
      const L1TopoConfigAlg &                   algorithm(const std::string& name) const;
      const std::vector<TXC::L1TopoConfigAlg> & getL1TopoConfigAlgs() const { return m_algos; }
      const L1TopoConfigGlobal &                getL1TopoConfigGlobal() const { return m_topoconfig; }
      const L1TopoConfigOutputList &            getL1TopoConfigOutputList() const { return m_outputlist; }


      //! \brief setter methods
      //! @{
      //! \brief set menu attributes
      void setName(const std::string & name) { m_name = name; }
      void setVersion(const std::string & version) {m_version = version; }
    
      //! \brief add algorithm
      void addAlgorithm(const TXC::L1TopoConfigAlg &alg);

      //! \brief add output
      void addL1TopoXMLOutput(const TXC::OutputListElement & output) { m_outputlist.addOutputListElement(output); }

      //! \brief add config
      void addL1TopoConfigGlobal(const TXC::TopoConfigElement &topoconfig) { m_topoconfig.addTopoConfigElement(topoconfig); }
      //! @}
    
      //! \brief print menu
      void print() const;

      L1TopoMenu& operator=(L1TopoMenu&& o);
      

   private:

      L1TopoMenu& operator=(const L1TopoMenu&) {return *this;}

      friend class L1TopoXMLParser;

      TCS::StatusCode setTriggerList();

      std::string m_name{""};
      std::string m_version{""};

      // vectors of menu objects
      std::vector<TXC::L1TopoConfigAlg> m_algos;
      TXC::L1TopoConfigGlobal           m_topoconfig;
      TXC::L1TopoConfigOutputList       m_outputlist;

   };

}

#endif

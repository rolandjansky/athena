/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************
Created by: Marc Hohlfeld
Modified  :
Purpose   :

****************************************************/
#ifndef MONTECARLOREACT_RESTOOL_H
#define MONTECARLOREACT_RESTOOL_H

#include "MonteCarloReactUtils/EffExceptions.h"
#include "MonteCarloReactUtils/Resolution.h"

#include <string>
#include <vector>
#include <iostream>

namespace MonteCarloReact {

  class ResInfo;
  
  class ResTool {
    
  public:
    
    // Constructors
    ResTool() : m_isValid(false), m_res(0)  {;}

    // path is a directory location with mcr files
    ResTool(const ResInfo& spec, const std::string& path="./") {
      m_isValid = m_makeResolutionObj(spec, path);
      if( !m_isValid ) m_res = 0;
    }

    // path is a vectro of directory locations with mcr files
    ResTool(const ResInfo& spec, const std::vector<std::string>& path) {
      m_isValid = m_makeResolutionObj(spec, path);
      if( !m_isValid ) m_res = 0;
    }

    // Destructor (virtual since we will inherit this class)
    virtual ~ResTool();

    // Query methods
    bool getInfo(const ResInfo& );
  
    const ResInfo& getInfo() const;

    const Resolution& getResObj() const;

    bool isValid() const { return m_isValid; }
    
  protected:
    bool m_makeResolutionObj(const ResInfo&, const std::string& path="./");
    bool m_makeResolutionObj(const ResInfo&, const std::vector<std::string>& path);
  
    bool m_isValid;
    Resolution * m_res;
  
  private:
    //recursive search for spc files in directory
    void m_addFilesToList(const char* path, std::vector<std::string>& files) ;
  };
}
#endif 

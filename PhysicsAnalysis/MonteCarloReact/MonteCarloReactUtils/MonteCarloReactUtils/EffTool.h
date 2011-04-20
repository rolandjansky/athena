/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************
Created by: Marc Hohlfeld
Modified  :
Purpose   :

****************************************************/
#ifndef MONTECARLOREACTUTILS_EFFTOOL_H
#define MONTECARLOREACTUTILS_EFFTOOL_H


#include "MonteCarloReactUtils/EffExceptions.h"
#include "MonteCarloReactUtils/Efficiency.h"

#include <string>
#include <vector>

namespace MonteCarloReact {

  class EffInfo;
  
  class EffTool {
    
  public:
    
    /* Constructors */
    EffTool() : m_isValid(false), m_eff(0)  {;}

    /* path is a directory location with spc files */
    EffTool(const EffInfo& spec, const std::string& path="./");

    /* path is a directory location with spc files */
    EffTool(const EffInfo& spec, const std::vector<std::string>& path);

    /* Destructor (virtual since we will inherit this class) */
    virtual ~EffTool();

    /* Query methods */
    bool getInfo(const EffInfo& spec);

    /* Return efficiency Info */
    const EffInfo& getInfo() const;

    /* Return efficiency object */
    const Efficiency& getEffObj() const;  

    /* Validate request */
    bool isValid() const { return m_isValid; }
    
  protected:

    bool m_isValid;
    Efficiency * m_eff;
    
    bool m_makeEfficiencyObj(const EffInfo&, const std::string& path="./");
    bool m_makeEfficiencyObj(const EffInfo&, const std::vector<std::string>& path);

  
  private:
    /* recursive search for spc files in directory */
    void m_addFilesToList(const char* path, std::vector<std::string>& files) ;
  };
}
#endif // MONTECARLOREACTUTILS_EFFTOOL_H

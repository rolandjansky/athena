/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MONTECARLOREACT_EFFWRITEMANAGER_H
#define MONTECARLOREACT_EFFWRITEMANAGER_H

#include "MonteCarloReactUtils/EffExceptions.h"
#include "MonteCarloReactUtils/Efficiency.h"

namespace MonteCarloReact {

  /*! \brief 
    Save efficiencies to the files. This menager will save efficiency to the different or same file 
    depending of EffInfo object. It also will verify there is no efficiency with the same EffInfo.

  */
class EffWriteManager {

  std::string m_path ; //< directory to write the efficiencies
public:

  // Constructors
  EffWriteManager(const std::string& dir_path) : m_path (dir_path+"/") {}
  
  // return true if saving was successful
  bool save(const Efficiency& eff) ;

private:
  //recursive search for spc files in directory
  void m_addFilesToList(const char* path, std::vector<std::string>& files) ;
  };
}
#endif // _EFFWRITEMANAGER_HPP_

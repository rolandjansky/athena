/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_RPCCONDITIONPAR_H
#define MUONCONDDATA_RPCCONDITIONPAR_H


#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

class RpcConditionPar{

 public:

  // Default constructor
  RpcConditionPar();
  // destructor
  ~RpcConditionPar();

  // Full constructor
   
  Identifier  identify()      {return m_panel;}
  //std::string key()           {return m_stationKey;}

  void setParametersPanel(const std::string& info_panel, const std::string& info_strip);

  void getParametersPanel(std::string& info_panel, std::string& info_strip);
  
  
  
 private:

  
  
  //float m_stripid;
  std::string m_info_panel;
  std::string m_info_strip;
  //float m_panel;
 protected:

  Identifier m_panel;
  

};


#endif  // MUONCONDDATA_RPCCONDITIONPAR_H


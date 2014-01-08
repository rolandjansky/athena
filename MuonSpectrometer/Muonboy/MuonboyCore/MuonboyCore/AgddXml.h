/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AgddXml_H
#define AgddXml_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class AgddXml

   This class is an interface for Agdd Xml variables extraction

  @author samusog@cern.ch

  */
  
class AgddXml{
public:
    AgddXml();
    virtual ~AgddXml();

public:
///////////////////////////////////

   void GetAgddXmlVariableNb    (int& NbVar );

   void GetAgddXmlVariableName  (int  IVar , std::string& NameVar );

   void GetAgddXmlVariableValue (std::string  NameVar, int& Iok, double& Value);

};

#endif


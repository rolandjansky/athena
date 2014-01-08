/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyConfigure_H
#define MuonboyConfigure_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MboyVarByName;

  /**
   @class MuonboyConfigure

   this class manages configurations variables

  @author samusog@cern.ch
  
  */

class MuonboyConfigure{
public:
    MuonboyConfigure();
   ~MuonboyConfigure();

public:
///////////////////////////////////

   /*Set configuration variables*/
   void   SetVariablesDefault(MboyVarByName*  pMboyVarByName);

   /*Do reconstruction initialisation*/
   void   IniReco(MboyVarByName*  pMboyVarByName);

   /*Reset configuration variables*/
   void   Reset();

   /*Get configuration variables*/
   int GetVarSingle            (std::string VarName, double& value);

   /*Set configuration variables*/
   int SetVarSingle            (std::string VarName, double  value);

private:
///////////////////////////////////

   /*Get configuration variables*/
   int GetMuboIntVarBazSingle  (std::string VarName, int&    value);

   /*Get configuration variables*/
   int GetMuboRealVarBazSingle (std::string VarName, double& value);

   /*Set configuration variables*/
   int SetMuboRealVarBazSingle (std::string VarName, double  value);

   /*Set configuration variables*/
   int SetMuboIntVarBazSingle  (std::string VarName, int     value);

   /*Dump configuration variables*/
   void   DumpMboyVar();
   
   MboyVarByName*  p_MboyVarByName ; //!< Set of configuration variables
   int m_verbose ; //!< Verbose option
   
};

#endif

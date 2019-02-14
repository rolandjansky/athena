/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef PATCORE_IUSERDATACALCTOOL_H
#define PATCORE_IUSERDATACALCTOOL_H

//============================================================================
// Name: IUserDataCalcTool.h
//
/**
   @class IUserDataCalcTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date February 2010

   @brief This is the base class for all tools that will be used to calculate 
          UserDate. It is of course not required, but it is meant to simplyfy
          life.
*/
//=============================================================================


// Athena Tool Interface
#include "GaudiKernel/IAlgTool.h"


// forward declaration
class IAthenaBarCode;



static const InterfaceID IID_IUserDataCalcTool("IUserDataCalcTool", 1, 0);


class IUserDataCalcTool
  : virtual public ::IAlgTool
{
public:


  /** AlgTool interface methods */
  static const InterfaceID& interfaceID();


public:

  /** Calculate the UserData for a simple element,
      e.g., an individual electron (must inherit from IAthenaBarCode) */
  virtual StatusCode calculateElementUserData( const IAthenaBarCode *abc ) = 0;
  
  /** Calculate the UserData for an entire event */
  virtual StatusCode calculateEventUserData( /*not sure what to use here yet*/ ) = 0;
  


}; // End class IUserDataCalcTool


///////////////////////////////////////////////////////////////////
/// Inline methods:
///////////////////////////////////////////////////////////////////
inline const InterfaceID& IUserDataCalcTool::interfaceID() 
{ 
  return IID_IUserDataCalcTool; 
}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INAVIGATIONCONDITION_H
#define INAVIGATIONCONDITION_H

///////////////////////////////////////////////////////////////////////////////
//
// Navigation Condition Abstract Base Class 
//
///////////////////////////////////////////////////////////////////////////////

class INavigable;

class INavigationCondition
{

 public:

  virtual ~INavigationCondition();

  // accept function
  virtual bool accept( const INavigable * thisNavigable ) = 0;

  // reset function
  virtual void reset() = 0;

};
#endif




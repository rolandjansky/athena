/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INAVIGABLE_H
#define INAVIGABLE_H
///////////////////////////////////////////////////////////////////////////////
//
// Common base type for classes representing navigable objects
//
///////////////////////////////////////////////////////////////////////////////

namespace boost {
  class any;
}

class INavigationToken;

class INavigable
{
 public:
  
  virtual ~INavigable();

  // enforce fillToken(,) method in derived classes!
  virtual void fillToken( INavigationToken & thisToken ) const = 0;
  virtual void fillToken( INavigationToken & thisToken, 
			  const boost::any& weight ) const = 0;

};
#endif

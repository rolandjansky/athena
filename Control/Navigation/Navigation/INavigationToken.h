/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INAVIGATIONTOKEN_H
#define INAVIGATIONTOKEN_H

///////////////////////////////////////////////////////////////////////////////
//
//  Navigation Abstract Base Class
//
//  Author:   H. Ma, S. Rajagopalan, P. Loch
//
//  This class is the base class for the templated NavigationToken class. A
//  method pushQuery(INavigable&) allows to terminate navigation.
//
///////////////////////////////////////////////////////////////////////////////

class INavigable;

namespace boost {
  class any; 
}

class INavigationToken
{
 public:

  virtual ~INavigationToken();

  // check if query must be forwarded
  virtual bool pushQuery( const INavigable& parent,
			  const INavigable& child ) = 0;

  // 
  virtual bool trySetObject(const INavigable* child) = 0;
  virtual bool trySetObject(const INavigable* child,
			    const boost::any& relPar) = 0; 

};
#endif



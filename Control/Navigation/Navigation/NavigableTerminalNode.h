/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAVIGATION_NAVIGABLETERMINALNODE_H
#define NAVIGATION_NAVIGABLETERMINALNODE_H

//////////////////////////
// Termination handling //
//////////////////////////

#include "Navigation/INavigable.h"

#include <boost/any.hpp>

//template<>
//class Navigable< NavigationDefaults::TerminalNode >
class NavigableTerminalNode : virtual public INavigable 
{
 public:

  // suppress compiler warnings
  // typedef NavigationDefaults::DefaultType constituent_coll;
  // typedef NavigationDefaults::DefaultType constituent_iter;
  // typedef NavigationDefaults::DefaultType constituent_const_iter;

  NavigableTerminalNode() { };
  virtual ~NavigableTerminalNode() { };

  // implement INavigable::fillToken(...) as "do-nothing"
  virtual void fillToken( INavigationToken & ) const { };
  virtual void fillToken( INavigationToken &, const boost::any& ) const { };
};
#endif

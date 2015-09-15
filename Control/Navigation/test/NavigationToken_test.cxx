/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: NavigableIterator_test.cxx,v 1.2 2008-09-09 18:52:06 ssnyder Exp $
/**
 * @file Navigation/test/NavigationToken_test.cxx
 * @author Paolo Calafiura
 * @date May 2010
 * @brief Compilation tests for NavigationToken
 */

#undef NDEBUG

#include "Navigation/NavigationToken.h"
#include "Navigation/Navigable.h"


struct Payload
  : public NavigableTerminalNode
{
  Payload (int the_x) : x (the_x) {}
  int x;
};




int main()
{
  NavigationToken<Payload,double> nT;
  nT.clear();

  return 0;
}

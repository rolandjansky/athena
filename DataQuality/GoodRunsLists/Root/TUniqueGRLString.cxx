/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GoodRunsLists/TUniqueGRLString.h"

// Constructor
Root::TUniqueGRLString::TUniqueGRLString()
 : TString()
{
}

// Destructor
Root::TUniqueGRLString::~TUniqueGRLString()
{
}

Root::TUniqueGRLString&
Root::TUniqueGRLString::instance() {
  // created on first call, deleted after main()
  static TUniqueGRLString _instance;
  return _instance;
}

void
Root::TUniqueGRLString::SetTo(const TString& other)
{
  TString::operator=(other);
}

